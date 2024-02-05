// Copyright floweryclover 2024, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BattleGameNetworkMessage.h"
#include "BattleGameNetworkEnums.h"

using UINT_PTR = unsigned long long;
using SOCKET = UINT_PTR;

class UBattleGameCtsRpc;
class AGameModeBase;

/**
 * 
 */
class BattleGameNetworkManager
{
public:
	static void Initialize(const FString& serverAddress, int32 serverPort);
	static inline BattleGameNetworkManager& GetInstance() { check(spSingleton != nullptr); return *spSingleton; }
	explicit BattleGameNetworkManager(const FString& serverAddress, int32 serverPort) noexcept;
	~BattleGameNetworkManager();

	UFUNCTION(BlueprintCallable)
	bool ConnectToServer(int32& errorCode);

	UFUNCTION(BlueprintCallable)
	void ManualTick(AGameModeBase* gameModeContext);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	inline UBattleGameCtsRpc* GetBattleGameCtsRpc() { return this->mpCtsRpcInstance; }

	void RequestSendMessage(EBattleGameSendReliability reliability, Message&& message);

	AGameModeBase* GetLastGameModeContext();
private:
	enum IoResult
	{
		WOULD_BLOCK,
		DISCONNECTED,
		SUCCESSFUL
	};

	static constexpr int MAX_MESSAGE_SIZE = 1024;
	static constexpr int MESSAGE_HEADER_SIZE = 8;
	static TUniquePtr<BattleGameNetworkManager> spSingleton;

	void CleanupSocket();
	void SendTcp();
	void ReceiveTcp();
	void SendUdp(Message&& message);
	void ReceiveUdp();
	IoResult HandleResult(int result, int& outErrorCode);

	void OnDisconnected(int32 reason);

	const FString mServerAddress;
	const int32 mServerPort;
	struct sockaddr_in* mServerAddrIn;
	int mServerAddrLen;
	SOCKET mTcpSocket;
	SOCKET mUdpSocket;
	TQueue<Message> mTcpSendQueue;
	int mCurrentSent;
	int mTotalSizeToReceive;
	int mCurrentReceived;
	char mReceiveBuffer[MAX_MESSAGE_SIZE];
	bool mIsReceivingHeader;
	int mLastReceivedHeaderType;

	AGameModeBase* mpLastGameModeContext;

	class UBattleGameCtsRpc* mpCtsRpcInstance;
	class UBattleGameStcRpc* mpStcRpcInstance;

};
