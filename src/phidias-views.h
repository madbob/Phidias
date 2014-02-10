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

#ifndef PHIDIAS_VIEWS_H
#define PHIDIAS_VIEWS_H

#include "common.h"

#define PHIDIAS_VIEWS_TYPE		(phidias_views_get_type ())
#define PHIDIAS_VIEWS(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj),	\
					 PHIDIAS_VIEWS_TYPE,			\
					 PhidiasViews))
#define PHIDIAS_VIEWS_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass),	\
					 PHIDIAS_VIEWS_TYPE,			\
					 PhidiasViewsClass))
#define IS_PHIDIAS_VIEWS(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj),	\
					 PHIDIAS_VIEWS_TYPE))
#define IS_PHIDIAS_VIEWS_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass),	\
					 PHIDIAS_VIEWS_TYPE))
#define PHIDIAS_VIEWS_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj),	\
					 PHIDIAS_VIEWS_TYPE,			\
					 PhidiasViewsClass))

typedef struct _PhidiasViews		PhidiasViews;
typedef struct _PhidiasViewsClass	PhidiasViewsClass;
typedef struct _PhidiasViewsPrivate	PhidiasViewsPrivate;

struct _PhidiasViews {
	GtkNotebook			parent;
	PhidiasViewsPrivate		*priv;
};

struct _PhidiasViewsClass {
	GtkNotebookClass		parent_class;
};

GType		phidias_views_get_type ();

GtkWidget*	phidias_views_new ();
void		phidias_views_set_items (PhidiasViews *views, GtkTreeModel *items);

#endif
