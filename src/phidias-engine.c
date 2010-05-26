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

#include "phidias-engine.h"
#include "phidias-marshal.h"
#include "expander.h"

#define PHIDIAS_ENGINE_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), PHIDIAS_ENGINE_TYPE, PhidiasEnginePrivate))

struct _PhidiasEnginePrivate {
	TrackerClient	*tracker;

	GtkListStore	*channels;
	GPtrArray	*item_requirements;

	GHashTable	*models_pool;
	GtkTreeModel	*selected_model;

	gchar		*filter;
};

enum {
	CHANNEL_CHANGED,
	LAST_SIGNAL
};

static guint signals [LAST_SIGNAL];

G_DEFINE_TYPE (PhidiasEngine, phidias_engine, G_TYPE_OBJECT);

static void fill_model_with_items (GPtrArray *result, GError *error, gpointer user_data)
{
	register int i;
	register int a;
	int tot;
	int *columns;
	gchar **values;
	GValue *dumps;
	GtkListStore *model;

	if (error != NULL) {
		g_warning ("Unable to fetch items: %s.", error->message);
		g_error_free (error);
		return;
	}

	if (result->len < 1)
		return;

	model = GTK_LIST_STORE (gtk_tree_model_filter_get_model ((GtkTreeModelFilter*) user_data));

	values = (gchar**) g_ptr_array_index (result, 0);
	tot = g_strv_length (values);

	columns = alloca (sizeof (int) * tot);

	dumps = alloca (sizeof (GValue) * tot);
	memset (dumps, 0, sizeof (GValue) * tot);

	for (i = 0; i < tot; i++) {
		columns [i] = i;
		g_value_init (&(dumps [i]), G_TYPE_STRING);
	}

	for (i = 0; i < result->len; i++) {
		values = (gchar**) g_ptr_array_index (result, i);

		for (a = 0; a < tot; a++)
			g_value_set_string (&(dumps [a]), values [a]);

		gtk_list_store_insert_with_valuesv (model, NULL, G_MAXINT, columns, dumps, tot);
	}
}

static void fill_model_with_extra_data (GPtrArray *result, GError *error, gpointer user_data)
{
	register int i;
	int index;
	gboolean found;
	gchar **values;
	gchar *subject;
	GtkListStore *model;
	GtkTreeModel *tmodel;
	GtkTreeIter iter;

	if (error != NULL) {
		g_warning ("Unable to fetch items: %s.", error->message);
		g_error_free (error);
		return;
	}

	if (result->len < 1)
		return;

	tmodel = user_data;
	model = GTK_LIST_STORE (tmodel);

	values = (gchar**) g_ptr_array_index (result, 0);
	index = strtoull (values [0], NULL, 10);

	for (i = 0; i < result->len; i++) {
		values = (gchar**) g_ptr_array_index (result, i);

		if (gtk_tree_model_get_iter_first (tmodel, &iter) == TRUE) {
			found = FALSE;

			do {
				gtk_tree_model_get (tmodel, &iter, 0, &subject, -1);

				if (strcmp (subject, values [1]) == 0) {
					found = TRUE;
					gtk_list_store_set (model, &iter, index, values [2], -1);
				}

				g_free (subject);

			} while (found == FALSE && gtk_tree_model_iter_next (tmodel, &iter) == TRUE);
		}
	}
}

static gchar* build_items_query (PhidiasEngine *engine, gchar *parent)
{
	int i;
	char c;
	GString *query;

	query = g_string_new ("SELECT ?item ");

	for (i = 1, c = 'a'; i < ITEM_INFO_LAST; i++, c++)
		g_string_append_printf (query, " ?%c", c);

	g_string_append_printf (query, " WHERE { ?item rdf:type %s . ?item %s <%s> ", ITEM_INFO_MAIN_CLASS, ITEM_TO_CONTAINER_PREDICATE, parent);

	for (i = 1, c = 'a'; i < ITEM_INFO_LAST; i++, c++)
		g_string_append_printf (query, " . ?item %s ?%c", ITEM_INFO_PREDICATES [i], c);

	g_string_append_printf (query, " }");
	return g_string_free (query, FALSE);
}

static gboolean check_filtered_item (GtkTreeModel *model, GtkTreeIter *iter, gpointer data)
{
	gchar *text;
	gboolean ret;
	PhidiasEngine *engine;

	engine = data;

	if (engine->priv->filter == NULL) {
		ret = TRUE;
	}
	else {
		gtk_tree_model_get (model, iter, ITEM_FILTER_CRITERIA, &text, -1);

		if (strcasestr (text, engine->priv->filter) == NULL)
			ret = FALSE;
		else
			ret = TRUE;

		g_free (text);
	}

	return ret;
}

static void add_channel_in_pool (PhidiasEngine *engine, gchar *subject)
{
	int i;
	gchar *query;
	GType *types;
	GtkTreeModel *items;

	/**
		TODO	Here extra columns are allocated to contain more extra informations
			(added with phidias_engine_add_managed_info()), but of course this is a
			wired margin which may be easily reached and if not used wastes memory.
			Alternative solution may be re-create new models with the correct number
			of columns when required (e.g. when a new plugin is loaded)
	*/

	if (ITEM_INFO_LAST > 50) {
		g_error ("Ehm... There is a little limitation... Look at the code for more informations.");
	}

	types = alloca (sizeof (GType) * 50);

	for (i = 0; i < 50; i++)
		types [i] = G_TYPE_STRING;

	items = gtk_tree_model_filter_new (GTK_TREE_MODEL (gtk_list_store_newv (50, types)), NULL);
	gtk_tree_model_filter_set_visible_func (GTK_TREE_MODEL_FILTER (items), check_filtered_item, engine, NULL);
	g_hash_table_insert (engine->priv->models_pool, g_strdup (subject), items);

	query = build_items_query (engine, subject);
	tracker_resources_sparql_query_async (engine->priv->tracker, query, fill_model_with_items, items);
	g_free (query);
}

static gchar* build_channels_query ()
{
	int i;
	char c;
	GString *query;

	query = g_string_new ("SELECT ?item ");

	for (i = 1, c = 'a'; i < CONTAINER_INFO_LAST; i++, c++)
		g_string_append_printf (query, " ?%c", c);

	g_string_append_printf (query, " WHERE { ?item rdf:type %s ", CONTAINER_INFO_MAIN_CLASS);

	for (i = 1, c = 'a'; i < CONTAINER_INFO_LAST; i++, c++)
		g_string_append_printf (query, " . ?item %s ?%c", CONTAINER_INFO_PREDICATES [i], c);

	g_string_append_printf (query, " }");
	return g_string_free (query, FALSE);
}

static void phidias_engine_finalize (GObject *obj)
{
	PhidiasEngine *engine;

	engine = PHIDIAS_ENGINE (obj);

	g_object_unref (engine->priv->tracker);
	g_object_unref (engine->priv->channels);

	/*
		priv->item_requirements links to PhidiasExtraColumn objects
		owned by single plugins
	*/
	g_ptr_array_free (engine->priv->item_requirements, TRUE);

	g_hash_table_destroy (engine->priv->models_pool);
	g_free (engine->priv->filter);
}

static void phidias_engine_class_init (PhidiasEngineClass *klass)
{
	GObjectClass *gobject_class;

	g_type_class_add_private (klass, sizeof (PhidiasEnginePrivate));

	gobject_class = G_OBJECT_CLASS (klass);
	gobject_class->finalize = phidias_engine_finalize;

	signals [CHANNEL_CHANGED] = g_signal_newv ("channel-changed",
				G_TYPE_FROM_CLASS (gobject_class), G_SIGNAL_RUN_LAST,
				NULL, NULL, NULL,
				g_cclosure_marshal_VOID__VOID,
				G_TYPE_NONE, 0, NULL);
}

static GtkTreeIter* search_element_in_model (GtkTreeModel *model, int offset, gchar *subject)
{
	gchar *this_subject;
	GtkTreeIter *ret;
	GtkTreeIter iter;

	ret = NULL;

	if (gtk_tree_model_get_iter_first (model, &iter) == TRUE) {
		do {
			gtk_tree_model_get (model, &iter, offset, &this_subject, -1);
			if (strcmp (subject, this_subject) == 0)
				ret = gtk_tree_iter_copy (&iter);

			g_free (this_subject);

		} while (ret == NULL && gtk_tree_model_iter_next (model, &iter) == TRUE);
	}

	return ret;
}

static GtkTreeIter* search_channel_in_model (PhidiasEngine *engine, gchar *subject)
{
	return search_element_in_model (GTK_TREE_MODEL (engine->priv->channels), CONTAINER_INFO_SUBJECT, subject);
}

static void add_channel_in_model (PhidiasEngine *engine, GPtrArray *chans)
{
	register int i;
	register int a;
	int tot;
	int *columns;
	gchar **values;
	GValue *dumps;

	if (chans->len < 1)
		return;

	values = (gchar**) g_ptr_array_index (chans, 0);
	tot = g_strv_length (values);

	columns = alloca (sizeof (int) * tot);

	dumps = alloca (sizeof (GValue) * tot);
	memset (dumps, 0, sizeof (GValue) * tot);

	for (i = 0; i < tot; i++) {
		columns [i] = i;
		g_value_init (&(dumps [i]), G_TYPE_STRING);
	}

	for (i = 0; i < chans->len; i++) {
		values = (gchar**) g_ptr_array_index (chans, i);

		for (a = 0; a < tot; a++)
			g_value_set_string (&(dumps [a]), values [a]);

		gtk_list_store_insert_with_valuesv (engine->priv->channels, NULL, G_MAXINT, columns, dumps, tot);
		add_channel_in_pool (engine, values [0]);
	}
}

static void channel_added_data_reply_cb (GPtrArray *result, GError *error, gpointer user_data)
{
	PhidiasEngine *engine;

	if (error != NULL) {
		g_warning ("Error fetching data about channel: %s.", error->message);
		g_error_free (error);
	}
	else {
		engine = user_data;
		add_channel_in_model (engine, result);
	}
}

static void channel_added_cb (DBusGProxy *proxy, gchar **subjects, PhidiasEngine *engine)
{
	register int i;
	register int a;
	gchar *str_query;
	gchar c;
	GString *query;

	for (i = 0; subjects [i] != NULL; i++) {
		query = g_string_new ("SELECT ");
		g_string_append_printf (query, " <%s>", subjects [i]);

		for (a = 1, c = 'a'; a < CONTAINER_INFO_LAST; a++, c++)
			g_string_append_printf (query, " ?%c", c);

		g_string_append_printf (query, " WHERE { <%s> a %s ", subjects [i], CONTAINER_INFO_MAIN_CLASS);

		for (a = 1, c = 'a'; a < CONTAINER_INFO_LAST; a++, c++)
			g_string_append_printf (query, " ; %s ?%c", CONTAINER_INFO_PREDICATES [a], c);

		g_string_append_printf (query, " }");
		str_query = g_string_free (query, FALSE);
		tracker_resources_sparql_query_async (engine->priv->tracker, str_query, channel_added_data_reply_cb, engine);
		g_free (str_query);
	}
}

static void channel_changed_data_reply_cb (GPtrArray *result, GError *error, gpointer user_data)
{
	int index;
	gchar **values;
	GtkTreeIter *iter;
	PhidiasEngine *engine;

	engine = user_data;
	values = (gchar**) g_ptr_array_index (result, 0);

	iter = search_channel_in_model (engine, values [0]);
	if (iter != NULL) {
		index = strtoull (values [1], NULL, 10);
		gtk_list_store_set (engine->priv->channels, iter, index, values [2], -1);
		gtk_tree_iter_free (iter);
	}
}

static void channel_changed_cb (DBusGProxy *proxy, gchar **subjects, gchar **data, PhidiasEngine *engine)
{
	register int i;
	register int a;
	gchar *query;

	for (i = 0; data [i] != NULL; i++) {
		for (a = 0; a < CONTAINER_INFO_LAST; a++) {
			if (strcmp (data [i], CONTAINER_INFO_PREDICATES [a]) == 0) {
				query = g_strdup_printf ("SELECT <%s> %d ?a WHERE {<%s> %s ?a}", subjects [i], a, subjects [i], data [i]);
				tracker_resources_sparql_query_async (engine->priv->tracker, query, channel_changed_data_reply_cb, engine);
				g_free (query);
				break;
			}
		}
	}
}

static void channel_removed_cb (DBusGProxy *proxy, gchar **subjects, PhidiasEngine *engine)
{
	register int i;
	GtkTreeIter *iter;

	for (i = 0; subjects [i] != NULL; i++) {
		iter = search_channel_in_model (engine, subjects [i]);
		if (iter != NULL) {
			gtk_list_store_remove (engine->priv->channels, iter);
			gtk_tree_iter_free (iter);
		}
	}
}

static gboolean search_item_in_models_pool (PhidiasEngine *engine, gchar *subject, GtkListStore **model, GtkTreeIter **iter)
{
	GList *channels_models;
	GList *channels_iter;
	GtkTreeModel *model_tmp;
	GtkTreeIter *item;

	channels_models = g_hash_table_get_values (engine->priv->models_pool);
	item = NULL;

	for (channels_iter = channels_models; channels_iter; channels_iter = g_list_next (channels_iter)) {
		model_tmp = gtk_tree_model_filter_get_model (((GtkTreeModelFilter*) channels_iter->data));
		item = search_element_in_model (model_tmp, ITEM_INFO_SUBJECT, subject);
		if (item != NULL)
			break;
	}

	g_list_free (channels_models);

	if (item != NULL) {
		*iter = item;
		*model = GTK_LIST_STORE (model_tmp);
		return TRUE;
	}
	else {
		return FALSE;
	}
}

static void item_added_data_reply_cb (GPtrArray *result, GError *error, gpointer user_data)
{
	register int i;
	int tot;
	int *columns;
	gchar **values;
	GValue *dumps;
	GtkTreeModelFilter *filtered_model;
	GtkListStore *model;
	PhidiasEngine *engine;

	if (error != NULL) {
		g_warning ("Error fetching data about item: %s.", error->message);
		g_error_free (error);
	}
	else if (result->len != 0) {
		engine = user_data;

		values = (gchar**) g_ptr_array_index (result, 0);
		tot = g_strv_length (values);

		columns = alloca (sizeof (int) * tot);

		dumps = alloca (sizeof (GValue) * tot);
		memset (dumps, 0, sizeof (GValue) * tot);

		/*
			In the last position of the array we required the subject of the
			container of the item: it has not to be put in the model, is used to
			retrieve the right model itself
		*/

		for (i = 0; i < (tot - 1); i++) {
			columns [i] = i;
			g_value_init (&(dumps [i]), G_TYPE_STRING);
		}

		for (i = 0; i < result->len; i++) {
			values = (gchar**) g_ptr_array_index (result, i);

			filtered_model = g_hash_table_lookup (engine->priv->models_pool, values [tot - 1]);
			if (filtered_model != NULL) {
				for (i = 0; i < (tot - 1); i++)
					g_value_set_string (&(dumps [i]), values [i]);

				model = GTK_LIST_STORE (gtk_tree_model_filter_get_model (filtered_model));
				gtk_list_store_insert_with_valuesv (model, NULL, G_MAXINT, columns, dumps, tot - 1);
			}
		}
	}
}

static void append_expanded_extra_column (PhidiasExtraColumn *col, const gchar *subject, GString *query, char c)
{
	register int i;
	int path_len;
	const gchar **path;

	switch (phidias_extra_column_get_content_type (col)) {
		case PHIDIAS_EXTRA_COLUMN_PREDICATE:
			g_string_append_printf (query, " . OPTIONAL { <%s> %s ?%c }",
						subject, phidias_extra_column_get_predicate (col), c);
			break;

		case PHIDIAS_EXTRA_COLUMN_PATH:
			path = phidias_extra_column_get_path (col);
			path_len = g_strv_length ((gchar**) path);

			g_string_append_printf (query, " . OPTIONAL { <%s> %s ?%c_offset_0 ", subject, path [0], c);

			for (i = 1; i < path_len - 1; i++) {
				g_string_append_printf (query, " . ?%c_offset_%d %s ?%c_offset_%d ",
							c, i - 1, path [i], c, i);
			}

			g_string_append_printf (query, " . ?%c_offset_%d %s ?%c }", c, i - 1, path [i], c);
			break;

		default:
			g_error ("Undefined extra column contents.");
			break;
	}
}

static void item_added_cb (DBusGProxy *proxy, gchar **subjects, PhidiasEngine *engine)
{
	register int i;
	register int a;
	gchar *str_query;
	gchar c;
	GString *query;
	PhidiasExtraColumn *col;

	for (i = 0; subjects [i] != NULL; i++) {
		query = g_string_new ("SELECT ");
		g_string_append_printf (query, " <%s>", subjects [i]);

		for (a = 1, c = 'a'; a < ITEM_INFO_LAST; a++, c++)
			g_string_append_printf (query, " ?%c", c);

		for (a = 0; a < engine->priv->item_requirements->len; a++, c++)
			g_string_append_printf (query, " ?%c", c);

		g_string_append_printf (query, " ?%c WHERE { <%s> a %s ", c, subjects [i], ITEM_INFO_MAIN_CLASS);

		for (a = 1, c = 'a'; a < ITEM_INFO_LAST; a++, c++)
			g_string_append_printf (query, " ; %s ?%c", ITEM_INFO_PREDICATES [a], c);

		/*
			Take care last value refere to the container of the fetched item, is not
			to be saved in final model
		*/
		g_string_append_printf (query, " ; %s ?%c ", ITEM_TO_CONTAINER_PREDICATE, c);
		c++;

		for (a = 0; a < engine->priv->item_requirements->len; a++, c++) {
			col = (PhidiasExtraColumn*) g_ptr_array_index (engine->priv->item_requirements, a);
			append_expanded_extra_column (col, subjects [i], query, c);
		}

		g_string_append (query, " }");

		str_query = g_string_free (query, FALSE);

		tracker_resources_sparql_query_async (engine->priv->tracker, str_query, item_added_data_reply_cb, engine);
		g_free (str_query);
	}
}

static void item_changed_data_reply_cb (GPtrArray *result, GError *error, gpointer user_data)
{
	int index;
	gchar **values;
	GtkListStore *model;
	GtkTreeIter *iter;
	PhidiasEngine *engine;

	engine = user_data;
	values = (gchar**) g_ptr_array_index (result, 0);

	if (search_item_in_models_pool (engine, values [0], &model, &iter)) {
		index = strtoull (values [1], NULL, 10);
		gtk_list_store_set (model, iter, index, values [2], -1);
		gtk_tree_iter_free (iter);
	}
}

static void item_changed_cb (DBusGProxy *proxy, gchar **subjects, gchar **data, PhidiasEngine *engine)
{
	register int i;
	register int a;
	gchar *query;

	for (i = 0; data [i] != NULL; i++) {
		for (a = 0; a < ITEM_INFO_LAST; a++) {
			if (strcmp (data [i], ITEM_INFO_PREDICATES [a]) == 0) {
				query = g_strdup_printf ("SELECT <%s> %d ?a WHERE {<%s> %s ?a}", subjects [i], a, subjects [i], data [i]);
				tracker_resources_sparql_query_async (engine->priv->tracker, query, item_changed_data_reply_cb, engine);
				g_free (query);
				break;
			}
		}
	}
}

static void item_removed_cb (DBusGProxy *proxy, gchar **subjects, PhidiasEngine *engine)
{
	register int i;
	GtkListStore *model;
	GtkTreeIter *iter;

	for (i = 0; subjects [i] != NULL; i++) {
		if (search_item_in_models_pool (engine, subjects [i], &model, &iter)) {
			gtk_list_store_remove (model, iter);
			gtk_tree_iter_free (iter);
		}
	}
}

static void listen_incoming_data (PhidiasEngine *engine)
{
	gchar *sep;
	gchar *name;
	gchar *namespace;
	gchar *path;
	DBusGConnection *bus;
	DBusGProxy *wrap;

	bus = dbus_g_bus_get (DBUS_BUS_SESSION, NULL);
	dbus_g_object_register_marshaller (phidias_marshal_VOID__BOXED_BOXED, G_TYPE_NONE, G_TYPE_STRV, G_TYPE_STRV, G_TYPE_INVALID);

	namespace = strdupa (CONTAINER_INFO_MAIN_CLASS);
	sep = strchr (namespace, ':');
	*sep = '\0';
	name = sep + 1;
	path = g_strdup_printf ("/org/freedesktop/Tracker1/Resources/Classes/%s/%s", namespace, name);
	wrap = dbus_g_proxy_new_for_name (bus, "org.freedesktop.Tracker1", path, "org.freedesktop.Tracker1.Resources.Class");

	dbus_g_proxy_add_signal (wrap, "SubjectsAdded", G_TYPE_STRV, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (wrap, "SubjectsAdded", G_CALLBACK (channel_added_cb), engine, NULL);

	dbus_g_proxy_add_signal (wrap, "SubjectsChanged", G_TYPE_STRV, G_TYPE_STRV, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (wrap, "SubjectsChanged", G_CALLBACK (channel_changed_cb), engine, NULL);

	dbus_g_proxy_add_signal (wrap, "SubjectsRemoved", G_TYPE_STRV, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (wrap, "SubjectsRemoved", G_CALLBACK (channel_removed_cb), engine, NULL);

	namespace = strdupa (ITEM_INFO_MAIN_CLASS);
	sep = strchr (namespace, ':');
	*sep = '\0';
	name = sep + 1;
	path = g_strdup_printf ("/org/freedesktop/Tracker1/Resources/Classes/%s/%s", namespace, name);
	wrap = dbus_g_proxy_new_for_name (bus, "org.freedesktop.Tracker1", path, "org.freedesktop.Tracker1.Resources.Class");

	dbus_g_proxy_add_signal (wrap, "SubjectsAdded", G_TYPE_STRV, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (wrap, "SubjectsAdded", G_CALLBACK (item_added_cb), engine, NULL);

	dbus_g_proxy_add_signal (wrap, "SubjectsChanged", G_TYPE_STRV, G_TYPE_STRV, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (wrap, "SubjectsChanged", G_CALLBACK (item_changed_cb), engine, NULL);

	dbus_g_proxy_add_signal (wrap, "SubjectsRemoved", G_TYPE_STRV, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (wrap, "SubjectsRemoved", G_CALLBACK (item_removed_cb), engine, NULL);
}

static void fill_model_with_channels (GPtrArray *result, GError *error, gpointer user_data)
{
	PhidiasEngine *engine;

	engine = user_data;

	if (error != NULL) {
		g_warning ("Unable to retrieve list of channels: %s.", error->message);
		g_error_free (error);
	}
	else {
		add_channel_in_model (engine, result);
	}

	listen_incoming_data (engine);
}

static void init_channels (PhidiasEngine *engine)
{
	int i;
	gchar *query;
	GType *types;

	types = alloca (sizeof (GType) * CONTAINER_INFO_LAST);
	for (i = 0; i < CONTAINER_INFO_LAST; i++)
		types [i] = G_TYPE_STRING;

	engine->priv->channels = gtk_list_store_newv (CONTAINER_INFO_LAST, types);

	query = build_channels_query ();
	tracker_resources_sparql_query_async (engine->priv->tracker, query, fill_model_with_channels, engine);
	g_free (query);
}

static void enrich_models_with_metadata (PhidiasEngine *engine, PhidiasExtraColumn *extra, int col_index)
{
	gchar *str_query;
	GList *channels;
	GList *iter;
	GtkTreeModel *model;
	GString *query;

	channels = g_hash_table_get_keys (engine->priv->models_pool);

	for (iter = channels; iter; iter = g_list_next (iter)) {
		query = g_string_new ("");
		model = gtk_tree_model_filter_get_model ((GtkTreeModelFilter*) g_hash_table_lookup (engine->priv->models_pool, iter->data));

		g_string_append_printf (query, "SELECT %d ?item ?a WHERE {?item a %s . ?item %s <%s>",
					col_index, ITEM_INFO_MAIN_CLASS, ITEM_TO_CONTAINER_PREDICATE, (gchar*) iter->data);
		append_expanded_extra_column (extra, iter->data, query, 'a');
		g_string_append_c (query, '}');

		str_query = g_string_free (query, FALSE);

		tracker_resources_sparql_query_async (engine->priv->tracker, str_query, fill_model_with_extra_data, model);
		g_free (str_query);
	}

	g_list_free (channels);
}

static void phidias_engine_init (PhidiasEngine *item)
{
	item->priv = PHIDIAS_ENGINE_GET_PRIVATE (item);
	item->priv->tracker = tracker_client_new (0, G_MAXINT);
	item->priv->item_requirements = g_ptr_array_new ();
	item->priv->models_pool = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_object_unref);

	init_channels (item);
}

PhidiasEngine* phidias_engine_new ()
{
	return g_object_new (PHIDIAS_ENGINE_TYPE, NULL);
}

GtkTreeModel* phidias_engine_get_channels (PhidiasEngine *engine)
{
	return GTK_TREE_MODEL (engine->priv->channels);
}

void phidias_engine_set_channel (PhidiasEngine *engine, GtkTreeIter *channel)
{
	gchar *subject;
	GtkTreeModel *model;

	subject = NULL;
	gtk_tree_model_get (GTK_TREE_MODEL (engine->priv->channels), channel, 0, &subject, -1);

	if (subject == NULL) {
		g_warning ("Unable to select the given channel.");
		return;
	}

	model = g_hash_table_lookup (engine->priv->models_pool, subject);
	if (model == NULL) {
		g_warning ("Unable to retrieve the given channel.");
	}
	else if (model != engine->priv->selected_model) {
		engine->priv->selected_model = model;
		g_signal_emit (engine, signals [CHANNEL_CHANGED], 0, NULL);
	}

	g_free (subject);
}

int phidias_engine_add_managed_info (PhidiasEngine *engine, PhidiasExtraColumn *extra)
{
	int index;

	g_ptr_array_add (engine->priv->item_requirements, extra);
	index = ITEM_INFO_LAST + engine->priv->item_requirements->len;
	enrich_models_with_metadata (engine, extra, index);
	return index;
}

GtkTreeModel* phidias_engine_get_current_model (PhidiasEngine *engine)
{
	return engine->priv->selected_model;
}

static void check_update (GError *error, gpointer data)
{
	if (error != NULL)
		g_warning ("Unable to update the item: %s.", error->message);
}

void phidias_engine_update_item (PhidiasEngine *engine, GtkTreeModel *model, GtkTreeIter *iter, int index, gchar *value)
{
	gchar *query;
	gchar *subject;
	GtkTreeModelFilter *fmodel;
	GtkTreeIter real_iter;

	gtk_tree_model_get (model, iter, 0, &subject, -1);

	query = g_strdup_printf ("DELETE FROM <%s> {<%s> %s ?v} WHERE {<%s> %s ?v} INSERT {<%s> %s \"%s\"}",
				 subject, subject, ITEM_INFO_PREDICATES [index],
				 subject, ITEM_INFO_PREDICATES [index],
				 subject, ITEM_INFO_PREDICATES [index], value);
	tracker_resources_sparql_update_async (engine->priv->tracker, query, check_update, engine);
	g_free (query);

	fmodel = GTK_TREE_MODEL_FILTER (model);
	gtk_tree_model_filter_convert_iter_to_child_iter (fmodel, &real_iter, iter);
	gtk_list_store_set (GTK_LIST_STORE (gtk_tree_model_filter_get_model (fmodel)), &real_iter, index, value, -1);
	g_free (subject);
}

void phidias_engine_apply_filter (PhidiasEngine *engine, const gchar *filter)
{
	GList *channels_models;
	GList *channels_iter;

	if (engine->priv->filter != NULL)
		g_free (engine->priv->filter);

	if (filter != NULL)
		engine->priv->filter = g_strdup (filter);
	else
		engine->priv->filter = NULL;

	channels_models = g_hash_table_get_values (engine->priv->models_pool);

	for (channels_iter = channels_models; channels_iter; channels_iter = g_list_next (channels_iter))
		gtk_tree_model_filter_refilter ((GtkTreeModelFilter*) channels_iter->data);

	g_list_free (channels_models);
}
