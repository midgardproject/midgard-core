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
 *   */

#include "midgard_core_metadata.h"
#include "midgard_core_query.h"

#define __SET_METADATA_PROPERTY_STRING(__property, __mstrval) { \
	if (!__mstrval) \
		return; \
	g_free (__property); \
	if (G_VALUE_TYPE (__mstrval) == GDA_TYPE_NULL) { \
		__property = g_strdup (""); \
	/* FIXME, buils select with explicitly defined Gda parameters */ \
	} else if (!G_VALUE_HOLDS_STRING (__mstrval)) { \
		GValue sval = {0, }; \
		g_value_init (&sval, G_TYPE_STRING); \
       		g_value_transform (__mstrval, &sval); \
		__property = g_value_dup_string (&sval); \
		g_value_unset (&sval); \
	} else { \
		__property = g_value_dup_string (__mstrval); \
	} \
	if (!__property) __property = g_strdup (""); \
}

#define __SET_METADATA_PROPERTY_DATE_FROM_STRING(__property, __mstrval) { \
	g_assert (__mstrval != NULL); \
	GTimeVal timeval; \
	g_time_val_from_iso8601(__mstrval, &timeval); \
	g_date_set_time_val(__property, &timeval); }

#define __SET_METADATA_PROPERTY_DATE_FROM_GVALUE(__property, __value) { \
	g_assert (__value != NULL); \
	/* if (__property != NULL) \
		g_value_unset (__property); */ \
	GValue __mt = {0, }; \
	g_value_init (&__mt, MGD_TYPE_TIMESTAMP); \
	if (G_VALUE_TYPE(__value) == MGD_TYPE_TIMESTAMP) \
		g_value_copy(__value, &__mt); \
	else \
		g_value_transform(__value, &__mt); \
	g_free (__property); \
	__property = (MidgardTimestamp *) g_value_dup_boxed (&__mt); \
	g_value_unset (&__mt); }

/* setters */
void 
midgard_core_metadata_set_creator (MidgardMetadata *self, const GValue *creator)
{
	__SET_METADATA_PROPERTY_STRING (self->priv->creator, creator);
}

void 
midgard_core_metadata_set_created (MidgardMetadata *self, const GValue *created)
{
	__SET_METADATA_PROPERTY_DATE_FROM_GVALUE (self->priv->created, created);
}

void 
midgard_core_metadata_set_revisor (MidgardMetadata *self, const GValue *revisor)
{
	__SET_METADATA_PROPERTY_STRING (self->priv->revisor, revisor);
}

void 
midgard_core_metadata_set_revised (MidgardMetadata *self, const GValue *revised)
{
	__SET_METADATA_PROPERTY_DATE_FROM_GVALUE (self->priv->revised, revised);
}

void 
midgard_core_metadata_set_revision (MidgardMetadata *self, const GValue *revision)
{
	if (G_VALUE_HOLDS_UINT (revision))
		self->priv->revision = g_value_get_uint (revision);

	if (G_VALUE_HOLDS_INT (revision))
		self->priv->revision = g_value_get_int (revision);

}

void 
midgard_core_metadata_set_locker (MidgardMetadata *self, const GValue *locker)
{
	__SET_METADATA_PROPERTY_STRING (self->priv->locker, locker);
}

void 
midgard_core_metadata_set_locked (MidgardMetadata *self, const GValue *locked)
{
	__SET_METADATA_PROPERTY_DATE_FROM_GVALUE (self->priv->locked, locked);
}

void 
midgard_core_metadata_set_approver (MidgardMetadata *self, const GValue *approver)
{
	__SET_METADATA_PROPERTY_STRING (self->priv->approver, approver);
}

void 
midgard_core_metadata_set_approved (MidgardMetadata *self, const GValue *approved)
{
	__SET_METADATA_PROPERTY_DATE_FROM_GVALUE (self->priv->approved, approved);
}

void 
midgard_core_metadata_set_authors (MidgardMetadata *self, const GValue *authors)
{
	__SET_METADATA_PROPERTY_STRING (self->priv->authors, authors);
}

void 
midgard_core_metadata_set_owner (MidgardMetadata *self, const GValue *owner)
{
	__SET_METADATA_PROPERTY_STRING (self->priv->owner, owner);
}

void 
midgard_core_metadata_set_size (MidgardMetadata *self, const GValue *size)
{
	if (G_VALUE_HOLDS_UINT (size))
		self->priv->size = g_value_get_uint (size);
	
	if (G_VALUE_HOLDS_INT (size) && g_value_get_int (size) > 0)
		self->priv->size = g_value_get_int (size);
}

void 
midgard_core_metadata_set_exported (MidgardMetadata *self, const GValue *exported)
{
	__SET_METADATA_PROPERTY_DATE_FROM_GVALUE (self->priv->exported, exported);
}

void 
midgard_core_metadata_set_imported (MidgardMetadata *self, const GValue *imported)
{
	__SET_METADATA_PROPERTY_DATE_FROM_GVALUE(self->priv->imported, imported);
}

void 
midgard_core_metadata_set_deleted (MidgardMetadata *self, const GValue *deleted)
{
	MIDGARD_GET_BOOLEAN_FROM_VALUE (self->priv->deleted, deleted);
}

void 
midgard_core_metadata_set_isapproved (MidgardMetadata *self, const GValue *isapproved)
{
	MIDGARD_GET_BOOLEAN_FROM_VALUE (self->priv->is_approved, isapproved);
}

void 
midgard_core_metadata_set_islocked (MidgardMetadata *self, const GValue *islocked)
{
	MIDGARD_GET_BOOLEAN_FROM_VALUE (self->priv->is_locked, islocked);
}

void 
midgard_core_metadata_set_schedule_start (MidgardMetadata *self, const GValue *schedule)
{
	__SET_METADATA_PROPERTY_DATE_FROM_GVALUE (self->priv->schedule_start, schedule);
}

void 
midgard_core_metadata_set_schedule_end (MidgardMetadata *self, const GValue *schedule)
{
	__SET_METADATA_PROPERTY_DATE_FROM_GVALUE (self->priv->schedule_end, schedule);
}

void 
midgard_core_metadata_set_published (MidgardMetadata *self, const GValue *published)
{
	__SET_METADATA_PROPERTY_DATE_FROM_GVALUE (self->priv->published, published);
}

void 
midgard_core_metadata_set_score (MidgardMetadata *self, const GValue *score)
{
	if (G_VALUE_HOLDS_INT (score))
		self->priv->score = g_value_get_int (score);

	if (G_VALUE_HOLDS_UINT (score))
		self->priv->score = g_value_get_uint (score);
}

void 
midgard_core_metadata_set_hidden (MidgardMetadata *self, const GValue *hidden)
{
	MIDGARD_GET_BOOLEAN_FROM_VALUE (self->priv->hidden, hidden);
}

void 
midgard_core_metadata_set_navnoentry (MidgardMetadata *self, const GValue *nav)
{
	MIDGARD_GET_BOOLEAN_FROM_VALUE (self->priv->nav_noentry, nav);
}

/* routines */
void 
midgard_core_metadata_increase_revision (MidgardMetadata *self)
{
	self->priv->revision++;
}

#define get_varchar_size(__prop) \
	if (__prop == NULL) \
		size++; \
	else \
		size += strlen((const gchar *)__prop) + 1;

guint 
midgard_core_metadata_get_size (MidgardMetadata *self)
{
	guint size = 0;

	/* CHAR */

	/* creator */
	get_varchar_size (self->priv->creator);

	/* revisor */
	get_varchar_size (self->priv->revisor);

	/* locker */
	get_varchar_size (self->priv->locker);
	
	/* authors */
	get_varchar_size (self->priv->authors);

	/* owner */
	get_varchar_size (self->priv->owner);

	/* DATETIME */
	/* datetime needs 8 bytes (MySQL at least).
	 * I do not use strlen here to avoid GValue conversion */
	/* created, revised, locked, approved, published, schedulestart,
	 * scheduleend, locked, approved, exported, imported */
	size += (11 * 8);

	/* BOOLEAN */
	/* hidden, nav-noentry , revision, is_locked, is_approved, deleted */
	size += (6 * 4);

	/* INT */
	/* size, score */
	size += (2 * 4);

	return size;
}

/* This is temporary routine. Ragnaroek compatible. 
 * If it's needed for longer period, core's timestamp API should be provided */
void __copy_date (const MidgardTimestamp *src, MidgardTimestamp **copy)
{
	GValue srcv = {0, };
	GValue dstv = {0, };

	g_value_init (&srcv, MGD_TYPE_TIMESTAMP);
	g_value_init (&dstv, MGD_TYPE_TIMESTAMP);

	g_value_set_boxed (&srcv, src);
	g_value_copy(&srcv, &dstv);

	*copy = (MidgardTimestamp *) g_value_dup_boxed (&dstv);

	g_value_unset (&srcv);
	g_value_unset (&dstv);

	return;
}

MidgardMetadata *
midgard_core_metadata_copy(MidgardMetadata *src)
{
	g_assert (src != NULL);

	/* Initialize it that way so all callbacks are correctly set */
	MidgardMetadata *copy = midgard_metadata_new (src->priv->object);
	g_assert (copy != NULL);

	midgard_core_metadata_copy_properties (src, copy);
	return copy;
}

void 
midgard_core_metadata_copy_properties (MidgardMetadata *src, MidgardMetadata *copy)
{
	g_return_if_fail (src != NULL);
	g_return_if_fail (copy != NULL);

	GValue sval = {0, };
	
	if (src->priv->creator != NULL) {
		g_value_init (&sval, G_TYPE_STRING);
		g_value_set_string (&sval, src->priv->creator);
		midgard_core_metadata_set_creator (copy, &sval);
		g_value_unset (&sval);
	}

	if (src->priv->revisor != NULL) {
		g_value_init (&sval, G_TYPE_STRING);
		g_value_set_string (&sval, src->priv->revisor);
		midgard_core_metadata_set_revisor (copy, &sval);
		g_value_unset (&sval);
	}

	if (src->priv->locker != NULL) {
		g_value_init (&sval, G_TYPE_STRING);
		g_value_set_string (&sval, src->priv->locker);
		midgard_core_metadata_set_creator (copy, &sval);
		g_value_unset (&sval);
	}

	if (src->priv->approver != NULL) {
		g_value_init (&sval, G_TYPE_STRING);
		g_value_set_string (&sval, src->priv->approver);
		midgard_core_metadata_set_approver (copy, &sval);
		g_value_unset (&sval);
	}

	if (src->priv->authors != NULL) {
		g_free (copy->priv->authors);
		copy->priv->authors = g_strdup (src->priv->authors);
	}

	if (src->priv->owner != NULL ) {
		g_free (copy->priv->owner);
		copy->priv->owner = g_strdup (src->priv->owner);
	}

	__copy_date (src->priv->approved,       &copy->priv->approved);
	__copy_date (src->priv->created,        &copy->priv->created);
	__copy_date (src->priv->exported,       &copy->priv->exported);
	__copy_date (src->priv->imported,       &copy->priv->imported);
	__copy_date (src->priv->locked,         &copy->priv->locked);
	__copy_date (src->priv->published,      &copy->priv->published);
	__copy_date (src->priv->revised,        &copy->priv->revised);
	__copy_date (src->priv->schedule_start, &copy->priv->schedule_start);
	__copy_date (src->priv->schedule_end,   &copy->priv->schedule_end);

	copy->priv->approve_is_set = src->priv->approve_is_set;
	copy->priv->deleted        = src->priv->deleted;
	copy->priv->hidden         = src->priv->hidden;
	copy->priv->is_approved    = src->priv->is_approved;
	copy->priv->is_locked      = src->priv->is_locked;
	copy->priv->lock_is_set    = src->priv->lock_is_set;
	copy->priv->nav_noentry    = src->priv->nav_noentry;
	copy->priv->revision       = src->priv->revision;
	copy->priv->score          = src->priv->score;
	copy->priv->size           = src->priv->size;

	return;
}

static GObjectClass *__parent_class = NULL;

gboolean        
midgard_core_metadata_dbpriv_set_property (MidgardDBObject *self, const gchar *name, GValue *value)
{
	if (!__parent_class)
		__parent_class = g_type_class_peek_parent (G_OBJECT_GET_CLASS (self));

       	gchar *datamodel_property = g_strconcat ("metadata_", name, NULL);
	MidgardDBObject *dbobject = MIDGARD_DBOBJECT (MIDGARD_METADATA (self)->priv->object);
	gboolean rv = MIDGARD_DBOBJECT_CLASS (__parent_class)->dbpriv->set_property (dbobject, datamodel_property, value);
	g_free (datamodel_property);

	return rv;
}
