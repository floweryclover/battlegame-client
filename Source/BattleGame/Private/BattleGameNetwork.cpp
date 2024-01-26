// Copyright floweryclover 2024, All rights reserved.


#include "BattleGameNetwork.h"
#include "BattleGameInstance.h"
#include <WinSock2.h>
#include <WS2tcpip.h>

SOCKET UBattleGameNetwork::GetSocket() 
{
	return Cast<UBattleGameInstance>(GetWorld()->GetGameInstance())->GetSocket(); 
}

TQueue<Message>& UBattleGameNetwork::GetSendQueue()
{
	return Cast<UBattleGameInstance>(GetWorld()->GetGameInstance())->GetSendQueue();
}

UBattleGameNetwork::UBattleGameNetwork()
{
	
}

UBattleGameNetwork::~UBattleGameNetwork()
{
}

void UBattleGameNetwork::Login(const FString& nickname)
{

}
