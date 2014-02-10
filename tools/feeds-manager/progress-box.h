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

#ifndef __PROGRESS_BOX_H__
#define __PROGRESS_BOX_H__

#include <gtk/gtk.h>

#define PROGRESS_BOX_TYPE		(progress_box_get_type())
#define PROGRESS_BOX(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o),	\
					 PROGRESS_BOX_TYPE,			\
					 ProgressBox))
#define PROGRESS_BOX_CLASS(c)		(G_TYPE_CHECK_CLASS_CAST ((c),		\
					 PROGRESS_BOX_TYPE,			\
					 ProgressBoxClass))
#define IS_PROGRESS_BOX(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o),	\
					 PROGRESS_BOX_TYPE))
#define IS_PROGRESS_BOX_CLASS(c)		(G_TYPE_CHECK_CLASS_TYPE ((c),		\
					 PROGRESS_BOX_TYPE))
#define PROGRESS_BOX_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o),	\
					 PROGRESS_BOX_TYPE,			\
					 ProgressBoxClass))

typedef struct _ProgressBox		ProgressBox;
typedef struct _ProgressBoxClass	ProgressBoxClass;
typedef struct _ProgressBoxPrivate	ProgressBoxPrivate;

struct _ProgressBox {
	GtkHBox parent;
	ProgressBoxPrivate *priv;
};

struct _ProgressBoxClass {
	GtkHBoxClass parent;
};

GType		progress_box_get_type	();

GtkWidget*	progress_box_new		();
void		progress_box_bar_mode		(ProgressBox *box, gboolean show_bar);
void		progress_box_pack_start		(ProgressBox *box, GtkWidget *widget, gboolean expand, gboolean fill, guint padding);
void		progress_box_pack_end		(ProgressBox *box, GtkWidget *widget, gboolean expand, gboolean fill, guint padding);
void		progress_box_set_fraction	(ProgressBox *box, double fraction);

#endif /* __PROGRESS_BOX_H__ */
