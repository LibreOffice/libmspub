bin_PROGRAMS = pub2xhtml

AM_CXXFLAGS = -I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib  $(WPG_CFLAGS) $(WPD_CFLAGS) $(DEBUG_CXXFLAGS)

pub2xhtml_DEPENDENCIES = @PUB2XHTML_WIN32_RESOURCE@

if STATIC_TOOLS
pub2xhtml_LDADD = ../../lib/@MSPUB_OBJDIR@/libmspub-@MSPUB_MAJOR_VERSION@.@MSPUB_MINOR_VERSION@.a $(WPD_LIBS) @PUB2XHTML_WIN32_RESOURCE@
pub2xhtml_LDFLAGS = -all-static 
else
pub2xhtml_LDADD = ../../lib/libmspub-@MSPUB_MAJOR_VERSION@.@MSPUB_MINOR_VERSION@.la $(WPD_LIBS) @PUB2XHTML_WIN32_RESOURCE@ 
endif

pub2xhtml_SOURCES = \
	pub2xhtml.cpp

if OS_WIN32

@PUB2XHTML_WIN32_RESOURCE@ : pub2xhtml.rc $(pub2xhtml_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource pub2xhtml.rc @PUB2XHTML_WIN32_RESOURCE@
endif

EXTRA_DIST = \
	$(pub2xhtml_SOURCES) \
	pub2xhtml.rc.in

# These may be in the builddir too
BUILD_EXTRA_DIST = \
	pub2xhtml.rc
