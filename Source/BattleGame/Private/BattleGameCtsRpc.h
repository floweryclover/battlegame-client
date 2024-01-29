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
	static constexpr int CTS_REQUEST_JOIN_GAME = 1;

	void Init(TQueue<Message>* _pSendQueue);
	UBattleGameCtsRpc();
	virtual ~UBattleGameCtsRpc();
	
	UFUNCTION(BlueprintCallable)
	void RequestJoinGame();

private:
	TQueue<Message>* pSendQueue;
};
