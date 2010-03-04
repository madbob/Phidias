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

#ifndef PHIDIAS_EXTRA_COLUMN_H
#define PHIDIAS_EXTRA_COLUMN_H

#include <gtk/gtk.h>

#define PHIDIAS_EXTRA_COLUMN_TYPE		(phidias_extra_column_get_type ())
#define PHIDIAS_EXTRA_COLUMN(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj),	\
						 PHIDIAS_EXTRA_COLUMN_TYPE,		\
						 PhidiasExtraColumn))
#define PHIDIAS_EXTRA_COLUMN_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass),	\
						 PHIDIAS_EXTRA_COLUMN_TYPE,		\
						 PhidiasExtraColumnClass))
#define IS_PHIDIAS_EXTRA_COLUMN(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj),	\
						 PHIDIAS_EXTRA_COLUMN_TYPE))
#define IS_PHIDIAS_EXTRA_COLUMN_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass),	\
						 PHIDIAS_EXTRA_COLUMN_TYPE))
#define PHIDIAS_EXTRA_COLUMN_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj),	\
						 PHIDIAS_EXTRA_COLUMN_TYPE,		\
						 PhidiasExtraColumnClass))

typedef struct _PhidiasExtraColumn		PhidiasExtraColumn;
typedef struct _PhidiasExtraColumnClass		PhidiasExtraColumnClass;
typedef struct _PhidiasExtraColumnPrivate	PhidiasExtraColumnPrivate;

typedef enum {
	PHIDIAS_EXTRA_COLUMN_NONE,
	PHIDIAS_EXTRA_COLUMN_PREDICATE,
	PHIDIAS_EXTRA_COLUMN_PATH,
} PHIDIAS_EXTRA_COLUMN_CONTENT_TYPE;

struct _PhidiasExtraColumn {
	GObject				parent;
	PhidiasExtraColumnPrivate	*priv;
};

struct _PhidiasExtraColumnClass {
	GObjectClass	parent_class;
};

GType					phidias_extra_column_get_type ();

PhidiasExtraColumn*			phidias_extra_column_new ();

PHIDIAS_EXTRA_COLUMN_CONTENT_TYPE	phidias_extra_column_get_content_type (PhidiasExtraColumn *column);

void					phidias_extra_column_set_predicate (PhidiasExtraColumn *column, gchar *predicate);
const gchar*				phidias_extra_column_get_predicate (PhidiasExtraColumn *column);
void					phidias_extra_column_set_path (PhidiasExtraColumn *column, gchar **path);
void					phidias_extra_column_set_pathv (PhidiasExtraColumn *column, gchar *first, ...);
const gchar**				phidias_extra_column_get_path (PhidiasExtraColumn *column);
void					phidias_extra_column_set_index (PhidiasExtraColumn *column, int index);
int					phidias_extra_column_get_index (PhidiasExtraColumn *column);

#endif
