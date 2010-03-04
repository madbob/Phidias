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

#include "phidias-items-viewer.h"
#include "phidias-plugins-marshal.h"

enum {
	ITEM_UPDATE_REQUIRED,
	LAST_SIGNAL
};

static guint signals [LAST_SIGNAL];

static void phidias_items_viewer_base_init (gpointer g_class)
{
	static gboolean is_initialized = FALSE;

	if (!is_initialized) {
		signals [ITEM_UPDATE_REQUIRED] = g_signal_new ("item-update-required",
				PHIDIAS_ITEMS_VIEWER_TYPE, G_SIGNAL_RUN_LAST,
				0, NULL, NULL,
				phidias_plugin_marshal_VOID__POINTER_INT_STRING,
				G_TYPE_NONE, 3, G_TYPE_POINTER, G_TYPE_INT, G_TYPE_STRING);

		is_initialized = TRUE;
	}
}

GType phidias_items_viewer_get_type ()
{
	static GType iface_type = 0;

	if (iface_type == 0) {
		static const GTypeInfo info = {
			sizeof (PhidiasItemsViewerInterface),
			phidias_items_viewer_base_init,
			NULL,
		};

		iface_type = g_type_register_static (G_TYPE_INTERFACE, "PhidiasItemsViewer", &info, 0);
		g_type_interface_add_prerequisite (iface_type, GTK_TYPE_WIDGET);
	}

	return iface_type;
}

const gchar* phidias_items_viewer_get_name (PhidiasItemsViewer *self)
{
	if (PHIDIAS_ITEMS_VIEWER_GET_INTERFACE (self)->get_name != NULL)
		return PHIDIAS_ITEMS_VIEWER_GET_INTERFACE (self)->get_name (self);
	else
		return "Anonymous";
}

void phidias_items_viewer_set_model (PhidiasItemsViewer *self, GtkTreeModel *items)
{
	if (PHIDIAS_ITEMS_VIEWER_GET_INTERFACE (self)->set_model != NULL)
		PHIDIAS_ITEMS_VIEWER_GET_INTERFACE (self)->set_model (self, items);
}

const GPtrArray* phidias_items_viewer_get_extra_data (PhidiasItemsViewer *self)
{
	if (PHIDIAS_ITEMS_VIEWER_GET_INTERFACE (self)->get_extra != NULL)
		return PHIDIAS_ITEMS_VIEWER_GET_INTERFACE (self)->get_extra (self);
	else
		return NULL;
}
