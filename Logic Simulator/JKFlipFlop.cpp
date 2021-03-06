#include "stdafx.h"

#define SELECTABLE_LINES 10


// Default constructor
JKFlipFlop::JKFlipFlop()
{
	mPrintName = "JK flip flop (edge driven)";
}


// Method performing initial routine for each part
void JKFlipFlop::InitializeRoutine()
{
	// Run the initialize routine of the base flip flop
	BaseLatch::InitializeRoutine();

	// Subscribe to clock's and D input's internal state changed events

	mClock->InternalStateChanged.Subscribe(std::dynamic_pointer_cast<IEventSubscriber>(shared_from_this()),
		std::bind(&JKFlipFlop::ClockChanged, this));

	mJ->InternalStateChanged.Subscribe(std::dynamic_pointer_cast<IEventSubscriber>(shared_from_this()),
		std::bind(&BasePart::RefreshInfoStatus, this));

	mK->InternalStateChanged.Subscribe(std::dynamic_pointer_cast<IEventSubscriber>(shared_from_this()),
		std::bind(&BasePart::RefreshInfoStatus, this));
}


// Computes and assigns output value
void JKFlipFlop::ComputeOutput()
{
	SetOutput((mJ->Value() && mQComplement->Value()) || (!mK->Value() && mQ->Value()));
}


// Returns the numberof selectable lines in this part's info
int JKFlipFlop::SelectableLines()
{
	return BaseLatch::SelectableLines() + SELECTABLE_LINES;
}


// Callback for when clock value changes
void JKFlipFlop::ClockChanged()
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
std::list<Line> JKFlipFlop::GenerateInfo()
{
	std::list<Line> l = BaseLatch::GenerateInfo();

	l.push_back(Line());
	l.push_back(Line("J input:"));
	l.splice(l.end(), mJ->GenerateInfo());

	l.push_back(Line());
	l.push_back(Line("K input:"));
	l.splice(l.end(), mK->GenerateInfo());

	return l;
}


// Returns a list with lines that compose a full info on this part.
std::list<Line> JKFlipFlop::GenerateStatus()
{
	std::list<Line> l = BaseLatch::GenerateStatus();

	// J input
	l.push_back(Line());
	l.push_back(Line());
	l.splice(l.end(), mJ->GenerateStatus());

	// K input
	l.push_back(Line());
	l.push_back(Line());
	l.splice(l.end(), mK->GenerateStatus());

	return l;
}


// Performs quick action for the given line (if there's an action to perform)
void JKFlipFlop::QuickActionRequest(int index)
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
				mJ->QuickAction();
			}
			break;

			case 44:
			{
				mK->QuickAction();
			}
			break;
		}
	}
}



// Generates string to save in a file which allows to recreate this part
std::vector<std::string> JKFlipFlop::GenerateStorageString()
{
	std::string firstPart = std::to_string(JK_FLIP_FLOP);

	// Get the storage string from the elements of the base latch
	auto baseLatchInfo = BaseLatch::GenerateStorageSecondThird();

	// Include mJ ID
	std::string secondPart = baseLatchInfo.first + ";" + std::to_string(-mJ->ID()) + ";" + std::to_string(-mK->ID());

	// Include mJ connection
	std::string thirdPart = baseLatchInfo.second + mJ->GenerateConnectionString() + mK->GenerateConnectionString();

	return std::vector<std::string> {thirdPart, firstPart, secondPart};
}


// Applies settings given in the vector
void JKFlipFlop::InterpretStorageString(std::string settings, std::list<std::pair<int, int>>& connections)
{
	auto v = Split(settings, ';');

	// Apply the settings of the base latch
	BaseLatch::ApplySettings(v, connections);

	try
	{
		SwapID(connections, std::stoi(v[10]), mJ->ID());
		SwapID(connections, std::stoi(v[11]), mK->ID());
	}
	catch (std::exception e)
	{
		throw Exception("Data is corrupted");
	}
}
