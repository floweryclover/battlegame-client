// Copyright floweryclover 2024, All rights reserved.


#include "BattleGameStcRpc.h"
#include "BattleGameCtsRpc.h"
#include "BattleGameNetworkManager.h"
#include "BattleGameNetworkMessage.h"
#include "GameFramework/GameModeBase.h"

void UBattleGameStcRpc::Handle(const Message& message)
{
	switch (message.mHeaderMessageType)
	{
	case STC_JOINED_GAME_ROOM:
		OnJoinedGameRoom();
		break;
	case STC_DISCONNECTED_FROM_GAME:
		OnDisconnectedFromGame();
		break;
	case STC_ASSIGN_UDP_TOKEN:
		unsigned long long token;
		memcpy(&token, message.mpBodyBuffer.Get(), 8);
		OnAssignUdpToken(token);
		break;
	}
}

void UBattleGameStcRpc::OnJoinedGameRoom()
{
	BattleGameNetworkManager::GetInstance()
		.GetLastGameModeContext()
		->GetWorld()
		->ServerTravel("/Game/BattleLevel");
}

void UBattleGameStcRpc::OnDisconnectedFromGame()
{
	BattleGameNetworkManager::GetInstance()
		.GetLastGameModeContext()
		->GetWorld()
		->ServerTravel("/Game/MainMenuLevel");
}

void UBattleGameStcRpc::OnAssignUdpToken(unsigned long long token)
{
	BattleGameNetworkManager::GetInstance()
		.GetBattleGameCtsRpc()
		->AckUdpToken(token);
}