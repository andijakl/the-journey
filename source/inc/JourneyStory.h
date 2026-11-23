/*
* ============================================================================
*  Name     : CJourneyStory from JourneyStory.h
*  Part of  : Journey
*  Created  : 04.05.2004 by Andreas Jakl
*  Description:
*     Prepares and stores the story text and information.
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

#ifndef __JOURNEYSTORY_H__
#define __JOURNEYSTORY_H__

#include <eikenv.h>
#include <AknUtils.h> // For Font names - Series 60 platform dependent code 
#include <journey.rsg>

#include <Journey.mbg>      // Bitmap enumeration

#define MAX_NUM_TALKINFOS       15
#define MAX_NUM_AREAS           3
#define MAX_NUM_CELLINFOS       10


struct STalkInfo
{
    TInt iAreaNum;
    TInt iTextRes;
    TInt iBitmapEnum;
    TInt iWrongAreaTextRes[MAX_NUM_AREAS];
    TInt iNoAssignedAreaTextRes;
};

struct SAreaInfo
{
    TInt iAreaNameRes;
    TInt iBitmapEnum;
    TInt iHowFar;       // If this area has not yet been initialized - the nth cell-id is the new area.
};

/**
* Story and game information management.
* Reads text and game information from resource files and stores it
* in a format easily accessibly by the game.
*/
class CJourneyStory
{
public:
    /// Constructors
    CJourneyStory();

    /// Destructor
    virtual ~CJourneyStory();

public:
    /**
    * Read text from the resource files and compile the arrays containing
    * information about the story and the areas.
    */
    void InitStory();

public:
    /**
    * Array containing all information about each step in the game progress.
    */
    STalkInfo iTalkInfos[MAX_NUM_TALKINFOS];

    /**
    * Array containing information about the individual areas that will be
    * used within the game.
    */
    SAreaInfo iAreaInfos[MAX_NUM_AREAS];

    /**
    * What to display when the player is in no assigned area.
    */
    SAreaInfo iNoAreaInfo;

    /**
    * Text resource ID of the warning message when no cell id can be determined
    * when starting/continuing a game.
    */
    TInt iNoNetTextRes;

    /**
    * The text that is displayed for the ok key when the end of a story segment has
    * been completed.
    */
    TInt iContinueTextRes;

};

#endif
