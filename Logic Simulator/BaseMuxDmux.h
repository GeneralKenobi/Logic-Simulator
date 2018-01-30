#pragma once
#include "stdafx.h"


class BaseMuxDmux : public BasePart
{

protected:

	#pragma region Members

	// Size of this multiplexer. Number of inputs is 2^mSize. Number of addresses is mSize
	int mSize = 0;

	// Addresses which determine which input socket to propagate. There are mSize addresses
	std::list<std::shared_ptr<InputSocket>> mAddresses;

	#pragma endregion

	#pragma region Virtual Methods

	// Method performing initial routine for this part
	virtual void InitializeRoutine();

	// Decreases the size of the part to newSize (which should be smaller than mSize). Helper of ChangeSize
	virtual void ReduceSize(int newSize) = 0;

	// Increases the size of the part to newSize (which should be greater than mSize). Helper of ChangeSize
	virtual void IncreaseSize(int newSize) = 0;

	#pragma endregion
	
	#pragma region Methods

	// Returns true if the size is big enough to enter compact display mode
	bool CompactMode();

	// Helper method, generates info about addresses
	std::list<Line> GenerateAddressInfo();

	// Helper method, generates compact info about addresses (one line per address)
	std::list<Line> GenerateAddressInfoCompact();

	// Helper method, generates only addresses' statuses
	std::list<Line> GenerateAddressStatus();

	#pragma endregion

public:

	#pragma region Constructors/Destructors

	// Default destructor
	virtual ~BaseMuxDmux() { }

	#pragma endregion

	#pragma region Accessors

	// Returns the numberof selectable lines in this part's info
	virtual int SelectableLines();

	#pragma endregion

	#pragma region Methods

	// Finds the new propagated input and reprints the statuses
	void AddressChanged();

	#pragma endregion
};