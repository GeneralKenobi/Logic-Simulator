#include "stdafx.h"


#define SELECTABLE_LINES 11



// Default Constructor
ButtonInput::ButtonInput()
{
	mPrintName = "Button";
}


// Prints the compact information on this part (dedicated to the list of parts)
void ButtonInput::PrintList()
{
	BaseInput::PrintList();
	std::cout << "\t(QA) Press";
}


// Returns a list with lines that compse a full info on this part.
// String is the line to print, int is the indentation (x offset) of the line
// Should be overriden to include info about the inherited class.
std::list<Line> ButtonInput::GenerateInfo()
{
	std::list<Line> l = BasePart::GenerateInfo();

	l.push_back(Line("On duration: " + std::to_string(mPeriod)));
	l.push_back(Line("Output:"));
	l.push_back(Line());
	l.splice(l.end(), mOutput->GenerateInfo());
	l.push_back(Line());
	l.push_back(Line("(QA) Press"));

	return l;
}


// Returns status lines for the part which has the same structure as list generated by GenerateInfo.
// All lines that don't carry information about status are empty
std::list<Line> ButtonInput::GenerateStatus()
{
	std::list<Line> l = BasePart::GenerateStatus();

	l.push_back(Line());
	l.push_back(Line());
	l.push_back(Line());
	l.splice(l.end(), mOutput->GenerateStatus());
	l.push_back(Line());
	l.push_back(Line());

	return l;
}


// Returns the numberof selectable lines in this part's info
int ButtonInput::SelectableLines()
{
	return SELECTABLE_LINES;
}


// Performs quick action for the given line (if there's an action to perform)
void ButtonInput::QuickActionRequest(int index)
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
		
		case 7:
		{
			mOutput->QuickAction();
		}
		break;

		case 10:
		{
			SetState(true);
			Sleep(mPeriod);
			SetState(false);
		}
		break;
	}
}


// Performs quick list action
void ButtonInput::QuickListActionRequest()
{
	SetState(true);
	Sleep(mPeriod);
	SetState(false);
}


// Generates string to save in a file which allows to recreate this part
// Structure: type_number|is on|is custom logic input
std::vector<std::string> ButtonInput::GenerateStorageString()
{
	std::string firstPart = std::to_string(BUTTON_INPUT);

	std::string secondPart = std::to_string(mPeriod) + ";" + std::to_string(-ID()) + ";" + std::to_string(-mOutput->ID());	

	return std::vector<std::string> {"", firstPart, secondPart};
}

void ButtonInput::InterpretStorageString(std::string settings, std::list<std::pair<int, int>>& connections)
{
	auto v = Split(settings, ';');

	if (v.size() != 3)
	{
		throw Exception("Data is corrupted");
	}

	try
	{
		mPeriod = std::stoi(v[0]);
		SwapID(connections, std::stoi(v[1]), ID());
		SwapID(connections, std::stoi(v[2]), mOutput->ID());
	}
	catch (std::exception e)
	{
		throw Exception("Data is corrupted");
	}

}
