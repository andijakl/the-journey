/*
============================================================================
 Name        : JourneyAppUi.cpp
 Author      : Andreas Jakl
 Copyright   : 2007, Mopius
 Description : CJourneyAppUi implementation
============================================================================
*/

// INCLUDE FILES
#include "JourneyAppUi.h"

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CJourneyAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CJourneyAppUi::ConstructL()
{
	// Initialise app UI with standard value.
	BaseConstructL();

	iSharedData = new(ELeave)TJourneySharedData();
	iSharedData->iCommInterface = this;

	// Check version of ini file
	TBool loadSettings = EFalse;
	TInt versionFileNumber = 0;
	TUint settingsSize = sizeof(versionFileNumber);
	if (ReadIniFile(uidVersion, &versionFileNumber, settingsSize))
	{
		if (versionFileNumber == INI_FILE_VERSION_NUMBER)
			loadSettings = ETrue;
	}

	if (!loadSettings)
	{
		// Either no ini file was present, or it was the wrong version number.
		// -> Create it / update contents to the new version number
		versionFileNumber = INI_FILE_VERSION_NUMBER;
		WriteIniFile(uidVersion, &versionFileNumber, settingsSize);
	}

	// Load Settings
	TJourneySettings settings;
	settingsSize = sizeof(settings);
	if (loadSettings && ReadIniFile(uidSettings, &settings, settingsSize))
	{
		iSharedData->iSettings = settings;
	}
	else
	{
		// Use default values
		iSharedData->iSettings.iAcceptedPrivacy = EFalse;
		WriteIniFile(uidSettings, &iSharedData->iSettings, settingsSize);
	}

	CJourneyVMenu* view1 = new (ELeave) CJourneyVMenu;

	CleanupStack::PushL( view1 );
	view1->ConstructL();
	AddViewL( view1 );      // transfer ownership to CAknViewAppUi
	CleanupStack::Pop();    // view1

	CJourneyVGame* view2 = new (ELeave) CJourneyVGame;

	CleanupStack::PushL( view2 );
	view2->ConstructL();
	AddViewL( view2 );      // transfer ownership to CAknViewAppUi
	CleanupStack::Pop();    // view2

	SetDefaultViewL(*view1);

}
// -----------------------------------------------------------------------------
// CJourneyAppUi::CJourneyAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CJourneyAppUi::CJourneyAppUi()
{
	// No implementation required
}

// -----------------------------------------------------------------------------
// CJourneyAppUi::~CJourneyAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CJourneyAppUi::~CJourneyAppUi()
{
	if ( iPeriodicTimer )
	{
		// Stop the periodic timer
		iPeriodicTimer->Cancel();
	}
	delete iPeriodicTimer;
	iPeriodicTimer = NULL;
	delete iSharedData;
	iSharedData = NULL;
}

// -----------------------------------------------------------------------------
// CJourneyAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CJourneyAppUi::HandleCommandL( TInt aCommand )
{
	switch ( aCommand )
	{
	case EAknSoftkeyOptions:
	{
		ProcessCommandL(aCommand);
		break;
	}
	case EEikCmdExit:
	{
		Exit();
		break;
	}
	case EJourneyCmdStartGame:
	{
		// Activate the game-view and tell it to start a new game.
		// A loading text is displayed, as the new view needs quite some
		// time to load the vector graphics. Therefore, we start a timer,
		// which will start the actual view switch with a slight delay,
		// giving time to the menu view to display the loading text.
		PrepareViewSwitch(KGameViewId, TUid::Uid(0));
		break;
	}
	case EJourneyCmdContinueGame:
	{
		// See the comment for starting the game
		PrepareViewSwitch(KGameViewId, TUid::Uid(1));
		break;
	}
	case EJourneyCmdAbout:
	{
		// Display a simple system about box.
		CAknInformationNote* note = new (ELeave) CAknInformationNote();
		HBufC* aboutText = CEikonEnv::Static()->AllocReadResourceLC(R_ABOUT_TEXT);
		note->ExecuteLD(*aboutText);
		CleanupStack::PopAndDestroy(aboutText);
		break;
	}
	case EJourneyCmdReturnToMenu:
	{
		// S60 seems to cache the parsed svg-files, so switching back
		// is fast and doesn't need anything asynchronous.
		ActivateLocalViewL(KMenuViewId);
		break;
	}
	default:
		break;
	}
}
// -----------------------------------------------------------------------------
//  Called by the framework when the application status pane
//  size is changed.  Passes the new client rectangle to the
//  AppView
// -----------------------------------------------------------------------------
//
void CJourneyAppUi::HandleStatusPaneSizeChange()
{}

// -----------------------------------------------------------------------------
// CJourneyAppUi::PrepareViewSwitch()
// Start an asynch. view switch, giving time to the current view to display
// a loading-box.
// -----------------------------------------------------------------------------
//
void CJourneyAppUi::PrepareViewSwitch(TUid aTargetViewId, TUid aParameter)
{
	// Tell the view to display the loading-textbox
	TUid curViewId = FocusedView();
	CJourneyVMenu* menuView = (static_cast<CJourneyVMenu*>(View(KMenuViewId)));
	if (menuView)
	{
		menuView->ShowLoadingText(ETrue);
	}
	// Start a timer so that the view-switch will be started soon,
	// after the view got time to do a redraw etc.
	iTargetViewId = aTargetViewId;
	iTargetViewParam = aParameter;
	iPeriodicTimer = CPeriodic::NewL( CActive::EPriorityStandard );
	iPeriodicTimer->Start(100, 1000, TCallBack( CJourneyAppUi::Period, this));
}

// -----------------------------------------------------------------------------
// CJourneyAppUi::Period()
// This function is called by the periodic timer.
// -----------------------------------------------------------------------------
//
TInt CJourneyAppUi::Period( TAny* aPtr )
{
	( static_cast<CJourneyAppUi*>( aPtr ) )->DoPeriodTask();
	// Returning a value of EFalse indicates the callback should not be done again
	return EFalse;
}

// -----------------------------------------------------------------------------
// CJourneyAppUi::DoPeriodTask()
// Switch to the new view.
// -----------------------------------------------------------------------------
//
void CJourneyAppUi::DoPeriodTask()
{
	TPtrC8 temp;
	// Start the view switch now, after the delay has passed.
	// Only an ActivateLocalViewL with one parameter specifying the
	// view is available, or the one used here with three parameters
	// and an descriptor as the third, which we do not need here, but still
	// have to pass.
	ActivateLocalViewL(iTargetViewId, iTargetViewParam, temp);
	// We do no longer need the timer
	iPeriodicTimer->Cancel();
}

// -----------------------------------------------------------------------------
TBool CJourneyAppUi::ReadIniFile(TUid aInfo, void *aPtr, TUint &aSize)
{
	TBool result = EFalse;
	TRAPD(err, result = DoReadIniFileL(aInfo, aPtr, aSize));
	return result;
}

// -----------------------------------------------------------------------------
TBool CJourneyAppUi::DoReadIniFileL(TUid aInfo, void *aPtr, TUint &aSize)
{
	TBool result = EFalse;
	RFs fs;
	fs.Connect();
	CleanupClosePushL( fs );
	RDictionaryReadStream rdsIniFile;
	CDictionaryStore *cdIniFile = Application()->OpenIniFileLC(fs);
	if (cdIniFile->IsPresentL(aInfo))
	{
		rdsIniFile.OpenLC(*cdIniFile, aInfo);
		TPtr8 buf((TUint8*)aPtr, aSize);
		rdsIniFile.ReadL(buf);
		CleanupStack::PopAndDestroy(); // rdsIniFile
		result = ETrue;
	}
	CleanupStack::PopAndDestroy( 2 ); // fs, cdIniFile
	return result;
}

// -----------------------------------------------------------------------------
void CJourneyAppUi::WriteIniFile(TUid aInfo, void *aPtr, TUint aSize)
{
	TRAPD(err, DoWriteIniFileL(aInfo, aPtr, aSize));
}

// -----------------------------------------------------------------------------
void CJourneyAppUi::DoWriteIniFileL(TUid aInfo, void *aPtr, TUint aSize)
{
	RFs fs;
	fs.Connect();
	CleanupClosePushL( fs );
	RDictionaryWriteStream rdsIniFile;
	CDictionaryStore *cdIniFile = Application()->OpenIniFileLC(fs);
	rdsIniFile.AssignLC(*cdIniFile, aInfo);
	TPtr8 buf((TUint8*)aPtr, aSize, aSize);
	rdsIniFile.WriteL(buf);
	rdsIniFile.CommitL();
	CleanupStack::PopAndDestroy();
	cdIniFile->CommitL();
	CleanupStack::PopAndDestroy( 2 );
}

// End of File
