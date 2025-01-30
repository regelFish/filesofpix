#include "utilities.h"

Except_T open_abort = { "Could not open file" };
Except_T Umem_aloc_fail = { "Failed to malloc memory" };


/* isInt
 * purpose: Checks if an inputted char is a digit
 *
 * arguments: 
 *      char c: char to be checked
 * returns: true is c is a digit, false if not
 */
bool isInt(char c) 
{
        if ((c >= 48) && (c <= 57)) {
                return true;
        }
        return false;
}

/* It is an unchecked exception for the inputed length to differ from the
 * actual length of the line
 */
int parser(char **line, Seq_T *values, int length) 
{
        bool intReal = false;
        char* startOfInt = NULL;
        Seq_T corrSeq = Seq_new(1);
        
        for (int i = 0; i < length; i++)
        {
                //printf("char %d of string %s with length of %d\n", i, *line, length);
                char *c = (*line) + i;
                //printf("%c\n", *c);
                if (isInt(*c)) {
                        /* When you encouter a digit and the character is the 
                         * first value of the int, make the location of this 
                         * character the start of the characters that will be
                         * turned into an int 
                         */
                        if (!intReal) {
                                startOfInt = (*line + i);
                        }
                        intReal = true;
                } else {
                        /* When you encouter a non-digit character, add 
                         * it to the string of corruption characters*/
                        intReal = false;
                        Seq_addhi(corrSeq, c);
                        //printf("corrSeq: ");
                        for (int i = 0; i < Seq_length(corrSeq); i++)
                        {
                                //printf("%c,  ", *((char *)Seq_get(corrSeq, i)));
                        }
                        //printf("\n");

                }
                /* Note: This is a bit of defensive programming and only 
                 *  one check is likely needed */
                /* When the current character is not an digit and you have 
                 * encoutered a digit before, convert the "string" of digit 
                 * characters into an int and add it to the sequence of ints*/
                if ((startOfInt != NULL) && (!intReal || (i == length-1))) {
                        char *endptr = *line + i;
                        int *toInt = malloc(sizeof(*toInt));
                        *toInt = strtol(startOfInt, &endptr, 10);
                        ////printf("int value: %d\n", toInt);
                        startOfInt = NULL;
                        intReal = false;
                        Seq_addhi(*values, toInt);
                        
                }
        }
        /* NOTE for speed: doing this conversion is just as fast if not 
        faster than using a dynamically allocated char array because if 
        you did that you would have to spend time rewritting, if not once 
        but possibly multiple times*/
        int corrLength = Seq_length(corrSeq);
        char* corrLine = malloc(sizeof(*corrLine) * (corrLength));

        //printf("corrSeq: ");
        for (int i = 0; i < corrLength; i++)
        {
                //printf("%c,  ", *((char *)Seq_get(corrSeq, i)));
                corrLine[i] = *((char *)Seq_get(corrSeq, i));
        }
        //printf("\nvalues (int seq): ");
        for (int i = 0; i <Seq_length(*values); i++)
        {
                //printf("%d, ", *((int *)Seq_get(*values, i)));
        }
        Seq_free(&corrSeq);
        if (corrLine == NULL) {
                RAISE(Umem_aloc_fail);
        }

        //printf("\ncorrLine pre switch: %s\n", corrLine);
        //printf("line pre switch: %s\n", *line);
        //free(*line);
        *line = corrLine;
        //printf("corrline after:%s\n", corrLine);
        //printf("line after: %s\n", *line);


        return corrLength;
}

/*
void printOutput(Seq_T *original)
{
        int rows = Seq_length(*original);
        int cols = Seq_length(Seq_get(*original, 0));
        int maxVal = 255;
        printf("P5\n%d %d\n%d\n", cols, rows, maxVal);
        for (int i = 0; i < rows; i++)
        {
                for (int j = 0; j < cols; i++)
                {
                        printf("%c", *((char *) Seq_get(Seq_get(*original, j), 
                                                                          i)));
                }
                
        }
}*/


void restore(FILE *fd)
{
        Table_T tableOfAtoms = Table_new(20, NULL, NULL);
        size_t length = -2;
        Seq_T *original;
        bool originFound = false;

        /* Loops until the end of the file/input stream. */
        while (length != 0) {
                char *line = NULL;

                length = readaline(fd, &line);
                Seq_T values = Seq_new(2);
                length = parser(&line, &values, length);
                
                //printf("This line parsed is: %s, with length %ld. And ", line, length);
                for (int i = 0; i < Seq_length(values); i++) {
                    //printf("%d ", *((int *)Seq_get(values, i)));
                }
                //printf("\n");
                
                
                /* Add values to table */
                const char *tempAtom = Atom_new(line, length);
                if (Table_get(tableOfAtoms, tempAtom) == NULL) {
                        if (!originFound) {
                                Table_put(tableOfAtoms, tempAtom, Seq_seq(
                                                               &values, NULL));
                        }
                }
                else if (originFound) {
                    Seq_addhi(original, values);
                }
                else {
                    original = (Seq_T *)Table_get(tableOfAtoms, tempAtom);
                    originFound = true;
                }

                free(line);
        }

        
        int cols = Seq_length(Seq_get(*original, 0));
        int rows = Seq_length(*original);
        int maxVal = 255;
        printf("P5\n%d %d\n%d\n", cols, rows, maxVal);
        for (int i = 0; i < rows; i++)
        {
                for (int j = 0; j < cols; i++)
                {
                        printf("%c", *((char *) Seq_get(Seq_get(*original, j), 
                                                                          i)));
                }
                
        }
       // printOutput(original);


        Table_free(&tableOfAtoms);
}

FILE *open_or_abort(char *fname, char *mode)
{
        FILE *fp = fopen(fname, mode);
        if (fp == NULL) {
                RAISE(open_abort);
        }
        return fp;
}

// int main() {
//         /* Parser tests */
//         char* singleIntChar = "12\n";//"2bc1b\\][99a 24tvs\n";
//         //singleIntChar = "";
//         Seq_T values = Seq_new(2);
        
//         int parsedL = parser(&singleIntChar, &values, 3);
//         for (int i = 0; i < Seq_length(values); i++)
//         {
//                 int *num = (int *)Seq_get(values, i);
//                 //printf("Index %d of sequence with length %d: %d \n", i, Seq_length(values), *num);
//         }
//         //printf("Length of string: %d, %s\n", parsedL, singleIntChar);
//         //free(singleIntChar);
//         Seq_free(&values);
// }