SUBDIRS = dcraw icons src 

libkdcrawpkgconfigdir = $(libdir)/pkgconfig
libkdcrawpkgconfig_DATA = libkdcraw.pc
EXTRA_DIST = libkdcraw.pc.in

# i18n translation messages
messages: 
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
		$(XGETTEXT) $$LIST -o $(podir)/libkdcraw.pot; \
	fi