/*
* ============================================================================
*  Name     : CJourneyVMenu from JourneyVMenu.h
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
#include  <aknviewappui.h>
#include  <avkon.hrh>
#include  <Journey.rsg>
#include  "JourneyVMenu.h"
#include  "JourneyCMenu.h"
#include  "journey.hrh"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CJourneyVMenu::ConstructL(const TRect& aRect)
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CJourneyVMenu::ConstructL()
{
	BaseConstructL( R_JOURNEY_VIEW1 );
}

// ---------------------------------------------------------
// CJourneyVMenu::~CJourneyVMenu()
// ?implementation_description
// ---------------------------------------------------------
//
CJourneyVMenu::~CJourneyVMenu()
{
	if ( iContainer )
	{
		AppUi()->RemoveFromViewStack( *this, iContainer );
	}

	delete iContainer;
}

// ---------------------------------------------------------
// TUid CJourneyVMenu::Id()
// ?implementation_description
// ---------------------------------------------------------
//
TUid CJourneyVMenu::Id() const
{
	return KMenuViewId;
}

// ---------------------------------------------------------
// CJourneyVMenu::HandleCommandL(TInt aCommand)
// ?implementation_description
// ---------------------------------------------------------
//
void CJourneyVMenu::HandleCommandL(TInt aCommand)
{
	switch ( aCommand )
	{
	case EJourneyCmdMenu:
	{
		CJourneyAppUi *appUi = (CJourneyAppUi *)AppUi();
		if (appUi && appUi->iSharedData && appUi->iSharedData->iSettings.iAcceptedPrivacy)
		{
			AppUi()->HandleCommandL(EAknSoftkeyOptions);
		}
		break;
	}
	case EEikCmdExit:
	case EJourneyCmdExit:
	{
		AppUi()->HandleCommandL(EEikCmdExit);
		break;
	}
	default:
	{
		AppUi()->HandleCommandL( aCommand );
		break;
	}
	}
}

// ---------------------------------------------------------
// CJourneyVMenu::HandleClientRectChange()
// ---------------------------------------------------------
//
void CJourneyVMenu::HandleClientRectChange()
{
	if ( iContainer )
	{
		iContainer->SetRect( ClientRect() );
	}
}

// ---------------------------------------------------------
// CJourneyVMenu::DoActivateL(...)
// ?implementation_description
// ---------------------------------------------------------
//
void CJourneyVMenu::DoActivateL(
    const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/)
{
	if (!iContainer)
	{
		CJourneyAppUi *appUi = (CJourneyAppUi *)AppUi();
		iContainer = new (ELeave) CJourneyCMenu(*(appUi->iSharedData), this);
		;
		iContainer->SetMopParent(this);
		iContainer->ConstructL( ClientRect() );
		AppUi()->AddToStackL( *this, iContainer );
	}
	iContainer->ShowLoadingText(EFalse);
}

// ---------------------------------------------------------
// CJourneyVMenu::HandleCommandL(TInt aCommand)
// ?implementation_description
// ---------------------------------------------------------
//
void CJourneyVMenu::DoDeactivate()
{
	if ( iContainer )
	{
		AppUi()->RemoveFromViewStack( *this, iContainer );
	}

	delete iContainer;
	iContainer = NULL;
}


// ---------------------------------------------------------
void CJourneyVMenu::ShowLoadingText(const TBool aShowText)
{
	if (iContainer)
	{
		iContainer->ShowLoadingText(aShowText);
	}
}

// ------------------------------------------------------------------------------
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
//
void CJourneyVMenu::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	if (R_JOURNEY_VIEW1_MENU == aResourceId)
	{
		// Check if the progress file is here, in that case allow to continue the game
		TBool progressFileExists = ETrue;

		RFs fileServerSession;
		User::LeaveIfError(fileServerSession.Connect());

		RBuf fullName;
		fullName.CleanupClosePushL();
		NJourneyFunctions::AddPrivatePathL(fileServerSession, KFileStore, fullName);
		
		RFile checkFile;
		TInt errorCode = checkFile.Open(fileServerSession, fullName, EFileRead);

		if (errorCode == KErrNotFound)
			progressFileExists = EFalse;

		checkFile.Close();
		fileServerSession.Close();
		CleanupStack::PopAndDestroy();	// fullName

		// Dim "Continue Game" if the progress file doesn't exist
		aMenuPane->SetItemDimmed(EJourneyCmdContinueGame, !progressFileExists);
		return;
	}

}
// End of File

