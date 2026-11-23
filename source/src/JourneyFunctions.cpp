/*
* ============================================================================
*  Name     : JourneyFunctions.cpp
*  Part of  : Journey
*  Created  : 04.05.2004 by Andreas Jakl
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

#include "JourneyFunctions.h"

const void NJourneyFunctions::AppendFullPath(const TDesC& aFilename, TFileName& aFullName)
{
    aFullName.Copy(aFilename);
    CompleteWithAppPath(aFullName);
}

const TInt NJourneyFunctions::GetStoreDriveL(RFs& aFs)
{
    TInt driveNum;
    TChar driveChar = NJourneyFunctions::GetStoreDriveCharL(aFs);
    aFs.CharToDrive(driveChar, driveNum);
    return driveNum;
}

const TChar NJourneyFunctions::GetStoreDriveCharL(RFs& aFs)
{
    // Symbian OS 9.2: return aFs.GetSystemDrive();
    // Get the full name and path of the application
    TFileName appNamePath = CEikonEnv::Static()->EikAppUi()->Application()->AppFullName();
#if defined(__WINS__) // if the resources are in emulators c (not z)
    appNamePath.Replace(0,1,_L("c"));
#endif
	return appNamePath[0];
}

const void NJourneyFunctions::AddPrivatePathL(RFs& aFs, const TDesC& aFilename, RBuf& aCompleteName)
{
	// Make sure the private path exists on the drive we need
    aFs.CreatePrivatePath(NJourneyFunctions::GetStoreDriveL(aFs));
    // Reset the RBuf-variable and create it with the required size
    aCompleteName.Close();
    aCompleteName.CreateL(255);
    // Get the private path
    aFs.PrivatePath(aCompleteName);
    // Get the drive where the application is installed again, this time
    // as char and not as a number
    TBuf<1> drive;
    drive.Append(NJourneyFunctions::GetStoreDriveCharL(aFs));
    // Insert the drive to the private path, otherwise it'd be missig
    aCompleteName.Insert(0, drive);
    _LIT(KCharColon, ":");
    aCompleteName.Insert(1, KCharColon);
    // Add the filename to the path, so that we now have a full URI, containing
    // the drive, path and filename
    aCompleteName.Append(aFilename);
}
