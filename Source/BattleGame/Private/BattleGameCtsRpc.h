// Copyright floweryclover 2024, All rights reserved.

#pragma once

#include "BattleGameNetworkMessage.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BattleGameCtsRpc.generated.h"

class UBattleGameInstance;
/**
 * 
 */
UCLASS()
class UBattleGameCtsRpc : public UObject
{
	GENERATED_BODY()

public:
	static constexpr int CTS_REQUEST_MATCHMAKING = 1;
	static constexpr int CTS_MOVE_CHARACTER = 2;
	static constexpr int CTS_NOTIFY_BATTLEGAME_PREPARED = 3;
	static constexpr int CTS_NOTIFY_OWNING_CHARACTER_DESTROYED = 4;
	static constexpr int CTS_SET_NICKNAME = 5;
	static constexpr int CTS_REQUEST_MY_NICKNAME = 6;
	static constexpr int CTS_BATTLE_COMMAND = 7;

	UBattleGameCtsRpc() = default;
	virtual ~UBattleGameCtsRpc() = default;
	
	UFUNCTION(BlueprintCallable, Category="BattleGame|CtsRpc")
	static void RequestMatchMaking();

	UFUNCTION(BlueprintCallable, Category = "BattleGame|CtsRpc")
	static void MoveCharacter(const FVector& position, double direction);

	UFUNCTION(BlueprintCallable, Category = "BattleGame|CtsRpc")
	static void NotifyBattleGamePrepared();

	UFUNCTION(BlueprintCallable, Category = "BattleGame|CtsRpc")
	static void NotifyOwningCharacterDestroyed();

	UFUNCTION(BlueprintCallable, Category = "BattleGame|CtsRpc")
	static void SetNickname(const FString& nickname);
	
	UFUNCTION(BlueprintCallable, Category = "BattleGame|CtsRpc")
	static void RequestMyNickname();
	
	UFUNCTION(BlueprintCallable, Category = "BattleGame|CtsRpc")
	static void BattleCommand(int32 command);
};
