// Copyright floweryclover 2024, All rights reserved.


#include "BattleGameNetwork.h"
#include "BattleGameInstance.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <cstring>

UBattleGameNetwork::UBattleGameNetwork(){}

UBattleGameNetwork::~UBattleGameNetwork(){}

void UBattleGameNetwork::Init(UBattleGameInstance* pInstance)
{
	check(IsValid(pInstance));
	pGameInstance = pInstance;
}

SOCKET UBattleGameNetwork::GetSocket() 
{
	check(IsValid(pGameInstance));
	SOCKET socket = pGameInstance->GetSocket();
	check(socket != INVALID_SOCKET);
	return socket;
}

TQueue<Message>& UBattleGameNetwork::GetSendQueue()
{
	check(IsValid(pGameInstance));
	return pGameInstance->GetSendQueue();
}

void UBattleGameNetwork::Login(const FString& nickname)
{
	const char* chars = TCHAR_TO_UTF8(*nickname);
	int charLength = strlen(chars);
	char* body = new char[charLength];
	memcpy(body, chars, charLength);

	Message loginMessage;
	loginMessage.headerMessageType = 1;
	loginMessage.headerBodySize = charLength;
	loginMessage.bodyBuffer = TUniquePtr<char>(body);
	check(GetSendQueue().Enqueue(std::move(loginMessage)));
}