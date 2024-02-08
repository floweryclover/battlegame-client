// Copyright floweryclover 2024, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameEntity.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UGameEntity : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class IGameEntity
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	int32 mEntityId;
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetEntityId(int32 id);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	int32 GetEntityId();
};
