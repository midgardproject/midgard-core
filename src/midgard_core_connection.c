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
 */

#include "midgard_connection.h"
#include "midgard_core_connection.h"
#include "midgard_core_xml.h"
#include "midgard_core_object.h"
#include "midgard_core_config.h"

MgdCoreAuthType *
midgard_core_auth_type_new (const gchar *name, guint id) 
{
	MgdCoreAuthType *mcat = g_new (MgdCoreAuthType, 1);
	mcat->name = g_strdup(name);
	mcat->id = id;

	return mcat;
}

void 
midgard_core_auth_type_free (MgdCoreAuthType *mcat)
{
	g_assert (mcat != NULL);

	g_free (mcat->name);
	mcat->name = NULL;
	mcat->id = 0;

	g_free (mcat);
	mcat = NULL;
}

static void
_add_auth_type (GSList **slist, MgdCoreAuthType *mcat)
{
	GSList *l = NULL;

	for (l = *slist; l != NULL; l = l->next) {
		
		MgdCoreAuthType *at = (MgdCoreAuthType *) l->data;
		if (g_str_equal (at->name, mcat->name))
			g_error ("Invalid, duplicated %s authentication type", mcat->name);

		if (at->id == mcat->id)
			g_error ("Invalid, duplicated %d authentication id", mcat->id);
	}

	*slist = g_slist_append (*slist, mcat);

	return;
}

static guint 
_get_auth_type_id (xmlNode *node)
{
	xmlNode *child;
	guint id = 0;

	for (child = node->children; child != NULL; child = child->next) {

		if (child->type == XML_ELEMENT_NODE
				&& g_str_equal (child->name, "id")) {

			xmlChar *val = xmlNodeGetContent(child);

			if (!val)
				return 0;

			 id = atoi ((const gchar *)val);
			 xmlFree(val);
		 }
	}

	return id;
}

void 
midgard_core_connection_initialize_auth_types (MidgardConnection *mgd)
{
	g_return_if_fail (mgd != NULL);

	MidgardConfig *config = mgd->priv->config;
	g_return_if_fail (config != NULL);
	gchar *authfile = g_build_path (G_DIR_SEPARATOR_S, config->sharedir, MGD_CORE_AUTH_FILE, NULL);

	if (!g_file_test ((const gchar *)authfile, G_FILE_TEST_IS_REGULAR))
		g_error("%s authentication types file not found", authfile);
	
	xmlDocPtr doc = xmlParseFile (authfile);
	if (doc == NULL) 
		g_error ("Malformed authentication type file %s", authfile);
	
	xmlNodePtr root = xmlDocGetRootElement (doc);

	if (root == NULL
			|| root->ns == NULL
			|| !g_str_equal(root->ns->href, MGD_CORE_AUTH_NS) 
			|| !g_str_equal(root->name, MGD_CORE_AUTH_ROOT_NAME)) 
		g_error ("Invalid authentication types file %s", authfile);
	
	xmlNode *node;
	GSList *slist = NULL;

	for (node = root->children; node != NULL; node = node->next) {
		
		if (node->type != XML_ELEMENT_NODE)
			continue;

		if (!g_str_equal (node->name, "type"))
			continue;

		xmlChar *name = xmlGetProp (node, (const xmlChar*) "name");

		if (!name) {
			
			g_warning ("Invalid authentication type name on line %ld", xmlGetLineNo (node));
			xmlFree (name);
			continue;
		}

		guint id = _get_auth_type_id (node);

		if (id == 0) {
			
			g_warning ("Invalid authentication type id on line %ld", xmlGetLineNo (node));
			xmlFree (name);
			continue;
		}

		MgdCoreAuthType *mcat = midgard_core_auth_type_new ((const gchar*)name, id);
		xmlFree (name);
		_add_auth_type (&slist, mcat);
	}

	mgd->priv->authtypes = slist;

	xmlFreeDoc (doc);
	g_free(authfile);
}

gboolean 
midgard_core_connection_auth_type_by_name (MidgardConnection *mgd, const gchar *name)
{
	GSList *list = NULL;

	for (list = mgd->priv->authtypes; list != NULL; list = list->next) {
		
		MgdCoreAuthType *mcat = (MgdCoreAuthType *) list->data;

		if (g_str_equal (mcat->name, name))
			return TRUE;	
	}

	return FALSE;
}

gboolean 
midgard_core_connection_auth_type_by_id (MidgardConnection *mgd, guint id)
{
	GSList *list = NULL;

	for (list = mgd->priv->authtypes; list != NULL; list = list->next) {
		
		MgdCoreAuthType *mcat = (MgdCoreAuthType *) list->data;

		if (mcat->id == id)
			return TRUE;	
	}

	return FALSE;
}

guint           
midgard_core_auth_type_id_from_name (MidgardConnection *mgd, const gchar *name)
{
	GSList *list = NULL;

	for (list = mgd->priv->authtypes; list != NULL; list = list->next) {
		
		MgdCoreAuthType *mcat = (MgdCoreAuthType *) list->data;

		if (g_str_equal (mcat->name, name))
			return mcat->id;	
	}

	return 0;

}

static void 
_connect_cnc_callback (GdaConnection *cnc, GdaConnectionEvent *event, gpointer ud)
{

	glong errcode = (glong) gda_connection_event_get_code(event);

	g_warning("Midgard connection error: %s, No:(%li), src:%s, SQL: %s",
			gda_connection_event_get_description(event),
			errcode,
			gda_connection_event_get_source(event),
			gda_connection_event_get_sqlstate (event));
	
	if (ud == NULL || !MIDGARD_IS_CONNECTION(ud))
		return;

	MidgardConnection *mgd = MIDGARD_CONNECTION(ud);
	MidgardConfig *config = mgd->priv->config;
	guint dbtype = config->priv->dbtype;

	switch (dbtype) {

		case MIDGARD_DB_TYPE_MYSQL:
			if (errcode == 2006) { /* MySQL server has gone away */

				g_signal_emit(mgd, MIDGARD_CONNECTION_GET_CLASS(mgd)->signal_id_lost_provider, 0);
				//gda_connection_close_no_warning(mgd->priv->connection);
				//__mysql_reconnect(mgd); /* TODO, check if it leaks */
			}

			break;
	}
}

void 
midgard_core_connection_connect_error_callback (MidgardConnection *mgd)
{
	g_return_if_fail (mgd != NULL);

	if (mgd->priv->error_clbk_connected)
		return;
	
	GdaConnection *connection = mgd->priv->connection;
	g_signal_connect(G_OBJECT(connection), "error", G_CALLBACK(_connect_cnc_callback), (gpointer) mgd);

	/* if (mgd->priv->pattern)
		g_signal_connect(G_OBJECT(connection), "error", G_CALLBACK(_connect_cnc_callback), (gpointer) mgd->priv->pattern); */

	mgd->priv->error_clbk_connected = TRUE;

	return;
}

void 
midgard_core_connection_disconnect_error_callback (MidgardConnection *mgd)
{
	g_return_if_fail (mgd != NULL);

	if (!mgd->priv->error_clbk_connected)
		return;
	
	GdaConnection *connection = mgd->priv->connection;
	g_object_disconnect(G_OBJECT(connection), "any_signal", G_CALLBACK(_connect_cnc_callback), mgd, NULL);

	/* if (mgd->priv->pattern)
		g_object_disconnect(G_OBJECT(connection), "any_signal", G_CALLBACK(_connect_cnc_callback), mgd->priv->pattern, NULL); */

	mgd->priv->error_clbk_connected = FALSE;

	return;
}
