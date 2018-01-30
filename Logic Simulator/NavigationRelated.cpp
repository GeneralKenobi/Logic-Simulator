#include "stdafx.h"
#include "PrivatePrintManager.h"
#include "ArrowPosition.h"
#include "Area.h"

// Mutex guard which should be locked whenever printing/changing cursor position is performed
// Origin: PrintManager.cpp
extern std::mutex ConsoleOwnership;

// Handle to the standard output (console)
// Origin: PrintManager.cpp
extern HANDLE Console;

// Currently selected area
extern Area CurrentArea;

// Position of the arrow in the part info sector. Origin: FlowControl.cpp
extern ArrowPosition PartInfoPosition;

// When true, currently presnted part in the info section is locked and won't be changed (but position on part list can change)
// Origin: FlowControl.cpp
extern bool LockInfo;

// Position of the locked part on part list. Origin: FlowControl.cpp
extern int LockedPosition;

// Number of lines printed by the currently presented part in the part info sector
int PrintedLines = 0;

// Position of the arrow in the part list sector. Used when current sector is changed
int PartListArrowPosition = 0;

// Position of the arrow in the part info sector. Used when current sector is changed
int PartInfoArrowPosition = 0;


// Callback for when arrow position in the part list changes. Removes the old arrow and prints a new one.
// Requests printing of the new part's info
void PartListPositionChanged(int oldPosition, int newPosition)
{
	// Save the new position
	PartListArrowPosition = newPosition;

	// Obtain ownership of console
	ConsoleOwnership.lock();

	// If the info part is locked, do not reprint the part
	if(!LockInfo)
	{
		// And request printing of the new part
		PrintManager(Sector::PartInfo);

		// Set the new limit for selectable lines in info part
		PartInfoPosition.SetLast(CurrentPart()->SelectableLines());

		// And remember how many lines were printed
		PrintedLines = CurrentPart()->SelectableLines();
	}
	COORD c;

	// Position self in the first column old arrow position
	c.X = FIRST_COLUMN_ARROW;
	c.Y = PART_LIST_ROW + oldPosition;

	SetConsoleCursorPosition(Console, c);

	// Clear the old arrow
	std::cout << "   ";
	
	// Position self in the new arrow's row
	c.Y = PART_LIST_ROW + newPosition;

	SetConsoleCursorPosition(Console, c);

	// If the part list is selected, change font to yellow, gray otherwise
	SetConsoleTextAttribute(Console, CurrentArea.Get() == Sector::PartList ? YELLOW : GRAY);
	
	std::cout << "-->";

	// If there's a locked part
	if (LockInfo && PartListArrowPosition != LockedPosition)
	{
		// Go to its row
		c.Y = PART_LIST_ROW + LockedPosition;

		SetConsoleCursorPosition(Console, c);

		// Set the color to dark yellow
		SetConsoleTextAttribute(Console, DARK_YELLOW);

		// And print an arrow indicating that it's locked
		std::cout << "-->";
	}

	// Revert the color
	SetConsoleTextAttribute(Console, NORMAL_COLOR);

	// Put the cursor back in command line
	CursorToCommandLine();

	// Free the console
	ConsoleOwnership.unlock();	
}

// Callback for when arrow position in the info part changes. Removes the old arrow and prints a new one
void PartInfoPositionChanged(int oldPosition, int newPosition)
{
	// Remember the new position
	PartInfoArrowPosition = newPosition;
	
	COORD c;

	// Position self in the row of the old arrow
	c.X = SECOND_COLUMN_ARROW;
	c.Y = oldPosition;

	// Lock the console
	ConsoleOwnership.lock();

	SetConsoleCursorPosition(Console, c);

	// Clear the old arrow
	std::cout << "   ";

	// Position self in the row of the new arrow
	c.Y = newPosition;

	SetConsoleCursorPosition(Console, c);	

	// If the part info is selected, change font to yellow, gray otherwise
	SetConsoleTextAttribute(Console, CurrentArea.Get() == Sector::PartInfo ? YELLOW : GRAY);

	std::cout << "-->";

	// Go back to normal color
	SetConsoleTextAttribute(Console, NORMAL_COLOR);

	// Return cursor to command line
	CursorToCommandLine();

	// Free the console
	ConsoleOwnership.unlock();
}


// Reprints the arrows (doesn't change their position). Should be used when the selected area has changed
// in order to update the colors of arrows. Locks the console
void PrintArrows(Sector newSector)
{
	COORD c;
	
	// Position of command line arrow
	c.X = FIRST_COLUMN_ARROW;
	c.Y = COMMAND_LINE_ROW;

	ConsoleOwnership.lock();

	// Enter the position
	SetConsoleCursorPosition(Console, c);

	// Set text to either yellow or gray
	SetConsoleTextAttribute(Console, CurrentArea.Get() ==Sector::CommandLine ? YELLOW : GRAY);

	std::cout<<"-->";

	// Position of part list arrow (same X, different Y)
	c.Y = PART_LIST_ROW + PartListArrowPosition;

	// Enter the position
	SetConsoleCursorPosition(Console, c);

	// If in PartList, set to Yellow, if not: if(LockInfo and locked and normal arrow are in the same position), dark yellow,
	// gray otherwise
	SetConsoleTextAttribute(Console, CurrentArea.Get() == Sector::PartList ? YELLOW : 
		((LockInfo && LockedPosition==PartListArrowPosition) ? DARK_YELLOW : GRAY));

	std::cout << "-->";

	// Position of part info arrow
	c.X = SECOND_COLUMN_ARROW;
	c.Y = PartInfoArrowPosition;

	// Enter the position
	SetConsoleCursorPosition(Console, c);

	// Set text to either yellow or gray
	SetConsoleTextAttribute(Console, CurrentArea.Get() == Sector::PartInfo ? YELLOW : GRAY);

	std::cout << "-->";

	// Return to normal color
	SetConsoleTextAttribute(Console, NORMAL_COLOR);

	// Put the cursor back in the command line
	CursorToCommandLine();

	// And free the console
	ConsoleOwnership.unlock();
}


// Cleans information printed by the last part. By default doesn't lock the console
void ClearPreviousPartInfo(bool lockConsole)
{
	if (lockConsole)
	{
		ConsoleOwnership.lock();
	}

	COORD c;
	c.X = SECOND_COLUMN;
	c.Y = 0;

	// Print empty lines depending on the saved value of PrintedValues
	for (int i = 0; i < PrintedLines; ++i, ++c.Y)
	{
		SetConsoleCursorPosition(Console, c);
		std::cout << SECOND_COLUMN_CLEARING_STRING;
	}

	if (lockConsole)
	{
		ConsoleOwnership.unlock();
	}
}


// Removes the locked arrow from the screen
void ClearLockedArrow(bool lockConsole)
{
	if (lockConsole)
	{
		ConsoleOwnership.lock();
	}

	COORD c;
	c.X = FIRST_COLUMN_ARROW;
	c.Y = PART_LIST_ROW + LockedPosition;

	SetConsoleCursorPosition(Console, c);

	std::cout<<"   ";

	if (lockConsole)
	{
		ConsoleOwnership.unlock();
	}
}