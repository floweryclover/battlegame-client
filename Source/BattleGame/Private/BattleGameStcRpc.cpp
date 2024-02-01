// Copyright floweryclover 2024, All rights reserved.


#include "BattleGameStcRpc.h"
#include "BattleGameInstance.h"

UBattleGameStcRpc::UBattleGameStcRpc()
{
}

UBattleGameStcRpc::~UBattleGameStcRpc()
{
}

void UBattleGameStcRpc::Init(UBattleGameInstance* _battleGameInstance)
{
	this->battleGameInstance = _battleGameInstance;
}

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
	battleGameInstance->GetWorld()->ServerTravel("/Game/BattleLevel");
}

void UBattleGameStcRpc::OnDisconnectedFromGame()
{
	battleGameInstance->GetWorld()->ServerTravel("/Game/MainMenuLevel");
}