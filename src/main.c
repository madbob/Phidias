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

static gboolean exit_app (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	gtk_main_quit ();
	return FALSE;
}

int main (int argc, char **argv)
{
	GtkWidget *first;

	g_thread_init (NULL);
	gtk_clutter_init (&argc, &argv);

	g_log_set_always_fatal (G_LOG_LEVEL_CRITICAL);

	g_set_application_name ("Phidias");

	first = phidias_shell_new ();
	g_signal_connect (G_OBJECT (first), "delete-event", G_CALLBACK (exit_app), NULL);

	gtk_widget_show_all (first);
	gtk_main ();

	exit (0);
}
