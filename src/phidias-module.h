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

/*
 *  Most of the code is from Epiphany 2.28.2:
 *
 *  Copyright © 2003 Marco Pesenti Gritti
 *  Copyright © 2003, 2004 Christian Persch
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 */

#ifndef PHIDIAS_MODULE_H
#define PHIDIAS_MODULE_H

#include "common.h"

#define PHIDIAS_MODULE_TYPE		(phidias_module_get_type ())
#define PHIDIAS_MODULE(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), PHIDIAS_MODULE_TYPE, PhidiasModule))
#define PHIDIAS_MODULE_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), PHIDIAS_MODULE_TYPE, PhidiasModuleClass))
#define IS_PHIDIAS_MODULE(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), PHIDIAS_MODULE_TYPE))
#define IS_PHIDIAS_MODULE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((obj), PHIDIAS_MODULE_TYPE))
#define PHIDIAS_MODULE_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), PHIDIAS_MODULE_TYPE, PhidiasModuleClass))

typedef struct _PhidiasModule		PhidiasModule;
typedef struct _PhidiasModuleClass	PhidiasModuleClass;

struct _PhidiasModule {
	GTypeModule	parent_instance;

	GModule		*library;

	char		*path;
	GType		type;
};

struct _PhidiasModuleClass {
	GTypeModuleClass	parent_class;
};

GType		phidias_module_get_type		();

PhidiasModule*	phidias_module_new		(const char *path);

const char*	phidias_module_get_path		(PhidiasModule *module);
GObject*	phidias_module_new_object	(PhidiasModule *module);

#endif
