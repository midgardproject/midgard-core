/* 
 * Copyright (C) 2006 Piotr Pokora <piotr.pokora@infoglob.pl>
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

#include <glib.h>
#include <glib-object.h>
#include <libxml/parser.h> 
#include <libxml/tree.h>
#include <libxml/xmlreader.h>
#include <libxml/parserInternals.h>
#include <sys/stat.h>
#include "midgard_defs.h"
#include "midgard_datatypes.h"
#include "midgard_error.h"
#include "midgard_core_object.h"
#include "midgard_core_query.h"
#include "midgard_reflection_property.h"
#include "midgard_collector.h"
#include "midgard_blob.h"
#include "midgard_user.h"
#include "midgard_object.h"
#include "midgard_metadata.h"
#include "guid.h"
#include "midgard_core_metadata.h"
#include "midgard_core_object_class.h"
#include "midgard_schema_object_factory.h"

static const gchar *MIDGARD_OBJECT_HREF = "http://www.midgard-project.org/midgard_object/1.8";

static void _write_nodes(GObject *object, xmlNodePtr node)
{
	g_assert(object);
	g_assert(node);

	guint prop_n;
	MidgardObject *mgdobject = (MidgardObject *)object;
	MidgardReflectionProperty *mrp = NULL;
	MidgardObjectClass *klass = NULL;
	GParamSpec **pspec = g_object_class_list_properties(
			G_OBJECT_GET_CLASS(G_OBJECT(object)), &prop_n);
	
	if(MIDGARD_IS_OBJECT(object)) {
		klass = 
			MIDGARD_OBJECT_GET_CLASS(object);
		if(klass)
			mrp = midgard_reflection_property_new(
					MIDGARD_DBOBJECT_CLASS(klass));
	}

	GValue pval = {0, }, *lval;
	GString *gstring;
	gchar *strprop = NULL;
	xmlChar *escaped;
	guint i;
	xmlNodePtr op_node = NULL;
	const gchar *linktype;
	MidgardCollector *mc;
	guint _uint;
	gint _int;
	GObject *_object;

	if(MIDGARD_IS_OBJECT(mgdobject)) {

		gint object_action = -1;	

		if(MGD_OBJECT_GUID (mgdobject)) {

			GString *_sql = g_string_new(" ");
			g_string_append_printf(_sql, "guid = '%s' ",	
					MGD_OBJECT_GUID (mgdobject));
			
			gchar *tmpstr = g_string_free(_sql, FALSE);
						
			GValue *avalue =
				midgard_core_query_get_field_value(
						MGD_OBJECT_CNC (mgdobject),	
						"object_action",
						"repligard", 
						(const gchar*)tmpstr);

			if(avalue) {
				
				MIDGARD_GET_UINT_FROM_VALUE(object_action, avalue);
				g_value_unset(avalue);
				g_free(avalue);
			}
			
			g_free(tmpstr);
		}

		gchar *_action;
		if(object_action > -1) {
			
			switch(object_action) {
				
				case MGD_OBJECT_ACTION_CREATE:
					_action = "created";		
					break;
				
				case MGD_OBJECT_ACTION_UPDATE:
					_action = "updated";
					break;
				
				case MGD_OBJECT_ACTION_DELETE:
					_action = "deleted";
					break;
				
				case MGD_OBJECT_ACTION_PURGE:
					_action = "purged";
					break;
				
				default:
					_action = "none";
					break;
			}
			
			xmlNewProp(node, BAD_CAST "action",
					BAD_CAST _action);
		}
	}

	for(i = 0; i < prop_n; i++) {
		
		g_value_init(&pval,pspec[i]->value_type);
		g_object_get_property(G_OBJECT(object), 
				pspec[i]->name, &pval);

		if(g_str_equal("guid", pspec[i]->name)) {
			/* Add guid attribute */
			xmlNewProp(node, BAD_CAST "guid", 
					BAD_CAST MGD_OBJECT_GUID (object));
			g_value_unset(&pval);
			continue;
		}
				
		/* Object is not fetched from database. Skip references */
		if(MGD_OBJECT_GUID (mgdobject) == NULL)
			goto export_unchecked_property;

		/* If property is a link we need to query guid 
		 * which identifies link object. Only if property 
		 * is not of guid or string type */
		if(mrp){	
			if(midgard_reflection_property_is_link(mrp, 
						pspec[i]->name)){

				lval = g_new0(GValue, 1);
				switch(pspec[i]->value_type) {
					
					case G_TYPE_UINT:
						g_value_init(lval, G_TYPE_UINT);
						_uint = g_value_get_uint(&pval);
						if(!_uint){
							g_value_unset(lval);
							g_free(lval);
							goto export_unchecked_property;
						}
						g_value_set_uint(lval, _uint);
						break;

					case G_TYPE_INT:
						g_value_init(lval, G_TYPE_INT);
						_int = g_value_get_int(&pval);
						if(!_int){
							g_value_unset(lval);
							g_free(lval);
							goto export_unchecked_property;
						}
						g_value_set_int(lval, _int);
						break;

					default:
						g_free(lval);
						goto export_unchecked_property;
				}
					
				linktype = 
					midgard_reflection_property_get_link_name(
							mrp, pspec[i]->name);
				
				if(linktype){
					mc = midgard_collector_new(
							MGD_OBJECT_CNC (mgdobject),
							linktype,
							"id",
							lval);
					
					midgard_collector_set_key_property(
							mc,
							"guid", NULL);
					if(!midgard_collector_execute(mc)){
						g_object_unref(mc);
						g_value_unset(&pval);
						continue;
					}				
					gchar **linkguid = 
						midgard_collector_list_keys(mc);
					if(linkguid){
						if(linkguid[0])
							strprop = g_strdup(linkguid[0]);
					}
					if(!strprop)
						strprop = g_strdup("");
					
					/* Create node */
					escaped = xmlEncodeEntitiesReentrant(
							NULL, (const xmlChar*)strprop);
					xmlNewTextChild(node, NULL,
							BAD_CAST pspec[i]->name,
							BAD_CAST escaped);
					
					g_free(linkguid);
					g_free(strprop);
					g_free(escaped);
					g_object_unref(mc);	
				}
				g_value_unset(&pval);
				continue;
			}
		}	
		
		export_unchecked_property:
		switch (G_TYPE_FUNDAMENTAL(pspec[i]->value_type)) {
			
			case G_TYPE_STRING:				
				strprop = g_value_dup_string(&pval);
				if(!strprop)
					strprop = g_strdup("");
				escaped = xmlEncodeEntitiesReentrant(
						NULL, (const xmlChar*)strprop);
				xmlNewTextChild(node, NULL, 
						BAD_CAST pspec[i]->name,
						BAD_CAST escaped);
			         
				g_free(strprop);
				g_free(escaped);
				break;

			case G_TYPE_INT:
				gstring = g_string_new("");
				g_string_append_printf(gstring,
						"%d", g_value_get_int(&pval));
				xmlNewChild(node, NULL, 
						BAD_CAST pspec[i]->name,
						BAD_CAST (xmlChar *)gstring->str);
				g_string_free (gstring, TRUE);
				break;

			case G_TYPE_UINT:
				gstring = g_string_new("");
				g_string_append_printf(gstring,
						"%d", g_value_get_uint(&pval));
				xmlNewChild(node, NULL, 
						BAD_CAST pspec[i]->name,
						BAD_CAST (xmlChar *)gstring->str);
				g_string_free (gstring, TRUE);
				break;

			case G_TYPE_FLOAT:
				gstring = g_string_new("");
				g_string_append_printf(gstring,
						"%g", g_value_get_float(&pval));
				xmlNewChild(node, NULL,
						BAD_CAST pspec[i]->name,
						BAD_CAST (xmlChar *)gstring->str);
				g_string_free (gstring, TRUE);
				break;

			case G_TYPE_BOOLEAN:
				if(g_value_get_boolean(&pval))
					strprop = "1";
				else 
					strprop = "0";

				xmlNewChild(node, NULL,
						BAD_CAST pspec[i]->name,
						BAD_CAST (xmlChar *)strprop);
				break;

			case G_TYPE_OBJECT:
	
				_object = g_value_get_object (&pval);
				
				if (_object) {

					op_node = xmlNewNode(NULL, BAD_CAST pspec[i]->name);
					_write_nodes(_object, op_node);
					xmlAddChild(node, op_node);
				}
				break;

			default:

				if (pspec[i]->value_type == MIDGARD_TYPE_TIMESTAMP) {
		
					GValue strval = {0, };
					g_value_init (&strval, G_TYPE_STRING);
					g_value_transform (&pval, &strval);
					xmlNewChild(node, NULL, 
						BAD_CAST pspec[i]->name,
						BAD_CAST (xmlChar *)g_value_get_string (&strval));
					g_value_unset (&strval);

				} else {
					g_warning ("midgard_replicator_serialize: unhandled %s property type (%s)", 
							pspec[i]->name, g_type_name (pspec[i]->value_type)); 
				}
		}

		g_value_unset(&pval);
	}

	g_free(pspec);

	if(mrp)
		g_object_unref(mrp);
}		

gchar *midgard_core_object_to_xml(GObject *object)
{
	g_assert(object);
	
	xmlDocPtr doc = NULL; 
	xmlNodePtr root_node = NULL;
		
	LIBXML_TEST_VERSION;
		
	doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, 
			BAD_CAST "midgard_object");
	xmlNewNs(root_node,
			BAD_CAST MIDGARD_OBJECT_HREF,
			NULL);
	xmlNodePtr object_node = 
		xmlNewNode(NULL, BAD_CAST G_OBJECT_TYPE_NAME(G_OBJECT(object)));
	/* Add purged info */
	/* We could add this attribute in _write_nodes function 
	 * but this could corrupt xsd compatibility for midgard_metadata nodes.
	 * So it's added here and for every multilingual content */
	xmlNewProp(object_node, BAD_CAST "purge", BAD_CAST "no");
	xmlAddChild(root_node, object_node);
	xmlDocSetRootElement(doc, root_node);
	_write_nodes(G_OBJECT(object), object_node);

	xmlChar *buf;
	gint size;
	xmlDocDumpFormatMemoryEnc(doc, &buf, &size, "UTF-8", 1);
	
	xmlFreeDoc(doc);
	xmlCleanupParser();

	return (gchar*) buf;
}

xmlDoc *midgard_core_object_create_xml_doc(void)
{
	xmlDocPtr doc = NULL;
	xmlNodePtr root_node = NULL;

	doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL,
			BAD_CAST "midgard_object");
	xmlNewNs(root_node,
			BAD_CAST MIDGARD_OBJECT_HREF,
			NULL);

	xmlDocSetRootElement(doc, root_node);

	return doc;
}

xmlNode *_get_type_node(xmlNode *node)
{
	xmlNode *cur = NULL;	
	for (cur = node; cur; cur = cur->next) {
		if (cur->type == XML_ELEMENT_NODE)
			return cur;
	}
	return NULL;
}

gboolean _nodes2object(GObject *object, xmlNode *node, gboolean force)
{
	g_assert(object);
	g_assert(node);

	xmlNode *cur = NULL;
	GObject *prop_object;
	gchar *nodeprop = NULL;
	xmlChar *decoded;
	xmlParserCtxtPtr parser;
	MidgardObject *mobject = NULL;
	MidgardObject *lobject = NULL;
	MidgardReflectionProperty *mrp = NULL;
	const gchar *linktype = NULL;

	if(MIDGARD_IS_OBJECT(object)) {
		mobject = MIDGARD_OBJECT(object);
		MidgardObjectClass *klass =
			MIDGARD_OBJECT_GET_CLASS(mobject);
		if(klass)
			mrp = midgard_reflection_property_new(
					MIDGARD_DBOBJECT_CLASS(klass));
	}

	gpointer set_from_xml_func = MIDGARD_DBOBJECT_GET_CLASS(object)->dbpriv->set_from_xml_node;
	if(set_from_xml_func != NULL) {
		MIDGARD_DBOBJECT_GET_CLASS(object)->dbpriv->set_from_xml_node(MIDGARD_DBOBJECT(object), node);
		return TRUE;
	}

	for (cur = node; cur; cur = cur->next) {
		if (cur->type == XML_ELEMENT_NODE) {
		
			linktype = NULL;
	
			GParamSpec *pspec = g_object_class_find_property(
					G_OBJECT_GET_CLASS(G_OBJECT(object)), 
					(const gchar *)cur->name);
			if(pspec) {
				GValue pval = {0, };
				g_value_init(&pval, pspec->value_type);
				
				if(nodeprop)
					g_free(nodeprop);

				nodeprop = (gchar *)xmlNodeGetContent(cur);

				if(mrp) {
					if(midgard_reflection_property_is_link(
								mrp, pspec->name)){
						linktype =
							midgard_reflection_property_get_link_name(
									mrp, pspec->name);
					}
				}

				/* moved out from mrp condition check to avoid nested indents */

				if(linktype && midgard_is_guid(
							(const gchar *) nodeprop)){

					/* Just set property quickly, if property holds a guid */
					GType mtype = midgard_reflection_property_get_midgard_type(mrp, pspec->name);
					if (mtype == MGD_TYPE_GUID) {
						
						g_value_unset(&pval);
						g_object_set(mobject, (const gchar *)cur->name, nodeprop, NULL);
						continue;	
					}

					/* we can use nodeprop directly */
					lobject = midgard_schema_object_factory_get_object_by_guid (
							MIDGARD_DBOBJECT (mobject)->dbpriv->mgd,
							(const gchar *) nodeprop);

					if(!lobject && !force){
						g_object_unref(mrp);
						g_value_unset(&pval);
						midgard_set_error(MGD_OBJECT_CNC (mobject), 
								MGD_GENERIC_ERROR, 
								MGD_ERR_MISSED_DEPENDENCE, 
								" Can not import %s. "
								"No '%s' object identified by '%s'",
								G_OBJECT_TYPE_NAME(object),
								linktype, nodeprop); 
						g_clear_error(&MIDGARD_DBOBJECT (mobject)->dbpriv->mgd->err);	
						return FALSE;
					}
					
					/* When force parameter is set we do not translate guids to ids */
					if(force && !lobject && midgard_is_guid(
								(const gchar *) nodeprop)) {
						
						switch(pspec->value_type) {
							
							case G_TYPE_UINT:
								g_value_set_uint(&pval, 0);
								break;
							
							case G_TYPE_INT:
								g_value_set_int(&pval, 0);
								break;
							
							default:
								goto set_property_unchecked;
								break;
						}
						
						g_object_set_property(
								G_OBJECT(object),
								(const gchar *) cur->name,
								&pval);
						g_value_unset(&pval);
						continue;
					}

					GValue tval = {0, };
					g_value_init(&tval, pspec->value_type);

					g_object_get_property(G_OBJECT(lobject),
							"id", &tval);

					if(G_VALUE_TYPE(&pval) == G_TYPE_INT)
						g_value_transform((const GValue *) &tval,
								&pval);
					else
						g_value_copy((const GValue*) &tval,
								&pval);

					g_object_set_property(
							G_OBJECT(object),
							(const gchar *) cur->name,
							&pval);
					g_value_unset(&pval);
					g_object_unref(lobject);
					g_value_unset(&tval);
					continue;					
				}

				set_property_unchecked:
				switch (G_TYPE_FUNDAMENTAL (pspec->value_type)) {
				
					case G_TYPE_STRING:
						parser = xmlNewParserCtxt();
						decoded = 
							xmlStringDecodeEntities(parser,
									(const xmlChar *) nodeprop, 
									XML_SUBSTITUTE_PEREF |
									XML_SUBSTITUTE_REF, 
									0, 0, 0);
						g_value_set_string(&pval, 
								(gchar *)decoded);
						g_free(decoded);
						xmlFreeParserCtxt(parser);
						break;

					case G_TYPE_INT:
						if(nodeprop) 
							g_value_set_int(&pval, 
									(gint)atoi((gchar *)nodeprop));
						break;

					case G_TYPE_UINT:
						if(nodeprop)
							g_value_set_uint(&pval,
									(guint)atoi((gchar *)nodeprop));
						break;

					case G_TYPE_FLOAT:
						g_value_set_float(&pval,
								(gfloat)atof((gchar *)nodeprop));
						break;

					case G_TYPE_BOOLEAN:
						g_value_set_boolean(&pval,
								(gboolean)atoi((gchar*)nodeprop));
						break;

					case G_TYPE_OBJECT:
						g_object_get(G_OBJECT(object),
								(const gchar *) cur->name,
								&prop_object, NULL);
						_nodes2object(prop_object, cur->children, force);	
						g_value_set_object(&pval, prop_object);
						break;

					default:
						/* do nothing */
						break;						
				}
				g_object_set_property(
						G_OBJECT(object), 
						(const gchar *) cur->name, 
						&pval);
				g_value_unset(&pval);
			} else {
				g_warning("Undefined property '%s' for '%s'",
						cur->name, G_OBJECT_TYPE_NAME(object));
			}
		}		
	}

	if(nodeprop)
		g_free(nodeprop);

	if(mrp)
		g_object_unref(mrp);

	return TRUE;
}

void midgard_core_object_get_xml_doc(	MidgardConnection *mgd, 
					const gchar *xml, 
					xmlDoc **doc, 
					xmlNode **root_node)
{
	g_assert(*doc == NULL);
	g_assert(*root_node == NULL);

	if(xml == NULL)
		g_warning("Creating Midgard Object from xml requires xml."
				"NULL passed");
	
	LIBXML_TEST_VERSION

	/* midgard_object.xml is a dummy base URL in this case */
	*doc = xmlReadMemory(xml, strlen(xml), "midgard_object.xml", NULL, 0);
	if(!*doc) {
		g_warning("Can not parse given xml");
		return;
	}

	*root_node = xmlDocGetRootElement(*doc);
	if(!*root_node) {
		g_warning("Can not find root element for given xml");
		xmlFreeDoc(*doc);
		doc = NULL;
		return;
	}

	/* TODO : Get version from href and fail if invalid version exists */
	if(!g_str_equal((*root_node)->ns->href, MIDGARD_OBJECT_HREF)
			|| !g_str_equal((*root_node)->name, "midgard_object")) {
		g_warning("Skipping invalid midgard_object xml");
		xmlFreeDoc(*doc);
		*doc = NULL;
		*root_node = NULL;
		return;
	}
}

GObject **midgard_core_object_from_xml(MidgardConnection *mgd, 
		const gchar *xml, gboolean force)
{
	xmlDoc *doc = NULL;
	xmlNode *root = NULL;
	midgard_core_object_get_xml_doc(mgd, xml, &doc, &root);

	if(doc == NULL && root == NULL)
		return NULL;

	/* we can try to get type name , so we can create schema object instance */
	/* Piotras: Either I am blind or there is no explicit libxml2 API call
	 * to get child node explicitly. That's why _get_type_node is used */
	xmlNodePtr child = _get_type_node(root->children);
	if(!child) {
		g_warning("Can not get midgard type name from the given xml");
		xmlFreeDoc(doc);
		return NULL;
	}

	guint _n_nodes = 0;
	MidgardObject **_objects = NULL;
	GSList *olist = NULL;

	for(; child; child = _get_type_node(child->next)) {
		
		if(!g_type_from_name((const gchar *)child->name)) {
			g_error("Type %s is not initialized in type system", 
					child->name);
			xmlFreeDoc(doc);
			return NULL;
		}
		
		/* Make any gobject instance. Avoid needless type name comparison. */
		GType object_type = g_type_from_name((const gchar *)child->name);
		GObject *object = g_object_new(object_type, NULL);

		if(!object) {
			g_error("Can not create %s instance", child->name);
			xmlFreeDoc(doc);
			return NULL;
		}

		/* DBObject instance so we need to set its connection */
		if(MIDGARD_IS_DBOBJECT(object))
			midgard_object_set_connection(MIDGARD_OBJECT(object), mgd);

		if(child->children && !_nodes2object(G_OBJECT(object), child->children, force)) {
			g_object_unref(object);
			object = NULL;		
		} else {

			xmlChar *attr = xmlGetProp(child, BAD_CAST "purge");
			if(g_str_equal(attr, "yes") && MIDGARD_IS_DBOBJECT(object)){
				g_object_set(object, "action", "purged", NULL);
				xmlFree(attr);
				attr = NULL;
				
				attr = xmlGetProp(child, BAD_CAST "guid");
				MIDGARD_DBOBJECT(object)->dbpriv->guid = 
					(const gchar *) g_strdup((gchar *)attr);
				xmlFree(attr);
				
				if(MIDGARD_IS_OBJECT(object)) {

					gchar *purged = (gchar *) xmlGetProp(child, BAD_CAST "purged");
					GValue strval = {0, };
					g_value_init(&strval, G_TYPE_STRING);
					g_value_set_string(&strval, purged);
					midgard_core_metadata_set_revised(MGD_DBOBJECT_METADATA(object), (const GValue*)&strval);
					g_value_unset(&strval);
				}
				
				olist = g_slist_prepend(olist, object);
				_n_nodes++;
				
				continue;
			}

			if(attr)
				xmlFree(attr);

			xmlChar *action_attr = xmlGetProp(child, BAD_CAST "action");
			g_object_set(G_OBJECT(object), "action", 
					(gchar *) action_attr, NULL);
			xmlFree(action_attr);

			olist = g_slist_prepend(olist, object);
			_n_nodes++;	

			/* Set guid, do not free attr, it will be freed when object
			 * is destroyed */
			attr = xmlGetProp(child, BAD_CAST "guid");
			if (attr) {
				if(MIDGARD_IS_DBOBJECT(object))
					MIDGARD_DBOBJECT(object)->dbpriv->guid = g_strdup((const gchar *)attr);
				if(MIDGARD_IS_BLOB(object)) 
					MIDGARD_BLOB(object)->priv->parentguid = g_strdup((gchar *)attr);

				xmlFree(attr);
			}
		}
	}

	if(_n_nodes > 0) {
		
		_objects = g_new(MidgardObject *, _n_nodes+1);
		_n_nodes = 0;
		
		for(; olist ; olist = olist->next){
			_objects[_n_nodes] = olist->data;
			_n_nodes++;
		}

		g_slist_free(olist);
		_objects[_n_nodes] = NULL;
	}

	xmlFreeDoc(doc);
	xmlCleanupParser();

	return (GObject **) _objects;
}

/* Returns TRUE if property is private and caller is not allowed to read/write it. */ 
gboolean 
midgard_core_object_property_refuse_private (MidgardConnection *mgd, MgdSchemaTypeAttr *type, 
		MidgardDBObject *object, const gchar *property)
{
	/* Unknown case, no connection  */
	g_return_val_if_fail (mgd != NULL, TRUE);
	g_return_val_if_fail (property != NULL, FALSE);

	MgdSchemaTypeAttr *type_attr = type;
	MidgardDBObjectClass *klass = NULL;

	if (object)
		klass = MIDGARD_DBOBJECT_GET_CLASS (object);

	/* TODO, check if it's valid for cases like collector usage */
	/* g_return_val_if_fail (object != NULL && type_attr != NULL, FALSE); */

	if (!type_attr && object)
		type_attr = klass->dbpriv->storage_data;

	/* Unknown case, no storage_data which means something like no property */
	g_return_val_if_fail (type_attr != NULL, FALSE);

	MgdSchemaPropertyAttr *attr = midgard_core_schema_type_property_lookup (type_attr, property);
	if (!attr)
		return FALSE;

	if (!attr->is_private)
		return FALSE;

	/* No object, so no chance to check metadata.creator.
	 * There might be corner case or feature work around, so refuse unconditionally. */
	if (!object) {
		MIDGARD_ERRNO_SET_STRING (mgd, MGD_ERR_ACCESS_DENIED, "No object. Can not check private property owner");
		return TRUE;
	}

	/* Also, no user, and anonymous is not allowed.
	 * If user exists, and has no person associated, refuse as well. */ 
	MidgardObject *person = MGD_CNC_PERSON (mgd);
	if (!mgd->priv->user) {
		MIDGARD_ERRNO_SET_STRING (mgd, MGD_ERR_ACCESS_DENIED, "No user logged in");
		return TRUE;
	}

	if (!person) {
		MIDGARD_ERRNO_SET_STRING (mgd, MGD_ERR_ACCESS_DENIED, "Can not check property owner. No person associated with logged in user");
		return TRUE;
	}

	/* OK, so we have admin, allow */
	if (midgard_user_is_admin (mgd->priv->user))
		return FALSE;

	const gchar *person_guid = MGD_OBJECT_GUID (person);

	/* Something really strange, person has no guid associated */
	if (!person_guid) {
		MIDGARD_ERRNO_SET_STRING (mgd, MGD_ERR_ACCESS_DENIED, "Midgard person identified by empty guid");
		return TRUE;
	}

	gchar *creator_guid = NULL;

	if (MIDGARD_IS_OBJECT (object)) {
		MidgardMetadata *metadata = MGD_DBOBJECT_METADATA (object);
		creator_guid = metadata->priv->creator;
	}

	/* TODO, sooner or later we will need metadata->owner check */

	/* No creator for this object. Might be very old in db or not set at all.
	 * Refuse, we do not know this. */
	if (!creator_guid) {
		MIDGARD_ERRNO_SET_STRING (mgd, MGD_ERR_ACCESS_DENIED, "Object's metadata holds null value instead of creator guid");
		return TRUE;
	}
	
	/* Finally, user which is logged in is the same person which created 
	 * object in question. Allow. */
	if (g_str_equal (person_guid, creator_guid))
		return FALSE;

	MIDGARD_ERRNO_SET (mgd, MGD_ERR_ACCESS_DENIED);
	return TRUE;
}
