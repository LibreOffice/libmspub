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
	libmspub-@MSPUB_MAJOR_VERSION@.@MSPUB_MINOR_VERSION@.pc.in \
	libmspub-zip.in \
	libmspub.spec \
	libmspub.spec.in

distclean-local:
	rm -rf *.cache *~

rpm: dist
	rpmbuild -ta $(PACKAGE)-$(VERSION).tar.gz
	@rm -f $(PACKAGE)-$(VERSION).tar.gz

zip: all install
	sh libmspub-zip 

dist-hook:
	git log --date=short --pretty="format:@%cd  %an  <%ae>  [%H]%n%n%s%n%n%e%b" | sed -e "s|^\([^@]\)|\t\1|" -e "s|^@||" >$(distdir)/ChangeLog

