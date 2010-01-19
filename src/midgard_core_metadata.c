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

#define __SET_METADATA_PROPERTY_STRING(__property, __mstrval) { \
	g_assert(__mstrval != NULL); \
	if (__property != NULL) \
		g_free(__property); \
	__property = g_strdup(__mstrval); }

#define __SET_METADATA_PROPERTY_DATE_FROM_STRING(__property, __mstrval) { \
	g_assert(__mstrval != NULL); \
	GTimeVal timeval; \
	g_time_val_from_iso8601(__mstrval, &timeval); \
	g_date_set_time_val(__property, &timeval); }

#define __SET_METADATA_PROPERTY_DATE_FROM_GVALUE(__property, __value) { \
	g_assert(__value != NULL); \
	/* if (__property != NULL) \
		g_value_unset(__property); */ \
	GValue __mt = {0, }; \
	g_value_init(&__mt, MGD_TYPE_TIMESTAMP); \
	if (G_VALUE_TYPE(__value) == MGD_TYPE_TIMESTAMP) \
		g_value_copy(__value, &__mt); \
	else \
		g_value_transform(__value, &__mt); \
	__property = (MidgardTimestamp *) g_value_dup_boxed(&__mt); \
	g_value_unset(&__mt); }

/* setters */
void midgard_core_metadata_set_creator(MidgardMetadata *self, const gchar *creator)
{
	__SET_METADATA_PROPERTY_STRING(self->priv->creator, creator);
}

void midgard_core_metadata_set_created(MidgardMetadata *self, const GValue *value)
{
	__SET_METADATA_PROPERTY_DATE_FROM_GVALUE(self->priv->created, value);
}

void midgard_core_metadata_set_revisor(MidgardMetadata *self, const gchar *revisor)
{
	__SET_METADATA_PROPERTY_STRING(self->priv->revisor, revisor);
}

void midgard_core_metadata_set_revised(MidgardMetadata *self, const GValue *revised)
{
	__SET_METADATA_PROPERTY_DATE_FROM_GVALUE(self->priv->revised, revised);
}

void midgard_core_metadata_set_revision(MidgardMetadata *self, guint revision)
{
	self->priv->revision = revision;
}

void midgard_core_metadata_set_locker(MidgardMetadata *self, const gchar *locker)
{
	__SET_METADATA_PROPERTY_STRING(self->priv->locker, locker);
}

void midgard_core_metadata_set_locked(MidgardMetadata *self, const GValue *locked)
{
	__SET_METADATA_PROPERTY_DATE_FROM_GVALUE(self->priv->locked, locked);
}

void midgard_core_metadata_set_approver(MidgardMetadata *self, const gchar *approver)
{
	__SET_METADATA_PROPERTY_STRING(self->priv->approver, approver);
}

void midgard_core_metadata_set_approved(MidgardMetadata *self, const GValue *approved)
{
	__SET_METADATA_PROPERTY_DATE_FROM_GVALUE(self->priv->approved, approved);
}

void midgard_core_metadata_set_size(MidgardMetadata *self, guint size)
{
	self->priv->size = size;
}

void midgard_core_metadata_set_exported(MidgardMetadata *self, const GValue *exported)
{
	__SET_METADATA_PROPERTY_DATE_FROM_GVALUE(self->priv->exported, exported);
}

void midgard_core_metadata_set_imported(MidgardMetadata *self, const GValue *imported)
{
	__SET_METADATA_PROPERTY_DATE_FROM_GVALUE(self->priv->imported, imported);
}

void midgard_core_metadata_set_deleted(MidgardMetadata *self, gboolean deleted)
{
	self->priv->deleted = deleted;
}

void midgard_core_metadata_set_isapproved(MidgardMetadata *self, gboolean isapproved)
{
	self->priv->is_approved = isapproved;
}

void midgard_core_metadata_set_islocked(MidgardMetadata *self, gboolean islocked)
{
	self->priv->is_locked = islocked;
}

/* routines */
void midgard_core_metadata_increase_revision(MidgardMetadata *self)
{
	self->priv->revision++;
}

#define get_varchar_size(__prop) \
	if (__prop == NULL) \
		size++; \
	else \
		size += strlen((const gchar *)__prop) + 1;

guint midgard_core_metadata_get_size(MidgardMetadata *self)
{
	guint size = 0;

	/* CHAR */

	/* creator */
	get_varchar_size(self->priv->creator);

	/* revisor */
	get_varchar_size(self->priv->revisor);

	/* locker */
	get_varchar_size(self->priv->locker);
	
	/* authors */
	get_varchar_size(self->priv->authors);

	/* owner */
	get_varchar_size(self->priv->owner);

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
void __copy_date(const MidgardTimestamp *src, MidgardTimestamp **copy)
{
	GValue srcv = {0, };
	GValue dstv = {0, };

	g_value_init(&srcv, MGD_TYPE_TIMESTAMP);
	g_value_init(&dstv, MGD_TYPE_TIMESTAMP);

	g_value_set_boxed(&srcv, src);
	g_value_copy(&srcv, &dstv);

	*copy = (MidgardTimestamp *) g_value_dup_boxed(&dstv);

	g_value_unset(&srcv);
	g_value_unset(&dstv);

	return;
}

MidgardMetadata *midgard_core_metadata_copy(MidgardMetadata *src)
{
	g_assert(src != NULL);

	/* Initialize it that way so all callbacks are correctly set */
	MidgardMetadata *copy = midgard_metadata_new(src->priv->object);

	g_assert(copy != NULL);

	if (src->priv->creator != NULL) 
		midgard_core_metadata_set_creator(copy, src->priv->creator);

	if (src->priv->revisor != NULL)
		midgard_core_metadata_set_revisor(copy, src->priv->revisor);

	if (src->priv->locker != NULL)
		midgard_core_metadata_set_locker(copy, src->priv->locker);

	if (src->priv->approver != NULL)
		midgard_core_metadata_set_approver(copy, src->priv->approver);

	if (src->priv->authors != NULL) {
		g_free(copy->priv->authors);
		copy->priv->authors = g_strdup(src->priv->authors);
	}

	if (src->priv->owner != NULL ) {
		g_free(copy->priv->owner);
		copy->priv->owner = g_strdup(src->priv->owner);
	}

	__copy_date(src->priv->approved,       &copy->priv->approved);
	__copy_date(src->priv->created,        &copy->priv->created);
	__copy_date(src->priv->exported,       &copy->priv->exported);
	__copy_date(src->priv->imported,       &copy->priv->imported);
	__copy_date(src->priv->locked,         &copy->priv->locked);
	__copy_date(src->priv->published,      &copy->priv->published);
	__copy_date(src->priv->revised,        &copy->priv->revised);
	__copy_date(src->priv->schedule_start, &copy->priv->schedule_start);
	__copy_date(src->priv->schedule_end,   &copy->priv->schedule_end);

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

	return copy;
}
