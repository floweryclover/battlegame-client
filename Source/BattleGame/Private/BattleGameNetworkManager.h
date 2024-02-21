// Copyright floweryclover 2024, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BattleGameNetworkMessage.h"
#include "BattleGameNetworkEnums.h"

using UINT_PTR = unsigned long long;
using SOCKET = UINT_PTR;

class UBattleGameCtsRpc;
class UBattleGameStcRpc;
class AGameModeBase;

class FNetworkRunnable : public FRunnable
{
public:
	FNetworkRunnable(SOCKET tcpSocket, SOCKET udpSocket, FString serverAddress, int32 serverPort);
	virtual ~FNetworkRunnable();

	void EnqueueMessage(EBattleGameSendReliability reliability, Message&& message);

private:
	enum IoResult
	{
		WOULD_BLOCK,
		DISCONNECTED,
		SUCCESSFUL
	};

	static constexpr int MAX_MESSAGE_SIZE = 1024;
	static constexpr int MESSAGE_HEADER_SIZE = 8;

	virtual bool Init() override; // 메인 스레드에서 호출
	virtual uint32 Run() override; // 새 스레드에서 호출
	virtual void Exit() override; // 새 스레드에서 호출
	virtual void Stop() override; // 메인 스레드에서 호출

	bool mStopping;
	FRunnableThread* mThread;

	TUniquePtr<struct sockaddr_in> mpServerAddrIn;

	SOCKET mTcpSocket;
	SOCKET mUdpSocket;
	TQueue<Message, EQueueMode::Mpsc> mTcpSendQueue;
	TQueue<Message, EQueueMode::Mpsc> mUdpSendQueue;
	int mCurrentSent;
	int mTotalSizeToReceive;
	int mCurrentReceived;
	char mReceiveBuffer[MAX_MESSAGE_SIZE];
	bool mIsReceivingHeader;
	int mLastReceivedHeaderType;

	void SendTcp();
	void ReceiveTcp();
	void SendUdp();
	void ReceiveUdp();
	IoResult HandleResult(int result, int& outErrorCode);
};

/**
 * 
 */
class BattleGameNetworkManager
{
public:
	static BattleGameNetworkManager& GetInstance();
	explicit BattleGameNetworkManager() noexcept;
	~BattleGameNetworkManager();

	bool Connect(const FString& serverAddress, int32 serverPort, int32& errorCode);
	AGameModeBase* GetGameModeContext();
	inline void SetGameModeContext(AGameModeBase* pGameModeContext) { mpGameModeContext = pGameModeContext;  };
	void OnDisconnected(int32 reason);

	inline UBattleGameStcRpc* GetBattleGameStcRpc() { return this->mpStcRpcInstance; }

	void EnqueueMessage(EBattleGameSendReliability reliability, Message&& message);

private:
	static TUniquePtr<BattleGameNetworkManager> spSingleton;

	TUniquePtr<FNetworkRunnable> mpNetworkRunnable;
	AGameModeBase* mpGameModeContext;

	UBattleGameStcRpc* mpStcRpcInstance;
};
