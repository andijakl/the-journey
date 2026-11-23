// Copyright (c) 2000, Nokia. All rights reserved.

#include "bitmapmethods.h"
#include <eikenv.h>
#include <fbs.h>


CFbsBitGc* NBitmapMethods::CreateGraphicsContextLC(CFbsBitmapDevice& aBitmapDevice)
{
	CFbsBitGc* graphicsContext = NULL;
	User::LeaveIfError(aBitmapDevice.CreateContext(graphicsContext));
	CleanupStack::PushL(graphicsContext);
	return graphicsContext;
}


CFbsBitGc* NBitmapMethods::CreateGraphicsContextL(CFbsBitmapDevice& aBitmapDevice)
{
	CFbsBitGc* gc = CreateGraphicsContextLC(aBitmapDevice);
	CleanupStack::Pop(gc);
	return gc;
}


CFbsBitmapDevice* NBitmapMethods::CreateBitmapDeviceLC(CFbsBitmap& aBitmap)
{
	CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(&aBitmap);
	CleanupStack::PushL(bitmapDevice);
	return bitmapDevice;
}


CFbsBitmapDevice* NBitmapMethods::CreateBitmapDeviceL(CFbsBitmap& aBitmap)
{
	CFbsBitmapDevice* device = CreateBitmapDeviceLC(aBitmap);
	CleanupStack::Pop(device);
	return device;
}


CFbsBitmap* NBitmapMethods::CreateBitmapLC(TSize aSizeInPixels,TDisplayMode aDispMode)
{
	CFbsBitmap* bitmap = new(ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Create(aSizeInPixels,aDispMode));
	ASSERT((bitmap->DisplayMode() == KColourDepth) || (bitmap->DisplayMode()  == EGray2));
	return bitmap;
}


CFbsBitmap* NBitmapMethods::CreateBitmapL(TSize aSizeInPixels,TDisplayMode aDispMode)
{
	CFbsBitmap* bitmap = CreateBitmapLC(aSizeInPixels,aDispMode);
	CleanupStack::Pop(bitmap);
	return bitmap;
}


CFbsBitmap* NBitmapMethods::CreateBitmapLC(const TDesC& aFileName,TInt aId)
{
	CFbsBitmap* bitmap = new(ELeave) CFbsBitmap();

	CleanupStack::PushL(bitmap);
	TInt loadException = bitmap->Load(aFileName,aId);
	User::LeaveIfError(loadException);

	if ((bitmap->DisplayMode() == KColourDepth) || (bitmap->DisplayMode()  == EGray2))
	{
		return bitmap;
	}
	else
	{
		CFbsBitmap* newBitmap = CreateBitmapLC(	bitmap->SizeInPixels(),KColourDepth);

		CFbsBitmapDevice*		bitmapDevice	= CreateBitmapDeviceLC(*newBitmap);
		CFbsBitGc*				bitmapGc		= CreateGraphicsContextLC(*bitmapDevice);
		bitmapGc->BitBlt(TPoint(0,0),bitmap,TRect(bitmap->SizeInPixels()));
		CleanupStack::PopAndDestroy(2); // gc and device
		// The next three lines are here to get rid of the old bitmap but keep the new one
		CleanupStack::Pop(newBitmap);
		CleanupStack::PopAndDestroy(bitmap);
		CleanupStack::PushL(newBitmap);

		return newBitmap;
	}
}


CFbsBitmap* NBitmapMethods::CreateBitmapL(const TDesC& aFileName,TInt aId)
{
	CFbsBitmap* bitmap = CreateBitmapLC(aFileName, aId);
	CleanupStack::Pop(bitmap);
	return bitmap;
}

void NBitmapMethods::BitBltMaskedEntireBitmap(CFbsBitGc& aTargetGc,TPoint aTopLeft,
        const CFbsBitmap& aBitmap,const CFbsBitmap& aBitMask)
{
	PartialReset(aTargetGc);
	aTargetGc.BitBltMasked(aTopLeft,&aBitmap,aBitmap.SizeInPixels(),&aBitMask,ETrue);
	PartialReset(aTargetGc);
}


void NBitmapMethods::PartialReset(CFbsBitGc& aGc)
{
	aGc.SetPenSize(TSize(1,1));
	aGc.SetPenColor(KRgbBlack);
	aGc.SetPenStyle(CFbsBitGc::ESolidPen);
	aGc.SetDrawMode(CFbsBitGc::EDrawModePEN);
	aGc.DiscardFont();
	aGc.DiscardBrushPattern();
	aGc.SetBrushColor(KRgbWhite);
	aGc.SetBrushStyle(CFbsBitGc::ENullBrush);
	aGc.SetCharJustification(0,0);
	aGc.SetWordJustification(0,0);
	aGc.SetDitherOrigin(TPoint(0,0));
	aGc.SetPenStyle(CFbsBitGc::ENullPen);
	aGc.SetShadowMode(EFalse);
	aGc.SetStrikethroughStyle(EStrikethroughOff);
	aGc.SetUnderlineStyle(EUnderlineOff);
	aGc.SetUserDisplayMode(ENone);
}


void NBitmapMethods::DrawArrow(CFbsBitGc* aBmpGc, TRect aRect, TArrowDirection aDirection)
{
	switch (aDirection)
	{
		case EDown:
			aBmpGc->DrawLine(aRect.iTl, TPoint(aRect.iBr.iX, aRect.iTl.iY));
			aBmpGc->DrawLineTo(TPoint((aRect.iTl.iX + aRect.iBr.iX) / 2, aRect.iBr.iY));
			aBmpGc->DrawLineTo(aRect.iTl);
			break;
		case EUp:
			aBmpGc->DrawLine(aRect.iBr, TPoint(aRect.iTl.iX, aRect.iBr.iY));
			aBmpGc->DrawLineTo(TPoint((aRect.iTl.iX + aRect.iBr.iX) / 2, aRect.iTl.iY));
			aBmpGc->DrawLineTo(aRect.iBr);
			break;
		case ERight:
			aBmpGc->DrawLine(aRect.iTl, TPoint(aRect.iBr.iX, (aRect.iTl.iY + aRect.iBr.iY) / 2));
			aBmpGc->DrawLineTo(TPoint(aRect.iTl.iX, aRect.iBr.iY));
			aBmpGc->DrawLineTo(aRect.iTl);
			break;
		case ELeft:
			aBmpGc->DrawLine(aRect.iBr, TPoint(aRect.iTl.iX, (aRect.iTl.iY + aRect.iBr.iY) / 2));
			aBmpGc->DrawLineTo(TPoint(aRect.iBr.iX, aRect.iTl.iY));
			aBmpGc->DrawLineTo(aRect.iBr);
			break;
	}
}


void NBitmapMethods::DrawMenuSymbol(CFbsBitGc* aBmpGc, TRect aRect)
{	
	// Top left -> top right
	aBmpGc->DrawLine(aRect.iTl, TPoint(aRect.iBr.iX, aRect.iTl.iY));
	// Middle left -> middle right
	aBmpGc->DrawLine(TPoint(aRect.iTl.iX, (aRect.iTl.iY + aRect.iBr.iY) / 2), 
			TPoint(aRect.iBr.iX, (aRect.iTl.iY + aRect.iBr.iY) / 2));
	// Bottom left -> bottom right
	aBmpGc->DrawLine(TPoint(aRect.iTl.iX, aRect.iBr.iY), aRect.iBr);
}

TSize NBitmapMethods::SetSizeToFullScreen(TSize aScreenSize, CFbsBitmap* aBitmap)
{
	if (!aBitmap)
		return TSize(0, 0);
	
	// To prevent re-loading of the bitmap from the storage medium
	//AknIconUtils::PreserveIconData(aBitmap);
	
	// Get the dimension of the svg file, as it is defined in the file.
	TSize bmpSize;
	AknIconUtils::GetContentDimensions(aBitmap, bmpSize);
	
	// Check the screen size, if it's not valid, return the size
	// of the svg as it would normally be
	if (aScreenSize.iWidth == 0 || aScreenSize.iHeight == 0)
		return bmpSize;
	
	// Calculate the aspect ratio of the screen and the bitmap
	const TReal aspectScreen = (TReal)aScreenSize.iWidth / (TReal)aScreenSize.iHeight;
	const TReal aspectBmp = (TReal)bmpSize.iWidth / (TReal)bmpSize.iHeight;
	TSize finalSize;
	
	// Set the size of the bitmap so that it fills the whole screen.
	// If the aspect ratio is different, this method ensures that the whole
	// screen is filled and the svg is clipped -> some parts of the svg
	// won't be visible, but the screen size bitmap won't have empty
	// spots and the aspect ratio of the bitmap won't change.
	if (aspectScreen <= aspectBmp)
	{
		finalSize.iHeight = aScreenSize.iHeight;
		finalSize.iWidth = (TInt)((TReal)aScreenSize.iHeight * aspectBmp);
	}
	else
	{
		finalSize.iWidth = aScreenSize.iWidth;
		finalSize.iHeight = (TInt)((TReal)aScreenSize.iWidth / aspectBmp);
	}
	return finalSize;
}


TSize NBitmapMethods::ScaleToWidth(TInt aWidth, CFbsBitmap* aBitmap)
{
	if (!aBitmap)
		return TSize(0, 0);
	
	// To prevent re-loading of the bitmap from the storage medium
	//AknIconUtils::PreserveIconData(aBitmap);
	
	// Get the dimension of the svg file, as it is defined in the file.
	TSize bmpSize;
	AknIconUtils::GetContentDimensions(aBitmap, bmpSize);
	
	// Check the screen size, if it's not valid, return the size
	// of the svg as it would normally be
	if (aWidth == 0)
		return bmpSize;
	
	// Calculate the aspect ratio of the scrren and the bitmap
	const TReal aspectBmp = (TReal)bmpSize.iWidth / (TReal)bmpSize.iHeight;
	TSize finalSize;
	finalSize.iWidth = aWidth;
	finalSize.iHeight = (TInt)((TReal)(aWidth) / aspectBmp);
	
	return finalSize;
}

TInt NBitmapMethods::setSvgSize(CFbsBitmap* aBitmap, const TSize& aSize)
{
	if (!aBitmap)
		return KErrBadHandle;
	
	TInt err = AknIconUtils::SetSize(aBitmap, aSize);
	if (err != KErrNone)
		{
		// Destroy cache and try again
		AknIconUtils::DestroyIconData(aBitmap);
		err = AknIconUtils::SetSize(aBitmap, aSize);
		}
	return err;
}

void NBitmapMethods::ReplaceColor(CFbsBitmap* aBmp, const TUint16 aSourceColor, const TUint16 aTargetColor)
{
    // Query size of the source bitmap
    const TSize bmpSize = aBmp->SizeInPixels();
    
    // Get the width of the source bitmap
    const TInt w = bmpSize.iWidth;
    
    // Correction for bitmaps with uneven width
    const TInt oddW =  w & 0x01;
    
    TBitmapUtil buSrcLock(aBmp);
    buSrcLock.Begin(TPoint(0,0));
    
    // Get the pointer to the bitmap's raw data
    TUint16* pRawSrc = (TUint16*)aBmp->DataAddress();
    
    for (TInt y = 0; y < bmpSize.iHeight; y++)
    {
        for (TInt x = 0; x < w; x++)
        {
        	// Check whether the current pixel is the color that should be replaced
            if ((*pRawSrc) == aSourceColor)
            {
            	// Yes, so replace the color
            	(*pRawSrc) = aTargetColor;
            }
            pRawSrc++;
        }
        pRawSrc += oddW;		// correction for bitmaps of odd width
    }
    
    buSrcLock.End();
}

//------------------------------------------------------------------------------
void NBitmapMethods::DarkenArea(CFbsBitmap* aBmp, TRect aArea, const TInt aBorder, const TInt aIntensity)
{
	// Get the size of the bitmap
    TSize bitmapSize = aBmp->SizeInPixels();
    
    // Extend the size of the area with the border and check
    // if its still within the image region.
	aArea.iTl.iX -= aBorder;
	if (aArea.iTl.iX < 0)
		aArea.iTl.iX = 0;
	aArea.iTl.iY -= aBorder;
	if (aArea.iTl.iY < 0)
		aArea.iTl.iY = 0;
		
	TInt areaWidth = aArea.Width() + aBorder;
	if (aArea.iTl.iX + areaWidth > bitmapSize.iWidth)
		areaWidth = bitmapSize.iWidth - aArea.iTl.iX;
	TInt areaHeight = aArea.Height() + aBorder;
	if (aArea.iTl.iY + areaHeight > bitmapSize.iHeight)
		areaHeight = bitmapSize.iHeight - aArea.iTl.iY;
		
	// Get the color depth of the image and create an appropriate mask
    TDisplayMode colorDepth = aBmp->DisplayMode();

    TUint16 mask = 0;
    if (aIntensity == 1)
    {
	    if (colorDepth == EColor64K)
	        mask = 0xF7DE;
	    else if (colorDepth == EColor4K)
	        mask = 0x0EEE;
    } else	// aIntensity = 2
    {
	    if (colorDepth == EColor64K)
	        mask = 0xE79C;
	    else if (colorDepth == EColor4K)
	        mask = 0x0CCC;
    }

    TBitmapUtil bmpUtil(aBmp);

	// Start access to the bitmap (locks it)
    bmpUtil.Begin(TPoint(0, 0));

	// The memory address of the upper left pixel
    TUint16* bitmapData = (TUint16*)aBmp->DataAddress();

	// Add the start offset to go directly to the point
	bitmapData += (aArea.iTl.iY * bitmapSize.iWidth + aArea.iTl.iX);

	// How much to add for a newline (adding bitmapSize.iWidth would go
	// down a whole line, positioning the pointer at the same x-position at the
	// right hand side of the rectangle, just one line farther down. When we 
	// subtract the width of the rectangle, we get to the left (start)-side of the rectangle.
	TInt newLineOffset = (bitmapSize.iWidth - areaWidth);

    for (TInt y = 0; y < areaHeight; y++)
    {
		// Round borders - in the first and last line, draw two pixels less
		if (y == 0 || y == areaHeight - 1)
		{
			areaWidth -= 2;	// Do not darken both border pixels
			bitmapData ++;	// Jump over first left pixel
		}

        for (TInt x = 0; x < areaWidth; x++)
        {
			// Mask the bitmap so that the leftmost bit of every RGB-color gets set to 0.
			// Then shift the whole word by one, in short making all RGB-values half.
			// We're using 12 bit color, but it's still stored in 16 bit, the first four are unused.
			//
			// Contents: ----RRRRGGGGBBBB 
			// Mask:     ----111-111-111- 
			// Shift:    -----RRR-GGG-BBB 
            //*bitmapData = ( *bitmapData & 0x0EEE) >> 1;
            //*bitmapData = ( *bitmapData & 0xF7DE) >> 1;
            *bitmapData = ( *bitmapData & mask) >> aIntensity;
			// Go to the next two pixels
            bitmapData ++;
        }
		
		// Round borders - in the first and last line, draw two pixels less
		// This here is only necessary after top line as we do not continue drawing
		// at the bottom.
		if (y == 0)
		{
			// For the next line, use normal width again
			areaWidth += 2;
			bitmapData ++;	// Correct left out top right pixel
		}

		// Go one line down
		bitmapData += newLineOffset;
    }
    bmpUtil.End();
}

