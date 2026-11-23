/*
============================================================================
 Name        : Journey.cpp
 Author      : Andreas Jakl
 Copyright   : 2007, Mopius
 Description : Main application class
============================================================================
*/

// INCLUDE FILES
#include <eikstart.h>
#include "JourneyApplication.h"


LOCAL_C CApaApplication* NewApplication()
	{
	return new CJourneyApplication;
	}

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication( NewApplication );
	}

