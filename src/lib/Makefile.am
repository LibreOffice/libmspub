if PLATFORM_WIN32
version_info = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -avoid-version
else
version_info = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)
endif

lib_LTLIBRARIES = libmspub-@MSPUB_MAJOR_VERSION@.@MSPUB_MINOR_VERSION@.la
libmspub_@MSPUB_MAJOR_VERSION@_@MSPUB_MINOR_VERSION@_includedir = $(includedir)/libmspub-@MSPUB_MAJOR_VERSION@.@MSPUB_MINOR_VERSION@/libmspub
libmspub_@MSPUB_MAJOR_VERSION@_@MSPUB_MINOR_VERSION@_include_HEADERS = \
	libmspub.h \
	MSPUBDocument.h \
	MSPUBStringVector.h

AM_CXXFLAGS = $(LIBMSPUB_CXXFLAGS) $(DEBUG_CXXFLAGS)

libmspub_@MSPUB_MAJOR_VERSION@_@MSPUB_MINOR_VERSION@_la_LIBADD  = $(LIBMSPUB_LIBS) @LIBMSPUB_WIN32_RESOURCE@
libmspub_@MSPUB_MAJOR_VERSION@_@MSPUB_MINOR_VERSION@_la_DEPENDENCIES = @LIBMSPUB_WIN32_RESOURCE@
libmspub_@MSPUB_MAJOR_VERSION@_@MSPUB_MINOR_VERSION@_la_LDFLAGS = $(version_info) -export-dynamic -no_undefined
libmspub_@MSPUB_MAJOR_VERSION@_@MSPUB_MINOR_VERSION@_la_SOURCES = \
	MSPUBDocument.cpp \
	MSPUBParser.cpp \
	MSPUBStringVector.cpp \
	MSPUBSVGGenerator.cpp \
	libmspub_utils.cpp \
	MSPUBParser.h \
	MSPUBSVGGenerator.h \
	libmspub_utils.h 

if OS_WIN32

@LIBMSPUB_WIN32_RESOURCE@ : libmspub.rc $(libmspub_@MSPUB_MAJOR_VERSION@_@MSPUB_MINOR_VERSION@_la_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource libmspub.rc @LIBMSPUB_WIN32_RESOURCE@

endif


EXTRA_DIST = \
	libmspub.rc.in \
	makefile.mk

# These may be in the builddir too
BUILD_EXTRA_DIST = \
	libmspub.rc
