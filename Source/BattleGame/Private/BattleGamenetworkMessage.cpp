// Copyright floweryclover 2024, All rights reserved.

#pragma once

#include "BattleGameNetworkMessage.h"

Message::Message() : mHeaderBodySize(0), mHeaderMessageType(0), mpBodyBuffer(nullptr) {}

Message::Message(int headerBodySize, int headerMessageType, const char* pBodyOnlySource)
	: mHeaderBodySize(headerBodySize),
	mHeaderMessageType(headerMessageType)
{
	char* pBodyBuffer = new char[headerBodySize];
	memcpy(pBodyBuffer, pBodyOnlySource, headerBodySize);
	mpBodyBuffer = TUniquePtr<char>(pBodyBuffer);
}
