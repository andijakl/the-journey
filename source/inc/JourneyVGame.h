/*
* ============================================================================
*  Name     : CJourneyVGame from JourneyVGame.h
*  Part of  : Journey
*  Created  : 04.05.2004 by Andreas Jakl
*  Description:
*     Declares view for application.
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

#ifndef JOURNEYVGAME_H
#define JOURNEYVGAME_H

// INCLUDES
#include <aknview.h>
#include <aknquerydialog.h>


// CONSTANTS
// UID of view
const TUid KGameViewId = {2};

// FORWARD DECLARATIONS
class CJourneyCGame;

// CLASS DECLARATION

/**
*  CJourneyVGame view class.
* 
*/
class CJourneyVGame : public CAknView
    {
    public: // Constructors and destructor

        /**
        * EPOC default constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CJourneyVGame();

    public: // Functions from base classes
        
        /**
        * From ?base_class ?member_description
        */
        TUid Id() const;

        /**
        * From ?base_class ?member_description
        */
        void HandleCommandL(TInt aCommand);

        /**
        * From ?base_class ?member_description
        */
        void HandleClientRectChange();


    private:

        /**
        * From AknView, ?member_description
        */
        void DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
            const TDesC8& aCustomMessage);

        /**
        * From AknView, ?member_description
        */
        void DoDeactivate();

	private:	// Own functions
        /**
        * Displays the dialog whether to continue the game or to quit it.
        */
		TInt DoPauseDlgL();

    private: // Data
        /**
        * Pointer to the container of this view.
        */
        CJourneyCGame* iContainer;

    };

#endif

// End of File
