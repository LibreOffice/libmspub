EXTERNAL_WARNINGS_NOT_ERRORS := TRUE

PRJ=..$/..$/..$/..$/..$/..

PRJNAME=libmspub
TARGET=mspublib
ENABLE_EXCEPTIONS=TRUE
LIBTARGET=NO

.INCLUDE :  settings.mk

.IF "$(GUI)$(COM)"=="WNTMSC"
CFLAGS+=-GR
.ENDIF
.IF "$(COM)"=="GCC"
CFLAGSCXX+=-frtti
.ENDIF

.IF "$(SYSTEM_LIBWPD)" == "YES"
INCPRE+=$(WPD_CFLAGS) -I..
.ELSE
INCPRE+=$(SOLARVER)$/$(INPATH)$/inc$/libwpd
.ENDIF

.IF "$(SYSTEM_LIBWPG)" == "YES"
INCPRE+=$(WPG_CFLAGS) -I..
.ELSE
INCPRE+=$(SOLARVER)$/$(INPATH)$/inc$/libwpg
.ENDIF

.IF "$(SYSTEM_ZLIB)" != "YES"
INCPRE+=-I$(SOLARVER)$/$(INPATH)$/inc$/external/zlib
.ENDIF

SLOFILES= \
    $(SLO)$/Fill.obj \
    $(SLO)$/MSPUBCollector.obj \
    $(SLO)$/MSPUBDocument.obj \
    $(SLO)$/MSPUBParser.obj \
    $(SLO)$/MSPUBStringVector.obj \
    $(SLO)$/MSPUBSVGGenerator.obj \
    $(SLO)$/libmspub_utils.obj

LIB1ARCHIV=$(LB)$/libmspublib.a
LIB1TARGET=$(SLB)$/$(TARGET).lib
LIB1OBJFILES= $(SLOFILES)

.INCLUDE :  target.mk
