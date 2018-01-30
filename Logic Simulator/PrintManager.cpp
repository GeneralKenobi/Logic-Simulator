#include "stdafx.h"
#include "PrivatePrintManager.h"
#include "ArrowPosition.h"


extern std::list<std::string> PastCommands;
extern std::list<std::shared_ptr<BasePart>> Parts;

extern ArrowPosition PartListPosition;

extern ArrowPosition PartInfoPosition;

extern bool Pause;

// Currently inputted command. Origin: FlowControl.cpp
extern std::string CurrentCommand;

// Mutex guard which should be locked whenever printing/changing cursor position is performed
std::mutex ConsoleOwnership;

// Handle to the standard output (console)
HANDLE Console = GetStdHandle(STD_OUTPUT_HANDLE);


// Manages printing requests, sector determines which screen area should be updated. Can be used with bitwise or
// to update 2 or more sectors
void PrintManager(Sector sector)
{
	if (sector & Sector::CommandLine)
	{
		RequestCommandLineUpdate();
	}

	if (sector & Sector::PartInfo)
	{
		RequestInfoUpdate();
	}

	if (sector & Sector::PartList)
	{
		RequestListUpdate();
	}
}


// Updates only the states in the given screen area. Passing Sector::CommandLine will not trigger any actions.
// Can be used with bitwise or to update 2 or more sectors
void PrintManagerStatus(Sector sector, int callerID)
{
	if (sector & Sector::PartInfo)
	{
		RequestInfoStatusUpdate(callerID);
	}

	if (sector & Sector::PartList)
	{
		RequestListStatusUpdate(callerID);
	}
}


// Prints the general UI shapes (column separations, row separations). Should be called when starting the program
void PrintUI()
{
	COORD c;
	c.X = 0;
	c.Y = 0;

	ConsoleOwnership.lock();

	// All separating lines are cyan
	SetConsoleTextAttribute(Console, CYAN);

	// Vertical line on the left of the screen
	for (int i = 0; i < 100; ++i, ++c.Y)
	{
		SetConsoleCursorPosition(Console, c);
		std::cout << '|';
	}

	c.X = SECOND_COLUMN - 7;
	c.Y = 0;

	// Vertical line separating command line/part list and part info
	for (int i = 0; i < 100; ++i, ++c.Y)
	{
		SetConsoleCursorPosition(Console, c);
		std::cout << '|';
	}

	c.X = 1;
	c.Y = PART_LIST_ROW - 2;

	SetConsoleCursorPosition(Console, c);

	// Horizontal line separating command line and part list
	std::cout << "________________________________________________________________________";

	c.Y = COMMAND_LINE_ROW - 1;

	SetConsoleCursorPosition(Console, c);

	// Horizontal line above command line input
	std::cout << "------------------------------------------------------------------------";

	c.Y = COMMAND_LINE_ROW + 1;

	SetConsoleCursorPosition(Console, c);

	// Horizontal line below command line input
	std::cout << "------------------------------------------------------------------------";

	// Return to normal color
	SetConsoleTextAttribute(Console, NORMAL_COLOR);

	ConsoleOwnership.unlock();
}

