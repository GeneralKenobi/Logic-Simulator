#pragma once
#include "stdafx.h"

// Class for an OR gate - which is ON if all inputs are on
class ANDGate : public BaseMultiInputGate
{

protected:

	#pragma region Virtual Methods

	// Computes value of OR gate (sets output to true if at least 1 input is true)
	virtual void ComputeOutput();

	// Method which returns code of the specific MultiInputGate
	virtual int GetCode() { return AND_GATE; }

	#pragma endregion

public:

	#pragma region Constructors/Destructors

	// Default Constructor
	ANDGate();

	// Default Destructor
	virtual ~ANDGate() {}

	#pragma endregion
};

