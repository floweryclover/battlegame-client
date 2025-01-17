﻿// Copyright floweryclover 2024, All rights reserved.


#include "BattleGameStcRpc.h"
#include "BattleGameCtsRpc.h"
#include "BattleGameNetworkManager.h"
#include "BattleGameNetworkMessage.h"
#include "BattleGameMode.h"
#include "BattleInterface.h"
#include "LogBattleGameNetwork.h"
#include "GameFramework/GameModeBase.h"
#include <string>

void UBattleGameStcRpc::Handle(const Message& message)
{
	auto pBuffer = message.mpBodyBuffer.Get();
	switch (message.mHeaderMessageType)
	{
	case STC_OPEN_LEVEL:
		int32 level;
		memcpy(&level, message.mpBodyBuffer.Get(), 4);
		OnOpenLevel(level);
		break;
	case STC_DISCONNECTED_FROM_GAME:
		OnDisconnectedFromGame();
		break;
	case STC_SPAWN_ENTITY:
	{
		int32 entityId;
		FVector location;
		double direction;
		memcpy(&entityId, pBuffer, 4);
		memcpy(&location.X, pBuffer + 4, 8);
		memcpy(&location.Y, pBuffer + 12, 8);
		memcpy(&location.Z, pBuffer + 20, 8);
		memcpy(&direction, pBuffer + 28, 8);
		OnSpawnEntity(entityId, location, direction);
		break;
	}
	case STC_DESPAWN_ENTITY:
	{
		int32 entityId;
		memcpy(&entityId, pBuffer, 4);
		OnDespawnEntity(entityId);
		break;
	}
	case STC_POSSESS_ENTITY:
	{
		int32 entityId;
		memcpy(&entityId, pBuffer, 4);
		OnPossessEntity(entityId);
		break;
	}
	case STC_MOVE_ENTITY:
	{
		int32 entityId;
		FVector location;
		double direction;
		memcpy(&entityId, pBuffer, 4);
		memcpy(&location.X, pBuffer + 4, 8);
		memcpy(&location.Y, pBuffer + 12, 8);
		memcpy(&location.Z, pBuffer + 20, 8);
		memcpy(&direction, pBuffer + 28, 8);
		OnMoveEntity(entityId, location, direction);
		break;
	}
	case STC_SET_TIMER:
	{
		unsigned short seconds;
		char text[30];
		memset(text, 0, 30);
		memcpy(&seconds, pBuffer, 2);
		memcpy(text, pBuffer +2, message.mHeaderBodySize-2);
		OnSetTimer(seconds, FText::FromString(UTF8_TO_TCHAR(text)));
		break;
	}
	case STC_SIGNAL_GAME_STATE:
	{
		int32 signal;
		memcpy(&signal, pBuffer, 4);
		OnSignalGameState(signal);
		break;
	}
	case STC_RESPAWN_ENTITY:
	{
		int32 entityId;
		FVector location;
		double direction;
		memcpy(&entityId, pBuffer, 4);
		memcpy(&location.X, pBuffer + 4, 8);
		memcpy(&location.Y, pBuffer + 12, 8);
		memcpy(&location.Z, pBuffer + 20, 8);
		memcpy(&direction, pBuffer + 28, 8);
		OnRespawnEntity(entityId, location, direction);
		break;
	}
	case STC_SET_SCORE:
	{
		int32 team;
		int32 score;
		memcpy(&team, pBuffer, 4);
		memcpy(&score, pBuffer +4, 4);
		OnSetScore(team, score);
		break;
	}
	case STC_ASSIGN_TEAM_ID:
	{
		int32 teamId;
		memcpy(&teamId, pBuffer, 4);
		OnAssignTeamId(teamId);
		break;
	}
	case STC_ASSIGN_ENTITY_NICKNAME:
	{
		int32 entityId;
		char nicknameChars[24] = {0,};
		memcpy(&entityId, pBuffer, 4);
		memcpy(nicknameChars, pBuffer + 4, message.mHeaderBodySize - 4);
		OnAssignEntityNickname(entityId, FText::FromString(UTF8_TO_TCHAR(nicknameChars)));
		break;
	}
	case STC_GET_MY_NICKNAME:
	{		
		char nicknameChars[24] = { 0, };
		memcpy(nicknameChars, pBuffer, message.mHeaderBodySize);
		OnGetMyNickname(FText::FromString(UTF8_TO_TCHAR(nicknameChars)));

		break;
	}
	case STC_SEND_GAME_DATA:
	{
		int myNicknameLength, opponentNicknameLength;
		char myNickname[25];
		char opponentNickname[25];
		memcpy(&myNicknameLength, pBuffer, 4);
		memcpy(myNickname, pBuffer + 4, myNicknameLength);
		memcpy(&opponentNicknameLength, pBuffer + 4 + myNicknameLength, 4);
		memcpy(opponentNickname, pBuffer + 4 + myNicknameLength + 4, opponentNicknameLength);
		OnSendGameData(FString(UTF8_TO_TCHAR(myNickname)), FString(UTF8_TO_TCHAR(opponentNickname)));
		break;
	}
	case STC_SEND_GAME_RESULT:
	{
		bool isGoodGame, isWinner;
		int32 myScore, opponentScore;
		memcpy(&isGoodGame, pBuffer, 1);
		memcpy(&isWinner, pBuffer +1, 1);
		memcpy(&myScore, pBuffer +2, 4);
		memcpy(&opponentScore, pBuffer +6, 4);
		OnSendGameResult(isGoodGame, isWinner, myScore, opponentScore);
		break;
	}
	case STC_KNOCKBACK_ENTITY:
	{
		int32 entityId;
		FVector location, impulse;
		memcpy(&entityId, pBuffer, 4);
		memcpy(&location.X, pBuffer+4, 8);
		memcpy(&location.Y, pBuffer+12, 8);
		memcpy(&location.Z, pBuffer+20, 8);		
		memcpy(&impulse.X, pBuffer+28, 8);
		memcpy(&impulse.Y, pBuffer+36, 8);
		impulse.Z = 0;
		OnKnockbackEntity(entityId, location, impulse);
		break;
	}
	default:
		UE_LOG(LogBattleGameNetwork, Error, TEXT("알 수 없는 메시지 수신: %d"), message.mHeaderMessageType);	}
}

void UBattleGameStcRpc::OnOpenLevel(int32 level)
{
	FString levelUrl;
	if (level == Level::MAINMENU)
	{levelUrl = "/Game/Levels/MainMenu/MainMenuLevel";}
	else if (level == Level::ONE_VS_ONE)
	{levelUrl = "/Game/Levels/Battle/OneVsOneLevel";}
	
	check(!levelUrl.IsEmpty());
	BattleGameNetworkManager::GetInstance()
		.GetGameModeContext()
		->GetWorld()
		->ServerTravel(levelUrl);
}

void UBattleGameStcRpc::OnDisconnectedFromGame()
{
	// 게임에서 나가졌습니다..!
}

void UBattleGameStcRpc::OnSpawnEntity(int32 entityId, const FVector& location, double direction)
{
	auto pBattleGameMode = Cast<ABattleGameMode>(BattleGameNetworkManager::GetInstance().GetGameModeContext());
	if (!IsValid(pBattleGameMode))
	{
		UE_LOG(LogBattleGameNetwork, Error, TEXT("BattleGameMode가 준비되지 않았습니다."));
		return;
	}

	pBattleGameMode->OnSpawnEntity(entityId, location, direction);
}

void UBattleGameStcRpc::OnDespawnEntity(int32 entityId)
{
	auto pBattleGameMode = Cast<ABattleGameMode>(BattleGameNetworkManager::GetInstance().GetGameModeContext());
	if (!IsValid(pBattleGameMode))
	{
		UE_LOG(LogBattleGameNetwork, Error, TEXT("BattleGameMode가 준비되지 않았습니다."));
		return;
	}

	pBattleGameMode->OnDespawnEntity(entityId);
}

void UBattleGameStcRpc::OnPossessEntity(int32 entityId)
{
	auto pBattleGameMode = Cast<ABattleGameMode>(BattleGameNetworkManager::GetInstance().GetGameModeContext());
	if (!IsValid(pBattleGameMode))
	{
		UE_LOG(LogBattleGameNetwork, Error, TEXT("BattleGameMode가 준비되지 않았습니다."));
		return;
	}

	pBattleGameMode->OnPossessEntity(entityId);
}

void UBattleGameStcRpc::OnMoveEntity(int32 entityId, const FVector& location, double direction)
{
	auto pBattleGameMode = Cast<ABattleGameMode>(BattleGameNetworkManager::GetInstance().GetGameModeContext());
	if (!IsValid(pBattleGameMode))
	{
		UE_LOG(LogBattleGameNetwork, Error, TEXT("BattleGameMode가 준비되지 않았습니다."));
		return;
	}

	pBattleGameMode->OnMoveEntity(entityId, location, direction);
}

void UBattleGameStcRpc::OnSetTimer(int32 seconds, const FText& text)
{
	IHasTimer::Execute_OnSetTimer(BattleGameNetworkManager::GetInstance().GetGameModeContext(), seconds, text);
}

void UBattleGameStcRpc::OnSignalGameState(int32 signal)
{
	auto pBattleGameMode = Cast<ABattleGameMode>(BattleGameNetworkManager::GetInstance().GetGameModeContext());
	if (!IsValid(pBattleGameMode))
	{
		UE_LOG(LogBattleGameNetwork, Error, TEXT("BattleGameMode가 준비되지 않았습니다."));
		return;
	}

	pBattleGameMode->OnSignalGameState(signal);
}

void UBattleGameStcRpc::OnRespawnEntity(int32 entityId, const FVector& location, double direction)
{
	auto pBattleGameMode = Cast<ABattleGameMode>(BattleGameNetworkManager::GetInstance().GetGameModeContext());
	if (!IsValid(pBattleGameMode))
	{
		UE_LOG(LogBattleGameNetwork, Error, TEXT("BattleGameMode가 준비되지 않았습니다."));
		return;
	}

	pBattleGameMode->OnRespawnEntity(entityId, location, direction);
}

void UBattleGameStcRpc::OnSetScore(int32 team, int32 score)
{
	auto pBattleGameMode = Cast<ABattleGameMode>(BattleGameNetworkManager::GetInstance().GetGameModeContext());
	if (!IsValid(pBattleGameMode))
	{
		UE_LOG(LogBattleGameNetwork, Error, TEXT("BattleGameMode가 준비되지 않았습니다."));
		return;
	}

	pBattleGameMode->OnSetScore(team, score);
}

void UBattleGameStcRpc::OnAssignTeamId(int32 teamId)
{
	auto pBattleGameMode = Cast<ABattleGameMode>(BattleGameNetworkManager::GetInstance().GetGameModeContext());
	if (!IsValid(pBattleGameMode))
	{
		UE_LOG(LogBattleGameNetwork, Error, TEXT("BattleGameMode가 준비되지 않았습니다."));
		return;
	}

	pBattleGameMode->OnAssignTeamId(teamId);
}

void UBattleGameStcRpc::OnAssignEntityNickname(int32 entityId, const FText& nickname)
{
	auto pBattleGameMode = Cast<ABattleGameMode>(BattleGameNetworkManager::GetInstance().GetGameModeContext());
	if (!IsValid(pBattleGameMode))
	{
		UE_LOG(LogBattleGameNetwork, Error, TEXT("BattleGameMode가 준비되지 않았습니다."));
		return;
	}

	pBattleGameMode->OnAssignEntityNickname(entityId, nickname);
}

void UBattleGameStcRpc::OnGetMyNickname(const FText& nickname)
{
	IReceivesMyNickname::Execute_OnGetMyNickname(BattleGameNetworkManager::GetInstance().GetGameModeContext(), nickname);
}

void UBattleGameStcRpc::OnSendGameData(const FString& myNickname, const FString& opponentNickname)
{
	auto pBattleGameMode = Cast<ABattleGameMode>(BattleGameNetworkManager::GetInstance().GetGameModeContext());
	if (!IsValid(pBattleGameMode))
	{
		UE_LOG(LogBattleGameNetwork, Error, TEXT("BattleGameMode가 준비되지 않았습니다."));
		return;
	}

	pBattleGameMode->OnSendGameData(myNickname, opponentNickname);
}

void UBattleGameStcRpc::OnSendGameResult(bool isGoodGame, bool isWinner, int32 myScore, int32 opponentScore)
{
	auto pBattleGameMode = Cast<ABattleGameMode>(BattleGameNetworkManager::GetInstance().GetGameModeContext());
	if (!IsValid(pBattleGameMode))
	{
		UE_LOG(LogBattleGameNetwork, Error, TEXT("BattleGameMode가 준비되지 않았습니다."));
		return;
	}

	pBattleGameMode->OnSendGameResult(isGoodGame, isWinner, myScore, opponentScore);
}

void UBattleGameStcRpc::OnKnockbackEntity(int32 entityId, const FVector& location, const FVector& impulse)
{
	auto pBattleGameMode = Cast<ABattleGameMode>(BattleGameNetworkManager::GetInstance().GetGameModeContext());
	if (!IsValid(pBattleGameMode))
	{
		UE_LOG(LogBattleGameNetwork, Error, TEXT("BattleGameMode가 준비되지 않았습니다."));
		return;
	}

	pBattleGameMode->OnKnockbackEntity(entityId, location, impulse);
}