#include "stdafx.h"

#define COMPACT_INPUTS_PER_LINE 5



// Default constructor
DMUX::DMUX()
{
	mPrintName = "Demultiplexer";
}


void DMUX::ComputeOutput()
{
	// Treat the inputs as a number in binary so that this combination of addresses: 10011 will give input indexed 19
	// Because (starting from end) 1*2^0 + 1*2^1 + 0*2^2 + 0*2^3 + 1*2^4 = 1 + 2 + 0 + 0 + 16 = 19

	// Address to choose
	int address = 0;

	// Multiplier used to give weight to the addresses
	int multiplier = std::pow(2, mAddresses.size() - 1);

	for (auto it = mAddresses.begin(); it != mAddresses.end(); ++it, multiplier /= 2)
	{
		// If the input is on
		if ((*it)->Value())
		{
			// Add the multiplier
			address += multiplier;
		}
	}

	// Get the iterator to the input with address index
	auto outputIt = mOutputs.begin();
	std::advance(outputIt, address);

	// If the new input is different than the currently selected input
	if (mSelectedOutput != *outputIt)
	{
		// reset the output
		if(mSelectedOutput)
		{
			mSelectedOutput->Value(false);
		}

		// Assign the new selected input
		mSelectedOutput = *outputIt;		

		// Get the new value
		bool newValue = mInput->Value();

		// If it's different than the currently outputted
		if (newValue != mSelectedOutput->Value())
		{
			// Assign it
			mSelectedOutput->Value(newValue);
			PrintManagerStatus(Sector::PartList, ID());
		}
	}
}

// Method performing initial routine for this part. Subscribes to input changed event
void DMUX::InitializeRoutine()
{
	BaseMuxDmux::InitializeRoutine();

	mInput->InternalStateChanged.Subscribe(std::dynamic_pointer_cast<IEventSubscriber>(shared_from_this()),
		std::bind(&DMUX::InputChanged, this));
}


// Decreases the size of the multiplexer to newSize (which should be smaller than mSize). Helper of ChangeSize
void DMUX::ReduceSize(int newSize)
{
	if (newSize >= mSize || newSize <= 0)
	{
		return;
	}

	// Create inputs to match the 2^newSize number
	int outputsToRemove = mOutputs.size() - std::pow(2, newSize);

	for (int i = 0; i < outputsToRemove; ++i)
	{
		mOutputs.back().reset();
		mOutputs.pop_back();
	}

	// Create addresses to match the newSize number
	for (int i = mAddresses.size(); i > newSize; --i)
	{
		mAddresses.back().reset();
		mAddresses.pop_back();
	}

	mSize = newSize;

	ComputeOutput();
}


// Increases the size of the multiplexer to newSize (which should be greater than mSize). Helper of ChangeSize
void DMUX::IncreaseSize(int newSize)
{
	if (newSize <= mSize)
	{
		return;
	}

	// Create inputs to match the 2^newSize number
	int outputsToCreate = std::pow(2, newSize) - mOutputs.size();

	// Loop creating inputs
	for (int i = 0; i < outputsToCreate; ++i)
	{
		auto newOutput = Identifiable::Create<OutputSocket>();

		AssignNodeOwnership(newOutput->mAssignedNode, newOutput);

		mOutputs.push_back(newOutput);
	}

	// Loop creating new Addresses
	for (int i = mAddresses.size(); i < newSize; ++i)
	{
		auto newAddress = Identifiable::Create<InputSocket>();

		newAddress->InternalStateChanged.Subscribe(std::dynamic_pointer_cast<IEventSubscriber>(shared_from_this()),
			std::bind(&DMUX::AddressChanged, this));

		mAddresses.push_back(newAddress);
	}

	mSize = newSize;

	ComputeOutput();
}

// Reprints the statuses
void DMUX::InputChanged()
{
	mSelectedOutput->Value(mInput->Value());

	if(CompactMode())
	{
		PrintManager(Sector::PartInfo);
	}
	else
	{
		PrintManagerStatus(Sector::PartInfo, ID());
	}
}


// Returns a list with lines that compose a full info on this part.
// Should be overriden to include info about the inherited class.
std::list<Line> DMUX::GenerateInfo()
{
	// General info
	std::list<Line> l = BasePart::GenerateInfo();

	// Info specific to MUX
	l.push_back(Line("Size: " + std::to_string(mSize)));
	l.push_back(Line("(QA) Increase"));
	l.push_back(Line("(QA) Decrease"));
	l.push_back(Line());

	// Output info
	l.push_back(Line("Input:"));
	l.splice(l.end(), mInput->GenerateInfo());
	l.push_back(Line());

	// Addresses info
	l.push_back(Line("Addresses (" + std::to_string(mSize) + "):"));
	l.splice(l.end(), CompactMode() ? GenerateAddressInfoCompact() : GenerateAddressInfo());
	l.push_back(Line());

	// Outputs info
	l.push_back(Line("Outputs (" + std::to_string(mOutputs.size()) + "):"));
	l.splice(l.end(), CompactMode() ? GenerateOutputsInfoCompact() : GenerateOutputsInfo());

	return l;
}


std::list<Line> DMUX::GenerateStatus()
{
	// General info
	std::list<Line> l = BasePart::GenerateStatus();

	// Info specific to MUX
	l.push_back(Line());
	l.push_back(Line());
	l.push_back(Line());
	l.push_back(Line());

	// Output info
	l.push_back(Line());
	l.splice(l.end(), mInput->GenerateStatus());
	l.push_back(Line());

	// Addresses info
	l.push_back(Line());
	l.splice(l.end(), GenerateAddressStatus());
	l.push_back(Line());

	// Inputs info
	l.push_back(Line());
	l.splice(l.end(), GenerateOutputsStatus());

	return l;
}


// Helper method, generates info about inputs
std::list<Line> DMUX::GenerateOutputsInfo()
{
	std::list<Line> l;

	for (auto it = mOutputs.begin(); it != mOutputs.end(); ++it)
	{
		l.push_back(Line());
		l.splice(l.end(), (*it)->GenerateInfo());
	}

	return l;
}


// Helper method, generates compact info about inputs (one line per inputs)
std::list<Line> DMUX::GenerateOutputsInfoCompact()
{
	Line line;
	std::list<Line> list;

	int counter = 0;

	for (auto it = mOutputs.begin(); it != mOutputs.end(); ++it, ++counter)
	{
		if (counter == COMPACT_INPUTS_PER_LINE)
		{
			list.push_back(line);
			line = Line();
			counter = 0;
		}

		line += (*it)->GenerateInfoCompact();
	}

	list.push_back(line);
	return list;
}


// Helper method, generates only inputs' statuses
std::list<Line> DMUX::GenerateOutputsStatus()
{
	std::list<Line> l;

	for (auto it = mOutputs.begin(); it != mOutputs.end(); ++it)
	{
		l.push_back(Line());
		l.splice(l.end(), (*it)->GenerateStatus());
	}

	return l;
}


// Prints the compact information on this part (dedicated to the list of parts)
// Printed informations is: "*PrintName*, ID: 5". Can (and should) be overriden
// to provide more specific information
void DMUX::PrintList()
{
	BasePart::PrintList();

	if(mSelectedOutput)
	{
		PrintStatus(mSelectedOutput->Value());
	}
	else
	{
		PrintStatus(false);
	}
}


// Performs quick action for the given line (if there's an action to perform)
void DMUX::QuickActionRequest(int index)
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

		// Increase size quick action
		case 4:
		{
			if(mSize < 7)
			{
				IncreaseSize(mSize + 1);
				RecalculateMaxInfoLines();
				PrintManager(Sector::PartInfo);
			}
		}
		break;

		// Decrease size quick action
		case 5:
		{
			if(mSize > 1)
			{
				ReduceSize(mSize - 1);
				RecalculateMaxInfoLines();
				PrintManager(Sector::PartInfo);
			}
		}
		break;

		// Quick connect input
		case 9:
		{
			mInput->QuickAction();
		}
		break;

		default:
		{
			// Quick action connect/disconnect on address
			if (index > 14 && index < 14 + mSize * 4 && index % 4 == 3)
			{
				int addressIndex = (index - 15) / 4;
				auto addressIterator = mAddresses.begin();
				std::advance(addressIterator, addressIndex);
				(*addressIterator)->QuickAction();
			}

			// Quick action connect/disconnect on outputs
			if (index > 14 + mSize * 4 && index % 4 == 1)
			{
				int outputIndex = (index - 17 - mSize * 4) / 4;
				auto outputIterator = mOutputs.begin();
				std::advance(outputIterator, outputIndex);
				QuickActionCommand("Connect " + std::to_string((*outputIterator)->ID()) + ",");
			}
		}
		break;
	}
}


std::vector<std::string> DMUX::GenerateStorageString()
{
	std::string firstPart = std::to_string(DEMULTIPLEXER);

	std::string secondPart = std::to_string(-ID()) + ";"  +
		std::to_string(-mInput->ID()) + ";" + std::to_string(mSize);

	std::string thirdPart = mInput->GenerateConnectionString();

	for (auto it = mAddresses.begin(); it != mAddresses.end(); ++it)
	{
		secondPart += ";" + std::to_string(-(*it)->ID());
		thirdPart += (*it)->GenerateConnectionString();
	}

	for (auto it = mOutputs.begin(); it != mOutputs.end(); ++it)
	{
		secondPart += ";" + std::to_string(-(*it)->ID());
	}

	return std::vector<std::string> {thirdPart, firstPart, secondPart};
}

void DMUX::InterpretStorageString(std::string settings, std::list<std::pair<int, int>>& connections)
{
	auto v = Split(settings, ';');

	std::vector<int> data;

	try
	{
		for (int i = 0; i<v.size(); ++i)
		{
			data.push_back(std::stoi(v[i]));
		}
	}
	catch (std::exception e)
	{
		throw Exception("Data is corrupted");
	}

	SwapID(connections, data[0], ID());
	SwapID(connections, data[1], mInput->ID());

	if (mSize > data[2])
	{
		ReduceSize(data[2]);
	}
	else
	{
		IncreaseSize(data[2]);
	}

	int counter = 3;

	for (auto it = mAddresses.begin(); it != mAddresses.end(); ++it, ++counter)
	{
		SwapID(connections, data[counter], (*it)->ID());
	}

	for (auto it = mOutputs.begin(); it != mOutputs.end(); ++it, ++counter)
	{
		SwapID(connections, data[counter], (*it)->ID());
	}
}
