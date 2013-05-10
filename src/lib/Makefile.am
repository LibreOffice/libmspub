if PLATFORM_WIN32
version_info = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -avoid-version
else
version_info = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)
endif

lib_LTLIBRARIES = libmspub-@MSPUB_MAJOR_VERSION@.@MSPUB_MINOR_VERSION@.la
libmspub_@MSPUB_MAJOR_VERSION@_@MSPUB_MINOR_VERSION@_includedir = $(includedir)/libmspub-@MSPUB_MAJOR_VERSION@.@MSPUB_MINOR_VERSION@/libmspub
libmspub_@MSPUB_MAJOR_VERSION@_@MSPUB_MINOR_VERSION@_include_HEADERS = \
	$(top_srcdir)/inc/libmspub/libmspub.h \
	$(top_srcdir)/inc/libmspub/MSPUBDocument.h \
	$(top_srcdir)/inc/libmspub/MSPUBStringVector.h

AM_CXXFLAGS = -I$(top_srcdir)/inc $(WPD_CFLAGS) $(WPG_CFLAGS) $(ZLIB_CFLAGS) $(ICU_CFLAGS) $(DEBUG_CXXFLAGS)

libmspub_@MSPUB_MAJOR_VERSION@_@MSPUB_MINOR_VERSION@_la_LIBADD  = $(WPD_LIBS) $(ZLIB_LIBS) $(ICU_LIBS) @LIBMSPUB_WIN32_RESOURCE@
libmspub_@MSPUB_MAJOR_VERSION@_@MSPUB_MINOR_VERSION@_la_DEPENDENCIES = @LIBMSPUB_WIN32_RESOURCE@
libmspub_@MSPUB_MAJOR_VERSION@_@MSPUB_MINOR_VERSION@_la_LDFLAGS = $(version_info) -export-dynamic -no-undefined
libmspub_@MSPUB_MAJOR_VERSION@_@MSPUB_MINOR_VERSION@_la_SOURCES = \
	MSPUBCollector.cpp \
	MSPUBDocument.cpp \
	MSPUBParser.cpp \
	MSPUBParser2k.cpp \
	MSPUBStringVector.cpp \
	MSPUBSVGGenerator.cpp \
	Fill.cpp \
	libmspub_utils.cpp \
	PolygonUtils.cpp \
	ShapeGroupElement.cpp \
	ColorReference.cpp \
	VectorTransformation2D.cpp \
	MSPUBParser97.cpp \
	Dash.cpp \
	Shadow.cpp \
	EscherContainerType.h \
	EscherFieldIds.h \
	FillType.h \
	MSPUBBlockID.h \
	MSPUBBlockType.h \
	MSPUBCollector.h \
	MSPUBConstants.h \
	MSPUBContentChunkType.h \
	MSPUBParser.h \
	MSPUBParser2k.h \
	MSPUBSVGGenerator.h \
	MSPUBTypes.h \
	libmspub_utils.h \
	ShapeFlags.h \
	ShapeType.h \
	Fill.h \
	ColorReference.h \
	PolygonUtils.h \
	Shapes.h \
	VectorTransformation2D.h \
	Coordinate.h \
	ShapeGroupElement.h \
	ShapeInfo.h \
	Line.h \
	Margins.h \
	MSPUBParser97.h \
	BorderArtInfo.h \
	NumberingType.h \
	NumberingDelimiter.h \
	ListInfo.h \
	Dash.h \
	TableInfo.h \
	Arrow.h \
	VerticalAlign.h \
	EmbeddedFontInfo.h \
	Shadow.h

if OS_WIN32

@LIBMSPUB_WIN32_RESOURCE@ : libmspub.rc $(libmspub_@MSPUB_MAJOR_VERSION@_@MSPUB_MINOR_VERSION@_la_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource libmspub.rc @LIBMSPUB_WIN32_RESOURCE@

endif


EXTRA_DIST = \
	libmspub.rc.in

# These may be in the builddir too
BUILD_EXTRA_DIST = \
	libmspub.rc
