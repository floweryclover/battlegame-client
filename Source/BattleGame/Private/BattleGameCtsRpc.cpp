// Copyright floweryclover 2024, All rights reserved.


#include "BattleGameCtsRpc.h"
#include "BattleGameNetworkManager.h"
#include "BattleGameNetworkEnums.h"
#include <cstring>

void UBattleGameCtsRpc::RequestMatchMaking()
{
	Message message;
	message.headerBodySize = 0;
	message.headerMessageType = CTS_REQUEST_MATCHMAKING;
	message.bodyBuffer = nullptr;
	BattleGameNetworkManager::GetInstance().RequestSendMessage(EBattleGameSendReliability::RELIABLE, std::move(message));
}

void UBattleGameCtsRpc::SendSomethingUnreliable()
{
	
}