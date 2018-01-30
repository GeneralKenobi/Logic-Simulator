#pragma once
#include "stdafx.h"


// Struct holding information about menu arrow's position. Prevents incorrect positions from being assigned
struct ArrowPosition
{
private:
	
	// Current position
	int mPosition = 0;

	// First impossible position
	int mLastPosition = 0;	

public:

	// Returns the current position
	int Get() { return mPosition; }

	// Sets the current position (if it's correct) and if it did set fires PositionChangedEvent
	void Set(int newPosition);

	// Sets new first impossible position. If the value is smaller than 0 throws an exception
	void SetLast(int lastPosition);

	// Event fired whenever position changes. First argument is the old position, second is the new position
	const Event<int, int> PositionChangedEvent;

	// If possible, incremenmts current position and fires PositionChangedEvent
	ArrowPosition& operator++();

	// If possible, decrements current position and fires PositionChangedEvent
	ArrowPosition& operator--();
};