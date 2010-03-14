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

#include "phidias-views.h"
#include "phidias-loader.h"
#include "phidias-items-viewer.h"
#include "phidias-marshal.h"

#define PHIDIAS_VIEWS_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), PHIDIAS_VIEWS_TYPE, PhidiasViewsPrivate))

struct _PhidiasViewsPrivate {
	GtkTreeModel	*current_model;
	gboolean	*model_updates;
};

enum {
	NEW_AVAILABLE_VIEW,
	ITEM_UPDATE_REQUIRED,
	LAST_SIGNAL
};

static guint signals [LAST_SIGNAL];

G_DEFINE_TYPE (PhidiasViews, phidias_views, GTK_TYPE_NOTEBOOK);

static void update_model_to_current_view (PhidiasViews *view, int page)
{
	GtkWidget *plug;

	plug = gtk_notebook_get_nth_page (GTK_NOTEBOOK (view), page);

	if (view->priv->current_model != NULL)
		phidias_items_viewer_set_model (PHIDIAS_ITEMS_VIEWER (plug), view->priv->current_model);
}

static void viewer_changed_cb (GtkNotebook *notebook, GtkNotebookPage *page, guint index, gpointer user_data)
{
	PhidiasViews *view;

	view = user_data;

	if (view->priv->model_updates == NULL)
		return;

	if (view->priv->model_updates [index] == FALSE) {
		update_model_to_current_view (view, index);
		view->priv->model_updates [index] = TRUE;
	}
}

static void update_required (PhidiasItemsViewer *viewer, GtkTreeIter *iter, int index, gchar *value, PhidiasViews *item)
{
	gchar *original_value;

	if (index == 0) {
		g_warning ("Invalid update request.");
		return;
	}

	original_value = NULL;
	gtk_tree_model_get (item->priv->current_model, iter, index, &original_value, -1);

	if (original_value != NULL) {
		/*
			If the required value is already set, we avoid to propagate the signal
		*/
		if (strcmp (value, original_value) != 0)
			g_signal_emit (item, signals [ITEM_UPDATE_REQUIRED], 0, item->priv->current_model, iter, index, value, NULL);

		g_free (original_value);
	}
}

static void new_plugin_cb (PhidiasLoader *loader, GObject *plugin, PhidiasViews *item)
{
	int index;
	PhidiasItemsViewer *plug;

	plug = PHIDIAS_ITEMS_VIEWER (plugin);
	g_signal_connect (plug, "item-update-required", G_CALLBACK (update_required), item);

	gtk_notebook_append_page (GTK_NOTEBOOK (item), GTK_WIDGET (plugin), gtk_label_new (phidias_items_viewer_get_name (plug)));
	gtk_widget_show_all (GTK_WIDGET (plugin));

	index = gtk_notebook_get_n_pages (GTK_NOTEBOOK (item));
	item->priv->model_updates = realloc (item->priv->model_updates, sizeof (gboolean) * index);

	g_signal_emit (item, signals [NEW_AVAILABLE_VIEW], 0, plug, NULL);
}

static void removed_plugin_cb (PhidiasLoader *loader, GObject *plugin, PhidiasViews *item)
{
	gtk_container_remove (GTK_CONTAINER (item), GTK_WIDGET (plugin));
}

static gboolean init_plugins_loader (gpointer data)
{
	gchar *path;
	PhidiasLoader *loader;
	PhidiasViews *item;

	item = data;

	path = g_build_filename (PLUGIN_DIR, "items", NULL);
	loader = phidias_loader_new (path);
	g_signal_connect (loader, "plugin-found", G_CALLBACK (new_plugin_cb), item);
	g_signal_connect (loader, "plugin-removed", G_CALLBACK (removed_plugin_cb), item);
	g_free (path);
	phidias_loader_run (loader);
	return FALSE;
}

static void phidias_views_class_init (PhidiasViewsClass *klass)
{
	g_type_class_add_private (klass, sizeof (PhidiasViewsPrivate));

	signals [NEW_AVAILABLE_VIEW] = g_signal_new ("new-available-view",
				G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
				0, NULL, NULL,
				g_cclosure_marshal_VOID__OBJECT,
				G_TYPE_NONE, 1, PHIDIAS_ITEMS_VIEWER_TYPE);

	signals [ITEM_UPDATE_REQUIRED] = g_signal_new ("item-update-required",
				G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
				0, NULL, NULL,
				phidias_marshal_VOID__OBJECT_POINTER_INT_STRING,
				G_TYPE_NONE, 4, GTK_TYPE_TREE_MODEL, G_TYPE_POINTER, G_TYPE_INT, G_TYPE_STRING);
}

static void phidias_views_init (PhidiasViews *item)
{
	item->priv = PHIDIAS_VIEWS_GET_PRIVATE (item);

	item->priv->model_updates = NULL;
	g_signal_connect (item, "switch-page", G_CALLBACK (viewer_changed_cb), item);

	/*
		Plugins are loaded at the end of initialization, when, already in the mainloop,
		to permit PhidiasShell to listen "new-available-view" signals and wire the
		components to PhidiasEngine
	*/
	g_idle_add (init_plugins_loader, item);
}

GtkWidget* phidias_views_new ()
{
	return g_object_new (PHIDIAS_VIEWS_TYPE, NULL);
}

void phidias_views_set_items (PhidiasViews *views, GtkTreeModel *items)
{
	int selected;

	views->priv->current_model = items;

	memset (views->priv->model_updates, 0, sizeof (gboolean) * gtk_notebook_get_n_pages (GTK_NOTEBOOK (views)));

	selected = gtk_notebook_get_current_page (GTK_NOTEBOOK (views));
	update_model_to_current_view (views, selected);
	views->priv->model_updates [selected] = TRUE;
}
