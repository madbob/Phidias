m4_define([phidias_major_version], [0])
m4_define([phidias_minor_version], [1])
m4_define([phidias_micro_version], [0])

m4_define([phidias_version],
          [phidias_major_version.phidias_minor_version.phidias_micro_version])

m4_define([phidias_interface_age], [0])
m4_define([phidias_binary_age],
          [m4_eval(100 * phidias_minor_version + phidias_micro_version)])

m4_define([lt_current],
          [m4_eval(100 * phidias_minor_version + phidias_micro_version - phidias_interface_age)])
m4_define([lt_revision], [phidias_interface_age])
m4_define([lt_age],
          [m4_eval(phidias_binary_age - phidias_interface_age)])

m4_define([dbus_req_version], [0.82])
m4_define([trackerclient_req_version], [0.8.1])
m4_define([webkit_req_version], [1.1.15.4])
m4_define([champlain_req_version], [0.4.2])
m4_define([cluttergtk_req_version], [0.10.2])

AC_PREREQ([2.59])
AC_INIT([phidias], [phidias_version], [], [phidias])
AM_INIT_AUTOMAKE([1.10])

AM_CONFIG_HEADER(config.h)

GNOME_COMMON_INIT
GNOME_COMPILE_WARNINGS
GNOME_DEBUG_CHECK

IT_PROG_INTLTOOL([0.35.0])

AM_DISABLE_STATIC
AM_PATH_GLIB_2_0
AM_PROG_CC_C_O
AM_PROG_LIBTOOL

AC_HEADER_STDC
AC_C_CONST

PHIDIAS_MAJOR_VERSION=phidias_major_version
PHIDIAS_MINOR_VERSION=phidias_minor_version
PHIDIAS_MICRO_VERSION=phidias_micro_version
PHIDIAS_VERSION=phidias_version
AC_SUBST(PHIDIAS_MAJOR_VERSION)
AC_SUBST(PHIDIAS_MICRO_VERSION)
AC_SUBST(PHIDIAS_MINOR_VERSION)
AC_SUBST(PHIDIAS_VERSION)

dnl phidias checks
PKG_CHECK_MODULES(PHIDIAS,
		dbus-glib-1 >= dbus_req_version
		tracker-client-0.8 >= trackerclient_req_version
		webkit-1.0 >= webkit_req_version
		champlain-gtk-0.4 >= champlain_req_version
		clutter-gtk-0.10 >= cluttergtk_req_version)
AC_SUBST(PHIDIAS_CFLAGS)
AC_SUBST(PHIDIAS_LIBS)

dnl = Enable debug level ===================================================

m4_define([debug_default],
          m4_if(m4_eval(phidias_minor_version % 2), [1], [yes], [minimum]))

AC_ARG_ENABLE(debug,
              AC_HELP_STRING([--enable-debug=@<:@no/minimum/yes@:>@],
                             [turn on debugging @<:@default=debug_default@:>@]),
,
              enable_debug=debug_default)

if test "x$enable_debug" = "xyes"; then
  test "$cflags_set" = set || CFLAGS="$CFLAGS -g"
  PHIDIAS_DEBUG_CFLAGS="-DPHIDIAS_ENABLE_DEBUG"
else
  if test "x$enable_debug" = "xno"; then
    PHIDIAS_DEBUG_CFLAGS="-DG_DISABLE_ASSERT -DG_DISABLE_CHECKS -DG_DISABLE_CAST_CHECKS"
  else # minimum
    PHIDIAS_DEBUG_CFLAGS="-DPHIDIAS_ENABLE_DEBUG -DG_DISABLE_CAST_CHECKS"
  fi
fi

AC_SUBST(PHIDIAS_DEBUG_CFLAGS)

dnl = Enable strict compiler flags =========================================

# use strict compiler flags only on development releases
#m4_define([maintainer_flags_default],
#          m4_if(m4_eval(phidias_minor_version % 2), [1], [yes], [no]))
m4_define([maintainer_flags_default], [no])
AC_ARG_ENABLE([maintainer-flags],
              AC_HELP_STRING([--enable-maintainer-flags=@<:@no/yes@:>@],
                             [Use strict compiler flags @<:@default=maintainer_flags_default@:>@]),,
              enable_maintainer_flags=maintainer_flags_default)

if test "x$enable_maintainer_flags" = "xyes"; then
  CPPFLAGS="$CPPFLAGS -g -Wall -Wshadow -Wcast-align -Wno-uninitialized -Werror"
else
  CPPFLAGS="$CPPFLAGS -g -Wall"
fi

plugindir='${libdir}/phidias/plugins/'
AC_SUBST(plugindir)
CFLAGS="$CFLAGS '-DPLUGIN_DIR=\"\$(plugindir)\"'"

dnl Internationalization
GETTEXT_PACKAGE=phidias
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], "$GETTEXT_PACKAGE", [Define the gettext package to use])
AC_SUBST(GETTEXT_PACKAGE)
AM_GLIB_GNU_GETTEXT

AC_CONFIG_FILES([
	Makefile
	lib/Makefile
	src/Makefile
	src/items_plugins/Makefile
	src/channels_plugins/Makefile
	po/Makefile.in
])

AC_OUTPUT

echo ""
echo "                phidias: $VERSION"
echo ""
echo "                 Prefix: ${prefix}"
echo "         Compiler flags: ${CPPFLAGS}"
echo ""
