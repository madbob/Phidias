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

#include "common.h"
#include "marshal.h"
#include "feed-settings.h"
#include "feeds-adder.h"

static gboolean exit_all (GtkWidget *widget, GdkEvent *event, GtkWidget *setts)
{
	feed_settings_set_id (FEED_SETTINGS (setts), NULL);
	gtk_main_quit ();
	return FALSE;
}

static void feed_selected_cb (GtkTreeSelection *treeselection, FeedSettings *setts)
{
	gboolean download;
	gchar *id;
	gchar *path;
	int maxsize;
	int expiry;
	GtkTreeModel *model;
	GtkTreeIter iter;

	if (gtk_tree_selection_get_selected (treeselection, &model, &iter) == TRUE) {
		gtk_tree_model_get (model, &iter, 0, &id, 2, &download, 3, &path, 4, &maxsize, 5, &expiry, -1);

		/*
			feed_settings_set_id() has to be called before every other related function
		*/
		feed_settings_set_id (setts, id);
		feed_settings_set_download_enclosures (setts, download);
		feed_settings_set_download_path (setts, path);
		feed_settings_set_enclosures_maxsize (setts, maxsize);
		feed_settings_set_autoexpiry_interval (setts, expiry);

		g_free (id);
		g_free (path);
	}
}

static void fill_model_with_channels (GObject *source_object, GAsyncResult *res, gpointer user_data)
{
	int i;
	GtkWidget *dialog;
	GtkListStore *list;
	TrackerSparqlCursor *cursor;
	GError *error;

	error = NULL;
	cursor = tracker_sparql_connection_query_finish (TRACKER_SPARQL_CONNECTION (source_object), res, &error);

	if (error != NULL) {
		dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Unable to fetch data about current feeds:\n%s", error->message);
		gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog), "Is Tracker running?");
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);
		g_error_free (error);
		return;
	}

	list = user_data;

	while (tracker_sparql_cursor_next (cursor, NULL, NULL)) {
		gtk_list_store_insert_with_values (list, NULL, G_MAXINT,
						   PREDICATE_SUBJECT, tracker_sparql_cursor_get_string (cursor, 0, NULL),
						   PREDICATE_NAME, tracker_sparql_cursor_get_string (cursor, 1, NULL),
						   PREDICATE_DOWNLOAD, tracker_sparql_cursor_get_boolean (cursor, 2),
						   PREDICATE_DOWNPATH, tracker_sparql_cursor_get_string (cursor, 3, NULL),
						   PREDICATE_MAXSIZE, tracker_sparql_cursor_get_double (cursor, 4),
						   PREDICATE_EXPIRY, tracker_sparql_cursor_get_double (cursor, 5),
						   6, FALSE, -1);
	}
}

static gboolean retrive_row_by_id (GtkTreeModel *model, gchar *id, GtkTreeIter *iter)
{
	gchar *cmp;
	gboolean found;

	if (gtk_tree_model_get_iter_first (model, iter) == FALSE)
		return FALSE;

	found = FALSE;

	do {
		gtk_tree_model_get (model, iter, PREDICATE_SUBJECT, &cmp, -1);
		if (strcmp (id, cmp) == 0)
			found = TRUE;

		g_free (cmp);

	} while (found == FALSE && gtk_tree_model_iter_next (model, iter) == TRUE);

	return found;
}

static void feed_removed (FeedSettings *setts, gchar *id, TrackerSparqlConnection *tracker_client)
{
	TrackerSparqlBuilder *sparql;

	sparql = tracker_sparql_builder_new_update ();

	tracker_sparql_builder_delete_open (sparql, NULL);
	tracker_sparql_builder_subject_iri (sparql, id);
	tracker_sparql_builder_predicate (sparql, "a");
	tracker_sparql_builder_object (sparql, "rdfs:Resource");
	tracker_sparql_builder_delete_close (sparql);
	tracker_sparql_builder_where_open (sparql);
	tracker_sparql_builder_subject_iri (sparql, id);
	tracker_sparql_builder_predicate (sparql, "a");
	tracker_sparql_builder_object (sparql, "rdfs:Resource");
	tracker_sparql_builder_where_close (sparql);

	tracker_sparql_connection_update (tracker_client, tracker_sparql_builder_get_result (sparql), 0, NULL, NULL);

	g_object_unref (sparql);
}

static void feed_saved (FeedSettings *setts, gchar *sparql, TrackerSparqlConnection *tracker_client)
{
	tracker_sparql_connection_update (tracker_client, sparql, 0, NULL, NULL);
}

static void feed_modified (FeedSettings *setts, gchar *id, PREDICATE_INDEX predicate, GValue *value, GtkListStore *model)
{
	GtkTreeIter iter;

	if (retrive_row_by_id (GTK_TREE_MODEL (model), id, &iter) == TRUE) {
		switch (predicate) {
			case PREDICATE_NAME:
			case PREDICATE_DOWNPATH:
				gtk_list_store_set (model, &iter, predicate, g_value_get_string (value), -1);
				break;

			case PREDICATE_DOWNLOAD:
				gtk_list_store_set (model, &iter, predicate, g_value_get_boolean (value), -1);
				break;

			case PREDICATE_MAXSIZE:
			case PREDICATE_EXPIRY:
				gtk_list_store_set (model, &iter, predicate, g_value_get_int (value), -1);
				break;

			default:
				break;
		}
	}
}

#if 0

static void channel_added_cb (DBusGProxy *proxy, gchar **subjects, GtkListStore *model)
{
	int i;

	for (i = 0; subjects [i] != NULL; i++) {
		gtk_list_store_insert_with_values (model, NULL, G_MAXINT,
						   PREDICATE_SUBJECT, subjects [i],
						   PREDICATE_NAME, "Feed added... Loading...",
						   6, TRUE, -1);
	}
}

static void channel_changed_cb (DBusGProxy *proxy, gchar **subjects, gchar **predicates, GtkListStore *model)
{
	int i;
	gchar *id;
	GtkTreeIter iter;

	if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (model), &iter) == FALSE)
		return;

	do {
		gtk_tree_model_get (GTK_TREE_MODEL (model), &iter, PREDICATE_SUBJECT, &id, -1);

		for (i = 0; subjects [i] != NULL; i++) {
			if (strcmp (subjects [i], id) == 0) {
				gtk_list_store_set (model, &iter, PREDICATE_NAME, "Feed modified... Loading...", 6, TRUE, -1);
				break;
			}
		}

		g_free (id);

	} while (gtk_tree_model_iter_next (GTK_TREE_MODEL (model), &iter) == TRUE);
}

static gboolean seek_and_destroy (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data)
{
	gboolean ret;
	gchar *sub;

	ret = FALSE;
	gtk_tree_model_get (model, iter, PREDICATE_SUBJECT, &sub, -1);

	if (strcmp (data, sub) == 0) {
		gtk_list_store_remove (GTK_LIST_STORE (model), iter);
		ret = TRUE;
	}

	g_free (sub);
	return ret;
}

static void channel_removed_cb (DBusGProxy *proxy, gchar **subjects, GtkListStore *model)
{
	int i;

	for (i = 0; subjects [i] != NULL; i++)
		gtk_tree_model_foreach (GTK_TREE_MODEL (model), seek_and_destroy, subjects [i]);
}

#endif

static void init_model (TrackerSparqlConnection *tracker, GtkListStore *model)
{
	gchar *query;

	query = "SELECT ?item ?name ?downenclosures ?enclosurespath ?enclosuressize ?expiry "
			"WHERE {?item a mfo:FeedChannel . "
				"?item nie:title ?name . "
				"?item mfo:feedSettings ?setts . "
				"OPTIONAL {?setts mfo:downloadFlag ?downenclosures} . "
				"OPTIONAL {?setts mfo:downloadPath ?enclosurespath} . "
				"OPTIONAL {?setts mfo:maxSize ?enclosuressize} . "
				"OPTIONAL {?setts mfo:expiryInterval ?expiry}}";

	tracker_sparql_connection_query_async (tracker, query, NULL, fill_model_with_channels, model);
}

static void find_channel_and_update (GObject *source_object, GAsyncResult *res, gpointer user_data)
{
	gchar **values;
	GtkTreeModel *model;
	GtkTreeIter iter;
	TrackerSparqlCursor *cursor;
	GError *error;

	error = NULL;
	cursor = tracker_sparql_connection_query_finish (TRACKER_SPARQL_CONNECTION (source_object), res, &error);

	if (error != NULL) {
		g_warning ("Unable to update channel: %s", error->message);
		g_error_free (error);
		return;
	}

	model = user_data;

	while (tracker_sparql_cursor_next (cursor, NULL, NULL)) {
		if (retrive_row_by_id (model, tracker_sparql_cursor_get_string (cursor, 0, NULL), &iter) == TRUE) {
			gtk_list_store_set (GTK_LIST_STORE (model), &iter,
					    PREDICATE_NAME, tracker_sparql_cursor_get_string (cursor, 1, NULL),
					    PREDICATE_DOWNLOAD, tracker_sparql_cursor_get_boolean (cursor, 2),
					    PREDICATE_DOWNPATH, tracker_sparql_cursor_get_string (cursor, 3, NULL),
					    PREDICATE_MAXSIZE, tracker_sparql_cursor_get_double (cursor, 4),
					    PREDICATE_EXPIRY, tracker_sparql_cursor_get_double (cursor, 5),
					    6, FALSE, -1);
		}
	}
}

static void row_added_in_model (GtkTreeModel *tree_model, GtkTreePath *path, GtkTreeIter *iter, TrackerSparqlConnection *tracker_client)
{
	gchar *id;
	gchar *query;
	gboolean update;

	gtk_tree_model_get (tree_model, iter, PREDICATE_SUBJECT, &id, 6, &update, -1);

	if (update == FALSE)
		return;

	query = g_strdup_printf ("SELECT <%s> ?name ?downenclosures ?enclosurespath ?enclosuressize ?expiry "
						"WHERE {<%s> nie:title ?name . "
							"<%s> mfo:feedSettings ?setts . "
							"OPTIONAL {?setts mfo:downloadFlag ?downenclosures} . "
							"OPTIONAL {?setts mfo:downloadPath ?enclosurespath} . "
							"OPTIONAL {?setts mfo:maxSize ?enclosuressize} . "
							"OPTIONAL {?setts mfo:expiryInterval ?expiry}}", id, id, id);

	tracker_sparql_connection_query_async (tracker_client, query, NULL, find_channel_and_update, tree_model);

	g_free (id);
	g_free (query);
}

static void tune_model (TrackerSparqlConnection *tracker_client, GtkListStore *model)
{
	g_signal_connect (model, "row-inserted", G_CALLBACK (row_added_in_model), tracker_client);
	g_signal_connect (model, "row-changed", G_CALLBACK (row_added_in_model), tracker_client);
}

static void listen_tracker (GtkListStore *model)
{
#if 0

	DBusGConnection *bus;
	DBusGProxy *wrap;

	bus = dbus_g_bus_get (DBUS_BUS_SESSION, NULL);
	dbus_g_object_register_marshaller (marshal_VOID__BOXED_BOXED, G_TYPE_NONE, G_TYPE_STRV, G_TYPE_STRV, G_TYPE_INVALID);

	wrap = dbus_g_proxy_new_for_name (bus,
					  "org.freedesktop.Tracker1",
					  "/org/freedesktop/Tracker1/Resources/Classes/mfo/FeedChannel",
					  "org.freedesktop.Tracker1.Resources.Class");

	dbus_g_proxy_add_signal (wrap, "SubjectsAdded", G_TYPE_STRV, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (wrap, "SubjectsAdded", G_CALLBACK (channel_added_cb), model, NULL);

	dbus_g_proxy_add_signal (wrap, "SubjectsChanged", G_TYPE_STRV, G_TYPE_STRV, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (wrap, "SubjectsChanged", G_CALLBACK (channel_changed_cb), model, NULL);

	dbus_g_proxy_add_signal (wrap, "SubjectsRemoved", G_TYPE_STRV, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (wrap, "SubjectsRemoved", G_CALLBACK (channel_removed_cb), model, NULL);

#endif
}

int main (int argc, char **argv)
{
	GtkWidget *window;
	GtkWidget *frame;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *list;
	GtkWidget *scroll;
	GtkWidget *setts;
	GtkWidget *adder;
	GtkListStore *model;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *col;
	GtkTreeSelection *selection;
	TrackerSparqlConnection *tracker;

	gtk_init (&argc, &argv);
	g_set_application_name ("Phidias Feeds Manager");

	tracker = tracker_sparql_connection_get (NULL, NULL);

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
	gtk_container_add (GTK_CONTAINER (window), vbox);

	frame = gtk_frame_new ("Manage");
	gtk_box_pack_start (GTK_BOX (vbox), frame, TRUE, TRUE, 0);

	hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_container_add (GTK_CONTAINER (frame), hbox);

	list = gtk_tree_view_new ();
	renderer = gtk_cell_renderer_text_new ();
	col = gtk_tree_view_column_new_with_attributes ("Title", renderer, "text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (list), col);
	scroll = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scroll), list);
	gtk_box_pack_start (GTK_BOX (hbox), scroll, TRUE, TRUE, 0);

	setts = feed_settings_new ();
	gtk_box_pack_start (GTK_BOX (hbox), setts, FALSE, FALSE, 0);

	frame = gtk_frame_new ("Add");
	gtk_box_pack_start (GTK_BOX (vbox), frame, TRUE, TRUE, 0);

	adder = feeds_adder_new ();
	gtk_container_add (GTK_CONTAINER (frame), adder);

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (list));
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_BROWSE);

	model = gtk_list_store_new (7, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_BOOLEAN);
	/*
		TODO	Sort elements by name
	*/
	gtk_tree_view_set_model (GTK_TREE_VIEW (list), GTK_TREE_MODEL (model));

	g_signal_connect (setts, "remove-feed", G_CALLBACK (feed_removed), tracker);
	g_signal_connect (setts, "save-feed", G_CALLBACK (feed_saved), tracker);
	g_signal_connect (setts, "update-feed", G_CALLBACK (feed_modified), model);
	g_signal_connect (selection, "changed", G_CALLBACK (feed_selected_cb), setts);
	g_signal_connect (window, "delete-event", G_CALLBACK (exit_all), setts);

	init_model (tracker, model);
	listen_tracker (model);
	tune_model (tracker, model);
	feeds_adder_wire_tracker (FEEDS_ADDER (adder), tracker);

	gtk_widget_show_all (window);
	gtk_main ();

	g_object_unref (tracker);

	exit (0);
}
