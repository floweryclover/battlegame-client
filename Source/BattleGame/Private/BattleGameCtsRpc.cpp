// Copyright floweryclover 2024, All rights reserved.


#include "BattleGameCtsRpc.h"
#include "LogBattleGameNetwork.h"
#include "BattleGameNetworkManager.h"
#include "BattleGameNetworkEnums.h"
#include <cstring>

void UBattleGameCtsRpc::RequestMatchMaking()
{
	Message message(0, CTS_REQUEST_MATCHMAKING, nullptr);
	BattleGameNetworkManager::GetInstance()
		.RequestSendMessage(EBattleGameSendReliability::RELIABLE, std::move(message));
}

void UBattleGameCtsRpc::MoveCharacter(const FVector& position)
{
	char body[24];
	memcpy(body, &position.X, 8);
	memcpy(body+8, &position.Y, 8);
	memcpy(body+16, &position.Z, 8);

	Message message(24, CTS_MOVE_CHARACTER, body);
	BattleGameNetworkManager::GetInstance()
		.RequestSendMessage(EBattleGameSendReliability::UNRELIABLE, std::move(message));
}

void UBattleGameCtsRpc::AckUdpToken(unsigned long long token)
{
	Message message(8, CTS_ACK_UDP_TOKEN, reinterpret_cast<char*>(&token));
	BattleGameNetworkManager::GetInstance()
		.RequestSendMessage(EBattleGameSendReliability::UNRELIABLE, std::move(message));
}