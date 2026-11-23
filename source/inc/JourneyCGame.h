/*
* ============================================================================
*  Name     : CJourneyCGame from JourneyCGame.h
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

#ifndef JOURNEYCGAME_H
#define JOURNEYCGAME_H

// INCLUDES
#include <coecntrl.h>
#include <eikenv.h>

#include <AknUtils.h> // For Font names - Series 60 platform dependent code
#include <akniconutils.h> 	// To convert svg to bitmaps
#include <journey.rsg>

//#include <Journey.mbg>      // Vector graphics enumeration
#include <eikappui.h>
#include <eikapp.h>

#ifdef __S60_5X__
#include <touchfeedback.h>
#endif

#include "Journey.hrh"
#include "SharedSettings.h"
#include "JourneyEngine.h"
#include "JourneyFunctions.h"
#include "TextBox.h"

#include "JourneyVGame.h"

// FORWARD DECLARATIONS
class CJourneyEngine;

#define DELETE_AND_NULLZ(p)		{delete p; p = NULL;}

// CLASS DECLARATION

/**
*  CJourneyCGame.
*  Container control class.
*/
class CJourneyCGame : public CCoeControl, MCoeControlObserver
#ifdef __S60_5X__
, MCoeControlHitTest
#endif
{
public: // Constructors and destructor

	CJourneyCGame(CJourneyVGame* aView);

	/**
	* EPOC default constructor.
	* @param aRect Frame rectangle for container.
	*/
	void ConstructL(const TRect& aRect);

	/**
	* Destructor.
	*/
	~CJourneyCGame();

public: // New functions
	/**
	* Draws the name of the current location and forces a 
	* screen update.
	* @param aTextResourceId ID of the TBuf that the
	* resource reader will fetch from the resource file.
	*/
	void DrawLocationText(TInt aTextResourceId);

	/**
	* Draws the bitmap of the current location / story related
	* picture. Forces a screen update.
	* @param aBmpEnum ID of the bitmap in the .mbm-file.
	*/
	void DrawLocationBitmapL(TInt aBmpEnum);

	/**
	* Updates the text displayed in the story text box.
	*/
	void UpdateText(TInt aTextResourceId);

public: // Functions from base classes

private: // CCoeControl
	/** Handle individual keys. */
	TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

    /**
     * From CCoeControl.
     * @param aPointerEvent The pointer event.
     */
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	
	/** Called when the screen layout changes. */
	void HandleResourceChange(TInt aType);

private: // Functions from base classes

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

#ifdef __S60_5X__
    /**
     * From MCoeControlHitTest.
     * Tests whether a pointer event occurred inside the control's hit region
     * @param aPoint The position of the pointer event
     * @param aControl
     */
    TBool HitRegionContains( const TPoint& aPoint, const CCoeControl& /*aControl*/ ) const;
#endif
private: // Own functions
	/**
	* Forces a redraw event for the whole window.
	*/
	void ForceRedraw();

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
	
	/**
	 * Handle what should happen when the ok button is pressed.
	 */
	void HandleOkPressedL();
public:
	/**
	* A pointer to the game engine which manages the gameplay.
	*/
	CJourneyEngine* iEngine;

private: //data
	/**
	* The text box.
	*/
	CTextBox* iTextBox;

	/**
	* Storing the currently displayed text resource id so that the text
	* can be restored in the case of a redraw event.
	*/
	TInt iLocationNameResId;

	/**
	* Storing the bitmap of the currently displayed location, to be able
	* to draw it again in the case of a redraw event.
	*/
	CFbsBitmap* iCurLocationBmp;

	/** Whether to draw the ok button label. */
	TBool iDrawOkSymbol;

	/** Back buffer bitmap */
	CFbsBitmap* iBackBufferBmp;

	/** Graphics context of back buffer bitmap  */
	CFbsBitGc* iBackBufferBmpGc;

	/** Drawing Device of back buffer bitmap. */
	CFbsBitmapDevice* iBackBufferBmpDrawingDevice;

	/** Size of the border around text boxes which should be darkened as well. */
	TInt iDarkenBorder;

	/** Stores the rectangles of various UI-elements. */
	TFixedArray<TRect, 8> iLayoutRects;

	/** Used to access the elements in the iLayoutRects-array. */
	typedef enum
	{
	    ETitle = 0,
	    EImage,
	    EText,
	    EContinueImg,	// Image for the continue text
	    EContinueText,	// Rectangle for the continue text
	    EContinueTouch,	// Touch area for continue text, contains both text + image
	    ESoftkey,	// Rectangle of the small downwards arrow
	    ESoftkeyArea	// Rectangle of the touch area
	} TLayoutElements;

	/** Saves the direction in which the softkey label should point. Important for landscape mode. */
	NBitmapMethods::TArrowDirection iSoftkeyArrowDir;

#ifdef __S60_5X__
    /**
     * Pointer to touch feedback instance
     * Not owned.
     */
    MTouchFeedback* iFeedback;
#endif
    
	CJourneyVGame* iView;
};

#endif

// End of File
