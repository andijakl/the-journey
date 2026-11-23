/*
* ============================================================================
*  Name     : CJourneyStory from JourneyStory.cpp
*  Part of  : Journey
*  Created  : 04.05.2004 by Andreas Jakl
*  Version  : 1.20
*  Copyright: 
*     'The Journey' is free software; you can redistribute
*     it and/or modify it under the terms of the GNU General Public License
*     as published by the Free Software Foundation; either version 2 of
*     the License, or (at your option) any later version.
* 
*     'The Journey' is distributed in the hope that it
*     will be useful, but WITHOUT ANY WARRANTY; without even the implied
*     warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*     See the GNU General Public License for more details.
* 
*     You should have received a copy of the GNU General Public License
*     along with 'The Journey'; if not, write to the Free Software
*     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
* ============================================================================
*/

#include "JourneyStory.h"


// Constructor
//------------------------------------------------------------------------------
CJourneyStory::CJourneyStory()
{
}

// Destructor
//------------------------------------------------------------------------------
CJourneyStory::~CJourneyStory()
{
}

//------------------------------------------------------------------------------
void CJourneyStory::InitStory()
{
    TInt i;
    for (i=0; i < MAX_NUM_TALKINFOS; i++)
    {
        iTalkInfos[i].iAreaNum = -1;
        iTalkInfos[i].iTextRes = -1;
        iTalkInfos[i].iBitmapEnum = -1;
        for (TInt j=0; j < MAX_NUM_AREAS; j++)
            iTalkInfos[i].iWrongAreaTextRes[j] = R_WRONGAREA_NOTPOSSIBLE;
        iTalkInfos[i].iNoAssignedAreaTextRes = R_WRONGAREA_NOTPOSSIBLE;
    }

    iNoNetTextRes = R_NONET;
    iContinueTextRes = R_CONTINUE;
    iNoAreaInfo.iAreaNameRes = R_AREANAME_NO;
    iNoAreaInfo.iBitmapEnum = EMbmJourneyAreastreets;
    iNoAreaInfo.iHowFar = 0;
    iAreaInfos[0].iAreaNameRes = R_AREANAME_00;
    iAreaInfos[0].iBitmapEnum = EMbmJourneyAreabureau;
    iAreaInfos[0].iHowFar = 0;
    iAreaInfos[1].iAreaNameRes = R_AREANAME_01;
    iAreaInfos[1].iBitmapEnum = EMbmJourneyAreabar;
    iAreaInfos[1].iHowFar = 2;
    iAreaInfos[2].iAreaNameRes = R_AREANAME_02;
    iAreaInfos[2].iBitmapEnum = EMbmJourneyAreafactory;
    iAreaInfos[2].iHowFar = 2;

    // First Area - Bureau
    for (i=0; i<=3; i++)
    {
        iTalkInfos[i].iAreaNum = 0;
    }
    iTalkInfos[0].iTextRes = R_STORY_000;
    iTalkInfos[0].iWrongAreaTextRes[0] = R_LOADING;
    iTalkInfos[0].iNoAssignedAreaTextRes = R_LOADING;
    iTalkInfos[0].iBitmapEnum = EMbmJourneyAreabureau;

    for (i=1; i<3; i++)
    {
        iTalkInfos[i].iWrongAreaTextRes[0] = R_WRONGAREA_011_NO;
        iTalkInfos[i].iNoAssignedAreaTextRes = R_WRONGAREA_011_NO;
    }
    iTalkInfos[1].iTextRes = R_STORY_001;
    iTalkInfos[1].iBitmapEnum = EMbmJourneyStorybureauguy;
    iTalkInfos[2].iTextRes = R_STORY_002;
    iTalkInfos[2].iBitmapEnum = EMbmJourneyStorybureauphoto;
    iTalkInfos[3].iTextRes = R_STORY_003;
    iTalkInfos[3].iBitmapEnum = EMbmJourneyStorybureauguy;

    // Second Area - Bar
    for (i=4; i<=7; i++)
    {
        iTalkInfos[i].iAreaNum = 1;
        iTalkInfos[i].iWrongAreaTextRes[0] = R_WRONGAREA_005_NO;
        iTalkInfos[i].iNoAssignedAreaTextRes = R_WRONGAREA_005_NO;
    }

    iTalkInfos[4].iTextRes = R_STORY_004;
    iTalkInfos[4].iWrongAreaTextRes[0] = R_WRONGAREA_004_00;
    iTalkInfos[4].iNoAssignedAreaTextRes = R_WRONGAREA_004_NO;
    iTalkInfos[4].iBitmapEnum = EMbmJourneyAreabar;
    
    iTalkInfos[5].iTextRes = R_STORY_005;
    iTalkInfos[5].iBitmapEnum = EMbmJourneyStorybarmaid;
    iTalkInfos[6].iTextRes = R_STORY_006;
    iTalkInfos[6].iBitmapEnum = EMbmJourneyStorybarguy;
    iTalkInfos[7].iTextRes = R_STORY_007;
    iTalkInfos[7].iBitmapEnum = EMbmJourneyStorybarguy;

    // Third Area - Factory
    for (i=8; i<=9; i++)
    {
        iTalkInfos[i].iAreaNum = 2;
    }
    iTalkInfos[8].iTextRes = R_STORY_008;
    iTalkInfos[8].iWrongAreaTextRes[0] = R_WRONGAREA_008_00;
    iTalkInfos[8].iWrongAreaTextRes[1] = R_WRONGAREA_008_01;
    iTalkInfos[8].iNoAssignedAreaTextRes = R_WRONGAREA_008_NO;
    iTalkInfos[8].iBitmapEnum = EMbmJourneyAreafactory;

    iTalkInfos[9].iTextRes = R_STORY_009;
    iTalkInfos[9].iWrongAreaTextRes[0] = R_WRONGAREA_009_NO;
    iTalkInfos[9].iWrongAreaTextRes[1] = R_WRONGAREA_009_NO;
    iTalkInfos[9].iNoAssignedAreaTextRes = R_WRONGAREA_009_NO;
    iTalkInfos[9].iBitmapEnum = EMbmJourneyStoryfactoryguys;

    // First Area - Bureau
    for (i=10; i<=14; i++)
    {
        iTalkInfos[i].iAreaNum = 0;
        iTalkInfos[i].iWrongAreaTextRes[1] = R_WRONGAREA_011_NO;
        iTalkInfos[i].iWrongAreaTextRes[2] = R_WRONGAREA_011_NO;
        iTalkInfos[i].iNoAssignedAreaTextRes = R_WRONGAREA_011_NO;
    }
    iTalkInfos[10].iTextRes = R_STORY_010;
    iTalkInfos[10].iWrongAreaTextRes[1] = R_WRONGAREA_010_01;
    iTalkInfos[10].iWrongAreaTextRes[2] = R_WRONGAREA_010_02;
    iTalkInfos[10].iNoAssignedAreaTextRes = R_WRONGAREA_010_NO;
    iTalkInfos[10].iBitmapEnum = EMbmJourneyStorybureauguy;

    iTalkInfos[11].iTextRes = R_STORY_011;
    iTalkInfos[11].iBitmapEnum = EMbmJourneyStorybureausurprise;
    iTalkInfos[12].iTextRes = R_STORY_012;
    iTalkInfos[12].iBitmapEnum = EMbmJourneyStorybureaugirl;
    iTalkInfos[13].iTextRes = R_STORY_013;
    iTalkInfos[13].iBitmapEnum = EMbmJourneyStorybureausun;
    iTalkInfos[14].iTextRes = R_STORY_014;
    iTalkInfos[14].iBitmapEnum = EMbmJourneyStorytheend;

}
