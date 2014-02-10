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

#ifndef PHIDIAS_ITEMS_TAGS_H
#define PHIDIAS_ITEMS_TAGS_H

#include "common.h"
#include <clutter-gtk/clutter-gtk.h>

#define PHIDIAS_ITEMS_TAGS_TYPE			(phidias_items_tags_get_type ())
#define PHIDIAS_ITEMS_TAGS(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj),	\
						 PHIDIAS_ITEMS_TAGS_TYPE,		\
						 PhidiasItemsTags))
#define PHIDIAS_ITEMS_TAGS_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass),	\
						 PHIDIAS_ITEMS_TAGS_TYPE,		\
						 PhidiasItemsTagsClass))
#define IS_PHIDIAS_ITEMS_TAGS(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj),	\
						 PHIDIAS_ITEMS_TAGS_TYPE))
#define IS_PHIDIAS_ITEMS_TAGS_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass),	\
						 PHIDIAS_ITEMS_TAGS_TYPE))
#define PHIDIAS_ITEMS_TAGS_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj),	\
						 PHIDIAS_ITEMS_TAGS_TYPE,		\
						 PhidiasItemsTagsClass))

typedef struct _PhidiasItemsTags		PhidiasItemsTags;
typedef struct _PhidiasItemsTagsClass		PhidiasItemsTagsClass;
typedef struct _PhidiasItemsTagsPrivate		PhidiasItemsTagsPrivate;

struct _PhidiasItemsTags {
	GtkClutterEmbed		parent;
	PhidiasItemsTagsPrivate	*priv;
};

struct _PhidiasItemsTagsClass {
	GtkClutterEmbedClass	parent_class;
};

GType			phidias_items_tags_get_type ();

PhidiasItemsTags*	phidias_items_tags_new ();

#endif
