INCLUDES = \
	-I$(srcdir)\
	-I$(builddir)\
	-I$(top_srcdir) \
	-I$(top_builddir) \
	$(PACKAGE_OPTICOMMAND_CFLAGS) \
	-DDATADIR=\""$(datadir)"\" \
	-DLIBDIR=\""$(libdir)"\" \
	-DCONFDIR=\""$(sysconfdir)"\" \
	-DPREFIXDIR=\""$(prefix)"\" \
	-DPKGDATADIR=\""$(pkgdatadir)"\" \
	-DPKGLIBDIR=\""$(pkglibdir)"\"

bin_PROGRAMS = opticommand

opticommand_CFLAGS = -g $(PACKAGE_OPTICOMMAND_CFLAGS)
opticommand_LDFLAGS = $(PACKAGE_OPTICOMMAND_LIBS)

opticommand_SOURCES = \
	opticommand.cc \
	application.cc \
	config.cc

noinst_HEADERS = \
	application.hh \
	config.hh

EXTRA_DIST = $(noinst_HEADERS)
