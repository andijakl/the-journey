/*
* ============================================================================
*  Name     : CTextBox from TextBox.h
*  Part of  : Journey
*  Created  : 04.05.2004 by Andreas Jakl
*  Description:
*     Is able to display multiline-text
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

#ifndef __JOURNEY2TEXTBOX_H__
#define __JOURNEY2TEXTBOX_H__

#include <eikenv.h>
#include <coecntrl.h>

#ifdef __UIQ__
#include <gulfont.h>
#else
#include <aknutils.h> 
#endif

#include "SharedSettings.h"
#include "bitmapmethods.h"

/** 
 * Maximum number of characters per line. A linebreak will be added
 * at the end in any case.
 */
#define MAX_CHARS_PER_LINE		150

/**
 * Special value used for the currently displayed page to indicate
 * that the user has just scrolled down and got to a new page where
 * the linefeeds have not been calculated yet.
 */
#define JUST_SCROLLED_DOWN		-1

// Various characters that are positions where a line break can be included
_LIT(KCharSpace, " ");
_LIT(KCharReturn, "\r");
_LIT(KCharNewline, "\n");
_LIT(KCharHyphen, "-");

/**
 * Defines the available scroll speeds.
 * ELine: Scroll line by line
 * EPage1: Scroll by a whole page minus one line. This means that the 
 * last line of the old page is the first line of the new page.
 * EPage: Scroll a full page
 */
typedef enum {
	ELine,
	EPage1,
	EPage
} eScrollTypes;

/**
 * Custom text box control.
 * 
 * The text box class will automatically add line breaks at optimal positions,
 * honoring good line break positions like a space and keeping custom linebreaks
 * that are included in the text using \n.
 * 
 * For performance reasons, this class does not precalculate the line break positions
 * for the whole text at once, but only for the pages that have already been
 * displayed. When the user scrolls down, the next page is calculated.
 * All calculated pages are cached.
 * 
 * The text box can draw an optional scroll bar = scroll arrows that indicate whether
 * scrolling up and/or down is possible. Additionally, it's possible to let the text box
 * automatically darken the area behind it.
 */
class CTextBox : public CCoeControl
{
public:
	static CTextBox* NewL(const TRect& aRect, const CCoeControl* aParent);

	/// Destructor
	~CTextBox();

private:
	/// Constructors
	CTextBox();

	/// Initialize contents of the class
	void ConstructL(const TRect& aRect, const CCoeControl* aParent);

public:     // Own public functions
	/**
	* Sets the text of this text box to the specified text resource ID.
	* @param aTextResId the resource id to read.
	*/
	void SetTextL(TInt aTextResId);

	/**
	* Creates a copy of the specified descriptor and sets it as text for this box.
	* @param aText descriptor to copy for display in the text box.
	*/
	void SetTextL(const TDesC& aText);
	
	/**
	 * Enable or disable dimmed text.
	 * @param aEnableDim if set to ETrue, the text will be printed in a dark color.
	 * If set to false (default), the bright color is used. 
	 */
	void DimText(TBool aEnableDim);

	/**
	 * Define the scroll behavior of the text box.
	 * ELine: Scroll line by line
	 * EPage1: Scroll by a whole page minus one line. This means that the 
	 * last line of the old page is the first line of the new page.
	 * EPage: Scroll a full page
	 * @param aScrollType the scrolling behavior. See the description of this
	 * function.
	 */
	void SetScrollType(eScrollTypes aScrollType);
	
	/**
	 * Define how big the border around the text box should be that
	 * is to be darkened as well. This is necessary as it'd look
	 * quite strange if the text would reach to the very same pixel
	 * as the border of the textbox.
	 * Set to -1 to disable darkening the background area.
	 * @param aBorder size of the border to be darkened (in pixels).
	 * Set to -1 to disable darkening of the background.
	 */
	void SetDarkenBorder(const TInt aBorder);
	
	/**
	 * Define whether the scroll bar should be shown.
	 * If set to true, the area for the text will be smaller, as the scroll
	 * arrows on the right side will take some space.
	 * @param aShowBar ETrue to show the scroll bar, EFalse to hide it.
	 */
	void ShowScrollBar(TBool aShowBar);

	/**
	 * Draw function from the CCoeControl base class.
	 */
	void Draw(const TRect &aRect) const;
	
	/**
	 * Draw the text box to the specified bitmap.
	 * The text box will draw itself to the rectangle the control has saved.
	 * It might darken an area bigger than this rectangle, if you activate
	 * the darkening border and set it to someting > 0.
	 * @param aBmp bitmap to draw to.
	 * @param aBmpGc bitmap context of the target bitmap.
	 */
	void Draw(CFbsBitmap* aBmp, CFbsBitGc* aBmpGc);

	/**
	 * Handle a pointer event from the stylus at the specified
	 * position. Tapping in the lower half scrolls down, tapping the box
	 * in the upper half scrolls up.
	 * @param aPosition position of the pointer event to determine whether
	 * the control should scroll down or up.
	 * @return ETrue if the pointer event has been handled.
	 * EFalse if it has not, e.g. if the pointer event was outside
	 * of the text box.
	 */
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);

	/**
	 * Query whether scrolling down is possible or whether the last
	 * page is already being displayed.
	 * @return ETrue if scrolling down is still possible, EFalse
	 * if the user is already looking at the last page.
	 */
	TBool IsScrollDownPossible();
	
	/**
	 * Query whether scrolling up is possible or whether the first
	 * page is already being displayed.
	 * @return ETrue if scrolling up is still possible, EFalse
	 * if the user is already looking at the first page.
	 */
	TBool IsScrollUpPossible();

	/**
	 * Scroll one page down. If it is not possible and the last page
	 * is already being displayed, this request will be ignored.
	 */
	void ScrollDown();
	
	/**
	 * Scroll one page up. If it is not possible and the first page
	 * is already being displayed, this request will be ignored.
	 */
	void ScrollUp();
	
	/**
	 * Scroll to the very first page.
	 */
	void ScrollToStart();

private:
	/** 
	 * Called by the owning container if the size of the control has changed,
	 * e.g. because of a screen layout change. */
	void SizeChanged();
	
	/**
	 * Recalculate how many text lines fit on one page, based on the
	 * height of the used font and the current size of the control.
	 */
	void UpdateNumLines();
	
	/**
	 * Reset the text box.
	 * Clears cached line break positions and other temponary data.
	 * @param aCleanText ETrue: also delete the text. Use this if
	 * the text should be replaced with a different text.
	 * EFalse: Just delete the cached values. Use this in case of a 
	 * layout change, when the text stays the same but the line break
	 * positions have to be recalculated.
	 */
	void Reset(TBool aCleanText);
	
	/**
	 * Calculate and cache the line break positions for a new page
	 */
	void CalculateNewPage();

public:     // Public data

private:    // Private Data
	/**
	 * Cache for the positions in the original text where the
	 * individual pages start. If the user scrolls down,
	 * the position will be stored in this array so that it
	 * can be easily found again when he scrolls up again.
	 */
	RArray<TInt> iPageStartAtCharPos;
	
	/**
	 * Cached line break positions where the lines of the 
	 * currently displayed page start.
	 * Note: Would be more efficient if we used a TPtr-array.
	 */
	RArray<TInt> iCurLinesStart;
	
	/**
	 * Length of the currently displayed lines.
	 */
	RArray<TInt> iCurLinesLength;
	
	/**
	 * Number of the currently displayed page.
	 * Can be set to JUST_SCROLLED_DOWN if the user just
	 * scrolled down and the new page has to be initialized.
	 */
	TInt iCurPage;
	
	/**
	 * ETrue if a new page should be displayed or something else
	 * in the displayed content has changed. The CalculateNewPage()
	 * method will only calculate a new page if this is set to true.
	 */
	TBool iDisplayNewPage;

	/**
	 * Cache for the font that is used to display the text.
	 */
	const CFont* iFontUsed;

	/**
	 * The font height (ascent) in pixels.
	 */
	TInt iFontHeight;
	
	/**
	 * The y-offset between two lines. Includes the size of a line,
	 * not just the space inbetween two lines.
	 */
	TInt iTextYOffset;

	/**
	 * Heap-based buffer to store a copy of the text that should be displayed
	 * by this text box.
	 */
	HBufC* iTextCache;

	/**
	 * Size of the whole text box. Therefore, this is the original, total size
	 * of the control.
	 */
	TRect iOrigSize;
	
	/**
	 * Top left point of the control.
	 */
	TPoint iTopLeft;
	
	/**
	 * Size of the text content of this text box. If scrollbars are to be displayed,
	 * this size is smaller than the size of the text box control.
	 */
	TSize iSize;
	
	/**
	 * True if the scroll bar should be visible.
	 */
	TBool iShowScrollBar;
	
	/**
	 * Size (width) in pixels of the scroll arrow.
	 * This is dynamically calculated based on the size of the character 'o' of the
	 * currently used font.
	 */
	TInt iArrowWidth;
	
	/**
	 * Rectangle for the arrow that should indicate that scrolling up is possible.
	 */
	TRect iRectArrowUp;
	
	/**
	 * Rectangle for the arrow that should indicate that scrolling down is possible.
	 */
	TRect iRectArrowDown;
	
	/**
	 * How many lines are visible at the same time (max.), given the platform font and
	 * the size of the textbox control.
	 */
	TInt iNumLines;

	/**
	 * Position in the text where the currently displayed page starts.
	 */
	TInt iCurStartAtTextPos;
	
	/**
	 * Position in the text where the currently displayed page ends.
	 */
	TInt iCurEndAtTextPos;
	TInt iDisplayEndAtTextPos;
	
	/**
	 * Length (in characters) of the whole text that that this text box should display.
	 */
	TInt iTextLength;

	/**
	 * Scroll behavior of this text box.
	 */
	eScrollTypes iScrollType;
	
	/**
	 * Border of darkened area. This is a border *around* the control,
	 * so if the border is set to something > 0, the control will draw to a bigger
	 * area than its actual size. Keep that in mind for your applications.
	 * If set to -1, the text box will not darken the area behind it.
	 */
	TInt iBorder;

	TBool iDimText;
};

#endif
