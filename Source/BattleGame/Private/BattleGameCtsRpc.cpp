// Copyright floweryclover 2024, All rights reserved.


#include "BattleGameCtsRpc.h"
#include <cstring>

UBattleGameCtsRpc::UBattleGameCtsRpc(){}

UBattleGameCtsRpc::~UBattleGameCtsRpc(){}

void UBattleGameCtsRpc::Init(TQueue<Message>* _pSendQueue)
{
	this->pSendQueue = _pSendQueue;
}

void UBattleGameCtsRpc::Login(const FString& nickname)
{
	const char* chars = TCHAR_TO_UTF8(*nickname);
	int charLength = strlen(chars);
	char* body = new char[charLength];
	memcpy(body, chars, charLength);

	Message loginMessage;
	loginMessage.headerMessageType = 1;
	loginMessage.headerBodySize = charLength;
	loginMessage.bodyBuffer = TUniquePtr<char>(body);
	check(pSendQueue->Enqueue(std::move(loginMessage)));
}