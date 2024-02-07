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
	static constexpr int CTS_ACK_UDP_TOKEN = 3;
	static constexpr int CTS_NOTIFY_BATTLEGAME_PREPARED = 4;

	UBattleGameCtsRpc() = default;
	virtual ~UBattleGameCtsRpc() = default;
	
	UFUNCTION(BlueprintCallable)
	void RequestMatchMaking();

	UFUNCTION(BlueprintCallable)
	void MoveCharacter(const FVector& position);

	void AckUdpToken(unsigned long long token);

	UFUNCTION(BlueprintCallable)
	void NotifyBattleGamePrepared();
};
