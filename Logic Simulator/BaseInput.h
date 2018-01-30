#pragma once
#include "stdafx.h"


class BaseInput : public BasePart
{
protected:

	#pragma region Members

	// Output Socket
	const std::shared_ptr<OutputSocket> mOutput = Identifiable::Create<OutputSocket>();

	#pragma endregion

	#pragma region Virtual Methods

	// Toggles this input's value
	virtual void ComputeOutput() { }

	// Initializes the part
	virtual void InitializeRoutine() { }

	// Assigns ownership of output socket to this particular object
	virtual void AssignOwnership();

	// Sets the given state to this input
	void SetState(bool state);

	#pragma endregion

public:

	#pragma region Constructors/Destructors

	// Default Constructor
	BaseInput() { }

	// Default Destructor
	virtual ~BaseInput() {}

	#pragma endregion
	
	#pragma region Virtual Methods

	// Prints the information about this part for list of parts
	virtual void PrintList();	

	#pragma endregion
};