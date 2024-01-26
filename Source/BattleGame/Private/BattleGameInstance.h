// Copyright floweryclover 2024, All rights reserved.

#pragma once

#include <Winsock2.h>
#include <WS2tcpip.h>

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BattleGameInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogBattleGameNetwork, Log, All);

struct Message
{
	int bodySize; // header
	int messageType; // header
	char* body; // body
};

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

protected:

public:
	UBattleGameInstance();
	virtual ~UBattleGameInstance();
	UFUNCTION(BlueprintCallable)
	bool ConnectToServer(const FString& serverAddress, const int32 serverPort, int32& errorCode);

	UFUNCTION(BlueprintCallable)
	bool ProcessNetworkTasks();
};
