/* 
 * Copyright (C) 2008 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_test.h"

void __metadata_datetime_empty(const gchar *datetime)
{
	/* TODO */
	/* We need a way to determine if datetime is empty.
	 * Current way is not efficient as it depends only on MySQL provider and fails with PostgreSQL for example */
}

void midgard_test_metadata_check_new(MidgardObject *object)
{
	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS (object);
	if (!midgard_object_class_has_metadata (klass))
		return;

	MidgardMetadata *metadata;
	g_object_get(object, "metadata", &metadata, NULL);

	GValue tval = {0, };

	/*created */
	gchar *metadata_created = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "created", &tval);
	metadata_created = midgard_timestamp_get_string_from_value(&tval);
	g_assert_cmpstr(metadata_created, ==, MIDGARD_DEFAULT_DATETIME); 
	g_free(metadata_created);
	g_value_unset(&tval);
	
	/* creator */
	gchar *metadata_creator = NULL;
	g_object_get(metadata, "creator", &metadata_creator, NULL);
	g_assert_cmpstr(metadata_creator, ==, NULL);
	g_free(metadata_creator);
	
	/* revisor */
	gchar *metadata_revisor = NULL;
	g_object_get(metadata, "revisor", &metadata_revisor, NULL);
	g_assert_cmpstr(metadata_revisor, ==, NULL);
	g_free(metadata_revisor);
	
	/* revision */
	guint metadata_revision = 0;
	g_object_get(metadata, "revision", &metadata_revision, NULL);
	g_assert_cmpuint(metadata_revision, <, 2);	

	/* revised */
	gchar *metadata_revised = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "revised", &tval);
	metadata_revised = midgard_timestamp_get_string_from_value(&tval);
	g_assert_cmpstr(metadata_revised, ==, MIDGARD_DEFAULT_DATETIME);
	g_value_unset(&tval);
	g_free(metadata_revised);
	
	/* locker */
	gchar *metadata_locker = NULL;
	g_object_get(metadata, "locker", &metadata_locker, NULL);
	g_assert_cmpstr(metadata_locker, ==, NULL);
	g_free(metadata_locker);

	/* locked */
	gchar *metadata_locked = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "locked", &tval);
	metadata_locked = midgard_timestamp_get_string_from_value(&tval);
	g_assert_cmpstr(metadata_locked, ==, MIDGARD_DEFAULT_DATETIME);
	g_value_unset(&tval);
	g_free(metadata_locked);
	
	/* approver */
	gchar *metadata_approver = NULL;
	g_object_get(metadata, "approver", &metadata_approver, NULL);
	g_assert_cmpstr(metadata_approver, ==, NULL);
	g_free(metadata_approver);

	/* approved */
	gchar *metadata_approved = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "approved", &tval);
	metadata_approved = midgard_timestamp_get_string_from_value(&tval);
	g_assert_cmpstr(metadata_approved, ==, MIDGARD_DEFAULT_DATETIME);
	g_value_unset(&tval);
	g_free(metadata_approved);

	/* authors */
	gchar *metadata_authors = NULL;
	g_object_get(metadata, "authors", &metadata_authors, NULL);
	g_assert_cmpstr(metadata_authors, ==, NULL);
	g_free(metadata_authors);

	/* owner */
	gchar *metadata_owner = NULL;
	g_object_get(metadata, "owner", &metadata_owner, NULL);
	g_assert_cmpstr(metadata_owner, ==, NULL);
	g_free(metadata_owner);

	/* schedulestart */
	gchar *metadata_schedulestart = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "schedulestart", &tval);
	metadata_schedulestart = midgard_timestamp_get_string_from_value(&tval);
	g_assert_cmpstr(metadata_schedulestart, ==, MIDGARD_DEFAULT_DATETIME);
	g_value_unset(&tval);
	g_free(metadata_schedulestart);

	/* scheduleend */
	gchar *metadata_scheduleend = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "scheduleend", &tval);
	metadata_scheduleend = midgard_timestamp_get_string_from_value(&tval);
	g_assert_cmpstr(metadata_scheduleend, ==, MIDGARD_DEFAULT_DATETIME);
	g_value_unset(&tval);
	g_free(metadata_scheduleend);

	/* hidden */
	gboolean metadata_hidden = FALSE;
	g_object_get(metadata, "hidden", &metadata_hidden, NULL);
	g_assert(metadata_hidden == FALSE);

	/* navnoentry */
	gboolean metadata_navnoentry = FALSE;
	g_object_get(metadata, "navnoentry", &metadata_navnoentry, NULL);
	g_assert(metadata_navnoentry == FALSE);

	/* size */
	guint metadata_size = 0;
	g_object_get(metadata, "size", &metadata_size, NULL);
	g_assert_cmpuint(metadata_size, ==, 0);

	/* published */
	gchar *metadata_published = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "published", &tval);
	metadata_published = midgard_timestamp_get_string_from_value(&tval);
	g_assert_cmpstr(metadata_published, ==, MIDGARD_DEFAULT_DATETIME);
	g_value_unset(&tval);
	g_free(metadata_published);

	/* score */
	gint metadata_score = 0;
	g_object_get(metadata, "score", &metadata_score, NULL);
	g_assert_cmpint(metadata_score, ==, 0);

	/* imported */
	gchar *metadata_imported = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "imported", &tval);
	metadata_imported = midgard_timestamp_get_string_from_value(&tval);
	g_assert_cmpstr(metadata_imported, ==, MIDGARD_DEFAULT_DATETIME);
	g_value_unset(&tval);
	g_free(metadata_imported);

	/* exported */
	gchar *metadata_exported = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "exported", &tval);
	metadata_exported = midgard_timestamp_get_string_from_value(&tval);
	g_assert_cmpstr(metadata_exported, ==, MIDGARD_DEFAULT_DATETIME);
	g_value_unset(&tval);
	g_free(metadata_exported);

	/* deleted */
	gboolean metadata_deleted = FALSE;
	g_object_get(metadata, "deleted", &metadata_deleted, NULL);
	g_assert(metadata_deleted == FALSE);

	/* islocked */
	gboolean metadata_islocked = FALSE;
	g_object_get(metadata, "islocked", &metadata_islocked, NULL);
	g_assert(metadata_islocked == FALSE);

	/* isapproved */
	gboolean metadata_isapproved = FALSE;
	g_object_get(metadata, "isapproved", &metadata_isapproved, NULL);
	g_assert(metadata_isapproved == FALSE);

	return;
}

void midgard_test_metadata_check_create(MidgardObject *object)
{
	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS (object);
	if (!midgard_object_class_has_metadata (klass))
		return;
	
	MidgardMetadata *metadata;
	g_object_get(object, "metadata", &metadata, NULL);	

	GValue tval = {0, };

	/*created */
	gchar *metadata_created = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "created", &tval);
	metadata_created = midgard_timestamp_get_string_from_value(&tval);
	g_assert_cmpstr(metadata_created, !=, NULL);
	g_assert_cmpstr(metadata_created, !=, MIDGARD_DEFAULT_DATETIME);
	g_value_unset(&tval);
	g_free(metadata_created);
	
	/* creator */
	gchar *metadata_creator = NULL;
	g_object_get(metadata, "creator", &metadata_creator, NULL);
	g_assert_cmpstr(metadata_creator, !=, NULL);
	g_free(metadata_creator);
	
	/* revisor */
	gchar *metadata_revisor = NULL;
	g_object_get(metadata, "revisor", &metadata_revisor, NULL);
	g_assert_cmpstr(metadata_revisor, !=, NULL);
	g_free(metadata_revisor);

	/* revised */
	gchar *metadata_revised = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "revised", &tval);
	metadata_revised = midgard_timestamp_get_string_from_value(&tval);
	g_assert_cmpstr(metadata_revised, !=, NULL);
	g_value_unset(&tval);
	g_free(metadata_revised);

	/* revision */
	guint metadata_revision = 0;
	g_object_get(metadata, "revision", &metadata_revision, NULL);
	g_assert_cmpuint(metadata_revision, <, 3);

	/* locker */
	gchar *metadata_locker = NULL;
	g_object_get(metadata, "locker", &metadata_locker, NULL);
	g_assert_cmpstr(metadata_locker, ==, NULL);
	g_free(metadata_locker);

	/* locked */
	gchar *metadata_locked = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "locked", &tval);
	metadata_locked = midgard_timestamp_get_string_from_value(&tval);
	__metadata_datetime_empty(metadata_locked);
	g_value_unset(&tval);
	g_free(metadata_locked);	

	/* approver */
	gchar *metadata_approver = NULL;
	g_object_get(metadata, "approver", &metadata_approver, NULL);
	g_assert_cmpstr(metadata_approver, ==, NULL);
	g_free(metadata_approver);

	/* approved */
	gchar *metadata_approved = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "approved", &tval);
	metadata_approved = midgard_timestamp_get_string_from_value(&tval);
	__metadata_datetime_empty(metadata_approved);
	g_value_unset(&tval);
	g_free(metadata_approved);	

	/* owner NA */
	/* schedulestart NA */
	/* scheduleend NA */
	/* hidden NA */
	/* navnoentry NA */

	/* size */
	guint metadata_size = 0;
	g_object_get(metadata, "size", &metadata_size, NULL);
	g_assert_cmpuint(metadata_size, >, 0);

	/* published NA */
	/* score NA */
	
	/* imported */
	gchar *metadata_imported = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "imported", &tval);
	metadata_imported = midgard_timestamp_get_string_from_value(&tval);
	__metadata_datetime_empty(metadata_imported);
	g_value_unset(&tval);
	g_free(metadata_imported);	

	/* exported */
	gchar *metadata_exported = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "exported", &tval);
	metadata_exported = midgard_timestamp_get_string_from_value(&tval);
	__metadata_datetime_empty(metadata_exported);
	g_value_unset(&tval);
	g_free(metadata_exported);	

	/* deleted */
	gboolean metadata_deleted = FALSE;
	g_object_get(metadata, "deleted", &metadata_deleted, NULL);
	g_assert(metadata_deleted == FALSE);

	/* islocked */
	gboolean metadata_islocked = FALSE;
	g_object_get(metadata, "islocked", &metadata_islocked, NULL);
	g_assert(metadata_islocked == FALSE);

	/* isapproved NA */
	gboolean metadata_isapproved = FALSE;
	g_object_get(metadata, "isapproved", &metadata_isapproved, NULL);
	g_assert(metadata_isapproved == FALSE);

	return;
}

void midgard_test_metadata_check_created(MidgardObject *object)
{
	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS (object);
	if (!midgard_object_class_has_metadata (klass))
		return;
	
	MidgardMetadata *metadata;
	g_object_get(object, "metadata", &metadata, NULL);	

	GValue tval = {0, };

	/*created */
	gchar *metadata_created = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "created", &tval);
	metadata_created = midgard_timestamp_get_string_from_value(&tval);
	g_assert_cmpstr(metadata_created, !=, NULL);
	g_assert_cmpstr(metadata_created, !=, MIDGARD_DEFAULT_DATETIME);
	g_value_unset(&tval);
	g_free(metadata_created);
	
	/* creator */
	gchar *metadata_creator = NULL;
	g_object_get(metadata, "creator", &metadata_creator, NULL);
	g_assert_cmpstr(metadata_creator, !=, NULL);
	g_free(metadata_creator);
	
	/* revisor */
	gchar *metadata_revisor = NULL;
	g_object_get(metadata, "revisor", &metadata_revisor, NULL);
	g_assert_cmpstr(metadata_revisor, !=, NULL);
	g_free(metadata_revisor);

	/* revised */
	gchar *metadata_revised = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "revised", &tval);
	metadata_revised = midgard_timestamp_get_string_from_value(&tval);
	g_assert_cmpstr(metadata_revised, !=, NULL);
	g_value_unset(&tval);
	g_free(metadata_revised);

	/* revision */
	guint metadata_revision = 0;
	g_object_get(metadata, "revision", &metadata_revision, NULL);
	g_assert_cmpuint(metadata_revision, <, 3);

	/* locker */
	gchar *metadata_locker = NULL;
	g_object_get(metadata, "locker", &metadata_locker, NULL);
	g_assert_cmpstr(metadata_locker, ==, "");
	g_free(metadata_locker);

	/* locked */
	gchar *metadata_locked = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "locked", &tval);
	metadata_locked = midgard_timestamp_get_string_from_value(&tval);
	__metadata_datetime_empty(metadata_locked);
	g_value_unset(&tval);
	g_free(metadata_locked);	

	/* approver */
	gchar *metadata_approver = NULL;
	g_object_get(metadata, "approver", &metadata_approver, NULL);
	g_assert_cmpstr(metadata_approver, ==, "");
	g_free(metadata_approver);

	/* approved */
	gchar *metadata_approved = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "approved", &tval);
	metadata_approved = midgard_timestamp_get_string_from_value(&tval);
	__metadata_datetime_empty(metadata_approved);
	g_value_unset(&tval);
	g_free(metadata_approved);	

	/* owner NA */
	/* schedulestart NA */
	/* scheduleend NA */
	/* hidden NA */
	/* navnoentry NA */

	/* size */
	guint metadata_size = 0;
	g_object_get(metadata, "size", &metadata_size, NULL);
	g_assert_cmpuint(metadata_size, >, 0);

	/* published NA */
	/* score NA */
	
	/* imported */
	gchar *metadata_imported = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "imported", &tval);
	metadata_imported = midgard_timestamp_get_string_from_value(&tval);
	__metadata_datetime_empty(metadata_imported);
	g_value_unset(&tval);
	g_free(metadata_imported);	

	/* exported */
	gchar *metadata_exported = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "exported", &tval);
	metadata_exported = midgard_timestamp_get_string_from_value(&tval);
	__metadata_datetime_empty(metadata_exported);
	g_value_unset(&tval);
	g_free(metadata_exported);	

	/* deleted */
	gboolean metadata_deleted = FALSE;
	g_object_get(metadata, "deleted", &metadata_deleted, NULL);
	g_assert(metadata_deleted == FALSE);

	/* islocked */
	gboolean metadata_islocked = FALSE;
	g_object_get(metadata, "islocked", &metadata_islocked, NULL);
	g_assert(metadata_islocked == FALSE);

	/* isapproved NA */
	gboolean metadata_isapproved = FALSE;
	g_object_get(metadata, "isapproved", &metadata_isapproved, NULL);
	g_assert(metadata_isapproved == FALSE);

	return;
}

void midgard_test_metadata_check_update(MidgardObject *object)
{
	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS (object);
	if (!midgard_object_class_has_metadata (klass))
		return;
	
	MidgardMetadata *metadata;
	g_object_get(object, "metadata", &metadata, NULL);	

	GValue tval = {0, };

	/* created */
	gchar *metadata_created = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "created", &tval);
	metadata_created = midgard_timestamp_get_string_from_value(&tval);
	g_assert_cmpstr(metadata_created, !=, NULL);
	g_value_unset(&tval);
	g_free(metadata_created);
	
	/* creator */
	gchar *metadata_creator = NULL;
	g_object_get(metadata, "creator", &metadata_creator, NULL);
	g_assert_cmpstr(metadata_creator, !=, NULL);
	g_free(metadata_creator);
	
	/* revisor */
	gchar *metadata_revisor = NULL;
	g_object_get(metadata, "revisor", &metadata_revisor, NULL);
	g_assert_cmpstr(metadata_revisor, !=, NULL);
	g_free(metadata_revisor);

	/* revised */
	gchar *metadata_revised = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "revised", &tval);
	metadata_revised = midgard_timestamp_get_string_from_value(&tval);
	g_assert_cmpstr(metadata_revised, !=, NULL);
	g_value_unset(&tval);
	g_free(metadata_revised);
	
	/* revision */
	guint metadata_revision = 0;
	g_object_get(metadata, "revision", &metadata_revision, NULL);
	g_assert_cmpuint(metadata_revision, <, 4); /* FIXME, move it standalone tests with revision argument to check */

	/* locker */
	gchar *metadata_locker = NULL;
	g_object_get(metadata, "locker", &metadata_locker, NULL);
	g_assert_cmpstr(metadata_locker, ==, NULL);
	g_free(metadata_locker);

	/* locked */
	gchar *metadata_locked = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "locked", &tval);
	__metadata_datetime_empty(metadata_locked);
	g_value_unset(&tval);
	g_free(metadata_locked);	

	/* approver */
	gchar *metadata_approver = NULL;
	g_object_get(metadata, "approver", &metadata_approver, NULL);
	g_assert_cmpstr(metadata_approver, ==, NULL);
	g_free(metadata_approver);

	/* approved */
	gchar *metadata_approved = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "approved", &tval);
	__metadata_datetime_empty(metadata_approved);
	g_value_unset(&tval);
	g_free(metadata_approved);	

	/* owner NA */
	/* schedulestart NA */
	/* scheduleend NA */
	/* hidden NA */
	/* navnoentry NA */

	/* size */
	guint metadata_size = 0;
	g_object_get(metadata, "size", &metadata_size, NULL);
	g_assert_cmpuint(metadata_size, >, 0);

	/* published NA */
	/* score NA */
	
	/* imported */
	gchar *metadata_imported = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "imported", &tval);
	__metadata_datetime_empty(metadata_imported);
	g_value_unset(&tval);
	g_free(metadata_imported);	

	/* exported */
	gchar *metadata_exported = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "exported", &tval);
	__metadata_datetime_empty(metadata_exported);
	g_value_unset(&tval);
	g_free(metadata_exported);	

	/* deleted */
	gboolean metadata_deleted = FALSE;
	g_object_get(metadata, "deleted", &metadata_deleted, NULL);
	g_assert(metadata_deleted == FALSE);

	/* islocked */
	gboolean metadata_islocked = FALSE;
	g_object_get(metadata, "islocked", &metadata_islocked, NULL);
	g_assert(metadata_islocked == FALSE);

	/* isapproved NA */
	gboolean metadata_isapproved = FALSE;
	g_object_get(metadata, "isapproved", &metadata_isapproved, NULL);
	g_assert(metadata_isapproved == FALSE);

	return;
}

void midgard_test_metadata_check_updated(MidgardObject *object)
{
	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS (object);
	if (!midgard_object_class_has_metadata (klass))
		return;
	
	MidgardMetadata *metadata;
	g_object_get(object, "metadata", &metadata, NULL);	

	GValue tval = {0, };

	/* created */
	gchar *metadata_created = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "created", &tval);
	metadata_created = midgard_timestamp_get_string_from_value(&tval);
	g_assert_cmpstr(metadata_created, !=, NULL);
	g_value_unset(&tval);
	g_free(metadata_created);
	
	/* creator */
	gchar *metadata_creator = NULL;
	g_object_get(metadata, "creator", &metadata_creator, NULL);
	g_assert_cmpstr(metadata_creator, !=, NULL);
	g_free(metadata_creator);
	
	/* revisor */
	gchar *metadata_revisor = NULL;
	g_object_get(metadata, "revisor", &metadata_revisor, NULL);
	g_assert_cmpstr(metadata_revisor, !=, NULL);
	g_free(metadata_revisor);

	/* revised */
	gchar *metadata_revised = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "revised", &tval);
	metadata_revised = midgard_timestamp_get_string_from_value(&tval);
	g_assert_cmpstr(metadata_revised, !=, NULL);
	g_value_unset(&tval);
	g_free(metadata_revised);
	
	/* revision */
	guint metadata_revision = 0;
	g_object_get(metadata, "revision", &metadata_revision, NULL);
	g_assert_cmpuint(metadata_revision, <, 4); /* FIXME, move it standalone tests with revision argument to check */

	/* locker */
	gchar *metadata_locker = NULL;
	g_object_get(metadata, "locker", &metadata_locker, NULL);
	g_assert_cmpstr(metadata_locker, ==, "");
	g_free(metadata_locker);

	/* locked */
	gchar *metadata_locked = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "locked", &tval);
	__metadata_datetime_empty(metadata_locked);
	g_value_unset(&tval);
	g_free(metadata_locked);	

	/* approver */
	gchar *metadata_approver = NULL;
	g_object_get(metadata, "approver", &metadata_approver, NULL);
	g_assert_cmpstr(metadata_approver, ==, "");
	g_free(metadata_approver);

	/* approved */
	gchar *metadata_approved = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "approved", &tval);
	__metadata_datetime_empty(metadata_approved);
	g_value_unset(&tval);
	g_free(metadata_approved);	

	/* owner NA */
	/* schedulestart NA */
	/* scheduleend NA */
	/* hidden NA */
	/* navnoentry NA */

	/* size */
	guint metadata_size = 0;
	g_object_get(metadata, "size", &metadata_size, NULL);
	g_assert_cmpuint(metadata_size, >, 0);

	/* published NA */
	/* score NA */
	
	/* imported */
	gchar *metadata_imported = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "imported", &tval);
	__metadata_datetime_empty(metadata_imported);
	g_value_unset(&tval);
	g_free(metadata_imported);	

	/* exported */
	gchar *metadata_exported = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "exported", &tval);
	__metadata_datetime_empty(metadata_exported);
	g_value_unset(&tval);
	g_free(metadata_exported);	

	/* deleted */
	gboolean metadata_deleted = FALSE;
	g_object_get(metadata, "deleted", &metadata_deleted, NULL);
	g_assert(metadata_deleted == FALSE);

	/* islocked */
	gboolean metadata_islocked = FALSE;
	g_object_get(metadata, "islocked", &metadata_islocked, NULL);
	g_assert(metadata_islocked == FALSE);

	/* isapproved NA */
	gboolean metadata_isapproved = FALSE;
	g_object_get(metadata, "isapproved", &metadata_isapproved, NULL);
	g_assert(metadata_isapproved == FALSE);

	return;
}

void midgard_test_metadata_check_delete(MidgardObject *object)
{
	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS (object);
	if (!midgard_object_class_has_metadata (klass))
		return;
	
	MidgardMetadata *metadata;
	g_object_get(object, "metadata", &metadata, NULL);	

	GValue tval = {0, };

	/*created */
	gchar *metadata_created = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "created", &tval);
	metadata_created = midgard_timestamp_get_string_from_value(&tval);
	g_assert_cmpstr(metadata_created, !=, NULL);
	g_value_unset(&tval);
	g_free(metadata_created);
	
	/* creator */
	gchar *metadata_creator = NULL;
	g_object_get(metadata, "creator", &metadata_creator, NULL);
	g_assert_cmpstr(metadata_creator, !=, NULL);
	g_free(metadata_creator);
	
	/* revisor */
	gchar *metadata_revisor = NULL;
	g_object_get(metadata, "revisor", &metadata_revisor, NULL);
	g_assert_cmpstr(metadata_revisor, !=, NULL);
	g_free(metadata_revisor);

	/* revised */
	gchar *metadata_revised = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "revised", &tval);
	metadata_revised = midgard_timestamp_get_string_from_value(&tval);
	g_assert_cmpstr(metadata_revised, !=, NULL);
	g_value_unset(&tval);
	g_free(metadata_revised);
	
	/* revision */
	guint metadata_revision = 0;
	g_object_get(metadata, "revision", &metadata_revision, NULL);
	/* FIXME , enable */
	/*g_assert_cmpuint(metadata_revision, >, 1); */

	/* locker  NA */
	/* Object might be locked and unlocked before deletion */

	/* locked */
	gchar *metadata_locked = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "locked", &tval);
	metadata_locked = midgard_timestamp_get_string_from_value(&tval);
	__metadata_datetime_empty(metadata_locked);
	g_value_unset(&tval);
	g_free(metadata_locked);	

	/* approver NA */
	/* object might be approved/unapproved many times before deletion */

	/* approved */
	gchar *metadata_approved = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "approved", &tval);
	metadata_approved = midgard_timestamp_get_string_from_value(&tval);
	__metadata_datetime_empty(metadata_approved);
	g_value_unset(&tval);
	g_free(metadata_approved);	

	/* owner NA */
	/* schedulestart NA */
	/* scheduleend NA */
	/* hidden NA */
	/* navnoentry NA */

	/* size */
	guint metadata_size = 0;
	g_object_get(metadata, "size", &metadata_size, NULL);
	g_assert_cmpuint(metadata_size, >, 0);

	/* published NA */
	/* score NA */
	
	/* imported */
	gchar *metadata_imported = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "imported", &tval);
	metadata_imported = midgard_timestamp_get_string_from_value(&tval);
	__metadata_datetime_empty(metadata_imported);
	g_value_unset(&tval);
	g_free(metadata_imported);	

	/* exported */
	gchar *metadata_exported = NULL;
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "exported", &tval);
	metadata_exported = midgard_timestamp_get_string_from_value(&tval);
	__metadata_datetime_empty(metadata_exported);
	g_value_unset(&tval);
	g_free(metadata_exported);	

	/* deleted */
	gboolean metadata_deleted = FALSE;
	g_object_get(metadata, "deleted", &metadata_deleted, NULL);
	g_assert(metadata_deleted == TRUE);

	/* islocked */
	gboolean metadata_islocked = FALSE;
	g_object_get(metadata, "islocked", &metadata_islocked, NULL);
	g_assert(metadata_islocked == FALSE);

	/* isapproved NA */
	
	return;
}

void midgard_test_metadata_check_person_references(MidgardObject *object, const gchar *guid, ...)
{
	g_assert (object != NULL);

	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS (object);
	if (!midgard_object_class_has_metadata (klass))
		return;
	
	va_list var_args;
	va_start (var_args, guid);
	const gchar *property = va_arg(var_args, gchar*);
	MidgardMetadata *metadata;
	g_object_get (object, "metadata", &metadata, NULL);
	gchar *metadata_property;

	while (property != NULL) {
		
		g_object_get (metadata, property, &metadata_property, NULL);

		/* Allow empty or null values */
		if (!metadata_property 
				|| (metadata_property && *metadata_property == '\0')) {

			g_free (metadata_property);
			property = va_arg (var_args, gchar *);
			continue;
		}

		/* Check if property value holds guid */
		g_assert (midgard_is_guid(metadata_property) == TRUE);
		/* Check if property holds the same value as given one */
		g_assert_cmpstr (metadata_property, ==, guid);
		g_free (metadata_property);
		property = va_arg (var_args, gchar*);
	}
	
	va_end (var_args);

	return;
}

void midgard_test_metadata_check_datetime_properties(MidgardObject *object, const gchar *datetime, ...)
{
	g_assert(object != NULL);

	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS (object);
	if (!midgard_object_class_has_metadata (klass))
		return;
	
	va_list var_args;
	va_start(var_args, datetime);
	const gchar *property = va_arg(var_args, gchar*);
	MidgardMetadata *metadata;
	g_object_get(object, "metadata", &metadata, NULL);
	gchar *metadata_property;
	GValue tval = {0, };

	while(property != NULL) {
		
		g_value_init(&tval, MGD_TYPE_TIMESTAMP);
		g_object_get_property(G_OBJECT(metadata), property, &tval);
		metadata_property = midgard_timestamp_get_string_from_value(&tval);
		g_assert_cmpstr(metadata_property, !=, NULL);

		/* check if value is the same as given one */
		/* Append timezone if DB doesn't support it */
		guint datetime_property_length = strlen(metadata_property);
		g_assert_cmpuint(datetime_property_length, <, 25); 

		GString *metadata_datetime_string = g_string_new(metadata_property);
		if(datetime_property_length == 19) {
			g_string_append(metadata_datetime_string, "+0000");
		}
		
		g_assert_cmpstr(metadata_datetime_string->str, ==, datetime);
		g_string_free(metadata_datetime_string, TRUE);
		g_free(metadata_property);
		g_value_unset(&tval);
		property = va_arg(var_args, gchar*);
	}
	
	va_end(var_args);

	return;
}
