#pragma once
#include "stdafx.h"


#pragma region Methods

// Delays the update of this Node
void Node::Delay()
{
	mReadyToUpdate = false;
	Sleep(10);
	mReadyToUpdate = true;
	Value(mLastCaughtValue);
}

#pragma endregion


#pragma region Accessors

// Setter of the value assigned to this node
void Node::Value(bool value)
{
	// If we're ready to update
	if (mReadyToUpdate)
	{
		if (mValue != value)
		{
			// Assign the value to mValue and as the last caught value
			mValue = value;
			mLastCaughtValue = value;

			// Begin delay
			std::thread delayThread(&Node::Delay, this);
			delayThread.detach();

			// Fire Events
			InternalStateChanged();
		}
	}
	else
	{
		// If we're not, update the lastCaughtValue
		mLastCaughtValue = value;
	}
}

#pragma endregion