# ============================================================================
#  Name     : Icons_scalable_dc.mk
#  Description : This is file for creating .mif file (scalable icons)
#  Version     : 
# ==============================================================================

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=$(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\Z
else
ZDIR=$(EPOCROOT)epoc32\data\z
endif

TARGETDIR=$(ZDIR)\resource\apps
HEADERDIR=$(EPOCROOT)epoc32\include

ICONTARGETFILENAMES=$(TARGETDIR)\Journey.mif
ICONHEADERFILENAMES=$(HEADERDIR)\Journey.mbg

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : 
	erase $(ICONHEADERFILENAMES) $(ICONTARGETFILENAMES) 2>>nul

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE :
	mifconv $(ICONTARGETFILENAMES)  \
		 /H$(ICONHEADERFILENAMES)  \
		 /c32 ..\gfx\hat.svg  \
		 /c32 ..\gfx\areaBureau.svg  \
		 /c32 ..\gfx\areaStreets.svg  \
		 /c32 ..\gfx\storyTheEnd.svg  \
		 /c32 ..\gfx\storyBureauSurprise.svg  \
		 /c32 ..\gfx\storyBureauPhoto.svg  \
		 /c32 ..\gfx\storyBarMaid.svg  \
		 /c32 ..\gfx\storyBureauGirl.svg  \
		 /c32 ..\gfx\interfaceBackground.svg  \
		 /c32 ..\gfx\storyBarGuy.svg  \
		 /c32 ..\gfx\areaFactory.svg  \
		 /c32 ..\gfx\areaBar.svg  \
		 /c32 ..\gfx\storyFactoryGuys.svg  \
		 /c32 ..\gfx\journeyTitleBg.svg  \
		 /c32 ..\gfx\storyBureauSun.svg  \
		 /c32 ..\gfx\storyBureauGuy.svg \
		 /c32,8 ..\gfx\journeyTitleText.svg


FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(ICONTARGETFILENAMES) 

FINAL : do_nothing

