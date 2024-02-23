// Copyright floweryclover 2024, All rights reserved.


#include "BattleGameCtsRpc.h"
#include "LogBattleGameNetwork.h"
#include "BattleGameNetworkManager.h"
#include "BattleGameNetworkEnums.h"
#include "BattleGameBpFunctionLibrary.h"
#include "BattleGameMode.h"
#include "Kismet/GameplayStatics.h"
#include <cstring>

void UBattleGameCtsRpc::RequestMatchMaking()
{
	Message message(0, CTS_REQUEST_MATCHMAKING, nullptr);
	BattleGameNetworkManager::GetInstance()
		.EnqueueMessage(EBattleGameSendReliability::RELIABLE, std::move(message));
}

void UBattleGameCtsRpc::MoveCharacter(const FVector& position, double direction)
{
	char body[32];
	memcpy(body, &position.X, 8);
	memcpy(body+8, &position.Y, 8);
	memcpy(body+16, &position.Z, 8);
	memcpy(body + 24, &direction, 8);

	Message message(32, CTS_MOVE_CHARACTER, body);
	BattleGameNetworkManager::GetInstance()
		.EnqueueMessage(EBattleGameSendReliability::UNRELIABLE, std::move(message));
}

void UBattleGameCtsRpc::NotifyBattleGamePrepared()
{
	int count = 0;
	auto pBattleGameMode = Cast<ABattleGameMode>(BattleGameNetworkManager::GetInstance().GetGameModeContext());
	check(pBattleGameMode != nullptr);
	
	Message message(0, CTS_NOTIFY_BATTLEGAME_PREPARED, nullptr);
	BattleGameNetworkManager::GetInstance()
		.EnqueueMessage(EBattleGameSendReliability::RELIABLE, std::move(message));
}

void UBattleGameCtsRpc::NotifyOwningCharacterDestroyed()
{
	Message message(0, CTS_NOTIFY_OWNING_CHARACTER_DESTROYED, nullptr);
	BattleGameNetworkManager::GetInstance()
		.EnqueueMessage(EBattleGameSendReliability::RELIABLE, std::move(message));
}

void UBattleGameCtsRpc::SetNickname(const FString& nickname)
{
	int sizeInBytes = UBattleGameBpFunctionLibrary::GetStringSizeInBytes(nickname);
	if (sizeInBytes > 24)
	{return;}

	const char* utf8Chars = TCHAR_TO_UTF8(*nickname);
	Message message(sizeInBytes, CTS_SET_NICKNAME, utf8Chars);
	BattleGameNetworkManager::GetInstance()
		.EnqueueMessage(EBattleGameSendReliability::RELIABLE, std::move(message));
}

void UBattleGameCtsRpc::RequestMyNickname()
{
	Message message(0, CTS_REQUEST_MY_NICKNAME, nullptr);
	BattleGameNetworkManager::GetInstance()
		.EnqueueMessage(EBattleGameSendReliability::RELIABLE, std::move(message));
}

void UBattleGameCtsRpc::BattleCommand(int32 command)
{
	Message message(4, CTS_BATTLE_COMMAND, reinterpret_cast<char*>(&command));
	BattleGameNetworkManager::GetInstance()
		.EnqueueMessage(EBattleGameSendReliability::RELIABLE, std::move(message));
}