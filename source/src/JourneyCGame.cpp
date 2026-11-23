/*
* ============================================================================
*  Name     : CJourneyCGame from JourneyCGame.cpp
*  Part of  : Journey
*  Created  : 04.05.2004 by Andreas Jakl
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

// INCLUDE FILES
#include "JourneyCGame.h"


// ================= MEMBER FUNCTIONS =======================

CJourneyCGame::CJourneyCGame(CJourneyVGame* aView)
: iView(aView)
	{
	
	}

// ---------------------------------------------------------
// CJourneyCGame::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CJourneyCGame::ConstructL(const TRect& /*aRect*/)
{
	CreateWindowL();
	// Get the fullscreen-size
	TRect screenRect = CEikonEnv::Static()->EikAppUi()->ApplicationRect();

	iLocationNameResId = 0;
	iCurLocationBmp = NULL;
	iDrawOkSymbol = EFalse;

	// Create backbuffer-bitmap (fullscreen)
	CreateBackBufferL(screenRect.Size());

	// The text box to display the game text
	// The size will be calculated later on - create the text
	// box with an arbritary size.
	iTextBox = CTextBox::NewL(TRect(0, 0, 10, 10), this);
	iTextBox->ShowScrollBar(ETrue);

	// Create the game engine which handles the game logic
	iEngine = CJourneyEngine::NewL(this);
	iEngine->LoadArtL();

#ifdef __S60_5X__
    iFeedback = MTouchFeedback::Instance();

    if (iFeedback)
    	{
		// Set hit tester to be object itself
		SetHitTest( this );
		// Set feedback area for this control
		iFeedback->SetFeedbackArea( this, 0, Rect(),
			ETouchFeedbackBasic, ETouchEventStylusDown );
	
		iFeedback->EnableFeedbackForControl( this, ETrue );
    	}
#endif
    
	// Set the window to fullscreen including the softkey-bar, not to the rectangle
	// that the application gets with this function (which would not draw over the
	// softkey-bar).
	SetExtentToWholeScreen();	// Results in a call of SizeChanged()
	ActivateL();
}

// Destructor
CJourneyCGame::~CJourneyCGame()
{
	DeleteBackBuffer();

	DELETE_AND_NULLZ(iCurLocationBmp);
	DELETE_AND_NULLZ(iEngine);
	DELETE_AND_NULLZ(iTextBox);

#ifdef __S60_5X__
    if ( iFeedback )
        {
        iFeedback->RemoveFeedbackForControl( this );
        }
#endif
}

void CJourneyCGame::DeleteBackBuffer()
{
	if (iBackBufferBmp) 
		AknIconUtils::DestroyIconData(iBackBufferBmp);
	if (iBackBufferBmpDrawingDevice)
		DELETE_AND_NULLZ(iBackBufferBmpDrawingDevice);
	if (iBackBufferBmpGc)
		DELETE_AND_NULLZ(iBackBufferBmpGc);
	if (iBackBufferBmp)
		DELETE_AND_NULLZ(iBackBufferBmp);
}

void CJourneyCGame::CreateBackBufferL(const TSize aSize)
{
	DeleteBackBuffer();
	iBackBufferBmp = new (ELeave) CFbsBitmap;
	User::LeaveIfError(iBackBufferBmp->Create(aSize, EColor64K));
	iBackBufferBmpDrawingDevice = CFbsBitmapDevice::NewL( iBackBufferBmp );
	//create graphics context for the bitmap
	User::LeaveIfError( iBackBufferBmpDrawingDevice->CreateContext(iBackBufferBmpGc) );
	// Remove white border that might be added around rasterized SVG-images
	// by the converter
	NBitmapMethods::ReplaceColor(iBackBufferBmp, TRgb(255, 255, 255)._Color64K(), 0);
	// To prevent re-loading of the bitmap from the storage medium
	AknIconUtils::PreserveIconData(iBackBufferBmp);
}

// ---------------------------------------------------------
// CJourneyCGame::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CJourneyCGame::SizeChanged()
{
	const TSize newScreenSize = this->Size();
	AknLayoutUtils::TAknCbaLocation cbaLocation = AknLayoutUtils::CbaLocation();
	// Resize the back buffer bitmap
	CreateBackBufferL(newScreenSize);
	
	// Calculate some common variables
	const TBool landscapeMode = (newScreenSize.iWidth > newScreenSize.iHeight);
	const TInt leftX = (landscapeMode) ? newScreenSize.iHeight / 10 : newScreenSize.iWidth / 10;
	const TInt rightX = newScreenSize.iWidth - leftX;
	const TInt widthX = rightX - leftX;
	const TInt screenCenterX = newScreenSize.iWidth / 2;
	
	// Change background size
	if (iEngine->iBmpBackground)
		{
		NBitmapMethods::setSvgSize(iEngine->iBmpBackground, NBitmapMethods::SetSizeToFullScreen(newScreenSize, iEngine->iBmpBackground));
		}
	
	// Get some font parameters	
	const CFbsFont* smallFont = (CFbsFont*)AknLayoutUtils::LayoutFontFromId(EAknLogicalFontPrimarySmallFont);
	const CFbsFont* fontNormal = (CFbsFont*)AknLayoutUtils::LayoutFontFromId(EAknLogicalFontPrimaryFont);
	const CFbsFont* fontTitle  = (CFbsFont*)AknLayoutUtils::LayoutFontFromId(EAknLogicalFontTitleFont);
	const TInt fontNormalHeight = fontNormal->HeightInPixels();
	const TInt halfFontNormalHeight = fontNormalHeight / 2;
	const TInt fontTitleHeight = fontTitle->HeightInPixels();
	const TInt lineSpacing = fontNormal->FontLineGap();
	// Additional border around text boxes for the darkened area
	iDarkenBorder = fontNormal->CharWidthInPixels('i');
	
	// Adapt layout to the background
	TInt curY = 0;
	// Current location text (title)
	iLayoutRects[ETitle].SetRect(leftX, curY, rightX, curY + fontTitleHeight);
	
	// Image (e.g.: 475x198, Aspect (w/h) = 2.4)
	TInt imageSpacing = lineSpacing;
	if (landscapeMode)
	{
		// Landscape mode
		// We don't have so much vertical room in landscape mode - make the
		// vertical space between the image and the text smaller
		imageSpacing /= 2;
	}
	curY += fontTitleHeight + imageSpacing;
	
	// The aspect ratio of the images is w:h = 2.4
	const TReal aspectRatio = 2.4;
	TReal imgWidth = (TReal)widthX;
	TReal imgHeight = imgWidth / aspectRatio;
	// Check if the image is not more than 30% of the screen height
	// If it is, make it smaller
	if (imgHeight / (TReal)newScreenSize.iHeight > 0.3)
	{
		imgHeight = (TReal)newScreenSize.iHeight * 0.3;
		imgWidth = imgHeight * aspectRatio;
	}
	const TInt imgWidthIntHalf = (TInt)(imgWidth / 2.0);
	const TInt imgHeightInt = (TInt)imgHeight;
	iLayoutRects[EImage].SetRect(screenCenterX - imgWidthIntHalf, curY, screenCenterX + imgWidthIntHalf, curY + imgHeightInt);
	curY += imgHeightInt + imageSpacing;
	
	// Spacing
	const TInt spacing = leftX / 4;
	
	// Continue-Circle
	iLayoutRects[EContinueImg].SetRect(screenCenterX - spacing, 
		newScreenSize.iHeight - halfFontNormalHeight - spacing,
		screenCenterX,
		newScreenSize.iHeight - halfFontNormalHeight);
		
	// Continue-Text

	TBuf<30> textResource;
	CEikonEnv::Static()->ReadResource(textResource, iEngine->iStory->iContinueTextRes);
	TInt continueTextWidth = smallFont->TextWidthInPixels(textResource);

	iLayoutRects[EContinueText].SetRect(screenCenterX + spacing, 
		newScreenSize.iHeight - fontNormalHeight,
		screenCenterX + spacing + continueTextWidth,
		newScreenSize.iHeight);

	iLayoutRects[EContinueTouch].SetRect(iLayoutRects[EContinueImg].iTl.iX - spacing,
			iLayoutRects[EContinueText].iTl.iY - fontNormalHeight,
			iLayoutRects[EContinueText].iBr.iX + spacing,
			iLayoutRects[EContinueText].iBr.iY);
		
	// Softkey-Arrow
	// The following code creates a rectangle based on the softkey label rectangle
	// It takes the position of the text label, but is a lot smaller and quadratic so that it
	// can contain the arrow.
	if (cbaLocation == AknLayoutUtils::EAknCbaLocationBottom)
	{
		// Portrait mode
		AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EControlPane, iLayoutRects[ESoftkeyArea]);
		if (landscapeMode)
			{
			// Landscape mode on S^3 - fix the softkeys
			const TInt skHeight = iLayoutRects[ESoftkeyArea].Height();
			const TInt skBottomY = newScreenSize.iHeight;
			const TInt skTopY = skBottomY - skHeight;
			iLayoutRects[ESoftkeyArea].SetRect(iLayoutRects[ESoftkeyArea].iTl.iX, skTopY, iLayoutRects[ESoftkeyArea].iBr.iX, skBottomY);
			}
		iLayoutRects[ESoftkey] = iLayoutRects[ESoftkeyArea];
		iLayoutRects[ESoftkey].iBr.iY -= spacing;
		iLayoutRects[ESoftkey].iBr.iX -= spacing;
		iLayoutRects[ESoftkey].iTl.iX = iLayoutRects[ESoftkey].iBr.iX - spacing * 2;
		iLayoutRects[ESoftkey].iTl.iY = iLayoutRects[ESoftkey].iBr.iY - spacing * 2;
		iSoftkeyArrowDir = NBitmapMethods::EDown;
	}
	else
	{
		// Landscape mode
		// We only need one softkey here
		// It should be on the secondary softkey -> top for n95, bottom for e90
		// SoftkeyOnTop queries the position of the primary softkey
		// Therefore, it's inversed.
		const TBool softkeyOnTop = !IsOptionsButtonOnTop();
		if (softkeyOnTop)
		{
			AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EControlPaneTop, iLayoutRects[ESoftkeyArea]);
		}
		else
		{
			AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EControlPaneBottom, iLayoutRects[ESoftkeyArea]);
			// Fix for wrong (?) rectangle returned by 3rd Edition SDK
			TRect tmpTopHeight;
			AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EControlPaneTop, tmpTopHeight);		
			iLayoutRects[ESoftkeyArea].iTl.iY = iLayoutRects[ESoftkeyArea].iBr.iY - tmpTopHeight.Height();
		}

		if (iLayoutRects[ESoftkeyArea].iTl.iX == 0 && iLayoutRects[ESoftkeyArea].iBr.iX == 0 ||
				iLayoutRects[ESoftkeyArea].iBr.iY > newScreenSize.iHeight)
			{
			// Fix for wrong rectangles returned by 5th Edition SDK (1.0) and 5800xm
			const int skHeight = newScreenSize.iHeight / 5;//f->FontMaxHeight() * 2;
			const int skWidth = skHeight * 2;
			// Vertical softkey placement - same for left and right softkeys
			// SK1: Top
			TRect sk1Rect;
			sk1Rect.iTl.iY = 0;
			sk1Rect.iBr.iY = sk1Rect.iTl.iY + skHeight;
			// SK2: Bottom
			TRect sk2Rect;
			sk2Rect.iBr.iY = newScreenSize.iHeight;
			sk2Rect.iTl.iY = sk2Rect.iBr.iY - skHeight;
			// Horizontal sofktey placement - different for right or left softkey positions
			if (cbaLocation == AknLayoutUtils::EAknCbaLocationLeft)
				{
				// Left side of the screen
				sk1Rect.iTl.iX = 0;
				sk1Rect.iBr.iX = skWidth;
				}
			else
				{
				// Right side of the screen
				sk1Rect.iBr.iX = newScreenSize.iWidth;
				sk1Rect.iTl.iX = sk1Rect.iBr.iX - skWidth; 
				}
			// Copy horizontal placement from SK1 to SK2 - they're below each
			// other, so it's the same.
			sk2Rect.iTl.iX = sk1Rect.iTl.iX;
			sk2Rect.iBr.iX = sk1Rect.iBr.iX;
			// Assign correct softkey to needed layout rectangle
			iLayoutRects[ESoftkeyArea] = (softkeyOnTop)? sk1Rect : sk2Rect;
			}
		
		iLayoutRects[ESoftkey] = iLayoutRects[ESoftkeyArea];
		
		if (cbaLocation == AknLayoutUtils::EAknCbaLocationLeft)
		{
			// Softkey label has to be on the left side, arrow should point to the left
			iSoftkeyArrowDir = NBitmapMethods::ELeft;
			iLayoutRects[ESoftkey].iTl.iX += spacing;
			iLayoutRects[ESoftkey].iBr.iX = iLayoutRects[ESoftkey].iTl.iX + spacing;
			if (softkeyOnTop)
				iLayoutRects[ESoftkey].iBr.iY = iLayoutRects[ESoftkey].iTl.iY + spacing;
			else
				iLayoutRects[ESoftkey].iBr.iY = iLayoutRects[ESoftkey].iTl.iY - spacing;
		}
		else
		{
			// Softkey label has to be on the right side, arrow should point to the right
			iSoftkeyArrowDir = NBitmapMethods::ERight;
			if (softkeyOnTop)
				iLayoutRects[ESoftkey].iTl.iY += spacing;
			else
				iLayoutRects[ESoftkey].iTl.iY -= spacing * 2;
			
			iLayoutRects[ESoftkey].iBr.iX -= spacing;
			iLayoutRects[ESoftkey].iTl.iX = iLayoutRects[ESoftkey].iBr.iX - spacing;
			iLayoutRects[ESoftkey].iBr.iY = iLayoutRects[ESoftkey].iTl.iY + spacing;
		}
	}

	// Text
	TInt textRectBottomY = newScreenSize.iHeight - fontNormalHeight;
	if (cbaLocation == AknLayoutUtils::EAknCbaLocationBottom)
		{
		if (landscapeMode)
			{
				textRectBottomY = newScreenSize.iHeight - iLayoutRects[ESoftkeyArea].Height();
			} else {
				textRectBottomY -= iLayoutRects[ESoftkeyArea].Height();
			}
		}
	iLayoutRects[EText].SetRect(leftX, curY, rightX, textRectBottomY);
	
	// Send new size to the text box control and the image
	if (iCurLocationBmp)
	{
		NBitmapMethods::setSvgSize(iCurLocationBmp, iLayoutRects[EImage].Size());
	}
	if (iTextBox)
	{
		iTextBox->SetDarkenBorder(iDarkenBorder);
		iTextBox->SetExtent(iLayoutRects[EText].iTl, iLayoutRects[EText].Size());
	}
}


TBool CJourneyCGame::IsOptionsButtonOnTop()
{
	CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
	if( !cba )
    {
    	return EFalse;
    }
  
	CCoeControl* options = cba->ControlOrNull( EJourneyCmdMenu );
	CCoeControl* exit = cba->ControlOrNull( EJourneyCmdExit );
  
	if( options && exit )
    {
		if( options->Position().iY < exit->Position().iY )
		{
			return ETrue;
		}
	}
	return EFalse;
}

// ---------------------------------------------------------
// CJourneyCGame::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CJourneyCGame::CountComponentControls() const
{
	return 1; // return nbr of controls inside this container
}

// ---------------------------------------------------------
// CJourneyCGame::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CJourneyCGame::ComponentControl(TInt aIndex) const
{
	switch ( aIndex )
	{
	case 0:
		return iTextBox;
	default:
		return NULL;
	}
}

// ---------------------------------------------------------
// CJourneyCGame::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CJourneyCGame::Draw(const TRect& aRect) const
{
	TPoint bmpPos(0,0);
	TBuf<30> textResource;

	// Initialize
	CWindowGc& gc = SystemGc();

	iBackBufferBmpGc->SetBrushColor(0);
	iBackBufferBmpGc->SetPenColor(KTextColor);
	iBackBufferBmpGc->SetPenStyle(CGraphicsContext::ESolidPen);

	// Background image
	iBackBufferBmpGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	iBackBufferBmpGc->BitBlt(bmpPos, iEngine->iBmpBackground);
	
	// Security check, only draw title text if one has already been displayed
	// and a resource number is known.
	if (iLocationNameResId > 0)
	{
		const CAknLayoutFont* titleFont = AknLayoutUtils::LayoutFontFromId(EAknLogicalFontTitleFont);	
		CFbsFont* titleF = (CFbsFont*) titleFont;
		
		// Darken the area behind the title
		if (iDarkenBorder >= 0)
			NBitmapMethods::DarkenArea(iBackBufferBmp, iLayoutRects[ETitle], iDarkenBorder, 2);
			
		// Print location name text
		CEikonEnv::Static()->ReadResource(textResource, iLocationNameResId);

		//iBackBufferBmpGc->UseFont(iEikonEnv->AnnotationFont());
		iBackBufferBmpGc->UseFont((CFont*) titleFont);
		iBackBufferBmpGc->DrawText(textResource, iLayoutRects[ETitle], titleF->FontMaxAscent(), CGraphicsContext::ECenter);
		iBackBufferBmpGc->DiscardFont();
	}
	
	// Location bitmap (if it's available)
	if (iCurLocationBmp)
		{
		iBackBufferBmpGc->SetBrushStyle(CGraphicsContext::ENullBrush);
		// TODO: Try if it helps to set the brush of the curlocationbmp to null
		iBackBufferBmpGc->BitBlt(iLayoutRects[EImage].iTl, iCurLocationBmp);
		}
	
	// Draw the text box	
	iTextBox->Draw(iBackBufferBmp, iBackBufferBmpGc);
	
	// Continue
	if (iDrawOkSymbol)
	{
		// Draw the OK-symbol
		iBackBufferBmpGc->SetBrushColor(KTextColor);
		iBackBufferBmpGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		iBackBufferBmpGc->SetPenColor(KTextColor);
		iBackBufferBmpGc->SetPenStyle(CGraphicsContext::ESolidPen);
		iBackBufferBmpGc->DrawEllipse(iLayoutRects[EContinueImg]);
		// Deactivate the brush again
		iBackBufferBmpGc->SetBrushStyle(CGraphicsContext::ENullBrush);

		// Print continue-text
		CEikonEnv::Static()->ReadResource(textResource, iEngine->iStory->iContinueTextRes);
		const CAknLayoutFont* smallFont = AknLayoutUtils::LayoutFontFromId(EAknLogicalFontPrimarySmallFont);
		CFbsFont* smallF = (CFbsFont*) smallFont;
		iBackBufferBmpGc->UseFont((CFont*) smallFont);
		iBackBufferBmpGc->DrawText(textResource, iLayoutRects[EContinueText], smallF->FontMaxAscent(), CGraphicsContext::ELeft);
		iBackBufferBmpGc->DiscardFont();
	}
	
	// Softkey-Label
	iBackBufferBmpGc->SetPenColor(KTextColorDark);

	// Check if touch is enabled or not
	// Would be recommended to use PenEnabled directly -
	// but then, the application can't be compiled with the 3rd Ed.-SDK anymore :(
	// -> This approach is used from the FN Discussion boards
	TBool penEnabled = EFalse;

	RLibrary avkonDll;
	if ( avkonDll.Load( _L( "avkon.dll" ) ) == KErrNone )
	{
		#ifdef __WINS__
		TLibraryFunction PenEnabled = avkonDll.Lookup( 3184 );
		#else
		TLibraryFunction PenEnabled = avkonDll.Lookup( 4251 );
		#endif
	
		if ( PenEnabled != NULL )
		{
			penEnabled = PenEnabled();
		}
	
		avkonDll.Close();
	}

    if( !penEnabled )
    	{
    	// Draw arrow pointing to softkey for non-touch devices
    	NBitmapMethods::DrawArrow(iBackBufferBmpGc, iLayoutRects[ESoftkey], iSoftkeyArrowDir);
	} else
		{
		// Draw menu symbol for touch devices
    	NBitmapMethods::DrawMenuSymbol(iBackBufferBmpGc, iLayoutRects[ESoftkey]);
		}
	
	// Blit the back buffer to the screen
	gc.BitBlt(TPoint(aRect.iTl), iBackBufferBmp, aRect);
}

//------------------------------------------------------------------------------
void CJourneyCGame::UpdateText(TInt aTextResourceId)
{
	iTextBox->SetTextL(aTextResourceId);
	if (!iEngine->iSearchActive && !iTextBox->IsScrollDownPossible())
    {
      	// If we're not currently waiting for another area (-> normal ingame)
       	// and reached the end of the text, enable the ok-button
       	iDrawOkSymbol = ETrue;
    }
	ForceRedraw();

}

//------------------------------------------------------------------------------
void CJourneyCGame::DrawLocationText(TInt aTextResourceId)
{
	// This function only updates the location text at the top of the screen.
	iLocationNameResId = aTextResourceId;
	ForceRedraw();
}

//------------------------------------------------------------------------------
void CJourneyCGame::DrawLocationBitmapL(TInt aBmpEnum)
{
	// This function displays a new location bitmap. The ID of it in the mif-file
	// is passed as a parameter, this function creates the bitmap and stores it
	// as a member variable so that the Draw()-function can easily draw it without
	// needing to call leaving functions.
	TFileName fullName;
	NJourneyFunctions::AppendFullPath(KJourneyMif, fullName);

	if (iCurLocationBmp)
		delete iCurLocationBmp;
	
	// Create the bitmap based on the vector file
	iCurLocationBmp = AknIconUtils::CreateIconL(fullName, aBmpEnum);
	// Specify its size according to the current screen size
	NBitmapMethods::setSvgSize(iCurLocationBmp, iLayoutRects[EImage].Size());
		
	ForceRedraw();
}

//------------------------------------------------------------------------------
void CJourneyCGame::ForceRedraw()
{
	Window().Invalidate();
}

// ---------------------------------------------------------
// CJourneyCGame::HandleControlEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//
void CJourneyCGame::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
{}


//------------------------------------------------------------------------------
void CJourneyCGame::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{
    // Check if touch is enabled or not
	TBool penEnabled = EFalse;

	RLibrary avkonDll;
	if ( avkonDll.Load( _L( "avkon.dll" ) ) == KErrNone )
	{
		#ifdef __WINS__
		TLibraryFunction PenEnabled = avkonDll.Lookup( 3184 );
		#else
		TLibraryFunction PenEnabled = avkonDll.Lookup( 4251 );
		#endif
	
		if ( PenEnabled != NULL )
		{
			penEnabled = PenEnabled();
		}
	
		avkonDll.Close();
	}

    if( !penEnabled )
        {
        return;
        }
    
    if ( aPointerEvent.iType==TPointerEvent::EButton1Up )
		{
		TPoint pos = aPointerEvent.iPosition;
		if(iDrawOkSymbol && iLayoutRects[EContinueTouch].Contains(pos))
			{
			// OK event
			HandleOkPressedL();
			Window().Invalidate();
			}
		else if (iLayoutRects[ESoftkeyArea].Contains(pos))
			{
			// Menu event
			iView->ProcessCommandL(EAknSoftkeyOptions);
			}
		else if (iTextBox)
			{
			// Textbox will check itself if the event is affecting it
			iTextBox->HandlePointerEventL(aPointerEvent);
            if (!iEngine->iSearchActive && !iTextBox->IsScrollDownPossible())
            {
            	// If we're not currently waiting for another area (-> normal ingame)
            	// and reached the end of the text, enable the ok-button
            	iDrawOkSymbol = ETrue;
            }
			Window().Invalidate();
			}
		}

    CCoeControl::HandlePointerEventL( aPointerEvent );
	}

//------------------------------------------------------------------------------
TKeyResponse CJourneyCGame::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
	TKeyResponse returnVal = EKeyWasNotConsumed;
	if (aType == EEventKeyDown)
	{
		returnVal = EKeyWasConsumed;
		switch ( aKeyEvent.iScanCode )
		{
		case EStdKeyDownArrow:
		{
			// Scroll down
            iTextBox->ScrollDown();
            if (!iEngine->iSearchActive && !iTextBox->IsScrollDownPossible())
            {
            	// If we're not currently waiting for another area (-> normal ingame)
            	// and reached the end of the text, enable the ok-button
            	iDrawOkSymbol = ETrue;
            }
			ForceRedraw();
			break;
		}
		case EStdKeyUpArrow:
		{
			// Scroll up
            iTextBox->ScrollUp();
			ForceRedraw();
			break;
		}
		case EStdKeyDevice3:		// OK (Softkey center)
		case EStdKeyEnter:
		case EStdKeyNkpEnter:
		{
			HandleOkPressedL();
			break;
		}

		default:
			returnVal = EKeyWasNotConsumed;
			break;
		}
	}
	return(returnVal);
}

void CJourneyCGame::HandleOkPressedL()
	{
	if (iDrawOkSymbol)
		{
			// If user is allowed to continue, handle the joystick press
			iDrawOkSymbol = EFalse;
			if (iTextBox)
			{
				iTextBox->DimText(ETrue);
				ForceRedraw();
			}
			iEngine->HandleJoystickPressL();
		}
	}

void CJourneyCGame::HandleResourceChange(TInt aType)
{
	CCoeControl::HandleResourceChange(aType);
	if(aType == KEikDynamicLayoutVariantSwitch)
	{
		// User switched the layout configuration or the screen resolution :)
		// -> we have to recreate the layout
		SetExtentToWholeScreen();	// Results in a call of SizeChanged()
	}
}


#ifdef __S60_5X__
// ---------------------------------------------------------------------------
// From class MCoeControlHitTest.
// Tests whether a pointer event occurred inside the control's hit region
// ---------------------------------------------------------------------------
//
TBool CJourneyCGame::HitRegionContains( const TPoint& aPoint,
      const CCoeControl& /*aControl*/ ) const
    {
    return IsVisible() ? Rect().Contains( aPoint ) : EFalse;
    }
#endif

// End of File
