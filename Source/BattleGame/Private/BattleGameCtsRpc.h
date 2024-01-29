// Copyright floweryclover 2024, All rights reserved.

#pragma once

#include "BattleGameNetworkStructs.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BattleGameCtsRpc.generated.h"

using UINT_PTR = unsigned long long;
using SOCKET = UINT_PTR;
class UBattleGameInstance;
/**
 * 
 */
UCLASS()
class UBattleGameCtsRpc : public UObject
{
	GENERATED_BODY()

public:
	void Init(UBattleGameInstance* pInstance);
	UBattleGameCtsRpc();
	virtual ~UBattleGameCtsRpc();
	
	UFUNCTION(BlueprintCallable)
	void Login(const FString& nickname);

private:
	UBattleGameInstance* pGameInstance;
	SOCKET GetSocket();
	TQueue<Message>& GetSendQueue();
};
