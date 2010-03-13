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

#ifndef PHIDIAS_ITEMS_STREAM_H
#define PHIDIAS_ITEMS_STREAM_H

#include "common.h"

#define PHIDIAS_ITEMS_STREAM_TYPE		(phidias_items_stream_get_type ())
#define PHIDIAS_ITEMS_STREAM(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj),	\
						 PHIDIAS_ITEMS_STREAM_TYPE,		\
						 PhidiasItemsStream))
#define PHIDIAS_ITEMS_STREAM_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass),	\
						 PHIDIAS_ITEMS_STREAM_TYPE,		\
						 PhidiasItemsStreamClass))
#define IS_PHIDIAS_ITEMS_STREAM(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj),	\
						 PHIDIAS_ITEMS_STREAM_TYPE))
#define IS_PHIDIAS_ITEMS_STREAM_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass),	\
						 PHIDIAS_ITEMS_STREAM_TYPE))
#define PHIDIAS_ITEMS_STREAM_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj),	\
						 PHIDIAS_ITEMS_STREAM_TYPE,		\
						 PhidiasItemsStreamClass))

typedef struct _PhidiasItemsStream		PhidiasItemsStream;
typedef struct _PhidiasItemsStreamClass		PhidiasItemsStreamClass;
typedef struct _PhidiasItemsStreamPrivate	PhidiasItemsStreamPrivate;

struct _PhidiasItemsStream {
	GtkScrolledWindow		parent;
	PhidiasItemsStreamPrivate	*priv;
};

struct _PhidiasItemsStreamClass {
	GtkScrolledWindowClass		parent_class;
};

GType			phidias_items_stream_get_type ();

PhidiasItemsStream*	phidias_items_stream_new ();

#endif
