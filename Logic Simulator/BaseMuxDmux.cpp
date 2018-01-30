#include "stdafx.h"


#define BASE_SELECTABLE_LINES 15
#define LINES_PER_INPUT 4
#define COMPACT_INPUTS_PER_LINE 5



// Method performing initial routine for this part. Subscribes to output changed event
void BaseMuxDmux::InitializeRoutine()
{
	IncreaseSize(1);
}


// Returns true if the size is big enough to enter compact display mode
bool BaseMuxDmux::CompactMode()
{
	return mSize > 3;
}


// Finds the new propagated input and reprints the statuses
void BaseMuxDmux::AddressChanged()
{
	ComputeOutput();

	if (CompactMode())
	{
		PrintManager(Sector::PartInfo);
	}
	else
	{
		PrintManagerStatus(Sector::PartInfo, ID());
	}
}


// Returns the numberof selectable lines in this part's info
int BaseMuxDmux::SelectableLines()
{
	if (CompactMode())
	{
		return BASE_SELECTABLE_LINES + std::ceil((double)mSize / COMPACT_INPUTS_PER_LINE) +
			std::ceil((double)std::pow(2, mSize) / COMPACT_INPUTS_PER_LINE);
	}
	else
	{
		return BASE_SELECTABLE_LINES + LINES_PER_INPUT*(mSize + std::pow(2, mSize));
	}
}


// Helper method, generates info about addresses
std::list<Line> BaseMuxDmux::GenerateAddressInfo()
{
	std::list<Line> l;

	for (auto it = mAddresses.begin(); it != mAddresses.end(); ++it)
	{
		l.push_back(Line());
		l.splice(l.end(), (*it)->GenerateInfo());
	}

	return l;
}


// Helper method, generates compact info about addresses (one line per address)
std::list<Line> BaseMuxDmux::GenerateAddressInfoCompact()
{
	Line line;
	std::list<Line> list;

	int counter = 0;

	for (auto it = mAddresses.begin(); it != mAddresses.end(); ++it, ++counter)
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


// Helper method, generates only addresses' statuses
std::list<Line> BaseMuxDmux::GenerateAddressStatus()
{
	std::list<Line> l;

	for (auto it = mAddresses.begin(); it != mAddresses.end(); ++it)
	{
		l.push_back(Line());
		l.splice(l.end(), (*it)->GenerateStatus());
	}

	return l;
}