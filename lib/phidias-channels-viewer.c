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

#include "phidias-channels-viewer.h"

enum {
	CHANNEL_CHANGED,
	LAST_SIGNAL
};

static guint signals [LAST_SIGNAL];

static void phidias_channels_viewer_base_init (gpointer class)
{
	static gboolean is_initialized = FALSE;

	if (!is_initialized) {
		signals [CHANNEL_CHANGED] = g_signal_new ("channel-changed",
				PHIDIAS_CHANNELS_VIEWER_TYPE, G_SIGNAL_RUN_LAST,
				0, NULL, NULL,
				g_cclosure_marshal_VOID__POINTER,
				G_TYPE_NONE, 1, G_TYPE_POINTER);

		is_initialized = TRUE;
	}
}

GType phidias_channels_viewer_get_type (void)
{
	static GType iface_type = 0;

	if (iface_type == 0) {
		static const GTypeInfo info = {
			sizeof (PhidiasChannelsViewerInterface),
			phidias_channels_viewer_base_init,
			NULL,
		};

		iface_type = g_type_register_static (G_TYPE_INTERFACE, "PhidiasChannelsViewer", &info, 0);
		g_type_interface_add_prerequisite (iface_type, GTK_TYPE_WIDGET);
	}

	return iface_type;
}

const gchar* phidias_channels_viewer_get_name (PhidiasChannelsViewer *self)
{
	return PHIDIAS_CHANNELS_VIEWER_GET_INTERFACE (self)->get_name (self);
}

void phidias_channels_viewer_set_channels (PhidiasChannelsViewer *self, GtkTreeModel *channels)
{
	return PHIDIAS_CHANNELS_VIEWER_GET_INTERFACE (self)->set_channels (self, channels);
}
