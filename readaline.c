#include "readaline.h"
#include <stdlib.h>
#include <except.h>

Except_T mem_aloc_fail = { "Failed to malloc memory" };
Except_T bad_input = { "One or more arguments point to NULL" };
Except_T read_fail = { "Failed to read file" };

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

        char ch;
        ch = fgetc(inputfd);

        while (ch != EOF) {
                if (ferror(inputfd))
                        RAISE(read_fail);
                if (ch != '\r') {
                        if(numChars == capacity)
                                expand(datapp, &capacity, &numChars);
                        (*datapp)[numChars] = ch;
                        numChars++;
                        if (ch == '\n') {
                                return numChars;
                        }
                }
                ch = fgetc(inputfd);
        }
        free(*datapp);    
        *datapp = NULL;
        return 0;
}