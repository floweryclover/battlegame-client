// Copyright floweryclover 2024, All rights reserved.


#include "BattleGameStcRpc.h"

UBattleGameStcRpc::UBattleGameStcRpc()
{
}

UBattleGameStcRpc::~UBattleGameStcRpc()
{
}

void UBattleGameStcRpc::Handle(const Message& message)
{
	UE_LOG(LogTemp, Log, TEXT("Bye"));
	switch (message.headerMessageType)
	{
	case STC_TEST:
		OnTest();
		break;
	}
}

void UBattleGameStcRpc::OnTest()
{
	UE_LOG(LogTemp, Log, TEXT("Hello"));
}