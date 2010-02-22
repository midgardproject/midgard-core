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

#include "midgard_query_select.h"
#include "midgard_core_query.h"

MidgardQuerySelect *
midgard_query_select_new (MidgardConnection *mgd, MidgardQueryStorage *storage)
{
	g_return_val_if_fail (mgd != NULL, NULL);
	g_return_val_if_fail (storage != NULL, NULL);

	MidgardQuerySelect *self = g_object_new (MIDGARD_TYPE_QUERY_SELECT, NULL);
	self->priv->mgd = mgd;
	self->priv->storage = storage;

	return self;
}

gboolean
_midgard_query_select_set_constraint (MidgardQueryExecutor *executor, MidgardQuerySimpleConstraint *constraint)
{
	g_return_val_if_fail (executor != NULL, FALSE);
	g_return_val_if_fail (constraint != NULL, FALSE);

	MidgardQuerySelect *self = MIDGARD_QUERY_SELECT (executor);

	self->priv->constraint = constraint;
}

gboolean
_midgard_query_select_set_limit (MidgardQueryExecutor *executor, guint limit)
{
	g_return_val_if_fail (executor != NULL, FALSE);
	g_return_val_if_fail (limit > 0, FALSE);

	MidgardQuerySelect *self = MIDGARD_QUERY_SELECT (executor);

	self->priv->limit = limit;
}

gboolean 
_midgard_query_select_set_offset (MidgardQueryExecutor *executor, guint offset)
{
	g_return_val_if_fail (executor != NULL, FALSE);
	g_return_val_if_fail (offset > -1, FALSE);
	
	MidgardQuerySelect *self = MIDGARD_QUERY_SELECT (executor);

	self->priv->offset = offset;
}

static gchar* valid_order_types[] = {"ASC", "DESC", NULL};

gboolean
_midgard_query_select_add_order (MidgardQueryExecutor *executor, MidgardQueryProperty *property, const gchar *type)
{
	g_return_val_if_fail (executor != NULL, FALSE);
	g_return_val_if_fail (property != NULL, FALSE);

	MidgardQuerySelect *self = MIDGARD_QUERY_SELECT (executor);

	/* TODO */

	return FALSE;

}

gboolean
_midgard_query_select_add_join (MidgardQueryExecutor *executor, const gchar *join_type, 
		MidgardQueryProperty *left_property, MidgardQueryProperty *right_property)
{
	g_return_val_if_fail (executor != NULL, FALSE);
	g_return_val_if_fail (join_type != NULL, FALSE);
	g_return_val_if_fail (left_property != NULL, FALSE);
	g_return_val_if_fail (right_property != NULL, FALSE);

	MidgardQuerySelect *self = MIDGARD_QUERY_SELECT (executor);

	/* TODO */

	return FALSE;
}

gboolean 
_midgard_query_select_execute (MidgardQueryExecutor *executor)
{
	g_return_val_if_fail (executor != NULL, FALSE);

	MidgardQuerySelect *self = MIDGARD_QUERY_SELECT (executor);

	/* TODO */

	return FALSE;
}

/* GOBJECT ROUTINES */

static GObjectClass *parent_class= NULL;

static GObject *
_midgard_query_select_constructor (GType type,
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
_midgard_query_select_dispose (GObject *object)
{
	MidgardQuerySelect *self = MIDGARD_QUERY_SELECT (object);
	parent_class->dispose (object);
}

static void 
_midgard_query_select_finalize (GObject *object)
{
	MidgardQuerySelect *self = MIDGARD_QUERY_SELECT (object);

	parent_class->finalize;
}

static void 
_midgard_query_select_class_init (MidgardQuerySelectClass *klass, gpointer class_data)
{
       	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	parent_class = g_type_class_peek_parent (klass);

	object_class->constructor = _midgard_query_select_constructor;
	object_class->dispose = _midgard_query_select_dispose;
	object_class->finalize = _midgard_query_select_finalize;

	klass->set_constraint = _midgard_query_select_set_constraint;
	klass->set_limit = _midgard_query_select_set_limit;
	klass->set_offset = _midgard_query_select_set_offset;
	klass->add_order = _midgard_query_select_add_order;
	klass->add_join = _midgard_query_select_add_join;
	klass->execute = _midgard_query_select_execute;
}

GType
midgard_query_select_get_type (void)
{
   	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardQuerySelectClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_query_select_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardQuerySelect),
			0,              /* n_preallocs */
			NULL /* instance_init */
		};
		type = g_type_register_static (MIDGARD_TYPE_QUERY_EXECUTOR, "MidgardQuerySelect", &info, G_TYPE_FLAG_ABSTRACT);
	}
	return type;
}
