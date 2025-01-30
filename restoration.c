#include "utilities.h"

Except_T bad_use = { "Only one argument allowed" };


int main(int argc, char *argv[])
{
        char *fname;

        if (argc == 2) {
                fname = argv[1];
        }
        else if (argc == 1) {
                printf("Type in a file name: ");
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