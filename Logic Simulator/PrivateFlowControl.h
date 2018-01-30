#pragma once
#include "stdafx.h"

// Moves down in the parts list / Moves down in the part's info /
// inputs the next command into line (or nothing if the chosen command was the latest one)
void DownArrowPressed();

// Moves up in the parts list / Moves up in the part's info / inputs the previous command into line
void UpArrowPressed();

// Switches between going through part list and command line
void TabPressed();

// Prints the key on the screen and adds it to the current command (if it's a letter/number/','
void DefaultKeyPressed(char c);

// If the current area is CommandLine executes the currently inputted command
void EnterPressed();

// If the current area is PartInfo executes the quick action of the currently selected line
void SpacePressed();

// Locks/unlocks the display of current part's info
void ShiftPressed();

// Removes the current part if the current sector is PartList
void DeletePressed();

// Handles backspaces - removes the last character in the current command if the current sector is the command line
// deletes the currently selected part if the current sector is PartList
void BackspacePressed();