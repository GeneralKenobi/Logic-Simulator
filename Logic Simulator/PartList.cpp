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

// List containing all parts. Origin: FlowControl.cpp
extern std::list<std::shared_ptr<BasePart>> Parts;


// Call this function to request update of the whole parts list
// The update can't be more frequent than once per MIN_REFRESH_PERIOD (defined in PrintManager.h)
void RequestListUpdate()
{
	// Indicates whether an update has happened in the near past (MIN_REFRESH_PERIOD time interval)
	static bool justUpdated = false;

	// Indicates whether during the cooldown period new updates were requested
	static bool updateInQueue = false;

	if (!justUpdated)
	{
		// Run a seperate thread
		std::thread t([]()
		{
			// Indicate that the list is currently/was just updated
			justUpdated = true;

			// Get the ownership of the console
			ConsoleOwnership.lock();			

			// Print the parts
			PrintPartsList();

			// Go back to command line (because it's the default position for cursor)
			CursorToCommandLine();

			// Free the console ownership
			ConsoleOwnership.unlock();

			// And begin the cooldown period
			Sleep(MIN_REFRESH_PERIOD);

			// Now indicate that new updates are possible
			justUpdated = false;

			// Additionally if there's an update scheduled
			if (updateInQueue)
			{
				// Clear the schedule
				updateInQueue = false;

				// And call request again
				RequestListUpdate();
			}
		});

		// Detach the thead (this function doesn't wait for it to end)
		t.detach();
	}
	else
	{
		// Otherwise schedule an update
		updateInQueue = true;
	}
}


// Call this function to request update of a single status in the parts list
// The update can't be more frequent than once per MIN_STATUS_REFRESH_PERIOD (defined at the top of the file)
// If caller ID is smaller than 0 printing will still be done if there are queued updates
void RequestListStatusUpdate(int callerID)
{
	// Indicates whether an update has happened in the near past (MIN_REFRESH_PERIOD time interval)
	static bool justUpdated = false;

	// Indicates whether during the cooldown period new updates were requested
	static std::set<int> updatesInQueue;

	// Guard for the updatesInQueue container
	static std::mutex updatesQueueMutex;

	updatesQueueMutex.lock();

	if (callerID >= 0)
	{
		updatesInQueue.emplace(callerID);
	}

	updatesQueueMutex.unlock();


	if (!justUpdated)
	{
		// Run a seperate thread
		std::thread t([=]()
		{
			// Indicate that the list is currently/was just updated
			justUpdated = true;

			// Here cooldown is done before the update in order to synch parts that are updated
			// simultaneously 
			Sleep(MIN_STATUS_REFRESH_PERIOD);

			// Get the ownership of the console
			ConsoleOwnership.lock();

			// Set position in the first column, PART_LIST_ROW
			COORD c;
			c.X = FIRST_COLUMN;
			c.Y = PART_LIST_ROW;

			updatesQueueMutex.lock();

			// Print the parts
			for (auto it = Parts.begin(); it != Parts.end(); ++it, ++c.Y)
			{
				if (updatesInQueue.find((*it)->ID()) != updatesInQueue.end())
				{
					SetConsoleCursorPosition(Console, c);
					(*it)->PrintList();
				}
			}

			updatesInQueue.clear();

			updatesQueueMutex.unlock();

			CursorToCommandLine();

			// Free the console ownership
			ConsoleOwnership.unlock();

			// Now indicate that new updates are possible
			justUpdated = false;

			// Additionally if there's an update scheduled
			if (!updatesInQueue.empty())
			{
				// Call request again
				RequestListStatusUpdate(-1);
			}
		});

		// Detach the thead (this function doesn't wait for it to end)
		t.detach();
	}
}


// Helper function printing the whole part list from the beginning
void PrintPartsList(bool lockConsole)
{
	static int PreviouslyPrintedLines = 0;

	COORD c;

	c.X = FIRST_COLUMN;
	c.Y = PART_LIST_ROW;

	// int storing the number of lines printed
	int i = 0;

	if (lockConsole)
	{
		ConsoleOwnership.lock();
	}

	// For each part
	for (auto it = Parts.begin(); it != Parts.end(); ++it, ++i, ++c.Y)
	{
		SetConsoleCursorPosition(Console, c);

		// Clear the line
		std::cout<< FIRST_COLUMN_CLEARING_STRING;

		SetConsoleCursorPosition(Console, c);

		// Print the info
		(*it)->PrintList();
	}

	// If this time there were less lines than previously
	if (i != PreviouslyPrintedLines)
	{
		// Keep going until the number of lines is matched
		for (int j = i; j < PreviouslyPrintedLines; ++j, ++c.Y)
		{
			SetConsoleCursorPosition(Console, c);

			// And clear each additional line
			std::cout << FIRST_COLUMN_CLEARING_STRING;
		}
	}

	if (lockConsole)
	{
		ConsoleOwnership.unlock();
	}
	
	// Remember the number of printed lines
	PreviouslyPrintedLines = i;
}