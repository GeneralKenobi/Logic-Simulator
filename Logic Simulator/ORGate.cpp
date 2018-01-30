#include "stdafx.h"


//Default Constructor
ORGate::ORGate()
{
	mPrintName = "OR Gate";
}


// Computes value of OR gate (sets output to true if at least 1 input is true)
void ORGate::ComputeOutput()
{
	// Default value is off
	bool newValue = false;

	// Go through each input
	for (auto it = mInputs.begin(); it != mInputs.end(); ++it)
	{
		// If any input is on
		if ((*it)->Value())
		{
			// New value will be on
			newValue = true;
			break;
		}
	}	

	// If the new value is different than the old one
	if (newValue != mOutput->Value())
	{
		// Set the new value and repring status
		mOutput->Value(newValue);
		PrintManagerStatus(Sector::PartList, ID());
	}
}