#pragma once
#include "stdafx.h"



class DMUX : public BaseMuxDmux
{

protected:

	#pragma region Members	

	// Output sockets, one of them has state propagated from the input. There are 2^mSize outputs
	std::list<std::shared_ptr<OutputSocket>> mOutputs;

	// Output that is selected by the addresses
	std::shared_ptr<OutputSocket> mSelectedOutput;

	// Input of the multiplexer
	std::shared_ptr<InputSocket> mInput = Identifiable::Create<InputSocket>();

	#pragma endregion

	#pragma region Virtual Methods	

	// Abstract method which computes and assignes output value
	virtual void ComputeOutput();

	// Method performing initial routine for this part. Subscribes to input internal state changed event
	virtual void InitializeRoutine();

	#pragma endregion

	#pragma region Methods

	// Decreases the size of the demultiplexer to newSize (which should be smaller than mSize). Helper of ChangeSize
	virtual void ReduceSize(int newSize);

	// Increases the size of the demultiplexer to newSize (which should be greater than mSize). Helper of ChangeSize
	virtual void IncreaseSize(int newSize);

	// Helper method, generates info about inputs
	std::list<Line> GenerateOutputsInfo();

	// Helper method, generates compact info about inputs (one line per inputs)
	std::list<Line> GenerateOutputsInfoCompact();

	// Helper method, generates only inputs' statuses
	std::list<Line> GenerateOutputsStatus();

	#pragma endregion

public:

	#pragma region Constructors/Destructors

	// Default constructor
	DMUX();

	// Default destructor
	virtual ~DMUX() { }

	#pragma endregion	

	#pragma region Virtual Methods

	// Reprints the statuses and sets new value to the selected output
	virtual void InputChanged();	

	// Returns a list with lines that compose a full info on this part.
	// Should be overriden to include info about the inherited class.
	virtual std::list<Line> GenerateInfo();

	// Returns status lines for the part which has the same structure as list generated by GenerateInfo.
	// All lines that don't carry information about status are empty
	virtual std::list<Line> GenerateStatus();

	// Prints the compact information on this part (dedicated to the list of parts)
	virtual void PrintList();

	// Performs quick action for the given line (if there's an action to perform)
	virtual void QuickActionRequest(int index);

	// Generates string to save in a file which allows to recreate this part
	virtual std::vector<std::string> GenerateStorageString();

	// Interprets a storage string and assigns the settings saved in it. Reads 
	virtual void InterpretStorageString(std::string settings, std::list<std::pair<int, int>>& connections);

	#pragma endregion

};