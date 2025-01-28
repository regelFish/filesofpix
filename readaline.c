#include "readaline.h"


size_t readaline(FILE *inputfd, char **datapp) 
{
        char buffer[1001];

        fgets(buffer, sizeof(buffer), inputfd);

        /*  
        If the last character of buffer is not '\n', that means the line was too
        long. 
        */

        datapp = buffer;
    
        size_t length = 0;

        while (buffer[length] != '\0') {
            length++;
        }

        return length;
}
