#pragma once
#include "stdafx.h"


	
// Typedef for a command mapping - string is mapped to an int which is then used in switch statements
typedef std::pair<std::string, int> CmdMap;



// Connects two sockets (assuming the connection is allowed and arguments are valid)
// Arguments should be in a form of the following string: "a,b" where a and b are IDs of
// the sockets that are to be connected
void ConnectCommand(std::string args);

// Attempts to connect 2 sockets, throws an exception if it wasn't possible (socket wasn't found, both sockets
// are of the same type)
void ConnectCommand(int id1, int id2);

// Attempts to load a saved custom logic from the hard drive. Throws an exception if something went wrong.
// Argument is the name of the logic to load
void LoadCommand(std::string arg);

// Creates a new part. Argument should be a string with the name of the part
void CreateCommand(std::string arg);

// Deletes a part given by the id in arg.
void DeleteCommand(std::string arg);

// Unlocks the currently locked part.
void UnlockInfo();

// Sets the period of a clock. Args should be 2 numbers: "id_of_the_clock, new_period_in_ms"
void ClockPeriodCommand(std::string args);

// Returns standardized version of command (lowercase, trimmed, multiple spaces removed)
std::string StandardizeCommand(std::string command);

// Tries to save the current circuit as a custom part
void SaveCommand(std::string partName);

// Parses a string saved in custom logic that contains pairs of sockets to connect separated by a ';'
// Ex: "1,2;3,7;9,13"
// Throws exception if there were any problems with parsing the string
std::list<std::pair<int, int>> ParseConnections(std::string connections);

// Empties the buffer and generates a part for each string extracted from the stream using >>
// If something goes wrong, throws an exception
std::list<std::pair<std::shared_ptr<BasePart>, std::string>> CreatePartsForCustomLogic(std::stringstream& buffer);

// Checks if there's no collision (file with the given name doesn't exist). Throws an exception if it exists (user-friendly)
void FileColisionCheck(std::string fileName);

// Deletes a given custom logic from the disc. Returns true if the file was deleted succesfully
bool DeleteCustomLogicCommand(std::string arg);