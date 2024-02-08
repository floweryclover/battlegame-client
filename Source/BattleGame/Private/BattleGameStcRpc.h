// Copyright floweryclover 2024, All rights reserved.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BattleGameStcRpc.generated.h"

struct Message;

/**
 *
 */
UCLASS()
class UBattleGameStcRpc : public UObject
{
	GENERATED_BODY()

public:
	static constexpr int STC_JOINED_GAME_ROOM = 1;
	static constexpr int STC_DISCONNECTED_FROM_GAME = 2;
	static constexpr int STC_ASSIGN_UDP_TOKEN = 3;
	static constexpr int STC_SPAWN_ENTITY = 4;
	static constexpr int STC_DESPAWN_ENTITY = 5;
	static constexpr int STC_POSSESS_ENTITY = 6;
	static constexpr int STC_MOVE_ENTITY = 7;

	UBattleGameStcRpc() = default;
	virtual ~UBattleGameStcRpc() = default;

	void Handle(const Message& message);
	
	void OnJoinedGameRoom();
	void OnDisconnectedFromGame();
	void OnAssignUdpToken(unsigned long long token);
	void OnSpawnEntity(int32 entityId, const FVector& location, double direction);
	void OnDespawnEntity(int32 entityId);
	void OnPossessEntity(int32 entityId);
	void OnMoveEntity(int32 entityId, const FVector& location, double direction);
};
