plugindir = @plugindir@/items

NULL =
LIBS =
PLUGIN_LDFLAGS = -module -avoid-version -shared

INCLUDES = \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/lib \
	-DPREFIX=\""$(prefix)"\" \
	-DLIBDIR=\""$(libdir)"\" \
	-DG_DISABLE_DEPRECATED \
	$(PHIDIAS_CFLAGS)

plugin_LTLIBRARIES = \
	phidias-items-classic.la	\
	phidias-items-geo.la		\
	phidias-items-stream.la		\
	phidias-items-tags.la

phidias_items_classic_la_LDFLAGS = $(PLUGIN_LDFLAGS)
phidias_items_classic_la_LIBADD = $(PHIDIAS_LIBS)
phidias_items_classic_la_SOURCES = \
	phidias-items-classic.c

phidias_items_geo_la_LDFLAGS = $(PLUGIN_LDFLAGS)
phidias_items_geo_la_LIBADD = $(PHIDIAS_LIBS)
phidias_items_geo_la_SOURCES = \
	phidias-items-geo.c

phidias_items_stream_la_LDFLAGS = $(PLUGIN_LDFLAGS)
phidias_items_stream_la_LIBADD = $(PHIDIAS_LIBS)
phidias_items_stream_la_SOURCES = \
	phidias-items-stream.c

phidias_items_tags_la_LDFLAGS = $(PLUGIN_LDFLAGS)
phidias_items_tags_la_LIBADD = $(PHIDIAS_LIBS)
phidias_items_tags_la_SOURCES = \
	phidias-items-tags.c
