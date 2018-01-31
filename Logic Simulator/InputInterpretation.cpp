#include "stdafx.h"
#include "PrivatePrintManager.h"
#include "PrivateFlowControl.h"
#include "ArrowPosition.h"
#include "Area.h"
#include "PrivateCommands.h"


// Currently selected area. Origin: FlowControl.cpp
extern Area CurrentArea;

// Position of the arrow in the part list sector. Origin: FlowControl.cpp
extern ArrowPosition PartListPosition;

// Position of the arrow in the part info sector. Origin: FlowControl.cpp
extern ArrowPosition PartInfoPosition;

// List containing all parts. Origin: FlowControl.cpp
extern std::list<std::shared_ptr<BasePart>> Parts;

// List containing past commands. Origin: FlowControl.cpp
extern std::list<std::string> PastCommands;

// Iterator used to go through the past commands. Origin: FlowControl.cpp
extern std::list<std::string>::iterator PastCommandsIterator;

// Currently inputted command. Origin: FlowControl.cpp
extern std::string CurrentCommand;

// When true, currently presnted part in the info section is locked and won't be changed (but position on part list can change)
// Origin: FlowControl.cpp
extern bool LockInfo;

// Position of the locked part on part list. Origin: FlowControl.cpp
extern int LockedPosition;



// Moves down in the parts list / Moves down in the part's info /
// inputs the next command into line (or nothing if the chosen command was the latest one)
void DownArrowPressed()
{
	switch (CurrentArea.Get())
	{
	case Sector::PartList:
	{

		// Reset the position in the part info
		if(PartListPosition.Get() < Parts.size() - 1)
		{
			PartInfoPosition.Set(0);
			++PartListPosition;
		}
	}
	break;

	case Sector::PartInfo:
	{		
		++PartInfoPosition;		
	}
	break;

	case Sector::CommandLine:
	{
		// If the PastCommandsIterator isn't equal to the end iterator
		if (PastCommandsIterator != PastCommands.end())
		{
			// Increment it, reassign the current command and print it
			++PastCommandsIterator;

			// If the iterator is now the last iterator
			if (PastCommandsIterator == PastCommands.end())
			{
				// Clear the current command and input line
				CurrentCommand = "";
				PrintCurrentCommand(true);
				return;
			}
			else
			{
				// Otherwise fill the input line and command with the appropriate command
				CurrentCommand = *PastCommandsIterator;
				PrintCurrentCommand(true);
			}

		}
	}
	break;
	}
}


// Moves up in the parts list / Moves up in the part's info / inputs the previous command into line
void UpArrowPressed()
{
	switch (CurrentArea.Get())
	{
		case Sector::PartList:
		{		
			// Reset the position in the part info
			if(PartListPosition.Get()>0)
			{
				PartInfoPosition.Set(0);	
				--PartListPosition;		
			}
		}
		break;

		case Sector::PartInfo:
		{		
			--PartInfoPosition;		
		}
		break;

		case Sector::CommandLine:
		{
			// If we're not at the beginning yet (the eldest command)
			if (PastCommandsIterator != PastCommands.begin())
			{
				// Move the iterator
				--PastCommandsIterator;

				// Assign the command
				CurrentCommand = *PastCommandsIterator;

				// And refresh the input line
				PrintCurrentCommand(true);
			}
		}
		break;
	}
}


// Prints the key on the screen and adds it to the current command (if it's a letter/number/','
void DefaultKeyPressed(char c)
{
	if (c == ',' || c == ' ' || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
		(c >= '0' && c <= '9'))
	{
		CurrentCommand += c;
		PrintCurrentCommand(true);
	}
}


// If the current area is PartInfo executes the quick action of the currently selected line
void SpacePressed()
{
	switch (CurrentArea.Get())
	{
		case Sector::PartInfo:
		{
			if (!Parts.empty())
			{
				CurrentPart()->QuickActionRequest(PartInfoPosition.Get());
			}
		}
		break;

		case Sector::PartList:
		{
			if (!Parts.empty())
			{
				CurrentListPart()->QuickListActionRequest();
			}
		}
		break;

		case Sector::CommandLine:
		{
			DefaultKeyPressed(' ');
		}
		break;
	}	
}


// If the current area is CommandLine executes the currently inputted command
void EnterPressed()
{
	if (CurrentArea.Get() == Sector::CommandLine)
	{
		// If there are only spaces in the command erase it and don't do anything
		if (CurrentCommand.find_first_not_of(' ') == std::string::npos)
		{
			CurrentCommand.clear();
			std::cout << '\r';
		}
		else
		{
			// Otherwise parse the command
			if (ParseCommand(CurrentCommand))
			{
				// Add it to the history
				PastCommands.push_back(CurrentCommand);

				// If it was a correct command, add it to the list of all commands
			}

			CurrentCommand.clear();
			PrintManager(Sector::CommandLine | Sector::PartInfo | Sector::PartList);
		}

		PastCommandsIterator = PastCommands.end();
	}
}


// Locks/unlocks the display of current part's info
void ShiftPressed()
{
	if (CurrentArea.Get() == Sector::PartList)
	{
		if (LockInfo)
		{
			UnlockInfo();
		}
		else
		{
			if (!Parts.empty())
			{
				LockedPosition = PartListPosition.Get();
				LockInfo = !LockInfo;
			}
		}
	}
}


void UnlockInfo()
{
	if (PartListPosition.Get() != LockedPosition)
	{
		ClearLockedArrow(true);
		LockInfo = !LockInfo;
		PartListPosition.PositionChangedEvent(PartListPosition.Get(), PartListPosition.Get());
	}
	else
	{
		LockInfo = !LockInfo;
	}
}


// Removes the current part if the current sector is PartList
void DeletePressed()
{
	if (CurrentArea.Get() == Sector::PartList)
	{
		Parts.remove(CurrentPart());

		// Reset the position in the part info section
		PartInfoPosition.Set(0);

		// Make sure the PartListPosition doesn't exceed the size of the list after deleting the element
		if (PartListPosition.Get() >= Parts.size())
		{
			PartListPosition.Set(Parts.size() - 1);
		}
	}
}


// Handles backspaces - removes the last character in the current command if the current sector is the command line
// deletes the currently selected part if the current sector is PartList
void BackspacePressed()
{
	// If user is in the command line and the current command is not empty, delete the last character
	if (CurrentArea.Get() == Sector::CommandLine)
	{
		if (CurrentCommand.size() > 0)
		{
			CurrentCommand.erase(CurrentCommand.size() - 1, 1);
			PrintCurrentCommand(true);
		}
	}
	// Else if the user is in the PartList backspace is treated as a Delete
	else if (CurrentArea.Get() == Sector::PartList)
	{
		DeletePressed();
	}
}