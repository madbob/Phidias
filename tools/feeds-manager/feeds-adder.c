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

#include "feeds-adder.h"
#include "progress-box.h"

#define GET_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), FEEDS_ADDER_TYPE, FeedsAdderPrivate))

struct _FeedsAdderPrivate {
	TrackerSparqlConnection *tracker;

	GtkWidget *remote;
	GtkWidget *remote_progress;

	GtkWidget *local;
};

G_DEFINE_TYPE (FeedsAdder, feeds_adder, GTK_TYPE_BOX);

static void feeds_adder_finalize (GObject *obj)
{
	/*
		dummy
	*/
}

static void feeds_adder_class_init (FeedsAdderClass *klass)
{
	GObjectClass *gobject_class;

	gobject_class = G_OBJECT_CLASS (klass);
	gobject_class->finalize = feeds_adder_finalize;

	g_type_class_add_private (klass, sizeof (FeedsAdderPrivate));
}

static void free_list (GList *list)
{
	GList *iter;

	for (iter = list; iter; iter = iter->next)
		g_object_unref (iter->data);
	g_list_free (list);
}

static void create_tracker_item (FeedsAdder *app, GrssFeedChannel *channel)
{
	const gchar *str;
	TrackerSparqlBuilder *sparql;

	str = grss_feed_channel_get_source (channel);
	if (str == NULL) {
		g_warning ("Feed without source, unable to manage.");
		return;
	}

	sparql = tracker_sparql_builder_new_update ();

	tracker_sparql_builder_insert_open (sparql, "_:setts");

	tracker_sparql_builder_subject (sparql, "_:setts");
	tracker_sparql_builder_predicate (sparql, "a");
	tracker_sparql_builder_object (sparql, "mfo:FeedSettings");

	tracker_sparql_builder_subject (sparql, "_:feed");
	tracker_sparql_builder_predicate (sparql, "a");
	tracker_sparql_builder_object (sparql, "mfo:FeedChannel");
	tracker_sparql_builder_predicate (sparql, "a");
	tracker_sparql_builder_object (sparql, "nie:DataObject");

	tracker_sparql_builder_predicate (sparql, "nie:url");
	tracker_sparql_builder_object_unvalidated (sparql, str);

	str = grss_feed_channel_get_title (channel);
	if (str != NULL) {
		tracker_sparql_builder_predicate (sparql, "nie:title");
		tracker_sparql_builder_object_unvalidated (sparql, str);
	}

	tracker_sparql_builder_predicate (sparql, "mfo:feedSettings");
	tracker_sparql_builder_object (sparql, "_:setts");

	tracker_sparql_builder_insert_close (sparql);

	tracker_sparql_connection_update (app->priv->tracker, tracker_sparql_builder_get_result (sparql), 0, NULL, NULL);
	g_object_unref (sparql);
}

static void feed_fetched (GObject *source, GAsyncResult *res, gpointer user_data)
{
	GError *error;
	GrssFeedChannel *channel;
	FeedsAdder *app;

	error = NULL;
	g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), &error);

	if (error != NULL) {
		g_warning ("%s", error->message);
		g_error_free (error);
	}
	else {
		app = user_data;
		channel = GRSS_FEED_CHANNEL (source);
		create_tracker_item (app, channel);
	}

	g_object_unref (source);
}

static void handle_feeds_list (GList *list, FeedsAdder *app)
{
	GList *iter;

	for (iter = list; iter; iter = iter->next)
		grss_feed_channel_fetch_async (iter->data, feed_fetched, app);
}

static void remote_file_loaded (GObject *source_object, GAsyncResult *res, gpointer user_data)
{
	gchar *dest_path;
	GList *list;
	GError *error;
	GtkWidget *dialog;
	FeedsAdder *app;
	GrssFeedChannel *chan;
	GrssFeedsGroup *opml;

	error = NULL;
	app = user_data;

	if (g_file_copy_finish (G_FILE (source_object), res, &error) == FALSE) {
		dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Unable to fetch remote file.");
		gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog), "%s", error->message);
		gtk_widget_show_all (dialog);
		gtk_dialog_run (GTK_DIALOG (dialog));
		g_error_free (error);
		gtk_widget_destroy (dialog);
	}
	else {
		dest_path = g_build_filename (g_get_tmp_dir (), "feeds_manager_download", NULL);

		opml = grss_feeds_group_new ();
		list = grss_feeds_group_parse_file (opml, dest_path, NULL);

		if (list == NULL) {
			chan = grss_feed_channel_new_from_file (dest_path, NULL);
			grss_feed_channel_set_source (chan, g_file_get_uri (G_FILE (source_object)));
			create_tracker_item (app, chan);
		}
		else {
			handle_feeds_list (list, app);
			g_list_free (list);
		}

		g_object_unref (opml);

		remove (dest_path);
		g_free (dest_path);
	}

	progress_box_bar_mode (PROGRESS_BOX (app->priv->remote_progress), FALSE);
}

static void remote_loading_progress (goffset current_num_bytes, goffset total_num_bytes, gpointer user_data)
{
	ProgressBox *box;

	box = user_data;
	progress_box_set_fraction (box, (double) current_num_bytes / (double) total_num_bytes);
}

static void add_from_remote (GtkButton *button, FeedsAdder *app)
{
	const gchar *url;
	gchar *dest_path;
	GFile *file;
	GFile *dest;

	url = gtk_entry_get_text (GTK_ENTRY (app->priv->remote));
	if (strcmp (url, "") == 0)
		return;

	file = g_file_new_for_uri (url);

	dest_path = g_build_filename (g_get_tmp_dir (), "feeds_manager_download", NULL);
	dest = g_file_new_for_path (dest_path);
	g_free (dest_path);

	progress_box_bar_mode (PROGRESS_BOX (app->priv->remote_progress), TRUE);

	g_file_copy_async (file, dest, G_FILE_COPY_OVERWRITE, 0, NULL, remote_loading_progress,
			   PROGRESS_BOX (app->priv->remote_progress), remote_file_loaded, app);
}

static void add_from_local (GtkButton *button, FeedsAdder *app)
{
	gchar *path;
	GList *list;
	GrssFeedsGroup *opml;

	opml = grss_feeds_group_new ();

	path = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (app->priv->local));
	if (path == NULL)
		return;

	list = grss_feeds_group_parse_file (opml, path, NULL);
	if (list != NULL) {
		handle_feeds_list (list, app);
		g_list_free (list);
	}

	g_object_unref (opml);
	g_free (path);
}

static void feeds_adder_init (FeedsAdder *app)
{
	GtkWidget *row;
	GtkWidget *button;

	gtk_orientable_set_orientation (GTK_ORIENTABLE (app), GTK_ORIENTATION_VERTICAL);
	app->priv = GET_PRIV (app);

	row = progress_box_new ();
	app->priv->remote = gtk_entry_new ();
	button = gtk_button_new_from_stock (GTK_STOCK_APPLY);
	g_signal_connect (button, "clicked", G_CALLBACK (add_from_remote), app);
	gtk_box_pack_start (GTK_BOX (app), row, TRUE, TRUE, 0);
	progress_box_pack_start (PROGRESS_BOX (row), gtk_label_new ("Paste here URL for a feed or OPML/XOXO/XBEL remote file: "), FALSE, FALSE, 0);
	progress_box_pack_start (PROGRESS_BOX (row), app->priv->remote, TRUE, TRUE, 0);
	progress_box_pack_start (PROGRESS_BOX (row), button, FALSE, FALSE, 0);
	app->priv->remote_progress = row;

	gtk_box_pack_start (GTK_BOX (app), gtk_label_new ("OR"), TRUE, TRUE, 0);

	row = gtk_hbox_new (FALSE, 0);
	app->priv->local = gtk_file_chooser_button_new (NULL, GTK_FILE_CHOOSER_ACTION_OPEN);
	button = gtk_button_new_from_stock (GTK_STOCK_APPLY);
	g_signal_connect (button, "clicked", G_CALLBACK (add_from_local), app);
	gtk_box_pack_start (GTK_BOX (app), row, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (row), gtk_label_new ("Select a local OPML/XOXO/XBEL file to import: "), FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (row), app->priv->local, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (row), button, FALSE, FALSE, 0);
}

GtkWidget* feeds_adder_new ()
{
	return g_object_new (FEEDS_ADDER_TYPE, NULL);
}

void feeds_adder_wire_tracker (FeedsAdder *adder, TrackerSparqlConnection *tracker)
{
	adder->priv->tracker = tracker;
}
