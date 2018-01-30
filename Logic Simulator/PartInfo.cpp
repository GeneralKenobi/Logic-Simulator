#include "stdafx.h"
#include "PrivatePrintManager.h"
#include "Area.h"

// Mutex guard which should be locked whenever printing/changing cursor position is performed
// Origin: PrintManager.cpp
extern std::mutex ConsoleOwnership;

// Handle to the standard output (console)
// Origin: PrintManager.cpp
extern HANDLE Console;




// Call this function to request update of the whole part info sector
// The update can't be more frequent than once per MIN_STATUS_PERIOD (defined in PrintManager.h)
void RequestInfoUpdate()
{
	// Indicates whether an update has happened in the near past (MIN_REFRESH_PERIOD time interval)
	static bool justUpdated = false;

	// Indicates whether during the cooldown period new updates were requested
	static bool updateInQueue = false;

	auto currentPart = CurrentPart();

	if (!justUpdated && currentPart)
	{
		// Run a seperate thread
		std::thread t([currentPart]()
		{

			// Indicate that the info is currently/was just updated
			justUpdated = true;

			// Get the ownership of the console
			ConsoleOwnership.lock();

			// Set position in the second column at the top
			COORD c;
			c.X = SECOND_COLUMN;
			c.Y = 0;

			// Stored the number of lines printed previously
			static int previouslyPrintedLines = 0;

			// Get the array to print
			auto infoArray = currentPart->GenerateInfo();

			// Print it with clearing
			PrintArray(infoArray, c, SECOND_COLUMN_CLEARING_STRING, true);

			// If previously there were more lines printed than now
			if (infoArray.size() < previouslyPrintedLines)
			{
				// For all last lines
				for (int i = infoArray.size(); i < previouslyPrintedLines; ++i, ++c.Y)
				{
					// Clear them
					SetConsoleCursorPosition(Console, c);
					std::cout << SECOND_COLUMN_CLEARING_STRING;
				}
			}

			// And remember the number of lines printed now
			previouslyPrintedLines = infoArray.size();

			// Go back to command line
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
				RequestInfoUpdate();
			}
		});

		// Detach the thread (this function doesn't wait for it to end)
		t.detach();
	}
	else
	{
		// Otherwise schedule an update
		updateInQueue = true;
	}
}


// Call this function to request update of the statuses of the currently presented part
// The update can't be more frequent than once per MIN_STATUS_REFRESH_PERIOD (defined in PrintManager.h)
void RequestInfoStatusUpdate(int callerID)
{

	// Get the shard_ptr to the current part
	auto currentPart = CurrentPart();

	// Only currently presented part should be updated
	if (!currentPart || (currentPart && currentPart->ID() != callerID))
	{
		return;
	}

	// Indicates whether an update has happened in the near past (MIN_REFRESH_PERIOD time interval)
	static bool justUpdated = false;

	// Indicates whether during the cooldown period new updates were requested
	static bool updateInQueue = false;

	if (currentPart)
	{
		// Run a seperate thread
		std::thread t([=]()
		{

			// Indicate that the info is currently/was just updated
			justUpdated = true;

			// Get the ownership of the console
			ConsoleOwnership.lock();

			// Set position in the second column at the top
			COORD c;
			c.X = SECOND_COLUMN;
			c.Y = 0;

			SetConsoleCursorPosition(Console, c);
						
			// Get the array to print
			auto infoArray = currentPart->GenerateStatus();

			// Print it without clearing
			PrintArray(infoArray, c);

			// Go back to command line (because it's the default position for cursor)
			CursorToCommandLine();

			// Free the console ownership
			ConsoleOwnership.unlock();

			// And begin the cooldown period
			Sleep(MIN_STATUS_REFRESH_PERIOD);

			// Now indicate that new updates are possible
			justUpdated = false;

			// Additionally if there's an update scheduled
			if (updateInQueue)
			{
				// Clear the schedule
				updateInQueue = false;

				// And call request again
				RequestInfoStatusUpdate(callerID);
			}
		});

		// Detach the theead (this function call doesn't wait for it to end)
		t.detach();
	}
	else
	{
		// Otherwise schedule an update
		updateInQueue = true;
	}
}