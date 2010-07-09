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
midgard_core_workspace_get_col_id_by_name (MidgardConnection *mgd, const gchar *name, gint col_idx, gint up_id_check)
{
	g_return_val_if_fail (mgd != NULL, -1);
	g_return_val_if_fail (name != NULL, -1);

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
	return midgard_core_workspace_get_col_id_by_name (mgd, name, MGD_WORKSPACE_FIELD_IDX_UP, up_id);
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

	gint exists = midgard_core_workspace_get_col_id_by_name (mgd, name, MGD_WORKSPACE_FIELD_IDX_ID, up_id);

	if (exists == -1)
		return FALSE;

	return TRUE;
}

static const GValue *
midgard_core_workspace_get_value_by_id (MidgardConnection *mgd, guint col_idx, guint id)
{
	g_return_val_if_fail (mgd != NULL, NULL);

	guint row;
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

		/* Check if the row in question holds values for parent workspace */
		if (id != model_id)
			continue;

		return gda_data_model_get_value_at (model, col_idx, row, NULL);	
	}

	return NULL;
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

	do {
		value = midgard_core_workspace_get_value_by_id (mgd, MGD_WORKSPACE_FIELD_IDX_NAME, up);
		if (value) {
			name = g_value_get_string (value);
			list = g_slist_prepend (list, (gpointer) name);
			const GValue *up_value = midgard_core_workspace_get_value_by_id (mgd, MGD_WORKSPACE_FIELD_IDX_UP, up);
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
