// Copyright floweryclover 2024, All rights reserved.

#pragma once
/**
 * 
 */
struct Message
{
	int bodySize; // header
	int messageType; // header
	char* body; // body
};
