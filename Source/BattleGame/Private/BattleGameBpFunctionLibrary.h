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
	static void InitializeBattleGameNetworkManager(const FString& serverAddress, int32 serverPort);

	UFUNCTION(BlueprintCallable)
	static bool Connect(int32& errorCode);

	UFUNCTION(BlueprintCallable)
	static void ManualTick(AGameModeBase* gameModeContext);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText InterpretWsaErrorCode(int32 wsaErrorCode);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UBattleGameCtsRpc* GetCtsRpcInstance();
};
