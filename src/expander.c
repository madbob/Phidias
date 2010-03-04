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

gchar *CONTAINER_INFO_PREDICATES [] = {
	"nie:InformationElement",
	#define CONTAINER_INFO(__i, __n)	__n,
	#include "container-conf.h"
	#undef CONTAINER_INFO
};

gchar *ITEM_INFO_PREDICATES [] = {
	"nie:InformationElement",
	#define ITEM_INFO(__i, __n)		__n,
	#include "item-conf.h"
	#undef ITEM_INFO
};
