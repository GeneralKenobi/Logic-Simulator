#include "stdafx.h"


// Prints the information about this part for list of parts
void BaseInput::PrintList()
{
	BasePart::PrintList();

	PrintStatus(mOutput->Value());
}


// Assigns ownership of output socket to this particular object
void BaseInput::AssignOwnership()
{
	AssignNodeOwnership(mOutput->mAssignedNode, mOutput);
}


// Sets the given state to this input
void BaseInput::SetState(bool state)
{
	mOutput->Value(state);
	PrintManagerStatus(Sector::PartInfo | Sector::PartList, ID());
}
