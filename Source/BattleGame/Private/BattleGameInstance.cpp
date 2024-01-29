// Copyright floweryclover 2024, All rights reserved.

#include "BattleGameInstance.h"
#include "BattleGameCtsRpc.h"
#include "BattleGameStcRpc.h"
#include <functional>
#include <cstring>
#include <Winsock2.h>
#include <WS2tcpip.h>

DEFINE_LOG_CATEGORY(LogBattleGameNetwork);

UBattleGameInstance::UBattleGameInstance() {}

UBattleGameInstance::~UBattleGameInstance() {}

void UBattleGameInstance::Init()
{
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	check(result == 0);

	clientSocket = INVALID_SOCKET;
	currentSent = 0;
	totalSizeToReceive = MESSAGE_HEADER_SIZE;
	isReceivingHeader = true;
	currentReceived = 0;
	memset(receiveBuffer, 0, MAX_MESSAGE_SIZE);
	pCtsRpcInstance = NewObject<UBattleGameCtsRpc>();
	pCtsRpcInstance->Init(&this->sendQueue);
	pStcRpcInstance = NewObject<UBattleGameStcRpc>();
}

void UBattleGameInstance::Shutdown()
{
	CleanupSocket();
	WSACleanup();
}

bool UBattleGameInstance::ConnectToServer(const FString& serverAddress, const int32 serverPort, int32& errorCode)
{
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	check(clientSocket != INVALID_SOCKET);

	struct sockaddr_in serverSockAddrIn;
	memset(&serverSockAddrIn, 0, sizeof(struct sockaddr_in));
	serverSockAddrIn.sin_family = AF_INET;
	serverSockAddrIn.sin_port = htons(serverPort);

	int result = inet_pton(AF_INET, TCHAR_TO_ANSI(*serverAddress), (void*)(&serverSockAddrIn.sin_addr));
	check(result == 1);

	result = connect(clientSocket, (struct sockaddr*)(&serverSockAddrIn), sizeof(serverSockAddrIn));
	if (result == SOCKET_ERROR)
	{
		errorCode = WSAGetLastError();
		CleanupSocket();
		return false;
	}
	else
	{
		u_long nonBlockingModeFlag = 1;
		result = ioctlsocket(clientSocket, FIONBIO, &nonBlockingModeFlag);
		check(result == 0);
		errorCode = 0;
		return true;
	}
}

bool UBattleGameInstance::ProcessNetworkTasks()
{
	enum Result
	{
		WOULD_BLOCK,
		DISCONNECTED,
		SUCCESSFUL
	};

	auto handleResult = [this](int result, int& outErrorCode)
		{
			if (result == SOCKET_ERROR)
			{
				int errorCode = WSAGetLastError();
				if (outErrorCode)
				{
					outErrorCode = errorCode;
				}
				if (errorCode == WSAEWOULDBLOCK)
				{
					return WOULD_BLOCK;
				}
				else if (errorCode == WSAECONNRESET || errorCode == WSAECONNABORTED)
				{
					return DISCONNECTED;
				}
				else
				{
					UE_LOG(LogBattleGameNetwork, Error, TEXT("An error occured while sending data: %d"), errorCode);
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
		};

	Message* messageToSend = sendQueue.Peek();
	if (messageToSend != nullptr)
	{
		char* pDataToSend = (currentSent < MESSAGE_HEADER_SIZE ? reinterpret_cast<char*>(messageToSend) : (messageToSend->bodyBuffer.Get())- MESSAGE_HEADER_SIZE) + currentSent;
		int lengthToSend = (currentSent < MESSAGE_HEADER_SIZE ? MESSAGE_HEADER_SIZE : messageToSend->headerBodySize+ MESSAGE_HEADER_SIZE) - currentSent;
		int result = send(clientSocket, pDataToSend, lengthToSend, 0);
		int errorCode;
		switch (handleResult(result, errorCode))
		{
		case Result::SUCCESSFUL:
			currentSent += result;
			check(currentSent <= MESSAGE_HEADER_SIZE + messageToSend->headerBodySize);
			if (currentSent == MESSAGE_HEADER_SIZE + messageToSend->headerBodySize)
			{
				sendQueue.Pop();
				currentSent = 0;
			}
			break;
		case Result::DISCONNECTED:
			CleanupSocket();
			OnDisconnectedEvent(static_cast<int32>(errorCode));
			return false;
		case Result::WOULD_BLOCK:
			break;
		}
	}

	auto completeMessage = [this]()
		{
			Message message;
			message.headerBodySize = totalSizeToReceive;
			message.headerMessageType = lastReceivedHeaderType;
			message.bodyBuffer = TUniquePtr<char>(new char[totalSizeToReceive]);
			memcpy(message.bodyBuffer.Get(), receiveBuffer, totalSizeToReceive);

			UE_LOG(LogBattleGameNetwork, Log, TEXT("수신 헤더 바디크기: %d, 타입: %d"), message.headerBodySize, message.headerMessageType);

			totalSizeToReceive = MESSAGE_HEADER_SIZE;
			isReceivingHeader = true;

			pStcRpcInstance->Handle(message);
		};
	int result = recv(clientSocket, (receiveBuffer + currentReceived), (totalSizeToReceive - currentReceived), 0);
	int errorCode;
	switch (handleResult(result, errorCode))
	{
	case Result::SUCCESSFUL:
		currentReceived += result;
		check(currentReceived <= totalSizeToReceive);
		if (currentReceived == totalSizeToReceive)
		{
			if (isReceivingHeader)
			{
				memcpy_s(&this->totalSizeToReceive, 4, this->receiveBuffer, 4);
				memcpy_s(&this->lastReceivedHeaderType, 4, this->receiveBuffer+4, 4);
				check(totalSizeToReceive <= MAX_MESSAGE_SIZE);
				
				if (totalSizeToReceive == 0)
				{
					completeMessage();
				}
				else
				{
					isReceivingHeader = false;
				}
			}
			else
			{
				completeMessage();
			}
			currentReceived = 0;
			ZeroMemory(this->receiveBuffer, MAX_MESSAGE_SIZE);
		}
		break;
	case Result::DISCONNECTED:
		CleanupSocket();
		OnDisconnectedEvent(static_cast<int32>(errorCode));
		return false;
	case Result::WOULD_BLOCK:
		break;
	}

	return true;
}

FText UBattleGameInstance::InterpretWsaErrorCode(const int32 wsaErrorCode) const
{
	FString reason;
	switch (wsaErrorCode)
	{
	case WSAEWOULDBLOCK:
		reason = FString(TEXT("IO 작업이 Would Block 상태입니다."));
		break;
	case WSAECONNABORTED:
	case WSAECONNRESET:
		reason = FString(TEXT("서버와의 연결이 끊어졌습니다."));
		break;
	case WSAECONNREFUSED:
		reason = FString(TEXT("서버에 연결할 수 없습니다."));
		break;
	default:
		reason = FString(TEXT("알 수 없는 에러입니다."));
		break;
	}

	return FText::FromString(reason);
}

void UBattleGameInstance::CleanupSocket()
{
	sendQueue.Empty();
	currentSent = 0;
	totalSizeToReceive = MESSAGE_HEADER_SIZE;
	currentReceived = 0;
	lastReceivedHeaderType = 0;
	isReceivingHeader = true;
	if (clientSocket != INVALID_SOCKET)
	{
		shutdown(clientSocket, SD_SEND);
		closesocket(clientSocket);
		clientSocket = INVALID_SOCKET;
	}
}