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

#include "phidias-shell.h"
#include "phidias-engine.h"
#include "phidias-channelview.h"
#include "phidias-views.h"
#include "phidias-extra-column.h"
#include "phidias-search-entry.h"
#include "phidias-items-viewer.h"
#include "logo.h"
#include <gdk/gdkkeysyms.h>

#define PHIDIAS_SHELL_GET_PRIVATE(obj)       (G_TYPE_INSTANCE_GET_PRIVATE ((obj), PHIDIAS_SHELL_TYPE, PhidiasShellPrivate))

struct _PhidiasShellPrivate {
	PhidiasEngine		*engine;
	GtkWidget		*channels;
	GtkWidget		*search;
	GtkWidget		*views;
};

G_DEFINE_TYPE (PhidiasShell, phidias_shell, GTK_TYPE_WINDOW);

static void change_selected_channel_cb (PhidiasChannelView *view, GtkTreeIter *selected, PhidiasShell *shell)
{
	phidias_engine_set_channel (shell->priv->engine, selected);
}

static void change_filtered_items_cb (PhidiasSearchEntry *search, const gchar *current_filter, PhidiasShell *shell)
{
	const gchar *filter;

	if (current_filter == NULL || strcmp (current_filter, "") == 0)
		filter = NULL;
	else
		filter = current_filter;

	phidias_engine_apply_filter (shell->priv->engine, filter);
}

static void change_shown_items_cb (PhidiasEngine *engine, PhidiasShell *shell)
{
	phidias_views_set_items (PHIDIAS_VIEWS (shell->priv->views), phidias_engine_get_current_model (engine));
}

static void wire_new_items_viewer_cb (PhidiasViews *view, PhidiasItemsViewer *viewer, PhidiasShell *item)
{
	int i;
	int index;
	const GPtrArray *extra;
	PhidiasExtraColumn *col;

	extra = phidias_items_viewer_get_extra_data (viewer);

	if (extra != NULL) {
		for (i = 0; i < extra->len; i++) {
			col = (PhidiasExtraColumn*) g_ptr_array_index (extra, i);
			index = phidias_engine_add_managed_info (item->priv->engine, col);
			phidias_extra_column_set_index (col, index);
		}
	}
}

static void item_update_required_cb (PhidiasViews *view, GtkTreeModel *model, GtkTreeIter *iter, int index, gchar *value, PhidiasShell *item)
{
	phidias_engine_update_item (item->priv->engine, model, iter, index, value);
}

static void phidias_shell_class_init (PhidiasShellClass *klass)
{
	g_type_class_add_private (klass, sizeof (PhidiasShellPrivate));
}

static void phidias_shell_init (PhidiasShell *item)
{
	GtkWidget *hor;
	GtkWidget *ver;

	item->priv = PHIDIAS_SHELL_GET_PRIVATE (item);
	memset (item->priv, 0, sizeof (PhidiasShellPrivate));

	gtk_window_set_icon (GTK_WINDOW (item), gdk_pixbuf_new_from_xpm_data (PhidiasIcon));

	item->priv->engine = phidias_engine_new ();

	hor = gtk_hpaned_new ();
	gtk_container_add (GTK_CONTAINER (item), hor);

	item->priv->channels = phidias_channelview_new ();
	gtk_paned_add1 (GTK_PANED (hor), item->priv->channels);

	ver = gtk_vbox_new (FALSE, 0);
	gtk_paned_add2 (GTK_PANED (hor), ver);

	item->priv->search = phidias_search_entry_new ();
	gtk_box_pack_start (GTK_BOX (ver), item->priv->search, FALSE, FALSE, 0);

	item->priv->views = phidias_views_new ();
	gtk_box_pack_end (GTK_BOX (ver), item->priv->views, TRUE, TRUE, 0);

	phidias_channelview_set_channels (PHIDIAS_CHANNELVIEW (item->priv->channels), phidias_engine_get_channels (item->priv->engine));
	g_signal_connect (item->priv->channels, "selected-channel-changed", G_CALLBACK (change_selected_channel_cb), item);
	g_signal_connect (item->priv->views, "new-available-view", G_CALLBACK (wire_new_items_viewer_cb), item);
	g_signal_connect (item->priv->views, "item-update-required", G_CALLBACK (item_update_required_cb), item);
	g_signal_connect (item->priv->search, "filter-changed", G_CALLBACK (change_filtered_items_cb), item);
	g_signal_connect (item->priv->engine, "channel-changed", G_CALLBACK (change_shown_items_cb), item);
}

GtkWidget* phidias_shell_new ()
{
	return g_object_new (PHIDIAS_SHELL_TYPE, "type", GTK_WINDOW_TOPLEVEL, NULL);
}
