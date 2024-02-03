// Copyright floweryclover 2024, All rights reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EBattleGameSendReliability : uint8
{
	RELIABLE UMETA(DisplayName = "½Å·Ú¼º"),
	UNRELIABLE UMETA(DisplayName = "ºñ½Å·Ú¼º"),
};