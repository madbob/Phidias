NULL =

INCLUDES = \
	-DPREFIX=\""$(prefix)"\"	\
	-DLIBDIR=\""$(libdir)"\"	\
	-DG_DISABLE_DEPRECATED		\
	$(PHIDIAS_CFLAGS)		\
	$(NULL)

LDADD = $(PHIDIAS_LT_LDFLAGS) -export-dynamic -rpath $(libdir)

marshal_source = \
	phidias-plugins-marshal.h      \
	phidias-plugins-marshal.c

BUILT_SOURCES = $(marshal_source)

sources_public_h =			\
	phidias-extra-column.h		\
	phidias-items-viewer.h		\
	phidias-channels-viewer.h

sources_c =				\
	$(BUILT_SOURCES)		\
	phidias-extra-column.c		\
	phidias-items-viewer.c		\
	phidias-channels-viewer.c

phidias-plugins-marshal.h: phidias-plugins-marshal.list
	$(GLIB_GENMARSHAL) $< --prefix=phidias_plugin_marshal --header > $@

phidias-plugins-marshal.c: phidias-plugins-marshal.list
	$(GLIB_GENMARSHAL) $< --prefix=phidias_plugin_marshal --body >> $@

lib_LTLIBRARIES = libphidiasplugins.la

libphidiasplugins_la_LIBADD = $(PHIDIAS_LIBS)
libphidiasplugins_la_SOURCES =	\
	$(sources_public_h)	\
	$(sources_c)
libphidiasplugins_la_LDFLAGS = $(LDADD)

libphidiaspluginsincludedir = $(includedir)/phidias
libphidiaspluginsinclude_DATA = $(sources_public_h)
