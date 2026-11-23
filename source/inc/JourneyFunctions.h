/*
* ============================================================================
*  Name     : JourneyFunctions.h
*  Part of  : Journey
*  Created  : 04.05.2004 by Andreas Jakl
*  Description:
*     Common functions.
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

#ifndef __JOURNEYFUNCTIONS_H__
#define __JOURNEYFUNCTIONS_H__

#include <e32std.h>

#include <eikenv.h>
#include <bautils.h>
#include <eikappui.h>
#include <eikapp.h>
#include <aknutils.h>

/**
* The multiple bitmap file containing the game graphics.
*/
_LIT(KJourneyMif, "\\resource\\apps\\Journey.mif");

/*! 
  @namespace NJourneyFunctions
  
  @discussion Useful functions that are necessary for many parts of the game.
  */
namespace NJourneyFunctions
{
    /**
    * Adds the full path and drive letter of the application to a filename.
    * @param aFilename the filename the path should be added to.
    * @param aFullName the filename with its full path is written to this
    * variable (pass by reference).
    */
    const void AppendFullPath(const TDesC& aFilename, TFileName& aFullName);
    
    /**
     * Get the drive number of the drive where this application is installed
     * (and therefore where it should put its data files)
     * @param aFs file server session, required to get the drive name
     * @return drive number of the drive where the app. is installed
     */
    const TInt GetStoreDriveL(RFs& aFs);
    
    /**
     * Get the drive letter of the drive where this application is installed
     * (and therefore where it should put its data files)
     * @param aFs file server session, required to get the drive name
     * @return drive letter (1 char) where the app. is installed
     */
    const TChar GetStoreDriveCharL(RFs& aFs);
    
    /**
     * Add the drive and full private path to the specified filename.
     * The drive will be the drive where the application was installed.
     * @param aFs file server session, required to get the drive name.
     * @param aFilename name of the file that should be appended to the full
     * private path.
     * @param aCompleteName the contents of this RBuf will be reset, it is
     * filled with the full URI of the specified filename - including
     * the drive and private path of the application.
     */
    const void AddPrivatePathL(RFs& aFs, const TDesC& aFilename, RBuf& aCompleteName);
}

#endif
