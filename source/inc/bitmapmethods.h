/* Copyright (c) 2001, Nokia. All rights reserved */
/*
* ============================================================================
*  Name     : BitmapMethods.h
*  Description:
*     Some additional bitmap utility functions, partly based on the
*     utilities by Nokia.
*  Version  : 1.32
*  Copyright: Mopius 2004-2007
* ============================================================================
*/

#ifndef __BITMAPMETHODS__
#define __BITMAPMETHODS__

#define KColourDepth EColor4K

//Ensure these libraries are linked against:
//LIBRARY           bitgdi.lib


#include <e32base.h>
#include <gdi.h>
#include <akniconutils.h> 

class CFbsBitGc;
class CFbsBitmapDevice;
class CWsBitmap;
class CFbsBitmap;


/*!
  @namespace NBitmapMethods

  @discussion A set of functions to make bitmap manipulation easier
  */
namespace NBitmapMethods
{
/*!
  @function CreateGraphicsContextLC

  @discussion Create a graphics context and leave it on the cleanup stack
  @param aBitmapDevice a reference to a bitmap device
  */
CFbsBitGc* CreateGraphicsContextLC(CFbsBitmapDevice& aBitmapDevice);

/*!
  @function CreateGraphicsContextL

  @discussion Create a graphics context
  @param aBitmapDevice a reference to a bitmap device
  */
CFbsBitGc* CreateGraphicsContextL(CFbsBitmapDevice& aBitmapDevice);

/*!
  @function CreateBitmapDeviceLC

  @discussion Create a bitmap device and leave it on the cleanup stack
  @param aBitmap a reference to a bitmap
  */
CFbsBitmapDevice* CreateBitmapDeviceLC(CFbsBitmap& aBitmap);

/*!
  @function CreateBitmapDeviceL

  @discussion Create a bitmap device
  @param aBitmap a reference to a bitmap
  */
CFbsBitmapDevice* CreateBitmapDeviceL(CFbsBitmap& aBitmap);

/*!
  @function CreateBitmapLC

  @discussion Create a bitmap and leave it on the cleanup stack
  @param aSizeInPixels the size of the bitmap to be created
  @param aDispMode the display mode of the bitmap
  */
CFbsBitmap* CreateBitmapLC(TSize aSizeInPixels,TDisplayMode aDispMode);

/*!
  @function CreateBitmapL

  @discussion Create a bitmap
  @param aSizeInPixels the size of the bitmap to be created
  @param aDispMode the display mode of the bitmap
  */
CFbsBitmap* CreateBitmapL(TSize aSizeInPixels,TDisplayMode aDispMode);

/*!
  @function CreateBitmapLC

  @discussion Create a bitmap from an mbm file bitmap and leave it on the cleanup stack
  @param aFileName the name of an mbm file
  @param aId the position of the bitmap in the mbm file
  */
CFbsBitmap* CreateBitmapLC(const TDesC& aFileName,TInt aId);

/*!
  @function CreateBitmapLC

  @discussion Create a bitmap from an mbm file bitmap
  @param aFileName the name of an mbm file
  @param aId the position of the bitmap in the mbm file
  */
CFbsBitmap* CreateBitmapL(const TDesC& aFileName,TInt aId);

/*!
  @function BitBltMaskedEntireBitmap

  @discussion Blit the entire of a bitmap with a mask onto a gc
  @param aTargetGc gc to blit onto
  @param aTopLeft the position at which the top left of the bitmap will be placed on the gc
  @param aBitmap the bitmap to blit
  @param aBitMask the mask
  */
void BitBltMaskedEntireBitmap(CFbsBitGc& aTargetGc,TPoint aTopLeft,const CFbsBitmap& aBitmap,
                              const CFbsBitmap& aBitMask);
/*!
  @function PartialReset

  @discussion Reset a gc so that it is in a sensible state.
  Not all settings are reset, just some of the more useful ones
  @param aGc the gc to reset
  */
void PartialReset(CFbsBitGc& aGc);

typedef enum {
	ERight = 0,
	EDown,
	ELeft,
	EUp
} TArrowDirection;

/**
 * Draw an arrow that fills the specified rect.
 * Uses the pen color that has been set previously, it
 * does not change settings of the bitmap graphics context.
 * The direction specifies in which direction the arrow should point.
 * @param aBmpGc graphics context for the bitmap to draw to.
 * @param aRect the rectangle that should be filled with the arrow.
 * @param aDirection direction in which the arrow should point.
 */
void DrawArrow(CFbsBitGc* aBmpGc, TRect aRect, TArrowDirection aDirection);

/**
 * Draw the menu symbol for touch devices.
 * The menu symbol is three horizontal lines below each other.
 * Uses the pen color that has been set previously, it
 * does not change settings of the bitmap graphics context.
 * @param aBmpGc graphics context for the bitmap to draw to.
 * @param aRect the rectangle that should be filled with the arrow.
 * 
 */
void DrawMenuSymbol(CFbsBitGc* aBmpGc, TRect aRect);

/**
 * Scans through the whole bitmap and replaces every
 * pixel that has the source color with the target color.
 * Works for EColor64K images, uses a TUint16-pointer to go through the image.
 * @param aBmp bitmap to scan and modify.
 * @param aSourceColor the color that should be replaced with the target color.
 * @param aTargetColor the target color to use.
 */
void ReplaceColor(CFbsBitmap* aBmp, const TUint16 aSourceColor, const TUint16 aTargetColor);

/**
 * Return a size that makes the bitmap full screen
 * based on the screen size, while keeping the aspect ratio
 * of the image.
 * If the image doesn't have the same aspect ratio as the screen,
 * this means that some parts of the image will be invisible
 * if the returned size is used.
 * @param aScreenSize size of the whole screen that should be filled
 * by the bitmap.
 * @param aBitmap bitmap that's used to get its size.
 * @return the size the bitmap needs to have keeping its aspect ratio
 * so that it's displayed full screen.
 */
TSize SetSizeToFullScreen(TSize aScreenSize, CFbsBitmap* aBitmap);

/**
 * Calculate the new image size if the new width should be used and
 * aspect ratio should be kept.
 * @param aWidth new target width of the image
 * @param aBitmap bitmap that's used to get its size.
 * @return the size of the bitmap with the specified width and the
 * according height that keeps the aspect ratio
 */
TSize ScaleToWidth(TInt aWidth, CFbsBitmap* aBitmap);

/**
 * Scales the svg image to the new size.
 */
TInt setSvgSize(CFbsBitmap* aBitmap, const TSize& aSize);

/**
 * Make the image darker at the specified region.
 * Works for EColor64K and EColor4K-images. Reduces the brightness of
 * the area through a shift operation. Uses a slightly rounded rectangle
 * (1px)
 * @param aBmp image to modify.
 * @param aArea area to process
 * @param aBorder additional border around the area which should be processed as well.
 */
void DarkenArea(CFbsBitmap* aBmp, const TRect aArea, const TInt aBorder, const TInt aIntensity);
}

#endif //__BITMAPMETHODS__
