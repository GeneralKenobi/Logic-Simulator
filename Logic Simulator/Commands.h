#pragma once
#include "stdafx.h"

// Parses a command and takes appropriate action. Returns true if the command was proper, false otherwise
bool ParseCommand(std::string command);

// Swaps all occurances of oldID with newID
void SwapID(std::list<std::pair<int, int>>& list, int oldID, int newID);