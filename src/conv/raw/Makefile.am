bin_PROGRAMS = pub2raw

AM_CXXFLAGS = -I$(top_srcdir)/inc \
	$(REVENGE_GENERATORS_CFLAGS) \
	$(REVENGE_CFLAGS) \
	$(REVENGE_STREAM_LIBS) \
	$(DEBUG_CXXFLAGS)

pub2raw_DEPENDENCIES = @PUB2RAW_WIN32_RESOURCE@

pub2raw_LDADD = \
	$(top_builddir)/src/lib/libmspub-@MSPUB_MAJOR_VERSION@.@MSPUB_MINOR_VERSION@.la \
	$(REVENGE_GENERATORS_LIBS) \
	$(REVENGE_LIBS) \
	$(REVENGE_STREAM_LIBS) \
	@PUB2RAW_WIN32_RESOURCE@ 

pub2raw_SOURCES = \
	pub2raw.cpp

if OS_WIN32

@PUB2RAW_WIN32_RESOURCE@ : pub2raw.rc $(pub2raw_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource pub2raw.rc @PUB2RAW_WIN32_RESOURCE@
endif

EXTRA_DIST = \
	pub2raw.rc.in

# These may be in the builddir too
BUILD_EXTRA_DIST = \
	pub2raw.rc	 
