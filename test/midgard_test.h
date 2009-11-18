#include <midgard/midgard.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void midgard_test_function(midgard *mgd);

int main(int argc, char** argv) {
	char *db_name = "midgard";
	char *db_user = "midgard";
	char *db_pass = "midgard";
	midgard *mgd;

	mgd_init();

	switch (argc) {
		case 4:
			db_pass = argv[3];
		case 3:
			db_user = argv[2];
		case 2:
			db_name = argv[1];
	}
//	mgd = mgd_connect(db_name,db_user,db_pass);
	mgd = mgd_connect(NULL,db_name,db_user,db_pass);
	if(!mgd) fprintf(stderr, "Error in DB connection\n");
	/* We select russian parser in order to dump content unchanged */
	mgd_select_parser(mgd, "russian");
	
	midgard_test_function(mgd);

	mgd_close(mgd);
	mgd_done();

   return 0;
}

