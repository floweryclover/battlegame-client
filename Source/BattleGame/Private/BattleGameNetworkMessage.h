// Copyright floweryclover 2024, All rights reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
struct Message
{
	Message();
	Message(int headerBodySize, int headerMessageType, const char* pBodyOnlySource);
	int mHeaderBodySize;
	int mHeaderMessageType;
	TUniquePtr<char> mpBodyBuffer;
};
