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

#include "midgard_cr_core_transaction.h"
#include "midgard_cr_core_uuid.h"
#include "midgardcr.h"
#include <libgda/libgda.h>

struct _MidgardCRCoreTransactionPrivate {
	MidgardCRSQLStorageManager *manager;
	const gchar *name;
};

#define _ASSERT_T_MGD(_s) { \
	g_assert(_s->priv->manager != NULL); \
	g_assert(_s->priv->manager->_cnc != NULL); }

#define _T_CNC(_s) (GdaConnection*)_s->priv->manager->_cnc;

MidgardCRCoreTransaction*	
midgard_cr_core_transaction_new (MidgardCRSQLStorageManager *manager) 
{
	g_return_val_if_fail (manager != NULL, NULL);

	MidgardCRCoreTransaction *self = g_object_new (MIDGARD_CR_CORE_TYPE_TRANSACTION, NULL);
	
	if (!self)
		return NULL;

	self->priv->manager = g_object_ref (manager);
	return self;
}

void
midgard_cr_core_transaction_begin (MidgardCRCoreTransaction *self, GError **error)
{
	_ASSERT_T_MGD (self);

	gboolean rv = FALSE;
	GdaConnection *cnc = _T_CNC (self);
	MidgardCRSQLStorageManager *manager = self->priv->manager;
	GError *err = NULL;

	rv = gda_connection_begin_transaction (cnc, self->priv->name, GDA_TRANSACTION_ISOLATION_UNKNOWN, &err);

	if (err)
		g_propagate_error (error, err);

	if (!rv && !err)
		g_warning ("Failed to begin underlying database transaction and no error has been set");

	return;
}

void
midgard_cr_core_transaction_commit (MidgardCRCoreTransaction *self, GError **error)
{
	_ASSERT_T_MGD (self);

	gboolean rv = FALSE;
	GdaConnection *cnc = _T_CNC (self);
	MidgardCRSQLStorageManager *manager = self->priv->manager;
	GError *err = NULL;

	rv = gda_connection_commit_transaction (cnc, self->priv->name, &err);

	if (err)
		g_propagate_error (error, err);

	if (!rv && !err)
		g_warning ("Failed to commit underlying database transaction and no error has been set");

	return;
}

void
midgard_cr_core_transaction_rollback (MidgardCRCoreTransaction *self, GError **error)
{
	_ASSERT_T_MGD (self);

	gboolean rv = FALSE;
	GdaConnection *cnc = _T_CNC (self);
	MidgardCRSQLStorageManager *manager = self->priv->manager;
	GError *err = NULL;

	rv = gda_connection_rollback_transaction (cnc, self->priv->name, &err);

	if (err)
		g_propagate_error (error, err);

	if (!rv && !err)
		g_warning ("Failed to rollback underlying database transaction and no error has been set");

	return;
}

gboolean
midgard_cr_core_transaction_get_status (MidgardCRCoreTransaction *self)
{
	_ASSERT_T_MGD (self);
	GdaConnection *cnc = _T_CNC (self);

	if (!cnc)
		return FALSE;

	GdaTransactionStatus *status = gda_connection_get_transaction_status(cnc);	
	if (status->state == 0)
		return TRUE;

	return FALSE;
}

/**
 * midgard_cr_core_transaction_get_name:
 * @self: #MidgardCRCoreTransaction instance
 *
 * Returns: unique name which identifies given transaction.
 * 
 * Since: 9.09
 */ 
const gchar*
midgard_cr_core_transaction_get_name (MidgardCRCoreTransaction *self)
{
	g_assert(self != NULL);

	return self->priv->name;
}

/* GOBJECT ROUTINES */

static GObjectClass *__parent_class= NULL;

static void __midgard_cr_core_transaction_instance_init(
		GTypeInstance *instance, gpointer g_class)
{
	MidgardCRCoreTransaction *self = (MidgardCRCoreTransaction *) instance;

	/* Point is to have unique name per transaction:
	   1. We do not use guid as MidgardConnection pointer is required for this.
	   2. SQLite has parser issues when transaction name begins with numeric value.
	   3. SQlite has parser issues when transaction name has dashes '-'.
	   That's why we create new 'midgardcr' prefixed uuid like string. */
	gchar *uuid = midgard_cr_core_uuid_new();
	uuid = g_strdelimit(uuid, "-", '_');

	self->priv = g_new (MidgardCRCoreTransactionPrivate, 1);
	self->priv->name = (const gchar *)g_strconcat("midgardcr", uuid, NULL);
	g_free(uuid);

	self->priv->manager = NULL;
}

static void
__midgard_cr_core_transaction_dispose (GObject *object)
{
	MidgardCRCoreTransaction *self = MIDGARD_CR_CORE_TRANSACTION (object);
	if (self->priv->manager != NULL) {
		g_object_unref (self->priv->manager);
		self->priv->manager = NULL;
	}

	__parent_class->dispose (object);
}

static void __midgard_cr_core_transaction_finalize(GObject *object)
{
	g_assert(object != NULL);

	MidgardCRCoreTransaction *self = (MidgardCRCoreTransaction *)object;

	g_free((gchar *)self->priv->name);
	self->priv->name = NULL;

	g_free (self->priv);
	self->priv = NULL;
}

static void __midgard_cr_core_transaction_class_init(
		gpointer g_class, gpointer g_class_data)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);		
	gobject_class->finalize = __midgard_cr_core_transaction_finalize;
	gobject_class->dispose = __midgard_cr_core_transaction_dispose;

	__parent_class = g_type_class_peek_parent (gobject_class);
}

GType
midgard_cr_core_transaction_get_type(void)
{
      static GType type = 0;

      if (type == 0) {

	      static const GTypeInfo info = {
		      sizeof (MidgardCRCoreTransactionClass),
		      NULL,           /* base_init */
		      NULL,           /* base_finalize */
		      (GClassInitFunc) __midgard_cr_core_transaction_class_init,
		      NULL,           /* class_finalize */
		      NULL,           /* class_data */
		      sizeof (MidgardCRCoreTransaction),
		      0,              /* n_preallocs */
		      (GInstanceInitFunc) __midgard_cr_core_transaction_instance_init/* instance_init */
	      };

	      type = g_type_register_static (G_TYPE_OBJECT, "MidgardCRCoreTransaction", &info, 0);
      }

      return type;
}
