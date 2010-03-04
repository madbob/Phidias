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

#ifndef PHIDIAS_CHANNELS_BY_NAME_H
#define PHIDIAS_CHANNELS_BY_NAME_H

#include "common.h"

#define PHIDIAS_CHANNELS_BY_NAME_TYPE			(phidias_channels_by_name_get_type ())
#define PHIDIAS_CHANNELS_BY_NAME(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj),	\
							 PHIDIAS_CHANNELS_BY_NAME_TYPE,		\
							 PhidiasChannelsByName))
#define PHIDIAS_CHANNELS_BY_NAME_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass),	\
							 PHIDIAS_CHANNELS_BY_NAME_TYPE,		\
							 PhidiasChannelsByNameClass))
#define IS_PHIDIAS_CHANNELS_BY_NAME(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj),	\
							 PHIDIAS_CHANNELS_BY_NAME_TYPE))
#define IS_PHIDIAS_CHANNELS_BY_NAME_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass),	\
							 PHIDIAS_CHANNELS_BY_NAME_TYPE))
#define PHIDIAS_CHANNELS_BY_NAME_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS ((obj),	\
							 PHIDIAS_CHANNELS_BY_NAME_TYPE,		\
							 PhidiasChannelsByNameClass))

typedef struct _PhidiasChannelsByName		PhidiasChannelsByName;
typedef struct _PhidiasChannelsByNameClass	PhidiasChannelsByNameClass;
typedef struct _PhidiasChannelsByNamePrivate	PhidiasChannelsByNamePrivate;

struct _PhidiasChannelsByName {
	GtkTreeView			parent;
	PhidiasChannelsByNamePrivate	*priv;
};

struct _PhidiasChannelsByNameClass {
	GtkTreeViewClass		parent_class;
};

GType			phidias_channels_by_name_get_type ();

PhidiasChannelsByName*	phidias_channels_by_name_new ();

#endif
