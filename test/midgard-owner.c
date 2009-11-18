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
"Example:"                                                              "\n"
"  %s page 5"                                                           "\n"
                                                                        "\n"
, progname, progname);
}

int main(int argc, char** argv)
{
   int c;
   int id, tableid;
   int owner;

	char *db_name = "midgard";
	char *db_user = "midgard";
	char *db_pass = "midgard";
   char *mgd_username = "";
   char *mgd_pass = "";

   char *objtype = NULL;

	midgard *mgd;

   while ((c = getopt (argc, argv, "?D:U:P:u:p:")) != -1) {
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
            mgd_username = strdup(optarg);
            break;

         case 'p':
            mgd_pass = strdup(optarg);
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

   if ((argc - optind) == 2) {
         objtype = argv[optind];
         id = atol(argv[optind+1]);
   } else {
      Usage(argv[0]);
      exit(1);
   }

	mgd_init();
   printf("Connecting to mysql://%s:%s@localhost/%s\n", db_user, db_pass, db_name);
	mgd = mgd_connect(NULL,db_name,db_user,db_pass);
	if(!mgd) { printf("Error in DB connection\n"); exit(1); }

	/* We select russian parser in order to dump content unchanged */
	mgd_select_parser(mgd, "russian");

   if (*mgd_username || *mgd_pass) { mgd_auth(mgd, mgd_username, mgd_pass, 0); }








   printf("User '%s' (%d) has %sownership of %s(%d)/%d\n",
      mgd_username, mgd_user(mgd),
      owner ? "" : "no ",
      objtype, tableid, id);

	mgd_close(mgd);
	mgd_done();

   return 0;
}

