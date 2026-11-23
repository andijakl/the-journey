/*
============================================================================
 Name        : JourneyAppUi.h
 Author      : Andreas Jakl
 Copyright   : 2007, Mopius
 Description : Declares UI class for application.
============================================================================
*/

#ifndef __JOURNEYAPPUI_h__
#define __JOURNEYAPPUI_h__

// INCLUDES
#include <aknviewappui.h>
#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <Journey.rsg>
#include <f32file.h>
#include <s32file.h>
#include <eikapp.h>

#include "Journey.pan"
#include "Journey.hrh"

#include "SharedSettings.h"

#include "JourneyVMenu.h"
#include "JourneyVGame.h"

// FORWARD DECLARATIONS
class CJourneyCMenu;
class CJourneyCGame;


// CLASS DECLARATION
/**
* CJourneyAppUi application UI class.
* Interacts with the user through the UI and request message processing
* from the handler class
*/
class CJourneyAppUi : public CAknViewAppUi, public MCommInterface
{
public: // Constructors and destructor

	/**
	* ConstructL.
	* 2nd phase constructor.
	*/
	void ConstructL();

	/**
	* CJourneyAppUi.
	* C++ default constructor. This needs to be public due to
	* the way the framework constructs the AppUi
	*/
	CJourneyAppUi();

	/**
	* ~CJourneyAppUi.
	* Virtual Destructor.
	*/
	virtual ~CJourneyAppUi();

private:  // Functions from base classes

	/**
	* From CEikAppUi, HandleCommandL.
	* Takes care of command handling.
	* @param aCommand Command to be handled.
	*/
	void HandleCommandL( TInt aCommand );

	/**
	* HandleStatusPaneSizeChange.
	* Called by the framework when the application status pane
	* size is changed.
	*/
	void HandleStatusPaneSizeChange();

public:
	/**
	 * Read data from the application's ini file.
	 * @param aInfo stream that should be read
	 * @param aPtr structure that will be filled with the information
	 * @param aSize size of the structure to read
	 * @return ETrue if the stream with the specified id was present,
	 * EFalse if it was not.
	 */
	virtual TBool ReadIniFile(TUid aInfo, void *aPtr, TUint &aSize);
	
	/**
	 * Does the actual file operations for ReadIniFile().
	 * Put into an own function to allow centralized leave-handling,
	 * instead of trapping each and every L-function.
	 * ReadIniFile() and WriteIniFile() are predefined filenames and
	 * can not have other names with a trailing L.
	 * @param aInfo stream that should be read
	 * @param aPtr structure that will be filled with the information
	 * @param aSize size of the structure to read
	 * @return ETrue if the stream with the specified id was present,
	 * EFalse if it was not.
	 */
	TBool DoReadIniFileL(TUid aInfo, void *aPtr, TUint &aSize);
	
	/**
	 * Write data to the application's ini file.
	 * @param aInfo stream that should be written to
	 * @param aPtr pointer to a structure containing the information
	 * to be written.
	 * @param aSize size of the data to be written.
	 */
	virtual void WriteIniFile(TUid aInfo, void *aPtr, TUint aSize);
	
	/**
	 * Does the actual file operations for WriteIniFile().
	 * Put into an own function to allow centralized leave-handling,
	 * instead of trapping each and every L-function.
	 * ReadIniFile() and WriteIniFile() are predefined filenames and
	 * can not have other names with a trailing L.
	 * @param aInfo stream that should be written to
	 * @param aPtr pointer to a structure containing the information
	 * to be written.
	 * @param aSize size of the data to be written.
	 */
	void DoWriteIniFileL(TUid aInfo, void *aPtr, TUint aSize);

private:
	/**
	 * Start the asynchronous timer to initiate the view switch after a very
	 * short delay.
	 * @param aTargetViewId id of the view to switch to after the delay.
	 * @param aParameter additional parameters to send to the view.
	 */
	void PrepareViewSwitch(TUid aTargetViewId, TUid aParameter);

	/**
	* DoPeriodTask.
	* Called by period task static function.
	*/
	void DoPeriodTask();

	/**
	* Period.
	* Call back function for a periodic timer
	* @param aPtr a parameter passed to the timer when the timer is started.
	* @return Value TRUE indicates the callback should be done again.
	*/
	static TInt Period( TAny* aPtr );

public: // Data
	/** Contains settings and callback pointers for communication interfaces. */
	TJourneySharedData* iSharedData;
	
	/**
	 * Timer for starting a delayed view switch, giving time to the
	 * menu view to display the loading text. */
	CPeriodic* iPeriodicTimer;
	
	/** Cache target view properties for the delayed view switch. */
	TUid iTargetViewId;
	
	/** Cache target view properties for the delayed view switch. */
	TUid iTargetViewParam;
};

#endif // __JOURNEYAPPUI_h__

// End of File
