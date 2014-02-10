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

#include "progress-box.h"

#define GET_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), PROGRESS_BOX_TYPE, ProgressBoxPrivate))

struct _ProgressBoxPrivate {
	GtkWidget *main_panel;
	GtkWidget *box;
	GtkWidget *progress;
};

G_DEFINE_TYPE (ProgressBox, progress_box, GTK_TYPE_HBOX);

static void progress_box_class_init (ProgressBoxClass *klass)
{
	g_type_class_add_private (klass, sizeof (ProgressBoxPrivate));
}

static void progress_box_init (ProgressBox *app)
{
	app->priv = GET_PRIV (app);

	gtk_box_set_homogeneous (GTK_BOX (app), FALSE);
	gtk_box_set_spacing (GTK_BOX (app), 0);

	app->priv->main_panel = gtk_notebook_new ();
	gtk_notebook_set_show_tabs (GTK_NOTEBOOK (app->priv->main_panel), FALSE);
	gtk_notebook_set_show_border (GTK_NOTEBOOK (app->priv->main_panel), FALSE);
	gtk_box_pack_start (GTK_BOX (app), app->priv->main_panel, TRUE, TRUE, 0);

	app->priv->box = gtk_hbox_new (FALSE, 0);
	gtk_notebook_append_page (GTK_NOTEBOOK (app->priv->main_panel), app->priv->box, NULL);

	app->priv->progress = gtk_progress_bar_new ();
	gtk_notebook_append_page (GTK_NOTEBOOK (app->priv->main_panel), app->priv->progress, NULL);

	gtk_notebook_set_current_page (GTK_NOTEBOOK (app->priv->main_panel), 0);
}

GtkWidget* progress_box_new ()
{
	return g_object_new (PROGRESS_BOX_TYPE, NULL);
}

void progress_box_bar_mode (ProgressBox *box, gboolean show_bar)
{
	if (show_bar == TRUE) {
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (box->priv->progress), 0);
		gtk_notebook_set_current_page (GTK_NOTEBOOK (box->priv->main_panel), 1);
	}
	else {
		gtk_notebook_set_current_page (GTK_NOTEBOOK (box->priv->main_panel), 0);
	}
}

void progress_box_set_fraction (ProgressBox *box, double fraction)
{
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (box->priv->progress), fraction);
}

void progress_box_pack_start (ProgressBox *box, GtkWidget *widget, gboolean expand, gboolean fill, guint padding)
{
	gtk_box_pack_start (GTK_BOX (box->priv->box), widget, expand, fill, padding);
}

void progress_box_pack_end (ProgressBox *box, GtkWidget *widget, gboolean expand, gboolean fill, guint padding)
{
	gtk_box_pack_end (GTK_BOX (box->priv->box), widget, expand, fill, padding);
}
