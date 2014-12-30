if PLATFORM_WIN32
version_info = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -avoid-version
else
version_info = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)
endif

lib_LTLIBRARIES = libmspub-@MSPUB_MAJOR_VERSION@.@MSPUB_MINOR_VERSION@.la
libmspub_@MSPUB_MAJOR_VERSION@_@MSPUB_MINOR_VERSION@_includedir = $(includedir)/libmspub-@MSPUB_MAJOR_VERSION@.@MSPUB_MINOR_VERSION@/libmspub
libmspub_@MSPUB_MAJOR_VERSION@_@MSPUB_MINOR_VERSION@_include_HEADERS = \
	$(top_srcdir)/inc/libmspub/libmspub.h \
	$(top_srcdir)/inc/libmspub/MSPUBDocument.h

AM_CXXFLAGS = -I$(top_srcdir)/inc $(REVENGE_CFLAGS) $(ZLIB_CFLAGS) $(ICU_CFLAGS) $(DEBUG_CXXFLAGS) -DLIBMSPUB_BUILD=1

libmspub_@MSPUB_MAJOR_VERSION@_@MSPUB_MINOR_VERSION@_la_LIBADD  = $(REVENGE_LIBS) $(ZLIB_LIBS) $(ICU_LIBS) @LIBMSPUB_WIN32_RESOURCE@
libmspub_@MSPUB_MAJOR_VERSION@_@MSPUB_MINOR_VERSION@_la_DEPENDENCIES = @LIBMSPUB_WIN32_RESOURCE@
libmspub_@MSPUB_MAJOR_VERSION@_@MSPUB_MINOR_VERSION@_la_LDFLAGS = $(version_info) -export-dynamic -no-undefined
libmspub_@MSPUB_MAJOR_VERSION@_@MSPUB_MINOR_VERSION@_la_SOURCES = \
	Arrow.h \
	BorderArtInfo.h \
	ColorReference.cpp \
	ColorReference.h \
	Coordinate.h \
	Dash.cpp \
	Dash.h \
	EmbeddedFontInfo.h \
	EscherContainerType.h \
	EscherFieldIds.h \
	Fill.cpp \
	Fill.h \
	FillType.h \
	Line.h \
	ListInfo.h \
	MSPUBBlockID.h \
	MSPUBBlockType.h \
	MSPUBCollector.cpp \
	MSPUBCollector.h \
	MSPUBConstants.h \
	MSPUBContentChunkType.h \
	MSPUBDocument.cpp \
	MSPUBMetaData.cpp \
	MSPUBMetaData.h \
	MSPUBParser.cpp \
	MSPUBParser.h \
	MSPUBParser2k.cpp \
	MSPUBParser2k.h \
	MSPUBParser97.cpp \
	MSPUBParser97.h \
	MSPUBTypes.h \
	Margins.h \
	NumberingDelimiter.h \
	NumberingType.h \
	PolygonUtils.cpp \
	PolygonUtils.h \
	Shadow.cpp \
	Shadow.h \
	ShapeFlags.h \
	ShapeGroupElement.cpp \
	ShapeGroupElement.h \
	ShapeInfo.h \
	ShapeType.h \
	Shapes.h \
	TableInfo.h \
	VectorTransformation2D.cpp \
	VectorTransformation2D.h \
	VerticalAlign.h \
	libmspub_utils.cpp \
	libmspub_utils.h

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
