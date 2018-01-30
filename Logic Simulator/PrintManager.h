#pragma once
#include "stdafx.h"



#pragma region Screen division and printing constants defines

#define FIRST_COLUMN 5
#define FIRST_COLUMN_ARROW 1
#define SECOND_COLUMN 80
#define SECOND_COLUMN_ARROW 76
#define STATUS_COLUMN 40
#define PART_LIST_ROW 18
#define COMMAND_LINE_ROW 13
#define INDENT 4

#pragma endregion

#pragma region Time constants defines

#define MIN_REFRESH_PERIOD 200					// ms
#define MIN_STATUS_REFRESH_PERIOD 50			// ms

#pragma endregion

#pragma region Color defines

#define DARK_YELLOW 6
#define NORMAL_COLOR 7
#define HIGHLIGHT_COLOR 8
#define BG 16
#define GRAY 8
#define BLUE 9
#define GREEN 10
#define CYAN 11
#define RED 12
#define PINK 13
#define YELLOW 14
#define WHITE 15

#pragma endregion

#define FIRST_COLUMN_CLEARING_STRING "                                                                    "
#define SECOND_COLUMN_CLEARING_STRING "                                                                                                   "



// Call to request a refresh of a whole sector. Can be used with bitwise or
// to update 2 or more sectors
void PrintManager(Sector sector);

// Updates only the states in the given screen area. Passing Sector::CommandLine will not trigger any actions.
// Can be used with bitwise or to update 2 or more sectors
void PrintManagerStatus(Sector sector, int callerID);

// Prints the general UI shapes (column separations, row separations). Should be called when starting the program
void PrintUI();

// Clears the old text and prints the current command in the command line. By default doesn't lock the console
void PrintCurrentCommand(bool lockConsole = false);

// Logs and displayes an error message (until new command is inputted)
void LogError(std::string error);

// Prints the string array arr line by line.
// C is the starting positions in the console (X position will be the same for each row,
// Y will be incremented after each element) By default doesn't clear the lines before printing and doesn't lock the console
void PrintArray(std::list<Line> list, COORD& c, std::string clearingString = "", bool clearLines = false, bool lockConsole = false);

// Prints 3-space green or rd rectangle depending on the status. By default doesn't lock the console
// Doesn't return the cursor to the command line
void PrintStatus(bool status, bool lockConsole = false);

// Generates line with 3-space green or red rectangle
Line GenerateStatusLine(bool status, int indent = 0);

// Returns position of the console's cursor
COORD GetCursorPosition();

// Reprints the arrows (doesn't change their position). Should be used when the selected area has changed
// in order to update the colors of arrows. Locks the console
void PrintArrows(Sector newSector);

// Moves the cursor to the command line. By default doesn't lock the console
void CursorToCommandLine(bool lockConsoleOwnership = false);

// Callback for when arrow position in the part list changes. Removes the old arrow and prints a new one
// Requests printing of the new part's info
void PartListPositionChanged(int oldPosition, int newPosition);

// Callback for when arrow position in the info part changes. Removes the old arrow and prints a new one
void PartInfoPositionChanged(int oldPosition, int newPosition);

// Recalculates the index of the last possibly selected line.
void RecalculateMaxInfoLines();

// Cleans information printed by the last part. By default doesn't lock the console
void ClearPreviousPartInfo(bool lockConsole = false);