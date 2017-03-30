noinst_PROGRAMS = pubfuzzer

AM_CXXFLAGS = -I$(top_srcdir)/inc \
	$(REVENGE_GENERATORS_CFLAGS) \
	$(REVENGE_CFLAGS) \
	$(REVENGE_STREAM_CFLAGS) \
	$(DEBUG_CXXFLAGS)

pubfuzzer_LDADD = \
	$(top_builddir)/src/lib/libmspub-@MSPUB_MAJOR_VERSION@.@MSPUB_MINOR_VERSION@.la \
	$(REVENGE_GENERATORS_LIBS) \
	$(REVENGE_LIBS) \
	$(REVENGE_STREAM_LIBS) \
	-lFuzzingEngine

pubfuzzer_SOURCES = \
	pubfuzzer.cpp
