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

#ifndef MIDGARD_CONTENT_MANAGER_H
#define MIDGARD_CONTENT_MANAGER_H

#include <glib-object.h>
#include "midgard_model.h"
#include "midgard_object_reference.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_CONTENT_MANAGER (midgard_content_manager_get_type()) 
#define MIDGARD_CONTENT_MANAGER(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_CONTENT_MANAGER, MidgardContentManager))
#define MIDGARD_IS_CONTENT_MANAGER(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_CONTENT_MANAGER))
#define MIDGARD_CONTENT_MANAGER_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), MIDGARD_TYPE_CONTENT_MANAGER, MidgardContentManagerIFace))

typedef struct _MidgardContentManager MidgardContentManager;
typedef struct _MidgardContentManagerIFace MidgardContentManagerIFace;

struct _MidgardContentManagerIFace {
	GTypeInterface parent;

	GObject*	(*get_by_id)	(MidgardContentManager *self, MidgardObjectReference *reference, MidgardModel *model, GError **error);
	void		(*load)		(MidgardContentManager *self, GObject *object, MidgardObjectReference *reference, MidgardModel *model, GError **error);
	void		(*exists)		(MidgardContentManager *self, GObject *object, MidgardObjectReference *reference, MidgardModel *model, GError **error);
	void		(*create)		(MidgardContentManager *self, GObject *object, MidgardObjectReference *reference, MidgardModel *model, GError **error);
	void		(*update)		(MidgardContentManager *self, GObject *object, MidgardObjectReference *reference, MidgardModel *model, GError **error);
	void		(*save)			(MidgardContentManager *self, GObject *object, MidgardObjectReference *reference, MidgardModel *model, GError **error);
	void		(*remove)		(MidgardContentManager *self, GObject *object, MidgardObjectReference *reference, MidgardModel *model, GError **error);
	void		(*purge)		(MidgardContentManager *self, GObject *object, MidgardObjectReference *reference, MidgardModel *model, GError **error);

};

GType 			midgard_content_manager_get_type		(void);
GObject*		midgard_content_manager_get_by_id	(MidgardContentManager *self, MidgardObjectReference *reference, MidgardModel *model, GError **error);
void			midgard_content_manager_load		(MidgardContentManager *self, GObject *content, MidgardObjectReference *reference, MidgardModel *model, GError **error);
void			midgard_content_manager_exists		(MidgardContentManager *self, GObject *content, MidgardObjectReference *reference, MidgardModel *model, GError **error);
void			midgard_content_manager_create		(MidgardContentManager *self, GObject *content, MidgardObjectReference *reference, MidgardModel *model, GError **error);
void			midgard_content_manager_update		(MidgardContentManager *self, GObject *content, MidgardObjectReference *reference, MidgardModel *model, GError **error);
void			midgard_content_manager_save		(MidgardContentManager *self, GObject *content, MidgardObjectReference *reference, MidgardModel *model, GError **error);
void			midgard_content_manager_remove		(MidgardContentManager *self, GObject *content, MidgardObjectReference *reference, MidgardModel *model, GError **error);
void			midgard_content_manager_purge		(MidgardContentManager *self, GObject *content, MidgardObjectReference *reference, MidgardModel *model, GError **error);

G_END_DECLS

#endif /* MIDGARD_CONTENT_MANAGER_H */
