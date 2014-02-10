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

#include "phidias-items-classic.h"
#include "phidias-items-viewer.h"
#include "expander.h"
#include <webkit/webkit.h>

#define PHIDIAS_ITEMS_CLASSIC_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), PHIDIAS_ITEMS_CLASSIC_TYPE, PhidiasItemsClassicPrivate))

struct _PhidiasItemsClassicPrivate {
	GtkWidget	*list;
	GtkWidget	*description;
};

static GType type;

static void item_selected_cb (GtkTreeSelection *treeselection, PhidiasItemsClassic *item)
{
	gchar *desc;
	gchar *is_read;
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkTreeIter original_iter;

	if (gtk_tree_selection_get_selected (treeselection, &model, &iter) == TRUE) {
		gtk_tree_model_get (model, &iter, ITEM_INFO_READ, &is_read, ITEM_INFO_DESCRIPTION, &desc, -1);
		webkit_web_view_load_html_string (WEBKIT_WEB_VIEW (item->priv->description), desc, "file://");

		if (strcmp (is_read, "false") == 0) {
			gtk_tree_model_sort_convert_iter_to_child_iter (GTK_TREE_MODEL_SORT (model), &original_iter, &iter);
			g_signal_emit_by_name (item, "item-update-required", &original_iter, ITEM_INFO_READ, "true");
		}

		g_free (is_read);
		g_free (desc);
	}
}

static void set_line_style (GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data)
{
	gchar *is_read;

	gtk_tree_model_get (tree_model, iter, ITEM_INFO_READ, &is_read, -1);
	if (is_read == NULL)
		return;

	if (strcmp (is_read, "true") == 0)
		g_object_set (cell, "weight", PANGO_WEIGHT_NORMAL, NULL);
	else
		g_object_set (cell, "weight", PANGO_WEIGHT_BOLD, NULL);

	g_free (is_read);
}

static const gchar* phidias_items_classic_get_name (PhidiasItemsViewer *self)
{
	return "Classic";
}

static gint sort_items (GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer user_data)
{
	int ret;
	gchar *vala;
	gchar *valb;
	time_t timea;
	time_t timeb;
	struct tm da;
	struct tm db;

	/**
		TODO	Odd parsing strategy, odd reiterate parsing: this has to be modified as
			soon as the model will be populated with already traslated datatypes
	*/

	gtk_tree_model_get (model, a, ITEM_INFO_DATE, &vala, -1);
	gtk_tree_model_get (model, b, ITEM_INFO_DATE, &valb, -1);

	if (vala == NULL && valb == NULL)
		return 0;
	else if (vala == NULL)
		return -1;
	else if (valb == NULL)
		return 1;

	memset (&da, 0, sizeof (struct tm));
	strptime (vala, "%Y-%m-%dT%H:%M:%SZ", &da);
	timea = mktime (&da);
	g_free (vala);

	memset (&db, 0, sizeof (struct tm));
	strptime (valb, "%Y-%m-%dT%H:%M:%SZ", &db);
	timeb = mktime (&db);
	g_free (valb);

	if (timea < timeb) {
		ret = -1;
	}
	else if (timea > timeb) {
		ret = 1;
	}
	else {
		/*
			On same date, we sort by title. Just to have some unique criteria...
		*/
		gtk_tree_model_get (model, a, ITEM_INFO_TITLE, &vala, -1);
		gtk_tree_model_get (model, b, ITEM_INFO_TITLE, &valb, -1);
		ret = strcmp (vala, valb);
		g_free (vala);
		g_free (valb);
	}

	return ret;
}

static void scroll_to_position (PhidiasItemsClassic *view)
{
	GtkAdjustment *adj;

	/**
		TODO	Save a status for each submitted model, so to be able
			to return to the original position
	*/

	adj = gtk_tree_view_get_vadjustment (GTK_TREE_VIEW (view->priv->list));
	if (adj != NULL) {
		gtk_adjustment_set_value (adj, 0.0);
		gtk_tree_view_set_vadjustment (GTK_TREE_VIEW (view->priv->list), adj);
	}
}

static void phidias_items_classic_set_model (PhidiasItemsViewer *self, GtkTreeModel *items)
{
	GtkTreeModel *sorter;
	PhidiasItemsClassic *item;

	item = PHIDIAS_ITEMS_CLASSIC (self);
	webkit_web_view_load_html_string (WEBKIT_WEB_VIEW (item->priv->description), "", "file://");

	sorter = gtk_tree_view_get_model (GTK_TREE_VIEW (item->priv->list));
	if (sorter != NULL)
		g_object_unref (sorter);

	sorter = gtk_tree_model_sort_new_with_model (items);
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (sorter), ITEM_INFO_DATE, GTK_SORT_DESCENDING);
	gtk_tree_sortable_set_sort_func (GTK_TREE_SORTABLE (sorter), ITEM_INFO_DATE, sort_items, NULL, NULL);
	gtk_tree_view_set_model (GTK_TREE_VIEW (item->priv->list), sorter);

	scroll_to_position (item);
}

static void phidias_items_viewer_interface_init (PhidiasItemsViewerInterface *iface)
{
	iface->get_name = phidias_items_classic_get_name;
	iface->set_model = phidias_items_classic_set_model;
}

static void phidias_items_classic_finalize (GObject *object)
{
}

static void phidias_items_classic_class_init (PhidiasItemsClassicClass *klass)
{
	GObjectClass *gobject_class;

	g_type_class_add_private (klass, sizeof (PhidiasItemsClassicPrivate));

	gobject_class = G_OBJECT_CLASS (klass);
	gobject_class->finalize = phidias_items_classic_finalize;
}

static void phidias_items_classic_init (PhidiasItemsClassic *item)
{
	GtkWidget *scroll;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *col;
	GtkTreeSelection *selection;

	item->priv = PHIDIAS_ITEMS_CLASSIC_GET_PRIVATE (item);

	item->priv->list = gtk_tree_view_new ();
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (item->priv->list), FALSE);

	renderer = gtk_cell_renderer_text_new ();
	col = gtk_tree_view_column_new_with_attributes ("Title", renderer, "text", ITEM_INFO_TITLE, NULL);
	gtk_tree_view_column_set_cell_data_func (col, renderer, set_line_style, NULL, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (item->priv->list), col);

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (item->priv->list));
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);
	g_signal_connect (selection, "changed", G_CALLBACK (item_selected_cb), item);

	scroll = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scroll), item->priv->list);
	gtk_paned_add1 (GTK_PANED (item), scroll);

	item->priv->description = webkit_web_view_new ();
	scroll = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scroll), item->priv->description);
	gtk_paned_add2 (GTK_PANED (item), scroll);
}

GType register_module (GTypeModule *module)
{
	static const GTypeInfo our_info = {
		sizeof (PhidiasItemsClassicClass),
		NULL, NULL,
		(GClassInitFunc) phidias_items_classic_class_init,
		NULL, NULL,
		sizeof (PhidiasItemsClassic), 0,
		(GInstanceInitFunc) phidias_items_classic_init
	};

	static const GInterfaceInfo extension_info = {
		(GInterfaceInitFunc) phidias_items_viewer_interface_init,
		NULL, NULL
	};

	type = g_type_module_register_type (module, GTK_TYPE_VPANED, "PhidiasItemsClassic", &our_info, 0);
	g_type_module_add_interface (module, type, PHIDIAS_ITEMS_VIEWER_TYPE, &extension_info);

	return type;
}

GType phidias_items_classic_get_type ()
{
	return type;
}
