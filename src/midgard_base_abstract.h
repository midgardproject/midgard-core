/* 
 * Copyright (C) 2011 Piotr Pokora <piotrek.pokora@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef _MIDGARD_BASE_ABSTRACT_H
#define _MIDGARD_BASE_ABSTRACT_H

#include <glib-object.h>
#include "midgard_dbobject.h"

#define MIDGARD_TYPE_BASE_ABSTRACT            (midgard_base_abstract_get_type())
#define MIDGARD_BASE_ABSTRACT(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), MIDGARD_TYPE_BASE_ABSTRACT, MidgardBaseAbstract))
#define MIDGARD_BASE_ABSTRACT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), MIDGARD_TYPE_BASE_ABSTRACT, MidgardBaseAbstractClass))
#define MIDGARD_IS_BASE_ABSTRACT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), MIDGARD_TYPE_BASE_ABSTRACT))
#define MIDGARD_IS_BASE_ABSTRACT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), MIDGARD_TYPE_BASE_ABSTRACT))
#define MIDGARD_BASE_ABSTRACT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), MIDGARD_TYPE_BASE_ABSTRACT, MidgardBaseAbstractClass))

typedef struct _MidgardBaseAbstract MidgardBaseAbstract;
typedef struct _MidgardBaseAbstractClass MidgardBaseAbstractClass;

struct _MidgardBaseAbstract {
	GObject parent;
};

struct _MidgardBaseAbstractClass {
	GObjectClass parent;
};

GType midgard_base_abstract_get_type(void);

#endif /* MIDGARD_BASE_ABSTRACT_H */
