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

#ifndef _MIDGARD_CORE_XML_H
#define _MIDGARD_CORE_XML_H

#include <libxml/xmlreader.h>
#include "midgard_core_xml.h"

xmlNode*
midgard_core_xml_get_node_by_name (xmlNode *node, const gchar *name)
{
	g_assert(node != NULL);
	g_assert(name != NULL);

	xmlNode *cur = NULL;

	for (cur = node; cur; cur = cur->next) {

		if (cur->type == XML_ELEMENT_NODE) {

		       g_warning("EL: %s ?= %s", cur->name, name);	
			if(g_str_equal(cur->name, name))
				return cur;
		}
	}

	return NULL;
}

gchar* 
midgard_core_xml_get_attribute_content (xmlNode *node)
{
	g_return_val_if_fail(node != NULL, NULL);

   	gchar *content = NULL;
	content = (gchar *)xmlNodeGetContent(node);

	if(content == NULL)
		return NULL;

	xmlParserCtxtPtr parser = xmlNewParserCtxt();
	xmlChar *decoded =
		xmlStringDecodeEntities(parser,
				(const xmlChar *) content,
				XML_SUBSTITUTE_REF,
				0, 0, 0);
	xmlFreeParserCtxt(parser);
	g_free(content);
	return (gchar*)decoded;
}

gchar*
midgard_core_xml_get_child_attribute_content_by_name (xmlNode *node, const gchar *nodename, const gchar *attr)
{
	g_return_val_if_fail(node != NULL, NULL);
	g_return_val_if_fail(nodename != NULL, NULL);
	g_return_val_if_fail(attr != NULL, NULL);

	xmlNode *cur;

	for (cur = node->children; cur; cur = cur->next) {
	
		if (cur->type == XML_ELEMENT_NODE
				&& g_str_equal(cur->name, nodename)) {

			return (gchar *)xmlGetProp(cur, (const xmlChar *)attr);
		}
	}

	return NULL;
}

gboolean midgard_core_xml_attribute_is_allowed(const char **allowed, const xmlChar *str) 
{
	g_assert(allowed != NULL);
	g_assert(str != NULL);
	
	while (*allowed != NULL) {

		if (g_str_equal(*allowed++, str)) {
			return TRUE;
		}
	}

	return FALSE;
}

#endif /* _MIDGARD_CORE_XML_H */
