#include "readaline.h"
#include <stdlib.h>
#include <stdbool.h>



bool mallocCheck(char *p, int SIZE) 
{
        if (p == NULL) {
                fprintf(stderr, "Failed to malloc %d bytes of memory\n", SIZE);
                return EXIT_FAILURE;
        //what is a checked runtime error
        } 
        return true;
}

void expand(char **buffer, int *capacityP, int *numCharsP) 
{

        *capacityP = *capacityP*2 + 2;
        char *temp = malloc(sizeof(char) * *capacityP);
        mallocCheck(temp, sizeof(char) * *capacityP);

        for (int i = 0; i < *numCharsP; i++) {
                temp[i] = *buffer[i];
        }
        free(*buffer);
        buffer = &temp;       

}

size_t readaline(FILE *inputfd, char **datapp) 
{
        int numChars = 0;
        int capacity = 2;
        char *buff;
        buff = malloc(sizeof(char) * capacity);
        mallocCheck(buff, sizeof(char) * capacity);
        free(buff);
        char ch;
        datapp = NULL;

        while ((ch = fgetc(inputfd))!= EOF) {
                buff[numChars] = ch;
                numChars++;
                if(numChars == capacity) {
                        expand(&buff, &capacity, &numChars);
                }
        }
        // char buffer[1001];

        // fgets(buffer, sizeof(buffer), inputfd);

        // If the last character of buffer is not '\n', that means the line was too
        // long.

        // datapp = buff;

        // size_t length = 0;

        // while (buffer[length] != '\0') {
        //         length++;
        // }

        if (numChars > 0) {
                datapp = &buff;
        }
        (void) datapp;

        return numChars;
}

int main(int argc, char *argv[])
{
        (void) argc;
        (void) argv;

        printf("hello, world");
        return EXIT_SUCCESS;
}