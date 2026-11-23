/*
* ============================================================================
*  Name     : CJourneyLocation from JourneyLocation.h
*  Part of  : Journey
*  Created  : 04.05.2004 by Andreas Jakl
*  Description:
*     Provides services for getting the Cell-ID and to
*     define and query defined areas.
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

#ifndef __JOURNEYLOCATION_H__
#define __JOURNEYLOCATION_H__

// Telephony libraries for location detection
//#include <etelbgsm.h>
#include <etel.h>

#include "SysInfo.h"
#include "JourneyStory.h"

#define CALLBACK_NONE           0
#define CALLBACK_SEARCH         1
#define CALLBACK_WAIT           2  

struct TCellInfo
{
    TUint iCellId;
    TInt iAreaNum;
};


class MSearchCallback
{
public:
    /**
     * Callback function from the location engine.
     * Called when the location engine has received a cell id that completes
     * an event the engine is waiting for.
     */
	virtual void SearchEventCompleted() = 0;
	
    /**
     * Callback function from the location engine.
     * Called when there has been an error in retrieving the cell id.
     * If everything went fine, this function can also be called with
     * KErrNone to reset any error state the engine might have been in
     * after a temponary problem.
     */
	virtual void LocationError(TInt aErrCode) = 0;
};


/**
* Location Management Class.
* Encapsulates the location tracking functionality. Provides methods to
* get the cell id, stores them and administrates the game areas.
*/
class CJourneyLocation : public CBase, public MLocationCallback
{
public:
    /// Creation of the class with cleanup stack handling
	static CJourneyLocation* NewL(MSearchCallback* aObserver);

    /// Constructors
    CJourneyLocation(MSearchCallback* aObserver);

    /// Initialize contents of the class
	void ConstructL();

    /// Destructor
    virtual ~CJourneyLocation();
    
public:   // Own functions
    /**
    * Initializes the search for a new area (if the area number is not known
    * yet) or wait until the player returns to a known area.
    * @param aAreaNum the number of the area to search for (if unknown) or to
    * wait for (if known)
    * @param aNthAreaNum if the area is new and not known yet, the Nth cell ID
    * will get the area.
    */
    void InitGoToArea(TInt aAreaNum, TInt aNthAreaNum);

    /**
    * Callback-function. Returns if the timer should continue or not, 
    * if not meaning that the task to wait for has been completed.
    * @return if the timer should continue or not (meaning that what was waited for,
    * initialized by InitGoToArea, is completed).
    */
    void StartGetCellIdL();

    /**
    * Returns the number of the area of the currently active cell ID. If the current
    * ID is not assigned to an area, returns -1.
    * @return the area of the active cell or -1 if it is not in an assigned area.
    */
    TInt GetCurAreaNumL();
    
    /**
    * Write the saved cell and area data to the stream.
    * @param aStream the stream to write the information to.
    */
    void ExternalizeL(RWriteStream& aStream) const;

    /**
    * Read the location data from the stream and save it in the member variables.
    * @param aStream the stream to get the information from.
    */
    void InternalizeL(RReadStream& aStream);
    
    /**
     * Callback-function from MLocationCallback
     */
    void GotLocation(TInt aErrCode, TUint aCellId);
    
    /**
     * Get the cached cell id value.
     */
    TUint GetLatestCellId();

public: // Data

private:
    /**
    * Checks if a cell-id has already been found during a search for a new area.
    * This does not indicate that it has to be in an assigned area, just that the
    * player has already been in this cell before during an active search for a new
    * area.
    * @param aCellId the cell ID to check.
    * @return whether this cell ID has already been recorded.
    */
    TBool IsCellIdKnown( const TUint aCellId );

    /**
    * Checks if a specific area does already have a cell-id assigned.
    * @return whether this area is already known to the game.
    */
    TBool IsAreaNumAssigned( const TInt aAreaNum );

    /**
    * Returns the cell-id of an area or -1 if this area isn't assigned yet.
    * @param aAreaNum the number of the area to check.
    * @return the cell id of the area if it is known, or -1 if it is unknown.
    */
    TUint GetCellIdOfArea( const TInt aAreaNum );

    /**
    * Stores the cell id and its associated area number. Use -1 for the area number
    * if this cell ID should not define an area.
    * @param aCellId the cell id to add to the internal storage.
    * @param aAreaNum the number of the defined area of this cell id. -1 if this cell
    * should not define a new area.
    */
    void StoreCellId( const TUint aCellId, const TInt aAreaNum );
    
    

private:    // Data
    /**
    * Defines which type of search is active - waiting for a known area or searching
    * for a new area.
    */
    TInt iWhichSearchActive;

    /**
    * - When searching for an area, this variable specifies how many more
    * cell-ids to find till a new cell-id gets an area. Every time a new cell-id
    * is found, this variable is reduced by 1. When it reaches 0, the current
    * cell-id will be the new area.
    * - When waiting for an area, this variable specifies the cell-id of the area
    * we're waiting for.*/
    TUint iWaitFor;

    /**
    * How many places of the iCellInfos-array are already filled, meaning
    * the number of different cell-ids known to the program.
    */
    TInt iAssignedCellInfos;

    /**
    * The number of the next area which will be assigned with the next search.
    */
    TInt iNextAreaNum;  

    /**
    * Array storing all cell-ids and for cell-ids that will also get an area,
    * also the area-number (otherwise -1).
    */
    TCellInfo iCellInfos[MAX_NUM_CELLINFOS];

	/**
	 * System interface to get the cell id information.
	 */
	CSysInfo* iSysInfo;
	
	/**
	 * Callback object to inform the engine about location updates.
	 */
	MSearchCallback* iObserver;
	
	/**
	 * Cached cell id value.
	 */
	TUint iCellId;
};

#endif

