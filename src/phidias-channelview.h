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

#ifndef PHIDIAS_CHANNELVIEW_H
#define PHIDIAS_CHANNELVIEW_H

#include "common.h"

#define PHIDIAS_CHANNELVIEW_TYPE		(phidias_channelview_get_type ())
#define PHIDIAS_CHANNELVIEW(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj),	\
						 PHIDIAS_CHANNELVIEW_TYPE,		\
						 PhidiasChannelView))
#define PHIDIAS_CHANNELVIEW_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass),	\
						 PHIDIAS_CHANNELVIEW_TYPE,		\
						 PhidiasChannelViewClass))
#define IS_PHIDIAS_CHANNELVIEW(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj),	\
						 PHIDIAS_CHANNELVIEW_TYPE))
#define IS_PHIDIAS_CHANNELVIEW_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass),	\
						 PHIDIAS_CHANNELVIEW_TYPE))
#define PHIDIAS_CHANNELVIEW_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj),	\
						 PHIDIAS_CHANNELVIEW_TYPE,		\
						 PhidiasChannelViewClass))

typedef struct _PhidiasChannelView		PhidiasChannelView;
typedef struct _PhidiasChannelViewClass		PhidiasChannelViewClass;
typedef struct _PhidiasChannelViewPrivate	PhidiasChannelViewPrivate;

struct _PhidiasChannelView {
	GtkVBox				parent;
	PhidiasChannelViewPrivate	*priv;
};

struct _PhidiasChannelViewClass {
	GtkVBoxClass			parent_class;

	void (*channel_changed) (PhidiasChannelView *view, GtkTreeIter *selected);
};

GType		phidias_channelview_get_type ();

GtkWidget*	phidias_channelview_new ();
void		phidias_channelview_set_channels (PhidiasChannelView *view, GtkTreeModel *channels);

#endif
