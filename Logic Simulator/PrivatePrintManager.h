#pragma once
#include "stdafx.h"

// Prints the previous commands (max 10). By default doesn't lock the console
void PrintLastComands(bool lockConsole = false);

// Prints all parts in the PartsList
void PrintPartsList(bool lockConsole = false);

// Call this function to request update of a single status in the parts list
// The update can't be more frequent than once per MIN_STATUS_REFRESH_PERIOD (defined at the top of the file)
// If caller ID is smaller than 0 printing will still be done if there are queued updates
void RequestListStatusUpdate(int callerID);

// Call this function to request update of the statuses of the currently presented part
// The update can't be more frequent than once per MIN_STATUS_REFRESH_PERIOD (defined in PrintManager.h)
void RequestInfoStatusUpdate(int callerID);

// Call this function to request update of the whole parts list
// The update can't be more frequent than once per MIN_REFRESH_PERIOD (defined in PrintManager.h)
void RequestListUpdate();

// Call this function to request update of the whole part info sector
// The update can't be more frequent than once per MIN_STATUS_PERIOD (defined in PrintManager.h)
void RequestInfoUpdate();

// Requests update of the command line. Refreshes previous commands, current command and error message
void RequestCommandLineUpdate();

// Helper for PrintLastComands, if ErrorMessage is empty doesn't do anything.
// Error is printed below the input command line. Doesn't return the cursor
// to the command line. By default doens't lock the console
void PrintError(bool lockConsole = false);

// Removes the locked arrow from the screen
void ClearLockedArrow(bool lockConsole = false);