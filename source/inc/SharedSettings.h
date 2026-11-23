#ifndef SHAREDSETTINGS_H_
#define SHAREDSETTINGS_H_

#include "CommunicationInterface.h"

#define KTextColor          TRgb(255, 221, 45)
#define KTextColorDark      TRgb(206, 179, 38)
/** Background color for softkey labels. */
#define KBgColor            TRgb(74, 56, 28)

_LIT(KFileStore, "Progress.dat");

#define INI_FILE_VERSION_NUMBER					1
#define PROGRESS_FILE_VERSION_NUMBER			2
const TUid uidVersion      = { 1 };
const TUid uidSettings     = { 2 };

//------------------------------------------------------------------------------
class TJourneySettings
{
public:
	TBool iAcceptedPrivacy;
};

//------------------------------------------------------------------------------
class TJourneySharedData
{
public:
	TJourneySettings iSettings;
	MCommInterface* iCommInterface;
};

#endif /*SHAREDSETTINGS_H_*/
