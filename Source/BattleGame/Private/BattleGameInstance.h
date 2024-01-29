// Copyright floweryclover 2024, All rights reserved.

#pragma once

#include "BattleGameCtsRpc.h"
#include "BattleGameNetworkStructs.h"

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BattleGameInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogBattleGameCtsRpc, Log, All);

using UINT_PTR = unsigned long long;
using SOCKET = UINT_PTR;

/**
 * 
 */
UCLASS()
class UBattleGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	virtual void Init() override;
	virtual void Shutdown() override;

	void CleanupSocket();
	
	SOCKET clientSocket;
	
	TQueue<Message> sendQueue;
	int currentSent;
	static constexpr int MAX_MESSAGE_SIZE = 1024;
	static constexpr int MESSAGE_HEADER_SIZE = 8;

	int totalSizeToReceive;
	int currentReceived;
	char receiveBuffer[MAX_MESSAGE_SIZE];

	int lastReceivedHeaderType;

	class UBattleGameCtsRpc* pCtsRpcInstance;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnDisconnectedEvent(const int32 reason);

public:
	UBattleGameInstance();
	virtual ~UBattleGameInstance();

	UFUNCTION(BlueprintCallable)
	bool ConnectToServer(const FString& serverAddress, const int32 serverPort, int32& errorCode);

	UFUNCTION(BlueprintCallable)
	bool ProcessNetworkTasks();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FText InterpretWsaErrorCode(const int32 wsaErrorCode) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	inline UBattleGameCtsRpc* GetBattleGameCtsRpc() { return this->pCtsRpcInstance; }

	inline SOCKET GetSocket() { return this->clientSocket; }
	inline TQueue<Message>& GetSendQueue() { return this->sendQueue; }
};
