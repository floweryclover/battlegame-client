// Copyright floweryclover 2024, All rights reserved.

#pragma once

#include "BattleGameNetworkStructs.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BattleGameCtsRpc.generated.h"

/**
 * 
 */
UCLASS()
class UBattleGameCtsRpc : public UObject
{
	GENERATED_BODY()

public:
	void Init(TQueue<Message>* _pSendQueue);
	UBattleGameCtsRpc();
	virtual ~UBattleGameCtsRpc();
	
	UFUNCTION(BlueprintCallable)
	void Login(const FString& nickname);

private:
	TQueue<Message>* pSendQueue;
};
