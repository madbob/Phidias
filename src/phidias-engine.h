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

#ifndef PHIDIAS_ENGINE_H
#define PHIDIAS_ENGINE_H

#include "common.h"
#include "phidias-extra-column.h"

#define PHIDIAS_ENGINE_TYPE		(phidias_engine_get_type ())
#define PHIDIAS_ENGINE(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj),	\
					 PHIDIAS_ENGINE_TYPE,			\
					 PhidiasEngine))
#define PHIDIAS_ENGINE_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass),	\
					 PHIDIAS_ENGINE_TYPE,			\
					 PhidiasEngineClass))
#define IS_PHIDIAS_ENGINE(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj),	\
					 PHIDIAS_ENGINE_TYPE))
#define IS_PHIDIAS_ENGINE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass),	\
					 PHIDIAS_ENGINE_TYPE))
#define PHIDIAS_ENGINE_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj),	\
					 PHIDIAS_ENGINE_TYPE,			\
					 PhidiasEngineClass))

typedef struct _PhidiasEngine		PhidiasEngine;
typedef struct _PhidiasEngineClass	PhidiasEngineClass;
typedef struct _PhidiasEnginePrivate	PhidiasEnginePrivate;

struct _PhidiasEngine {
	GObject			parent;
	PhidiasEnginePrivate	*priv;
};

struct _PhidiasEngineClass {
	GObjectClass		parent_class;

	void (*channel_changed) (PhidiasEngine *engine);
};

GType		phidias_engine_get_type ();

PhidiasEngine*	phidias_engine_new ();

GtkTreeModel*	phidias_engine_get_channels (PhidiasEngine *engine);
void		phidias_engine_set_channel (PhidiasEngine *engine, GtkTreeIter *channel);

int		phidias_engine_add_managed_info (PhidiasEngine *engine, PhidiasExtraColumn *extra);
GtkTreeModel*	phidias_engine_get_current_model (PhidiasEngine *engine);
void		phidias_engine_update_item (PhidiasEngine *engine, GtkTreeModel *model, GtkTreeIter *iter, int index, gchar *value);

void		phidias_engine_apply_filter (PhidiasEngine *engine, const gchar *filter);

#endif
