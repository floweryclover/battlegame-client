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

// FNetworkRunnable

FNetworkRunnable::FNetworkRunnable(SOCKET tcpSocket, SOCKET udpSocket, FString serverAddress, int32 serverPort)
	: mStopping(false),
	mTcpSocket(tcpSocket),
	mUdpSocket(udpSocket),
	mCurrentSent(0),
	mCurrentReceived(0),
	mTotalSizeToReceive(MESSAGE_HEADER_SIZE),
	mIsReceivingHeader(true),
	mpServerAddrIn(new struct sockaddr_in())
{
	ZeroMemory(mpServerAddrIn.Get(), sizeof(struct sockaddr_in));
	mpServerAddrIn->sin_family = AF_INET;
	mpServerAddrIn->sin_port = htons(serverPort);
	check(inet_pton(AF_INET, TCHAR_TO_ANSI(*serverAddress), &mpServerAddrIn->sin_addr) == 1);

	mThread = FRunnableThread::Create(this, UTF8_TO_TCHAR("NetworkThread"));
}

FNetworkRunnable::~FNetworkRunnable()
{
	if (mThread != nullptr)
	{
		mThread->Kill();
		delete mThread;
	}

	if (mTcpSocket != INVALID_SOCKET)
	{
		shutdown(mTcpSocket, SD_SEND);
		closesocket(mTcpSocket);
		mTcpSocket = INVALID_SOCKET;
	}
	if (mUdpSocket != INVALID_SOCKET)
	{
		shutdown(mUdpSocket, SD_SEND);
		closesocket(mUdpSocket);
		mUdpSocket = INVALID_SOCKET;
	}
}

bool FNetworkRunnable::Init()
{
	return true;
}

uint32 FNetworkRunnable::Run()
{
	while (!mStopping)
	{
		//FPlatformProcess::Sleep(0.05);
		SendUdp();
		SendTcp();
		ReceiveTcp();
		ReceiveUdp();
	}
	return 0;
}

void FNetworkRunnable::Exit()
{
}

void FNetworkRunnable::Stop()
{mStopping = true;}

FNetworkRunnable::IoResult FNetworkRunnable::HandleResult(int result, int& outErrorCode)
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

void FNetworkRunnable::SendTcp()
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
			FFunctionGraphTask::CreateAndDispatchWhenReady([errorCode]() { BattleGameNetworkManager::GetInstance().OnDisconnected(static_cast<int32>(errorCode)); }, TStatId(), nullptr, ENamedThreads::GameThread);
			mStopping = true;
			return;
		case WOULD_BLOCK:
			break;
		}
	}
}

void FNetworkRunnable::ReceiveTcp()
{
	if (mTcpSocket == INVALID_SOCKET)
	{return;}

	auto completeMessage = [this]()
		{
			Message message;
			message.mHeaderBodySize = mTotalSizeToReceive;
			message.mHeaderMessageType = mLastReceivedHeaderType;
			message.mpBodyBuffer = TUniquePtr<char>(new char[mTotalSizeToReceive]);
			memcpy(message.mpBodyBuffer.Get(), mReceiveBuffer, mTotalSizeToReceive);
			mTotalSizeToReceive = MESSAGE_HEADER_SIZE;
			mIsReceivingHeader = true;
			FFunctionGraphTask::CreateAndDispatchWhenReady([message = std::move(message)]() { BattleGameNetworkManager::GetInstance().GetBattleGameStcRpc()->Handle(message); }, TStatId(), nullptr, ENamedThreads::GameThread);
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
				{completeMessage();}
				else
				{mIsReceivingHeader = false;}
			}
			else
			{completeMessage();}
			mCurrentReceived = 0;
			ZeroMemory(mReceiveBuffer, MAX_MESSAGE_SIZE);
		}
		break;
	case DISCONNECTED:
		FFunctionGraphTask::CreateAndDispatchWhenReady([errorCode]() { BattleGameNetworkManager::GetInstance().OnDisconnected(static_cast<int32>(errorCode)); }, TStatId(), nullptr, ENamedThreads::GameThread);
		mStopping = true;
		return;
	case WOULD_BLOCK:
		break;
	}
}

void FNetworkRunnable::SendUdp()
{
	if (mUdpSocket == INVALID_SOCKET || mUdpSendQueue.IsEmpty())
	{return;}

	auto message = mUdpSendQueue.Peek();

	char serializedMessage[MAX_MESSAGE_SIZE];
	int serializedMessageLength = 4 + 4 + message->mHeaderBodySize;
	memcpy(serializedMessage, &message->mHeaderBodySize, 4);
	memcpy(serializedMessage + 4, &message->mHeaderMessageType, 4);
	memcpy(serializedMessage + 8, message->mpBodyBuffer.Get(), message->mHeaderBodySize);
	mUdpSendQueue.Pop();

	int result = sendto(mUdpSocket, serializedMessage, serializedMessageLength, 0, reinterpret_cast<struct sockaddr*>(mpServerAddrIn.Get()), static_cast<socklen_t>(sizeof(struct sockaddr_in)));
	if (result == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSAEWOULDBLOCK)
		{UE_LOG(LogBattleGameNetwork, Error, TEXT("UDP 메시지 송신 중 에러가 발생하였습니다: 에러 코드 %d"), errorCode);}
	}
}

void FNetworkRunnable::ReceiveUdp()
{
	if (mUdpSocket == INVALID_SOCKET)
	{
		return;
	}

	char serializedMessage[MAX_MESSAGE_SIZE];
	socklen_t addrLen = sizeof(struct sockaddr_in);
	int result = recvfrom(mUdpSocket, serializedMessage, MAX_MESSAGE_SIZE, 0, reinterpret_cast<struct sockaddr*>(mpServerAddrIn.Get()), &addrLen);
	if (result == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSAEWOULDBLOCK)
		{UE_LOG(LogBattleGameNetwork, Error, TEXT("UDP 메시지 수신 중 에러가 발생하였습니다: 에러 코드 %d"), errorCode);}
		return;
	}

	Message message;
	memcpy(&message.mHeaderBodySize, serializedMessage, 4);
	memcpy(&message.mHeaderMessageType, serializedMessage + 4, 4);
	char* body = new char[message.mHeaderBodySize];
	memcpy(body, serializedMessage + 8, message.mHeaderBodySize);
	message.mpBodyBuffer = TUniquePtr<char>(body);
	FFunctionGraphTask::CreateAndDispatchWhenReady([message = std::move(message)]() { BattleGameNetworkManager::GetInstance().GetBattleGameStcRpc()->Handle(message); }, TStatId(), nullptr, ENamedThreads::GameThread);
}

void FNetworkRunnable::EnqueueMessage(EBattleGameSendReliability reliability, Message&& message)
{
	if (reliability == EBattleGameSendReliability::RELIABLE)
	{mTcpSendQueue.Enqueue(std::move(message));}
	else
	{mUdpSendQueue.Enqueue(std::move(message));}
}

// BattleGameNetworkManager

TUniquePtr<BattleGameNetworkManager> BattleGameNetworkManager::spSingleton = nullptr;

BattleGameNetworkManager& BattleGameNetworkManager::GetInstance()
{
	if (spSingleton == nullptr)
	{spSingleton = MakeUnique<BattleGameNetworkManager>();}
	return *spSingleton;
}

BattleGameNetworkManager::BattleGameNetworkManager() noexcept
	: mpStcRpcInstance(NewObject<UBattleGameStcRpc>()),
	mpGameModeContext(nullptr)
{
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	check(result == 0);
}

BattleGameNetworkManager::~BattleGameNetworkManager()
{
	WSACleanup();
}


bool BattleGameNetworkManager::Connect(const FString& serverAddress, int32 serverPort, int32& errorCode)
{
	struct sockaddr_in serverAddrIn {};
	ZeroMemory(&serverAddrIn, sizeof(serverAddrIn));
	serverAddrIn.sin_family = AF_INET;
	serverAddrIn.sin_port = htons(serverPort);
	int result = inet_pton(AF_INET, TCHAR_TO_ANSI(*serverAddress), &serverAddrIn.sin_addr);
	check(result == 1);

	SOCKET tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	check(tcpSocket != INVALID_SOCKET);
	result = connect(tcpSocket, reinterpret_cast<struct sockaddr*>(&serverAddrIn), sizeof(serverAddrIn));
	if (result == SOCKET_ERROR)
	{
		errorCode = WSAGetLastError();
		closesocket(tcpSocket);
		return false;
	}
	else
	{
		u_long nonBlockingModeFlag = 1;
		result = ioctlsocket(tcpSocket, FIONBIO, &nonBlockingModeFlag);
		check(result == 0);

		struct sockaddr_in boundTcpAddr;
		socklen_t len = sizeof(boundTcpAddr);
		result = getsockname(tcpSocket, reinterpret_cast<struct sockaddr*>(&boundTcpAddr), &len);
		check(result != SOCKET_ERROR);

		SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		check(udpSocket != INVALID_SOCKET);
		struct sockaddr_in udpAddr;
		ZeroMemory(&udpAddr, sizeof(udpAddr));
		udpAddr.sin_family = AF_INET;
		udpAddr.sin_addr.S_un.S_addr = INADDR_ANY;
		udpAddr.sin_port = boundTcpAddr.sin_port;
		result = bind(udpSocket, reinterpret_cast<struct sockaddr*>(&udpAddr), sizeof(udpAddr));
		check(result != SOCKET_ERROR);
		result = ioctlsocket(udpSocket, FIONBIO, &nonBlockingModeFlag);
		check(result == 0);

		check(mpNetworkRunnable == nullptr);
		mpNetworkRunnable = MakeUnique<FNetworkRunnable>(tcpSocket, udpSocket, serverAddress, serverPort);
		errorCode = 0;
		return true;
	}
}

AGameModeBase* BattleGameNetworkManager::GetGameModeContext()
{
	if (!IsValid(mpGameModeContext))
	{
		mpGameModeContext = nullptr;
	}
	return mpGameModeContext;
}

void BattleGameNetworkManager::EnqueueMessage(EBattleGameSendReliability reliability, Message&& message)
{
	if (mpNetworkRunnable == nullptr)
	{
		return;
	}

	mpNetworkRunnable->EnqueueMessage(reliability, std::move(message));
}

void BattleGameNetworkManager::OnDisconnected(int32 reason)
{
	mpNetworkRunnable.Reset(nullptr);
	Cast<UBattleGameInstance>(mpGameModeContext->GetGameInstance())->OnDisconnected(reason);
}