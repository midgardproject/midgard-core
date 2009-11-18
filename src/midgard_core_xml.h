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

#ifndef _CORE_XML_H
#define _CORE_XML_H

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlreader.h>
#include <libxml/parserInternals.h>
#include <glib.h>

xmlNode		*midgard_core_xml_get_node_by_name			(xmlNode *node, const gchar *name);
gchar 		*midgard_core_xml_get_attribute_content			(xmlNode *node); 
gchar		*midgard_core_xml_get_attribute_content_by_name 	(xmlNode *node, const gchar *name);
gboolean	midgard_core_xml_attribute_is_allowed			(const char **allowed, const xmlChar *str);
gchar*		midgard_core_xml_get_child_attribute_content_by_name 	(xmlNode *node, const gchar *nodename, const gchar *attr);

#endif /* _PRIVATE_SCHEMA_H */
