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

#include "midgard_query_holder.h"
#include "midgard_query_value.h"
#include "midgard_dbobject.h"

struct _MidgardQueryValuePrivate {
	GObject parent;
	GValue value;
};

#warning constructor and finalize missed for MidgardQueryValue

MidgardQueryValue *
midgard_query_value_new (const GValue *value)
{
	g_return_val_if_fail (value != NULL, NULL);
	MidgardQueryValue *self = g_object_new (MIDGARD_QUERY_VALUE_TYPE, NULL);

	GValue pval = {0, };
	g_value_init (&pval, G_VALUE_TYPE (value));
	g_value_copy (value, &pval);

	self->priv = g_new (MidgardQueryValuePrivate, 1);
	self->priv->value = pval;

	return self;
}

static void
__get_value (MidgardQueryHolder *self, GValue *value)
{
	g_return_if_fail (self != NULL);
	g_return_if_fail (self != NULL);

	MidgardQueryValue *mqp = (MidgardQueryValue *) self;

	g_value_init (value, G_VALUE_TYPE (&mqp->priv->value));
	g_value_copy ((const GValue *) &mqp->priv->value, value);

	return;
}

static gboolean
__set_value (MidgardQueryHolder *self, const GValue *value)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (value != NULL, FALSE);

	MidgardQueryValue *mqp = (MidgardQueryValue *) self;

	if (G_IS_VALUE (&mqp->priv->value))
		g_value_unset (&mqp->priv->value);

	g_value_init (&mqp->priv->value, G_VALUE_TYPE (value));
	g_value_copy (value, &mqp->priv->value);

	return TRUE;
}

/* GOBJECT ROUTINES */

static void
midgard_query_value_init (MidgardQueryHolderIFace *iface)
{
       iface->get_value = __get_value;
       iface->set_value = __set_value;
}

GType
midgard_query_value_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
	  	static const GTypeInfo info = {
			sizeof (MidgardQueryValueClass),
			NULL,   /* base_init */
			NULL,   /* base_finalize */
			NULL,   /* class_init */
			NULL,   /* class_finalize */
			NULL,   /* class_data */
			sizeof (MidgardQueryValue),
			0,      /* n_preallocs */
			NULL    /* instance_init */
		};
      
		static const GInterfaceInfo value_info = {
			(GInterfaceInitFunc) midgard_query_value_init,   
			NULL,	/* interface_finalize */
			NULL	/* interface_data */
		};

  		type = g_type_register_static (G_TYPE_OBJECT, "MidgardQueryValue", &info, 0);
		g_type_add_interface_static (type, MIDGARD_TYPE_QUERY_HOLDER, &value_info);
    	}
    	return type;
}

