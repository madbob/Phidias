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

#include "phidias-channelview.h"
#include "phidias-loader.h"
#include "phidias-channels-viewer.h"

#define PHIDIAS_CHANNELVIEW_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), PHIDIAS_CHANNELVIEW_TYPE, PhidiasChannelViewPrivate))

struct _PhidiasChannelViewPrivate {
	GtkWidget	*selector;
	GtkTreeModel	*channels_model;
	GList		*plugins;

	GtkWidget	*viewer_frame;
};

enum {
	CHANNEL_CHANGED,
	LAST_SIGNAL
};

static guint signals [LAST_SIGNAL];

G_DEFINE_TYPE (PhidiasChannelView, phidias_channelview, GTK_TYPE_VBOX);

static void viewer_changed_cb (GtkComboBox *widget, PhidiasChannelView *item)
{
	int index;
	GList *iter;
	GtkWidget *current;
	PhidiasChannelsViewer *plug;

	index = gtk_combo_box_get_active (widget);

	iter = g_list_nth (item->priv->plugins, index);
	if (iter == NULL) {
		g_warning ("Unable to select the viewer.");
		return;
	}

	plug = iter->data;

	current = gtk_bin_get_child (GTK_BIN (item->priv->viewer_frame));
	if (current != GTK_WIDGET (plug)) {
		if (current != NULL)
			gtk_container_remove (GTK_CONTAINER (item->priv->viewer_frame), current);
		gtk_container_add (GTK_CONTAINER (item->priv->viewer_frame), GTK_WIDGET (plug));
	}
}

static void selected_channel_changed_cb (PhidiasChannelsViewer *plug, GtkTreeIter *selected, gpointer userdata)
{
	PhidiasChannelView *item;

	item = userdata;
	g_signal_emit (item, signals [CHANNEL_CHANGED], 0, selected, NULL);
}

static void new_plugin_cb (PhidiasLoader *loader, GObject *plugin, PhidiasChannelView *item)
{
	PhidiasChannelsViewer *plug;

	plug = PHIDIAS_CHANNELS_VIEWER (plugin);
	g_signal_connect (plug, "channel-changed", G_CALLBACK (selected_channel_changed_cb), item);
	item->priv->plugins = g_list_append (item->priv->plugins, plug);
	gtk_combo_box_append_text (GTK_COMBO_BOX (item->priv->selector), phidias_channels_viewer_get_name (plug));

	if (gtk_bin_get_child (GTK_BIN (item->priv->viewer_frame)) == NULL)
		gtk_combo_box_set_active (GTK_COMBO_BOX (item->priv->selector), 0);
}

static void init_plugins_loader (PhidiasChannelView *item)
{
	gchar *path;
	PhidiasLoader *loader;

	path = g_build_filename (PLUGIN_DIR, "channels", NULL);
	loader = phidias_loader_new (path);
	g_signal_connect (loader, "plugin-found", G_CALLBACK (new_plugin_cb), item);
	g_free (path);
	phidias_loader_run (loader);
}

static void phidias_channelview_finalize (GObject *obj)
{
	/**
		TODO
	*/
}

static void phidias_channelview_class_init (PhidiasChannelViewClass *klass)
{
	GObjectClass *gobject_class;

	g_type_class_add_private (klass, sizeof (PhidiasChannelViewPrivate));

	gobject_class = G_OBJECT_CLASS (klass);
	gobject_class->finalize = phidias_channelview_finalize;

	signals [CHANNEL_CHANGED] = g_signal_new ("selected-channel-changed",
				G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
				0, NULL, NULL,
				g_cclosure_marshal_VOID__POINTER,
				G_TYPE_NONE, 1, G_TYPE_POINTER);
}

static void phidias_channelview_init (PhidiasChannelView *item)
{
	GtkWidget *scroll;

	item->priv = PHIDIAS_CHANNELVIEW_GET_PRIVATE (item);

	item->priv->selector = gtk_combo_box_new_text ();
	gtk_box_pack_start (GTK_BOX (item), item->priv->selector, FALSE, FALSE, 0);
	g_signal_connect (item->priv->selector, "changed", G_CALLBACK (viewer_changed_cb), item);

	scroll = gtk_scrolled_window_new (NULL, NULL);
	gtk_box_pack_end (GTK_BOX (item), scroll, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	item->priv->viewer_frame = gtk_viewport_new (NULL, NULL);
	gtk_container_add (GTK_CONTAINER (scroll), item->priv->viewer_frame);

	init_plugins_loader (item);
}

GtkWidget* phidias_channelview_new ()
{
	return g_object_new (PHIDIAS_CHANNELVIEW_TYPE, NULL);
}

void phidias_channelview_set_channels (PhidiasChannelView *view, GtkTreeModel *channels)
{
	GList *iter;
	PhidiasChannelsViewer *plug;

	view->priv->channels_model = channels;

	for (iter = view->priv->plugins; iter; iter = g_list_next (iter)) {
		plug = (PhidiasChannelsViewer*) iter->data;
		phidias_channels_viewer_set_channels (plug, channels);
	}
}
