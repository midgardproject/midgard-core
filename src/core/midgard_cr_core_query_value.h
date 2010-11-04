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

#ifndef MIDGARD_CR_CORE_QUERY_VALUE_H
#define MIDGARD_CR_CORE_QUERY_VALUE_H

#include <glib-object.h>

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_CR_CORE_TYPE_QUERY_VALUE (midgard_cr_core_query_value_get_type()) 
#define MIDGARD_CR_CORE_QUERY_VALUE(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_CR_CORE_TYPE_QUERY_VALUE, MidgardCRCoreQueryValue))
#define MIDGARD_CR_CORE_QUERY_VALUE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_CR_CORE_TYPE_QUERY_VALUE, MidgardCRCoreQueryValueClass))
#define MIDGARD_CR_CORE_IS_QUERY_VALUE(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_CR_CORE_TYPE_QUERY_VALUE))
#define MIDGARD_CR_CORE_IS_QUERY_VALUE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_CR_CORE_TYPE_QUERY_VALUE))
#define MIDGARD_CR_CORE_QUERY_VALUE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_CR_CORE_TYPE_QUERY_VALUE, MidgardCRCoreQueryValueClass))

typedef struct _MidgardCRCoreQueryValue MidgardCRCoreQueryValue;
typedef struct _MidgardCRCoreQueryValuePrivate MidgardCRCoreQueryValuePrivate;
typedef struct _MidgardCRCoreQueryValueClass MidgardCRCoreQueryValueClass; 

struct _MidgardCRCoreQueryValue {
	GObject parent;

	/* < private > */
	MidgardCRCoreQueryValuePrivate *priv;
};

struct _MidgardCRCoreQueryValueClass {
	GObjectClass parent;

	void			(*get_value)	(MidgardCRCoreQueryValue *self, GValue *value);
	MidgardCRCoreQueryValue 	*(*set_value)	(MidgardCRCoreQueryValue *self, const GValue *value);
};

GType 			midgard_cr_core_query_value_get_type		(void);
MidgardCRCoreQueryValue	*midgard_cr_core_query_value_new		(void);
MidgardCRCoreQueryValue 	*midgard_cr_core_query_value_create_with_value 	(const GValue *value);

G_END_DECLS

#endif /* MIDGARD_CR_CORE_QUERY_VALUE_H */
