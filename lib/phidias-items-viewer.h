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

#ifndef PHIDIAS_ITEMS_VIEWER_H
#define PHIDIAS_ITEMS_VIEWER_H

#include <gtk/gtk.h>
#include "phidias-extra-column.h"

#define PHIDIAS_ITEMS_VIEWER_TYPE			(phidias_items_viewer_get_type ())
#define PHIDIAS_ITEMS_VIEWER(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj),	\
							 PHIDIAS_ITEMS_VIEWER_TYPE,		\
							 PhidiasItemsViewer))
#define IS_PHIDIAS_ITEMS_VIEWER(obj)			(G_TYPE_CHECK_INSTANCE_TYPE ((obj),	\
							 PHIDIAS_ITEMS_VIEWER_TYPE))
#define PHIDIAS_ITEMS_VIEWER_GET_INTERFACE(inst) 	(G_TYPE_INSTANCE_GET_INTERFACE ((inst),	\
							 PHIDIAS_ITEMS_VIEWER_TYPE, PhidiasItemsViewerInterface))

typedef struct _PhidiasItemsViewer		PhidiasItemsViewer;
typedef struct _PhidiasItemsViewerInterface	PhidiasItemsViewerInterface;

struct _PhidiasItemsViewerInterface {
	GTypeInterface		parent;

	const gchar* (*get_name) (PhidiasItemsViewer *view);
	void (*set_model) (PhidiasItemsViewer *view, GtkTreeModel *items);
	const GPtrArray* (*get_extra) (PhidiasItemsViewer *view);

	void (*item_update_required) (PhidiasItemsViewer *view, GtkTreeIter *iter, int index, gchar *value);
};

GType			phidias_items_viewer_get_type ();

const gchar*		phidias_items_viewer_get_name (PhidiasItemsViewer *self);
void			phidias_items_viewer_set_model (PhidiasItemsViewer *self, GtkTreeModel *items);

const GPtrArray*	phidias_items_viewer_get_extra_data (PhidiasItemsViewer *self);

#endif
