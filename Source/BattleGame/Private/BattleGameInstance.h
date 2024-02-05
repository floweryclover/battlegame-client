// Copyright floweryclover 2024, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BattleGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class UBattleGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnDisconnected(int32 reason);
};
