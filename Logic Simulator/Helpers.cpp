#include "stdafx.h"
#include "PrivatePrintManager.h"
#include "ArrowPosition.h"


// Handle to the standard output (console)
// Origin: PrintManager.cpp
extern HANDLE Console;

// Mutex guard which should be locked whenever printing/changing cursor position is performed
// Origin: PrintManager.cpp
extern std::mutex ConsoleOwnership;

// Currently inputted command. Origin: FlowControl.cpp
extern std::string CurrentCommand;

// Position of the arrow in the part info sector. Origin: FlowControl.cpp
extern ArrowPosition PartInfoPosition;

// Number of lines printed by the currently presented part in the part info sector. Origin: NavigationRelated.cpp
extern int PrintedLines;



// Moves the cursor to the command line. By default doesn't lock the console
void CursorToCommandLine(bool lockConsoleOwnership)
{
	COORD c;

	// Position for arrow (command line row and first column + number of letters inputted)
	c.X = FIRST_COLUMN + CurrentCommand.length();
	c.Y = COMMAND_LINE_ROW;

	if (lockConsoleOwnership)
	{
		ConsoleOwnership.lock();
	}

	SetConsoleCursorPosition(Console, c);

	if (lockConsoleOwnership)
	{
		ConsoleOwnership.unlock();
	}
}


// Returns position of the console's cursor
COORD GetCursorPosition()
{
	CONSOLE_SCREEN_BUFFER_INFO info;

	if (GetConsoleScreenBufferInfo(Console, &info))
	{
		return info.dwCursorPosition;
	}
	else
	{
		// If operation wasn't successful return a 0,0 coord
		COORD c;

		c.X = 0;
		c.Y = 0;

		return c;
	}
}


// Prints the string array arr line by line.
// C is the starting positions in the console (X position will be the same for each row,
// Y will be incremented after each element) By default doesn't clear the lines before printing and doesn't lock the console
void PrintArray(std::list<Line> list, COORD& c, std::string clearingString, bool clearLines, bool lockConsole)
{
	if (lockConsole)
	{
		ConsoleOwnership.lock();
	}

	for (auto it = list.begin(); it != list.end(); ++it, ++c.Y)
	{
		if (clearLines)
		{
			SetConsoleCursorPosition(Console, c);
			std::cout<< clearingString;
		}

		// If the line is empty skip the iteration after clearing
		if (it->IsEmpty())
		{
			continue;
		}

		SetConsoleCursorPosition(Console, c);		

		it->Print();
	}

	if (lockConsole)
	{
		ConsoleOwnership.unlock();
	}
}


// Generates line with 3-space green or red rectangle
Line GenerateStatusLine(bool status, int indent)
{
	return Line("Status:    ", indent, 8, 3, status ? GREEN*BG : RED*BG);
}


// Prints 3-space green or red rectangle depending on the status. By default doesn't lock the console.
// Doesn't return the cursor to the command line
void PrintStatus(bool status, bool lockConsole)
{
	if (lockConsole)
	{
		ConsoleOwnership.lock();
	}

	// Position self in the current row and status column
	COORD c = GetCursorPosition();
	c.X = STATUS_COLUMN;

	SetConsoleCursorPosition(Console, c);	
	
	// Depending on the status set either green or red background
	SetConsoleTextAttribute(Console, status ? GREEN*BG : RED*BG);

	std::cout << "   ";

	// Return the normal color
	SetConsoleTextAttribute(Console, NORMAL_COLOR);

	if (lockConsole)
	{
		ConsoleOwnership.unlock();
	}
}


// Recalculates the index of the last possibly selected line.
void RecalculateMaxInfoLines()
{
	std::shared_ptr<BasePart> current;

	if(current=CurrentPart())
	{
		PartInfoPosition.SetLast(current->SelectableLines());
		PrintedLines = current->SelectableLines();
	}
	else
	{
		PartInfoPosition.SetLast(0);
		PrintedLines = 0;
	}
}