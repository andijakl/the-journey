/*
============================================================================
 Name        : CSysInfo.h
 Author      : Andreas Jakl
 Version     : 1.20
 Copyright   : Copyright 2006-2007, Mopius
 Description : CSysInfo.h - header file
============================================================================
*/

#ifndef SYSINFO_H
#define SYSINFO_H

// CLASS DECLARATION
#include <e32base.h>
#include <Etel3rdParty.h>

#define KErrWrongNet -40000
#define KErrCellId0 -40001

class MLocationCallback
{
public:
    virtual void GotLocation(TInt aErrCode, TUint aCellId) = 0;
};


class CSysInfo : public CActive
{
public:
    static CSysInfo* NewL(MLocationCallback* aObserver);
    ~CSysInfo();

private:
    CSysInfo(MLocationCallback* aObserver);
    void ConstructL();
    
    /**
     * Do cleanup and close the telephony server.
     */
    void CloseTelServer();

public:
	/**
	 * Issue a request to get the network info.
	 * If a request is already active, that one will be cancelled
	 * and the new request is sent.
	 */
    void StartGetNetworkInfo();
    
    /**
     * Returns the latest (cached) cell id that was retrieved.
     * @return The latest cell id, or 0 if no cell id has been
     * retrieved yet.
     */
    TUint GetLatestCellId();

private:
    /**
     * Method from CActive, callback when the request to get the
     * location info is finished.
     * Parses the information and sends a callback to the location
     * observer (MLocationCallback) that is registered for this
     * class.
     */
    void RunL();
    
    /**
     * Cancels retrieving the network information
     */
    void DoCancel();

private:
	/**
	 * Cache for the latest cell id information
	 */
    TInt iCellId;
    
    /**
     * Stores whether the telephony server has already been opened.
     */
    TBool iTelServerOpen;
    
    /**
     * Pointer for the telephony-object.
     */
    CTelephony* iTelephony;
    
    /**
     * Contains all kinds of network information. We only need the cell id.
     */
    CTelephony::TNetworkInfoV1 iNetworkInfoV1;
    
    /**
     * SDK-Doc: A typedef'd packaged CTelephony::TNetworkInfoV1 
     * for passing through a generic API method.
     */
    CTelephony::TNetworkInfoV1Pckg iNetworkInfoV1Pckg;
    
    /**
     * Pointer to the observer which will get callbacks when a
     * location request completes.
     */
    MLocationCallback* iObserver;
};

#endif // SYSINFO_H

// End of File
