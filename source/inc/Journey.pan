/*
============================================================================
 Name        : Journey.pan
 Author      : Andreas Jakl
 Copyright   : 2007, Mopius
 Description : This file contains panic codes.
============================================================================
*/

#ifndef __JOURNEY_PAN__
#define __JOURNEY_PAN__

/** Journey application panic codes */
enum TJourneyPanics
    {
    EJourneyUi = 1
    // add further panics here
    };

inline void Panic(TJourneyPanics aReason)
    {
    _LIT(applicationName,"Journey");
    User::Panic(applicationName, aReason);
    }

#endif // __JOURNEY_PAN__
