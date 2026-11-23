/*
* ============================================================================
*  Name     : CJourneyCMenu from JourneyCMenu.h
*  Part of  : Journey
*  Created  : 04.05.2004 by Andreas Jakl
*  Description:
*     Declares container control for application.
*  Version  : 1.32
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

#ifndef JOURNEYCMENU_H
#define JOURNEYCMENU_H

// INCLUDES
#include <coecntrl.h>

#include "bitmapmethods.h"
#include "JourneyFunctions.h"
#include <akniconutils.h>
#include <aknutils.h>
#include <w32std.h>
#include <aknmessagequerydialog.h>
//#include <e32debug.h>	// For using RDebug

#include "Journey.hrh"
#include "TextBox.h"
#include "SharedSettings.h"
#include <journey.rsg>
#include <Journey.mbg>      // Bitmap enumeration
#include "JourneyVMenu.h"

#define DELETE_AND_NULLZ(p)		{delete p; p = NULL;}

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  CJourneyCMenu
*  Container control class.
*/
class CJourneyCMenu : public CCoeControl, MCoeControlObserver
{
public: // Constructors and destructor
	CJourneyCMenu(TJourneySharedData& aSharedData, CJourneyVMenu* aView);

	/**
	* EPOC default constructor.
	* @param aRect Frame rectangle for container.
	*/
	void ConstructL(const TRect& aRect);

	/**
	* Destructor.
	*/
	~CJourneyCMenu();

public: // New functions
	void ShowLoadingText(const TBool aShowText);

public: // Functions from base classes

private: // Functions from base classes
	/** Handle individual keys. */
	TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

    /**
     * From CCoeControl.
     * @param aPointerEvent The pointer event.
     */
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	
	/**
	 * From CoeControl,SizeChanged.
	 */
	void SizeChanged();

	/**
	 * From CoeControl,CountComponentControls.
	 */
	TInt CountComponentControls() const;

	/**
	 * From CCoeControl,ComponentControl.
	 */
	CCoeControl* ComponentControl(TInt aIndex) const;

	/**
	 * From CCoeControl,Draw.
	 */
	void Draw(const TRect& aRect) const;

	/**
	 * Handle events from controls that we observe.
	 */
	void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);

	/**
	 * Used for handling screen layout and resoultion changes.
	 */
	void HandleResourceChange(TInt aType);

private:  // New functions
	/**
	* Load the intro screen bitmap into the member variable.
	*/
	void LoadArtL();

	/**
	 * Delete the back buffer bitmap and its other cached objects (context, device).
	 */
	void DeleteBackBuffer();

	/**
	 * Create a back buffer bitmap (and context + device) with the specified size.
	 */
	void CreateBackBufferL(TSize aSize);
	
	/**
	 * Find out if the options botton is at the top in landscape mode.
	 */
	TBool IsOptionsButtonOnTop();
	
	void CloseTextBox();
private: //data
	/**
	* Member variable storing the intro screen bitmap background.
	*/
	CFbsBitmap* iBmpIntroScreenBg;
	
	/**
	* Member variable storing the logo text.
	*/
	CFbsBitmap* iBmpIntroScreenText;

	/**
	* Member variable storing the mask of the logo text.
	*/
	CFbsBitmap* iBmpIntroScreenTextM;

	/** Back buffer bitmap */
	CFbsBitmap* iBackBufferBmp;

	/** Graphics context of back buffer bitmap  */
	CFbsBitGc* iBackBufferBmpGc;

	/** Drawing Device of back buffer bitmap. */
	CFbsBitmapDevice* iBackBufferBmpDrawingDevice;

	/** Position and size of the first (options, ok) softkey. */
	TRect iSk1Rect;
	/** Text alignment of the first softkey. Important because of landscape mode. */
	CGraphicsContext::TTextAlign iSk1Align;
	/** Position and size of the second (back, cancel) softkey. */
	TRect iSk2Rect;
	/** Text alignment of the second softkey. Important because of landscape mode. */
	CGraphicsContext::TTextAlign iSk2Align;

	/** Size of the border around text boxes which should be darkened as well. */
	TInt iDarkenBorder;

	/** Interface containing some global settings. */
	TJourneySharedData& iSharedData;
	
	/** Custom text box control for displaying the game text. */
	CTextBox* iTextBox;
	
	/** Page of the info text that is currently visible. */
	TInt iTextBoxPage;
	
	/** Whether to draw the ok button label. */
	TBool iDrawOkSymbol;
	
	/** Whether to dim the screen and show the loading text on top of it. */
	TBool iShowLoadingText;
	
	/** Pointer to the view object. */
	CJourneyVMenu* iView;
};

#endif

// End of File
