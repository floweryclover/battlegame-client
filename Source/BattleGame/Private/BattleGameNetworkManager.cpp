// Copyright floweryclover 2024, All rights reserved.


#include "BattleGameNetworkManager.h"
#include "BattleGameCtsRpc.h"
#include "BattleGameStcRpc.h"
#include "BattleGameInstance.h"
#include "LogBattleGameNetwork.h"
#include "Gameframework/GameModeBase.h"
#include <functional>
#include <cstring>
#include <Winsock2.h>
#include <WS2tcpip.h>

TUniquePtr<BattleGameNetworkManager> BattleGameNetworkManager::spSingleton = nullptr;

void BattleGameNetworkManager::Initialize(const FString& serverAddress, int32 serverPort)
{
	check(spSingleton == nullptr);
	spSingleton = MakeUnique<BattleGameNetworkManager>(serverAddress, serverPort);
}

BattleGameNetworkManager::BattleGameNetworkManager(const FString& serverAddress, int32 serverPort) noexcept
	: mServerAddress(serverAddress),
	mServerPort(serverPort),
	mTcpSocket(INVALID_SOCKET),
	mUdpSocket(INVALID_SOCKET),
	mCurrentSent(0),
	mCurrentReceived(0),
	mTotalSizeToReceive(MESSAGE_HEADER_SIZE),
	mIsReceivingHeader(true),
	mpCtsRpcInstance(NewObject<UBattleGameCtsRpc>()),
	mpStcRpcInstance(NewObject<UBattleGameStcRpc>()),
	mServerAddrIn(new struct sockaddr_in()),
	mServerAddrLen(sizeof(struct sockaddr_in)),
	mpLastGameModeContext(nullptr)
{
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	check(result == 0);

	ZeroMemory(mServerAddrIn, mServerAddrLen);
	mServerAddrIn->sin_family = AF_INET;
	mServerAddrIn->sin_port = htons(mServerPort);

	result = inet_pton(AF_INET, TCHAR_TO_ANSI(*mServerAddress), &mServerAddrIn->sin_addr);
	check(result == 1);
}

BattleGameNetworkManager::~BattleGameNetworkManager()
{
	CleanupSocket();
	WSACleanup();
}


bool BattleGameNetworkManager::ConnectToServer(int32& errorCode)
{
	mUdpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	check(mUdpSocket != INVALID_SOCKET);

	u_long nonBlockingModeFlag = 1;
	int result = ioctlsocket(mUdpSocket, FIONBIO, &nonBlockingModeFlag);
	check(result == 0);

	mTcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	check(mTcpSocket != INVALID_SOCKET);

	result = connect(mTcpSocket, reinterpret_cast<struct sockaddr*>(mServerAddrIn), mServerAddrLen);
	if (result == SOCKET_ERROR)
	{
		errorCode = WSAGetLastError();
		CleanupSocket();
		return false;
	}
	else
	{
		result = ioctlsocket(mTcpSocket, FIONBIO, &nonBlockingModeFlag);
		check(result == 0);
		errorCode = 0;
		return true;
	}
}

void BattleGameNetworkManager::ManualTick(AGameModeBase* gameModeContext)
{
	mpLastGameModeContext = gameModeContext;
	SendTcp();
	ReceiveTcp();
	ReceiveUdp();
}

void BattleGameNetworkManager::CleanupSocket()
{
	mTcpSendQueue.Empty();
	mCurrentSent = 0;
	mTotalSizeToReceive = MESSAGE_HEADER_SIZE;
	mCurrentReceived = 0;
	mLastReceivedHeaderType = 0;
	mIsReceivingHeader = true;
	if (mTcpSocket != INVALID_SOCKET)
	{
		shutdown(mTcpSocket, SD_SEND);
		closesocket(mTcpSocket);
		mTcpSocket = INVALID_SOCKET;
		shutdown(mUdpSocket, SD_SEND);
		closesocket(mUdpSocket);
		mUdpSocket = INVALID_SOCKET;
	}
}

BattleGameNetworkManager::IoResult BattleGameNetworkManager::HandleResult(int result, int& outErrorCode)
{
	outErrorCode = 0;
	if (result == SOCKET_ERROR)
	{
		outErrorCode = WSAGetLastError();
		if (outErrorCode == WSAEWOULDBLOCK)
		{
			return WOULD_BLOCK;
		}
		else if (outErrorCode == WSAECONNRESET || outErrorCode == WSAECONNABORTED)
		{
			return DISCONNECTED;
		}
		else
		{
			UE_LOG(LogBattleGameNetwork, Error, TEXT("TCP 메시지를 보내던 도중 에러가 발생하였습니다: %d"), outErrorCode);
			return DISCONNECTED;
		}
	}
	else
	{
		if (result == 0)
		{
			return DISCONNECTED;
		}
		else
		{
			return SUCCESSFUL;
		}
	}
}

void BattleGameNetworkManager::OnDisconnected(int32 reason)
{
	Cast<UBattleGameInstance>(mpLastGameModeContext->GetGameInstance())->OnDisconnected(reason);
}

void BattleGameNetworkManager::RequestSendMessage(EBattleGameSendReliability reliability, Message&& message)
{
	if (reliability == EBattleGameSendReliability::RELIABLE)
	{
		mTcpSendQueue.Enqueue(std::move(message));
	}
	else
	{
		SendUdp(std::move(message));
	}
}

void BattleGameNetworkManager::SendTcp()
{
	if (mTcpSocket == INVALID_SOCKET)
	{
		return;
	}

	Message* messageToSend = mTcpSendQueue.Peek();
	if (messageToSend != nullptr)
	{
		char* pDataToSend = (mCurrentSent < MESSAGE_HEADER_SIZE ? reinterpret_cast<char*>(messageToSend) : (messageToSend->mpBodyBuffer.Get()) - MESSAGE_HEADER_SIZE) + mCurrentSent;
		int lengthToSend = (mCurrentSent < MESSAGE_HEADER_SIZE ? MESSAGE_HEADER_SIZE : messageToSend->mHeaderBodySize + MESSAGE_HEADER_SIZE) - mCurrentSent;
		int result = send(mTcpSocket, pDataToSend, lengthToSend, 0);
		int errorCode;
		switch (HandleResult(result, errorCode))
		{
		case SUCCESSFUL:
			mCurrentSent += result;
			check(mCurrentSent <= MESSAGE_HEADER_SIZE + messageToSend->mHeaderBodySize);
			if (mCurrentSent == MESSAGE_HEADER_SIZE + messageToSend->mHeaderBodySize)
			{
				mTcpSendQueue.Pop();
				mCurrentSent = 0;
			}
			break;
		case DISCONNECTED:
			CleanupSocket();
			OnDisconnected(static_cast<int32>(errorCode));
			return;
		case WOULD_BLOCK:
			break;
		}
	}
}

void BattleGameNetworkManager::ReceiveTcp()
{
	if (mTcpSocket == INVALID_SOCKET)
	{
		return;
	}
	auto completeMessage = [this]()
		{
			Message message;
			message.mHeaderBodySize = mTotalSizeToReceive;
			message.mHeaderMessageType = mLastReceivedHeaderType;
			message.mpBodyBuffer = TUniquePtr<char>(new char[mTotalSizeToReceive]);
			memcpy(message.mpBodyBuffer.Get(), mReceiveBuffer, mTotalSizeToReceive);

			mTotalSizeToReceive = MESSAGE_HEADER_SIZE;
			mIsReceivingHeader = true;

			mpStcRpcInstance->Handle(message);
		};
	int result = recv(mTcpSocket, (mReceiveBuffer + mCurrentReceived), (mTotalSizeToReceive - mCurrentReceived), 0);
	int errorCode;
	switch (HandleResult(result, errorCode))
	{
	case SUCCESSFUL:
		mCurrentReceived += result;
		check(mCurrentReceived <= mTotalSizeToReceive);
		if (mCurrentReceived == mTotalSizeToReceive)
		{
			if (mIsReceivingHeader)
			{
				memcpy(&this->mTotalSizeToReceive, this->mReceiveBuffer, 4);
				memcpy(&this->mLastReceivedHeaderType, this->mReceiveBuffer + 4, 4);
				check(mTotalSizeToReceive <= MAX_MESSAGE_SIZE);

				if (mTotalSizeToReceive == 0)
				{
					completeMessage();
				}
				else
				{
					mIsReceivingHeader = false;
				}
			}
			else
			{
				completeMessage();
			}
			mCurrentReceived = 0;
			ZeroMemory(mReceiveBuffer, MAX_MESSAGE_SIZE);
		}
		break;
	case DISCONNECTED:
		CleanupSocket();
		OnDisconnected(static_cast<int32>(errorCode));
		return;
	case WOULD_BLOCK:
		break;
	}
}

void BattleGameNetworkManager::SendUdp(Message&& message)
{
	if (mUdpSocket == INVALID_SOCKET)
	{
		return;
	}

	char serializedMessage[MAX_MESSAGE_SIZE];
	int serializedMessageLength = 4 + 4 + message.mHeaderBodySize;
	memcpy(serializedMessage, &message.mHeaderBodySize, 4);
	memcpy(serializedMessage + 4, &message.mHeaderMessageType, 4);
	memcpy(serializedMessage + 8, message.mpBodyBuffer.Get(), message.mHeaderBodySize);

	int result = sendto(mUdpSocket, serializedMessage, serializedMessageLength, 0, reinterpret_cast<struct sockaddr*>(mServerAddrIn), mServerAddrLen);
	if (result == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSAEWOULDBLOCK)
		{
			UE_LOG(LogBattleGameNetwork, Error, TEXT("UDP 메시지 송신 중 에러가 발생하였습니다: 에러 코드 %d"), errorCode);
		}
	}
}

void BattleGameNetworkManager::ReceiveUdp()
{
	if (mUdpSocket == INVALID_SOCKET)
	{
		return;
	}

	char serializedMessage[MAX_MESSAGE_SIZE];

	int result = recvfrom(mUdpSocket, serializedMessage, MAX_MESSAGE_SIZE, 0, reinterpret_cast<struct sockaddr*>(mServerAddrIn), &mServerAddrLen);
	if (result == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSAEWOULDBLOCK)
		{
			UE_LOG(LogBattleGameNetwork, Error, TEXT("UDP 메시지 수신 중 에러가 발생하였습니다: 에러 코드 %d"), errorCode);
		}
		return;
	}

	Message message;
	memcpy(&message.mHeaderBodySize, serializedMessage, 4);
	memcpy(&message.mHeaderMessageType, serializedMessage+4, 4);
	char* body = new char[message.mHeaderBodySize];
	memcpy(body, serializedMessage + 8, message.mHeaderBodySize);
	message.mpBodyBuffer = TUniquePtr<char>(body);
	
	mpStcRpcInstance->Handle(message);
}

AGameModeBase* BattleGameNetworkManager::GetLastGameModeContext()
{
	if (!IsValid(mpLastGameModeContext))
	{ 
		mpLastGameModeContext = nullptr;
	} 
	return mpLastGameModeContext; 
}