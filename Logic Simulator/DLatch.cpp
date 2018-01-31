#include "stdafx.h"

#define SELECTABLE_LINES 5


// Default constructor
DLatch::DLatch()
{
	mPrintName = "D Latch (level driven)";
}


// Method performing initial routine for each part
void DLatch::InitializeRoutine()
{
	// Run the initialize routine of the base flip flop
	BaseLatch::InitializeRoutine();

	// Subscribe to clock's and D input's internal state changed events

	mClock->InternalStateChanged.Subscribe(std::dynamic_pointer_cast<IEventSubscriber>(shared_from_this()),
		std::bind(&BasePart::RefreshInfoStatus, this));

	mD->InternalStateChanged.Subscribe(std::dynamic_pointer_cast<IEventSubscriber>(shared_from_this()),
		std::bind(&DLatch::DChanged, this));
}


// Computes and assigns output value
void DLatch::ComputeOutput()
{
	// Part enabled condition
	if (mNegateStraw ^ mStraw->Value())
	{
		// Set value of D input
		SetOutput(mD->Value());
	}
}


// Returns the numberof selectable lines in this part's info
int DLatch::SelectableLines()
{
	return BaseLatch::SelectableLines() + SELECTABLE_LINES;
}


// Callback for when clock value changes
void DLatch::DChanged()
{
	// Output is only assigned in the brief moment when clock signal changes (rising or falling edge, depending
	// on the value set in mUseNegativeClock)
	if (mClock->Value() ^ mUseNegativeClock)
	{
		ComputeOutput();
	}
	
	PrintManagerStatus(Sector::PartInfo, ID());
	
}


// Returns a list with lines that compose a full info on this part.
std::list<Line> DLatch::GenerateInfo()
{
	std::list<Line> l = BaseLatch::GenerateInfo();

	l.push_back(Line());
	l.push_back(Line("D input:"));
	l.splice(l.end(), mD->GenerateInfo());

	return l;
}


// Returns a list with lines that compose a full info on this part.
std::list<Line> DLatch::GenerateStatus()
{
	std::list<Line> l = BaseLatch::GenerateStatus();

	l.push_back(Line());
	l.push_back(Line());
	l.splice(l.end(), mD->GenerateStatus());

	return l;
}


// Performs quick action for the given line (if there's an action to perform)
void DLatch::QuickActionRequest(int index)
{
	if (index <= 35)
	{
		BaseLatch::QuickActionRequest(index);
	}
	else
	{
		switch (index)
		{
			case 39:
			{
				mD->QuickAction();
			}
			break;
		}
	}
}


// Generates string to save in a file which allows to recreate this part
std::vector<std::string> DLatch::GenerateStorageString()
{
	std::string firstPart = std::to_string(D_LATCH);

	// Get the storage string from the elements of the base latch
	auto baseLatchInfo = BaseLatch::GenerateStorageSecondThird();

	// Include mD ID
	std::string secondPart = baseLatchInfo.first + ";" + std::to_string(-mD->ID());

	// Include mD connection
	std::string thirdPart = baseLatchInfo.second + mD->GenerateConnectionString();

	return std::vector<std::string> {thirdPart, firstPart, secondPart};
}


// Applies settings given in the vector
void DLatch::InterpretStorageString(std::string settings, std::list<std::pair<int, int>>& connections)
{
	auto v = Split(settings, ';');

	// Apply the settings of the base latch
	BaseLatch::ApplySettings(v, connections);

	try
	{
		SwapID(connections, std::stoi(v[10]), mD->ID());
	}
	catch (std::exception e)
	{
		throw Exception("Data is corrupted");
	}
}
