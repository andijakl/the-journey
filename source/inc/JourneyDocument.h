/*
============================================================================
 Name        : JourneyDocument.h
 Author      : Andreas Jakl
 Copyright   : 2007, Mopius
 Description : Declares document class for application.
============================================================================
*/

#ifndef __JOURNEYDOCUMENT_h__
#define __JOURNEYDOCUMENT_h__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CJourneyAppUi;
class CEikApplication;


// CLASS DECLARATION

/**
* CJourneyDocument application class.
* An instance of class CJourneyDocument is the Document part of the
* AVKON application framework for the Journey example application.
*/
class CJourneyDocument : public CAknDocument
    {
    public: // Constructors and destructor

        /**
        * NewL.
        * Two-phased constructor.
        * Construct a CJourneyDocument for the AVKON application aApp
        * using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CJourneyDocument.
        */
        static CJourneyDocument* NewL( CEikApplication& aApp );

        /**
        * NewLC.
        * Two-phased constructor.
        * Construct a CJourneyDocument for the AVKON application aApp
        * using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CJourneyDocument.
        */
        static CJourneyDocument* NewLC( CEikApplication& aApp );

        /**
        * ~CJourneyDocument
        * Virtual Destructor.
        */
        virtual ~CJourneyDocument();

    public: // Functions from base classes

        /**
        * CreateAppUiL
        * From CEikDocument, CreateAppUiL.
        * Create a CJourneyAppUi object and return a pointer to it.
        * The object returned is owned by the Uikon framework.
        * @return Pointer to created instance of AppUi.
        */
        CEikAppUi* CreateAppUiL();

    private: // Constructors

        /**
        * ConstructL
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * CJourneyDocument.
        * C++ default constructor.
        * @param aApp Application creating this document.
        */
        CJourneyDocument( CEikApplication& aApp );

    };

#endif // __JOURNEYDOCUMENT_h__

// End of File
