#pragma once
#include "stdafx.h"


// Enum for different sections of UI
enum Sector
{
	PartList = 1,
	PartInfo = 2,
	CommandLine = 4,
};

inline Sector operator|(Sector a, Sector b) { return static_cast<Sector>(static_cast<int>(a) | static_cast<int>(b)); }

// Main loop controlling the program. Reads and interprets the input provided by the user
void MainLoop();

// Accessor to the currently presented part
std::shared_ptr<BasePart> CurrentPart();

// Accessor to the currently selected part
std::shared_ptr<BasePart> CurrentListPart();

// Performs all actions that need to be done before entering the main loop of the program
void Initialize();

// Accepts the given string as a quickly inputted command (overwrites the current one) and moves to the command line sector
void QuickActionCommand(std::string command);

// Attempts to create a part based on the given name (recognized names are given in Defines.h)
// If it wasn't successful returns an empty pointer
std::shared_ptr<BasePart> CreatePart(std::string name);

// Attempts to create a part based on the given part code (codes are given in Defines.h)
// If it wasn't successful returns an empty pointer
std::shared_ptr<BasePart> CreatePart(int code);