/* 
 * Copyright (C) 2005 Piotr Pokora <piotr.pokora@infoglob.pl>
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

#include "midgard_type.h"
#include <string.h>
#include <glib.h>
#include <gobject/gvaluecollector.h>

/* MIDGARD_TYPE_LONGTEXT */

/* Copied from glib's gvaluetypes */
static void _init_string (GValue *value)
{
        value->data[0].v_pointer = NULL;
}

static void _free_string (GValue *value)
{
        if (!(value->data[1].v_uint & G_VALUE_NOCOPY_CONTENTS))
                g_free (value->data[0].v_pointer);
}

static void _copy_string (const GValue *src_value, GValue *dest_value)
{
        dest_value->data[0].v_pointer = g_strdup (src_value->data[0].v_pointer);
}

static gpointer _peek_pointer0 (const GValue *value)
{
        return value->data[0].v_pointer;
}

static gchar* _collect_string (GValue    *value,
                guint        n_collect_values,
                GTypeCValue *collect_values,
                guint        collect_flags)
{
          if (!collect_values[0].v_pointer)
                  value->data[0].v_pointer = NULL;
          else if (collect_flags & G_VALUE_NOCOPY_CONTENTS){
                  value->data[0].v_pointer = collect_values[0].v_pointer;
                  value->data[1].v_uint = G_VALUE_NOCOPY_CONTENTS;
          }
          else  
                  value->data[0].v_pointer = g_strdup (collect_values[0].v_pointer);
          return NULL;
}

static gchar* _lcopy_string (const GValue *value,
                guint         n_collect_values,
                GTypeCValue  *collect_values,
                guint         collect_flags)
{
        gchar **string_p = collect_values[0].v_pointer;
        
        if (!string_p)
                return g_strdup_printf ("value location for `%s' passed as NULL", G_VALUE_TYPE_NAME (value));
        if (!value->data[0].v_pointer)
                *string_p = NULL;
        else if (collect_flags & G_VALUE_NOCOPY_CONTENTS)
                *string_p = value->data[0].v_pointer;
        else
                *string_p = g_strdup (value->data[0].v_pointer);
        
        return NULL;
}

/* Register midgard_longtext fundamental type */
GType midgard_longtext_get_type(void) {
        static GType type = 0;
        if (type == 0) {
                static const GTypeValueTable value_table = {
                        _init_string,  /* value_init */
                        _free_string,  /* value_free */
                        _copy_string,  /* value_copy */
                        _peek_pointer0,  /* value_peek_pointer */
                        "p",      /* collect_format */
                        _collect_string, /* collect_value */
                        "p",      /* lcopy_format */
                        _lcopy_string, /* lcopy_value */
                };
                static const GTypeInfo info = {
                        0,                         /* class_size */
                        NULL,                      /* base_init */
                        NULL,                      /* base_destroy */
                        NULL,                      /* class_init */ 
                        NULL,                      /* class_destroy */
                        NULL,                      /* class_data */
                        0,                         /* instance_size */
                        0,                         /* n_preallocs */
                        NULL,                      /* instance_init */
                        &value_table               /* value_table */
                };
                static const GTypeFundamentalInfo finfo = {
                        G_TYPE_FLAG_DERIVABLE
                };
                type = g_type_register_fundamental(
                                g_type_fundamental_next(), "MidgardLongtext",
                                &info, &finfo, 0);
        }
        return type;
}                                       

/* MIDGARD_TYPE_PARAM_LONGTEXT */
struct _MidgardParamSpecLongtext {
	GParamSpec    parent_instance;
	gchar        *default_value;
	gchar        *cset_first;
	gchar        *cset_nth;
	gchar         substitutor;
	guint         null_fold_if_empty : 1;
	guint         ensure_non_null : 1;
};

static void _param_string_init (GParamSpec *pspec)
{
	MidgardParamSpecLongtext *ltspec = MGD_PARAM_SPEC_LONGTEXT (pspec);
	
	ltspec->default_value = NULL;
	ltspec->cset_first = NULL;
	ltspec->cset_nth = NULL;
	ltspec->substitutor = '_'; 
	ltspec->null_fold_if_empty = FALSE;
	ltspec->ensure_non_null = FALSE;
}

static void _param_string_finalize (GParamSpec *pspec)	
{
	MidgardParamSpecLongtext *ltspec = MGD_PARAM_SPEC_LONGTEXT (pspec);
	GParamSpecClass *parent_class = g_type_class_peek (g_type_parent (MGD_TYPE_PARAM_LONGTEXT));

	g_free (ltspec->default_value);
	g_free (ltspec->cset_first);
	g_free (ltspec->cset_nth);
	ltspec->default_value = NULL;
	ltspec->cset_first = NULL;
	ltspec->cset_nth = NULL;
	
	parent_class->finalize (pspec);
}   
  
static void _param_string_set_default (GParamSpec *pspec, GValue *value)
{
	value->data[0].v_pointer = g_strdup (MGD_PARAM_SPEC_LONGTEXT (pspec)->default_value);
}

static gboolean _param_string_validate (GParamSpec *pspec, GValue *value)
{
	MidgardParamSpecLongtext *ltspec = MGD_PARAM_SPEC_LONGTEXT (pspec);
	gchar *string = value->data[0].v_pointer;
	guint changed = 0;

	if (string && string[0]){
		
		gchar *s;

		if (ltspec->cset_first && !strchr (ltspec->cset_first, string[0])){
			
			string[0] = ltspec->substitutor;
			changed++;
		}
		
		if (ltspec->cset_nth)
			for (s = string + 1; *s; s++)
				if (!strchr (ltspec->cset_nth, *s))
				{
					*s = ltspec->substitutor;
					changed++;
				}
	}

	if (ltspec->null_fold_if_empty && string && string[0] == 0){
		
		g_free (value->data[0].v_pointer);
		value->data[0].v_pointer = NULL;
		changed++;
		string = value->data[0].v_pointer;
	}
	
	if (ltspec->ensure_non_null && !string){
		
		value->data[0].v_pointer = g_strdup ("");
		changed++;
		// string = value->data[0].v_pointer;
	}
	
	return changed;
}


static gint _param_string_values_cmp (GParamSpec *pspec, const GValue *value1, const GValue *value2)
{
	if (!value1->data[0].v_pointer)
		return value2->data[0].v_pointer != NULL ? -1 : 0;
	else if (!value2->data[0].v_pointer)
		return value1->data[0].v_pointer != NULL;
	else
		return strcmp (value1->data[0].v_pointer, value2->data[0].v_pointer);
}


/* Register midgard_longtext paramspec */
GType midgard_param_longtext_get_type(void) 
{
	static GType type = 0;
	if (type == 0) {
		{
			const GParamSpecTypeInfo pspec_info = {
				sizeof (MidgardParamSpecLongtext),     /* instance_size */
				16,                                /* n_preallocs */
				_param_string_init,     	   /* instance_init */
				MGD_TYPE_LONGTEXT,                 /* value_type */
				_param_string_finalize,            /* finalize */
				_param_string_set_default,         /* value_set_default */
				_param_string_validate,            /* value_validate */
				_param_string_values_cmp,          /* values_cmp */
			};
			type = g_param_type_register_static ("MidgardParamLongtext", &pspec_info);
		}
	}
	return type;	
}

/* Register MGD_TYPE_GUID type */
GType midgard_guid_get_type(void)
{
	static GType type = 0;
	if(type == 0) {
		
		GTypeInfo info = {
			0,	       /* class size */
			NULL,          /* base_init */
			NULL,          /* base_finalize */
			NULL,          /* class_init */
			NULL,          /* class_finalize */
			NULL,          /* class_data */
			0,
			0,             /* n_preallocs */
			NULL 	       /* instance_init */
		};
		type = g_type_register_static(G_TYPE_STRING, "MidgardGuid", &info, 0);
	}
	return type;
}
