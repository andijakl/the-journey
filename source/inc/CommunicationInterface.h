#ifndef __COMMUNICATIONINTERFACE_H__
#define __COMMUNICATIONINTERFACE_H__

#include <eikenv.h>

//------------------------------------------------------------------------------
// Interface for passing commands to the AppUi
class MCommInterface
{
	public:
		virtual TBool ReadIniFile(TUid iInfo, void *ptr, TUint &size)=0;
		virtual void WriteIniFile(TUid iInfo, void *ptr, TUint size)=0;
};


#endif

