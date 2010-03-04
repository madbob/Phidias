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

#include <string.h>
#include <stdlib.h>
#include "phidias-extra-column.h"

#define PHIDIAS_EXTRA_COLUMN_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), PHIDIAS_EXTRA_COLUMN_TYPE, PhidiasExtraColumnPrivate))

struct _PhidiasExtraColumnPrivate {
	int	index;
	gchar	*predicate;
	gchar	**path;
};

G_DEFINE_TYPE (PhidiasExtraColumn, phidias_extra_column, G_TYPE_OBJECT);

static void phidias_extra_column_finalize (GObject *obj)
{
	PhidiasExtraColumn *item;

	item = PHIDIAS_EXTRA_COLUMN (obj);

	if (item->priv->predicate != NULL)
		g_free (item->priv->predicate);

	if (item->priv->path != NULL)
		g_strfreev (item->priv->path);
}

static void phidias_extra_column_class_init (PhidiasExtraColumnClass *klass)
{
	GObjectClass *gobject_class;

	g_type_class_add_private (klass, sizeof (PhidiasExtraColumnPrivate));

	gobject_class = G_OBJECT_CLASS (klass);
	gobject_class->finalize = phidias_extra_column_finalize;
}

static void phidias_extra_column_init (PhidiasExtraColumn *item)
{
	item->priv = PHIDIAS_EXTRA_COLUMN_GET_PRIVATE (item);
	memset (item->priv, 0, sizeof (PhidiasExtraColumnPrivate));
}

PhidiasExtraColumn* phidias_extra_column_new ()
{
	return g_object_new (PHIDIAS_EXTRA_COLUMN_TYPE, NULL);
}

PHIDIAS_EXTRA_COLUMN_CONTENT_TYPE phidias_extra_column_get_content_type (PhidiasExtraColumn *column)
{
	if (column->priv->predicate != NULL)
		return PHIDIAS_EXTRA_COLUMN_PREDICATE;
	else if (column->priv->path != NULL)
		return PHIDIAS_EXTRA_COLUMN_PATH;
	else
		return PHIDIAS_EXTRA_COLUMN_NONE;
}

void phidias_extra_column_set_predicate (PhidiasExtraColumn *column, gchar *predicate)
{
	if (column->priv->predicate != NULL)
		g_free (column->priv->predicate);
	if (column->priv->path != NULL)
		g_strfreev (column->priv->path);

	column->priv->predicate = g_strdup (predicate);
}

const gchar* phidias_extra_column_get_predicate (PhidiasExtraColumn *column)
{
	return column->priv->predicate;
}

void phidias_extra_column_set_path (PhidiasExtraColumn *column, gchar **path)
{
	if (column->priv->predicate != NULL)
		g_free (column->priv->predicate);
	if (column->priv->path != NULL)
		g_strfreev (column->priv->path);

	column->priv->path = g_strdupv (path);
}

void phidias_extra_column_set_pathv (PhidiasExtraColumn *column, gchar *first, ...)
{
	int len;
	gchar **params;
	va_list ap;

	va_start (ap, first);
	params = NULL;
	len = 0;

	while (first != NULL) {
		len++;
		params = realloc (params, sizeof (gchar*) * len);
		params [len - 1] = first;
		first = va_arg (ap, char*);
	}

	va_end (ap);

	len++;
	params = realloc (params, sizeof (gchar*) * len);
	params [len - 1] = NULL;

	phidias_extra_column_set_path (column, params);
	free (params);
}

const gchar** phidias_extra_column_get_path (PhidiasExtraColumn *column)
{
	return (const gchar**) column->priv->path;
}

void phidias_extra_column_set_index (PhidiasExtraColumn *column, int index)
{
	column->priv->index = index;
}

int phidias_extra_column_get_index (PhidiasExtraColumn *column)
{
	return column->priv->index;
}
