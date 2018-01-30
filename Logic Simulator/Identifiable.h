#pragma once
#include "stdafx.h"

// Class which is repsonsible for assigning and managing IDs on
// types that derive from it
class Identifiable : public virtual std::enable_shared_from_this<Identifiable>
{

private:

	#pragma region Members

	// Field to store ID
	int mID = -1;

	#pragma endregion

protected:

	#pragma region Constructor
	
	// Default Constructor
	Identifiable() { }

	#pragma endregion

public:

	#pragma region Lifetime

	// Obtains ID for the object
	void Initialize();

	// Removes this object from the IDManager and
	// resets its ID (if it had an ID and was registered in the first place)
	void Dispose();

	#pragma endregion

	#pragma region Constructors/Destructors

	// Default destructor, calls Dispose()
	virtual ~Identifiable() { Dispose(); }

	#pragma endregion

	#pragma region Accessors

	// ID of this object
	const int ID () const {return mID;}

	#pragma endregion

	#pragma region Static Methods

	// Creates a new shared_ptr of T with already assigned ID
	// and returns it. If T does not derive from Identifiable, throws an exception.
	// CtorArgs - arguments to pass to the object's constructor
	template<typename T, typename... CtorArgs>
	static std::shared_ptr<T> Create(CtorArgs... args)
	{
		std::shared_ptr<T> newObject = std::make_shared<T>(args...);

		if (std::shared_ptr<Identifiable> casted = std::dynamic_pointer_cast<Identifiable>(newObject))
		{
			// If we could cast the type T to Identifiable, assign it an ID
			newObject->Identifiable::Initialize();

			return newObject;
		}
		else
		{
			throw std::exception("Type to create does not derive from Identifiable");
		}
	}


	// Tries to get owner of the given ID. Returns true on success and assigns the
	// found object to the result. Otherwise returns false and assigns nullptr to result.
	static bool TryGetIDOwner(int ID, std::shared_ptr<Identifiable>& owner);

	// Tries to get owner of the given ID and type T. Returns true on success and assigns the
	// found object to the result. Otherwise returns false and assigns nullptr to result.
	template<typename T>
	static bool TryGetIDOwner(int ID, std::shared_ptr<T>& owner)
	{
		std::shared_ptr<Identifiable> result;

		if (TryGetIDOwner(ID, result))
		{
			return (bool)(owner = std::dynamic_pointer_cast<T>(result));
		}

		owner.reset();
		return false;
	}
	
	#pragma endregion

};