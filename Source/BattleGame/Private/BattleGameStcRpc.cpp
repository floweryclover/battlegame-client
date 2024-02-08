// Copyright floweryclover 2024, All rights reserved.


#include "BattleGameStcRpc.h"
#include "BattleGameCtsRpc.h"
#include "BattleGameNetworkManager.h"
#include "BattleGameNetworkMessage.h"
#include "BattleGameMode.h"
#include "LogBattleGameNetwork.h"
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
	{
		int32 entityId;
		FVector location;
		double direction;
		memcpy(&entityId, message.mpBodyBuffer.Get(), 4);
		memcpy(&location.X, message.mpBodyBuffer.Get() + 4, 8);
		memcpy(&location.Y, message.mpBodyBuffer.Get() + 12, 8);
		memcpy(&location.Z, message.mpBodyBuffer.Get() + 20, 8);
		memcpy(&direction, message.mpBodyBuffer.Get() + 28, 8);
		OnSpawnEntity(entityId, location, direction);
		break;
	}
	case STC_DESPAWN_ENTITY:
	{
		int32 entityId;
		memcpy(&entityId, message.mpBodyBuffer.Get(), 4);
		OnDespawnEntity(entityId);
		break;
	}
	case STC_POSSESS_ENTITY:
	{
		int32 entityId;
		memcpy(&entityId, message.mpBodyBuffer.Get(), 4);
		OnPossessEntity(entityId);
		break;
	}
	case STC_MOVE_ENTITY:
	{
		int32 entityId;
		FVector location;
		double direction;
		memcpy(&entityId, message.mpBodyBuffer.Get(), 4);
		memcpy(&location.X, message.mpBodyBuffer.Get() + 4, 8);
		memcpy(&location.Y, message.mpBodyBuffer.Get() + 12, 8);
		memcpy(&location.Z, message.mpBodyBuffer.Get() + 20, 8);
		memcpy(&direction, message.mpBodyBuffer.Get() + 28, 8);
		OnMoveEntity(entityId, location, direction);
		break;
	}
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

void UBattleGameStcRpc::OnSpawnEntity(int32 entityId, const FVector& location, double direction)
{
	auto pBattleGameMode = Cast<ABattleGameMode>(BattleGameNetworkManager::GetInstance().GetLastGameModeContext());
	if (pBattleGameMode == nullptr)
	{
		UE_LOG(LogBattleGameNetwork, Error, TEXT("BattleGameMode가 준비되지 않았습니다."));
		return;
	}

	pBattleGameMode->OnSpawnEntity(entityId, location, direction);
}

void UBattleGameStcRpc::OnDespawnEntity(int32 entityId)
{
	auto pBattleGameMode = Cast<ABattleGameMode>(BattleGameNetworkManager::GetInstance().GetLastGameModeContext());
	if (pBattleGameMode == nullptr)
	{
		UE_LOG(LogBattleGameNetwork, Error, TEXT("BattleGameMode가 준비되지 않았습니다."));
		return;
	}

	pBattleGameMode->OnDespawnEntity(entityId);
}

void UBattleGameStcRpc::OnPossessEntity(int32 entityId)
{
	auto pBattleGameMode = Cast<ABattleGameMode>(BattleGameNetworkManager::GetInstance().GetLastGameModeContext());
	if (pBattleGameMode == nullptr)
	{
		UE_LOG(LogBattleGameNetwork, Error, TEXT("BattleGameMode가 준비되지 않았습니다."));
		return;
	}

	pBattleGameMode->OnPossessEntity(entityId);
}

void UBattleGameStcRpc::OnMoveEntity(int32 entityId, const FVector& location, double direction)
{
	auto pBattleGameMode = Cast<ABattleGameMode>(BattleGameNetworkManager::GetInstance().GetLastGameModeContext());
	if (pBattleGameMode == nullptr)
	{
		UE_LOG(LogBattleGameNetwork, Error, TEXT("BattleGameMode가 준비되지 않았습니다."));
		return;
	}

	pBattleGameMode->OnMoveEntity(entityId, location, direction);
}