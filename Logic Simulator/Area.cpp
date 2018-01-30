#include "stdafx.h"
#include "Area.h"




// Sets the currently selected sector, if the new sector is different fires SectorChangedEvent
void Area::Set(Sector sector)
{
	if (sector != mSector)
	{
		mSector = sector;
		SectorChangedEvent(mSector);
	}
}
