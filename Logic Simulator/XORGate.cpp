#include "stdafx.h"


//Default Constructor
XORGate::XORGate()
{
	mPrintName = "XOR Gate";
}


// Computes value of OR gate (sets output to true if at least 1 input is true)
void XORGate::ComputeOutput()
{
	// Default value is off
	int onCount = 0;

	// Go through each input
	for (auto it = mInputs.begin(); it != mInputs.end(); ++it)
	{
		// If the input is on
		if ((*it)->Value())
		{
			++onCount;
		}
	}

	// XOR is true if an odd number of inputs is on
	bool newValue = onCount % 2 == 1;

	// If the new value is different than the old one
	if (newValue != mOutput->Value())
	{
		// Set the new value and repring status
		mOutput->Value(newValue);
		PrintManagerStatus(Sector::PartList, ID());
	}
}