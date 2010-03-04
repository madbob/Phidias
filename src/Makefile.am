NULL =

INCLUDES = \
	-DPREFIX=\""$(prefix)"\"				\
	-DLIBDIR=\""$(libdir)"\"				\
	-DDATADIR=\""$(datadir)"\"				\
	-DGNOMELOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale"\"

common_cflags = \
	-I$(top_srcdir)/lib \
	$(PHIDIAS_CFLAGS)

common_ldadd = \
	$(top_builddir)/lib/libphidiasplugins.la \
	$(PHIDIAS_LIBS)

bin_PROGRAMS = phidias

marshal_source = \
	phidias-marshal.h      \
	phidias-marshal.c

BUILT_SOURCES = $(marshal_source)

phidias_SOURCES = 			\
	$(BUILT_SOURCES)		\
	expander.c			\
	main.c				\
	phidias-channelview.c		\
	phidias-engine.c		\
	phidias-loader.c		\
	phidias-module.c		\
	phidias-search-entry.c		\
	phidias-shell.c			\
	phidias-views.c

phidias-marshal.h: phidias-marshal.list
	$(GLIB_GENMARSHAL) $< --prefix=phidias_marshal --header > $@

phidias-marshal.c: phidias-marshal.list
	$(GLIB_GENMARSHAL) $< --prefix=phidias_marshal --body >> $@

phidias_CFLAGS = $(common_cflags)
phidias_LDADD = $(common_ldadd)

plugindir = @plugindir@
SUBDIRS = items_plugins channels_plugins
