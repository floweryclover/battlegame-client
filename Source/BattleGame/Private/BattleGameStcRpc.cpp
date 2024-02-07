// Copyright floweryclover 2024, All rights reserved.


#include "BattleGameStcRpc.h"
#include "BattleGameCtsRpc.h"
#include "BattleGameNetworkManager.h"
#include "BattleGameNetworkMessage.h"
#include "BattleGameMode.h"
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
	case STC_SPAWN_ENTITY:
		OnSpawnEntity();
		break;
	case STC_DESPAWN_ENTITY:
		OnDespawnEntity();
		break;
	case STC_POSSESS_ENTITY:
		OnPossessEntity();
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

void UBattleGameStcRpc::OnSpawnEntity()
{
	auto pBattleGameMode = Cast<ABattleGameMode>(BattleGameNetworkManager::GetInstance().GetLastGameModeContext());
	if (pBattleGameMode == nullptr)
	{
		return;
	}

	pBattleGameMode->OnSpawnEntity();
}

void UBattleGameStcRpc::OnDespawnEntity()
{
	auto pBattleGameMode = Cast<ABattleGameMode>(BattleGameNetworkManager::GetInstance().GetLastGameModeContext());
	if (pBattleGameMode == nullptr)
	{
		return;
	}

	pBattleGameMode->OnDespawnEntity();
}

void UBattleGameStcRpc::OnPossessEntity()
{
	auto pBattleGameMode = Cast<ABattleGameMode>(BattleGameNetworkManager::GetInstance().GetLastGameModeContext());
	if (pBattleGameMode == nullptr)
	{
		return;
	}

	pBattleGameMode->OnPossessEntity();
}