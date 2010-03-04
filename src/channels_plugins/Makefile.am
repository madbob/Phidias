plugindir = @plugindir@/channels

NULL =
LIBS =

INCLUDES = \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/lib \
	-DPREFIX=\""$(prefix)"\" \
	-DLIBDIR=\""$(libdir)"\" \
	-DG_DISABLE_DEPRECATED \
	$(PHIDIAS_CFLAGS)

plugin_LTLIBRARIES = phidias-channels-by-name.la

phidias_channels_by_name_la_LDFLAGS = -module -avoid-version -shared
phidias_channels_by_name_la_LIBADD = $(PHIDIAS_LIBS)
phidias_channels_by_name_la_SOURCES =	\
	phidias-channels-by-name.c
