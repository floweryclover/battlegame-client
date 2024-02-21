// Copyright floweryclover 2024, All rights reserved.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BattleGameStcRpc.generated.h"

struct Message;

namespace Level
{
	static constexpr int MAINMENU = 0;
	static constexpr int ONE_VS_ONE = 1;
}

/**
 *
 */
UCLASS()
class UBattleGameStcRpc : public UObject
{
	GENERATED_BODY()

public:
	static constexpr int STC_OPEN_LEVEL = 1;
	static constexpr int STC_DISCONNECTED_FROM_GAME = 2;
	static constexpr int STC_SPAWN_ENTITY = 4;
	static constexpr int STC_DESPAWN_ENTITY = 5;
	static constexpr int STC_POSSESS_ENTITY = 6;
	static constexpr int STC_MOVE_ENTITY = 7;
	static constexpr int STC_SET_TIMER = 8;
	static constexpr int STC_SIGNAL_GAME_STATE = 9;
	static constexpr int STC_RESPAWN_ENTITY = 10;
	static constexpr int STC_SET_SCORE = 11;
	static constexpr int STC_ASSIGN_TEAM_ID = 12;
	static constexpr int STC_ASSIGN_ENTITY_NICKNAME = 13;
	static constexpr int STC_GET_MY_NICKNAME = 14;
	static constexpr int STC_SEND_GAME_DATA = 15;
	static constexpr int STC_SEND_GAME_RESULT = 16;

	UBattleGameStcRpc() = default;
	virtual ~UBattleGameStcRpc() = default;

	void Handle(const Message& message);
	
	void OnOpenLevel(int32 level);
	void OnDisconnectedFromGame();
	void OnSpawnEntity(int32 entityId, const FVector& location, double direction);
	void OnDespawnEntity(int32 entityId);
	void OnPossessEntity(int32 entityId);
	void OnMoveEntity(int32 entityId, const FVector& location, double direction);
	void OnSetTimer(int32 seconds, const FText& text);
	void OnSignalGameState(int32 signal);
	void OnRespawnEntity(int32 entityId, const FVector& location, double direction);
	void OnSetScore(int32 team, int32 score);
	void OnAssignTeamId(int32 teamId);
	void OnAssignEntityNickname(int32 entityId, const FText& nickname);
	void OnGetMyNickname(const FText& nickname);
	void OnSendGameData(const FString& myNickname, const FString& opponentNickname);
	void OnSendGameResult(bool isGoodGame, bool isWinner, int32 myScore, int32 opponentScore);
};
