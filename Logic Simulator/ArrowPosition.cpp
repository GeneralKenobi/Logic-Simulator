#include "stdafx.h"
#include "ArrowPosition.h"



// Sets the current position (if it's correct) and if it did set fires PositionChangedEvent
void ArrowPosition::Set(int newPosition)
{
	if (newPosition != mPosition && newPosition >= 0 && newPosition < mLastPosition)
	{
		int oldPosition = mPosition;
		mPosition = newPosition;
		PositionChangedEvent(oldPosition, mPosition);
	}
}


// Sets new first impossible position. If the value is smaller than 0 throws an exception
void ArrowPosition::SetLast(int lastPosition)
{
	if (lastPosition < 0)
	{
		throw new Exception("Last position can't be negative");
	}

	mLastPosition = lastPosition;

	if (mPosition > lastPosition)
	{
		int oldPosition = mPosition;
		mPosition = mLastPosition - 1;
		PositionChangedEvent(oldPosition, mPosition);
	}

}


// If possible, incremenmts current position and fires PositionChangedEvent
ArrowPosition & ArrowPosition::operator++()
{
	Set(mPosition + 1);
	return *this;
}


// If possible, decrements current position and fires PositionChangedEvent
ArrowPosition & ArrowPosition::operator--()
{
	Set(mPosition - 1);
	return *this;
}
