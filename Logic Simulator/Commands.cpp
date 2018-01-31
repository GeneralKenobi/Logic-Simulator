#include "stdafx.h"
#include "PrivateCommands.h"
#include "ArrowPosition.h"



// List containing all parts. Origin: FlowControl.cpp
extern std::list<std::shared_ptr<BasePart>> Parts;

// Position of the arrow in the part list sector
extern ArrowPosition PartListPosition;

// Position of the arrow in the part info sector
extern ArrowPosition PartInfoPosition;

// When true, currently presnted part in the info section is locked and won't be changed
// (but position on part list can change). Origin: FlowControl.cpp
extern bool LockInfo;

// Position of the locked part on part list. Origin: FlowControl.cpp
extern int LockedPosition;



// Creates a new part. Argument should be a string with the name of the part
void CreateCommand(std::string arg)
{
	auto part = CreatePart(arg);

	if(part)
	{
		Parts.push_back(part);

		// If it was the first part added
		if (Parts.size() == 1)
		{
			// Set the number of possible selections in the part info section
			// (because currently it's zero and the part list position doesn't change when adding the 1st part)
			RecalculateMaxInfoLines();
		}

		// Reassign the maximum position in the parts list
		PartListPosition.SetLast(Parts.size());
	}
	else
	{
		// Error: part name wasn't recogninzed (can't add the part)
		throw Exception("Unrecognized part name: \"" + arg + "\"");
	}
}


// Connects two sockets (assuming the connection is allowed and arguments are valid)
// Arguments should be in a form of the following string: "a,b" where a and b are IDs of
// the sockets that are to be connected
void ConnectCommand(std::string args)
{
	// Find the comma (separator of arguments)
	int comma = args.find(',');

	// If it was found
	if (comma == std::string::npos)
	{
		throw Exception("Connect requires 2 arguments (\"Connect 2,4\"");
	}

	// Extract both arguments from the string and convert them to int
	int firstArgument = -1, secondArgument = -1;

	try
	{
		firstArgument = std::stoi(args.substr(0, comma));
		secondArgument = std::stoi(args.substr(comma + 1));
	}
	catch (std::exception e) 
	{
		// If conversion wasn't successful
		throw Exception("Incorrect arguments: \""+args+"\". (Hint: 1,2)");
	}
	 
	ConnectCommand(firstArgument, secondArgument);
}

// Attempts to connect 2 sockets, throws an exception if it wasn't possible (socket wasn't found, both sockets
// are of the same type)
void ConnectCommand(int id1, int id2)
{
	std::shared_ptr<InputSocket> inputSocket;
	std::shared_ptr<OutputSocket> outputSocket;

	// Try to get the input and output socket
	if ((Identifiable::TryGetIDOwner<InputSocket>(id1, inputSocket)
		&& Identifiable::TryGetIDOwner<OutputSocket>(id2, outputSocket)) ||
		(Identifiable::TryGetIDOwner<InputSocket>(id2, inputSocket)
			&& Identifiable::TryGetIDOwner<OutputSocket>(id1, outputSocket)))
	{
		// If we managed to do it, assign the output's node to the input socket
		inputSocket->AssignedNode(outputSocket->mAssignedNode);
	}
	else
	{
		throw Exception("Couldn't connect sockets: " + std::to_string(id1) +
			", " + std::to_string(id2));
	}
}


// Deletes a part given by the id in arg.
void DeleteCommand(std::string arg)
{
	int id = -1;

	try
	{
		id = std::stoi(arg);
	}
	catch (std::exception)
	{
		throw Exception("Incorrect argument: \"" + arg + "\". (Hint: it should be the ID of the part to delete)");
	}

	std::shared_ptr<BasePart> owner;

	if (Identifiable::TryGetIDOwner(id, owner))
	{
		// Reset the position in the part info section
		PartInfoPosition.Set(0);

		// Make sure the PartListPosition doesn't exceed the size of the list after deleting the element
		if (PartListPosition.Get() == Parts.size() - 1)
		{
			PartListPosition.Set(Parts.size() - 2);
		}

		if (LockInfo && CurrentPart() == owner)
		{
			UnlockInfo();
		}

		PartListPosition.SetLast(Parts.size() - 1);

		Parts.remove(owner);

		if (Parts.empty())
		{
			ClearPreviousPartInfo(true);
		}

		RecalculateMaxInfoLines();

		PrintArrows(Sector::PartList);

		owner.reset();
	}
	else
	{
		throw Exception("Can't find part with ID: " + arg + ".");
	}	
}


// Sets the period of a clock. Args should be 2 numbers: "id_of_the_clock, new_period_in_ms"
void ClockPeriodCommand(std::string args)
{
	// Find the comma (separator of arguments)
	int comma = args.find(',');

	// If it was found
	if (comma == std::string::npos)
	{
		throw Exception("ClockPeriod requires 2 arguments (\"Connect 2,4\")");
	}

	int firstArgument = -1, secondArgument = -1;

	// Extract both arguments from the string and convert them to int
	try
	{
		firstArgument = std::stoi(args.substr(0, comma));
		secondArgument = std::stoi(args.substr(comma + 1));
	}
	catch (std::exception e)
	{
		// If conversion wasn't successful, throw an exception with an error message
		throw Exception("Incorrect arguments: \"" + args + "\". (Hint: 1,700)");
	}

	std::shared_ptr<ClockInput> clock;

	// Try to get the shared_ptr to the clock
	if (Identifiable::TryGetIDOwner<ClockInput>(firstArgument, clock))
	{
		// Set the new period
		clock->SetPeriod(secondArgument);

		// Get the current part
		auto current = CurrentPart();

		// If the clock is also currently printed in the part info section
		if(current && clock->ID()==current->ID())
		{
			// Refresh that section in order to print new period
			PrintManager(Sector::PartInfo);
		}
	}
	else
	{
		// If there was no part with such id or it wasn't a clock throw an exception with an error message
		throw Exception("Part with ID " + std::to_string(firstArgument) + " doesn't exist or it's not a clock");
	}
}


// Parses a string saved in custom logic that contains pairs of sockets to connect separated by a ';'
// Ex: "1,2;3,7;9,13"
// Throws exception if there were any problems with parsing the string
std::list<std::pair<int, int>> ParseConnections(std::string connections)
{
	std::list<std::pair<int, int>> result;

	if (connections == "null")
	{
		return result;
	}

	// Split the string based on ';'
	auto splitConnections = Split(connections, ';');

	// For each resulting substring
	for (int i = 0; i < splitConnections.size(); ++i)
	{
		if (Trim(splitConnections[i]).empty())
		{
			continue;
		}

		// Split the pair of items on comma
		auto ids = Split(splitConnections[i], ',');

		if (ids.size() != 2)
		{
			throw Exception("File is corrupted");
		}

		try
		{
			result.push_back(std::make_pair(std::stoi(ids[0]), std::stoi(ids[1])));
		}
		catch (std::exception e)
		{
			throw Exception("File is corrupted");
		}
	}

	return result;
}


// Empties the buffer and generates a part for each string extracted from the stream using >>
// If something goes wrong, throws an exception
std::list<std::pair<std::shared_ptr<BasePart>, std::string>> CreatePartsForCustomLogic(std::stringstream& buffer)
{
	std::list<std::pair<std::shared_ptr<BasePart>, std::string>> result;

	// Go on until the end of stream
	while (!buffer.eof())
	{
		std::string s;

		// Get the next line
		buffer >> s;

		// Split the string on 2 separate parts: part code|part info
		auto v = Split(s, '|');

		if (v.size() != 2)
		{
			throw Exception("File is corrupted");
		}

		int partCode = -1;

		// Try to get the part's code
		try
		{
			partCode = std::stoi(v[0]);
		}
		catch (std::exception e)
		{
			throw Exception("File is corrupted");
		}

		// Create the part
		auto part = CreatePart(partCode);

		// If failure
		if (!part)
		{
			throw Exception("File is corrupted");
		}

		result.push_back(std::make_pair(part, v[1]));
	}

	return result;
}


// Attempts to load a saved custom logic from the hard drive. Throws an exception if something went wrong.
// Argument is the name of the logic to load
void LoadCommand(std::string arg)
{
	// Try to open the given file
	std::ifstream f("saves/" + arg + ".txt", std::ios::in);

	if (!f.is_open())
	{
		throw Exception("No record of: " + arg);
	}

	std::stringstream buffer;

	// Transfer the contents to a stringstream
	buffer<<f.rdbuf();

	std::string connections;

	// Extract the connections which are stored on the top of the file
	buffer >> connections;

	std::list<std::pair<int, int>> connectionArchive = ParseConnections(connections);
	
	// Extract the rest of the buffer and create parts base on that
	std::list<std::pair<std::shared_ptr<BasePart>, std::string>> customLogicParts = CreatePartsForCustomLogic(buffer);
	
	// List of parts that have already been set to the saved values and that have altered the old IDs in connectionArchive
	std::list<std::shared_ptr<BasePart>> readyParts;

	// For each part
	for (auto it = customLogicParts.begin(); it != customLogicParts.end(); ++it)
	{
		// Interpret the storage string - adapt the saved settings and alter the connectionArchive with current IDs
		it->first->InterpretStorageString(it->second, connectionArchive);

		// Add the part to the list of parts that are ready to use
		readyParts.push_back(it->first);
	}

	// Make the connections
	for (auto it = connectionArchive.begin(); it != connectionArchive.end(); ++it)
	{
		ConnectCommand(it->first, it->second);
	}

	// Create a CustomLogic and initialize it with the obtained ports and parts (it's necessary to keep track
	// of them for each custom logic in order to be able to save that custom logic in another custom logic
	// and to keep the strong reference to these parts)
	auto custom = BasePart::Create<CustomLogic>(readyParts, arg);

	// Finally add the custom logic to the list of parts
	Parts.push_back(custom);

	f.close();

	// If it was the first part added
	if (Parts.size() == 1)
	{
		// Set the number of possible selections in the part info section
		// (because currently it's zero and the part list position doesn't change when adding the 1st part)
		RecalculateMaxInfoLines();
	}

	// Reassign the maximum position in the parts list
	PartListPosition.SetLast(Parts.size());
}


// Checks if there's no collision (file with the given name doesn't exist). Throws an exception if it exists (user-friendly)
void FileColisionCheck(std::string fileName)
{
	std::ifstream f(fileName);

	if (f.good())
	{
		throw Exception("There already exists a record with the given name");
	}
}


// Saves the current circuit as a Custom Logic. The argument is the 
void SaveCommand(std::string arg)
{
	if (Parts.empty())
	{
		throw Exception("The circuit is empty");
	}

	// Check for file collision
	FileColisionCheck("saves/" + arg + ".txt");

	// String that will store all connections
	std::string connections;

	// List of strings that describe each part in the circuit
	std::list<std::string> partsInfo;

	// For each created part
	for (auto it = Parts.begin(); it != Parts.end(); ++it)
	{
		// Get its storage string
		auto s = (*it)->GenerateStorageString();

		// If the part doesn't influence the output (example: Output which is not marked as a custom logic output)
		// it will provide an empty string
		if (s.empty())
		{
			continue;
		}

		// If it had any connections
		if (s[0]!="")
		{
			// Add them the string holding all connections
			connections += s[0] + ";";
		}

		// Add the first (part code) and second (internal settings) part to the list of part infos
		partsInfo.push_back(s[1] + (s.size() == 3 ? "|" + s[2] : ""));
	
	}

	if (Trim(connections).empty())
	{
		connections = "null";
	}
	else
	{
		// Get rid of the final ';'
		connections = connections.substr(0, connections.size()-1);
	}

	// Create a folder for the saved parts. If this folder alredy exist this function won't do anything
	CreateDirectory(L"saves", NULL);

	// Open the desired file
	std::ofstream f("saves/" + arg + ".txt", std::ios::out);

	if (!f.is_open())
	{
		throw Exception("Something went wrong");
	}

	// Input the connections as the first record
	f << connections;

	// Input all the info about parts
	for (auto it = partsInfo.begin(); it != partsInfo.end(); ++it)
	{
		f << std::endl <<(*it);
	}

	// Close the file
	f.close();
}


// Swaps all occurances of oldID with newID
void SwapID(std::list<std::pair<int, int>>& list, int oldID, int newID)
{
	for (auto it = list.begin(); it != list.end(); ++it)
	{
		if (it->first == oldID)
		{
			it->first = newID;
		}

		if (it->second == oldID)
		{
			it->second = newID;
		}
	}
}


// Deletes a given custom logic from the disc. Returns true if the file was deleted succesfully
bool DeleteCustomLogicCommand(std::string arg)
{
	return !std::remove(("saves/"+arg+".txt").c_str());
}