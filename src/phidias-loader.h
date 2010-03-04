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

#ifndef PHIDIAS_LOADER_H
#define PHIDIAS_LOADER_H

#include "common.h"

#define PHIDIAS_LOADER_TYPE		(phidias_loader_get_type ())
#define PHIDIAS_LOADER(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj),	\
					 PHIDIAS_LOADER_TYPE,			\
					 PhidiasLoader))
#define PHIDIAS_LOADER_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass),	\
					 PHIDIAS_LOADER_TYPE,			\
					 PhidiasLoaderClass))
#define IS_PHIDIAS_LOADER(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj),	\
					 PHIDIAS_LOADER_TYPE))
#define IS_PHIDIAS_LOADER_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass),	\
					 PHIDIAS_LOADER_TYPE))
#define PHIDIAS_LOADER_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj),	\
					 PHIDIAS_LOADER_TYPE,			\
					 PhidiasLoaderClass))

typedef struct _PhidiasLoader		PhidiasLoader;
typedef struct _PhidiasLoaderClass	PhidiasLoaderClass;
typedef struct _PhidiasLoaderPrivate	PhidiasLoaderPrivate;

struct _PhidiasLoader {
	GObject			parent;
	PhidiasLoaderPrivate	*priv;
};

struct _PhidiasLoaderClass {
	GObjectClass		parent_class;

	void (*plugin_found) (PhidiasLoader *loader, GObject *plug);
};

GType		phidias_loader_get_type ();

PhidiasLoader*	phidias_loader_new (gchar *folder_path);
void		phidias_loader_run (PhidiasLoader *loader);

#endif
