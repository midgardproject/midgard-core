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


#include "midgard_core_query.h"
#include "midgard_workspace_storage.h"
#include "midgard_workspace.h"
#include "midgard_core_object.h"
#include "midgard_core_workspace.h"

void
midgard_core_workspace_list_all (MidgardConnection *mgd)
{
	g_return_if_fail (mgd != NULL);

	/* TODO, provide statement with parameters */
	GString *sql = g_string_new ("SELECT ");
	g_string_append_printf (sql, "%s, %s, %s, %s FROM %s",
			MGD_WORKSPACE_FIELD_ID, 
			MGD_WORKSPACE_FIELD_UP,
			MGD_WORKSPACE_FIELD_GUID,
			MGD_WORKSPACE_FIELD_NAME,
			MGD_WORKSPACE_TABLE);

	/* Query all workspaces. Returned data model will be used as temporary 
	 * (in memory dataset) for faster and easier workspaces management */
	GdaDataModel *model = midgard_core_query_get_model (mgd, sql->str);
	g_string_free (sql, TRUE);

	if (!model)
		return;

	mgd->priv->workspace_model = model;
}

gint
midgard_core_workspace_get_col_id_by_name (MidgardConnection *mgd, const gchar *name, gint col_idx, gint up_id_check, guint *row_id)
{
	g_return_val_if_fail (mgd != NULL, -1);
	g_return_val_if_fail (name != NULL, -1);

	*row_id = 0;
	guint row;
	GdaDataModel *model = mgd->priv->workspace_model;
	/* model is not initialized, which means there's no workspace created yet */
	if (!model)
		return -1;

	guint ret_rows = gda_data_model_get_n_rows (model);
	if (ret_rows == 0)
		return -1;

	guint ret_cols = gda_data_model_get_n_columns (model);

	for (row = 0; row < ret_rows; row++) {

		const GValue *upval = gda_data_model_get_value_at (model, MGD_WORKSPACE_FIELD_IDX_UP, row, NULL);
		guint up_id;
		if (G_VALUE_HOLDS_UINT (upval))
			up_id = g_value_get_uint (upval);
		else 
			up_id = g_value_get_int (upval);

		/* Check if the row in question holds values for parent workspace */
		if (up_id != up_id_check)
			continue;

		const GValue *nval = gda_data_model_get_value_at (model, MGD_WORKSPACE_FIELD_IDX_NAME, row, NULL);	
		if (g_str_equal (name, g_value_get_string (nval))) {
			*row_id = row;
			const GValue *colval = gda_data_model_get_value_at (model, col_idx, row, NULL);
			if (G_VALUE_HOLDS_UINT (colval))
				return (gint) g_value_get_uint (colval);
			return g_value_get_int (colval);
		}
	}

	return -1;
}

gint
midgard_core_workspace_get_up_id_by_name (MidgardConnection *mgd, const gchar *name, guint up_id)
{
	guint row_id;
	return midgard_core_workspace_get_col_id_by_name (mgd, name, MGD_WORKSPACE_FIELD_IDX_UP, up_id, &row_id);
}

gint
midgard_core_workspace_get_id_by_name (MidgardConnection *mgd, const gchar *name)
{
	return midgard_core_workspace_get_up_id_by_name (mgd, name, MGD_WORKSPACE_FIELD_IDX_ID);
}

gboolean 
midgard_core_workspace_name_exists (MidgardWorkspace *workspace, MidgardWorkspace *parent)
{
	MidgardConnection *mgd = MGD_OBJECT_CNC (workspace);
	g_return_val_if_fail (mgd != NULL, FALSE);
	g_return_val_if_fail (workspace != NULL, FALSE);

	const gchar *name = (const gchar *)workspace->priv->name;
	gint up_id = 0;
	if (parent) 
		up_id = parent->priv->id;

	guint row_id;
	gint exists = midgard_core_workspace_get_col_id_by_name (mgd, name, MGD_WORKSPACE_FIELD_IDX_ID, up_id, &row_id);

	if (exists == -1)
		return FALSE;

	return TRUE;
}

const GValue *
midgard_core_workspace_get_value_by_id (MidgardConnection *mgd, guint col_idx, guint id, guint *row_id)
{
	g_return_val_if_fail (mgd != NULL, NULL);

	guint row;
	if (row_id)
		*row_id = 0;

	GdaDataModel *model = mgd->priv->workspace_model;
	if (!model) {
		g_critical ("Failed to get workspace name by itd id. Workspace data model is not set");
		return NULL;
	}

	guint ret_rows = gda_data_model_get_n_rows (model);
	if (ret_rows == 0)
		return NULL;

	guint ret_cols = gda_data_model_get_n_columns (model);

	for (row = 0; row < ret_rows; row++) {

		const GValue *idval = gda_data_model_get_value_at (model, MGD_WORKSPACE_FIELD_IDX_ID, row, NULL);
		guint model_id;
		if (G_VALUE_HOLDS_UINT (idval))
			model_id = g_value_get_uint (idval);
		else 
			model_id = g_value_get_int (idval);

		/* Check if the row in question holds id value */
		if (id != model_id)
			continue;

		if (row_id)
			*row_id = row;

		return gda_data_model_get_value_at (model, col_idx, row, NULL);	
	}

	return NULL;
}

static GSList *
_get_parent_values (MidgardConnection *mgd, guint up, guint field_idx)
{
	g_return_val_if_fail (mgd != NULL, NULL);

	GSList *list = NULL;
	if (up == 0)
		return NULL;

	const GValue *value = NULL;
	guint row_id;

	do {
		value = midgard_core_workspace_get_value_by_id (mgd, field_idx, up, &row_id);
		if (value) {
			list = g_slist_prepend (list, (gpointer) value);
			const GValue *up_value = midgard_core_workspace_get_value_by_id (mgd, MGD_WORKSPACE_FIELD_IDX_UP, up, &row_id);
			if (up_value) {
				guint up_id = 0;
				if (G_VALUE_HOLDS_UINT (up_value))
					up_id = g_value_get_uint (up_value);
				else 
					up_id = g_value_get_int (up_value);
				up = up_id;
			}

			if (up == 0)
				break;
		}

	} while (value != NULL);
	
	return list;
}

GSList *
midgard_core_workspace_get_context_ids (MidgardConnection *mgd, guint id)
{
	g_return_val_if_fail (mgd != NULL, NULL);
	g_return_val_if_fail (id != 0, NULL);

	guint row_id;
	const GValue *up_val = midgard_core_workspace_get_value_by_id (mgd, MGD_WORKSPACE_FIELD_IDX_UP, id, &row_id);
	guint up_id = 0;
	if (G_VALUE_HOLDS_UINT (up_val))
		up_id = g_value_get_uint (up_val);
	else 
		up_id = (guint) g_value_get_int (up_val);

	GSList *l = _get_parent_values (mgd, up_id, MGD_WORKSPACE_FIELD_IDX_ID);

	GValue *id_val = g_new0 (GValue, 1);
	g_value_init (id_val, G_TYPE_UINT);
	g_value_set_uint (id_val, id);

	if (l)
		l = g_slist_reverse (l);

	l = g_slist_prepend (l, (gpointer) id_val);

	return l;
}

GSList *
midgard_core_workspace_get_parent_names (MidgardConnection *mgd, guint up)
{
	g_return_val_if_fail (mgd != NULL, NULL);

	GSList *list = NULL;
	if (up == 0)
		return NULL;

	const GValue *value = NULL;
	const gchar *name = NULL;
	guint row_id;

	do {
		value = midgard_core_workspace_get_value_by_id (mgd, MGD_WORKSPACE_FIELD_IDX_NAME, up, &row_id);
		if (value) {
			name = g_value_get_string (value);
			list = g_slist_prepend (list, (gpointer) name);
			const GValue *up_value = midgard_core_workspace_get_value_by_id (mgd, MGD_WORKSPACE_FIELD_IDX_UP, up, &row_id);
			if (up_value) {
				guint up_id = 0;
				if (G_VALUE_HOLDS_UINT (up_value))
					up_id = g_value_get_uint (up_value);
				else 
					up_id = g_value_get_int (up_value);
				up = up_id;
			}

			if (up == 0)
				break;
		}

	} while (name != NULL);
	
	return list;
}

gint 
midgard_core_workspace_get_id_by_path (MidgardConnection *mgd, const gchar *path, guint *row_id, GError **error)
{
	GError *err = NULL;
	gchar **tokens = g_strsplit (path, "/", 0);
	guint i = 0;
	*row_id = 0;
	/* If path begins with slash, first element is an empty string. Ignore it. */
	if (*tokens[0] == '\0')
		i++;

	guint id = -1;
	gint j = i;
	gboolean valid_path = TRUE;
	/* Validate path */
	while (tokens[i] != NULL) {
		if (tokens[i] == '\0') 
			valid_path = FALSE;
		i++;
	}

	if (!valid_path) {
		err = g_error_new (MIDGARD_WORKSPACE_STORAGE_ERROR, WORKSPACE_STORAGE_ERROR_INVALID_PATH, "An empty element found in given path");
		g_propagate_error (error, err);
		g_strfreev (tokens);
		return id;
	}

	/* Check every possible workspace_storage name */
	id = 0;
	guint up = 0;
	const gchar *name = NULL;
	while (tokens[j] != NULL) {
		name = tokens[j];	
		id = midgard_core_workspace_get_col_id_by_name (mgd, name, MGD_WORKSPACE_FIELD_IDX_ID, up, row_id);
		if (id == -1)
			break;
		up = id;
		j++;
	}

	if (id == -1) {
		err = g_error_new (MIDGARD_WORKSPACE_STORAGE_ERROR, WORKSPACE_STORAGE_ERROR_OBJECT_NOT_EXISTS, "WorkspaceStorage doesn't exists at given path");
		g_propagate_error (error, err);
	}
	
	g_strfreev (tokens);
	return id;
}

