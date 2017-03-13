bin_PROGRAMS = pubfuzz

AM_CXXFLAGS = -I$(top_srcdir)/inc \
	$(REVENGE_GENERATORS_CFLAGS) \
	$(REVENGE_CFLAGS) \
	$(REVENGE_STREAM_LIBS) \
	$(DEBUG_CXXFLAGS)

pubfuzz_LDADD = \
	$(top_builddir)/src/lib/libmspub-@MSPUB_MAJOR_VERSION@.@MSPUB_MINOR_VERSION@.la \
	$(REVENGE_GENERATORS_LIBS) \
	$(REVENGE_LIBS) \
	$(REVENGE_STREAM_LIBS) \
	-lFuzzingEngine

pubfuzz_SOURCES = \
	pubfuzz.cpp
