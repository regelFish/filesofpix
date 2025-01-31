#include "utilities.h"

Except_T bad_use = { "Only one argument allowed" };

/* main
 * purpose: Opens the requested file while performing error checks, and calls 
 *                      restore to remove corruption from the requested file. 
 *
 * arguments: 
 *      int argc:       The number of arguments provided when executed.
 *      char *argv:     The contents in each argument. In this case, 
 *                      the name of the file to restore. 
 * 
 * returns: Returns EXIT_SUCCESS if successfully executed. 
 */
int main(int argc, char *argv[])
{
        char *fname;

        if (argc == 2) {
                fname = argv[1];
        }
        else if (argc == 1) {
                int index = readaline(stdin, &fname);
                fname[index - 1] = '\0';
        }
        else 
                RAISE(bad_use);

        FILE *fd = open_or_abort(fname, "r");
        if (argc == 1)
                free(fname);
        
        restore(fd);

        fclose(fd);
        return EXIT_SUCCESS;
} 