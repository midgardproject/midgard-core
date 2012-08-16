/* 
 * Copyright (C) 2012 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_content_manager.h"

/**
 * midgard_content_manager_error_quark: (skip)
 * 
 * Returns: MIDGARD_CONTENT_MANAGER_ERROR quark
 */
GQuark
midgard_content_manager_error_quark (void)
{
	static GQuark q = 0;
	if (q == 0)
		q = g_quark_from_static_string ("midgard-content-manager-error-quark");
	return q;
}

struct _MidgardContentManager {
	GObject parent;
};

/**
 * midgard_content_manager_get_by_id:
 * @self: #MidgardContentManager instance
 * @reference: #MidgardObjectReference instance
 * @model: #MidgardModel instance
 * @error: pointer to store returned error
 *
 * From storage, get object which is identified by @reference.
 *
 * Returns: (transfer full): GObject derived instance or NULL
 *
 * Since: 10.05.8
 */ 
GObject*
midgard_content_manager_get_by_id (MidgardContentManager *self, MidgardObjectReference *reference, MidgardModel *model, GError **error)
{
	return MIDGARD_CONTENT_MANAGER_GET_INTERFACE (self)->get_by_id (self, reference, model, error);
}

/**
 * midgard_content_manager_load:
 * @self: #MidgardContentManager instance
 * @content: #GObject object which holds content data
 * @reference: #MidgardObjectReference instance
 * @model: (allow-none): #MidgardModel instance
 * @error: pointer to store returned error
 *
 * From storage, load object's content identified by @reference.
 *
 * Since: 10.05.8
 */ 
void
midgard_content_manager_load (MidgardContentManager *self, GObject *content, MidgardObjectReference *reference, MidgardModel *model, GError **error)
{
	return MIDGARD_CONTENT_MANAGER_GET_INTERFACE (self)->load (self, content, reference, model, error);
}

/**
 * midgard_content_manager_exists:
 * @self: #MidgardContentManager instance
 * @content: #GObject object which holds content data
 * @reference: #MidgardObjectReference instance
 * @model: (allow-none): #MidgardModel instance
 * @error: pointer to store returned error
 *
 * Check if given object's content identified by @reference exists in storage.
 *
 * Since: 10.05.8
 */ 
void
midgard_content_manager_exists (MidgardContentManager *self, GObject *content, MidgardObjectReference *reference, MidgardModel *model, GError **error)
{
	return MIDGARD_CONTENT_MANAGER_GET_INTERFACE (self)->exists (self, content, reference, model, error);
}

/**
 * midgard_content_manager_create:
 * @self: #MidgardContentManager instance
 * @content: #GObject object which holds content data
 * @reference: #MidgardObjectReference instance
 * @model: (allow-none): #MidgardModel instance
 * @error: pointer to store returned error
 *
 * Create object's content identified by @reference.
 *
 * Since: 10.05.8
 */ 
void
midgard_content_manager_create (MidgardContentManager *self, GObject *content, MidgardObjectReference *reference, MidgardModel *model, GError **error)
{
	return MIDGARD_CONTENT_MANAGER_GET_INTERFACE (self)->create (self, content, reference, model, error);
}

/**
 * midgard_content_manager_update:
 * @self: #MidgardContentManager instance
 * @content: #GObject object which holds content data
 * @reference: #MidgardObjectReference instance
 * @model: (allow-none): #MidgardModel instance
 * @error: pointer to store returned error
 *
 * Update object's content identified by @reference.
 *
 * Since: 10.05.8
 */ 
void
midgard_content_manager_update (MidgardContentManager *self, GObject *content, MidgardObjectReference *reference, MidgardModel *model, GError **error)
{
	return MIDGARD_CONTENT_MANAGER_GET_INTERFACE (self)->update (self, content, reference, model, error);
}

/**
 * midgard_content_manager_save:
 * @self: #MidgardContentManager instance
 * @content: #GObject object which holds content data
 * @reference: #MidgardObjectReference instance
 * @model: (allow-none): #MidgardModel instance
 * @error: pointer to store returned error
 *
 * Save object's content identified by @reference.
 *
 * Since: 10.05.8
 */ 
void
midgard_content_manager_save (MidgardContentManager *self, GObject *content, MidgardObjectReference *reference, MidgardModel *model, GError **error)
{
	return MIDGARD_CONTENT_MANAGER_GET_INTERFACE (self)->save (self, content, reference, model, error);
}

/**
 * midgard_content_manager_remove:
 * @self: #MidgardContentManager instance
 * @content: #GObject object which holds content data
 * @reference: #MidgardObjectReference instance
 * @model: (allow-none): #MidgardModel instance
 * @error: pointer to store returned error
 *
 * From storage, remove object's content identified by @reference.
 *
 * Since: 10.05.8
 */ 
void
midgard_content_manager_remove (MidgardContentManager *self, GObject *content, MidgardObjectReference *reference, MidgardModel *model, GError **error)
{
	return MIDGARD_CONTENT_MANAGER_GET_INTERFACE (self)->remove (self, content, reference, model, error);
}

/**
 * midgard_content_manager_purge:
 * @self: #MidgardContentManager instance
 * @content: #GObject object which holds content data
 * @reference: #MidgardObjectReference instance
 * @model: (allow-none): #MidgardModel instance
 * @error: pointer to store returned error
 *
 * From storage, purge object's content identified by @reference.
 *
 * Since: 10.05.8
 */ 
void
midgard_content_manager_purge (MidgardContentManager *self, GObject *content, MidgardObjectReference *reference, MidgardModel *model, GError **error)
{
	return MIDGARD_CONTENT_MANAGER_GET_INTERFACE (self)->purge (self, content, reference, model, error);
}

/* GOBJECT ROUTINES */

GType
midgard_content_manager_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
	  	static const GTypeInfo info = {
			sizeof (MidgardContentManagerIFace),
			NULL,   /* base_init */
			NULL,   /* base_finalize */
			NULL,   /* class_init */
			NULL,   /* class_finalize */
			NULL,   /* class_data */
			0,
			0,      /* n_preallocs */
			NULL    /* instance_init */
		};
		type = g_type_register_static (G_TYPE_INTERFACE, "MidgardContentManager", &info, 0);
		g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
    	}
    	return type;
}
