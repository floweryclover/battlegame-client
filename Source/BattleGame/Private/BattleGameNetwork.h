// Copyright floweryclover 2024, All rights reserved.

#pragma once

#include "BattleGameNetworkStructs.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BattleGameNetwork.generated.h"

using UINT_PTR = unsigned long long;
using SOCKET = UINT_PTR;

/**
 * 
 */
UCLASS()
class UBattleGameNetwork : public UObject
{
	GENERATED_BODY()

public:
	UBattleGameNetwork();
	virtual ~UBattleGameNetwork();
	
	UFUNCTION(BlueprintCallable)
	void Login(const FString& nickname);

private:
	SOCKET GetSocket();
	TQueue<Message>& GetSendQueue();
};
