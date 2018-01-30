#include "stdafx.h"

#define SELECTABLE_LINES 5


// Default constructor
TLatch::TLatch()
{
	mPrintName = "T Latch (level driven)";
}


// Method performing initial routine for each part
void TLatch::InitializeRoutine()
{
	// Run the initialize routine of the base flip flop
	BaseLatch::InitializeRoutine();

	// Subscribe to clock's and D input's internal state changed events

	mClock->InternalStateChanged.Subscribe(std::dynamic_pointer_cast<IEventSubscriber>(shared_from_this()),
		std::bind(&BasePart::RefreshInfoStatus, this));

	mT->InternalStateChanged.Subscribe(std::dynamic_pointer_cast<IEventSubscriber>(shared_from_this()),
		std::bind(&TLatch::TChanged, this));
}


// Computes and assigns output value
void TLatch::ComputeOutput()
{
	// Part enabled condition
	if (mNegateStraw ^ mStraw->Value())
	{
		// Toggle the output (if T is on). Equation: Qnext = T^Q
		SetOutput(mT->Value() ^ mQ->Value());
	}
}


// Returns the numberof selectable lines in this part's info
int TLatch::SelectableLines()
{
	return BaseLatch::SelectableLines() + SELECTABLE_LINES;
}


// Callback for when clock value changes
void TLatch::TChanged()
{
	// Output is only assigned in the brief moment when clock signal changes (rising or falling edge, depending
	// on the value set in mUseNegativeClock)
	if (mClock->Value() ^ mUseNegativeClock)
	{
		ComputeOutput();
	}
	else
	{
		PrintManagerStatus(Sector::PartInfo, ID());
	}
}


// Returns a list with lines that compose a full info on this part.
std::list<Line> TLatch::GenerateInfo()
{
	std::list<Line> l = BaseLatch::GenerateInfo();

	l.push_back(Line());
	l.push_back(Line("T input:"));
	l.splice(l.end(), mT->GenerateInfo());

	return l;
}


// Returns a list with lines that compose a full info on this part.
std::list<Line> TLatch::GenerateStatus()
{
	std::list<Line> l = BaseLatch::GenerateStatus();

	l.push_back(Line());
	l.push_back(Line());
	l.splice(l.end(), mT->GenerateStatus());

	return l;
}


// Performs quick action for the given line (if there's an action to perform)
void TLatch::QuickActionRequest(int index)
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
				mT->QuickAction();
			}
			break;
		}
	}
}


// Generates string to save in a file which allows to recreate this part
std::vector<std::string> TLatch::GenerateStorageString()
{
	std::string firstPart = std::to_string(T_LATCH);

	// Get the storage string from the elements of the base latch
	auto baseLatchInfo = BaseLatch::GenerateStorageSecondThird();

	// Include mT ID
	std::string secondPart = baseLatchInfo.first + ";" + std::to_string(-mT->ID());

	// Include mT connection
	std::string thirdPart = baseLatchInfo.second + mT->GenerateConnectionString();

	return std::vector<std::string> {thirdPart, firstPart, secondPart};
}


// Applies settings given in the vector
void TLatch::InterpretStorageString(std::string settings, std::list<std::pair<int, int>>& connections)
{
	auto v = Split(settings, ';');

	// Apply the settings of the base latch
	BaseLatch::ApplySettings(v, connections);

	try
	{
		SwapID(connections, std::stoi(v[10]), mT->ID());
	}
	catch (std::exception e)
	{
		throw Exception("Data is corrupted");
	}
}
