// Copyright floweryclover 2024, All rights reserved.


#include "BattleGameStcRpc.h"
#include "BattleGameNetworkManager.h"
#include "GameFramework/GameModeBase.h"

void UBattleGameStcRpc::Handle(const Message& message)
{
	switch (message.headerMessageType)
	{
	case STC_JOINED_GAME_ROOM:
		OnJoinedGameRoom();
		break;
	case STC_DISCONNECTED_FROM_GAME:
		OnDisconnectedFromGame();
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