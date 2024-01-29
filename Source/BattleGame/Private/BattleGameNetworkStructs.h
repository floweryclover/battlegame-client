// Copyright floweryclover 2024, All rights reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
struct Message
{
	int headerBodySize;
	int headerMessageType;
	TUniquePtr<char> bodyBuffer;
};
