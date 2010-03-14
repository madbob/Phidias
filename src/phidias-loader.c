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
	gchar		*path;

	GFile		*folder;
	GFileMonitor	*monitor;

	GList		*loaded_plugins;
};

enum {
	PLUGIN_FOUND,
	PLUGIN_REMOVED,
	LAST_SIGNAL
};

enum {
	PROP_0,
	PROP_PATH
};

static guint signals [LAST_SIGNAL];

G_DEFINE_TYPE (PhidiasLoader, phidias_loader, G_TYPE_OBJECT);

static void unload_plugin (PhidiasLoader *loader, const gchar *plug_path)
{
	/**
		TODO	Plugins unload doesn't works, probably due linking issues
	*/

	/*
	gchar *p;
	GList *iter;
	GObject *obj;

	for (iter = loader->priv->loaded_plugins; iter; iter = g_list_next (iter)) {
		obj = G_OBJECT (iter->data);
		p = (gchar*) g_object_get_data (obj, "path");

		if (strcmp (p, plug_path) == 0) {
			g_signal_emit (loader, signals [PLUGIN_REMOVED], 0, obj, NULL);
			loader->priv->loaded_plugins = g_list_delete_link (loader->priv->loaded_plugins, iter);
			break;
		}
	}
	*/

	return;
}

static void load_plugin (PhidiasLoader *loader, const gchar *plug_path)
{
	GObject *obj;
	PhidiasModule *module;

	module = phidias_module_new (plug_path);

	if (g_type_module_use (G_TYPE_MODULE (module)) == FALSE) {
		g_warning ("Unable to load module at %s.", plug_path);
		return;
	}

	obj = phidias_module_new_object (module);
	if (obj == NULL) {
		g_warning ("Unable to load module at %s.", plug_path);
	}
	else {
		loader->priv->loaded_plugins = g_list_prepend (loader->priv->loaded_plugins, obj);
		g_object_set_data_full (obj, "path", g_strdup (plug_path), g_free);
		g_signal_emit (loader, signals [PLUGIN_FOUND], 0, obj, NULL);
	}
}

static void load_plugins (PhidiasLoader *item)
{
	gchar *plug_path;
	gchar *plug_name;
	GError *error;
	GFile *plugins_folder;
	GFileEnumerator *enumerator;
	GFileInfo *plugin;

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
				load_plugin (item, (const gchar*) plug_path);
				g_free (plug_path);
			}

			g_free (plug_name);
			g_object_unref (plugin);
		}
	}

	g_object_unref (plugins_folder);
}

static void plugins_changed_cb (GFileMonitor *monitor, GFile *file, GFile *other_file,
                                GFileMonitorEvent event_type, gpointer user_data)
{
	gchar *path;
	PhidiasLoader *loader;

	path = g_file_get_path (file);
	loader = user_data;

	if (g_str_has_suffix (path, ".la") == TRUE) {
		switch (event_type) {
			case G_FILE_MONITOR_EVENT_DELETED:
				unload_plugin (loader, path);
				break;

			case G_FILE_MONITOR_EVENT_CREATED:
				load_plugin (loader, path);
				break;

			default:
				break;
		}
	}

	g_free (path);
}

static void run_monitor (PhidiasLoader *loader)
{
	loader->priv->monitor = g_file_monitor_directory (loader->priv->folder, G_FILE_MONITOR_NONE, NULL, NULL);
	g_signal_connect (loader->priv->monitor, "changed", G_CALLBACK (plugins_changed_cb), loader);
}

static void phidias_loader_finalize (GObject *obj)
{
	PhidiasLoader *loader;

	loader = PHIDIAS_LOADER (obj);

	if (loader->priv->path != NULL)
		g_free (loader->priv->path);

	if (loader->priv->monitor != NULL)
		g_object_unref (loader->priv->monitor);
	if (loader->priv->folder != NULL)
		g_object_unref (loader->priv->folder);
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
	gboolean monitor;
	PhidiasLoader *loader;

	loader = PHIDIAS_LOADER (object);

	switch (prop_id) {
		case PROP_PATH:
			monitor = FALSE;

			if (loader->priv->monitor != NULL) {
				monitor = TRUE;
				g_object_unref (loader->priv->monitor);
			}

			if (loader->priv->path != NULL) {
				g_free (loader->priv->path);
				g_object_unref (loader->priv->folder);
			}

			loader->priv->path = g_value_dup_string (value);
			loader->priv->folder = g_file_new_for_path (loader->priv->path);

			if (monitor == TRUE)
				run_monitor (loader);

			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void phidias_loader_plugin_removed (PhidiasLoader *loader, GObject *plug)
{
	g_object_unref (plug);
}

static void phidias_loader_class_init (PhidiasLoaderClass *klass)
{
	GObjectClass *gobject_class;

	g_type_class_add_private (klass, sizeof (PhidiasLoaderPrivate));

	gobject_class = G_OBJECT_CLASS (klass);
	gobject_class->finalize = phidias_loader_finalize;
	gobject_class->set_property = phidias_loader_set_property;
	gobject_class->get_property = phidias_loader_get_property;

	klass->plugin_removed = phidias_loader_plugin_removed;

	g_object_class_install_property (gobject_class, PROP_PATH,
			g_param_spec_string  ("path", "Path", "Plugins path", NULL,
				G_PARAM_READWRITE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_CONSTRUCT_ONLY));

	signals [PLUGIN_FOUND] = g_signal_new ("plugin-found",
			G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
			G_STRUCT_OFFSET (PhidiasLoaderClass, plugin_found), NULL, NULL,
			g_cclosure_marshal_VOID__OBJECT,
			G_TYPE_NONE, 1, G_TYPE_OBJECT);

	signals [PLUGIN_REMOVED] = g_signal_new ("plugin-removed",
			G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
			G_STRUCT_OFFSET (PhidiasLoaderClass, plugin_removed), NULL, NULL,
			g_cclosure_marshal_VOID__OBJECT,
			G_TYPE_NONE, 1, G_TYPE_OBJECT);
}

static void phidias_loader_init (PhidiasLoader *item)
{
	item->priv = PHIDIAS_LOADER_GET_PRIVATE (item);
	memset (item->priv, 0, sizeof (PhidiasLoaderPrivate));
}

PhidiasLoader* phidias_loader_new (gchar *folder_path)
{
	return g_object_new (PHIDIAS_LOADER_TYPE, "path", folder_path, NULL);
}

void phidias_loader_run (PhidiasLoader *loader)
{
	load_plugins (loader);
	run_monitor (loader);
}
