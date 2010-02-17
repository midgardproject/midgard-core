/* 
 * Copyright (C) 2010 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef MIDGARD_QUERY_VALUE_H
#define MIDGARD_QUERY_VALUE_H

#include <glib-object.h>

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_QUERY_VALUE_TYPE (midgard_query_value_get_type()) 
#define MIDGARD_QUERY_VALUE(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_QUERY_VALUE_TYPE, MidgardQueryValue))
#define MIDGARD_QUERY_VALUE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_QUERY_VALUE_TYPE, MidgardQueryValueClass))
#define MIDGARD_IS_QUERY_VALUE(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_QUERY_VALUE_TYPE))
#define MIDGARD_IS_QUERY_VALUE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_QUERY_VALUE_TYPE))
#define MIDGARD_QUERY_VALUE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_QUERY_VALUE_TYPE, MidgardQueryValueClass))

typedef struct _MidgardQueryValue MidgardQueryValue;
typedef struct _MidgardQueryValueClass MidgardQueryValueClass; 

struct _MidgardQueryValueClass {
	GObjectClass parent;

	void			(*get_value)	(MidgardQueryValue *self, GValue *value);
	MidgardQueryValue 	*(*set_value)	(MidgardQueryValue *self);

};

GType 			midgard_query_value_get_type	(void);
MidgardQueryValue	*midgard_query_value_new	(const GValue *value);

G_END_DECLS

#endif /* MIDGARD_QUERY_VALUE_H */
