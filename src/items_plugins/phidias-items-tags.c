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

#include "phidias-items-tags.h"
#include "phidias-items-viewer.h"
#include "phidias-extra-column.h"
#include "expander.h"

#define PHIDIAS_ITEMS_TAGS_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), PHIDIAS_ITEMS_TAGS_TYPE, PhidiasItemsTagsPrivate))

struct _PhidiasItemsTagsPrivate {
	gint		current_width;
	gint		current_height;

	GList		*words;

	ClutterActor	*cloud;
};

typedef struct {
	gchar	*word;
	int	size;
} SizedWord;

static GType type;

static void free_existing_words (PhidiasItemsTags *viewer)
{
	GList *iter;
	SizedWord *s;

	if (viewer->priv->words == NULL)
		return;

	for (iter = viewer->priv->words; iter; iter = g_list_next (iter)) {
		s = iter->data;
		g_free (s->word);
		g_free (s);
	}

	g_list_free (viewer->priv->words);
	viewer->priv->words = NULL;
}

static void size_received_cb (GtkWidget *widget, GtkAllocation *allocation, gpointer user_data)
{
	PhidiasItemsTags *viewer;

	viewer = user_data;
	viewer->priv->current_width = allocation->width;
	viewer->priv->current_height = allocation->height;
}

static ClutterActorBox* set_word_position (PhidiasItemsTags *viewer, ClutterActor *label, GList *managed)
{
	int i;
	int try;
	int managed_num;
	gfloat x1;
	gfloat y1;
	gfloat x2;
	gfloat y2;
	gfloat width;
	gfloat height;
	gboolean more;
	GList *iter;
	GList *first;
	ClutterActorBox *box;
	ClutterActorBox *sibling_box;
	static int move = -1;

	clutter_actor_get_size (label, &width, &height);

	x1 = viewer->priv->current_width / 2 - width / 2;
	y1 = viewer->priv->current_height / 2 - height / 2;
	x2 = x1 + width;
	y2 = y1 + height;

	if (managed != NULL) {
		managed_num = g_list_length (managed);

		for (try = 0; try < 50; try++) {
			more = FALSE;
			first = g_list_nth (managed, rand () % managed_num);

			for (iter = first; iter; iter = g_list_next (iter)) {
				sibling_box = iter->data;

				if (sibling_box->x1 < x2 && sibling_box->x2 > x1 && sibling_box->y1 < y2 && sibling_box->y2 > y1) {
					more = TRUE;
					break;
				}
			}

			if (more == FALSE) {
				for (iter = managed; iter != first; iter = g_list_next (iter)) {
					sibling_box = iter->data;

					if (sibling_box->x1 < x2 && sibling_box->x2 > x1 && sibling_box->y1 < y2 && sibling_box->y2 > y1) {
						more = TRUE;
						break;
					}
				}
			}

			if (more == TRUE) {
				for (i = move, move = (i + 1) % 4; more == TRUE && move != i; move = (move + 1) % 4) {
					switch (move) {
						case 0:
							if (sibling_box->x2 + width < viewer->priv->current_width) {
								x1 = sibling_box->x2;
								x2 = x1 + width;
								more = FALSE;
							}

							break;

						case 1:
							if (sibling_box->x1 - width > 0) {
								x2 = sibling_box->x1;
								x1 = x2 - width;
								more = FALSE;
							}

							break;

						case 2:
							if (sibling_box->y2 + height < viewer->priv->current_height) {
								y1 = sibling_box->y2;
								y2 = y1 + height;
								more = FALSE;
							}

							break;

						case 3:
							if (sibling_box->y1 - height > 0) {
								y2 = sibling_box->y1;
								y1 = y2 - height;
								more = FALSE;
							}

							break;

						default:
							break;
					}
				}

				if (more == TRUE)
					return NULL;
			}
			else {
				break;
			}
		}

		if (try == 50)
			return NULL;
	}

	box = g_new0 (ClutterActorBox, 1);
	box->x1 = x1;
	box->x2 = x2;
	box->y1 = y1;
	box->y2 = y2;

	clutter_actor_set_position (label, box->x1, box->y1);
	return box;
}

/*
	mode:
		0 - no trasformation
		1 - increase
		2 - decrease
*/
static ClutterActor* do_autosize_tag_cloud (PhidiasItemsTags *viewer, GList *words, int mode)
{
	gchar font [100];
	gboolean retry;
	GList *iter;
	GList *managed;
	ClutterActor *label;
	ClutterActor *tagcloud;
	ClutterContainer *tagcloud_c;
	ClutterActorBox *box;
	SizedWord *s;

	managed = NULL;
	retry = FALSE;
	tagcloud = clutter_group_new ();
	tagcloud_c = CLUTTER_CONTAINER (tagcloud);

	for (iter = words; iter; iter = g_list_next (iter)) {
		s = iter->data;

		snprintf (font, 100, "Serif %d", s->size);
		label = clutter_text_new_with_text (font, s->word);
		clutter_container_add_actor (tagcloud_c, label);

		box = set_word_position (viewer, label, managed);
		if (box == NULL) {
			retry = TRUE;
			break;
		}

		managed = g_list_prepend (managed, box);
	}

	if (managed != NULL) {
		for (iter = managed; iter; iter = g_list_next (iter)) {
			box = iter->data;
			g_free (box);
		}

		g_list_free (managed);
	}

	if (retry == TRUE) {
		for (iter = words; iter; iter = g_list_next (iter)) {
			s = iter->data;
			s->size -= 1;
		}

		g_object_unref (tagcloud);
		tagcloud = do_autosize_tag_cloud (viewer, words, 2);
	}
	else if (mode != 2) {
		for (iter = words; iter; iter = g_list_next (iter)) {
			s = iter->data;
			s->size += 1;
		}

		g_object_unref (tagcloud);
		tagcloud = do_autosize_tag_cloud (viewer, words, 1);
	}

	return tagcloud;
}

static ClutterActor* do_tag_cloud (PhidiasItemsTags *viewer, GList *words)
{
	return do_autosize_tag_cloud (viewer, words, 0);
}

static void retrieve_words (PhidiasItemsTags *viewer, GtkTreeModel *items)
{
	int i;
	gchar *t;
	gchar *title;
	gchar **tokens;
	gboolean done;
	GList *iter;
	GtkTreeIter row;
	SizedWord *s;

	free_existing_words (viewer);

	if (gtk_tree_model_get_iter_first (items, &row) == FALSE)
		return;

	do {
		gtk_tree_model_get (items, &row, ITEM_INFO_TITLE, &title, -1);
		tokens = g_strsplit (title, " ", -1);

		for (i = 0; tokens [i] != NULL; i++) {
			t = g_strdelimit (tokens [i], "':)", '\0');
			done = FALSE;

			for (iter = viewer->priv->words; iter; iter = g_list_next (iter)) {
				s = iter->data;

				if (strcasecmp (s->word, t) == 0) {
					s->size += 1;
					done = TRUE;
					break;
				}
			}

			if (done == FALSE) {
				s = g_new0 (SizedWord, 1);
				s->word = g_strdup (t);
				s->size = 1;
				viewer->priv->words = g_list_prepend (viewer->priv->words, s);
			}
		}

		g_free (title);
		g_strfreev (tokens);

	} while (gtk_tree_model_iter_next (items, &row));
}

static const gchar* phidias_items_tags_get_name (PhidiasItemsViewer *self)
{
	return "TagCloud";
}

static void phidias_items_tags_set_model (PhidiasItemsViewer *self, GtkTreeModel *items)
{
	ClutterContainer *stage;
	PhidiasItemsTags *item;

	item = PHIDIAS_ITEMS_TAGS (self);

	if (item->priv->current_width == 0 || item->priv->current_height == 0)
		return;

	stage = CLUTTER_CONTAINER (gtk_clutter_embed_get_stage (GTK_CLUTTER_EMBED (item)));

	if (item->priv->cloud != NULL) {
		clutter_group_remove_all (CLUTTER_GROUP (item->priv->cloud));
		clutter_container_remove_actor (stage, item->priv->cloud);
		item->priv->cloud = NULL;
	}

	retrieve_words (item, items);
	if (item->priv->words == NULL)
		return;

	item->priv->cloud = do_tag_cloud (item, item->priv->words);
	clutter_actor_set_position (item->priv->cloud, 0, 0);
	clutter_container_add_actor (stage, item->priv->cloud);
}

static void phidias_items_viewer_interface_init (PhidiasItemsViewerInterface *iface)
{
	iface->get_name = phidias_items_tags_get_name;
	iface->set_model = phidias_items_tags_set_model;
}

static void phidias_items_tags_finalize (GObject *object)
{
	PhidiasItemsTags *item;

	item = PHIDIAS_ITEMS_TAGS (object);
	free_existing_words (item);
}

static void phidias_items_tags_class_init (PhidiasItemsTagsClass *klass)
{
	GObjectClass *gobject_class;

	g_type_class_add_private (klass, sizeof (PhidiasItemsTagsPrivate));

	gobject_class = G_OBJECT_CLASS (klass);
	gobject_class->finalize = phidias_items_tags_finalize;
}

static void phidias_items_tags_init (PhidiasItemsTags *item)
{
	srand (time (NULL));
	item->priv = PHIDIAS_ITEMS_TAGS_GET_PRIVATE (item);
	g_signal_connect (item, "size-allocate", G_CALLBACK (size_received_cb), item);
}

GType register_module (GTypeModule *module)
{
	static const GTypeInfo our_info = {
		sizeof (PhidiasItemsTagsClass),
		NULL, NULL,
		(GClassInitFunc) phidias_items_tags_class_init,
		NULL, NULL,
		sizeof (PhidiasItemsTags), 0,
		(GInstanceInitFunc) phidias_items_tags_init
	};

	static const GInterfaceInfo extension_info = {
		(GInterfaceInitFunc) phidias_items_viewer_interface_init,
		NULL, NULL
	};

	type = g_type_module_register_type (module, GTK_CLUTTER_TYPE_EMBED, "PhidiasItemsTags", &our_info, 0);
	g_type_module_add_interface (module, type, PHIDIAS_ITEMS_VIEWER_TYPE, &extension_info);

	return type;
}

GType phidias_items_tags_get_type ()
{
	return type;
}
