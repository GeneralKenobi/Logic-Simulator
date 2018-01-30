#pragma once

// Class for an output part. In standard mode allows the user to connect an input to it.
// When in custom logic allows the user to connect something to it.
class Propagator : public BasePart
{
protected:

	#pragma region Members
	
	// Output socket, presented on the screen when the part is placed in a custom logic
	std::shared_ptr<OutputSocket> mOutput = Identifiable::Create<OutputSocket>();

	// Input socket, presented on the screen when the part is placed in the circuit
	std::shared_ptr<InputSocket> mInput = Identifiable::Create<InputSocket>();

	// Operation mode of the propagator.
	// 0 - hidden
	// 1 - custom logic port as input
	// 2 - custom logic port as output
	int mOperationMode = 0;

	#pragma endregion

	#pragma region Virtual Methods

	// Computes and assignes output value (propagates input)
	virtual void ComputeOutput();

	// Method performing initial routine for each part
	// Subscribes to the InternalStateChanged event of the input
	virtual void InitializeRoutine();

	// Method that will assign owner id to every OutputSocket, should be overridden
	// by every derived class that introduces a new OutputSocket
	virtual void AssignOwnership();

	#pragma endregion

public:

	#pragma region Members

	// Method called whenever one of the inputs changes its value. Base version calls ComputeOutput
	// and PrintManagerStatus(PartInfo | PartList)
	virtual void InputChanged();

	#pragma endregion

	#pragma region Constructor/Desturctor

	// Default constructor
	Propagator();

	// Default destructor
	virtual ~Propagator() {}

	#pragma endregion

	#pragma region Accessors

	// Getter to the mIsOutput
	int OperationMode() { return mOperationMode; }

	std::shared_ptr<OutputSocket> Output() { return mOutput; }

	#pragma endregion

	#pragma region Virtual Methods

	// Returns the numberof selectable lines in this part's info
	virtual int SelectableLines();

	// Returns a list with lines that compose a full info on this part.
	// Should be overriden to include info about the inherited class.
	virtual std::list<Line> GenerateInfo();

	// Returns status lines for the part which has the same structure as list generated by GenerateInfo.
	// All lines that don't carry information about status are empty
	virtual std::list<Line> GenerateStatus();

	// Prints the compact information on this part (dedicated to the list of parts)
	// Printed informations is: "*PrintName*, ID: 5". Can (and should) be overriden
	// to provide more specific information
	virtual void PrintList();

	// Performs quick action for the given line (if there's an action to perform)
	virtual void QuickActionRequest(int index);

	// Interprets a storage string and assigns the settings saved in it. Reads 
	virtual void InterpretStorageString(std::string settings, std::list<std::pair<int, int>>& connections);

	// Generates string to save in a file which allows to recreate this part
	virtual std::vector<std::string> GenerateStorageString();

	#pragma endregion


};