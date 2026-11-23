/*
============================================================================
 Name        : CSysInfo.cpp
 Author      : Andreas Jakl
 Version     : 1.21
 Copyright   : Copyright 2006, Mopius
 Description : CSysInfo.cpp - source file
============================================================================
*/

// INCLUDE FILES
#include "SysInfo.h"

// ================= MEMBER FUNCTIONS =======================
/**
 * Standard Symbian OS static function for creating an instance of the object.
 * If creation fails, a leave will be sent.
 * @param aContainer reference to the callback observer.
 */
CSysInfo* CSysInfo::NewL(MLocationCallback* aObserver)
{
	CSysInfo* self = new (ELeave) CSysInfo(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

//------------------------------------------------------------------------------
CSysInfo::CSysInfo(MLocationCallback* aObserver)
		: CActive(EPriorityStandard),
		iCellId(0),
		iTelServerOpen(EFalse),
		iNetworkInfoV1Pckg(iNetworkInfoV1),
		iObserver(aObserver)
{
	CActiveScheduler::Add(this);
}

//------------------------------------------------------------------------------
void CSysInfo::ConstructL()
{
	iTelephony = CTelephony::NewL();
	iTelServerOpen = ETrue;
}

//------------------------------------------------------------------------------
CSysInfo::~CSysInfo()
{
	CloseTelServer();
}

//------------------------------------------------------------------------------
void CSysInfo::CloseTelServer( void )
{
	if ( iTelServerOpen )
	{
		delete iTelephony;
		iTelephony = NULL;
		iTelServerOpen = EFalse;
	}
}

//------------------------------------------------------------------------------
void CSysInfo::StartGetNetworkInfo()
{
	if (!iTelServerOpen)
		return;

	if (IsActive())
		Cancel();

	// Send the request to the asynchronous service provider
	iTelephony->GetCurrentNetworkInfo(iStatus, iNetworkInfoV1Pckg);
	// We're waiting for the callback...
	SetActive();
}

//------------------------------------------------------------------------------
void CSysInfo::RunL()
{
#ifdef __WINS__
	// Simulate that we got a valid cell-id for windows,
	// so that it's possible to debug everything.
	// Note: For testing, also simulate the error that happens
	// when not returning a cell id in the emulator!
	iCellId = 1;
	iObserver->GotLocation(KErrNone, iCellId);
	return;
#endif
	if (iStatus==KErrNone)
	{
		CTelephony::TNetworkMode mode = iNetworkInfoV1.iMode;
		
		if (mode != CTelephony::ENetworkModeUnregistered)
		{
			//CEikonEnv::InfoWinL(_L("Got cell id"), _L(""));
			iCellId = iNetworkInfoV1.iCellId;
			iObserver->GotLocation(KErrNone, iCellId);
		}
		else
		{
			iObserver->GotLocation(KErrWrongNet, 0);
		}
	}
	else
	{
		// Return the error code
		iObserver->GotLocation(iStatus.Int(), 0);
	}
}

//------------------------------------------------------------------------------
TUint CSysInfo::GetLatestCellId()
{
	return iCellId;
}

//------------------------------------------------------------------------------
void CSysInfo::DoCancel()
{
	iTelephony->CancelAsync(CTelephony::EGetCurrentNetworkInfoCancel);
}

// End of File
