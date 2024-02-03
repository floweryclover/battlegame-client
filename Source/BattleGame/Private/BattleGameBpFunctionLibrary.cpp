// Copyright floweryclover 2024, All rights reserved.


#include "BattleGameBpFunctionLibrary.h"
#include "BattleGameNetworkManager.h"
#include "BattleGameCtsRpc.h"
#include "GameFramework/GameModeBase.h"
#include <WinSock2.h>

void UBattleGameBpFunctionLibrary::InitializeBattleGameNetworkManager(const FString& serverAddress, int32 serverPort)
{
	BattleGameNetworkManager::Initialize(serverAddress, serverPort);
}

bool UBattleGameBpFunctionLibrary::Connect(int32& errorCode)
{
	return BattleGameNetworkManager::GetInstance().ConnectToServer(errorCode);
}


FText UBattleGameBpFunctionLibrary::InterpretWsaErrorCode(int32 wsaErrorCode)
{
	FString reason;
	switch (wsaErrorCode)
	{
	case WSAEWOULDBLOCK:
		reason = FString(TEXT("IO 작업이 Would Block 상태입니다."));
		break;
	case WSAECONNABORTED:
	case WSAECONNRESET:
		reason = FString(TEXT("서버와의 연결이 끊어졌습니다."));
		break;
	case WSAECONNREFUSED:
		reason = FString(TEXT("서버에 연결할 수 없습니다."));
		break;
	default:
		reason = FString(TEXT("알 수 없는 에러입니다: "));
		reason.Append(FString::FromInt(wsaErrorCode));
		break;
	}

	return FText::FromString(reason);
}

UBattleGameCtsRpc* UBattleGameBpFunctionLibrary::GetCtsRpcInstance()
{
	return BattleGameNetworkManager::GetInstance().GetBattleGameCtsRpc();
}

void UBattleGameBpFunctionLibrary::ManualTick(AGameModeBase* gameModeContext)
{
	BattleGameNetworkManager::GetInstance().ManualTick(gameModeContext);
}