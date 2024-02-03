// Copyright floweryclover 2024, All rights reserved.

#pragma once

#include "BattleGameNetworkStructs.h"
#include <functional>

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BattleGameStcRpc.generated.h"


class UBattleGameInstance;
/**
 *
 */
UCLASS()
class UBattleGameStcRpc : public UObject
{
	GENERATED_BODY()

public:
	static constexpr int STC_JOINED_GAME_ROOM = 1;
	static constexpr int STC_DISCONNECTED_FROM_GAME = 2;

	UBattleGameStcRpc() = default;
	virtual ~UBattleGameStcRpc() = default;

	void Handle(const Message& message);
	
	void OnJoinedGameRoom();
	void OnDisconnectedFromGame();
};
