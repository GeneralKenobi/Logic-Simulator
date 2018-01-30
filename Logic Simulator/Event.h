#pragma once
#include "stdafx.h"

// Event is a type of an object that can store pointers to functions
// and IEventSubscriber objects. Event is used (as the name suggests) to notify
// subscribers (listeners) whenever something relevant to this event happens.
// Example: MemberChangedEvent which would be called whenever a member variable
// changes its value.
// The parameters are types of data which will be passed to the listeners as
// arguments of the callback method. In the given example the type could be a std::string
// denoting the name of the member which was changed.
// Clients don't have to unsubscribe from the event - Events store weak pointers only
// and automatically remove all pointers that are expired.
template<typename... EventArgs>
class Event
{

	#pragma region Typedefs
	
	// std::function wrap typedef
	typedef std::function<void(EventArgs...)> FunctionListener;

	// weak_ptr of an IEventSubscriber
	typedef std::weak_ptr<IEventSubscriber<EventArgs...>> ObjectListener;

	#pragma endregion

protected:

	#pragma region Members

	// Container with function listeners of the event
	mutable std::map<size_t, FunctionListener> mFunctionListeners;

	// Container with object listeners of the event
	mutable std::map<ObjectListener, std::list<FunctionListener>, std::owner_less<ObjectListener>> mObjectListeners;

	#pragma endregion

public:
	
	#pragma region Subscribing	

	// Subscribes / adds a new callback for the given IEventSubscriber to this event and with a callback method.
	void Subscribe(ObjectListener newSubscriber, FunctionListener callback) const
	{
		if (mObjectListeners.find(newSubscriber) == mObjectListeners.end())
		{
			mObjectListeners.insert(std::make_pair(newSubscriber, std::list<FunctionListener>{callback}));
		}
		else
		{
			mObjectListeners.find(newSubscriber)->second.push_back(callback);
		}
	}

	// Subscribes the given function to this event
	void Subscribe(FunctionListener pointer) const
	{
		mFunctionListeners.insert(std::make_pair(GetFunctionAddress(pointer), pointer));
	}

	#pragma endregion

	#pragma region Unsubscribing

	// Unsubscribes the given IEventSubscriber from this event
	void Unsubscribe(ObjectListener subscriber) const
	{
		// If the passed weak_ptr is empty, return as there's nothing to do
		if (subscriber.expired())
		{
			return;
		}		
		
		mObjectListeners.erase(subscriber);
	}

	// Unsubscribes the given function from this event
	void Unsubscribe(FunctionListener pointer) const
	{
		mFunctionListeners.erase(GetFunctionAddress(pointer));
	}	

	#pragma endregion

	#pragma region () operator

	// Fires the event
	const void operator()(EventArgs... args) const
	{
		auto it = mObjectListeners.begin();
		
		// Keep going until the end of the list
		while(it!=mObjectListeners.end())
		{
			// If the pointer is expired
			if (it->first.expired())
			{
				// Remove it from the list
				mObjectListeners.erase(it++);
			}
			else
			{
				// Invoke all callback methods
				for (auto membersIt = it->second.begin(); membersIt != it->second.end(); ++membersIt)
				{
					(*membersIt)(args...);
				}

				++it;
			}
		}

		for (std::map<size_t, FunctionListener>::const_iterator it =
			mFunctionListeners.cbegin();
			it != mFunctionListeners.cend(); ++it)
		{
			it->second(args...);
		}
	}

	#pragma endregion

	#pragma region Constructors/Destructors

	virtual ~Event() {}

	#pragma endregion

};