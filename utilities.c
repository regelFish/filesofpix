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

void freeSeqNest(Seq_T *target)
{
        for (int i = 0; i < Seq_length(*target); i++)
        {
                for (int j = 0; j < 
                                  Seq_length(*(Seq_T *)Seq_get(*target, i)); j++)
                {
                        free(Seq_get(*(Seq_T *)Seq_get(*target, i), j));
                }
                
                Seq_free((Seq_T *)Seq_get(*target, i));
                free((Seq_T *)Seq_get(*target, i));
        }
        Seq_free(target);
        free(target);
}

static void vfree(const void *key, void **value, void *cl)
{
        freeSeqNest(*value);
        (void) key;
        (void) cl;
}

char *seqToStr(Seq_T *seq, int length)
{
        char* str = malloc(sizeof(*str) * (length));
        
        if (str == NULL) {
                RAISE(Umem_aloc_fail);
        }

        //printf("corrSeq: ");
        for (int i = 0; i < length; i++)
        {
                //printf("%c,  ", *((char *)Seq_get(corrSeq, i)));
                str[i] = *((char *)Seq_get(*seq, i));
        }

        return str;
}

/* parser
 * purpose: 
 *
 * arguments: 
 *      char **line:    a pointer to the address of the original 
 *      Seq_T *values:  
 *      int length:     
 * returns: the length of the string of corrupted characters
 * Notes: 
 *      - It is an unchecked exception for the inputted length to differ from 
 *      the actual length of the line
 *      - Will CRE if malloc fails
 */
int parser(char **line, Seq_T *values, int length) 
{
        bool makingInt = false;
        char* startOfInt = NULL;
        Seq_T corrSeq = Seq_new(1);
        
        for (int i = 0; i < length; i++)
        {
                char *c = (*line) + i;
                //printf("%c\n", *c);
                if (isInt(*c)) {
                        /* When you encouter a digit and the character is the 
                         * first value of the int, make the location of this 
                         * character the start of the characters that will be
                         * turned into an int 
                         */
                        if (!makingInt) {
                                startOfInt = (*line + i);
                        }
                        makingInt = true;
                } else {
                        /* When you encouter a non-digit character, add 
                         * it to the string of corruption characters*/
                        makingInt = false;
                        Seq_addhi(corrSeq, c);
                        //printf("corrSeq: ");
                        for (int i = 0; i < Seq_length(corrSeq); i++)
                        {
                                //printf("%c,  ", *((char *)Seq_get(corrSeq, i)));
                        }
                }

                /* Note: This is a bit of defensive programming and only 
                 *  one check is likely needed */
                /* When the current character is not an digit and you have 
                 * encoutered a digit before, convert the "string" of digit 
                 * characters into an int and add it to the sequence of ints*/
                if ((startOfInt != NULL) &&
                                           (!makingInt || (i == length - 1))) {
                        char *endOfInt = *line + i;
                        int *toInt = malloc(sizeof(*toInt));
                        *toInt = strtol(startOfInt, &endOfInt, 10);
                        ////printf("int value: %d\n", toInt);
                        startOfInt = NULL;
                        makingInt = false;
                        Seq_addhi(*values, toInt);
                }
        }
        int corrLength = Seq_length(corrSeq);
        char* corrLine = seqToStr(&corrSeq, corrLength);
        
        Seq_free(&corrSeq);
        
        

        //printf("\ncorrLine pre switch: %s\n", corrLine);
        //printf("line pre switch: %s\n", *line);
        
        free(*line);
        *line = corrLine;
        //printf("corrline after:%s\n", corrLine);
        //printf("line after: %s\n", *line);


        return corrLength;
}

/* printOutput
 * purpose: prints out the pgm file
 *
 * arguments: 
 *      Seq_T *original: a pointer to the sequence of sequences representing 
 *                      the raster. This will be used to get the length and 
 *                      width of the raster, alongside the contents of the 
 *                      raster, and print all of the information needed for 
 *                      to stdout.
 * returns: void 
 */
void printOutput(Seq_T *original)
{
        
        int rows = Seq_length(*original);
        int cols = Seq_length(*(Seq_T *)Seq_get(*original, 0));
        int maxVal = 255;
        printf("P5\n%d %d\n%d\n", cols, rows, maxVal);
        for (int i = 0; i < Seq_length(*original); i++)
        {
                for (int j = 0; j < 
                               Seq_length(*(Seq_T *)Seq_get(*original, i)); j++)
                        printf("%c", 
                           *(int *)Seq_get(*(Seq_T *)Seq_get(*original, i), j));
        }
}


void restore(FILE *fd)
{
        Table_T tableOfAtoms = Table_new(20, NULL, NULL);
        size_t length = -2;
        Seq_T *original = NULL;
        char *line = NULL;

        /* Loops until the end of the file/input stream. */
        length = readaline(fd, &line);
        while (length > 0) {
                Seq_T *values = malloc(sizeof(*values));
                *values = Seq_new(1);

                length = parser(&line, values, length);


                const char *tempAtom = Atom_new(line, length);
                if (Table_get(tableOfAtoms, tempAtom) == NULL) {
                        Seq_T *valuesSeq = malloc(sizeof(*valuesSeq));
                        *valuesSeq = Seq_seq(values, NULL);
                        Table_put(tableOfAtoms, tempAtom, valuesSeq);
                }
                else {
                        if (original == NULL)
                                original = (Seq_T *)Table_get(tableOfAtoms, 
                                                                      tempAtom);
                        Seq_addhi(*original, values);
                }
                

                values = NULL;


                free(line);
                length = readaline(fd, &line);
        }
        free(line);
        
        printOutput(original);

        Table_map(tableOfAtoms, vfree, NULL);
        
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


