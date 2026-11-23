/*
* ============================================================================
*  Name     : CJourneyVGame from JourneyVGame.h
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
#include  "JourneyVGame.h"
#include  "JourneyCGame.h"
#include  "journey.hrh"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CJourneyVGame::ConstructL(const TRect& aRect)
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CJourneyVGame::ConstructL()
{
	BaseConstructL( R_JOURNEY_VIEW2 );
}

// ---------------------------------------------------------
// CJourneyVGame::~CJourneyVGame()
// ?implementation_description
// ---------------------------------------------------------
//
CJourneyVGame::~CJourneyVGame()
{
	if ( iContainer )
	{
		AppUi()->RemoveFromViewStack( *this, iContainer );
	}

	delete iContainer;
}

// ---------------------------------------------------------
// TUid CJourneyVGame::Id()
// ?implementation_description
// ---------------------------------------------------------
//
TUid CJourneyVGame::Id() const
{
	return KGameViewId;
}

// ---------------------------------------------------------
// CJourneyVGame::HandleCommandL(TInt aCommand)
// ?implementation_description
// ---------------------------------------------------------
//
void CJourneyVGame::HandleCommandL(TInt aCommand)
{
	TInt err;
	switch ( aCommand )
	{
	case EJourneyCmdMenu:
	{
		AppUi()->HandleCommandL(EAknSoftkeyOptions);
		break;
	}
	case EJourneyCmdReturnToMenu:
	{
		// Don't display an error message here to prevent problems
		// when the memory is full - the user should still be able
		// to return to the menu.
		// The game progress is saved without a trap every time
		// he advances to a new area, so this is more like a
		// "save to be sure"
		TRAP(err, iContainer->iEngine->SaveGameProgressL());
		AppUi()->HandleCommandL(EJourneyCmdReturnToMenu);
		break;
	}
	case EJourneyCmdExit:
	{
		// Same as above.
		TRAP(err, iContainer->iEngine->SaveGameProgressL());
		TInt userAction = DoPauseDlgL();
		if (userAction == EJourneyCmdPauseEnd)
		{
			AppUi()->HandleCommandL(EEikCmdExit);
		}
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
// CJourneyVGame::HandleClientRectChange()
// ---------------------------------------------------------
//
void CJourneyVGame::HandleClientRectChange()
{
	if ( iContainer )
	{
		iContainer->SetRect( ClientRect() );
	}
}

// ---------------------------------------------------------
// CJourneyVGame::DoActivateL(...)
// ?implementation_description
// ---------------------------------------------------------
//
void CJourneyVGame::DoActivateL(
    const TVwsViewId& /*aPrevViewId*/,TUid aCustomMessageId,
    const TDesC8& /*aCustomMessage*/)
{
	if (!iContainer)
	{
		iContainer = new (ELeave) CJourneyCGame(this);
		iContainer->SetMopParent(this);
		iContainer->ConstructL( ClientRect() );
		AppUi()->AddToStackL( *this, iContainer );
		if (aCustomMessageId.iUid == 0)
			iContainer->iEngine->StartNewGameL();
		else
			iContainer->iEngine->ContinueGameL();
	}
}

// ---------------------------------------------------------
void CJourneyVGame::DoDeactivate()
{
	if ( iContainer )
	{
		AppUi()->RemoveFromViewStack( *this, iContainer );
	}

	delete iContainer;
	iContainer = NULL;
}



// ---------------------------------------------------------
TInt CJourneyVGame::DoPauseDlgL()
{
	CAknQueryDialog* dlg;

	dlg = CAknQueryDialog::NewL( CAknQueryDialog::ENoTone );

	return dlg->ExecuteLD( R_JOURNEY_PAUSE_QUERY );
}


// End of File


