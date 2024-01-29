// Copyright floweryclover 2024, All rights reserved.

#pragma once

#include "BattleGameNetworkStructs.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BattleGameStcRpc.generated.h"

/**
 *
 */
UCLASS()
class UBattleGameStcRpc : public UObject
{
	GENERATED_BODY()

public:
	static constexpr int STC_TEST = 1;
	UBattleGameStcRpc();
	virtual ~UBattleGameStcRpc();

	void Handle(const Message& message);
	void OnTest();
};
