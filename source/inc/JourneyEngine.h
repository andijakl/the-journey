/*
* ============================================================================
*  Name     : CJourneyEngine from JourneyEngine.h
*  Part of  : Journey
*  Created  : 04.05.2004 by Andreas Jakl
*  Description:
*     The game-engine. Handles the control of the gameflow.
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

#ifndef __JOURNEYENGINE_H__
#define __JOURNEYENGINE_H__

#include <f32file.h>
#include <s32file.h>

#include "bitmapmethods.h"

#include "JourneyLocation.h"
#include "JourneyStory.h"
#include "JourneyFunctions.h"
#include "JourneyCGame.h"
#include "SharedSettings.h"

#include "Journey.hrh"

#define DELETE_AND_NULLZ(p)		{delete p; p = NULL;}

class CJourneyCGame;

/**
* Game Engine.
* Handles and manages functions related to the gameplay.
*/
class CJourneyEngine : public CCoeControl, public MSearchCallback
{
public:
	static CJourneyEngine* NewL(CJourneyCGame* aOwningControl);

    /// Constructors
    CJourneyEngine(CJourneyCGame* aOwningControl);

    /// Initialize contents of the class
	void ConstructL();

    /// Destructor
    virtual ~CJourneyEngine();

public:     // Own public functions
    /**
    * Continues a game using the savegame file. If no file is found,
    * a new game will be started. Called once after initializing this class.
    */
    void ContinueGameL();

    /**
    * Starts a new game. Called once after initializing this class.
    */
    void StartNewGameL();

    /**
    * Take the appropriate action when the joystick is pressed. If the game
    * is not in search or wait mode, this is a progress to the next part.
    */
    void HandleJoystickPressL();
    
    /**
    * Save the current game progress to a file.
    */
    void SaveGameProgressL();

    /**
    * Deletes the game progress file.
    */
    void DeleteGameProgressL();

    /**
    * Returns whether the game progress file exists.
    * @return ETrue if a game progress file was found.
    */
	TBool GameProgressFileExistsL();

    /**
    * Load the static background pictures and keep them.
    */
    void LoadArtL();

    /**
    * Displays the correct text, location picture and text according to the game
    * progress and state.
    */
    void UpdateScreenL();
    
    /**
     * Callback function from the location engine.
     * Called when the location engine has received a cell id that completes
     * an event the engine is waiting for.
     */
    void SearchEventCompleted();
    
    /**
     * Callback function from the location engine.
     * Called when there has been an error in retrieving the cell id.
     * If everything went fine, this function can also be called with
     * KErrNone to reset any error state the engine might have been in
     * after a temponary problem.
     */
    void LocationError(TInt aErrCode);

private:
	/**
	 * Send an event to the owning control that we'd like to change
	 * the game text. This function checks whether the new game text is
	 * really different to the old one and doesn't relay the message if
	 * it's the same.
	 */
	void ChangeText(const TInt aTextResourceId);
	
    /**
    * Load the game progress from a file.
    */
    void LoadGameProgressL();

    /**
    * Start the timer when waiting for the user to return to an area or when
    * searching for a new area.
    */
    void StartTimerL();

    /**
    * Stops the timer when the search is complete or the game is exited.
    */
    void StopTimer();

    /**
    * The timer callback function. Has to be static to be called by the active
    * object of the timer.
    * @param aObject this class.
    * @return whether the timer should continue.
    */
    static TInt TimerCallBackL(TAny* aObject);

    /**
    * The non-static timer function that does the work.
    * @return whether the timer should continue.
    */
    TInt DoCallBackL();

public: // Own public data
    /**
    * The background bitmap.
    */
    CFbsBitmap* iBmpBackground;

    /**
    * An instance of the story class containing the game information.
    */
    CJourneyStory* iStory;
    
    /**
    * Specifies whether a search or wait is currently active.
    */
    TBool iSearchActive;
private:

    /**
    * An instance of the location tracking and management class.
    */
    CJourneyLocation* iLocation;
    
    /**
    * A pointer back to the owning container class.
    */
    CJourneyCGame* iOwningControl;

    /**
    * The periodic timer used to do periodic checks of the cell id.
    */
    CPeriodic* iPeriodicTimer;

    /**
    * The file server for saving the game progress.
    */
    RFs iFileServerSession;

    /**
    * How far the player has already progressed in the game. Is the position
    * in the iTalkInfos array of the CJourneyStory class.
    */
    TInt iGameProgress;

    /**
    * The number of the area of the previous story element. If the area stays the same
    * between two story elements, no check if the user might have left the area is conduced.
    * This is to let frequent cell id switches at cell area borders not interrupt the game.
    */
    TInt iPrevAreaNum;

    /**
    * The number of the text resource currently displayed in the text box.
    */
    TInt iCurTextResId;

    /**
    * This variable is set to true when the game is first started. This is used in the following way:
    * When no cell id can be determined (for example no reception), the should display an
    * information message - but only when starting the game. If there is no reception for
    * a short time within the game, this should not grab the player out of the game experience.
    */
    TBool iJustStartedGame;

	/**
	 * Saves the error state if there has been a problem with getting the location.
	 * This can eiter be KErrNone, the error code returned directly by the system interface
	 * or a custom error defined in the callback interface.
	 */
	TInt iErrorState;
    
};

#endif
