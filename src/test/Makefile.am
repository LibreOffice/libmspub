tests = importtest

check_PROGRAMS = $(tests)

importtest_CPPFLAGS = \
	-DTDOC=\"$(top_srcdir)/src/test/data\" \
	-I$(top_srcdir)/inc \
	$(REVENGE_CFLAGS) \
	$(REVENGE_STREAM_CFLAGS) \
	$(CPPUNIT_CFLAGS) \
	$(DEBUG_CXXFLAGS)

importtest_LDADD = \
	$(top_builddir)/src/lib/libmspub-@MSPUB_MAJOR_VERSION@.@MSPUB_MINOR_VERSION@.la \
	$(REVENGE_LIBS) \
	$(REVENGE_STREAM_LIBS) \
	$(CPPUNIT_LIBS)

importtest_SOURCES = \
	importtest.cpp \
	test.cpp

EXTRA_DIST = \
	data/fdo59355-1.pub

TESTS = $(tests)
