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

#ifndef MIDGARD_CR_CORE_TRANSACTION_H
#define MIDGARD_CR_CORE_TRANSACTION_H

#include "midgardcr.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_CR_CORE_TYPE_TRANSACTION (midgard_cr_core_transaction_get_type())
#define MIDGARD_CR_CORE_TRANSACTION(object)  \
	        (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_CR_CORE_TYPE_TRANSACTION, MidgardCRCoreTransaction))
#define MIDGARD_CR_CORE_TRANSACTION_CLASS(klass)  \
	        (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_CR_CORE_TYPE_TRANSACTION, MidgardCRCoreTransactionClass))
#define MIDGARD_CR_CORE_IS_TRANSACTION(object)   \
	        (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_CR_CORE_TYPE_TRANSACTION))
#define MIDGARD_CR_CORE_IS_TRANSACTION_CLASS(klass) \
	        (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_CR_CORE_TYPE_TRANSACTION))
#define MIDGARD_CR_CORE_TRANSACTION_GET_CLASS(obj) \
	        (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_CR_CORE_TYPE_TRANSACTION, MidgardCRCoreTransactionClass))

typedef struct _MidgardCRCoreTransaction MidgardCRCoreTransaction;
typedef struct _MidgardCRCoreTransactionPrivate MidgardCRCoreTransactionPrivate;
typedef struct _MidgardCRCoreTransactionClass MidgardCRCoreTransactionClass;

struct _MidgardCRCoreTransaction {
	GObject parent;

	/* < private > */
	MidgardCRCoreTransactionPrivate *priv;
};

struct _MidgardCRCoreTransactionClass{
	GObjectClass parent;
};

GType			midgard_cr_core_transaction_get_type			(void);
MidgardCRCoreTransaction	*midgard_cr_core_transaction_new		(MidgardCRSQLStorageManager *manager);
void 				midgard_cr_core_transaction_begin		(MidgardCRCoreTransaction *self, GError **error);
void 				midgard_cr_core_transaction_commit		(MidgardCRCoreTransaction *self, GError **error);
void				midgard_cr_core_transaction_rollback		(MidgardCRCoreTransaction *self, GError **error);
gboolean			midgard_cr_core_transaction_get_status		(MidgardCRCoreTransaction *self);
const gchar 			*midgard_cr_core_transaction_get_name		(MidgardCRCoreTransaction *self);

G_END_DECLS

#endif /* MIDGARD_CR_CORE_TRANSACTION_H */
