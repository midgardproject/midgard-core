/*
 * Copyright (c) 2005 Jukka Zitting <jz@yukatan.fi>
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

#include "midgard_timestamp.h"
#include <glib.h>
#include "midgard_type.h"
#include <libgda/gda-value.h>

#define leapsecs_add(a,b) (a)

static gchar *caltime_fmt(MidgardTimestamp *mt) 
{
        GString *string = g_string_sized_new(100);
        g_string_printf(
                string, "%04ld-%02d-%02d %02d:%02d:%02d",
                mt->year, mt->month, mt->day, mt->hour, mt->minute, mt->second);
        if (mt->nano > 0) {
                unsigned long n = mt->nano;
                while ((n % 10) == 0) { n = n / 10; }
                g_string_append_printf(string, ".%lo", n);
        }
        g_string_append_printf(string, "%+05ld", mt->offset);
        return g_string_free(string, FALSE);
}

static unsigned int caltime_scan(const char *s, MidgardTimestamp *ct) 
{
	int sign = 1;
	const char *t = s;
	unsigned long z;
	unsigned long c;
	
	if (*t == '-') { ++t; sign = -1; }
	z = 0; while ((c = (unsigned char) (*t - '0')) <= 9) { z = z * 10 + c; ++t; }
	
	ct->year = z * sign;
	/* Avoid 0000 year*/
	if (ct->year == 0)
		ct->year = 1;

	if (*t++ != '-') return 0;
	z = 0; while ((c = (unsigned char) (*t - '0')) <= 9) { z = z * 10 + c; ++t; }
	ct->month = z;
	/* Avoid 00 month */
	if (ct->month == 0)
		ct->month = 1;

	if (*t++ != '-') return 0;
	z = 0; while ((c = (unsigned char) (*t - '0')) <= 9) { z = z * 10 + c; ++t; }
	ct->day = z;
	/* Avoid 00 day */
	if (ct->day == 0)
		ct->day = 1;

	while ((*t == ' ') || (*t == '\t') || (*t == 'T')) ++t;
	z = 0; while ((c = (unsigned char) (*t - '0')) <= 9) { z = z * 10 + c; ++t; }
	ct->hour = z;

	if (*t++ != ':') return 0;
	z = 0; while ((c = (unsigned char) (*t - '0')) <= 9) { z = z * 10 + c; ++t; }
	ct->minute = z;

	if (*t != ':')
		ct->second = 0;
	else {
		++t;
		z = 0; while ((c = (unsigned char) (*t - '0')) <= 9) { z = z * 10 + c; ++t; }
		ct->second = z;
	}

	if (*t != '.')
		ct->nano = 0;
	else {
		++t;
		ct->nano = 1000000000;
		z = 0; while ((c = (unsigned char) (*t - '0')) <= 9) { if (ct->nano > 1) { z = z * 10 + c; ++t; ct->nano /= 10; } }
		ct->nano *= z;
	}

	while ((*t == ' ') || (*t == '\t')) ++t;
	
	if(*t == '\0' || *t == 'Z') {
		ct->offset = z * sign;
		return t - s;
	}

	if (*t == '+') sign = 1; else if (*t == '-') sign = -1; else return 0;
	++t;
	c = (unsigned char) (*t++ - '0'); if (c > 9) return 0; z = c;
	c = (unsigned char) (*t++ - '0'); if (c > 9) return 0; z = z * 10 + c;
	c = (unsigned char) (*t++ - '0'); if (c > 9) return 0; z = z * 6 + c;
	c = (unsigned char) (*t++ - '0'); if (c > 9) return 0; z = z * 10 + c;
	ct->offset = z * sign;
	return t - s;
}


static void __timestamp_reset(const GValue *value) 
{
	MidgardTimestamp *ct = (MidgardTimestamp *) g_value_get_boxed(value);
	
	guint64 u = ct->date_value + 58486;
	long s = u % 86400ULL;
	long day = u / 86400ULL - 53375995543064ULL;
	long year = day / 146097L;
	long month;
	//int yday; 

	ct->second = s % 60; s /= 60;
	ct->minute = s % 60; s /= 60;
	ct->hour = s;

	day = (day % 146097L) + 678881L;
	while (day >= 146097L) { day -= 146097L; ++year; }

	// year * 146097 + day - 678881 is MJD; 0 <= day < 146097 
	// 2000-03-01, MJD 51604, is year 5, day 0 

	year *= 4;
	if (day == 146096L) { year += 3; day = 36524L; }
	else { year += day / 36524L; day %= 36524L; }
	year = year * 25 + day / 1461;
	day %= 1461;
	year *= 4;

	if (day == 1460) { year += 3; day = 365; }
	else { year += day / 365; day %= 365; }

	day *= 10;
	month = (day + 5) / 306;
	day = (day + 5) % 306;
	day /= 10;
	
	if (month >= 10) { ++year; month -= 10; }
	else { month += 2; }

	ct->year = year;
	ct->month = month + 1;
	ct->day = day + 1;
	
	ct->offset = 0;	
}

static void midgard_timestamp_set(MidgardTimestamp *ct) 
{
	static const unsigned long times365[4] = { 0, 365, 730, 1095 };
	static const unsigned long times36524[4] = { 0, 36524UL, 73048UL, 109572UL };
	static const unsigned long montab[12] =  { 0, 31, 61, 92, 122, 153, 184, 214, 245, 275, 306, 337 };
	/* month length after february is (306 * m + 5) / 10 */

	long y = ct->year;
	long m = ct->month - 1;
	long d = ct->day - 678882L;
	long s = ((ct->hour * 60 + ct->minute) - ct->offset) * 60 + ct->second;

	d += 146097L * (y / 400);
	y %= 400;

	if (m >= 2) m -= 2; else { m += 10; --y; }

	y += (m / 12);
	m %= 12;
	if (m < 0) { m += 12; --y; }

	d += montab[m];

	d += 146097L * (y / 400);
	y %= 400;
	if (y < 0) { y += 400; d -= 146097L; }

	d += times365[y & 3];
	y >>= 2;

	d += 1461L * (y % 25);
	y /= 25;

	d += times36524[y & 3];

	ct->date_value = d * 86400ULL + 4611686014920671114ULL + (long long) s;
	/* TODO: leapsecs */
}

/*
static void midgard_timestamp_set_value(GValue *value)
{
	MidgardTimestamp *mt = (MidgardTimestamp *)g_value_get_boxed(value);
	midgard_timestamp_set(mt);
}
*/

MidgardTimestamp *midgard_timestamp_new()
{
	MidgardTimestamp *mt = g_new0(MidgardTimestamp, 1);

	mt->year  = 1;
	mt->month = 1;
	mt->day = 1;
	mt->hour = 0;
	mt->minute = 0;
	mt->second = 0;
	mt->nano = 0;
	mt->offset = 0;
	mt->date_value = 4611686014920671114ULL;
	mt->date_string = NULL;

	midgard_timestamp_set(mt);	

	return mt;
}

static void midgard_timestamp_free(MidgardTimestamp *mt)
{
	if (mt == NULL)
		return;

	if (mt->date_string != NULL)
		g_free(mt->date_string);

	g_free(mt);

	mt = NULL;
}

/* Internal initialization function for the midgard_timestamp value type. */
static void value_init_timestamp(GValue *value) 
{
        g_assert(G_VALUE_HOLDS(value, MIDGARD_TYPE_TIMESTAMP));

	MidgardTimestamp *mt = midgard_timestamp_new();
	g_value_take_boxed(value, mt);
}

/* Internal copy function for the midgard_timestamp value type. */
static void value_copy_timestamp(const GValue *src, GValue *dst) 
{
        g_assert(G_VALUE_HOLDS(src, MIDGARD_TYPE_TIMESTAMP));
        g_assert(G_VALUE_HOLDS(dst, MIDGARD_TYPE_TIMESTAMP));

	MidgardTimestamp *mt = midgard_timestamp_new();	

	MidgardTimestamp *mtsrc = (MidgardTimestamp *) g_value_get_boxed(src);

	if(mtsrc == NULL)
		mtsrc = midgard_timestamp_new();

	mt->year = mtsrc->year;
	mt->month = mtsrc->month;
	mt->day = mtsrc->day;

	mt->hour = mtsrc->hour;
	mt->minute = mtsrc->minute;
	mt->second = mtsrc->second;

	mt->offset = mtsrc->offset;
	mt->nano = mtsrc->nano;

	mt->date_value = mtsrc->date_value;

	if (mtsrc->date_string != NULL)
		mt->date_string = g_strdup(mtsrc->date_string);

	g_value_take_boxed(dst, mt);
}

static void midgard_timestamp_value_free(GValue *value)
{
	MidgardTimestamp *mt = (MidgardTimestamp *) g_value_get_boxed(value);
	midgard_timestamp_free(mt);	

	return;
}

/* Internal collect function for the midgard_timestamp value type. */
static gchar *value_collect_timestamp(
                GValue *value, guint n_collect_values,
                GTypeCValue *collect_values, guint collect_flags) 
{
        g_assert(G_VALUE_HOLDS(value, MIDGARD_TYPE_TIMESTAMP));
        g_assert(n_collect_values == 1);
        g_assert(collect_values != NULL);

        const gchar *time = (const gchar *) collect_values;
        if (time == NULL) {
                return g_strdup("Midgard timestamp value string passed as NULL");
        }

        return NULL;
}

/* Internal lcopy function for the midgard_timestamp value type. */
static gchar *value_lcopy_timestamp(
                const GValue *value, guint n_collect_values,
                GTypeCValue *collect_values, guint collect_flags) 
{
        g_assert(G_VALUE_HOLDS(value, MIDGARD_TYPE_TIMESTAMP));
        g_assert(n_collect_values == 1);
        g_assert(collect_values != NULL);

        const gchar **time = (const gchar **) collect_values;
        if (time == NULL) {
                return g_strdup("Midgard timestamp value string passed as NULL");
        }

        return NULL;
}

static void midgard_timestamp_transform_to_string(const GValue *src, GValue *dst)
{
	g_return_if_fail(G_VALUE_HOLDS_STRING(dst) &&
			G_VALUE_HOLDS(src, MGD_TYPE_TIMESTAMP));

	MidgardTimestamp *mt = (MidgardTimestamp *) g_value_get_boxed(src);	
	gchar *str_val = (gchar *)caltime_fmt(mt);

	g_value_take_string(dst, str_val);
}

static void midgard_timestamp_transform_from_string(const GValue *src, GValue *dst) 
{
	g_return_if_fail(G_VALUE_HOLDS_STRING(src) &&
			G_VALUE_HOLDS(dst, MGD_TYPE_TIMESTAMP));

	const gchar *time = g_value_get_string(src);
	g_return_if_fail(time != NULL);

        MidgardTimestamp *mt = (MidgardTimestamp *) g_value_get_boxed(dst);

	if(mt == NULL) {
		
		mt = midgard_timestamp_new();
		g_value_take_boxed(dst, mt);
	}	

	/* GDA seems to convert datetime better. Follow its value. */

	GValue gt = {0, };
	g_value_init(&gt, GDA_TYPE_TIMESTAMP);
	g_value_transform(src, &gt);

	g_value_transform(&gt, dst);
	g_value_unset(&gt);

        /*if (caltime_scan(time, mt) > 0) {

		g_warning("set");
                midgard_timestamp_set(mt);

        } else {

		g_warning("reset");
        	__timestamp_reset(dst);
        }*/
}

static void midgard_timestamp_transform_from_gda_timestamp(const GValue *src, GValue *dst)
{
	g_return_if_fail(G_VALUE_HOLDS(src, GDA_TYPE_TIMESTAMP) &&
			G_VALUE_HOLDS(dst, MGD_TYPE_TIMESTAMP));

	MidgardTimestamp *mt = midgard_timestamp_new();	

	GdaTimestamp *gt = (GdaTimestamp *) g_value_get_boxed(src);

	mt->year = gt->year;
	/* Avoid 0000 year */
	if (mt->year == 0)
		mt->year = 1;

	mt->month = gt->month;
	/* Avoid 00 month */
	if (mt->month == 0)
		mt->month = 1;

	mt->day = gt->day;
	/* Avoid 00 day */
	if (mt->day == 0)
		mt->day = 1;

	mt->hour = gt->hour;
	mt->minute = gt->minute;
	mt->second = gt->second;

	mt->offset = gt->timezone;

	g_value_take_boxed(dst, mt);	
}

static void midgard_timestamp_transform_to_gda_timestamp(const GValue *src, GValue *dst)
{
	g_return_if_fail(G_VALUE_HOLDS(src, MGD_TYPE_TIMESTAMP) &&
			G_VALUE_HOLDS(dst, GDA_TYPE_TIMESTAMP));

	GdaTimestamp *gt = g_new0(GdaTimestamp, 1);	
	MidgardTimestamp *mt = (MidgardTimestamp *) g_value_get_boxed(src);
	/* __timestamp_reset(src); */

	gt->year = mt->year;
	gt->month = mt->month;
	gt->day = mt->day;

	gt->hour = mt->hour;
	gt->minute = mt->minute;
	gt->second = mt->second;

	gt->timezone = mt->offset;

	gda_value_set_timestamp(dst, (const GdaTimestamp *)gt);

	g_free (gt);
}

GType midgard_timestamp_get_type(void) 
{
        static GType type = 0;

        if (type == 0) {

                static const GTypeValueTable value_table = {
                        &value_init_timestamp,     /* value_init */
                        &midgard_timestamp_value_free,	/* value_free */
                        &value_copy_timestamp,     /* value_copy */
                        NULL,                      /* value_peek_pointer */
                        "p",                       /* collect_format */
                        &value_collect_timestamp,  /* collect_value */
                        "p",                       /* lcopy_format */
                        &value_lcopy_timestamp     /* lcopy_value */
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

		type = g_type_register_static (G_TYPE_BOXED, "MidgardTimestamp", &info, 0);

		/* Register transform functions */
		g_value_register_transform_func(type, G_TYPE_STRING, midgard_timestamp_transform_to_string);
		g_value_register_transform_func(G_TYPE_STRING, type, midgard_timestamp_transform_from_string);
		g_value_register_transform_func(GDA_TYPE_TIMESTAMP, type, midgard_timestamp_transform_from_gda_timestamp);
		g_value_register_transform_func(type, GDA_TYPE_TIMESTAMP, midgard_timestamp_transform_to_gda_timestamp);
        }

        return type;
}

void midgard_timestamp_set_current_time(const GValue *value)
{
	g_assert(value != NULL);
	g_return_if_fail(G_VALUE_HOLDS(value, MGD_TYPE_TIMESTAMP));

	MidgardTimestamp *mt = (MidgardTimestamp *) g_value_get_boxed(value);
	time_t utctime = time(NULL);
	mt->date_value = utctime + 4611686018427387914ULL;	
	mt->nano = 0;
}

GValue *midgard_timestamp_new_current()
{
	GValue *tval = g_new0(GValue, 1);
	g_value_init(tval, MGD_TYPE_TIMESTAMP);
	midgard_timestamp_set_current_time(tval);
	__timestamp_reset(tval);

	return tval;
}

gchar *midgard_timestamp_get_string(const GValue *value)
{
	g_assert(value != NULL);
	g_return_val_if_fail(G_VALUE_HOLDS(value, MGD_TYPE_TIMESTAMP), NULL);

	MidgardTimestamp *mt = (MidgardTimestamp *) g_value_get_boxed(value);

	return caltime_fmt(mt);
}

GValue *midgard_timestamp_new_value_from_iso8601(const gchar *iso_date)
{
	GValue *tval = g_new0(GValue, 1);
	g_value_init(tval, MGD_TYPE_TIMESTAMP);

	MidgardTimestamp *mt = (MidgardTimestamp *) g_value_get_boxed(tval);

	if (caltime_scan(iso_date, mt) > 0) {
		
		midgard_timestamp_set(mt);
	
	} else {
		
		__timestamp_reset(tval);
	}

	return tval;
}

MidgardTimestamp *midgard_timestamp_new_from_iso8601(const gchar *iso_date)
{
	MidgardTimestamp *mt = midgard_timestamp_new();

	caltime_scan(iso_date, mt);

	return mt;
}

gchar *midgard_timestamp_dup_string(const GValue *value) 
{
        g_assert(G_VALUE_HOLDS(value, MIDGARD_TYPE_TIMESTAMP));

	MidgardTimestamp *mt = (MidgardTimestamp *) g_value_get_boxed(value);
        return (gchar *)caltime_fmt(mt); 
}

time_t midgard_timestamp_get_time(const GValue *value) 
{
        g_assert(G_VALUE_HOLDS(value, MIDGARD_TYPE_TIMESTAMP));

        if (value->data) {
	       	
		if (value->data[1].v_ulong >= 1000000000 / 2) 
	    		return value->data[0].v_uint64 - 4611686018427387914ULL + 1;
	       	else 
	    		return value->data[0].v_uint64 - 4611686018427387914ULL;
	}
      
	return (time_t)-1;
}

void midgard_timestamp_set_time(GValue *value, time_t time) 
{
        g_assert(G_VALUE_HOLDS(value, MIDGARD_TYPE_TIMESTAMP));

	if (value->data) {
		
		if (time)
			value->data[0].v_uint64 = time + 4611686018427387914ULL;
	 
		value->data[1].v_ulong = 0;
	}
}

GValue __get_current_timestamp_value()
{
	GValue tval = {0, };
	time_t utctime = time(NULL);
	g_value_init(&tval, MIDGARD_TYPE_TIMESTAMP);
	midgard_timestamp_set_time(&tval, utctime);

	return tval;
}

gchar *midgard_timestamp_current_as_char()
{
	GValue tval = __get_current_timestamp_value();
	gchar *dtstr = midgard_timestamp_dup_string(&tval);
	g_value_unset(&tval);

	return dtstr;
}

GValue midgard_timestamp_current_as_value(GType type)
{
	GValue tval = __get_current_timestamp_value();

	if(type == G_TYPE_STRING) {
		
		GValue sval = {0, };
		g_value_init(&sval, G_TYPE_STRING);
		g_value_take_string(&sval,  midgard_timestamp_dup_string(&tval));
		g_value_unset(&tval);
		
		return sval;
	}

	return tval;
}
