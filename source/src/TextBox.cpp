/*
* ============================================================================
*  Name     : CTextBox from TextBox.cpp
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
#include "TextBox.h"

/**
 * Standard Symbian OS static function for creating an instance of the object.
 * If creation fails, a leave will be sent.
 * @param aContainer reference to the container containing the label control.
 */
CTextBox* CTextBox::NewL(const TRect& aRect, const CCoeControl* aParent)
{
	CTextBox* self = new (ELeave) CTextBox();
	CleanupStack::PushL(self);
	self->ConstructL(aRect, aParent);
	CleanupStack::Pop(self);
	return self;
}

// Constructor
//------------------------------------------------------------------------------
CTextBox::CTextBox()
:
	iCurPage(0),
	iDisplayNewPage(ETrue),
	iShowScrollBar(EFalse),
    iNumLines(0),
	iCurStartAtTextPos(0),
	iCurEndAtTextPos(0),
	iTextLength(0),
	iScrollType(EPage1),
	iDimText(EFalse)
{
}

//------------------------------------------------------------------------------
void CTextBox::ConstructL(const TRect& aRect, const CCoeControl* aParent)
{
	if (aParent)
	{
		// Control has a parent ...
		// ... reuse parents window
		SetContainerWindowL(*aParent);
	}
	else
	{
		// Control should be window-owning ...
		// ... create an own window
		CreateWindowL();
	}
	// Assign size to this control
	SetRect(aRect);

	UpdateNumLines();
	
	// Control-initialisation finished, ready to draw
	ActivateL();
}

// Destructor
//------------------------------------------------------------------------------
CTextBox::~CTextBox()
{
	iPageStartAtCharPos.Close();
	iCurLinesStart.Close();
	iCurLinesLength.Close();
	if (iTextCache)
		delete iTextCache;
}

//------------------------------------------------------------------------------
void CTextBox::SetDarkenBorder(const TInt aBorder)
{
	iBorder = aBorder;
}

//------------------------------------------------------------------------------
void CTextBox::SizeChanged()
{
	iOrigSize = this->Rect();
	iTopLeft = iOrigSize.iTl;
	iSize = iOrigSize.Size();
	
	UpdateNumLines();
	
	// Scroll bar arrows
	if (iShowScrollBar)
	{
		iSize.iWidth = iOrigSize.Width() - iArrowWidth * 2;
		const TInt arrowLeftX = iOrigSize.iBr.iX - iArrowWidth;
		
		// Up-Arrow
		iRectArrowUp.SetRect(arrowLeftX, iTopLeft.iY, iOrigSize.iBr.iX, iTopLeft.iY + iArrowWidth);
		
		// Down-Arrow
		iRectArrowDown.SetRect(arrowLeftX, iOrigSize.iBr.iY - iArrowWidth, iOrigSize.iBr.iX, iOrigSize.iBr.iY);		
	}
	
	
	Reset(EFalse);
	CalculateNewPage();
}

//------------------------------------------------------------------------------
void CTextBox::UpdateNumLines()
{
#ifdef __UIQ__
    iFontUsed = iEikonEnv->Font(TLogicalFont(TLogicalFont::EView, TLogicalFont::ENormal, TZoomFactor()));
#else
	iFontUsed = (CFont*) AknLayoutUtils::LayoutFontFromId(EAknLogicalFontPrimarySmallFont);//EAknLogicalFontPrimaryFont);
#endif
    iFontHeight = iFontUsed->AscentInPixels();
    // The standard font line gap isn't enough for really text
    // that should be very readable (compare this to a linespacing
    // in an office application of 1 and 1.3 - the latter one
    // will be a lot better than the default.
	const TInt lineSpacing = iFontUsed->FontLineGap();
	iTextYOffset = lineSpacing + lineSpacing / 4; // + iFontHeight
	
	iArrowWidth = iFontUsed->CharWidthInPixels('o');
	
    // Calculate the number of lines
    // Correct for first line only having the height of the font, all others
    // need more space.
    iNumLines = (iSize.iHeight - iFontHeight) / iTextYOffset + 1;

    if (iNumLines < 1)
        iNumLines = 1;
}

//------------------------------------------------------------------------------
void CTextBox::SetTextL(TInt aTextResId)
{
	Reset(ETrue);
    iTextCache = CEikonEnv::Static()->AllocReadResourceL(aTextResId);
   	//iTextCache = iSharedData.iResourceInterface->AllocReadAnyResourceL(aTextResId);
	iTextLength = iTextCache->Length();
	CalculateNewPage();
	
}

//------------------------------------------------------------------------------
void CTextBox::SetTextL(const TDesC& aText)
{
	Reset(ETrue);
	iTextCache = aText.AllocL();
	iTextLength = iTextCache->Length();
	CalculateNewPage();
}

//------------------------------------------------------------------------------
void CTextBox::ShowScrollBar(TBool aShowBar)
{
	iShowScrollBar = aShowBar;
}

//------------------------------------------------------------------------------
void CTextBox::DimText(TBool aEnableDim)
{
	iDimText = aEnableDim;
}

//------------------------------------------------------------------------------
void CTextBox::Reset(TBool aCleanText)
{
	iPageStartAtCharPos.Close();
	iCurStartAtTextPos = 0;
	iCurEndAtTextPos = 0;
	iCurPage = 0;
	if (aCleanText)
	{
		DimText(EFalse);
		if (iTextCache)
		{
			delete iTextCache;
			iTextCache = NULL;
		}
		iTextLength = 0;
	}
	iDisplayNewPage = ETrue;
}

//------------------------------------------------------------------------------
void CTextBox::SetScrollType(eScrollTypes aScrollType)
{
	iScrollType = aScrollType;
}

void CTextBox::Draw(const TRect &aRect) const
	{
	// We don’t want to do partial redraws – area specified by aRect is ignored
	// Get Graphics Context to draw contents
	CWindowGc& gc = SystemGc();
	DrawBackground(Rect());
	// TODO: Implement
	}

//------------------------------------------------------------------------------
void CTextBox::Draw(CFbsBitmap* aBmp, CFbsBitGc* aBmpGc)
{	
	// Darken the area behind the text box (if activated).
	if (iBorder >= 0)
	{
		NBitmapMethods::DarkenArea(aBmp, iOrigSize, iBorder, 2);
	}
	
	// If this text box doesn't contain text, don't go any further.
	if (iTextLength == 0)
		return;
	
	// Make sure that the current page has already been calculated.
	CalculateNewPage();
		
	if (iShowScrollBar)
	{
		// Scroll Arrows
		aBmpGc->SetPenColor(KTextColor);
		aBmpGc->SetPenStyle(CGraphicsContext::ESolidPen);
		// Up-Arrow
	    if (IsScrollUpPossible())
			NBitmapMethods::DrawArrow(aBmpGc, iRectArrowUp, NBitmapMethods::EUp);
		// Down-Arrow
	    if (IsScrollDownPossible())
			NBitmapMethods::DrawArrow(aBmpGc, iRectArrowDown, NBitmapMethods::EDown);
	}
	
	// Draw all visible text lines using the cached values.
    aBmpGc->UseFont(iFontUsed);
    if (iDimText)
    {
    	aBmpGc->SetPenColor(KTextColorDark);
    }
    else
    {
    	aBmpGc->SetPenColor(KTextColor);
    }
	TPtrC tempText;
	TInt curLine;
		
	for (curLine = 0; curLine < iCurLinesStart.Count(); curLine++)
	{
		tempText.Set(iTextCache->Mid(iCurLinesStart[curLine], iCurLinesLength[curLine]));
		TPoint curTextPos(iTopLeft.iX, iTopLeft.iY + curLine * iTextYOffset + iFontHeight);
		aBmpGc->DrawText(tempText, curTextPos);
	}
    aBmpGc->DiscardFont();
}

//------------------------------------------------------------------------------
void CTextBox::CalculateNewPage()
{
	if (iSize.iHeight == 0 || iTextLength == 0)
		return;
	if (iCurPage == JUST_SCROLLED_DOWN)
	{
		// Just before, a scrolldown event has been received.
		// -> Store the current text start position so that we
		// don't have to search again when scrolling up.
		iPageStartAtCharPos.Append(iCurStartAtTextPos);
		iCurPage = iPageStartAtCharPos.Count();
		iCurStartAtTextPos = iCurEndAtTextPos;
	}
	
	if (iDisplayNewPage)
	{
		TInt curLine;
		TPtrC tempText;
		TInt i;
		TInt copyStart = iCurStartAtTextPos;
		// We'll display a new page - do line breaking stuff.
		// Reset the cache-arrays
		iCurLinesStart.Close();
		iCurLinesLength.Close();

		TPtrC analyzeSeg;
		RArray<TInt> possibleDividePos;
		RArray<TInt> removeChars;
		RArray<TBool> forceLinebreak;

		iCurEndAtTextPos = -1;

		for (curLine = 0; curLine < iNumLines; curLine++)
		{
			TInt copyLength = MAX_CHARS_PER_LINE;
			if ((copyStart + copyLength) > iTextLength)
				copyLength = iTextLength - copyStart;

			tempText.Set(iTextCache->Mid(copyStart, copyLength));

			// Get the places of all spaces and "-" in the text where it could possibly be divided
			possibleDividePos.Close();
			removeChars.Close();
			forceLinebreak.Close();

			TBool removeReturn = EFalse;
			for (i=0; i<copyLength; i++)
			{
				analyzeSeg.Set(tempText.Mid(i,1));
				if (analyzeSeg.Compare(KCharReturn) == 0)
				{
					removeReturn = ETrue;
				}
				else if (analyzeSeg.Compare(KCharNewline) == 0)
				{
					if (removeReturn)
					{
						possibleDividePos.Append(i-1);
						removeChars.Append(2);
						forceLinebreak.Append(ETrue);
					}
					else
					{
						possibleDividePos.Append(i);
						removeChars.Append(1);
						forceLinebreak.Append(ETrue);
					}
					break;
				}
				else if (analyzeSeg.Compare(KCharSpace) == 0)
				{
					possibleDividePos.Append(i);
					removeChars.Append(1);
					forceLinebreak.Append(EFalse);
				}
				else if (analyzeSeg.Compare(KCharHyphen) == 0)
				{
					possibleDividePos.Append(i+1);
					removeChars.Append(0);
					forceLinebreak.Append(EFalse);
				}
			}
			if (copyLength < MAX_CHARS_PER_LINE)
			{
				// We're at the end of the text. Also add the end of the text.
				possibleDividePos.Append(copyLength);
				removeChars.Append(0);
				forceLinebreak.Append(ETrue);
			}
			
			TInt prevLineLength = 0;
			TInt curLineLength = 0;
			TInt divideAtCharAtArrayPos = -1;
			for (i=0; i<possibleDividePos.Count(); i++)
			{
				analyzeSeg.Set(tempText.Mid(0, possibleDividePos[i]));
				curLineLength = iFontUsed->TextWidthInPixels(analyzeSeg);
				if (prevLineLength < iSize.iWidth && curLineLength >= iSize.iWidth)
				{
					divideAtCharAtArrayPos = i - 1;
					break;
				}
				if (forceLinebreak[i])
				{
					divideAtCharAtArrayPos = i;
					break;
				}
				prevLineLength = curLineLength;
			}

			if (divideAtCharAtArrayPos == -1 && copyLength < MAX_CHARS_PER_LINE)
			{
				// The text is too short for this line and the rest of the
				// text fits into the line, it doesn't need to be divided.
				possibleDividePos.Append(copyLength);
				removeChars.Append(0);
				divideAtCharAtArrayPos = possibleDividePos.Count() - 1;
			}

			if (divideAtCharAtArrayPos == -1)
				divideAtCharAtArrayPos = possibleDividePos.Count() - 1;

			if (divideAtCharAtArrayPos > -1)
			{
				iCurLinesStart.Append(copyStart);
				iCurLinesLength.Append(possibleDividePos[divideAtCharAtArrayPos]);
				//analyzeSeg.Set(tempText.Mid(0, dividePos[divideAtCharAtArrayPos]));
				copyStart += possibleDividePos[divideAtCharAtArrayPos];
				copyStart += removeChars[divideAtCharAtArrayPos];
				//TPoint curTextPos(iTopLeft.iX, iTopLeft.iY + curLine * iTextYOffset + iFontHeight);
				//aBmpGc->DrawText(analyzeSeg, curTextPos);
			}

			if (copyStart >= iTextLength)
			{
				copyStart = iTextLength;
				break;
			}

			if (iScrollType == ELine && curLine == 0)
				iCurEndAtTextPos = copyStart;
			else if (iScrollType == EPage1 && curLine == (iNumLines - 2))
				iCurEndAtTextPos = copyStart;
		}

		if (iCurEndAtTextPos == -1)
			iCurEndAtTextPos = copyStart;

		iDisplayEndAtTextPos = copyStart;

		possibleDividePos.Close();
		removeChars.Close();
		forceLinebreak.Close();

		// Next time, use the cached linebreak positions
		iDisplayNewPage = EFalse;
	}
}


//------------------------------------------------------------------------------
void CTextBox::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
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
		return;
		}

	TPoint pos = aPointerEvent.iPosition;
    // Don't check left side as we also want to scroll if user hits the scroll arrows
    // outside of the textbox
    if (pos.iY >= iTopLeft.iY && 
    	pos.iY <= iTopLeft.iY + iSize.iHeight &&
    	pos.iX >= iTopLeft.iX)
    {
        // Check if scrolling up or down
        if (pos.iY < iTopLeft.iY + iSize.iHeight / 2)
        {
            ScrollUp();
        }
        else
        {
            ScrollDown();
        }
    }

    // Call base class HandlePointerEventL()
    CCoeControl::HandlePointerEventL(aPointerEvent);
}


//------------------------------------------------------------------------------
TBool CTextBox::IsScrollDownPossible()
{
	return iDisplayEndAtTextPos < iTextLength;
}

//------------------------------------------------------------------------------
TBool CTextBox::IsScrollUpPossible()
{
	return iCurPage > 0;
}

//------------------------------------------------------------------------------
void CTextBox::ScrollDown()
{
	if (IsScrollDownPossible())
	{
		// If we've not been at the new page yet, save the previous page
		// in the next Draw-event
		if (iPageStartAtCharPos.Count() < iCurPage + 1)
		{
			iCurPage = JUST_SCROLLED_DOWN;
		}
		else
		{
			iCurStartAtTextPos = iCurEndAtTextPos;
			iCurEndAtTextPos = iTextLength;		// Just to make sure, will be corrected with the next draw
			iCurPage ++;
		}
		iDisplayNewPage = ETrue;
		CalculateNewPage();
	}
}

//------------------------------------------------------------------------------
void CTextBox::ScrollUp()
{
	if (IsScrollUpPossible())
	{
		iCurPage --;
		iCurStartAtTextPos = iPageStartAtCharPos[iCurPage];
		iCurEndAtTextPos = iTextLength;		// Just to make sure, will be corrected with the next draw
		iDisplayNewPage = ETrue;
		CalculateNewPage();
	}
}


//------------------------------------------------------------------------------
void CTextBox::ScrollToStart()
{
	if (iCurPage != 0)
	{
		iCurPage = 0;
		iCurStartAtTextPos = iPageStartAtCharPos[iCurPage];
		iCurEndAtTextPos = iTextLength;		// Just to make sure, will be corrected with the next draw
		iDisplayNewPage = ETrue;
		CalculateNewPage();
	}
}
