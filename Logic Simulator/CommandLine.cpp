#include "stdafx.h"
#include "PrivatePrintManager.h"


// Mutex guard which should be locked whenever printing/changing cursor position is performed
// Origin: PrintManager.cpp
extern std::mutex ConsoleOwnership;

// Handle to the standard output (console). Origin: PrintManager.cpp
extern HANDLE Console;

// Currently inputted command. Origin: FlowControl.cpp
extern std::string CurrentCommand;

// List containing past commands. Origin: FlowControl.cpp
extern std::list<std::string> PastCommands;

// Current error message to be displayed.
std::string ErrorMessage;


// Requests update of the command line. Refreshes previous commands, current command and error message
void RequestCommandLineUpdate()
{
	// Acquire the ownership of the console
	ConsoleOwnership.lock();

	PrintLastComands();

	PrintError();

	PrintCurrentCommand();

	// Free the console
	ConsoleOwnership.unlock();
}


// Prints the previous commands (max 10). By default doesn't lock the console. Doesn't return the cursor
// to the command line
void PrintLastComands(bool lockConsole)
{
	// Get the iterator from the past commands list
	auto it = PastCommands.begin();

	// If there are more than 10 commands, advance it to the 10th position from the end
	if (PastCommands.size() > 10)
	{
		std::advance(it, PastCommands.size() - 10);
	}

	// Coord set at the first column 2 rows above command line
	COORD c;

	c.X = FIRST_COLUMN;
	c.Y = COMMAND_LINE_ROW - 2;

	if (lockConsole)
	{
		ConsoleOwnership.lock();
	}

	// If there are more than 10 commands
	if (PastCommands.size() >= 10)
	{
		// Set the position one row above the last command
		c.Y = COMMAND_LINE_ROW - 12;

		SetConsoleCursorPosition(Console, c);

		std::cout << "..." << std::endl;

		// Move to the last command's position
		++c.Y;
	}
	else
	{
		// Otherwise position self in the last command's position
		c.Y = COMMAND_LINE_ROW - 1 - PastCommands.size();
	}

	// For each of the commands that are to be printed
	for (it; it != PastCommands.end(); ++it, ++c.Y)
	{
		// Set the position
		SetConsoleCursorPosition(Console, c);

		// Clear the previous command
		std::cout << "                                                                 ";

		// Go back to the beginning of the line
		SetConsoleCursorPosition(Console, c);

		// Print the command
		std::cout << *it;
	}

	if (lockConsole)
	{
		ConsoleOwnership.unlock();
	}
}


// Clears the old text and prints the current command in the command line. By default doesn't lock the console
void PrintCurrentCommand(bool lockConsole)
{
	COORD c;

	// Position self in the command line
	c.X = FIRST_COLUMN;
	c.Y = COMMAND_LINE_ROW;

	if (lockConsole)
	{
		ConsoleOwnership.lock();
	}

	SetConsoleCursorPosition(Console, c);

	// Clear the previous command
	std::cout << "                                             ";

	// Go back to the command line position
	SetConsoleCursorPosition(Console, c);

	// Print the current command
	std::cout << CurrentCommand;

	if (lockConsole)
	{
		ConsoleOwnership.unlock();
	}
}


// Helper for PrintLastComands, if ErrorMessage is empty doesn't do anything.
// Error is printed below the input command line. Doesn't return the cursor
// to the command line. By default doens't lock the console
void PrintError(bool lockConsole)
{

	COORD c;

	// Position self 2 rows below the command line
	c.X = FIRST_COLUMN;
	c.Y = COMMAND_LINE_ROW + 2;

	if (lockConsole)
	{
		ConsoleOwnership.lock();
	}

	SetConsoleCursorPosition(Console, c);

	// Clear the previous error
	std::cout << FIRST_COLUMN_CLEARING_STRING;

	// If the error message is empty, don't do anything else
	if (!ErrorMessage.empty())
	{
		SetConsoleCursorPosition(Console, c);

		// Print the error
		std::cout << ErrorMessage;
	}

	if (lockConsole)
	{
		ConsoleOwnership.unlock();
	}
}


// Logs and displayes an error message (until new command is inputted)
void LogError(std::string error)
{
	ErrorMessage = error;
}