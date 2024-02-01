// Copyright floweryclover 2024, All rights reserved.

#pragma once

#include "BattleGameNetworkStructs.h"

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

	void Init(UBattleGameInstance* _battleGameInstance);
	UBattleGameCtsRpc();
	virtual ~UBattleGameCtsRpc();
	
	UFUNCTION(BlueprintCallable)
	void RequestMatchMaking();

private:
	UBattleGameInstance* battleGameInstance;
};
