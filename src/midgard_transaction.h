/* 
 * Copyright (C) 2009 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef MIDGARD_TRANSACTION_H
#define MIDGARD_TRANSACTION_H

#include "midgard_defs.h"
#include "midgard_connection.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_TRANSACTION (midgard_transaction_get_type())
#define MIDGARD_TRANSACTION(object)  \
	        (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_TRANSACTION, MidgardTransaction))
#define MIDGARD_TRANSACTION_CLASS(klass)  \
	        (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_TRANSACTION, MidgardTransactionClass))
#define MIDGARD_IS_TRANSACTION(object)   \
	        (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_TRANSACTION))
#define MIDGARD_IS_TRANSACTION_CLASS(klass) \
	        (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_TRANSACTION))
#define MIDGARD_TRANSACTION_GET_CLASS(obj) \
	        (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_TRANSACTION, MidgardTransactionClass))

typedef struct _MidgardTransaction MidgardTransaction;
typedef struct _MidgardTransactionClass MidgardTransactionClass;

struct _MidgardTransactionClass{
	GObjectClass parent;

	/* class members */
	gboolean	(*begin)		(MidgardTransaction *self);
	gboolean	(*commit)		(MidgardTransaction *self);
	gboolean	(*rollback)		(MidgardTransaction *self);
	gboolean	(*get_status)		(MidgardTransaction *self);
	const gchar 	*(*get_name)		(MidgardTransaction *self);
};

GType			midgard_transaction_get_type			(void);
MidgardTransaction	*midgard_transaction_new			(MidgardConnection *mgd);
gboolean 		midgard_transaction_begin			(MidgardTransaction *self);
gboolean 		midgard_transaction_commit			(MidgardTransaction *self);
gboolean 		midgard_transaction_rollback			(MidgardTransaction *self);
gboolean		midgard_transaction_get_status			(MidgardTransaction *self);
const gchar 		*midgard_transaction_get_name			(MidgardTransaction *self);

G_END_DECLS

#endif /* MIDGARD_TRANSACTION_H */
