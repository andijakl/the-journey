/*
 * ============================================================================
 *  Name     : CJourneyCMenu from JourneyCMenu.h
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
#include "JourneyCMenu.h"

// ================= MEMBER FUNCTIONS =======================
CJourneyCMenu::CJourneyCMenu(TJourneySharedData& aSharedData,
		CJourneyVMenu* aView) :
	iSharedData(aSharedData), iTextBoxPage(1), 
	iShowLoadingText(EFalse), iView(aView)
	{
	}

// ---------------------------------------------------------
// CJourneyCMenu::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CJourneyCMenu::ConstructL(const TRect& /*aRect*/)
	{
	CreateWindowL();

	// Load the background image
	LoadArtL();

	if (!iSharedData.iSettings.iAcceptedPrivacy)
		{
		// The text box to display the privacy text
		iDrawOkSymbol = EFalse;
		// The size will be calculated later on - create the text
		// box with an arbritary size.
		iTextBox = CTextBox::NewL(TRect(0, 0, 10, 10), this);
		iTextBox->ShowScrollBar(ETrue);
		iTextBox->SetTextL(R_PRIVACY_TEXT);
		if (!iTextBox->IsScrollDownPossible())
			{
			// We reached the end of the text, enable the ok-button
			iDrawOkSymbol = ETrue;
			}
		}

	SetExtentToWholeScreen(); // Results in a call of SizeChanged()
	ActivateL();

	}

// Destructor
CJourneyCMenu::~CJourneyCMenu()
	{
	DELETE_AND_NULLZ(iTextBox);
	if (iBmpIntroScreenBg) AknIconUtils::DestroyIconData(iBmpIntroScreenBg);
	DELETE_AND_NULLZ(iBmpIntroScreenBg);
	if (iBmpIntroScreenText) AknIconUtils::DestroyIconData(iBmpIntroScreenText);
	DELETE_AND_NULLZ(iBmpIntroScreenText);
	if (iBmpIntroScreenTextM) AknIconUtils::DestroyIconData(iBmpIntroScreenTextM);
	DELETE_AND_NULLZ(iBmpIntroScreenTextM);
	DeleteBackBuffer();
	}

// ---------------------------------------------------------
// CJourneyCMenu::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CJourneyCMenu::SizeChanged()
	{
	const TSize newScreenSize = this->Size();

	// Recreate backbuffer bitmap
	CreateBackBufferL(newScreenSize);

	const CFbsFont* fontNormal = (CFbsFont*) AknLayoutUtils::LayoutFontFromId(
			EAknLogicalFontPrimaryFont);
	// Additional border around text boxes for the darkened area
	iDarkenBorder = fontNormal->CharWidthInPixels('i');

	// Make sure the whole screen is covered by the background image
	NBitmapMethods::setSvgSize(iBmpIntroScreenBg, NBitmapMethods::SetSizeToFullScreen(
			newScreenSize, iBmpIntroScreenBg));
	NBitmapMethods::setSvgSize(iBmpIntroScreenText, NBitmapMethods::ScaleToWidth(
			(newScreenSize.iWidth / 3) * 2, iBmpIntroScreenText));
	NBitmapMethods::setSvgSize(iBmpIntroScreenTextM, iBmpIntroScreenText->SizeInPixels());
	
	// Get location of softkeys
	AknLayoutUtils::TAknCbaLocation cbaLocation = AknLayoutUtils::CbaLocation();
	

	if (cbaLocation == AknLayoutUtils::EAknCbaLocationBottom)
		{
		// Portrait mode / S^3 landscape mode
		AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EControlPane,
				iSk1Rect);
		iSk2Rect = iSk1Rect;
		// Give each softkey only half of the total width
		// Otherwise each of those whold have the whole area.
		// When printing the text with a background color, this would cause
		// the second label to overwrite the first.
		iSk1Rect.SetWidth(iSk1Rect.Width() / 2);
		iSk2Rect.iTl.iX = iSk1Rect.iBr.iX;
		iSk1Align = CGraphicsContext::ELeft;
		iSk2Align = CGraphicsContext::ERight;
		if (newScreenSize.iWidth > newScreenSize.iHeight)
			{
			// Landscape mode on S^3 - fix the softkeys
			// Original iSk1Rect on N8: Top Left: (0, 579), Bottom Right: (640, 640)
			// Current values after initial processing:
			// iSk1Rect Top Left: (0, 579), Bottom Right: (320, 640)
			// iSk2Rect Top Left: (320, 579), Bottom Right: (640, 640)
			// Correct y corrdinates!
			const TInt skHeight = iSk1Rect.Height();
			const TInt skBottomY = newScreenSize.iHeight;
			const TInt skTopY = skBottomY - skHeight;
			iSk1Rect.SetRect(iSk1Rect.iTl.iX, skTopY, iSk1Rect.iBr.iX, skBottomY);
			iSk2Rect.SetRect(iSk2Rect.iTl.iX, skTopY, iSk2Rect.iBr.iX, skBottomY);
			}
		}
	else
		{
		// Landscape mode
		AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EControlPaneTop,
				iSk2Rect);
		AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EControlPaneBottom,
				iSk1Rect);
		// Fix for wrong (?) rectangle returned by 3rd Edition SDK
		iSk1Rect.iTl.iY = iSk1Rect.iBr.iY - iSk2Rect.Height();
		
		// Fix for wrong rectangles returned by 5th Edition SDK (1.0) and 5800xm
		if (iSk1Rect.iTl.iX == 0 && iSk1Rect.iBr.iX == 0)
			{
			const int skHeight = newScreenSize.iHeight / 5;//f->FontMaxHeight() * 2;
			const int skWidth = skHeight * 2;
			// Vertical softkey placement - same for left and right softkeys
			// SK1: Top
			iSk1Rect.iTl.iY = 0;
			iSk1Rect.iBr.iY = iSk1Rect.iTl.iY + skHeight;
			// SK2: Bottom
			iSk2Rect.iBr.iY = newScreenSize.iHeight;
			iSk2Rect.iTl.iY = iSk2Rect.iBr.iY - skHeight;
			// Horizontal sofktey placement - different for right or left softkey positions
			if (cbaLocation == AknLayoutUtils::EAknCbaLocationLeft)
				{
				// Left side of the screen
				iSk1Rect.iTl.iX = 0;
				iSk1Rect.iBr.iX = skWidth;
				}
			else
				{
				// Right side of the screen
				iSk1Rect.iBr.iX = newScreenSize.iWidth;
				iSk1Rect.iTl.iX = iSk1Rect.iBr.iX - skWidth; 
				}
			// Copy horizontal placement from SK1 to SK2 - they're below each
			// other, so it's the same.
			iSk2Rect.iTl.iX = iSk1Rect.iTl.iX;
			iSk2Rect.iBr.iX = iSk1Rect.iBr.iX;
			// On 5th Edition, softkey labels are aligned in the center of the 
			// rectangle
			iSk1Align = iSk2Align = CGraphicsContext::ECenter;
			}
		else
			{
			// Check whether the softkey labels are right or left of the screen
			// This also influences the alignment of the text labels
			if (cbaLocation == AknLayoutUtils::EAknCbaLocationLeft)
				{
				iSk1Align = iSk2Align = CGraphicsContext::ELeft;
				}
			else
				{
				iSk1Align = iSk2Align = CGraphicsContext::ERight;
				}
			}

		// Check if the softkey layout is reversed and the options button
		// is on top (e.g. for the E90 communicator)
		if (IsOptionsButtonOnTop())
			{
			// Yes... swap the rectangles of the softkeys.
			TRect tmpRect = iSk1Rect;
			iSk1Rect = iSk2Rect;
			iSk2Rect = tmpRect;
			}
		}

	if (iTextBox)
		{
		// Calculate textbox size
		TRect textRect;
		textRect.SetRect(TPoint(iDarkenBorder * 2, iSk2Rect.Height()
				+ iDarkenBorder * 2), TPoint(newScreenSize.iWidth
				- iDarkenBorder * 2, newScreenSize.iHeight - iSk2Rect.Height()
				- iDarkenBorder * 2));

		iTextBox->SetDarkenBorder(iDarkenBorder);
		iTextBox->SetExtent(textRect.iTl, textRect.Size());

		if (!iTextBox->IsScrollDownPossible())
			{
			iDrawOkSymbol = ETrue;
			}
		}

	}

TBool CJourneyCMenu::IsOptionsButtonOnTop()
	{
	CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
	if (!cba)
		{
		return EFalse;
		}

	CCoeControl* options = cba->ControlOrNull(EJourneyCmdMenu);
	CCoeControl* exit = cba->ControlOrNull(EJourneyCmdExit);

	if (options && exit)
		{
		if (options->Position().iY < exit->Position().iY)
			{
			return ETrue;
			}
		}
	return EFalse;
	}

void CJourneyCMenu::DeleteBackBuffer()
	{
	if (iBackBufferBmpDrawingDevice)DELETE_AND_NULLZ(iBackBufferBmpDrawingDevice);
	if (iBackBufferBmpGc)DELETE_AND_NULLZ(iBackBufferBmpGc);
	if (iBackBufferBmp)DELETE_AND_NULLZ(iBackBufferBmp);
	}

void CJourneyCMenu::CreateBackBufferL(const TSize aSize)
	{
	DeleteBackBuffer();
	iBackBufferBmp = new (ELeave) CFbsBitmap;
	User::LeaveIfError(iBackBufferBmp->Create(aSize, EColor64K));
	iBackBufferBmpDrawingDevice = CFbsBitmapDevice::NewL(iBackBufferBmp);
	//create graphics context for the bitmap
	User::LeaveIfError(iBackBufferBmpDrawingDevice->CreateContext(
			iBackBufferBmpGc));
	}

// ---------------------------------------------------------
// CJourneyCMenu::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CJourneyCMenu::CountComponentControls() const
	{
	if (iTextBox)
		return 1;
	return 0; // return nbr of controls inside this container
	}

// ---------------------------------------------------------
// CJourneyCMenu::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CJourneyCMenu::ComponentControl(TInt aIndex) const
	{
	if (iTextBox && aIndex == 0)
		return iTextBox;
	return NULL;
	}

// ---------------------------------------------------------
// CJourneyCMenu::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CJourneyCMenu::Draw(const TRect& aRect) const
	{
	CWindowGc& gc = SystemGc();
	
	// Screen size
	const TSize screenSize = iBackBufferBmp->SizeInPixels();

	// Background image
	// Align bottom right
	const TSize bgBmpSize = iBmpIntroScreenBg->SizeInPixels();
	iBackBufferBmpGc->BitBlt(TPoint(screenSize.iWidth - bgBmpSize.iWidth, 
			screenSize.iHeight - bgBmpSize.iHeight), iBmpIntroScreenBg);
	// Logo text
	const TSize txtBmpSize = iBmpIntroScreenText->SizeInPixels();
	TInt topOffset = (screenSize.iHeight - txtBmpSize.iHeight) / 10;
	TInt leftOffset = (screenSize.iWidth - txtBmpSize.iWidth) / 10;
	if (topOffset < 0)
		topOffset = 0;
	if (leftOffset < 0)
		leftOffset = 0;
	iBackBufferBmpGc->SetBrushColor(0);
	iBackBufferBmpGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	

	
	iBackBufferBmpGc->BitBltMasked(
				TPoint(leftOffset, topOffset), 
				iBmpIntroScreenText,
				iBmpIntroScreenText->SizeInPixels(),
				iBmpIntroScreenTextM, EFalse);

//	iBackBufferBmpGc->AlphaBlendBitmaps(
//				TPoint(leftOffset, topOffset), 
//				iBmpIntroScreenText,
//				TRect(TPoint(0,0),iBmpIntroScreenText->SizeInPixels()),
//				iBmpIntroScreenTextM, TPoint(0,0));
	
	/*iBackBufferBmpGc->BitBlt(TPoint(leftOffset, topOffset), 
				iBmpIntroScreenText);*/
	

	if (iShowLoadingText)
		{
		// Draw the loading text
		TRect fullScreenRect(TPoint(0, 0), screenSize);
		// Darken the background
		NBitmapMethods::DarkenArea(iBackBufferBmp, fullScreenRect, 0, 2);

		// Prepare the graphics operations
		const CFbsFont* fontTitle =
				(CFbsFont*) AknLayoutUtils::LayoutFontFromId(
						EAknLogicalFontTitleFont);
		iBackBufferBmpGc->UseFont((CFont*) fontTitle);
		iBackBufferBmpGc->SetPenColor(KTextColor);
		iBackBufferBmpGc->SetPenStyle(CGraphicsContext::ESolidPen);
		iBackBufferBmpGc->SetBrushStyle(CGraphicsContext::ENullBrush);

		// Finally draw the text
		HBufC* txtLoading = CEikonEnv::Static()->AllocReadResourceL(R_LOADING);
		const TInt yOffset = fullScreenRect.Height() / 2;
		iBackBufferBmpGc->DrawText(*txtLoading, fullScreenRect, yOffset,
				CGraphicsContext::ECenter, 0);
		delete txtLoading;

		iBackBufferBmpGc->DiscardFont();
		}
	else
		{
		// Draw the text box
		if (iTextBox)
			iTextBox->Draw(iBackBufferBmp, iBackBufferBmpGc);

		// Set up for softkey labels
		iBackBufferBmpGc->SetPenColor(KTextColor);
		iBackBufferBmpGc->SetBrushColor(KBgColor);
		iBackBufferBmpGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		const CAknLayoutFont* font = AknLayoutUtils::LayoutFontFromId(
				EAknLogicalFontPrimaryFont);
		iBackBufferBmpGc->UseFont((CFont*) font);
		CFbsFont* f = (CFbsFont*) font;

		TBuf<30> textResource;
		// Left softkey
		if (iTextBox)
			CEikonEnv::Static()->ReadResource(textResource, R_MENU_ACCEPT);
		else
			CEikonEnv::Static()->ReadResource(textResource, R_MENU_OPTIONS);

		// Don't draw the softkey text when the privacy statement is being displayed
		// and the user is not yet allowed to continue (has to scroll through the text first).
		if (iTextBox && !iDrawOkSymbol)
			{
			// Only draw the empty rectangle
			iBackBufferBmpGc->SetPenColor(KBgColor);
			iBackBufferBmpGc->DrawRect(iSk1Rect);
			iBackBufferBmpGc->SetPenColor(KTextColor);
			}
		else
			{
			// Draw the softkey label
			iBackBufferBmpGc->DrawText(textResource, iSk1Rect,
					iSk1Rect.Height() / 2 + f->FontMaxAscent() / 2, iSk1Align,
					3);
			}

		// Right softkey
		CEikonEnv::Static()->ReadResource(textResource, R_MENU_EXIT);
		iBackBufferBmpGc->DrawText(textResource, iSk2Rect, iSk2Rect.Height()
				/ 2 + f->FontMaxAscent() / 2, iSk2Align, 3);

		iBackBufferBmpGc->DiscardFont();
		}

	// Blit the back buffer to the screen
	gc.BitBlt(TPoint(aRect.iTl), iBackBufferBmp, aRect);
	}

// ---------------------------------------------------------
void CJourneyCMenu::LoadArtL()
	{
	TFileName fullName;
	NJourneyFunctions::AppendFullPath(KJourneyMif, fullName);

	iBmpIntroScreenBg = AknIconUtils::CreateIconL(fullName,
			EMbmJourneyJourneytitlebg);

	// To prevent re-loading of the bitmap from the storage medium
	AknIconUtils::PreserveIconData(iBmpIntroScreenBg);
	
	AknIconUtils::CreateIconL(iBmpIntroScreenText, iBmpIntroScreenTextM, fullName,
			EMbmJourneyJourneytitletext, EMbmJourneyJourneytitletext_mask);

	// To prevent re-loading of the bitmap from the storage medium
	AknIconUtils::PreserveIconData(iBmpIntroScreenText);
	AknIconUtils::PreserveIconData(iBmpIntroScreenTextM);
	// SetSize will be called through SizeChanged, we don't have to do that now!
	}

// ---------------------------------------------------------
void CJourneyCMenu::ShowLoadingText(const TBool aShowText)
	{
	iShowLoadingText = aShowText;
	// Redraw the screen
	Window().Invalidate();
	}

// ---------------------------------------------------------
void CJourneyCMenu::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{
	CCoeControl::HandlePointerEventL(aPointerEvent);
	if (aPointerEvent.iType == TPointerEvent::EButton1Up)
		{
		if (iSk1Rect.Contains(aPointerEvent.iPosition))
			{
			// Softkey 1 was selected -> close text box or open menu
			if (iTextBox && iDrawOkSymbol)
				{
				// Close the textbox
				CloseTextBox();
				Window().Invalidate();
				}
			else if (!iTextBox)
				{
				// Show the menu
				iView->ProcessCommandL(EAknSoftkeyOptions);
				}
			}
		else if (iSk2Rect.Contains(aPointerEvent.iPosition))
			{
			// Softkey 2 was selected -> Exit
			iView->ProcessCommandL(EEikCmdExit);
			}
		else if (iTextBox)
			{
			iTextBox->HandlePointerEventL(aPointerEvent);
			if (!iTextBox->IsScrollDownPossible())
				{
				// We reached the end of the text, enable the ok-button
				iDrawOkSymbol = ETrue;
				}
			Window().Invalidate();
			}

		}
	}

//------------------------------------------------------------------------------
TKeyResponse CJourneyCMenu::OfferKeyEventL(const TKeyEvent& aKeyEvent,
		TEventCode aType)
	{
	TKeyResponse returnVal = EKeyWasNotConsumed;
	if (aType == EEventKeyDown)
		{
		returnVal = EKeyWasConsumed;
		switch (aKeyEvent.iScanCode)
			{
			case EStdKeyDownArrow:
				{
				// Scroll down if the privacy text box is visible
				if (iTextBox)
					{
					iTextBox->ScrollDown();
					if (!iTextBox->IsScrollDownPossible())
						{
						// We reached the end of the text, enable the ok-button
						iDrawOkSymbol = ETrue;
						}
					Window().Invalidate();
					}
				break;
				}
			case EStdKeyUpArrow:
				{
				// Scroll up if the privacy text box is visible
				if (iTextBox)
					{
					iTextBox->ScrollUp();
					Window().Invalidate();
					}
				break;
				}
			case EStdKeyDevice0: // Left softkey
			case EStdKeyDevice3: // OK (Softkey center)
				{
				if (iTextBox && iDrawOkSymbol)
					{
					CloseTextBox();
					}
				break;
				}

			default:
				returnVal = EKeyWasNotConsumed;
				break;
			}
		}
	return (returnVal);
	}

void CJourneyCMenu::CloseTextBox()
	{
	// If user is allowed to continue, handle the joystick press
	iDrawOkSymbol = EFalse;
	// Check which page of the info text was currently active
	if (iTextBoxPage == 1)
		{
		// Switch to the second info-page
		iTextBoxPage = 2;
		iTextBox->SetTextL(R_INFO_TEXT);
		if (!iTextBox->IsScrollDownPossible())
			{
			// We reached the end of the text, enable the ok-button
			iDrawOkSymbol = ETrue;
			}
		}
		else
		{
		// The user has accepted the privacy note
		iSharedData.iSettings.iAcceptedPrivacy = ETrue;
		// We do no longer need the text box
		delete iTextBox;
		iTextBox = NULL;
		// Save that the user has accepted the privacy note
		TUint settingsSize = sizeof(iSharedData.iSettings);
		iSharedData.iCommInterface->WriteIniFile(uidSettings,
				&iSharedData.iSettings, settingsSize);
		}
	// Redraw the screen 
	Window().Invalidate();
	}

// ---------------------------------------------------------
// CJourneyCMenu::HandleControlEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//
void CJourneyCMenu::HandleControlEventL(CCoeControl* /*aControl*/, TCoeEvent /*aEventType*/)
	{
	}

void CJourneyCMenu::HandleResourceChange(TInt aType)
	{
	CCoeControl::HandleResourceChange(aType);
	if (aType == KEikDynamicLayoutVariantSwitch)
		{
		// User switched the layout configuration or the screen resolution :)
		// -> we have to recreate the layout
		SetExtentToWholeScreen(); // Results in a call of SizeChanged()
		}
	}

// End of File
