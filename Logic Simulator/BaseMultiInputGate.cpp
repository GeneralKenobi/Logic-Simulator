#include "stdafx.h"

#define BASE_SELECTABLE_LINES 10
#define SELECTABLE_LINES_PER_INPUT 4
#define MAX_NUMBER_OF_INPUTS 12



// Default Constructor
BaseMultiInputGate::BaseMultiInputGate()
{

}

BaseMultiInputGate::~BaseMultiInputGate()
{
	
}


// Returns the numberof selectable lines in this part's info
int BaseMultiInputGate::SelectableLines()
{
	return BASE_SELECTABLE_LINES + mInputs.size()*SELECTABLE_LINES_PER_INPUT;
}


// Adds 2 default inputs, assigns ownership
void BaseMultiInputGate::InitializeRoutine()
{
	CreateInput();
	CreateInput();
}


// Prints the compact information on this part (dedicated to the list of parts)
// Printed informations is: "*PrintName*, ID: 5, Status: On/Off".
void BaseMultiInputGate::PrintList()
{
	// Print the name and the ID
	BasePart::PrintList();	

	// Print the status
	PrintStatus(mOutput->Value());
}


// Crates a new input in this part
void BaseMultiInputGate::CreateInput()
{
	auto newInput = Identifiable::Create<InputSocket>();

	// Subscribe to the InternalStateChanged event
	newInput->InternalStateChanged.Subscribe(
		std::dynamic_pointer_cast<IEventSubscriber>(shared_from_this()), std::bind(&BasePart::InputChanged, this));

	mInputs.push_back(newInput);

	ComputeOutput();
}


// Deletes the input at the given index
void BaseMultiInputGate::DeleteInput(int inputIndex)
{
	if (inputIndex >= 0 && inputIndex < mInputs.size())
	{
		// Remove the item using an iterator advanced to the inputIndex position
		auto it = mInputs.begin();
		std::advance(it, inputIndex);

		mInputs.erase(it);

		ComputeOutput();
	}
}


// Method that will assign owner id to every OutputSocket, should be overridden
// by every derived class that introduces a new OutputSocket
void BaseMultiInputGate::AssignOwnership()
{
	AssignNodeOwnership(mOutput->mAssignedNode, mOutput);
}

// Performs quick action for the given line (if there's an action to perform)
void BaseMultiInputGate::QuickActionRequest(int index)
{
	switch (index)
	{
		case 0:
		case 1:
		case 2:
		{
			BasePart::QuickActionRequest(index);
		}
		break;

		case 6:
		{
			mOutput->QuickAction();
		}
		break;

		case 9:
		{
			// Check if the part isn't full yet
			if(mInputs.size() < MAX_NUMBER_OF_INPUTS)
			{
				CreateInput();
				RecalculateMaxInfoLines();
				PrintManager(Sector::PartInfo);
			}
		}
		break;
		
		// Filter out unused cases so that they don't accidentally trigger in default
		case 3:case 4:case 5:case 7:case 8:case 10:
		break;

		default:
		{
			// All indexes that give 3 remainder after dividing by 4 indicate that delete input action was requested
			if (index % 4 == 3)
			{
				// Subtract the previous lines and then divide by 4 (each input occupies 4 lines)
				DeleteInput((index - 11)/4);
				RecalculateMaxInfoLines();
				PrintManager(Sector::PartInfo);
			}

			// All indexes that give 9 remainder after dividing by 4 indicate that quick connection action was requested
			if (index % 4 == 0)
			{
				int input = (index - 12)/4;
				auto it = mInputs.begin();
				std::advance(it, input);
				(*it)->QuickAction();				
			}
		}
		break;

	}

}


// Generates string to save in a file which allows to recreate this part
// part_code
// ID, mOutputID, 
std::vector<std::string> BaseMultiInputGate::GenerateStorageString()
{
	std::string firstPart = std::to_string(GetCode());

	std::string secondPart = std::to_string(-ID()) + ";" + std::to_string(-mOutput->ID()) + ";" + std::to_string(mInputs.size());

	std::string thirdPart;

	for (auto it = mInputs.begin(); it != mInputs.end(); ++it)
	{
		secondPart += ";" + std::to_string(-(*it)->ID());
		thirdPart += (*it)->GenerateConnectionString();
	}

	return std::vector<std::string> {thirdPart, firstPart, secondPart};
}


void BaseMultiInputGate::InterpretStorageString(std::string settings, std::list<std::pair<int, int>>& connections)
{
	std::vector<std::string> v = Split(settings, ';');
	std::vector<int> data;

	try
	{
		for (int i = 0; i < v.size(); ++i)
		{
			data.push_back(std::stoi(v[i]));
		}
	}
	catch (std::exception e)
	{
		throw Exception("Data is corrupted");
	}

	SwapID(connections, data[0], ID());
	SwapID(connections, data[1], mOutput->ID());

	mInputs.clear();

	for (int i = 3; i < data[2] + 3; ++i)
	{
		auto newInput = Identifiable::Create<InputSocket>();

		// Subscribe to the InternalStateChanged event
		newInput->InternalStateChanged.Subscribe(
			std::dynamic_pointer_cast<IEventSubscriber>(shared_from_this()), std::bind(&BasePart::InputChanged, this));

		mInputs.push_back(newInput);

		SwapID(connections, data[i], newInput->ID());
	}
}


// Returns a list with lines that compse a full info on this part.
// String is the line to print, int is the indentation (x offset) of the line
// Should be overriden to include info about the inherited class.
std::list<Line> BaseMultiInputGate::GenerateInfo()
{
	std::list<Line> l = BasePart::GenerateInfo();

	l.splice(l.end(), GenerateOutputInfo());
	l.splice(l.end(), GenerateInputInfo());

	return l;
}

// Returns status lines for the part which has the same structure as list generated by GenerateInfo.
// All lines that don't carry information about status are empty
std::list<Line> BaseMultiInputGate::GenerateStatus()
{
	std::list<Line> l = BasePart::GenerateStatus();

	l.splice(l.end(), GenerateOutputStatus());
	l.splice(l.end(), GenerateInputStatus());

	return l;
}

// Helper method which generates information on all input sockets in the form:
// 1. ID: 11
// Connected To : 17 (Toggle Input) (press f to disconnect)
// Status : ON
//
// 2. ID: 12
// Not Connected    (press f to create a connection)
// Status : OFF
std::list<Line> BaseMultiInputGate::GenerateInputInfo()
{
	std::list<Line> l
	{
		Line("Inputs: " + std::to_string(mInputs.size()) + " (QA) Add"),
	};

	int iteration = 1;
	for (auto it = mInputs.begin(); it != mInputs.end(); ++it, ++iteration)
	{
		l.push_back(Line());

		auto inputInfo = (*it)->GenerateInfo();
		inputInfo.begin()->Text += " (QA) Remove";

		l.splice(l.end(), inputInfo);
	}

	return l;
}

// Helper method which generates list of lines either empty or containing information on status
std::list<Line> BaseMultiInputGate::GenerateInputStatus()
{
	std::list<Line> l
	{
		Line(),
	};

	int iteration = 1;

	for (auto it = mInputs.begin(); it != mInputs.end(); ++it, ++iteration)
	{
		l.push_back(Line());
		l.splice(l.end(), (*it)->GenerateStatus());
	}

	return l;
}


// Helper method which generates information on all input sockets in the form:
// Example:
// Output:
//		ID: 14
//		Status: OFF
//		Press f to create a connection
//
std::list<Line> BaseMultiInputGate::GenerateOutputInfo()
{
	std::list<Line> l
	{
		Line("Output:"),
		Line(),
	};

	l.splice(l.end(), mOutput->GenerateInfo());

	l.push_back(Line());

	return l;
}

// Helper method which generates list of lines either empty or containing information on status
std::list<Line> BaseMultiInputGate::GenerateOutputStatus()
{
	std::list<Line> l
	{
		Line(),
		Line(),
	};

	l.splice(l.end(), mOutput->GenerateStatus());

	l.push_back(Line());

	return l;
}