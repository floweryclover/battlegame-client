// Copyright floweryclover 2024, All rights reserved.


#include "BattleGameCtsRpc.h"
#include <cstring>

UBattleGameCtsRpc::UBattleGameCtsRpc(){}

UBattleGameCtsRpc::~UBattleGameCtsRpc(){}

void UBattleGameCtsRpc::Init(TQueue<Message>* _pSendQueue)
{
	this->pSendQueue = _pSendQueue;
}

void UBattleGameCtsRpc::RequestJoinGame()
{
	Message message;
	message.headerBodySize = 0;
	message.headerMessageType = CTS_REQUEST_JOIN_GAME;
	message.bodyBuffer = nullptr;
	check(pSendQueue->Enqueue(std::move(message)));
}