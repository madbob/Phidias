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

#include "phidias-channels-by-name.h"
#include "phidias-channels-viewer.h"
#include "expander.h"

#define PHIDIAS_CHANNELS_BY_NAME_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), PHIDIAS_CHANNELS_BY_NAME_TYPE, PhidiasChannelsByNamePrivate))

struct _PhidiasChannelsByNamePrivate {
	int	dummy;
};

static GType type;

static void item_selected_cb (GtkTreeSelection *selection, PhidiasChannelsByName *item)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkTreeIter original_iter;

	if (gtk_tree_selection_get_selected (selection, &model, &iter) == TRUE) {
		gtk_tree_model_sort_convert_iter_to_child_iter (GTK_TREE_MODEL_SORT (model), &original_iter, &iter);
		g_signal_emit_by_name (item, "channel-changed", &original_iter);
	}
}

static const gchar* phidias_channels_by_name_get_name (PhidiasChannelsViewer *self)
{
	return "Sort by Name";
}

static gint sort_channels (GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer user_data)
{
	int ret;
	gchar *titlea;
	gchar *titleb;

	gtk_tree_model_get (model, a, CONTAINER_INFO_TITLE, &titlea, -1);
	gtk_tree_model_get (model, b, CONTAINER_INFO_TITLE, &titleb, -1);

	if (titlea == NULL && titleb == NULL)
		return 0;
	else if (titlea == NULL)
		return 1;
	else if (titleb == NULL)
		return -1;

	ret = strcmp (titlea, titleb);
	g_free (titlea);
	g_free (titleb);
	return ret;
}

static void phidias_channels_by_name_set_channels (PhidiasChannelsViewer *view, GtkTreeModel *channels)
{
	GtkTreeModel *sorter;

	sorter = gtk_tree_view_get_model (GTK_TREE_VIEW (view));
	if (sorter != NULL)
		g_object_unref (sorter);

	sorter = gtk_tree_model_sort_new_with_model (channels);
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (sorter), CONTAINER_INFO_TITLE, GTK_SORT_ASCENDING);
	gtk_tree_sortable_set_sort_func (GTK_TREE_SORTABLE (sorter), CONTAINER_INFO_TITLE, sort_channels, NULL, NULL);

	gtk_tree_view_set_model (GTK_TREE_VIEW (view), sorter);
}

static void phidias_channels_viewer_interface_init (PhidiasChannelsViewerInterface *iface)
{
	iface->get_name = phidias_channels_by_name_get_name;
	iface->set_channels = phidias_channels_by_name_set_channels;
}

static void phidias_channels_by_name_finalize (GObject *object)
{
	/**
		TODO
	*/
}

static void phidias_channels_by_name_class_init (PhidiasChannelsByNameClass *klass)
{
	GObjectClass *gobject_class;

	g_type_class_add_private (klass, sizeof (PhidiasChannelsByNamePrivate));

	gobject_class = G_OBJECT_CLASS (klass);
	gobject_class->finalize = phidias_channels_by_name_finalize;
}

static void phidias_channels_by_name_init (PhidiasChannelsByName *item)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *col;
	GtkTreeSelection *selection;

	item->priv = PHIDIAS_CHANNELS_BY_NAME_GET_PRIVATE (item);

	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (item), FALSE);

	renderer = gtk_cell_renderer_text_new ();
	col = gtk_tree_view_column_new_with_attributes ("Title", renderer, "text", CONTAINER_INFO_TITLE, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (item), col);

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (item));
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_BROWSE);
	g_signal_connect (selection, "changed", G_CALLBACK (item_selected_cb), item);
}

GType register_module (GTypeModule *module)
{
	static const GTypeInfo our_info = {
		sizeof (PhidiasChannelsByNameClass),
		NULL, NULL,
		(GClassInitFunc) phidias_channels_by_name_class_init,
		NULL, NULL,
		sizeof (PhidiasChannelsByName), 0,
		(GInstanceInitFunc) phidias_channels_by_name_init
	};

	static const GInterfaceInfo extension_info = {
		(GInterfaceInitFunc) phidias_channels_viewer_interface_init,
		NULL, NULL
	};

	type = g_type_module_register_type (module, GTK_TYPE_TREE_VIEW, "PhidiasChannelsByName", &our_info, 0);
	g_type_module_add_interface (module, type, PHIDIAS_CHANNELS_VIEWER_TYPE, &extension_info);

	return type;
}

GType phidias_channels_by_name_get_type ()
{
	return type;
}
