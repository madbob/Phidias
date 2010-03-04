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

#ifndef PHIDIAS_ITEMS_GEO_H
#define PHIDIAS_ITEMS_GEO_H

#include "common.h"
#include <champlain-gtk/champlain-gtk.h>

#define PHIDIAS_ITEMS_GEO_TYPE			(phidias_items_geo_get_type ())
#define PHIDIAS_ITEMS_GEO(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj),	\
						 PHIDIAS_ITEMS_GEO_TYPE,		\
						 PhidiasItemsGeo))
#define PHIDIAS_ITEMS_GEO_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass),	\
						 PHIDIAS_ITEMS_GEO_TYPE,		\
						 PhidiasItemsGeoClass))
#define IS_PHIDIAS_ITEMS_GEO(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj),	\
						 PHIDIAS_ITEMS_GEO_TYPE))
#define IS_PHIDIAS_ITEMS_GEO_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass),	\
						 PHIDIAS_ITEMS_GEO_TYPE))
#define PHIDIAS_ITEMS_GEO_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj),	\
						 PHIDIAS_ITEMS_GEO_TYPE,		\
						 PhidiasItemsGeoClass))

typedef struct _PhidiasItemsGeo		PhidiasItemsGeo;
typedef struct _PhidiasItemsGeoClass	PhidiasItemsGeoClass;
typedef struct _PhidiasItemsGeoPrivate	PhidiasItemsGeoPrivate;

struct _PhidiasItemsGeo {
	GtkChamplainEmbed	parent;
	PhidiasItemsGeoPrivate	*priv;
};

struct _PhidiasItemsGeoClass {
	GtkChamplainEmbedClass	parent_class;
};

GType			phidias_items_geo_get_type ();

PhidiasItemsGeo*	phidias_items_geo_new ();

#endif
