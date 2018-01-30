#include "stdafx.h"

#define COMPACT_INPUTS_PER_LINE 5


// Default constructor
MUX::MUX()
{
	mPrintName = "Multiplexer";
}


void MUX::ComputeOutput()
{
	// Treat the inputs as a number in binary so that this combination of addresses: 10011 will give input indexed 19
	// Because (starting from end) 1*2^0 + 1*2^1 + 0*2^2 + 0*2^3 + 1*2^4 = 1 + 2 + 0 + 0 + 16 = 19

	// Address to choose
	int address = 0;

	// Multiplier used to give weight to the addresses
	int multiplier = std::pow(2, mAddresses.size() - 1);

	for (auto it = mAddresses.begin(); it != mAddresses.end(); ++it, multiplier/=2)
	{
		// If the input is on
		if ((*it)->Value())
		{
			// Add the multiplier
			address+=multiplier;
		}
	}

	// Get the iterator to the input with address index
	auto inputIt = mInputs.begin();
	std::advance(inputIt, address);

	// If the new input is different than the currently selected input
	if (mSelectedInput != *inputIt)
	{
		// If the currently selected input isn't expired
		if(mSelectedInput)
		{
			// Unsubscribe from its events
			mSelectedInput->InternalStateChanged.Unsubscribe(std::dynamic_pointer_cast<IEventSubscriber>(shared_from_this()));

			// And subscribe InputChanged method
			mSelectedInput->InternalStateChanged.Subscribe(std::dynamic_pointer_cast<IEventSubscriber>(shared_from_this()),
				std::bind(&MUX::InputChanged, this));
		}

		// Assign the new selected input
		mSelectedInput = *inputIt;
				
		// And subscribe OutputChanged method
		mSelectedInput->InternalStateChanged.Subscribe(std::dynamic_pointer_cast<IEventSubscriber>(shared_from_this()),
			std::bind(&MUX::SelectedInputChanged, this));

		// Get the new value
		bool newValue = (*inputIt)->Value();

		// If it's different than the currently outputted
		if (newValue != mOutput->Value())
		{
			// Assign it
			mOutput->Value(newValue);
			PrintManagerStatus(Sector::PartList | Sector::PartInfo, ID());
		}
	}	
}



// Method that will assign owner id to every OutputSocket, should be overridden
// by every derived class that introduces a new OutputSocket
void MUX::AssignOwnership()
{
	AssignNodeOwnership(mOutput->mAssignedNode, mOutput);
}


// Decreases the size of the multiplexer to newSize (which should be smaller than mSize). Helper of ChangeSize
void MUX::ReduceSize(int newSize)
{
	if (newSize >= mSize || newSize <= 0)
	{
		return;
	}

	// Create inputs to match the 2^newSize number
	int inputsToRemove = mInputs.size() - std::pow(2, newSize);

	for (int i = 0; i < inputsToRemove; ++i)
	{
		mInputs.back().reset();
		mInputs.pop_back();
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
void MUX::IncreaseSize(int newSize)
{
	if (newSize <= mSize)
	{
		return;
	}

	// Create inputs to match the 2^newSize number
	int inputsToCreate = std::pow(2, newSize) - mInputs.size();

	// Loop creating inputs
	for (int i = 0; i < inputsToCreate; ++i)
	{
		auto newInput = Identifiable::Create<InputSocket>();

		newInput->InternalStateChanged.Subscribe(std::dynamic_pointer_cast<IEventSubscriber>(shared_from_this()),
			std::bind(&MUX::InputChanged, this));

		mInputs.push_back(newInput);
	}

	// Loop creating new Addresses
	for (int i = mAddresses.size(); i < newSize; ++i)
	{
		auto newAddress = Identifiable::Create<InputSocket>();

		newAddress->InternalStateChanged.Subscribe(std::dynamic_pointer_cast<IEventSubscriber>(shared_from_this()),
			std::bind(&MUX::AddressChanged, this));

		mAddresses.push_back(newAddress);
	}

	mSize = newSize;

	ComputeOutput();
}





// Reprints the statuses
void MUX::InputChanged()
{
	if(CompactMode())
	{
		PrintManager(Sector::PartInfo);		
	}
	else
	{
		PrintManagerStatus(Sector::PartInfo, ID());
	}
}


// Reprints part list statuses
void MUX::SelectedInputChanged()
{
	mOutput->Value(mSelectedInput->Value());
	if (CompactMode())
	{
		PrintManagerStatus(Sector::PartList, ID());
		PrintManager(Sector::PartInfo);
	}
	else
	{
		PrintManagerStatus(Sector::PartList | PartInfo, ID());
	}
}





// Returns a list with lines that compose a full info on this part.
std::list<Line> MUX::GenerateInfo()
{
	// General info
	std::list<Line> l = BasePart::GenerateInfo();

	// Info specific to MUX
	l.push_back(Line("Size: " + std::to_string(mSize)));
	l.push_back(Line("(QA) Increase"));
	l.push_back(Line("(QA) Decrease"));
	l.push_back(Line());

	// Output info
	l.push_back(Line("Output:"));
	l.splice(l.end(), mOutput->GenerateInfo());
	l.push_back(Line());

	// Addresses info
	l.push_back(Line("Addresses (" + std::to_string(mSize) + "):"));
	l.splice(l.end(), CompactMode() ? GenerateAddressInfoCompact() : GenerateAddressInfo());
	l.push_back(Line());

	// Inputs info
	l.push_back(Line("Inputs (" + std::to_string(mInputs.size()) + "):"));
	l.splice(l.end(), CompactMode() ? GenerateInputsInfoCompact() : GenerateInputsInfo());

	return l;
}


// Returns status lines for the part which has the same structure as list generated by GenerateInfo.
// All lines that don't carry information about status are empty
std::list<Line> MUX::GenerateStatus()
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
	l.splice(l.end(), mOutput->GenerateStatus());
	l.push_back(Line());

	// Addresses info
	l.push_back(Line());
	l.splice(l.end(), GenerateAddressStatus());
	l.push_back(Line());

	// Inputs info
	l.push_back(Line());
	l.splice(l.end(), GenerateInputsStatus());

	return l;
}





// Helper method, generates info about inputs
std::list<Line> MUX::GenerateInputsInfo()
{
	std::list<Line> l;

	for (auto it = mInputs.begin(); it != mInputs.end(); ++it)
	{
		l.push_back(Line());
		l.splice(l.end(), (*it)->GenerateInfo());
	}

	return l;
}


// Helper method, generates compact info about inputs (one line per inputs)
std::list<Line> MUX::GenerateInputsInfoCompact()
{
	Line line;
	std::list<Line> list;

	int counter = 0;

	for (auto it = mInputs.begin(); it != mInputs.end(); ++it, ++counter)
	{		
		if (counter == COMPACT_INPUTS_PER_LINE)
		{
			list.push_back(line);
			line = Line();
			counter = 0;
		}

		line+=(*it)->GenerateInfoCompact();
	}

	list.push_back(line);
	return list;
}


// Helper method, generates only inputs' statuses
std::list<Line> MUX::GenerateInputsStatus()
{
	std::list<Line> l;

	for (auto it = mInputs.begin(); it != mInputs.end(); ++it)
	{
		l.push_back(Line());
		l.splice(l.end(), (*it)->GenerateStatus());
	}

	return l;
}


// Prints the compact information on this part (dedicated to the list of parts)
void MUX::PrintList()
{
	BasePart::PrintList();

	PrintStatus(mOutput->Value());
}


// Performs quick action for the given line (if there's an action to perform)
void MUX::QuickActionRequest(int index)
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
				IncreaseSize(mSize+1);
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

		// Quick connect output
		case 9:
		{
			mOutput->QuickAction();
		}
		break;

		default:
		{
			// Compact mode doesn't allow quick actions
			if (CompactMode())
			{
				break;
			}

			// Quick action connect/disconnect on address
			if (index > 14 && index < 14 + mSize*4 && index % 4 == 3)
			{
				int addressIndex = (index - 15)/4;
				auto addressIterator = mAddresses.begin();
				std::advance(addressIterator, addressIndex);
				(*addressIterator)->QuickAction();
			}

			// Quick action connect/disconnect on input
			if (index > 14 + mSize * 4 && index % 4 == 1)
			{
				int inputIndex = (index - 15 - mSize*4) / 4;
				auto inputIterator = mInputs.begin();
				std::advance(inputIterator, inputIndex);
				(*inputIterator)->QuickAction();
			}			
		}
		break;
	}
}

std::vector<std::string> MUX::GenerateStorageString()
{
	std::string firstPart = std::to_string(MULTIPLEXER);

	std::string secondPart = std::to_string(-ID()) + ";" + 
		std::to_string(-mOutput->ID()) + ";" + std::to_string(mSize);

	std::string thirdPart;
	
	for (auto it = mAddresses.begin(); it != mAddresses.end(); ++it)
	{
		secondPart += ";" + std::to_string(-(*it)->ID());
		thirdPart += (*it)->GenerateConnectionString();
	}

	for (auto it = mInputs.begin(); it != mInputs.end(); ++it)
	{
		secondPart += ";" + std::to_string(-(*it)->ID());
		thirdPart += (*it)->GenerateConnectionString();
	}

	return std::vector<std::string> {thirdPart, firstPart, secondPart};
}

void MUX::InterpretStorageString(std::string settings, std::list<std::pair<int, int>>& connections)
{
	auto v = Split(settings, ';');

	std::vector<int> data;

	try
	{
		for(int i=0; i<v.size(); ++i)
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
		SwapID(connections,data[counter], (*it)->ID());
	}

	for (auto it = mInputs.begin(); it != mInputs.end(); ++it, ++counter)
	{
		SwapID(connections, data[counter], (*it)->ID());
	}
}
