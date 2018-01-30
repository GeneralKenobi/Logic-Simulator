#pragma once
#include "stdafx.h"

// Base class for exceptions in this program
// Holds a string message
class Exception : public std::exception
{

protected:

	#pragma region Members

	// string which stores the error message
	std::string mErrorMessage;

	#pragma endregion

public:

	#pragma region Accessors

	// Returns the error message assigned to this exception
	std::string ErrorMessage() { return mErrorMessage; }

	#pragma endregion

	#pragma region Constructors/Destructors

	// Default constructor
	Exception() { }

	// Constructor assigning an error message
	Exception(std::string errorMessage) { mErrorMessage = errorMessage; }

	// Default destructor
	virtual ~Exception() { }

	#pragma endregion
	
};