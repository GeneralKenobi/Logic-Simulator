#pragma once
#include "stdafx.h"

#pragma region Variables

// Next expected ID to assign
int NextID = 1;

// Map containing all IDs and assigned shared_ptrs to them
std::map<int, std::weak_ptr<Identifiable>> Records;

#pragma endregion

#pragma region IIDentification body

// Tries to get owner of the given ID. Returns true on success and assigns the
// found object to the owner. Otherwise returns false and resets the owner.
bool Identifiable::TryGetIDOwner(int ID, std::shared_ptr<Identifiable>& owner)
{
	// Try to find the element in the Records map
	std::map<int, std::weak_ptr<Identifiable>>::iterator it = Records.find(ID);

	// If it was found
	if (it != Records.end())
	{
		// Assign the owner
		owner = std::shared_ptr<Identifiable>((*it).second);
		return true;
	}
	else
	{
		// Otherwise reset the owner
		owner.reset();
		return false;
	}
}

// Method which should be called right after construction
// Obtains ID for this object
void Identifiable::Initialize()
{
	// If the object alredy has an ID, return
	if (mID >= 0)
	{
		return;
	}

	// Find the next free ID
	while (Records.find(NextID) != Records.end())
	{
		++NextID;
	}

	// Assign it to the object
	mID = NextID;

	// Add the object to the list of references
	Records.insert(std::make_pair(NextID, shared_from_this()));

	// And increase the expected next ID
	++NextID;
}

// Cleans up after this object. Removes it from the IDManager and
// resets its ID
void Identifiable::Dispose()
{
	if(mID >= 0)
	{
		Records.erase(mID);

		if(NextID > mID)
		{
			NextID = mID;
		}

		mID = -1;
	}
}

#pragma endregion