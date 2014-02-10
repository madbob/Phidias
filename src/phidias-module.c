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

#include "phidias-module.h"

typedef GType (*PhidiasModuleRegisterFunc) (GTypeModule *);

G_DEFINE_TYPE (PhidiasModule, phidias_module, G_TYPE_TYPE_MODULE)

static gboolean phidias_module_load (GTypeModule *gmodule)
{
	PhidiasModule *module;
	PhidiasModuleRegisterFunc register_func;

	module = PHIDIAS_MODULE (gmodule);

	if (module->library == NULL)
		module->library = g_module_open (module->path, G_MODULE_BIND_LOCAL | G_MODULE_BIND_LAZY);

	if (module->library == NULL) {
		g_warning ("No library: %s", g_module_error ());
		return FALSE;
	}

	if (!g_module_symbol (module->library, "register_module", (void *) &register_func)) {
		g_warning ("No symbol: %s", g_module_error ());
		g_module_close (module->library);
		return FALSE;
	}

	if (!register_func) {
		g_warning ("Symbol 'register_module' is NULL.");
		g_module_close (module->library);
		return FALSE;
	}

	module->type = register_func (gmodule);

	if (module->type == 0) {
		g_warning ("Failed to register the GType(s)!");
		g_module_close (module->library);
		return FALSE;
	}

	g_module_make_resident (module->library);
	return TRUE;
}

static void phidias_module_unload (GTypeModule *gmodule)
{
	PhidiasModule *module;

	module = PHIDIAS_MODULE (gmodule);
	g_module_close (module->library);
	module->library = NULL;
	module->type = 0;
}

const char* phidias_module_get_path (PhidiasModule *module)
{
	g_return_val_if_fail (IS_PHIDIAS_MODULE (module), NULL);
	return module->path;
}

GObject* phidias_module_new_object (PhidiasModule *module)
{
	if (module->type == 0)
		return NULL;
	else
		return g_object_new (module->type, NULL);
}

static void phidias_module_init (PhidiasModule *module)
{
}

static void phidias_module_finalize (GObject *object)
{
	PhidiasModule *module;

	module = PHIDIAS_MODULE (object);
	g_free (module->path);
	G_OBJECT_CLASS (phidias_module_parent_class)->finalize (object);
}

static void phidias_module_class_init (PhidiasModuleClass *class)
{
	GObjectClass *object_class;
	GTypeModuleClass *module_class;

	object_class = G_OBJECT_CLASS (class);
	object_class->finalize = phidias_module_finalize;

	module_class = G_TYPE_MODULE_CLASS (class);
	module_class->load = phidias_module_load;
	module_class->unload = phidias_module_unload;
}

PhidiasModule* phidias_module_new (const char *path)
{
	PhidiasModule *result;

	if (path == NULL || path [0] == '\0')
		return NULL;

	result = g_object_new (PHIDIAS_MODULE_TYPE, NULL);

	g_type_module_set_name (G_TYPE_MODULE (result), path);
	result->path = g_strdup (path);

	return result;
}
