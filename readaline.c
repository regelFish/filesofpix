#include "readaline.h"
#include <stdlib.h>
#include <except.h>

Except_T mem_aloc_fail = { "Failed to malloc memory" };
Except_T bad_input = { "One or more arguments point to NULL" };
Except_T read_fail = { "Failed to read file" };

/* expand
 * purpose: checks if CharArrayList is at capacity and if so increases the 
 * capacity of the ArrayList
 *
 * arguments: 
 *      char **buffer:  a pointer to the address of the start of the char buffer
 *      int *capacityP: a pointer to the capacity of the buffer
 *      int *numCharsP: a pointer to the number of chars currently in the buffer
 * Notes:
 *      Will CRE if malloc fails
 */
void expand(char **buffer, int *capacityP, int *numCharsP) 
{
        *capacityP = *capacityP * 2;
        char *temp = malloc(sizeof(*temp) * (*capacityP));
        if (temp == NULL)
                RAISE(mem_aloc_fail);

        for (int i = 0; i < *numCharsP; i++) {
                temp[i] = (*buffer)[i];
        }
        free(*buffer);
        *buffer = temp;
}

/* readaline
 * purpose: Reads the contents of a file until newline or EOF is reached
 *
 * arguments: 
 *      FILE *inputfd: a pointer to a location in a filestream
 *      char **datapp: a pointer to a char string, where its contents are 
 *                      the contents of the line that was read in the file
 * returns: size_t, the number of characters on the line that was read 
 * Notes:
 *      Will CRE if either argument is NULL
 *      Will CRE if there is an error when reading the file
 *      Will CRE if the if malloc fails
 */
size_t readaline(FILE *inputfd, char **datapp) 
{
        /* Error Checks */
        if ((inputfd == NULL) || (datapp == NULL))
                RAISE(bad_input);

        int numChars = 0;
        int capacity = 1;
        *datapp = malloc(sizeof(**datapp) * capacity);
        if (*datapp == NULL) 
                RAISE(mem_aloc_fail);

        int ch;
        ch = fgetc(inputfd);

        while (ch != EOF) {
                if (ferror(inputfd))
                        RAISE(read_fail);
                if(numChars == capacity)
                        expand(datapp, &capacity, &numChars);
                (*datapp)[numChars] = (char) ch;
                numChars++;
                if (ch == '\n') {
                        return numChars;
                }
                ch = fgetc(inputfd);
        }
        return numChars;
}