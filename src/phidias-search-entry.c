/*  Copyright (C) 2010/2014 Roberto Guido <bob@linux.it>
 *
 *  This file is part of Phidias
 *
 *  Phidias is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "phidias-search-entry.h"
#include "phidias-marshal.h"

#define PHIDIAS_SEARCH_ENTRY_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), PHIDIAS_SEARCH_ENTRY_TYPE, PhidiasSearchEntryPrivate))

struct _PhidiasSearchEntryPrivate {
	int		rfu;
};

enum {
	FILTER_CHANGED,
	LAST_SIGNAL
};

static guint signals [LAST_SIGNAL];

G_DEFINE_TYPE (PhidiasSearchEntry, phidias_search_entry, GTK_TYPE_ENTRY);

static void string_input_cb (GtkEditable *editable, PhidiasSearchEntry *item)
{
	const gchar *str;

	str = gtk_entry_get_text (GTK_ENTRY (editable));
	g_signal_emit (item, signals [FILTER_CHANGED], 0, str, NULL);
}

static void phidias_search_entry_finalize (GObject *obj)
{
	PhidiasSearchEntry *item;
	item = PHIDIAS_SEARCH_ENTRY (obj);
}

static void phidias_search_entry_class_init (PhidiasSearchEntryClass *klass)
{
	GObjectClass *gobject_class;

	g_type_class_add_private (klass, sizeof (PhidiasSearchEntryPrivate));

	gobject_class = G_OBJECT_CLASS (klass);
	gobject_class->finalize = phidias_search_entry_finalize;

	signals [FILTER_CHANGED] = g_signal_new ("filter-changed",
				G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
				0, NULL, NULL,
				g_cclosure_marshal_VOID__STRING,
				G_TYPE_NONE, 1, G_TYPE_STRING);
}

static void phidias_search_entry_init (PhidiasSearchEntry *item)
{
	item->priv = PHIDIAS_SEARCH_ENTRY_GET_PRIVATE (item);
	g_signal_connect (GTK_EDITABLE (item), "changed", G_CALLBACK (string_input_cb), item);
}

GtkWidget* phidias_search_entry_new ()
{
	return g_object_new (PHIDIAS_SEARCH_ENTRY_TYPE, NULL);
}
