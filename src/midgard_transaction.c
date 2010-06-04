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

struct _MidgardTransactionPrivate {
	MidgardConnection *mgd;
	const gchar *name;
};

#define _ASSERT_T_MGD(_s) { \
	g_assert(_s->priv->mgd != NULL); \
	g_assert(_s->priv->mgd->priv->connection != NULL); }

#define _T_CNC(_s) _s->priv->mgd->priv->connection;

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

	MidgardTransaction *self = g_object_new (MIDGARD_TYPE_TRANSACTION, "connection", mgd, NULL);

	if (!self)
		return NULL;

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
	MidgardConnection *mgd = self->priv->mgd;
	GError *error = NULL;

	g_debug("Begin named transaction '%s'", self->priv->name);

	rv = gda_connection_begin_transaction(cnc, self->priv->name, 
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
	MidgardConnection *mgd = self->priv->mgd;
	GError *error = NULL;

	g_debug("Commit named transaction '%s'", self->priv->name);

	rv = gda_connection_commit_transaction(cnc, self->priv->name, &error);

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
	MidgardConnection *mgd = self->priv->mgd;
	GError *error = NULL;

	g_debug("Rollback named transaction '%s'", self->priv->name);

	rv = gda_connection_rollback_transaction(cnc, self->priv->name, &error);

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

	return self->priv->name;
}

/* GOBJECT ROUTINES */

static GObjectClass *__parent_class= NULL;

enum {
	PROPERTY_CONNECTION = 1
};

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

	self->priv = g_new (MidgardTransactionPrivate, 1);
	self->priv->name = (const gchar *)g_strconcat("mgd", uuid, NULL);
	g_free(uuid);

	self->priv->mgd = NULL;
}

static void
__midgard_transaction_dispose (GObject *object)
{
	MidgardTransaction *self = MIDGARD_TRANSACTION (object);
	if (self->priv->mgd != NULL) {
		g_object_unref (self->priv->mgd);
		self->priv->mgd = NULL;
	}

	__parent_class->dispose (object);
}

static void __midgard_transaction_finalize(GObject *object)
{
	g_assert(object != NULL);

	MidgardTransaction *self = (MidgardTransaction *)object;

	g_free((gchar *)self->priv->name);
	self->priv->name = NULL;

	g_free (self->priv);
	self->priv = NULL;
}

static void
__midgard_transaction_get_property (GObject *object, guint property_id,
		GValue *value, GParamSpec *pspec)
{
	switch (property_id) {
		
		case PROPERTY_CONNECTION:
			/* write and construct only */			
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
__midgard_transaction_set_property (GObject *object, guint property_id,
		const GValue *value, GParamSpec *pspec)
{
	GObject *mgd;
	switch (property_id) {

		case PROPERTY_CONNECTION:
			/* Add new reference to MidgardConnection object */
			if (!G_VALUE_HOLDS_OBJECT (value))
				return;
			MIDGARD_TRANSACTION (object)->priv->mgd = g_value_dup_object (value);
			break;

  		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void __midgard_transaction_class_init(
		gpointer g_class, gpointer g_class_data)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);		
	gobject_class->finalize = __midgard_transaction_finalize;
	gobject_class->dispose = __midgard_transaction_dispose;
	gobject_class->set_property = __midgard_transaction_set_property;
	gobject_class->get_property = __midgard_transaction_get_property;

	__parent_class = g_type_class_peek_parent (gobject_class);

	/* Properties */
	GParamSpec *pspec = g_param_spec_object ("connection",
			"",
			"",
			MIDGARD_TYPE_CONNECTION,
			G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY);
	/**
	 * MidgardTransaction:connection:
	 * 
	 * Pointer to #MidgardConnection, given object has been initialized for
	 */  
	g_object_class_install_property (gobject_class, PROPERTY_CONNECTION, pspec);
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

	      type = g_type_register_static (G_TYPE_OBJECT, "MidgardTransaction", &info, 0);
      }

      return type;
}
