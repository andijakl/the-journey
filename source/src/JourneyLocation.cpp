/*
* ============================================================================
*  Name     : CJourneyLocation from JourneyLocation.cpp
*  Part of  : Journey
*  Created  : 04.05.2004 by Andreas Jakl
*  Version  : 1.20
*  Copyright:
*     'The Journey' is free software; you can redistribute
*     it and/or modify it under the terms of the GNU General Public License
*     as published by the Free Software Foundation; either version 2 of
*     the License, or (at your option) any later version.
*
*     'The Journey' is distributed in the hope that it
*     will be useful, but WITHOUT ANY WARRANTY; without even the implied
*     warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*     See the GNU General Public License for more details.
*
*     You should have received a copy of the GNU General Public License
*     along with 'The Journey'; if not, write to the Free Software
*     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
* ============================================================================
*/

#include "JourneyLocation.h"


#include <aknnotewrappers.h> // TEMP DEBUG

//------------------------------------------------------------------------------
CJourneyLocation* CJourneyLocation::NewL(MSearchCallback* aObserver)
{
    CJourneyLocation* self = new (ELeave) CJourneyLocation(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();        // self
    return self;
}

// Constructor
//------------------------------------------------------------------------------
CJourneyLocation::CJourneyLocation(MSearchCallback* aObserver)
        :
        iWhichSearchActive(CALLBACK_NONE),
        iNextAreaNum(0),
        iObserver(aObserver)
{}

//------------------------------------------------------------------------------
void CJourneyLocation::ConstructL()
{
    iSysInfo = CSysInfo::NewL(this);
    for (TInt i=0; i < MAX_NUM_CELLINFOS; i++)
    {
        iCellInfos[i].iCellId = 0;
        iCellInfos[i].iAreaNum = -1;
    }
}

// Destructor
//------------------------------------------------------------------------------
CJourneyLocation::~CJourneyLocation()
{
	if (iSysInfo)
	{
	    delete iSysInfo;
	    iSysInfo = NULL;
	}
}

//------------------------------------------------------------------------------
void CJourneyLocation::InitGoToArea(TInt aAreaNum, TInt aNthAreaNum)
{
    TInt cellIdOfArea = GetCellIdOfArea(aAreaNum);
    if (cellIdOfArea == 0)
    {
        // Area is not yet known - start a new search
        // Store how many more cell-ids to find till we declare the cell-id
        // as the next game-relevant area.
        iWaitFor = aNthAreaNum;
        iWhichSearchActive = CALLBACK_SEARCH;
    }
    else
    {
        // Area has already been assigned - wait till the user returns to it
        // Store the cell-id we're waiting for
        iWaitFor = GetCellIdOfArea(aAreaNum);
        iWhichSearchActive = CALLBACK_WAIT;
    }
}

//------------------------------------------------------------------------------
void CJourneyLocation::StartGetCellIdL()
{
    // Send request to the network interaction class which will
    // get the cell information.
    iSysInfo->StartGetNetworkInfo();
}


//------------------------------------------------------------------------------
void CJourneyLocation::GotLocation(TInt aErrCode, TUint aCellId)
{
	if (aErrCode != KErrNone)
	{
		iCellId = 0;
		iObserver->LocationError(aErrCode);
		return;
	}
    TBool returnVar = ETrue;

    // Only handle the cell-id if getting it was successful (!= 0) and if
    // the user is in an area where he has a network (otherwise the cell-id is 0).
    if (aCellId > 0)
    {
    	// Store latest cell id for future reference
    	iCellId = aCellId;
        if (iWhichSearchActive == CALLBACK_SEARCH)
        {
            // Check if this cell id is already known or if it's a new one
            if (!IsCellIdKnown(aCellId))
            {
                // We've found a new cell-id!
                iWaitFor --;
                if (iWaitFor == 0)
                {
                    // This cell id will get the area we've been waiting for...
                    StoreCellId(aCellId, iNextAreaNum);
                    iNextAreaNum ++;
                    returnVar = EFalse;
                }
                else
                {
                    // Store the cell-id, but don't assign an area to it.
                    StoreCellId(aCellId, -1);
                }
            }
        }
        else
        {
            // Check if the current cell-id is the one of the area we're waiting for.
            if ( aCellId == iWaitFor)
            {
                // The user has returned to the area we're waiting for!
                returnVar = EFalse;
            }
        }
        if (!returnVar)
        {
            iWhichSearchActive = CALLBACK_NONE;
		    iObserver->SearchEventCompleted();
        }
        else
        {
        	// We got a valid cell id, make sure that the engine is no longer
        	// in an error state, in case it has been in that before.
			iObserver->LocationError(KErrNone);
        }
    }
    else
    {
    	// Cell-ID is 0
		iObserver->LocationError(KErrCellId0);
    }
}

//------------------------------------------------------------------------------
TInt CJourneyLocation::GetCurAreaNumL()
{
    TInt returnVar = -1;
    TUint curCellId = iSysInfo->GetLatestCellId();

    if (iAssignedCellInfos > 0)
    {
        for (TInt i = 0; i < iAssignedCellInfos; i++)
        {
            if (iCellInfos[i].iCellId == curCellId)
            {
                returnVar = iCellInfos[i].iAreaNum;
                break;
            }
        }
    }

    return returnVar;

}

//------------------------------------------------------------------------------
TBool CJourneyLocation::IsCellIdKnown( const TUint aCellId )
{
    TBool found = EFalse;
    if (iAssignedCellInfos > 0)
    {
        for (TInt i = 0; i < iAssignedCellInfos; i++)
        {
            if (iCellInfos[i].iCellId == aCellId)
            {
                found = ETrue;
                break;
            }
        }
    }
    return found;
}


//------------------------------------------------------------------------------
TUint CJourneyLocation::GetCellIdOfArea( const TInt aAreaNum )
{
    TUint returnVar = 0;
    if (iAssignedCellInfos > 0)
    {
        for (TInt i = 0; i < iAssignedCellInfos; i++)
        {
            if (iCellInfos[i].iAreaNum == aAreaNum)
            {
                returnVar = iCellInfos[i].iCellId;
                break;
            }
        }
    }
    return returnVar;
}

//------------------------------------------------------------------------------
TUint CJourneyLocation::GetLatestCellId()
{
	return iCellId;
}

//------------------------------------------------------------------------------
TBool CJourneyLocation::IsAreaNumAssigned( const TInt aAreaNum )
{
    return ( GetCellIdOfArea(aAreaNum) > (0) );
}

//------------------------------------------------------------------------------
void CJourneyLocation::StoreCellId( const TUint aCellId, const TInt aAreaNum )
{
    if (iAssignedCellInfos < MAX_NUM_CELLINFOS)
    {
        iCellInfos[iAssignedCellInfos].iCellId = aCellId;
        iCellInfos[iAssignedCellInfos].iAreaNum = aAreaNum;
        iAssignedCellInfos ++;
    }
}


//------------------------------------------------------------------------------
void CJourneyLocation::ExternalizeL(RWriteStream& aStream) const
{
    aStream.WriteInt32L(iAssignedCellInfos);
    aStream.WriteInt32L(iNextAreaNum);
    aStream.WriteInt32L(iWhichSearchActive);
    aStream.WriteInt32L(iWaitFor);
    // Write the contents of the iCelInfos-array to the file-stream
    // as 32bit Integers
    for (TInt i=0; i < MAX_NUM_CELLINFOS; i++)
    {
        aStream.WriteUint32L(iCellInfos[i].iCellId);
        aStream.WriteInt32L(iCellInfos[i].iAreaNum);
    }
}


//------------------------------------------------------------------------------
void CJourneyLocation::InternalizeL(RReadStream& aStream)
{
    iAssignedCellInfos = aStream.ReadInt32L();
    iNextAreaNum = aStream.ReadInt32L();
    iWhichSearchActive = aStream.ReadInt32L();
    iWaitFor = aStream.ReadInt32L();

    // Read the values from the file and fill the array with it
    for (TInt i=0; i < MAX_NUM_CELLINFOS; i++)
    {
        iCellInfos[i].iCellId = aStream.ReadUint32L();
        iCellInfos[i].iAreaNum = aStream.ReadInt32L();
    }
}

