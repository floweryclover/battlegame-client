// Copyright floweryclover 2024, All rights reserved.

#include "BattleGameInstance.h"
#include <functional>
#include <cstring>

DEFINE_LOG_CATEGORY(LogBattleGameNetwork);

UBattleGameInstance::UBattleGameInstance() : clientSocket(INVALID_SOCKET), currentSent(0), totalSizeToReceive(MESSAGE_HEADER_SIZE), currentReceived(0), lastReceivedHeaderType(0)
{
	memset(receiveBuffer, 0, MAX_MESSAGE_SIZE);
}

UBattleGameInstance::~UBattleGameInstance()
{
}

void UBattleGameInstance::Init()
{
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	check(result == 0);
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

	const auto messageToSend = sendQueue.Peek();
	if (messageToSend != nullptr)
	{
		int result = send(clientSocket, reinterpret_cast<char*>(messageToSend + currentSent), (8+messageToSend->bodySize)-currentSent, 0);
		int errorCode;
		switch (handleResult(result, errorCode))
		{
		case Result::SUCCESSFUL:
			currentSent += result;
			check(currentSent <= 8 + messageToSend->bodySize);
			if (currentSent == 8 + messageToSend->bodySize)
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

	int result = recv(clientSocket, (receiveBuffer + currentReceived), (totalSizeToReceive - currentReceived), 0);
	int errorCode;
	switch (handleResult(result, errorCode))
	{
	case Result::SUCCESSFUL:
		currentReceived += result;
		check(currentReceived <= totalSizeToReceive);
		if (currentReceived == totalSizeToReceive)
		{
			if (totalSizeToReceive == MESSAGE_HEADER_SIZE)
			{
				memcpy_s(&this->lastReceivedHeaderType, 4, this->receiveBuffer, 4);
				memcpy_s(&this->totalSizeToReceive, 4, this->receiveBuffer + 4, 4);
				check(totalSizeToReceive <= MAX_MESSAGE_SIZE);
			}
			else
			{
				// deserialize body and handle that here...
				char* testMessage = new char[totalSizeToReceive];
				memcpy_s(testMessage, totalSizeToReceive, receiveBuffer, totalSizeToReceive);
				FString a(testMessage);
				UE_LOG(LogBattleGameNetwork, Log, TEXT("Received Test Message, Type: %d, Size: %d, Body: %s"), lastReceivedHeaderType, totalSizeToReceive, *a);
				delete[] testMessage;
				totalSizeToReceive = 8;
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

FText UBattleGameInstance::InterpretWsaErrorCode(const int32 wsaErrorCode)
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
	if (clientSocket != INVALID_SOCKET)
	{
		shutdown(clientSocket, SD_SEND);
		closesocket(clientSocket);
		clientSocket = INVALID_SOCKET;
	}
}