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

#include "midgard_cr_core_query_executor.h"
#include "midgard_cr_core_query_private.h"

/* MidgardCRCoreQueryExecutor properties */
enum {
	PROPERTY_RESULTS_COUNT = 1
};

MidgardCRCoreQueryExecutor *
midgard_cr_core_query_executor_new (GObject *manager, MidgardCRCoreQueryStorage *storage)
{
	return NULL;
}

/**
 * midgard_cr_core_query_executor_set_constraint:
 * @self: #MidgardCRCoreQueryExecutor instance
 * @constraint: #MidgardCRCoreConstraintSimple instance
 *
 * Set constraint object which will be used for query execution
 *
 * Returns: %TRUE on success, %FALSE otherwise
 *
 * Since: 10.05
 */ 
gboolean
midgard_cr_core_query_executor_set_constraint (MidgardCRCoreQueryExecutor *self, MidgardCRCoreQueryConstraintSimple *constraint)
{
	return MIDGARD_CR_CORE_QUERY_EXECUTOR_GET_CLASS (self)->set_constraint (self, constraint);
}

/**
 * midgard_cr_core_query_executor_set_limit:
 * @self: #MidgardCRCoreQueryExecutor instance
 * @limit: execution limit
 *
 * Returns: %TRUE on success, %FALSE otherwise
 *
 * Since: 10.05
 */ 
gboolean
midgard_cr_core_query_executor_set_limit (MidgardCRCoreQueryExecutor *self, guint limit)
{
	return MIDGARD_CR_CORE_QUERY_EXECUTOR_GET_CLASS (self)->set_limit (self, limit);
}

/**
 * midgard_cr_core_query_executor_set_offset:
 * @self: #MidgardCRCoreQueryExecutor instance
 * @offset: execution offset
 *
 * Returns: %TRUE on success, %FALSE otherwise
 *
 * Since: 10.05
 */ 
gboolean midgard_cr_core_query_executor_set_offset (MidgardCRCoreQueryExecutor *self, guint offset)
{
	return MIDGARD_CR_CORE_QUERY_EXECUTOR_GET_CLASS (self)->set_offset (self, offset);
}

/**
 * midgard_cr_core_query_executor_add_order:
 * @self: #MidgardCRCoreQueryExecutor instance
 * @type: execution order
 *
 * Returns: %TRUE on success, %FALSE otherwise
 *
 * Since: 10.05
 */ 
gboolean
midgard_cr_core_query_executor_add_order (MidgardCRCoreQueryExecutor *self, MidgardCRCoreQueryProperty *property, const gchar *type)
{
	return MIDGARD_CR_CORE_QUERY_EXECUTOR_GET_CLASS (self)->add_order (self, property, type);
}

gboolean
midgard_cr_core_query_executor_add_join (MidgardCRCoreQueryExecutor *self, const gchar *join_type, 
		MidgardCRCoreQueryProperty *left_property, MidgardCRCoreQueryProperty *right_property)
{
	return MIDGARD_CR_CORE_QUERY_EXECUTOR_GET_CLASS (self)->add_join (self, join_type, left_property, right_property);
}

/**
 * midgard_cr_core_query_executor_get_results_count:
 * @self: #MidgardCRCoreQueryExecutor instance
 *
 * Returns: number of objects or records returned from execution
 *
 * Since: 10.05
 */ 
guint 
midgard_cr_core_query_executor_get_results_count (MidgardCRCoreQueryExecutor *self)
{
	return MIDGARD_CR_CORE_QUERY_EXECUTOR_GET_CLASS (self)->get_results_count (self);
}

/**
 * midgard_cr_core_query_executor_execute:
 * @self: #MidgardCRCoreQueryExecutor instance
 *
 * Returns: %TRUE on success, %FALSE otherwise
 *
 * Since: 10.05
 */ 
gboolean
midgard_cr_core_query_executor_execute (MidgardCRCoreQueryExecutor *self, GError **error)
{
	return MIDGARD_CR_CORE_QUERY_EXECUTOR_GET_CLASS (self)->execute (self, error);
}

/* GOBJECT ROUTINES */

static GObjectClass *parent_class= NULL;

static void
__midgard_cr_core_query_executor_instance_init (GTypeInstance *instance, gpointer g_class)
{
	MidgardCRCoreQueryExecutor *object = (MidgardCRCoreQueryExecutor *) instance;

	MIDGARD_CR_CORE_QUERY_EXECUTOR (object)->priv = g_new (MidgardCRCoreQueryExecutorPrivate, 1);
	MIDGARD_CR_CORE_QUERY_EXECUTOR (object)->priv->storage_manager = NULL;
	MIDGARD_CR_CORE_QUERY_EXECUTOR (object)->priv->storage = NULL;
	MIDGARD_CR_CORE_QUERY_EXECUTOR (object)->priv->constraint = NULL;
	MIDGARD_CR_CORE_QUERY_EXECUTOR (object)->priv->n_constraints = 0;
	MIDGARD_CR_CORE_QUERY_EXECUTOR (object)->priv->orders = NULL;
	MIDGARD_CR_CORE_QUERY_EXECUTOR (object)->priv->joins = NULL;
	MIDGARD_CR_CORE_QUERY_EXECUTOR (object)->priv->limit = -1;
	MIDGARD_CR_CORE_QUERY_EXECUTOR (object)->priv->offset = -1;
	MIDGARD_CR_CORE_QUERY_EXECUTOR (object)->priv->resultset = NULL;
	MIDGARD_CR_CORE_QUERY_EXECUTOR (object)->priv->tableid = 0;
	MIDGARD_CR_CORE_QUERY_EXECUTOR (object)->priv->table_alias = NULL;
	MIDGARD_CR_CORE_QUERY_EXECUTOR (object)->priv->stmt = NULL;
	MIDGARD_CR_CORE_QUERY_EXECUTOR (object)->priv->joinid = 0;
	MIDGARD_CR_CORE_QUERY_EXECUTOR (object)->priv->results_count = 0;
	MIDGARD_CR_CORE_QUERY_EXECUTOR (object)->priv->read_only = TRUE;
	MIDGARD_CR_CORE_QUERY_EXECUTOR (object)->priv->include_deleted = FALSE;
	MIDGARD_CR_CORE_QUERY_EXECUTOR (object)->priv->include_deleted_targets = NULL;
}

static GObject *
_midgard_cr_core_query_executor_constructor (GType type,
		guint n_construct_properties,
		GObjectConstructParam *construct_properties)
{
	GObject *object = (GObject *)
		G_OBJECT_CLASS (parent_class)->constructor (type,
				n_construct_properties,
				construct_properties);

	return G_OBJECT(object);
}

static void
_midgard_cr_core_query_executor_dispose (GObject *object)
{
	MidgardCRCoreQueryExecutor *self = (MidgardCRCoreQueryExecutor *) object;

	if (MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->storage_manager) {
		g_object_unref (MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->storage_manager);
		MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->storage_manager = NULL;
	}
	
	if (MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->storage) {
		g_object_unref (MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->storage);
		MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->storage = NULL;
	}

	if (MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->constraint) {
		g_object_unref (MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->constraint);
		MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->constraint = NULL;
	}

	parent_class->dispose (object);
}

static void 
_midgard_cr_core_query_executor_finalize (GObject *object)
{
	MidgardCRCoreQueryExecutor *self = MIDGARD_CR_CORE_QUERY_EXECUTOR (object);

	if (self->priv->joins) {
		g_slist_foreach (self->priv->joins, (GFunc) g_free, NULL);
		g_slist_free (self->priv->joins);
		self->priv->joins = NULL;
	}

	if (self->priv->orders) {
		g_slist_foreach (self->priv->orders, (GFunc) g_free, NULL);
		g_slist_free (self->priv->orders);
		self->priv->orders = NULL;
	}

	g_free (self->priv->table_alias);
	self->priv->table_alias = NULL;

	if (self->priv->include_deleted_targets) {
		g_slist_free (self->priv->include_deleted_targets);
		self->priv->include_deleted_targets = NULL;
	}

	g_free (self->priv);
	self->priv = NULL;

	parent_class->finalize (object);
}

static void
_midgard_cr_core_query_executor_get_property (GObject *object, guint property_id,
		GValue *value, GParamSpec *pspec)
{
	MidgardCRCoreQueryExecutor *self = MIDGARD_CR_CORE_QUERY_EXECUTOR (object);

	switch (property_id) {
		
		case PROPERTY_RESULTS_COUNT:
			g_value_set_uint (value, self->priv->results_count);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(self, property_id, pspec);
			break;
	}
}

static void 
_midgard_cr_core_query_executor_class_init (MidgardCRCoreQueryExecutorClass *klass, gpointer class_data)
{
       	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	parent_class = g_type_class_peek_parent (klass);

	object_class->constructor = _midgard_cr_core_query_executor_constructor;
	object_class->dispose = _midgard_cr_core_query_executor_dispose;
	object_class->finalize = _midgard_cr_core_query_executor_finalize;
	object_class->get_property = _midgard_cr_core_query_executor_get_property;

	GParamSpec *pspec = g_param_spec_uint ("resultscount",
			"Number of objects.", "",
			0, G_MAXUINT32, 0, G_PARAM_READABLE);
	g_object_class_install_property (object_class,
			PROPERTY_RESULTS_COUNT, pspec);

	klass->set_constraint = NULL;
	klass->set_limit = NULL;
	klass->set_offset = NULL;
	klass->add_order = NULL;
	klass->add_join = NULL;
	klass->execute = NULL;
	klass->get_results_count = NULL;
}

GType
midgard_cr_core_query_executor_get_type (void)
{
   	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardCRCoreQueryExecutorClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_cr_core_query_executor_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardCRCoreQueryExecutor),
			0,              /* n_preallocs */
			__midgard_cr_core_query_executor_instance_init /* instance_init */
		};
		type = g_type_register_static (G_TYPE_OBJECT, "MidgardCRCoreQueryExecutor", &info, G_TYPE_FLAG_ABSTRACT);
	}
	return type;
}
