#pragma once
#include "stdafx.h"

// Class for an OR gate - which is ON if at least one of the inputs is on
class ORGate : public BaseMultiInputGate
{

protected:

	#pragma region Virtual Methods

	// Computes value of OR gate (sets output to true if at least 1 input is true)
	virtual void ComputeOutput();

	// Method which returns code of the specific MultiInputGate
	virtual int GetCode() { return OR_GATE; }

	#pragma endregion

public:
	
	#pragma region Constructors/Destructors

	// Default Constructor
	ORGate();

	// Default Destructor
	virtual ~ORGate() {}

	#pragma endregion
};