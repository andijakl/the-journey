/*
============================================================================
 Name        : JourneyDocument.cpp
 Author      : Andreas Jakl
 Copyright   : 2007, Mopius
 Description : CJourneyDocument implementation
============================================================================
*/


// INCLUDE FILES
#include "JourneyAppUi.h"
#include "JourneyDocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CJourneyDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CJourneyDocument* CJourneyDocument::NewL( CEikApplication& aApp )
{
	CJourneyDocument* self = NewLC( aApp );
	CleanupStack::Pop( self );
	return self;
}

// -----------------------------------------------------------------------------
// CJourneyDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CJourneyDocument* CJourneyDocument::NewLC( CEikApplication& aApp )
{
	CJourneyDocument* self =
	    new ( ELeave ) CJourneyDocument( aApp );

	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
}

// -----------------------------------------------------------------------------
// CJourneyDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CJourneyDocument::ConstructL()
{
	// No implementation required
}

// -----------------------------------------------------------------------------
// CJourneyDocument::CJourneyDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CJourneyDocument::CJourneyDocument( CEikApplication& aApp )
		: CAknDocument( aApp )
{
	// No implementation required
}

// ---------------------------------------------------------------------------
// CJourneyDocument::~CJourneyDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CJourneyDocument::~CJourneyDocument()
{
	// No implementation required
}

// ---------------------------------------------------------------------------
// CJourneyDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CJourneyDocument::CreateAppUiL()
{
	// Create the application user interface, and return a pointer to it;
	// the framework takes ownership of this object
	return ( static_cast <CEikAppUi*> ( new ( ELeave )
	                                    CJourneyAppUi ) );
}

// End of File
