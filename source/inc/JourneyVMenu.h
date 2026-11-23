/*
* ============================================================================
*  Name     : CJourneyVMenu from JourneyVMenu.h
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

#ifndef JOURNEYVMENU_H
#define JOURNEYVMENU_H

// INCLUDES
#include <aknview.h>
#include <eikmenup.h> // for the menu pane

#include "JourneyAppUi.h"
#include "SharedSettings.h"

// CONSTANTS
// UID of view
const TUid KMenuViewId =
    {
        1
    };


// FORWARD DECLARATIONS
class CJourneyCMenu;

// CLASS DECLARATION

/**
*  CJourneyVMenu view class.
*
*/
class CJourneyVMenu : public CAknView
{
public: // Constructors and destructor

	/**
	* EPOC default constructor.
	*/
	void ConstructL();

	/**
	* Destructor.
	*/
	~CJourneyVMenu();

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

	// From MEikMenuObserver
	/**
	* Handles hiding the continue game when building the menu.
	*/
	void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

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

public:  // Own functions
	void ShowLoadingText(const TBool aShowText);

private: // Data
	/**
	* Pointer to the container of this view.
	*/
	CJourneyCMenu* iContainer;
};

#endif

// End of File
