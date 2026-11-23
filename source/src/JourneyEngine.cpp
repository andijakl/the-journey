/*
* ============================================================================
*  Name     : CJourneyEngine from JourneyEngine.cpp
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

#include "JourneyEngine.h"


//------------------------------------------------------------------------------
CJourneyEngine* CJourneyEngine::NewL(CJourneyCGame* aOwningControl)
{
	CJourneyEngine* self = new (ELeave) CJourneyEngine( aOwningControl);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();        // self
	return self;
}

// Constructor
//------------------------------------------------------------------------------
CJourneyEngine::CJourneyEngine(CJourneyCGame* aOwningControl):
		iOwningControl(aOwningControl),
		iErrorState(KErrNone)
{}

//------------------------------------------------------------------------------
void CJourneyEngine::ConstructL()
{
	// Connect to the file server and create the directory if needed
	User::LeaveIfError(iFileServerSession.Connect());
	//iFileServerSession.MkDirAll(KFileStore);

	// Initialize story
	iStory = new (ELeave) CJourneyStory();
	iStory->InitStory();

	// Init location-engine
	iLocation = CJourneyLocation::NewL(this);

	iPeriodicTimer = NULL;
	iCurTextResId = -1;
	iJustStartedGame = EFalse;
}

// Destructor
//------------------------------------------------------------------------------
CJourneyEngine::~CJourneyEngine()
{
	// In case the timer is currently running, remove it
	StopTimer();

	iFileServerSession.Close();
	DELETE_AND_NULLZ(iLocation);
	DELETE_AND_NULLZ(iStory);
	DELETE_AND_NULLZ(iBmpBackground);

}

//------------------------------------------------------------------------------
void CJourneyEngine::StartNewGameL()
{
	iSearchActive = ETrue;
	iGameProgress = 0;
	iPrevAreaNum = iStory->iTalkInfos[iGameProgress].iAreaNum;
	iLocation->InitGoToArea(iStory->iTalkInfos[iGameProgress].iAreaNum, 1);
	StartTimerL();
	UpdateScreenL();
	iJustStartedGame = ETrue;
}

//------------------------------------------------------------------------------
void CJourneyEngine::ContinueGameL()
{
	if (GameProgressFileExistsL())
	{
		LoadGameProgressL();
		if (iSearchActive)
			StartTimerL();
		else
		{
			iLocation->InitGoToArea(iStory->iTalkInfos[iGameProgress].iAreaNum, 1);
			iSearchActive = ETrue;
			StartTimerL();
			/*TInt curAreaNum = iLocation->GetCurAreaNumL();
			if (iStory->iTalkInfos[iGameProgress].iAreaNum != curAreaNum)
			{
				iLocation->InitGoToArea(iStory->iTalkInfos[iGameProgress].iAreaNum, 1);
				iSearchActive = ETrue;
				StartTimerL();
			}*/
		}
		UpdateScreenL();
		iJustStartedGame = ETrue;
	}
	else
	{
		StartNewGameL();
	}
}

//------------------------------------------------------------------------------
void CJourneyEngine::UpdateScreenL()
{
	TInt locTextResId = 0;
	TInt storyTextResId = 0;
	TInt imgBmpId = -1;

	TInt curAreaNum = iLocation->GetCurAreaNumL();

	if (iErrorState != KErrNone)
	{
		locTextResId = iStory->iNoAreaInfo.iAreaNameRes;
	}
	else if (!iSearchActive)
	{
		// If no search is active (for example when changing from one story element to the next,
		// without a required area-change), always take the location name from the talk info that is
		// about to be displayed, and not the location name of where the player is currently located.
		// This is necessary so that the player can read the whole story of one location, even if
		// the location changes inbetween (could also be because the user is exactly on the border of
		// two cells).
		locTextResId = iStory->iAreaInfos[iStory->iTalkInfos[iGameProgress].iAreaNum].iAreaNameRes;

		// Choose picture of story-part. If none is available, use the area-pic
		imgBmpId = iStory->iTalkInfos[iGameProgress].iBitmapEnum;
	}
	else
	{
		// Search is active - decide whether to take the name of an assigned area or of an unused
		// area (e.g. "Streets")
		locTextResId = (curAreaNum >= 0) ? iStory->iAreaInfos[curAreaNum].iAreaNameRes : iStory->iNoAreaInfo.iAreaNameRes;
	}

	if (imgBmpId == -1)
		if (curAreaNum == -1)
			imgBmpId = iStory->iNoAreaInfo.iBitmapEnum;
		else
			imgBmpId = iStory->iAreaInfos[curAreaNum].iBitmapEnum;

	iOwningControl->DrawLocationText(locTextResId);

	if (imgBmpId != -1)
		iOwningControl->DrawLocationBitmapL(imgBmpId);

	// Update screen according to the area the user is currently in

	if (iErrorState != KErrNone)
	{
		switch (iErrorState)
		{
		case KErrAccessDenied:	// Phone in offline mode
			storyTextResId = R_OFFLINE;
			break;
		case KErrWrongNet:			// No GSM Network
			storyTextResId = R_NOGSM;
			break;
		case KErrCellId0:			// Cell ID = 0 at startup
			storyTextResId = R_NONET;
			break;
		}
	}
	else if (iSearchActive && iStory->iTalkInfos[iGameProgress].iAreaNum != curAreaNum)
	{
		if (curAreaNum == -1)
		{
			storyTextResId = iStory->iTalkInfos[iGameProgress].iNoAssignedAreaTextRes;
		}
		else
		{
			storyTextResId = iStory->iTalkInfos[iGameProgress].iWrongAreaTextRes[curAreaNum];
		}
	}
	else
	{
		storyTextResId = iStory->iTalkInfos[iGameProgress].iTextRes;
	}

	ChangeText(storyTextResId);
}

void CJourneyEngine::ChangeText(const TInt aTextResourceId)
{
	if (aTextResourceId != iCurTextResId)
	{
		iOwningControl->UpdateText(aTextResourceId);
		iCurTextResId = aTextResourceId;
	}
}


//------------------------------------------------------------------------------
void CJourneyEngine::StartTimerL()
{
	// tickInterval in microseconds: 5000000 equals to 5 seconds
	const TTimeIntervalMicroSeconds32 KTickInterval = 5000000;
	// Delay for the first start of the timer (has to be >0 )
	const TTimeIntervalMicroSeconds32 KTickFirstDelay = 10;
	iPeriodicTimer = CPeriodic::NewL(CActive::EPriorityLow);
	iPeriodicTimer->Start(KTickFirstDelay, KTickInterval, TCallBack(TimerCallBackL, this));
}

//------------------------------------------------------------------------------
void CJourneyEngine::StopTimer()
{
	if (iPeriodicTimer)
	{
		iPeriodicTimer->Cancel();
		DELETE_AND_NULLZ(iPeriodicTimer);
	}
}

//------------------------------------------------------------------------------
TInt CJourneyEngine::TimerCallBackL(TAny* aObject)
{
	// Call non-static method
	return ((CJourneyEngine*)aObject)->DoCallBackL();
}

//------------------------------------------------------------------------------
TInt CJourneyEngine::DoCallBackL()
{
	if (iSearchActive)
	{
		iLocation->StartGetCellIdL();
		if (iJustStartedGame && iLocation->GetLatestCellId() == 0)
		{
			ChangeText(iStory->iNoNetTextRes);
		}
	}
	else
	{
		StopTimer();
	}
	UpdateScreenL();
	return iSearchActive;
}

//------------------------------------------------------------------------------
void CJourneyEngine::SearchEventCompleted()
{
	if (iLocation->GetLatestCellId() > 0)
	{
		iErrorState = KErrNone;
		if (iJustStartedGame)
		{
			iJustStartedGame = EFalse;
		}
	}
	iSearchActive = EFalse;
	// The task the program waited for has been completed - continue the game
	TRAPD(err, SaveGameProgressL());

	//ContinueGame();
	StopTimer();
	TRAPD(err2, UpdateScreenL());
}

//------------------------------------------------------------------------------
void CJourneyEngine::LocationError(TInt aErrCode)
{
	if (aErrCode == KErrCellId0)		// Cell-ID is 0
	{
		if (iSearchActive || iJustStartedGame)
		{
			// Only save this error code (cell id == 0) if a search is active
			// We don't want to display an error instantly if the
			// user wouldn't be affected by it.
			iErrorState = aErrCode;
		}
	}
	else
	{
		// All other errors will be saved to the error state instantly.
		iErrorState = aErrCode;
	}
	TRAPD(err2, UpdateScreenL());
}

//------------------------------------------------------------------------------
void CJourneyEngine::HandleJoystickPressL()
{
	if (!iSearchActive)
	{
		if (iGameProgress == (MAX_NUM_TALKINFOS - 1))
		{
			// Played through the game
			DeleteGameProgressL();

			CEikonEnv::Static()->EikAppUi()->HandleCommandL(EJourneyCmdReturnToMenu);
		}
		else
		{
			iPrevAreaNum = iStory->iTalkInfos[iGameProgress].iAreaNum;
			iGameProgress ++;
			TInt newAreaNum = iStory->iTalkInfos[iGameProgress].iAreaNum;

			if (iPrevAreaNum != newAreaNum)
			{
				iLocation->InitGoToArea(newAreaNum, iStory->iAreaInfos[newAreaNum].iHowFar);
				iSearchActive = ETrue;
			}

			SaveGameProgressL();
			StartTimerL();
		}
	}
}


//------------------------------------------------------------------------------
void CJourneyEngine::LoadArtL()
{
	TFileName fullName;
	NJourneyFunctions::AppendFullPath(KJourneyMif, fullName);
	iBmpBackground = AknIconUtils::CreateIconL(fullName, EMbmJourneyInterfacebackground);
}

//------------------------------------------------------------------------------
void CJourneyEngine::DeleteGameProgressL()
{
	RBuf fullName;
	fullName.CleanupClosePushL();
	NJourneyFunctions::AddPrivatePathL(iFileServerSession, KFileStore, fullName);

	// Delete the file through the file server session
	iFileServerSession.Delete(fullName);
	CleanupStack::PopAndDestroy();
}

//------------------------------------------------------------------------------
void CJourneyEngine::SaveGameProgressL()
{	
	// Create full filename
	RBuf fullName;
	fullName.CleanupClosePushL();
	NJourneyFunctions::AddPrivatePathL(iFileServerSession, KFileStore, fullName);

	// Create a direct file store that will contain the game progress
	CFileStore* store = CDirectFileStore::ReplaceLC(iFileServerSession, fullName, EFileWrite);
	store->SetTypeL(KDirectFileStoreLayoutUid);

	// Create the stream
	RStoreWriteStream stream;
	TStreamId id = stream.CreateLC(*store);

	// Write game progress
	stream.WriteInt32L(PROGRESS_FILE_VERSION_NUMBER);
	stream.WriteInt32L(iGameProgress);
	stream.WriteInt32L(iSearchActive);
	stream.WriteInt32L(iPrevAreaNum);

	// Then write the location-data
	iLocation->ExternalizeL(stream);

	// Commit the changes to the stream
	stream.CommitL();
	CleanupStack::PopAndDestroy();	// stream

	// Set the stream in the store and commit the store
	store->SetRootL(id);
	store->CommitL();
	CleanupStack::PopAndDestroy(2);	// store, fullName
}

//------------------------------------------------------------------------------
void CJourneyEngine::LoadGameProgressL()
{
	// Create full filename
	RBuf fullName;
	fullName.CleanupClosePushL();
	NJourneyFunctions::AddPrivatePathL(iFileServerSession, KFileStore, fullName);

	// Open the file store
	CFileStore* store = CDirectFileStore::OpenLC(iFileServerSession, fullName, EFileRead);

	// Open the data stream inside the store
	RStoreReadStream stream;
	stream.OpenLC(*store, store->Root());

	// Read all the data
	TInt fileVersion = stream.ReadInt32L();
	if (fileVersion == PROGRESS_FILE_VERSION_NUMBER)
	{
		// Only load the data if the file version of the saved file
		// matches with the compatible version of the current application
		iGameProgress = stream.ReadInt32L();
		iSearchActive = stream.ReadInt32L();
		iPrevAreaNum = stream.ReadInt32L();
	
		iLocation->InternalizeL(stream);
	}

	// Delete all the remaining stuff on the cleanup stack
	// (fullName, store, stream)
	CleanupStack::PopAndDestroy(3);
}


//------------------------------------------------------------------------------
TBool CJourneyEngine::GameProgressFileExistsL()
{
	TBool returnVar = ETrue;

	// Create full filename
	RBuf fullName;
	fullName.CleanupClosePushL();
	NJourneyFunctions::AddPrivatePathL(iFileServerSession, KFileStore, fullName);

	// Try to open the file
	RFile checkFile;
	TInt errorCode = checkFile.Open(iFileServerSession, fullName, EFileRead);
	// If the file is not found, return false.
	if (errorCode == KErrNotFound)
		returnVar = EFalse;

	// Close the handle to the file
	checkFile.Close();
	CleanupStack::PopAndDestroy();

	return returnVar;
}

