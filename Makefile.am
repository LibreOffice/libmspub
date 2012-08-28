ACLOCAL_AMFLAGS = -I m4

pkgconfdir = $(libdir)/pkgconfig

pkgconf_DATA = libmspub-@MSPUB_MAJOR_VERSION@.@MSPUB_MINOR_VERSION@.pc

SUBDIRS = src build docs

EXTRA_DIST = \
	AUTHORS \
	COPYING.GPL \
	COPYING.LGPL \
	COPYING.MPL \
	NEWS \
	README \
	autogen.sh \
	libmspub-@MSPUB_MAJOR_VERSION@.@MSPUB_MINOR_VERSION@.pc.in

distclean-local:
	rm -rf *.cache *~

dist-hook:
	git log --date=short --pretty="format:@%cd  %an  <%ae>  [%H]%n%n%s%n%n%e%b" | sed -e "s|^\([^@]\)|\t\1|" -e "s|^@||" >$(distdir)/ChangeLog

