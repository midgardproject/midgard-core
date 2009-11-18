#include <midgard/midgard_legacy.h>
#include <midgard/pageresolve.h>
#ifndef WIN32
#include <getopt.h>
#else
#include "win32/getopt.h"
#endif

void printstr(char *buffer, int len, void *userdata)
{
    fwrite(buffer, sizeof(char), len, stdout);
}

int main(int argc, char** argv)
{
    mgd_parser_itf_t itf;

    itf.output.func = printstr;
    itf.output.userdata = NULL;
    itf.get_element.func = NULL;
    itf.output.userdata = NULL;

    mgd_preparse_file(stdin, &itf);
    
    return 0;
}
