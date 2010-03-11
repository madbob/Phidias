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

#include "phidias-items-geo.h"
#include "phidias-items-viewer.h"
#include "phidias-extra-column.h"
#include "expander.h"

#define PHIDIAS_ITEMS_GEO_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), PHIDIAS_ITEMS_GEO_TYPE, PhidiasItemsGeoPrivate))

struct _PhidiasItemsGeoPrivate {
	GPtrArray	*extras;

	guint		signals [2];
	GtkTreeModel	*tree_model;

	ChamplainLayer	*current_layer;
	GHashTable	*markers;

	gboolean	show_text;
	ClutterActor	*marker_icon;
};

static GType type;

static ClutterActor* do_marker_icon ()
{
	GtkIconTheme *theme;
	GtkIconInfo *icon_info;

	theme = gtk_icon_theme_get_default ();
	icon_info = gtk_icon_theme_lookup_icon (theme, "go-jump", 22, 0);
	return clutter_texture_new_from_file (gtk_icon_info_get_filename (icon_info), NULL);
}

static gboolean add_marker (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data)
{
	int latitude_offset;
	int longitude_offset;
	double lat;
	double lon;
	gchar *title;
	gchar *latitude_str;
	gchar *longitude_str;
	gchar *check;
	ClutterActor *marker;
	ClutterActor *icon;
	PhidiasItemsGeo *item;

	item = data;

	latitude_offset = phidias_extra_column_get_index ((PhidiasExtraColumn*) g_ptr_array_index (item->priv->extras, 0));
	longitude_offset = phidias_extra_column_get_index ((PhidiasExtraColumn*) g_ptr_array_index (item->priv->extras, 1));

	gtk_tree_model_get (model, iter, ITEM_INFO_TITLE, &title,
			    latitude_offset, &latitude_str,
			    longitude_offset, &longitude_str, -1);

	if (latitude_str == NULL || longitude_str == NULL)
		goto end;

	check = NULL;
	lat = strtod (latitude_str, &check);
	if (*check != '\0')
		goto end;

	check = NULL;
	lon = strtod (longitude_str, &check);
	if (*check != '\0')
		goto end;

	/*
		TODO	Provide a proper ChamplainMarker implementation, able to store the icon,
			the title, the description, and able to react to clicks
	*/

	icon = do_marker_icon ();

	if (item->priv->show_text) {
		marker = champlain_marker_new_with_text (title, "Serif 9", NULL, NULL);
	}
	else {
		marker = champlain_marker_new_with_text (title, "Serif 0", NULL, NULL);
		champlain_marker_set_image (marker, icon);
	}

	g_object_set_data_full (G_OBJECT (marker), "icon", icon, g_object_unref);

	champlain_base_marker_set_position (CHAMPLAIN_BASE_MARKER (marker), lat, lon);
	champlain_layer_add_marker (item->priv->current_layer, CHAMPLAIN_BASE_MARKER (marker));
	g_hash_table_insert (item->priv->markers, gtk_tree_path_to_string (path), marker);

end:
	if (title != NULL)
		g_free (title);
	if (latitude_str != NULL)
		g_free (latitude_str);
	if (longitude_str != NULL)
		g_free (longitude_str);

	return FALSE;
}

static void item_deleted_cb (GtkTreeModel *tree_model, GtkTreePath *path, PhidiasItemsGeo *item)
{
	gchar *str;
	ClutterActor *marker;

	str = gtk_tree_path_to_string (path);
	marker = g_hash_table_lookup (item->priv->markers, str);

	if (marker != NULL) {
		champlain_layer_remove_marker (item->priv->current_layer, CHAMPLAIN_BASE_MARKER (marker));
		g_hash_table_remove (item->priv->markers, str);
	}

	g_free (str);
}

static void item_added_cb (GtkTreeModel *tree_model, GtkTreePath  *path, GtkTreeIter *iter, gpointer user_data)
{
	add_marker (tree_model, path, iter, user_data);
}

static gboolean remove_marker (gpointer key, gpointer value, gpointer user_data)
{
	ChamplainLayer *layer;

	layer = user_data;
	champlain_layer_remove_marker (layer, CHAMPLAIN_BASE_MARKER (value));
	return TRUE;
}

static void show_text_markers (ClutterActor *actor, ClutterEvent *event, PhidiasItemsGeo *item)
{
	GList *markers;
	GList *iter;
	ChamplainMarker *m;

	if (item->priv->show_text == TRUE)
		return;

	markers = g_hash_table_get_values (item->priv->markers);

	for (iter = markers; iter; iter = g_list_next (iter)) {
		m = CHAMPLAIN_MARKER (iter->data);
		champlain_marker_set_image (m, NULL);
		champlain_marker_set_font_name (m, "Serif 9");
	}

	g_list_free (markers);
	item->priv->show_text = TRUE;
}

static void show_point_markers (ClutterActor *actor, ClutterEvent *event, PhidiasItemsGeo *item)
{
	GList *markers;
	GList *iter;
	ClutterActor *icon;
	ChamplainMarker *m;

	if (item->priv->show_text == FALSE)
		return;

	markers = g_hash_table_get_values (item->priv->markers);

	for (iter = markers; iter; iter = g_list_next (iter)) {
		m = CHAMPLAIN_MARKER (iter->data);
		icon = g_object_get_data (G_OBJECT (m), "icon");
		champlain_marker_set_image (m, icon);
		champlain_marker_set_font_name (m, "Serif 0");
	}

	g_list_free (markers);
	item->priv->show_text = FALSE;
}

static void dispose_zoom_icons (PhidiasItemsGeo *item)
{
	GtkIconTheme *theme;
	GtkIconInfo *icon_info;
	ClutterActor *icon;
	ClutterActor *stage;
	ChamplainView *view;

	theme = gtk_icon_theme_get_default ();
	view = gtk_champlain_embed_get_view (GTK_CHAMPLAIN_EMBED (item));
	stage = clutter_actor_get_stage (CLUTTER_ACTOR (view));

	icon_info = gtk_icon_theme_lookup_icon (theme, "zoom-in", 24, 0);
	if (icon_info != NULL) {
		icon = clutter_texture_new_from_file (gtk_icon_info_get_filename (icon_info), NULL);
		clutter_actor_set_reactive (icon, TRUE);
		clutter_actor_set_fixed_position_set (icon, TRUE);
		clutter_container_add_actor (CLUTTER_CONTAINER (stage), icon);
		clutter_actor_set_position (icon, 10, 10);
		g_signal_connect_swapped (icon, "button-press-event", G_CALLBACK (champlain_view_zoom_in), view);
	}

	icon_info = gtk_icon_theme_lookup_icon (theme, "zoom-out", 24, 0);
	if (icon_info != NULL) {
		icon = clutter_texture_new_from_file (gtk_icon_info_get_filename (icon_info), NULL);
		clutter_actor_set_reactive (icon, TRUE);
		clutter_actor_set_fixed_position_set (icon, TRUE);
		clutter_container_add_actor (CLUTTER_CONTAINER (stage), icon);
		clutter_actor_set_position (icon, 10, 40);
		g_signal_connect_swapped (icon, "button-press-event", G_CALLBACK (champlain_view_zoom_out), view);
	}

	/*
		TODO	Those have to be mutually exclusive
	*/

	icon_info = gtk_icon_theme_lookup_icon (theme, "format-text-bold", 24, 0);
	if (icon_info != NULL) {
		icon = clutter_texture_new_from_file (gtk_icon_info_get_filename (icon_info), NULL);
		clutter_actor_set_reactive (icon, TRUE);
		clutter_actor_set_fixed_position_set (icon, TRUE);
		clutter_container_add_actor (CLUTTER_CONTAINER (stage), icon);
		clutter_actor_set_position (icon, 10, 70);
		g_signal_connect (icon, "button-press-event", G_CALLBACK (show_text_markers), item);
	}

	icon_info = gtk_icon_theme_lookup_icon (theme, "go-down", 24, 0);
	if (icon_info != NULL) {
		icon = clutter_texture_new_from_file (gtk_icon_info_get_filename (icon_info), NULL);
		clutter_actor_set_reactive (icon, TRUE);
		clutter_actor_set_fixed_position_set (icon, TRUE);
		clutter_container_add_actor (CLUTTER_CONTAINER (stage), icon);
		clutter_actor_set_position (icon, 10, 100);
		g_signal_connect (icon, "button-press-event", G_CALLBACK (show_point_markers), item);
	}
}

static const gchar* phidias_items_geo_get_name (PhidiasItemsViewer *self)
{
	return "Geo";
}

static void phidias_items_geo_set_model (PhidiasItemsViewer *self, GtkTreeModel *items)
{
	ChamplainView *view;
	PhidiasItemsGeo *geo;

	geo = PHIDIAS_ITEMS_GEO (self);

	if (geo->priv->tree_model != NULL) {
		g_signal_handler_disconnect (geo->priv->tree_model, geo->priv->signals [0]);
		g_signal_handler_disconnect (geo->priv->tree_model, geo->priv->signals [1]);
	}

	view = gtk_champlain_embed_get_view (GTK_CHAMPLAIN_EMBED (geo));
	g_hash_table_foreach_remove (geo->priv->markers, remove_marker, geo->priv->current_layer);

	if (items == NULL)
		return;

	/**
		TODO	This has to be done in a idle() callback
	*/
	gtk_tree_model_foreach (items, add_marker, geo);
	champlain_layer_show (geo->priv->current_layer);
	champlain_layer_show_all_markers (geo->priv->current_layer);
	champlain_layer_animate_in_all_markers (geo->priv->current_layer);

	geo->priv->tree_model = items;
	geo->priv->signals [0] = g_signal_connect (items, "row-deleted", G_CALLBACK (item_deleted_cb), geo);
	geo->priv->signals [1] = g_signal_connect (items, "row-inserted", G_CALLBACK (item_added_cb), geo);
}

static const GPtrArray* phidias_items_geo_get_extra (PhidiasItemsViewer *self)
{
	PhidiasItemsGeo *item;
	PhidiasExtraColumn *col;

	item = PHIDIAS_ITEMS_GEO (self);

	if (item->priv->extras == NULL) {
		item->priv->extras = g_ptr_array_sized_new (2);

		col = phidias_extra_column_new ();
		phidias_extra_column_set_pathv (col, "mlo:location", "mlo:asGeoPoint", "mlo:latitude", NULL);
		g_ptr_array_add (item->priv->extras, col);

		col = phidias_extra_column_new ();
		phidias_extra_column_set_pathv (col, "mlo:location", "mlo:asGeoPoint", "mlo:longitude", NULL);
		g_ptr_array_add (item->priv->extras, col);
	}

	return item->priv->extras;
}

static void phidias_items_viewer_interface_init (PhidiasItemsViewerInterface *iface)
{
	iface->get_name = phidias_items_geo_get_name;
	iface->set_model = phidias_items_geo_set_model;
	iface->get_extra = phidias_items_geo_get_extra;
}

static void phidias_items_geo_finalize (GObject *object)
{
	PhidiasItemsGeo *geo;

	geo = PHIDIAS_ITEMS_GEO (object);

	champlain_view_remove_layer (gtk_champlain_embed_get_view (GTK_CHAMPLAIN_EMBED (geo)), geo->priv->current_layer);
	g_object_unref (geo->priv->current_layer);

	g_hash_table_destroy (geo->priv->markers);
}

static void phidias_items_geo_class_init (PhidiasItemsGeoClass *klass)
{
	GObjectClass *gobject_class;

	g_type_class_add_private (klass, sizeof (PhidiasItemsGeoPrivate));

	gobject_class = G_OBJECT_CLASS (klass);
	gobject_class->finalize = phidias_items_geo_finalize;
}

static void phidias_items_geo_init (PhidiasItemsGeo *item)
{
	ChamplainView *view;

	item->priv = PHIDIAS_ITEMS_GEO_GET_PRIVATE (item);
	memset (item->priv, 0, sizeof (PhidiasItemsGeoPrivate));
	item->priv->markers = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);

	dispose_zoom_icons (item);

	view = gtk_champlain_embed_get_view (GTK_CHAMPLAIN_EMBED (item));

	item->priv->current_layer = champlain_selection_layer_new ();
	champlain_view_add_layer (view, item->priv->current_layer);

	champlain_view_set_min_zoom_level (view, 2);
	champlain_view_set_zoom_on_double_click (view, FALSE);
	champlain_view_set_keep_center_on_resize (view, TRUE);
}

GType register_module (GTypeModule *module)
{
	static const GTypeInfo our_info = {
		sizeof (PhidiasItemsGeoClass),
		NULL, NULL,
		(GClassInitFunc) phidias_items_geo_class_init,
		NULL, NULL,
		sizeof (PhidiasItemsGeo), 0,
		(GInstanceInitFunc) phidias_items_geo_init
	};

	static const GInterfaceInfo extension_info = {
		(GInterfaceInitFunc) phidias_items_viewer_interface_init,
		NULL, NULL
	};

	type = g_type_module_register_type (module, GTK_TYPE_CHAMPLAIN_EMBED, "PhidiasItemsGeo", &our_info, 0);
	g_type_module_add_interface (module, type, PHIDIAS_ITEMS_VIEWER_TYPE, &extension_info);

	return type;
}

GType phidias_items_geo_get_type ()
{
	return type;
}
