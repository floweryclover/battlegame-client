// Copyright floweryclover 2024, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BattleGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ABattleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnSpawnEntity(int32 entityId, const FVector& location, double direction);

	UFUNCTION(BlueprintImplementableEvent)
	void OnDespawnEntity(int32 entityId);

	UFUNCTION(BlueprintImplementableEvent)
	void OnPossessEntity(int32 entityId);

	UFUNCTION(BlueprintImplementableEvent)
	void OnMoveEntity(int32 entityId, const FVector& location, double direction);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnSignalGameState(int32 signal);

	UFUNCTION(BlueprintImplementableEvent)
	void OnRespawnEntity(int32 entityId, const FVector& location, double direction);

	UFUNCTION(BlueprintImplementableEvent)
	void OnSetScore(int32 team, int32 score);

	UFUNCTION(BlueprintImplementableEvent)
	void OnAssignTeamId(int32 teamId);	
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnAssignEntityNickname(int32 entityId, const FText& nickname);	

	UFUNCTION(BlueprintImplementableEvent)
	void OnSendGameData(const FString& myNickname, const FString& opponentNickname);

	UFUNCTION(BlueprintImplementableEvent)
	void OnSendGameResult(bool isGoodGame, bool isWinner, int32 myScore, int32 opponentScore);

	UFUNCTION(BlueprintImplementableEvent)
	void OnKnockbackEntity(int32 entityId, const FVector& location, const FVector& impulse);
protected:
	UPROPERTY(BlueprintReadWrite)
	TMap<int32, APawn*> mEntities;
};
