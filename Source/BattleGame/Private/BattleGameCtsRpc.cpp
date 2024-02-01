// Copyright floweryclover 2024, All rights reserved.


#include "BattleGameCtsRpc.h"
#include "BattleGameInstance.h"
#include <cstring>

UBattleGameCtsRpc::UBattleGameCtsRpc() {}

UBattleGameCtsRpc::~UBattleGameCtsRpc(){}

void UBattleGameCtsRpc::Init(UBattleGameInstance* _battleGameInstance)
{
	battleGameInstance = _battleGameInstance;
}

void UBattleGameCtsRpc::RequestMatchMaking()
{
	Message message;
	message.headerBodySize = 0;
	message.headerMessageType = CTS_REQUEST_MATCHMAKING;
	message.bodyBuffer = nullptr;
	battleGameInstance->GetSendQueue().Enqueue(std::move(message));
}