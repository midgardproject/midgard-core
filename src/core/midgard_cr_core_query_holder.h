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

#ifndef MIDGARD_CR_CORE_QUERY_HOLDER_H
#define MIDGARD_CR_CORE_QUERY_HOLDER_H

#include <glib-object.h>

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_CR_CORE_TYPE_QUERY_HOLDER (midgard_cr_core_query_holder_get_type()) 
#define MIDGARD_CR_CORE_QUERY_HOLDER(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_CR_CORE_TYPE_QUERY_HOLDER, MidgardCRCoreQueryHolder))
#define MIDGARD_CR_CORE_IS_QUERY_HOLDER(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_CR_CORE_TYPE_QUERY_HOLDER))
#define MIDGARD_CR_CORE_QUERY_HOLDER_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), MIDGARD_CR_CORE_TYPE_QUERY_HOLDER, MidgardCRCoreQueryHolderIFace))

typedef struct _MidgardCRCoreQueryHolder MidgardCRCoreQueryHolder; /* dummy object */
typedef struct _MidgardCRCoreQueryHolderIFace MidgardCRCoreQueryHolderIFace; 

struct _MidgardCRCoreQueryHolderIFace {
	GTypeInterface	parent;

	void			(*get_value)	(MidgardCRCoreQueryHolder *self, GValue *value);
	gboolean	 	(*set_value)	(MidgardCRCoreQueryHolder *self, const GValue *value);
};

GType 			midgard_cr_core_query_holder_get_type	(void);
void			midgard_cr_core_query_holder_get_value	(MidgardCRCoreQueryHolder *self, GValue *value);
gboolean		midgard_cr_core_query_holder_set_value	(MidgardCRCoreQueryHolder *self, const GValue *value);

G_END_DECLS

#endif /* MIDGARD_CR_CORE_QUERY_HOLDER_H */
