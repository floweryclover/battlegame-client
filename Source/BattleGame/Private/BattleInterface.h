// Copyright floweryclover 2024, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BattleInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UMovableEntity : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class IMovableEntity
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnMove(const FVector& location, double direction);
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UHasTimer : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class IHasTimer
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnSetTimer(int32 seconds, const FText& text);
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UReceivesMyNickname : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class IReceivesMyNickname
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnGetMyNickname(const FText& text);
};