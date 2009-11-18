#include <midgard/midgard_legacy.h>
#include <midgard/pageresolve.h>
#ifndef WIN32
#include <getopt.h>
#else
#include "win32/getopt.h"
#endif

struct {
   int verbose;
} globals = {
   0
};

void report_args(const char *arg, void *userdata)
{
   printf("ARG='%s'\n", arg);
}

void do_cmd(midgard *mgd, char *cmd, mgd_page_t *pagerec)
{
midgard_res *res;

   if (strcmp(cmd, "ls") == 0) {
      res = mgd_ungrouped_select(mgd, "id,name,title", "page", "up=$d", NULL, pagerec->page);
      if (res && mgd_fetch(res)) {
         while (mgd_fetch(res)) {
            printf("<%s> %s [%s]\n", mgd_colvalue(res, 0), mgd_colvalue(res, 1), mgd_colvalue(res, 2));
         }
      }
      if (res) mgd_release(res);

      res = mgd_ungrouped_select(mgd, "id,name,mimetype,location", "blobs", "ptable='page' AND pid=$d", NULL, pagerec->page);
      if (res && mgd_fetch(res)) {
         while (mgd_fetch(res)) {
            printf("<%s> %s [%s,%s]\n", mgd_colvalue(res, 0), mgd_colvalue(res, 1), mgd_colvalue(res, 2), mgd_colvalue(res,3));
         }
      }
      if (res) mgd_release(res);
   } else if (strcmp(cmd, "show") == 0) {
   } else {
      printf("Command '%s' not recognized\n", cmd);
   }
}

void showpage(char *buffer, int len, void *userdata)
{
FILE *f = (FILE*)userdata;

int i;

   for (i=0; i<len; i++) putc(buffer[i], f);
}

gboolean free_elt(gpointer key, gpointer value, gpointer userdata)
{
   g_free(key);
   g_free(value);
   return TRUE;
}

const char* fetch_elt(const char *name, void *userdata)
{
GHashTable *elts = (GHashTable*)userdata;
char *value;

   value = g_hash_table_lookup(elts, name);
   if (value == NULL) return "";
   return value;
}

void store_elt(const char *name, const char *value, void *userdata)
{
GHashTable *store = (GHashTable*)userdata;

   g_hash_table_insert(store, g_strdup(name), g_strdup(value));
   if (globals.verbose) { printf("Storing element '%s'\n", name); }
}

GHashTable  *element_store = NULL;
GArray *path = NULL;
midgard *mgd = NULL;

void cleanup()
{
   if (element_store != NULL) {
      g_hash_table_foreach_remove(element_store, free_elt, NULL);   
      g_hash_table_destroy(element_store);
   }

   if (path != NULL) { g_array_free(path, TRUE); }

	if (mgd != NULL) { mgd_close(mgd); }

	mgd_done();
}

void Usage(char *progname)
{
   printf(
"Copyright (c) 2000 The Midgard Project Ry."                            "\n"
#ifdef WIN32
"Win32 port (w) 2002 <dsr@best-off.org>."                               "\n\n"
#endif
"Usage: %s [options]"                                                   "\n"
"  Option               Meaning                          Default"       "\n"
"  -------------------- -------------------------------- -------"       "\n"
"  -S server name       Hostname of database server      localhost"     "\n"
"  -N database name     Name of database to connect to   midgard"       "\n"
"  -U database user     Username used to connect         midgard"       "\n"
"  -P database password Username used to connect         midgard"       "\n"
"  -h server name       Hostname of webserver            localhost"     "\n"
"  -p port              Portnumber of webserver          80"            "\n"
"  -u URI               URI to test                      /"             "\n"
"  -r root file         Root file to start parse         ./midgard-root.php\n"
"  -c command           ls                               [empty]"       "\n"
"  -d                   Dump page if found               No"            "\n"
"  -t                   Touch (clear) the cache          No"            "\n"
"  -v                   Verbose logging to terminal      No"            "\n"
"Example:"                                                              "\n"
"  %s -u /admin/ -d dump.html -c ls"                                    "\n"
                                                                        "\n"
, progname, progname);
}

int main(int argc, char** argv)
{
	char *db_name = "midgard";
	char *db_user = "midgard";
	char *db_pass = "midgard";
   char *db_server = NULL;
   char *host = "localhost";
   int port = 80;
   char *uri = "/";
   int c;
   char *blobname = NULL;
   char *cmd = NULL;
   char *dumppage = NULL;
   mgd_parser_itf_t parserdata;
   int cache_is_current;
   int touch_cache;
   FILE *root;
   char *rootfile = "midgard-root.php";

   mgd_host_t hostrec;
   mgd_page_t pagerec;

   touch_cache = 0;

   while ((c = getopt (argc, argv, "?vtr:S:d:N:U:P:h:p:u:c:")) != -1) {
      switch (c) {
         case 'S':
            db_server = strdup(optarg);
            break;

         case 'N':
            db_name = strdup(optarg);
            break;

         case 'U':
            db_user = strdup(optarg);
            break;

         case 'P':
            db_pass = strdup(optarg);
            break;

         case 'h':
            host = strdup(optarg);
            break;

         case 'p':
            port = atoi(optarg);
            break;

         case 'u':
            uri = strdup(optarg);
            break;

         case 'c':
            cmd = strdup(optarg);
            break;

         case 'd':
            dumppage = strdup(optarg);
            break;

         case 'r':
            rootfile = strdup(optarg);
            break;

         case 't':
            touch_cache = 1;
            break;

         case 'v':
            globals.verbose = 1;
            break;

         case '?':
            Usage(argv[0]);
            exit(1);

         default:
            printf("Unexpected option '%c'\n", c);
            Usage(argv[0]);
            exit(1);
      }
   }

   mgd_init();
   g_type_init();	
   path = g_array_new(FALSE, FALSE, sizeof(long));
   element_store = g_hash_table_new(g_str_hash, g_str_equal);

   pagerec.found = 0;
   pagerec.page = 0;
   hostrec.found = 0;
   blobname = NULL;
   if (db_server == NULL) { db_server = "localhost"; }
   printf("Connecting to mysql://%s:<hidden>@%s/%s\n",
      db_user, /* db_pass, */ db_server, db_name);
	mgd = mgd_connect(db_server,db_name,db_user,db_pass);
	if (mgd == NULL) {
      printf("RESULT: Error in DB connection\n"); cleanup(); exit(0);
   }

	/* We select russian parser in order to dump content unchanged */
	mgd_select_parser(mgd, "russian");

   if (touch_cache) { 
      printf("Touching the cache\n");
      mgd_cache_touch(mgd, 0);
   }

   printf("Resolving host: http://%s:%d%s\n", host, port, uri);
   mgd_find_host(mgd, host, port, uri, 0, &hostrec);

   if (!hostrec.found) {
      printf("RESULT: No host record match\n"); cleanup(); exit(0);
   }

   printf("Host found: %ld in sitegroup %ld\n", hostrec.host, hostrec.sitegroup);

   printf("Resolving page: http://%s:%d%s\n", host, port, uri);
   mgd_parse_uri(mgd, &hostrec, uri, &pagerec, report_args, path, NULL);

   switch (pagerec.found) {
      case MGD_FOUND_NONE:
         printf("RESULT: No page or blob record found\n");
         cleanup();
         exit(0);

      case MGD_FOUND_PAGE:
         printf("Page found: %ld, active=%d, redirect=%d\n",
            pagerec.page, pagerec.active, pagerec.addslash);
         break;

      case MGD_FOUND_BLOB:
         printf("Blob found: %ld, active=%d, redirect=%d\n",
            pagerec.page, pagerec.active, pagerec.addslash);
         break;

      default:
         printf("RESULT: Unexpected uri parse result %d\n", pagerec.found);
         cleanup();
         exit(0);
   }

   if (pagerec.auth_required) {
      printf("*** AUTHENTICATE ***\n");
   }

   if (dumppage != NULL && pagerec.found == MGD_FOUND_PAGE) {
      printf("Path: ");
      for (c=0; c<path->len; c++) { printf("%d:", path->data[c]); }
      printf("\n");
      printf("Style: %ld\n", pagerec.style);

      parserdata.output.userdata = fopen(dumppage, "w");
      if (parserdata.output.userdata != NULL) {
#if HAVE_CACHE_FIX
         cache_is_current = mgd_cache_is_current(mgd, hostrec.host, pagerec.page, pagerec.style);
#else 
         cache_is_current = mgd_cache_is_current(mgd, pagerec.page);
#endif
         if (cache_is_current) {
            printf("Cache is current for page %ld\n", pagerec.page);
         } else {
            printf("Rebuilding cache for %ld\n", pagerec.page);
         }

         mgd_load_styles(mgd, path, hostrec.host, pagerec.style, midgard_pc_set_element, element_store,
            (cache_is_current ? pagerec.page : 0));

         parserdata.get_element.func = midgard_pc_get_element;
         parserdata.get_element.userdata = element_store;

         parserdata.output.func = showpage;

         printf("Parse page\n");
         if ((root = fopen(rootfile, "r")) == NULL) {
            fprintf(stderr, "Cannot open root file %s\n", rootfile);
            exit(1);
         }
         mgd_preparse_file(root, &parserdata);
         fclose(root);
         printf("Page parse done\n");

         fclose(parserdata.output.userdata);
      }
   }

   if (cmd) do_cmd(mgd, cmd, &pagerec);

   cleanup();
   return 0;
}

