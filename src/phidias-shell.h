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

#ifndef PHIDIAS_SHELL_H
#define PHIDIAS_SHELL_H

#include "common.h"

#define PHIDIAS_SHELL_TYPE		(phidias_shell_get_type ())
#define PHIDIAS_SHELL(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj),	\
					 PHIDIAS_SHELL_TYPE,			\
					 PhidiasShell))
#define PHIDIAS_SHELL_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass),	\
					 PHIDIAS_SHELL_TYPE,			\
					 PhidiasShellClass))
#define IS_PHIDIAS_SHELL(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj),	\
					 PHIDIAS_SHELL_TYPE))
#define IS_PHIDIAS_SHELL_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass),	\
					 PHIDIAS_SHELL_TYPE))
#define PHIDIAS_SHELL_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj),	\
					 PHIDIAS_SHELL_TYPE,			\
					 PhidiasShellClass))

typedef struct _PhidiasShell		PhidiasShell;
typedef struct _PhidiasShellClass	PhidiasShellClass;
typedef struct _PhidiasShellPrivate	PhidiasShellPrivate;

struct _PhidiasShell {
	GtkWindow			parent;
	PhidiasShellPrivate		*priv;
};

struct _PhidiasShellClass {
	GtkWindowClass			parent_class;
};

GType		phidias_shell_get_type ();

GtkWidget*	phidias_shell_new ();

#endif
