#pragma once
#include "stdafx.h"



class CustomLogic : public BasePart
{
protected:

	#pragma region Members

	// List containing all Propagators that should be exposed to the user as Inputs
	std::list<std::shared_ptr<Propagator>> mInputs;

	// List containing all Propagators that should be exposed to the user as Outputs
	std::list<std::shared_ptr<Propagator>> mOutputs;

	// List containing all parts that make up this custom logic
	std::list<std::shared_ptr<BasePart>> mSubParts;

	#pragma endregion

	#pragma region Virtual methods

	// Custom logic is only a storage part - it doesn't perform any output computations
	virtual void ComputeOutput() { }

	// Initial routine - subscribes to Input and Output InternalStateChanged with RefreshInfoStatus
	virtual void InitializeRoutine();

	// Assigns node ownership to every output socket in mOutputs
	virtual void AssignOwnership();

	#pragma endregion

public:

	#pragma region Constructor/Destructor

	// Default constructor which allows to assign a name to this CustomLogic
	CustomLogic(std::string name = "Custom Logic");

	// Constructor which creates a CustomLogic with a set of parts that it should store. Finds and exposes marked Propagators
	CustomLogic(std::list<std::shared_ptr<BasePart>> subParts, std::string name = "Custom Logic");

	// Default destructor
	virtual ~CustomLogic() { }

	#pragma endregion

	#pragma region Virtual Methods

	// Returns the number of selectable lines in this part's info
	virtual int SelectableLines();

	// Returns a list with lines that compose a full info on this part.
	virtual std::list<Line> GenerateInfo();

	// Returns status lines for the part which has the same structure as list generated by GenerateInfo.
	// All lines that don't carry information about status are empty
	virtual std::list<Line> GenerateStatus();

	// Quick Action
	virtual void QuickActionRequest(int index);

	// Generates string to save in a file which allows to recreate this part. Gathers information on all parts that
	// make up this CustomLogic
	virtual std::vector<std::string> GenerateStorageString();

	// Custom logic is never loaded/saved from the memory
	virtual void InterpretStorageString(std::string settings, std::list<std::pair<int, int>>& connections) {}

	#pragma endregion
	
};