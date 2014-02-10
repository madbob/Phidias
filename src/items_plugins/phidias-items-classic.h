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

#ifndef PHIDIAS_ITEMS_CLASSIC_H
#define PHIDIAS_ITEMS_CLASSIC_H

#include "common.h"

#define PHIDIAS_ITEMS_CLASSIC_TYPE		(phidias_items_classic_get_type ())
#define PHIDIAS_ITEMS_CLASSIC(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj),	\
						 PHIDIAS_ITEMS_CLASSIC_TYPE,		\
						 PhidiasItemsClassic))
#define PHIDIAS_ITEMS_CLASSIC_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass),	\
						 PHIDIAS_ITEMS_CLASSIC_TYPE,		\
						 PhidiasItemsClassicClass))
#define IS_PHIDIAS_ITEMS_CLASSIC(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj),	\
						 PHIDIAS_ITEMS_CLASSIC_TYPE))
#define IS_PHIDIAS_ITEMS_CLASSIC_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass),	\
						 PHIDIAS_ITEMS_CLASSIC_TYPE))
#define PHIDIAS_ITEMS_CLASSIC_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj),	\
						 PHIDIAS_ITEMS_CLASSIC_TYPE,		\
						 PhidiasItemsClassicClass))

typedef struct _PhidiasItemsClassic		PhidiasItemsClassic;
typedef struct _PhidiasItemsClassicClass	PhidiasItemsClassicClass;
typedef struct _PhidiasItemsClassicPrivate	PhidiasItemsClassicPrivate;

struct _PhidiasItemsClassic {
	GtkVPaned			parent;
	PhidiasItemsClassicPrivate	*priv;
};

struct _PhidiasItemsClassicClass {
	GtkVPanedClass		parent_class;
};

GType			phidias_items_classic_get_type ();

PhidiasItemsClassic*	phidias_items_classic_new ();

#endif
