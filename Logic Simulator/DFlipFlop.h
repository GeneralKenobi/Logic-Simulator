#pragma once
#include "stdafx.h"


// D FlipFlop propagates the D input value to Q output on clock changes (either rising or falling edge, depends on 
// value of mUseNegativeClock)
class DFlipFlop : public BaseLatch
{

protected:

	#pragma region Members

	// D input of the flip flop
	std::shared_ptr<InputSocket> mD = Identifiable::Create<InputSocket>();

	#pragma endregion

	#pragma region Virtual Methods

	// Computes and assigns output value
	virtual void ComputeOutput();

	// Method performing initial routine for this part
	virtual void InitializeRoutine();

	#pragma endregion

public:

	#pragma region Constructor/Destructor

	// Default constructor
	DFlipFlop();

	// Default destructor
	virtual ~DFlipFlop() { }

	#pragma endregion

	#pragma region Accessors

	// Returns the number of selectable lines in this part's info
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