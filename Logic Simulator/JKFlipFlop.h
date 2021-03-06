#pragma once
#include "stdafx.h"


class JKFlipFlop : public BaseLatch
{

protected:

	#pragma region Members

	// J input of the flip flop
	std::shared_ptr<InputSocket> mJ = Identifiable::Create<InputSocket>();

	// K input of the flip flop
	std::shared_ptr<InputSocket> mK = Identifiable::Create<InputSocket>();

	#pragma endregion

	#pragma region Virtual Methods

	// Computes and assigns output value
	virtual void ComputeOutput();

	// Method performing initial routine for each part
	virtual void InitializeRoutine();

	#pragma endregion

public:

	#pragma region Constructor/Destructor

	// Default constructor
	JKFlipFlop();

	// Default destructor
	virtual ~JKFlipFlop() { }

	#pragma endregion

	#pragma region Accessors

	// Returns the numberof selectable lines in this part's info
	virtual int SelectableLines();

	#pragma endregion

	#pragma region Virtual methods

	// Callback for when clock value changes
	virtual void ClockChanged();

	// Returns a list with lines that compose a full info on this part.
	virtual std::list<Line> GenerateInfo();

	// Returns status lines for the part which has the same structure as list generated by GenerateInfo.
	// All lines that don't carry information about status are empty
	virtual std::list<Line> GenerateStatus();

	// Performs quick action for the given line (if there's an action to perform)
	virtual void QuickActionRequest(int index);

	// Generates string to save in a file which allows to recreate this part
	virtual std::vector<std::string> GenerateStorageString();

	// Applies settings given in the vector
	virtual void InterpretStorageString(std::string settings, std::list<std::pair<int, int>>& connections);

	#pragma endregion

};