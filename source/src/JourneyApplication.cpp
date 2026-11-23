/*
============================================================================
 Name        : JourneyApplication.cpp
 Author      : Andreas Jakl
 Copyright   : 2007, Mopius
 Description : Main application class
============================================================================
*/

// INCLUDE FILES
#include "Journey.hrh"
#include "JourneyDocument.h"
#include "JourneyApplication.h"

// ============================ MEMBER FUNCTIONS ===============================

// UID for the application;
// this should correspond to the uid defined in the mmp file
const TUid KUidJourneyApp = { _UID3 };

// -----------------------------------------------------------------------------
// CJourneyApplication::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CJourneyApplication::CreateDocumentL()
    {
    // Create an Journey document, and return a pointer to it
    return (static_cast<CApaDocument*>
                    ( CJourneyDocument::NewL( *this ) ) );
    }

// -----------------------------------------------------------------------------
// CJourneyApplication::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CJourneyApplication::AppDllUid() const
    {
    // Return the UID for the Journey application
    return KUidJourneyApp;
    }

CDictionaryStore* CJourneyApplication::OpenIniFileLC(RFs& aFs) const
{
    return CEikApplication::OpenIniFileLC(aFs);
}
// End of File
