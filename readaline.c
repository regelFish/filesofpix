#include "readaline.h"
#include <stdio.h>
int numChars = 0;
int capacity = 2;

size_t readaline(FILE *inputfd, char **datapp) 
{
        char *buff;
        buff = malloc(sizeof(char)*capacity);
        mallocCheck(buff, sizeof(char)*capacity);
        free(buff);

        while ((ch = fgetc(inputfd))!= EOF) {
                buff[numChars] = ch;
                numChars++;
                expand(&buff);
        }
        // char buffer[1001];

        // fgets(buffer, sizeof(buffer), inputfd);

        // // If the last character of buffer is not '\n', that means the line was too
        // // long.

        // datapp = buff;

        // size_t length = 0;

        // while (buffer[length] != '\0') {
        //         length++;
        // }

        if (numChars == 0) {
                datapp = NULL;
        } else {
                datapp = &buff;
        }

        return numChars;
}

bool mallocCheck(char *p, int SIZE) 
{
        if (p == NULL) {
                fprintf(stderr, "Failed to malloc %d bytes of memory\n", SIZE);
                return EXIT_FAILURE;
        //what is a checked runtime error
        } 
}

void expand(char **buffer) 
{
        if(numItems == capacity) {
                capacity = capacity*2 + 2;
                char *temp = malloc(sizeof(char)*capacity);
                mallocCheck(temp, sizeof(char)*capacity);

                for (int i = 0; i < numItems; i++) {
                        temp[i] = data[i];
                }
                free(*buffer);
                buffer = &temp;
        }        


}
