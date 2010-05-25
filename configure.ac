AC_INIT(opticommand, 2.1)

AM_INIT_AUTOMAKE([subdir-objects])

AC_CONFIG_MACRO_DIR([m4])
AC_PROG_LIBTOOL

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_ISC_POSIX
AC_STDC_HEADERS
AC_PROG_CXX
AC_HEADER_STDC

AC_PROG_INSTALL
AC_PROG_MAKE_SET

AC_CHECK_LIB([gcrypt],[gcry_check_version],[],[AC_MSG_ERROR(Could not find gcrypt)])
AC_CHECK_LIB([readline],[rl_redisplay],[],[AC_MSG_ERROR(Could not find readline)])

AC_CONFIG_HEADERS(config.h)
AM_MAINTAINER_MODE

PKG_CHECK_MODULES(PACKAGE_OPTICOMMAND, [
	liboptimization-2.0
])

AC_SUBST(PACKAGE_OPTICOMMAND_CFLAGS)
AC_SUBST(PACKAGE_OPTICOMMAND_LIBS)

if test "x$USE_MAINTAINER_MODE" = "xyes"; then
PACKAGE_OPTICOMMAND_CFLAGS="$PACKAGE_OPTICOMMAND_CFLAGS -Wall -Werror -Wuninitialized -Wmissing-declarations"
fi

AC_OUTPUT(
Makefile
src/Makefile
)
