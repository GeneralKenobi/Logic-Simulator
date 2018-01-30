#pragma once
#include "stdafx.h"

// Struct holding the currently selected screen area
struct Area
{
private:

	// Private field to store the sector
	Sector mSector = Sector::CommandLine;

public:

	// Getter, returns the currently selected sector
	Sector Get() { return mSector; }

	// Sets the currently selected sector, if the new sector is different fires SectorChangedEvent
	void Set(Sector sector);

	// Event fired whenever currently selected sector changes
	Event<Sector> SectorChangedEvent;
};