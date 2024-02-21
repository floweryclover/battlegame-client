// Copyright floweryclover 2024, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BattleGameBpFunctionLibrary.generated.h"

class AGameModeBase;
class UBattleGameCtsRpc;

/**
 * 
 */
UCLASS()
class UBattleGameBpFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static bool Connect(const FString& serverAddress, int32 serverPort, int32& errorCode);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText InterpretWsaErrorCode(int32 wsaErrorCode);

	UFUNCTION(BlueprintCallable)
	static void SetGameModeContext(AGameModeBase* context);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static int32 GetStringSizeInBytes(const FString& string);
};
