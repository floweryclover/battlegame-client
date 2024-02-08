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

protected:
	UPROPERTY(BlueprintReadWrite)
	TMap<int32, APawn*> mEntities;
};
