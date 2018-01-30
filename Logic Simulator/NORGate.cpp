#include "stdafx.h"


//Default Constructor
NORGate::NORGate()
{
	mPrintName = "NOR Gate";
}


// Computes value of OR gate (sets output to true if at least 1 input is true)
void NORGate::ComputeOutput()
{
	// Default value is on
	bool newValue = true;

	// Go through each input
	for (auto it = mInputs.begin(); it != mInputs.end(); ++it)
	{
		// If any input is on
		if ((*it)->Value())
		{
			// Output will be off
			newValue = false;
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