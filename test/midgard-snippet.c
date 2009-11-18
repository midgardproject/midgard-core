#include <midgard/midgard_legacy.h>
#include <midgard/pageresolve.h>
#ifndef WIN32
#include <getopt.h>
#else
#include "win32/getopt.h"
#endif

void Usage(char *progname)
{
   printf(
"Copyright (c) 2000 The Midgard Project Ry."                            "\n"
#ifdef WIN32
"Win32 port (w) 2002 <dsr@best-off.org>."                               "\n\n"
#endif
"Usage: %s [options] class id"                                          "\n"
"Where class is one of article, blobs, element, event, eventmember,"    "\n"
"  file, grp, history, host, image, member, page, pageelement,"         "\n"
"  pagelink, person, preference, record_extension, repligard,"          "\n"
"  sitegroup, snippet, snippetdir, style, topic."                       "\n"
                                                                        "\n"
"  Option               Meaning                          Default"       "\n"
"  -------------------- -------------------------------- -------"       "\n"
"  -D database name     Name of database to connect to   midgard"       "\n"
"  -U database user     Username used to connect         midgard"       "\n"
"  -P database password Username used to connect         midgard"       "\n"
"  -u midgard user      Midgard username                 [empty]"       "\n"
"  -p midgard password  Midgard password                 [empty]"       "\n"
"  -d                   Dump result to stdout            No"            "\n"
"  -f                   Filter dump result               No"            "\n"
"Example:"                                                              "\n"
"  %s /snippetdir/snippetname"                                          "\n"
                                                                        "\n"
, progname, progname);
}

void show(const char* str, int filter);

int main(int argc, char** argv)
{
   int c;
   int id, parent;
   int missed;

	char *db_name = "midgard";
	char *db_user = "midgard";
	char *db_pass = "midgard";
   char *mgd_user = "";
   char *mgd_pass = "";
   int dump = 0;
   int filter = 0;

   char *snippet = "/";

	midgard *mgd;
   midgard_res *res;

   while ((c = getopt (argc, argv, "?D:U:P:u:p:df")) != -1) {
      switch (c) {
         case 'D':
            db_name = strdup(optarg);
            break;

         case 'U':
            db_user = strdup(optarg);
            break;

         case 'P':
            db_pass = strdup(optarg);
            break;

         case 'u':
            mgd_user = strdup(optarg);
            break;

         case 'p':
            mgd_pass = strdup(optarg);
            break;

         case 'd':
            dump = 1;
            break;

         case 'f':
            filter = 1;
            break;

         case '?':
            Usage(argv[0]);
            exit(0);

         default:
            printf("Unexpected option '%c'\n", c);
            Usage(argv[0]);
            exit(1);
      }
   }

   switch (argc - optind) {
      case 0:
         break;
      case 1:
         snippet = argv[optind];
         break;
      default:
         Usage(argv[0]);
         exit(1);
   }

	mgd_init();
   printf("Connecting to mysql://%s:%s@localhost/%s\n", db_user, db_pass, db_name);
	mgd = mgd_connect(NULL,db_name,db_user,db_pass);
	if(!mgd) { printf("Error in DB connection\n"); exit(1); }

	/* We select russian parser in order to dump content unchanged */
	mgd_select_parser(mgd, "russian");

   if (*mgd_user || *mgd_pass) { mgd_auth(mgd, mgd_user, mgd_pass, 0); }

   missed = MGD_PARSE_COMMON_PATH(mgd, snippet, "snippetdir", "snippet",
      &id, &parent);
         
   if (! missed && id != 0 && parent != 0) {
      printf("Snippet %s has ID %d (parent %d)\n", snippet, id, parent);
      if (dump) {
         res = mgd_sitegroup_record(mgd, "code", "snippet", id);
         if (res && mgd_fetch(res)) show(mgd_colvalue(res, 0),filter);
         else printf("ERROR: No such snippet\n");
         if (res) mgd_release(res);
      }
   } else {
      missed = MGD_PARSE_COMMON_PATH(mgd, snippet, "snippetdir", "snippetdir",
         &id, &parent);
      if (! missed && id != 0) {
         printf("Snippetdir %s has ID %d (parent %d)\n", snippet, id, parent);
         if (dump) {
            res = mgd_sitegroup_select(mgd, "id,name", "snippetdir",
               "up=$d", NULL, id);
            while (res && mgd_fetch(res)) {
               printf("dir: %s (%s)\n",
                  mgd_colvalue(res, 1),
                  mgd_colvalue(res, 0));
            }
            if (res) mgd_release(res);

            res = mgd_sitegroup_select(mgd, "id,name", "snippet",
               "up=$d", NULL, id);
            while (res && mgd_fetch(res)) {
               printf("snippet: %s (%s)\n",
                  mgd_colvalue(res, 1),
                  mgd_colvalue(res, 0));
            }
            if (res) mgd_release(res);
         }
      } else {
         if (strcmp(snippet, "/") != 0) {
            printf("Snippet %s not found\n", snippet);
         } else {
            res = mgd_sitegroup_select(mgd, "id,name", "snippetdir",
               "up=0", NULL);
            while (res && mgd_fetch(res)) {
               printf("rootdir: %s (%s)\n",
                  mgd_colvalue(res, 1),
                  mgd_colvalue(res, 0));
            }
            if (res) mgd_release(res);
         }
      }
   }

	mgd_close(mgd);
	mgd_done();

   return 0;
}

static void mgd_preparse_buffer_add(char *text, int len, void *userdata)
{
GByteArray *buffer = (GByteArray*)userdata;

   g_byte_array_append(buffer, (guint8*)text, len);
}

void show(const char* str, int filter)
{
mgd_parser_itf_t itf;
GByteArray *buffer;

   if (!filter) { printf(str); return; }

   buffer = g_byte_array_new();
   itf.output.func = mgd_preparse_buffer_add;
   itf.output.userdata = buffer;
   itf.get_element.func = midgard_pc_get_element;
   itf.get_element.userdata = NULL;
   mgd_preparse_buffer(str, &itf);
   g_byte_array_append(buffer, (const guint8 *)"", 1);
   printf((gchar *)buffer->data);
   g_byte_array_free(buffer, TRUE);
}
