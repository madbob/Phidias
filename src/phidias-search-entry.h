/*  Copyright (C) 2010 Roberto Guido <madbob@users.barberaware.org>
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

#ifndef PHIDIAS_SEARCH_ENTRY_H
#define PHIDIAS_SEARCH_ENTRY_H

#include "common.h"
#include "phidias-views.h"

#define PHIDIAS_SEARCH_ENTRY_TYPE		(phidias_search_entry_get_type ())
#define PHIDIAS_SEARCH_ENTRY(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj),	\
						 PHIDIAS_SEARCH_ENTRY_TYPE,		\
						 PhidiasSearchEntry))
#define PHIDIAS_SEARCH_ENTRY_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass),	\
						 PHIDIAS_SEARCH_ENTRY_TYPE,		\
						 PhidiasSearchEntryClass))
#define IS_PHIDIAS_SEARCH_ENTRY(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj),	\
						 PHIDIAS_SEARCH_ENTRY_TYPE))
#define IS_PHIDIAS_SEARCH_ENTRY_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass),	\
						 PHIDIAS_SEARCH_ENTRY_TYPE))
#define PHIDIAS_SEARCH_ENTRY_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj),	\
						 PHIDIAS_SEARCH_ENTRY_TYPE,		\
						 PhidiasSearchEntryClass))

typedef struct _PhidiasSearchEntry		PhidiasSearchEntry;
typedef struct _PhidiasSearchEntryClass		PhidiasSearchEntryClass;
typedef struct _PhidiasSearchEntryPrivate	PhidiasSearchEntryPrivate;

struct _PhidiasSearchEntry {
	GtkEntry			parent;
	PhidiasSearchEntryPrivate	*priv;
};

struct _PhidiasSearchEntryClass {
	GtkEntryClass			parent_class;

	void (*filter_changed) (PhidiasSearchEntry *entry, const gchar *old_filter, const gchar *new_filter);
};

GType		phidias_search_entry_get_type ();

GtkWidget*	phidias_search_entry_new ();

#endif
