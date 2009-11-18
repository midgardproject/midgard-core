/* $Id$
 *
 * midgard-lib: database access for Midgard clients
 *
 * Copyright (C) 1999 Jukka Zitting <jukka.zitting@iki.fi>
 * Copyright (C) 2000 The Midgard Project ry
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#include <config.h>
#include "midgard/midgard_legacy.h"

#ifdef WIN32
#include "win32/win95nt.h"
#endif

midgard_pool *mgd_pool(midgard * mgd)
{
	assert(mgd);
	return mgd->pool;
}

midgard_pool *mgd_res_pool(midgard_res * res)
{
	assert(res);
	return res->pool;
}

typedef char *charp;
typedef int *intp;
typedef void *voidp;

char *mgd_format_ext(mgd_parser * parser, midgard_pool * pool, const char *fmt,
		     ...)
{
	char *str;
	va_list args;
	va_start(args, fmt);
	str = mgd_vformat_ext(parser, pool, fmt, args);
	va_end(args);
	return str;
}

char *mgd_format(midgard * mgd, midgard_pool * pool, const char *fmt, ...)
{
	char *str;
	va_list args;
	va_start(args, fmt);
	str = mgd_vformat(mgd, pool, fmt, args);
	va_end(args);
	return str;
}

static char mgd_parser_EntTable[][7] = {
	"nbsp", "iexcl", "cent", "pound", "curren", "yen", "brvbar",
	"sect", "uml", "copy", "ordf", "laquo", "not", "shy", "reg",
	"macr", "deg", "plusmn", "sup2", "sup3", "acute", "micro",
	"para", "middot", "cedil", "sup1", "ordm", "raquo", "frac14",
	"frac12", "frac34", "iquest", "Agrave", "Aacute", "Acirc",
	"Atilde", "Auml", "Aring", "AElig", "Ccedil", "Egrave",
	"Eacute", "Ecirc", "Euml", "Igrave", "Iacute", "Icirc",
	"Iuml", "ETH", "Ntilde", "Ograve", "Oacute", "Ocirc", "Otilde",
	"Ouml", "times", "Oslash", "Ugrave", "Uacute", "Ucirc", "Uuml",
	"Yacute", "THORN", "szlig", "agrave", "aacute", "acirc",
	"atilde", "auml", "aring", "aelig", "ccedil", "egrave",
	"eacute", "ecirc", "euml", "igrave", "iacute", "icirc",
	"iuml", "eth", "ntilde", "ograve", "oacute", "ocirc", "otilde",
	"ouml", "divide", "oslash", "ugrave", "uacute", "ucirc",
	"uuml", "yacute", "thorn", "yuml"
};

char mgd_parser_HexTable[16] = "0123456789ABCDEF";

static char *fmtbuf = NULL;
static int fmtlen = 0, fmtsiz = 0;

static char *accbuf = NULL;
static int acclen = 0, accsiz = 0, acccol = 0;

int mgd_parser_addchar(mgd_parser * parser, char ch)
{
	char *fmttmp;
	if (fmtlen == fmtsiz) {
		fmttmp = realloc(fmtbuf, fmtsiz + 1024);
		if (!fmttmp)
			return 0;
		fmtbuf = fmttmp;
		fmtsiz += 1024;
	}
	fmtbuf[fmtlen++] = ch;
	return 1;
}

int mgd_parser_accchar(mgd_parser * parser, char ch)
{
	char *acctmp;
	if (acclen == accsiz) {

		acctmp = realloc(accbuf, accsiz + 1024);
		if (!acctmp)
			return 0;
		accbuf = acctmp;
		accsiz += 1024;
	}
	accbuf[acclen++] = ch;
	if (ch == '\n')
		acccol = 0;
	else
		acccol++;
	return 1;
}

int mgd_parser_addint(mgd_parser * parser, int num)
{
	int begin, end;
	char tmp;
	if (num < 0) {
		num = -num;
		if (!mgd_parser_addchar(parser, '-'))
			return 0;
	}
	begin = fmtlen;
	do {
		if (!mgd_parser_addchar(parser, '0' + num % 10))
			return 0;
		num /= 10;
	} while (num);
	for (end = fmtlen - 1; begin < end; begin++, end--) {
		tmp = fmtbuf[begin];
		fmtbuf[begin] = fmtbuf[end];
		fmtbuf[end] = tmp;
	}
	return 1;
}

int mgd_parser_adddate(mgd_parser * parser, const char *str)
{
	int d, m, y;
	if (!str || !*str || sscanf(str, "%d.%d.%d", &d, &m, &y) != 3) {
		mgd_parser_addchar(parser, '\'');
      /*[eeh] Add string verbatim if it can't be parsed */
      mgd_parser_addstr(parser, str);
		mgd_parser_addchar(parser, '\'');
		return 0;
	}
	if (y < 100)
		y += 1900;
	mgd_parser_addchar(parser, '\'');
	mgd_parser_addint(parser, y);
	mgd_parser_addchar(parser, '-');
	mgd_parser_addint(parser, m);
	mgd_parser_addchar(parser, '-');
	mgd_parser_addint(parser, d);
	mgd_parser_addchar(parser, '\'');
	return 1;
}

int mgd_parser_addstr(mgd_parser * parser, const char *str)
{
	if (str)
		while (*str)
			if (!mgd_parser_addchar(parser, *str++))
				return 0;
	return 1;
}

int mgd_parser_addsql(mgd_parser * parser, const char *str)
{
	if (str)
		while (*str)
			switch (*str) {
				case '\'':
				if (!mgd_parser_addstr(parser, "\\\'"))
					return 0;
				str++;
				break;
				case '\"':
				if (!mgd_parser_addstr(parser, "\\\""))
					return 0;
				str++;
				break;
				case '\\':
				if (!mgd_parser_addstr(parser, "\\\\"))
					return 0;
				str++;
				break;
				default:
				if (!mgd_parser_addchar(parser, *str))
					return 0;
				str++;
				break;
			}
	return 1;
}

/* This function is different from mgd_parser_addsql in that it
   tries to eat all '\n'. This is needed for 'Q' formatter
*/

int mgd_parser_addcleansql(mgd_parser * parser, const char *str)
{
	if (str)
		while (*str)
			switch (*str) {
				case '\'':
				if (!mgd_parser_addstr(parser, "\\\'"))
					return 0;
				str++;
				break;
				case '\"':
				if (!mgd_parser_addstr(parser, "\\\""))
					return 0;
				str++;
				break;
				case '\\':
				if (!mgd_parser_addstr(parser, "\\\\"))
					return 0;
				str++;
				break;
				case '\n':
				/* Skip '\n' and do not put into output stream */
				str++;
				break;
				default:
				if (!mgd_parser_addchar(parser, *str))
					return 0;
				str++;
				break;
			}
	return 1;
}

static int addplain(mgd_parser * parser, const char *str, int html)
{
	if (!str)
		return 0;
	while (*str) {
		for (; *str && (!html || (*str != '[' || *(str + 1) != '<'));
		     str++)
			if (*str == '&')
				mgd_parser_addstr(parser, "&amp;");
			else if (*str == '<')
				mgd_parser_addstr(parser, "&lt;");
			else if (*str == '>')
				mgd_parser_addstr(parser, "&gt;");
			else if (*str == '\"')
				mgd_parser_addstr(parser, "&quot;");
			else if (((unsigned char) *str) >= 160) {
				mgd_parser_addchar(parser, '&');
				mgd_parser_addstr(parser,
						  mgd_parser_EntTable[
								      ((unsigned
									char)
								       *str) -
								      160]);
				mgd_parser_addchar(parser, ';');
			}
			else
				mgd_parser_addchar(parser, *str);
		if (*str && html && *str == '[' && *(str + 1) == '<') {
			for (str += 2;
			     *str && (*str != '>' || *(str + 1) != ']'); str++)
				if (((unsigned char) *str) >= 160) {
					mgd_parser_addchar(parser, '&');
					mgd_parser_addstr(parser,
							  mgd_parser_EntTable[
									      (
									       (unsigned
										char)
									       *str)
- 160]);
					mgd_parser_addchar(parser, ';');
				}
				else
					mgd_parser_addchar(parser, *str);
		}
		if (*str && html && *str == '>' && *(str + 1) == ']')
			str += 2;
	}
	return 1;
}

static int addhtml(mgd_parser * parser, const char *str)
{
	if (!str)
		return 0;
	for (; *str; str++)
		if (((unsigned char) *str) >= 160) {
			mgd_parser_addchar(parser, '&');
			mgd_parser_addstr(parser,
					  mgd_parser_EntTable[
							      ((unsigned char)
							       *str) - 160]);
			mgd_parser_addchar(parser, ';');
		}
		else
			mgd_parser_addchar(parser, *str);
	return 1;
}


static const char *skipws(const char *str)
{
	while (*str == ' ' || *str == '\t')
		str++;
	return str;
}

static int accline = 0, accword = 0, acclist = 0, accpara = 0;

static int crlf(const char *str)
{
	if (*str == '\n')
		return 1;
	if (*str == '\r' && *(str + 1) == '\n')
		return 2;
	return 0;
}

static const char *doline(mgd_parser * parser, const char *str)
{
	str = skipws(str);
	while (*str && !crlf(str)) {
		if (accword++)
			mgd_parser_accchar(parser, acccol > 70 ? '\n' : ' ');
		while (*str && *str != ' ' && *str != '\t' && !crlf(str)) {
			if (*str == '.')
				accpara = 1;
			mgd_parser_accchar(parser, *str++);
		}
		str = skipws(str);
	}
	str += crlf(str);
	str = skipws(str);
	return str;
}

int mgd_parser_addtext(mgd_parser * parser, const char *str, int headers)
{
	if (!str)
		return 0;
	acclist = 0;
	do {
		acclen = 0;
		accline = 0;
		accword = 0;
		acccol = 0;
		accpara = 0;
		for (str = skipws(str); crlf(str);) {
			str += crlf(str);
			str = skipws(str);
		}
		if (*str == '-') {
			if (!acclist++)
				mgd_parser_addstr(parser, "<ul>\n");
			str = skipws(str + 1);
		}
		do {
			accline++;
			str = doline(parser, str);
		} while (*str && *str != '-' && !crlf(str));
		mgd_parser_accchar(parser, '\0');
		if (acclist) {
			mgd_parser_addstr(parser, "  <li>");
			parser->callbacks['P'].func(parser, accbuf);
			mgd_parser_addstr(parser, "</li>\n");
		}
		else if (headers && !accpara && accline == 1 && accword > 0
			 && accword < 10) {
			mgd_parser_addstr(parser, "<h2>");
			parser->callbacks['P'].func(parser, accbuf);
			mgd_parser_addstr(parser, "</h2>\n");
		}
		else if (accword > 0) {
			mgd_parser_addstr(parser, "<p>");
			parser->callbacks['P'].func(parser, accbuf);
			mgd_parser_addstr(parser, "\n</p>\n");
		}
		if (acclist && *str != '-') {
			acclist = 0;
			mgd_parser_addstr(parser, "</ul>\n");
		}
	} while (*str);
	return 1;
}

static int mgd_parser_std_d(mgd_parser * parser, void *data)
{
	return mgd_parser_addint(parser, (int) data);
}

static int mgd_parser_std_D(mgd_parser * parser, void *data)
{
	int *ids = (int *) data;
	mgd_parser_addchar(parser, '(');
	if (ids && *ids) {
		mgd_parser_addint(parser, *ids);
		for (ids++; *ids; ids++) {
			mgd_parser_addchar(parser, ',');
			mgd_parser_addint(parser, *ids);
		}
	}
	else
		mgd_parser_addstr(parser, "-1");
	mgd_parser_addchar(parser, ')');
	return 1;
}

static int mgd_parser_std_t(mgd_parser * parser, void *data)
{
	return mgd_parser_adddate(parser, (const char *) data);
}

static int mgd_parser_std_s(mgd_parser * parser, void *data)
{
	return mgd_parser_addstr(parser, (const char *) data);
}

static int mgd_parser_std_q(mgd_parser * parser, void *data)
{
	mgd_parser_addchar(parser, '\'');
	mgd_parser_addsql(parser, (const char *) data);
	mgd_parser_addchar(parser, '\'');
	return 1;
}

static int mgd_parser_std_Q(mgd_parser * parser, void *data)
{
	mgd_parser_addcleansql(parser, (const char *) data);
	return 1;
}

static int mgd_parser_std_p(mgd_parser * parser, void *data)
{
	return addplain(parser, (const char *) data, 0);
}

static int mgd_parser_std_P(mgd_parser * parser, void *data)
{
	return addplain(parser, (const char *) data, 1);
}

static int mgd_parser_std_h(mgd_parser * parser, void *data)
{
	return addplain(parser, (const char *) data, 1);
}

static int mgd_parser_std_H(mgd_parser * parser, void *data)
{
	return addhtml(parser, (const char *) data);
}

static int mgd_parser_std_f(mgd_parser * parser, void *data)
{
	return mgd_parser_addtext(parser, (const char *) data, 0);
}

static int mgd_parser_std_F(mgd_parser * parser, void *data)
{
	return mgd_parser_addtext(parser, (const char *) data, 1);
}

static int mgd_parser_std_u(mgd_parser * parser, void *data)
{
	const char *str;
	for (str = (const char *) data; str && *str; str++)
		if (*str == '\n')
			mgd_parser_addstr(parser, "%0D%0A");
		else if (isalnum(*str))
			mgd_parser_addchar(parser, *str);
		else {
			mgd_parser_addchar(parser, '%');
			mgd_parser_addchar(parser,
					   mgd_parser_HexTable[
							       (((unsigned
								  char) *str) &
								0xF0) >> 4]);
			mgd_parser_addchar(parser,
					   mgd_parser_HexTable[((unsigned char)
								*str) & 0x0F]);
		}
	return 1;
}

static mgd_parser *parserdb = NULL;

mgd_parser *mgd_parser_create(const char *name, const char *encoding,
			      int need_qp)
{
	mgd_parser *parser, *fparser;
	int i;
	midgard_pool *pool;

	pool = mgd_alloc_pool();
	parser = (mgd_parser *) mgd_alloc(pool, sizeof (mgd_parser)); /* LEAK */
	parser->pool = pool;
	parser->next = NULL;
	parser->name = mgd_strdup(pool, name);
	parser->encoding = mgd_strdup(pool, encoding);
	parser->need_qp = need_qp;
	parser->callbacks = (mgd_parser_callback_info *) mgd_alloc(pool,
								   256 *
								   sizeof
								   (mgd_parser_callback_info));

	for (i = 0; i < 256; i++)
		parser->callbacks[i].func = NULL;

	mgd_parser_setcallback(parser, 'd', MGD_INT, mgd_parser_std_d);
	mgd_parser_setcallback(parser, 'D', MGD_PTR, mgd_parser_std_D);
	mgd_parser_setcallback(parser, 'i', MGD_INT, mgd_parser_std_d);
	mgd_parser_setcallback(parser, 't', MGD_STR, mgd_parser_std_t);
	mgd_parser_setcallback(parser, 's', MGD_STR, mgd_parser_std_s);
	mgd_parser_setcallback(parser, 'q', MGD_STR, mgd_parser_std_q);
	mgd_parser_setcallback(parser, 'Q', MGD_STR, mgd_parser_std_Q);
	mgd_parser_setcallback(parser, 'p', MGD_STR, mgd_parser_std_p);
	mgd_parser_setcallback(parser, 'P', MGD_STR, mgd_parser_std_P);
	mgd_parser_setcallback(parser, 'h', MGD_STR, mgd_parser_std_h);
	mgd_parser_setcallback(parser, 'H', MGD_STR, mgd_parser_std_H);
	mgd_parser_setcallback(parser, 'f', MGD_STR, mgd_parser_std_f);
	mgd_parser_setcallback(parser, 'F', MGD_STR, mgd_parser_std_F);
	mgd_parser_setcallback(parser, 'u', MGD_STR, mgd_parser_std_u);

	if (!parserdb) {
		parserdb = parser;
	}
	else {
		fparser = parserdb;
		while (fparser->next) {
			fparser = fparser->next;
		}
		fparser->next = parser;
	}	
	return parser;
}

void mgd_parser_free(mgd_parser * parser)
{
	mgd_parser *fparser;
	if (parser) {
		fparser = parserdb;
		while (fparser->next && fparser->next != parser) {
			fparser = fparser->next;
		}
		if (fparser->next == parser) {
			fparser->next = parser->next;
		}
		mgd_free_pool(parser->pool);
	}	
}

void mgd_parser_free_all()
{
	mgd_parser *parser = NULL, *fparser;

	if (!parserdb)
		return;
	fparser = parserdb;
	while (fparser->next) {
		parser = fparser;
		fparser = fparser->next;
		mgd_free_pool(parser->pool);
	}
	
	g_free(parser->name);
	g_free(parser->encoding);
	g_free(parser->callbacks);
	g_free(parser);
	
	parserdb = NULL;
}

void mgd_parser_setcallback(mgd_parser * parser, char symbol,
			    mgd_parser_type ptype, mgd_parser_callback callback)
{
	if (parser) {
		parser->callbacks[(int) symbol].func = callback;
		parser->callbacks[(int) symbol].ptype = ptype;
	}
}

void mgd_parser_clearcallback(mgd_parser * parser, char symbol)
{
	if (parser) {
		parser->callbacks[(int) symbol].func = NULL;
	}
}

mgd_parser *mgd_parser_get(const char *name)
{
	mgd_parser *fparser;
	fparser = parserdb;
	while (fparser) {
		if (!strcasecmp(name, fparser->name)) {
			return fparser;
		}
		fparser = fparser->next;
	}
	return NULL;
}

mgd_parser *mgd_parser_list()
{
	return parserdb;
}

int mgd_parser_activate(midgard * mgd, const char *name)
{
	mgd_parser *parser;
	if (mgd) {
		parser = mgd_parser_get(name);
		if (parser) {
			mgd->parser = parser;
		}
		else {
			if (!mgd->parser)
				mgd->parser = parserdb;
		}
		return (parser != NULL);
	}
	return 0;
}

char *mgd_vformat_ext(mgd_parser * parser, midgard_pool * pool, const char *fmt,
		      va_list args)
{
	char *str;
	assert(pool);
   assert(parser);
	if (!fmt)
		return NULL;

	fmtlen = 0;
	while (*fmt)
		if (*fmt == '$') {
			if (*(fmt + 1) == '$') {
				fmt++;	/* fall thru */
			}
			else {
				/* Force manual selection of system-vital parsers */
				switch (*(fmt + 1)) {
					case 'd':
					case 'i':
					mgd_parser_std_d(parser, (void *)
							 va_arg(args, int));
					fmt += 2;
					break;
					case 'q':
					mgd_parser_std_q(parser, (void *)
							 va_arg(args, charp));
					fmt += 2;
					break;
					case 'D':
					mgd_parser_std_D(parser, (void *)
							 va_arg(args, intp));
					fmt += 2;
					break;
					default:
					if (parser->callbacks
					    [(int) (*(fmt + 1))].func)
						switch (parser->callbacks[(int)
									  
									  (*
									   (fmt
									    +
									    1))].
							ptype) {
							case
							MGD_CHAR:
							parser->callbacks[(int)
									  
									  (*
									   (fmt
									    +
									    1))].
							   func(parser,
								(void *) ((int)
									  va_arg
									  (args,
									   int)));
							fmt += 2;
							break;
							case
							MGD_INT:
							parser->callbacks[(int)
									  
									  (*
									   (fmt
									    +
									    1))].
							   func(parser, (void *)
								va_arg(args,
								       int));
							fmt += 2;
							break;
							case
							MGD_INTPTR:
							parser->callbacks[(int)

									  (*
									   (fmt
									    +
									    1))].func
							   (parser, (void *)
							    va_arg(args, intp));
							fmt += 2;
							break;
							case
							MGD_STR:
							parser->callbacks[(int)

									  (*
									   (fmt
									    +
									    1))].func
							   (parser, (void *)
							    va_arg(args,
								   charp));
							fmt += 2;
							break;
							case
							MGD_PTR:
							parser->callbacks[(int)

									  (*
									   (fmt
									    +
									    1))].func
							   (parser, (void *)
							    va_arg(args,
								   voidp));
							fmt += 2;
							break;
						}
					else
						mgd_parser_addchar(parser,
								   *fmt++);
					break;
				}
			}
		}
		else
			mgd_parser_addchar(parser, *fmt++);

	str = mgd_stralloc(pool, fmtlen);
	if (!str)
		return NULL;
	strncpy(str, fmtbuf, fmtlen);
	str[fmtlen] = '\0';

	return str;
}

char *mgd_vformat(midgard * mgd, midgard_pool * pool, const char *fmt,
		  va_list args)
{
	char *str;
	assert(pool);
   assert(mgd);
   assert(mgd->parser);
	if (!fmt)
		return NULL;
	fmtlen = 0;
	while (*fmt) {
		if (*fmt == '$') {
			if (*(fmt + 1) == '$') {
				fmt++;	/* fall thru */
			}
			else {
				/* Force manual selection of system-vital parsers */
				switch (*(fmt + 1)) {
					case 'd':
					case 'i':
					mgd_parser_std_d(mgd->parser, (void *)
							 va_arg(args, int));
					fmt += 2;
					break;
					case 'q':
					mgd_parser_std_q(mgd->parser, (void *)
							 va_arg(args, charp));
					fmt += 2;
					break;
					case 'D':
					mgd_parser_std_D(mgd->parser, (void *)
							 va_arg(args, intp));
					fmt += 2;
					break;
					default:
					if (mgd->parser->callbacks[(int)
								   (*(fmt + 1))].func)
						switch (mgd->parser->callbacks[
									       (int)
									       (*
										(fmt
										 +
										 1))].ptype)
						{
							case
							MGD_CHAR:
							mgd->parser->callbacks
							   [(int)
							    (*(fmt + 1))].func
							   (mgd->parser,
							    (void *) ((int)
								      va_arg
								      (args,
								       int)));
							fmt += 2;
							break;
							case
							MGD_INT:
							mgd->parser->callbacks
							   [(int)
							    (*(fmt + 1))].func
							   (mgd->parser,
							    (void *)
							    va_arg(args, int));
							fmt += 2;
							break;
							case
							MGD_INTPTR:
							mgd->parser->callbacks
							   [(int)
							    (*(fmt + 1))].func
							   (mgd->parser,
							    (void *)
							    va_arg(args, intp));
							fmt += 2;
							break;
							case
							MGD_STR:
							mgd->parser->callbacks
							   [(int)
							    (*(fmt + 1))].func
							   (mgd->parser,
							    (void *)
							    va_arg(args,
								   charp));
							fmt += 2;
							break;
							case
							MGD_PTR:
							mgd->parser->callbacks
							   [(int)
							    (*(fmt + 1))].func
							   (mgd->parser,
							    (void *)
							    va_arg(args,
								   voidp));
							fmt += 2;
							break;
						}
					else
						mgd_parser_addchar(mgd->parser,
								   *fmt++);
					break;
				}
			}
		}
		else
			mgd_parser_addchar(mgd->parser, *fmt++);
	}

	str = mgd_stralloc(pool, fmtlen);
	if (!str)
		return NULL;
	strncpy(str, fmtbuf, fmtlen);
	str[fmtlen] = '\0';
	return str;
    //return g_strdup(fmtbuf);
    //return fmtbuf;
}

int mgd_sql2id(midgard_res * res, int col)
{
	const char *value;
	value = NULL;
	if (!value)
		return 0;
	return strtol(value, NULL, 10);
}

int mgd_sql2int(midgard_res * res, int col)
{
	const char *value = NULL;
	if (!value)
		return 0;
	return strtol(value, NULL, 10);
}

const char *mgd_sql2str(midgard_res * res, int col)
{
	assert(res);
	return NULL;
}

time_t mgd_sql2date(midgard_res * res, int col)
{
	assert(res);
	return (time_t) 0;
}

time_t mgd_sql2time(midgard_res * res, int col)
{
	assert(res);
	return (time_t) 0;
}

time_t mgd_sql2datetime(midgard_res * res, int col)
{
	assert(res);
	return (time_t) 0;
}
