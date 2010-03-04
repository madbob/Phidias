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

#ifndef PHIDIAS_CHANNELS_VIEWER_H
#define PHIDIAS_CHANNELS_VIEWER_H

#include <gtk/gtk.h>

#define PHIDIAS_CHANNELS_VIEWER_TYPE			(phidias_channels_viewer_get_type ())
#define PHIDIAS_CHANNELS_VIEWER(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj),	\
							 PHIDIAS_CHANNELS_VIEWER_TYPE,		\
							 PhidiasChannelsViewer))
#define IS_PHIDIAS_CHANNELS_VIEWER(obj)			(G_TYPE_CHECK_INSTANCE_TYPE ((obj),	\
							 PHIDIAS_CHANNELS_VIEWER_TYPE))
#define PHIDIAS_CHANNELS_VIEWER_GET_INTERFACE(inst) 	(G_TYPE_INSTANCE_GET_INTERFACE ((inst),	\
							 PHIDIAS_CHANNELS_VIEWER_TYPE, PhidiasChannelsViewerInterface))

typedef struct _PhidiasChannelsViewer		PhidiasChannelsViewer;
typedef struct _PhidiasChannelsViewerInterface	PhidiasChannelsViewerInterface;

struct _PhidiasChannelsViewerInterface {
	GTypeInterface		parent;

	const gchar* (*get_name) (PhidiasChannelsViewer *view);
	void (*set_channels) (PhidiasChannelsViewer *view, GtkTreeModel *channels);

	void (*channel_changed) (PhidiasChannelsViewer *view, GtkTreeIter *selected);
};

GType		phidias_channels_viewer_get_type ();

const gchar*	phidias_channels_viewer_get_name (PhidiasChannelsViewer *self);
void		phidias_channels_viewer_set_channels (PhidiasChannelsViewer *self, GtkTreeModel *channels);

#endif
