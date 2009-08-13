INCLUDES = \
	-I$(top_srcdir)\
	-I$(top_builddir)\
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
opticommand_CFLAGS = D_GNU_SOURCE -g $(PACKAGE_OPTICOMMAND_CFLAGS)
opticommand_LDFLAGS = $(PACKAGE_OPTICOMMAND_LIBS)
opticommand_SOURCES = \
	opticommand.cc \
	Application/sendcommand.cc \
	Application/handlelistjobs.cc \
	Application/initcommands.cc \
	Application/statics.cc \
	Application/run.cc \
	Application/handleaddjob.cc \
	Application/printjob.cc \
	Application/application.cc \
	Application/parseuri.cc \
	Application/commandconstructor1.cc \
	Application/ondata.cc \
	Application/showjobinfo.cc \
	Application/commandconstructor2.cc \
	Application/interactivemode.cc \
	Application/prompt.cc \
	Application/handlehelp.cc \
	Application/parsecommand.cc \
	Application/oninterrupt.cc \
	Application/showlistjobs.cc \
	Application/handlejobinfo.cc \
	Application/handleremovejob.cc \
	Application/parsearguments.cc \
	Application/onclosed.cc \
	Application/directmode.cc \
	Application/handlechain.cc \
	Application/handlequit.cc \
	Config/statics.cc \
	Config/initialize.cc \
	Config/config.cc \
	Config/instance.cc