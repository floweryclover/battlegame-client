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
	void OnSpawnEntity();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDespawnEntity();

	UFUNCTION(BlueprintImplementableEvent)
	void OnPossessEntity();

protected:
	UPROPERTY(BlueprintReadWrite)
	TMap<int32, APawn*> mEntities;
};
