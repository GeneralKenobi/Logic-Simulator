#pragma once
#include "stdafx.h"

// Base class for classes that want to subscribe to events.
// Unsubscribing from events isn't necessary (Events don't store strong references
// to the objects and automatically remove all pointers that are expired).
// It's necessary to inherit from IEventSubscriber so that Event can store weak_ptrs to objects that
// subscribe with their method(s) so that when the object was deleted but it didn't unsubscribe from the event
// calling callback methods doesn't result in undefined behaviour but instead expired weak_ptr subscribers can be removed.
template<typename... EventArgs>
class IEventSubscriber
{

public:	

	#pragma region Constructor/Destructor

	// Default Destructor
	virtual ~IEventSubscriber() { }

	#pragma endregion

};