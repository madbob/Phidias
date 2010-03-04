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

#include "phidias-loader.h"
#include "phidias-module.h"

#define PHIDIAS_LOADER_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), PHIDIAS_LOADER_TYPE, PhidiasLoaderPrivate))

struct _PhidiasLoaderPrivate {
	gchar	*path;
};

enum {
	PLUGIN_FOUND,
	LAST_SIGNAL
};

enum {
	PROP_0,
	PROP_PATH
};

static guint signals [LAST_SIGNAL];

G_DEFINE_TYPE (PhidiasLoader, phidias_loader, G_TYPE_OBJECT);

static void load_plugins (PhidiasLoader *item)
{
	gchar *plug_name;
	gchar *plug_path;
	GObject *obj;
	GError *error;
	GFile *plugins_folder;
	GFileEnumerator *enumerator;
	GFileInfo *plugin;
	PhidiasModule *module;

	plugins_folder = g_file_new_for_path (item->priv->path);

	error = NULL;
	enumerator = g_file_enumerate_children (plugins_folder, "standard::*", G_FILE_QUERY_INFO_NONE, NULL, &error);
	if (enumerator == NULL) {
		g_warning ("Unable to list plugins in %s: %s.", item->priv->path, error->message);
		g_error_free (error);
	}
	else {
		for (;;) {
			error = NULL;
			plugin = g_file_enumerator_next_file (enumerator, NULL, &error);

			if (plugin == NULL) {
				if (error != NULL) {
					g_warning ("Unable to iterate plugins in %s: %s.", item->priv->path, error->message);
					g_error_free (error);
				}

				break;
			}

			plug_name = g_file_info_get_attribute_as_string (plugin, G_FILE_ATTRIBUTE_STANDARD_NAME);
			if (g_str_has_suffix (plug_name, ".la") == TRUE) {
				plug_path = g_build_filename (item->priv->path, plug_name, NULL);
				module = phidias_module_new (plug_path);

				if (g_type_module_use (G_TYPE_MODULE (module)) == FALSE) {
					g_warning ("Unable to load module at %s.", plug_path);
					g_free (plug_path);
					continue;
				}

				obj = phidias_module_new_object (module);
				if (obj == NULL) {
					g_warning ("Unable to load module at %s.", plug_path);
				}
				else {
					g_signal_emit (item, signals [PLUGIN_FOUND], 0, obj, NULL);
				}

				g_free (plug_path);
			}

			g_free (plug_name);
			g_object_unref (plugin);
		}
	}

	g_object_unref (plugins_folder);
}

static void phidias_loader_finalize (GObject *obj)
{
	/**
		TODO
	*/
}

static void phidias_loader_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	PhidiasLoader *loader;

	loader = PHIDIAS_LOADER (object);

	switch (prop_id) {
		case PROP_PATH:
			g_value_set_string (value, loader->priv->path);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void phidias_loader_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	PhidiasLoader *loader;

	loader = PHIDIAS_LOADER (object);

	switch (prop_id) {
		case PROP_PATH:
			if (loader->priv->path != NULL)
				g_free (loader->priv->path);
			loader->priv->path = g_value_dup_string (value);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void phidias_loader_class_init (PhidiasLoaderClass *klass)
{
	GObjectClass *gobject_class;

	g_type_class_add_private (klass, sizeof (PhidiasLoaderPrivate));

	gobject_class = G_OBJECT_CLASS (klass);
	gobject_class->finalize = phidias_loader_finalize;
	gobject_class->set_property = phidias_loader_set_property;
	gobject_class->get_property = phidias_loader_get_property;

	g_object_class_install_property (gobject_class, PROP_PATH,
			g_param_spec_string  ("path", "Path", "Plugins path", NULL,
				G_PARAM_READWRITE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_CONSTRUCT_ONLY));

	signals [PLUGIN_FOUND] = g_signal_new ("plugin-found",
			G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
			G_STRUCT_OFFSET (PhidiasLoaderClass, plugin_found), NULL, NULL,
			g_cclosure_marshal_VOID__OBJECT,
			G_TYPE_NONE, 1, G_TYPE_OBJECT);
}

static void phidias_loader_init (PhidiasLoader *item)
{
	item->priv = PHIDIAS_LOADER_GET_PRIVATE (item);
}

PhidiasLoader* phidias_loader_new (gchar *folder_path)
{
	return g_object_new (PHIDIAS_LOADER_TYPE, "path", folder_path, NULL);
}

void phidias_loader_run (PhidiasLoader *loader)
{
	load_plugins (loader);

	/**
		TODO	Install a monitor over the folder and notify about added/removed plugins
	*/
}
