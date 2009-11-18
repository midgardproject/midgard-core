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

#include "midgard_transaction.h"
#include "midgard_core_object.h"
#include "midgard_error.h"
#include "uuid.h"

struct _MidgardTransaction{
	GObject parent;

	/* < private > */
	MidgardConnection *mgd;
	const gchar *name;
};

#define _ASSERT_T_MGD(_s) { \
	g_assert(_s->mgd != NULL); \
	g_assert(_s->mgd->priv->connection != NULL); }

#define _T_CNC(_s) _s->mgd->priv->connection;

/**
 * midgard_transaction_new:
 * @mgd:#MidgardConnection instance
 *
 * Returns: New #MidgardTransaction instance or NULL on failure
 * 
 * Since: 9.09
 */
MidgardTransaction*	
midgard_transaction_new (MidgardConnection *mgd) 
{
	g_return_val_if_fail(mgd != NULL, NULL);

	MidgardTransaction *self = g_object_new(MIDGARD_TYPE_TRANSACTION, NULL);

	if (!self)
		return NULL;

	self->mgd = mgd;

	return self;
}

/**
 * midgard_transaction_begin:
 * @self: #MidgardTransaction instance
 * 
 * Begins new, underlying database provider's transaction.
 * In case of error, #MidgardConnection error is set to MGD_ERR_INTERNAL.
 *
 * Returns: %TRUE on success, %FALSE otherwise.
 * 
 * Since: 9.09
 */
gboolean
midgard_transaction_begin (MidgardTransaction *self)
{
	_ASSERT_T_MGD(self);

	gboolean rv = FALSE;
	GdaConnection *cnc = _T_CNC(self);
	MidgardConnection *mgd = self->mgd;
	GError *error = NULL;

	g_debug("Begin named transaction '%s'", self->name);

	rv = gda_connection_begin_transaction(cnc, self->name, 
			GDA_TRANSACTION_ISOLATION_UNKNOWN, &error);

	if (!error && rv)
		return TRUE;

	midgard_set_error(mgd,
			MGD_GENERIC_ERROR,
			MGD_ERR_INTERNAL,
			error && error->message ? error->message : " Unknown error.");

	if (error)
		g_error_free(error);

	return FALSE;
}

/**
 * midgard_transaction_commit:
 * @self: #MidgardTransaction instance
 * 
 * In case of error, #MidgardConnection error is set to MGD_ERR_INTERNAL.
 *
 * Returns: %TRUE on success, %FALSE otherwise
 * 
 * Since: 9.09
 */ 
gboolean
midgard_transaction_commit (MidgardTransaction *self)
{
	_ASSERT_T_MGD(self);

	gboolean rv = FALSE;
	GdaConnection *cnc = _T_CNC(self);
	MidgardConnection *mgd = self->mgd;
	GError *error = NULL;

	g_debug("Commit named transaction '%s'", self->name);

	rv = gda_connection_commit_transaction(cnc, self->name, &error);

	if (!error && rv)
		return TRUE;

	midgard_set_error(mgd,
			MGD_GENERIC_ERROR,
			MGD_ERR_INTERNAL,
			error && error->message ? error->message : " Unknown error.");

	if (error)
		g_error_free(error);

	return FALSE;
}

/**
 * midgard_transaction_rollback:
 * @self: #MidgardTransaction instance
 *
 * In case of error, #MidgardConnection error is set to MGD_ERR_INTERNAL
 * 
 * Returns: %TRUE on success, %FALSE otherwise.
 * 
 * Since: 9.09
 */
gboolean
midgard_transaction_rollback (MidgardTransaction *self)
{
	_ASSERT_T_MGD(self);

	gboolean rv = FALSE;
	GdaConnection *cnc = _T_CNC(self);
	MidgardConnection *mgd = self->mgd;
	GError *error = NULL;

	g_debug("Rollback named transaction '%s'", self->name);

	rv = gda_connection_rollback_transaction(cnc, self->name, &error);

	if (!error && rv)
		return TRUE;

	midgard_set_error(mgd,
			MGD_GENERIC_ERROR,
			MGD_ERR_INTERNAL,
			error && error->message ? error->message : " Unknown error.");

	if (error)
		g_error_free(error);

	return FALSE;
}

/**
 * midgard_transaction_get_status:
 * @self: #MidgardTransaction instance
 *
 * Returns transaction status. %FALSE means, any transaction operation failed.
 * No #MidgardConnection error is set in case of error.
 *
 * Returns: %TRUE on success, %FALSE otherwise
 * 
 * Since: 9.09
 */
gboolean
midgard_transaction_get_status (MidgardTransaction *self)
{
	_ASSERT_T_MGD(self);
	
	GdaConnection *cnc = _T_CNC(self);

	if (!cnc)
		return FALSE;

	GdaTransactionStatus *status = 
	       gda_connection_get_transaction_status(cnc);	
	
	if (status->state == 0)
		return TRUE;

	return FALSE;
}

/**
 * midgard_transaction_get_name:
 * @self: #MidgardTransaction instance
 *
 * Returns: unique name which identifies given transaction.
 * 
 * Since: 9.09
 */ 
const gchar*
midgard_transaction_get_name (MidgardTransaction *self)
{
	g_assert(self != NULL);

	return self->name;
}

/* GOBJECT ROUTINES */

static void __midgard_transaction_instance_init(
		GTypeInstance *instance, gpointer g_class)
{
	MidgardTransaction *self = (MidgardTransaction *) instance;

	/* Point is to have unique name per transaction:
	   1. We do not use guid as MidgardConnection pointer is required for this.
	   2. SQLite has parser issues when transaction name begins with numeric value.
	   3. SQlite has parser issues when transaction name has dashes '-'.
	   That's why we create new 'mgd' prefixed uuid like string. */
	gchar *uuid = midgard_uuid_new();
	uuid = g_strdelimit(uuid, "-", '_');
	self->name = (const gchar *)g_strconcat("mgd", uuid, NULL);
	g_free(uuid);

	self->mgd = NULL;
}


static void __midgard_transaction_finalize(GObject *object)
{
	g_assert(object != NULL);

	MidgardTransaction *self = (MidgardTransaction *)object;

	g_free((gchar *)self->name);
	self->name = NULL;
}

static void __midgard_transaction_class_init(
		gpointer g_class, gpointer g_class_data)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);		
	gobject_class->finalize = __midgard_transaction_finalize;
}

GType
midgard_transaction_get_type(void)
{
      static GType type = 0;

      if (type == 0) {

	      static const GTypeInfo info = {
		      sizeof (MidgardTransactionClass),
		      NULL,           /* base_init */
		      NULL,           /* base_finalize */
		      (GClassInitFunc) __midgard_transaction_class_init,
		      NULL,           /* class_finalize */
		      NULL,           /* class_data */
		      sizeof (MidgardTransaction),
		      0,              /* n_preallocs */
		      (GInstanceInitFunc) __midgard_transaction_instance_init/* instance_init */
	      };

	      type = g_type_register_static (G_TYPE_OBJECT, "midgard_transaction", &info, 0);
      }

      return type;
}
