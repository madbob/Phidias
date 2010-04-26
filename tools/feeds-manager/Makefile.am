NULL =

INCLUDES = \
	-DPREFIX=\""$(prefix)"\"				\
	-DLIBDIR=\""$(libdir)"\"				\
	-DDATADIR=\""$(datadir)"\"				\
	-DGNOMELOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale"\"

common_cflags =			\
	-I$(top_srcdir)/lib	\
	$(PHIDIAS_CFLAGS)

common_ldadd = \
	$(PHIDIAS_LIBS)

bin_PROGRAMS = feeds_manager

marshal_source = \
	marshal.h	\
	marshal.c

BUILT_SOURCES = $(marshal_source)

feeds_manager_SOURCES = \
	$(BUILT_SOURCES)		\
	main.c				\
	feed-settings.c			\
	feed-settings.h

marshal.h: marshal.list
	$(GLIB_GENMARSHAL) $< --prefix=marshal --header > $@

marshal.c: marshal.list
	$(GLIB_GENMARSHAL) $< --prefix=marshal --body >> $@

feeds_manager_CFLAGS = $(common_cflags)
feeds_manager_LDADD = $(common_ldadd)
