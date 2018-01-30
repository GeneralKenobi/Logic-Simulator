#include "stdafx.h"


//Default Constructor
ANDGate::ANDGate()
{
	mPrintName = "AND Gate";
}


// Computes value of OR gate (sets output to true if at least 1 input is true)
void ANDGate::ComputeOutput()
{
	// Default value is on
	bool newValue = true;

	// If there are no inputs set the output will be off
	if (mInputs.empty())
	{
		newValue = false;
	}
	else
	{
		// Go through each input
		for (auto it = mInputs.begin(); it != mInputs.end(); ++it)
		{
			// If any input is off
			if (!(*it)->Value())
			{
				// New value will be off
				newValue = false;
				break;
			}
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


