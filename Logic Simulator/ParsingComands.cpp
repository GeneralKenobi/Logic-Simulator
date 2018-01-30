#include "stdafx.h"
#include "PrivateCommands.h"

#pragma region Command mapping defines

#define CREATE_COMMAND 0
#define CONNECT_COMMAND 1
#define LOAD_COMMAND 2
#define ADD_INPUT_COMMAND 3
#define SAVE_COMMAND 4
#define DELETE_COMMAND 5
#define CLOCK_PERIOD_COMMAND 6
#define DELETE_CUSTOM_LOGIC_COMMAND 7

#pragma endregion



// Returns standardized version of command (lowercase, trimmed, multiple spaces removed)
std::string StandardizeCommand(std::string command)
{
	// transform the input to all lowercase
	std::transform(command.begin(), command.end(), command.begin(), tolower);

	command = Trim(command);

	// Remove multiple spaces
	command = ReplaceSubstr(command, "  ", " ");

	return command;
}



// Parses a command and takes appropriate action. Returns true if the command was proper and executed
// without errors, false otherwise
bool ParseCommand(std::string command)
{
	// Mapping of command names to ints (to use later in switch)
	std::map<std::string, int> commandMap
	{
		CmdMap("create", CREATE_COMMAND),
		CmdMap("load", LOAD_COMMAND),
		CmdMap("delete", DELETE_COMMAND),
		CmdMap("connect", CONNECT_COMMAND),
		CmdMap("clockperiod", CLOCK_PERIOD_COMMAND),
		CmdMap("save", SAVE_COMMAND),
		CmdMap("remove", DELETE_CUSTOM_LOGIC_COMMAND),
	};

	command = StandardizeCommand(command);

	// Get the name of the command (first word)
	std::string commandName(command.substr(0, command.find(' ')));
	
	// If the command was recognized
	if (commandMap.find(commandName) != commandMap.end())
	{
		// Find the space
		if (command.find(' ') != std::string::npos)
		{
			// If it was found, extract the argument (substring from one space after space)
			std::string arg = command.substr(command.find(' ')+1);
			
			// If something goes wrong in the one of the command methods it will throw an exception
			// with a message to the user
			try
			{
				// Call an appropriate command
				switch (commandMap[commandName])
				{
					case CREATE_COMMAND:
					{
						CreateCommand(arg);
					}
					break;

					case DELETE_COMMAND:
					{
						DeleteCommand(arg);
					}
					break;

					case CONNECT_COMMAND:
					{
						ConnectCommand(arg);
					}
					break;

					case CLOCK_PERIOD_COMMAND:
					{
						ClockPeriodCommand(arg);
					}
					break;

					case LOAD_COMMAND:
					{
						LoadCommand(arg);
					}
					break;

					case SAVE_COMMAND:
					{
						SaveCommand(arg);
					}
					break;

					case DELETE_CUSTOM_LOGIC_COMMAND:
					{
						return DeleteCustomLogicCommand(arg);
					}
					break;

				}
			}
			// Catch which serves to retrieve errors in execution of command method
			// It will print the given error message to the user
			catch (Exception e)
			{
				LogError(e.ErrorMessage());
				return false;
			}

		}
		// In case the space wasn't found (no arguments)
		else
		{
			LogError("Arguments are missing");
			return false;
		}
	}
	// If the command wasn't recognized
	else
	{
		LogError("Unknown command: " + command);
		return false;
	}	

	// In case there was an error message stored, remove it
	LogError("");
	return true;
}