#pragma once
#include "stdafx.h"

// Class for an OR gate - which is ON if at least one of the inputs is on
class XORGate : public BaseMultiInputGate
{

protected:

	#pragma region Virtual Methods

	// Computes value of OR gate (sets output to true if at least 1 input is true)
	virtual void ComputeOutput();

	// Method which returns code of the specific MultiInputGate
	virtual int GetCode() { return XOR_GATE; }


	#pragma endregion

public:

	#pragma region Constructors/Destructors

	// Default Constructor
	XORGate();

	// Default Destructor
	virtual ~XORGate() {}

	#pragma endregion
};