/*
============================================================================
 Name        : JourneyApplication.h
 Author      : Andreas Jakl
 Copyright   : 2007, Mopius
 Description : Declares main application class.
============================================================================
*/

#ifndef __JOURNEYAPPLICATION_H__
#define __JOURNEYAPPLICATION_H__

// INCLUDES
#include <aknapp.h>

// CLASS DECLARATION

/**
* CJourneyApplication application class.
* Provides factory to create concrete document object.
* An instance of CJourneyApplication is the application part of the
* AVKON application framework for the Journey example application.
*/
class CJourneyApplication : public CAknApplication
    {
    public: // Functions from base classes

        /**
        * From CApaApplication, AppDllUid.
        * @return Application's UID (KUidJourneyApp).
        */
        TUid AppDllUid() const;
        
        CDictionaryStore* OpenIniFileLC(RFs& aFs) const;

    protected: // Functions from base classes

        /**
        * From CApaApplication, CreateDocumentL.
        * Creates CJourneyDocument document object. The returned
        * pointer in not owned by the CJourneyApplication object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
    };

#endif // __JOURNEYAPPLICATION_H__

// End of File
