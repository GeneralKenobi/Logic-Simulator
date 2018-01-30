#include "stdafx.h"
#include "PrivateFlowControl.h"
#include "ArrowPosition.h"
#include "Area.h"

#pragma region Global Variables

// Position of the arrow in the part list sector
ArrowPosition PartListPosition;

// Position of the arrow in the part info sector
ArrowPosition PartInfoPosition;

// When true, currently presnted part in the info section is locked and won't be changed (but position on part list can change)
bool LockInfo = false;

// Position of the locked part on part list
int LockedPosition = 0;

// Currently selected area
Area CurrentArea;

// List containing all parts
std::list<std::shared_ptr<BasePart>> Parts;

// Currently inputted command
std::string CurrentCommand;

// List containing past commands
std::list<std::string> PastCommands;

// Iterator used to go through the past commands
std::list<std::string>::iterator PastCommandsIterator = PastCommands.end();

#pragma endregion

// Typedef for a command mapping - string is mapped to an int which is then used in switch statements
typedef std::pair<std::string, int> CmdMap;



// Performs all actions that need to be done before entering the main loop of the program
void Initialize()
{
	// Subscribe to events:
	// When part list position changes
	PartListPosition.PositionChangedEvent.Subscribe(PartListPositionChanged);

	// When part info position changes
	PartInfoPosition.PositionChangedEvent.Subscribe(PartInfoPositionChanged);

	// When currently selected sector changes
	CurrentArea.SectorChangedEvent.Subscribe(PrintArrows);

	// Print the UI frame
	PrintUI();

	// Print the arrows
	PrintArrows(CurrentArea.Get());
}


// Main loop controlling the program. Reads and interprets the input provided by the user
void MainLoop()
{
	// Handle to the standard input
	HANDLE input = GetStdHandle(STD_INPUT_HANDLE);

	// number of events that occured
	DWORD Events = 0;

	// Number of events console inputs read
	DWORD EventsRead = 0;

	// Keep going until the end of the program
	while (true)
	{

		// Get the number of input events
		GetNumberOfConsoleInputEvents(input, &Events);

		// If there were some events
		if (Events != 0)
		{
			// Read them
			INPUT_RECORD* buffer = new INPUT_RECORD[Events];

			ReadConsoleInput(input, buffer, Events, &EventsRead);

			// Go through every event
			for (DWORD i = 0; i < EventsRead; ++i)
			{
				// If it was a key event and it was key down
				if (buffer[i].EventType == KEY_EVENT && buffer[i].Event.KeyEvent.bKeyDown)
				{
					// Do something based on the key
					switch (buffer[i].Event.KeyEvent.wVirtualKeyCode)
					{
						// Backspace - if in CommandLine, delete the last char
						case VK_RETURN:
						{
							EnterPressed();
						}
						break;


						// When right arrow is pressed, if we're in part list,
						// Go to part info
						case VK_RIGHT:
						{
							if (Parts.size() > 0 && CurrentArea.Get() == Sector::PartList)
							{
								CurrentArea.Set(Sector::PartInfo);								
							}
						}
						break;

						// When left arrow is pressed, if we're in part info,
						// Go to part list
						case VK_LEFT:
						{
							if (CurrentArea.Get() == Sector::PartInfo)
							{
								CurrentArea.Set(Sector::PartList);
							}
						}
						break;

						// Backspace
						case VK_BACK:
						{
							BackspacePressed();
						}
						break;

						case VK_UP:
						{
							UpArrowPressed();
						}
						break;

						case VK_DOWN:
						{
							DownArrowPressed();
						}
						break;

						case VK_F1:
						{
							CurrentArea.Set(Sector::CommandLine);
						}
						break;

						case VK_F2:
						{
							CurrentArea.Set(Sector::PartList);
						}
						break;

						case VK_F3:
						{
							CurrentArea.Set(Sector::PartInfo);
						}
						break;

						case VK_SPACE:
						{
							SpacePressed();
						}
						break;

						case VK_SHIFT:
						{
							ShiftPressed();
						}
						break;

						case VK_DELETE:						
						{
							DeletePressed();
						}
						break;
						

						default:
						{
							// Handle all other keys
							if (CurrentArea.Get() == Sector::CommandLine)
							{
								DefaultKeyPressed(buffer[i].Event.KeyEvent.uChar.AsciiChar);
							}
						}
						break;
					}
				}
			}
		}
	}
}


// Accepts the given string as a quickly inputted command (overwrites the current one) and moves to the command line sector
void QuickActionCommand(std::string command)
{
	CurrentCommand = command;

	PrintCurrentCommand(true);

	CurrentArea.Set(Sector::CommandLine);
}


// Accessor to the currently presented part
std::shared_ptr<BasePart> CurrentPart()
{
	if (!Parts.empty())
	{
		auto it = Parts.begin();
		std::advance(it, LockInfo ? LockedPosition : PartListPosition.Get());
		return *it;
	}
	else
	{
		return std::shared_ptr<BasePart>();
	}
}

// Accessor to the currently selected part
std::shared_ptr<BasePart> CurrentListPart()
{
	if (!Parts.empty())
	{
		auto it = Parts.begin();
		std::advance(it, PartListPosition.Get());
		return *it;
	}
	else
	{
		return std::shared_ptr<BasePart>();
	}
}


// Attempts to create a part based on the given name (recognized names are given in Defines.h)
// If it wasn't successful returns an empty pointer
std::shared_ptr<BasePart> CreatePart(int code)
{
	// Switch on the int mapping
	switch (code)
	{
		case OR_GATE:
		{
			return BasePart::Create<ORGate>();
		}
		break;

		case NOR_GATE:
		{
			return BasePart::Create<NORGate>();
		}
		break;

		case TOGGLE_INPUT:
		{
			return BasePart::Create<ToggleInput>();
		}
		break;

		case CLOCK_INPUT:
		{
			return BasePart::Create<ClockInput>();
		}
		break;

		case AND_GATE:
		{
			return BasePart::Create<ANDGate>();
		}
		break;

		case NAND_GATE:
		{
			return BasePart::Create<NANDGate>();
		}
		break;

		case XOR_GATE:
		{
			return BasePart::Create<XORGate>();
		}
		break;

		case XNOR_GATE:
		{
			return BasePart::Create<XNORGate>();
		}
		break;

		case NOT_GATE:
		{
			return BasePart::Create<NOTGate>();
		}
		break;

		case BUTTON_INPUT:
		{
			return BasePart::Create<ButtonInput>();
		}
		break;

		case MULTIPLEXER:
		{
			return BasePart::Create<MUX>();
		}
		break;

		case DEMULTIPLEXER:
		{
			return BasePart::Create<DMUX>();
		}
		break;

		case D_LATCH:
		{
			return BasePart::Create<DLatch>();
		}
		break;

		case D_FLIP_FLOP:
		{
			return BasePart::Create<DFlipFlop>();
		}
		break;

		case T_LATCH:
		{
			return BasePart::Create<TLatch>();
		}
		break;

		case T_FLIP_FLOP:
		{
			return BasePart::Create<TFlipFlop>();
		}
		break;

		case JK_LATCH:
		{
			return BasePart::Create<JKLatch>();
		}
		break;

		case JK_FLIP_FLOP:
		{
			return BasePart::Create<JKFlipFlop>();
		}
		break;

		case OUTPUT:
		{
			return BasePart::Create<Output>();
		}
		break;

		case CUSTOM_LOGIC:
		{
			return BasePart::Create<CustomLogic>();
		}
		break;

		case CUSTOM_LOGIC_PORT:
		{
			return BasePart::Create<Propagator>();
		}
		break;



		default:
		{
			return std::shared_ptr<BasePart>();
		}
		break;

	}
}


// Attempts to create a part based on the given part code (codes are given in Defines.h)
// If it wasn't successful returns an empty pointer
std::shared_ptr<BasePart> CreatePart(std::string name)
{
	// Mapping of part names to ints (to use later in switch)
	std::map<std::string, int> partMap
	{
		CmdMap("i", TOGGLE_INPUT),
		CmdMap("input", TOGGLE_INPUT),
		CmdMap("clock", CLOCK_INPUT),
		CmdMap("cl", CLOCK_INPUT),
		CmdMap("button", BUTTON_INPUT),
		CmdMap("b", BUTTON_INPUT),
		CmdMap("not", NOT_GATE),
		CmdMap("or", OR_GATE),
		CmdMap("nor", NOR_GATE),
		CmdMap("and", AND_GATE),
		CmdMap("nand", NAND_GATE),
		CmdMap("xor", XOR_GATE),
		CmdMap("xnor", XNOR_GATE),
		CmdMap("mux", MULTIPLEXER),
		CmdMap("multiplexer", MULTIPLEXER),
		CmdMap("dmux", DEMULTIPLEXER),
		CmdMap("demultiplexer", DEMULTIPLEXER),
		CmdMap("dl", D_LATCH),
		CmdMap("d latch", D_LATCH),
		CmdMap("dff", D_FLIP_FLOP),
		CmdMap("d flipflop", D_FLIP_FLOP),
		CmdMap("d flip flop", D_FLIP_FLOP),
		CmdMap("tl", T_LATCH),
		CmdMap("t latch", T_LATCH),
		CmdMap("tff", T_FLIP_FLOP),
		CmdMap("t flipflop", T_FLIP_FLOP),
		CmdMap("t flip flop", T_FLIP_FLOP),
		CmdMap("jkl", JK_LATCH),
		CmdMap("jk latch", JK_LATCH),
		CmdMap("jkff", JK_FLIP_FLOP),
		CmdMap("jk flipflop", JK_FLIP_FLOP),
		CmdMap("jk flip flop", JK_FLIP_FLOP),
		CmdMap("o", OUTPUT),
		CmdMap("output", OUTPUT),
	};


	// If the name is inside the map
	if (partMap.find(name) != partMap.end())
	{
		return CreatePart(partMap[name]);
	}
	else
	{
		return std::shared_ptr<BasePart>();	
	}
}