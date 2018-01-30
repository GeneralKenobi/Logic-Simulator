#include "stdafx.h"


//Default Constructor
NANDGate::NANDGate()
{
	mPrintName = "NAND Gate";
}


// Computes value of OR gate (sets output to true if at least 1 input is true)
void NANDGate::ComputeOutput()
{
	// Default value is off
	bool newValue = false;

	// If there are no inputs the output will be on
	if (mInputs.empty())
	{
		newValue = true;
	}
	else
	{
		// Go through each input
		for (auto it = mInputs.begin(); it != mInputs.end(); ++it)
		{
			// If any input is off
			if (!(*it)->Value())
			{
				// New output will be on
				newValue = true;
				break;;
			}
		}	
	}

	// If the new value is different than the old one
	if (mOutput->Value() != newValue)
	{
		// Set the new value and repring status
		mOutput->Value(newValue);
		PrintManagerStatus(Sector::PartList, ID());
	}
}