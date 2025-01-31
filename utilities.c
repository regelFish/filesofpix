/* 
 * utilities.c
 * by Lawer Nyako {lnyako01}, Rigoberto Rodriguez-Anton {rrodri08}, 01/30/2025
 * filesofpix
 * 
 * summary: Holds the implementations for restore and related helper functions.
 *              Additionally, holds the implementation for the open_or_abort 
 *              function used by main.
 *          
 *              restore uses Hanson's data structures to quickly and efficiently
 *              extract the original data from a corrupted pgm file, outputing
 *              the result to stdout. 
 */

#include "utilities.h"

Except_T open_abort = { "Could not open file" };
Except_T Umem_aloc_fail = { "Failed to malloc memory" };


/* isInt
 * purpose: Checks if an inputted char is a digit
 *
 * Parameters: 
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

/* freeSeqNest
 * purpose: Frees a nest of sequences.
 * 
 * Parameters: target, a pointer to the target Sequence nest to free.
 */
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


/* allocCheck
 * purpose: Checks if memory has been successfully allocated to the given 
 *              address. Will raise an Umem_aloc_fail exception if memory 
 *              hasn't been allocated.
 * 
 * Parameters: ptr, the address to check the memory of.
 */
void allocCheck(void* ptr)
{
        if (ptr == NULL)
                RAISE(Umem_aloc_fail);
}

/* vfree
 * purpose: Higher order function to map into Table_map
 * 
 * Parameters: 
 *             const void *key 
 *             void *value
 *              
 * Parameters: cl 
 */
static void vfree(const void *key, void **value, void *cl)
{
        freeSeqNest(*value);
        (void) key;
        (void) cl;
}

/* seqToStr
 * purpose: converts a sequence of chars to a char array 
 *
 * Parameters: 
 *      Seq_T *seq:     pointer to sequence to be turned into a char array
 *      int length:     length of sequence (is passed through as parameter
 *                      for readability)
 * seq 
 * Parameters: length 
 * returns: char*, 
 * Notes:
 *      Assumes that this a sequence consisting of only chars or values that
 *      can be casted to be chars
 */
char *seqToStr(Seq_T *seq, int length)
{
        char* str = malloc(sizeof(*str) * (length));
        allocCheck(str);

        for (int i = 0; i < length; i++)
        {
                str[i] = *((char *)Seq_get(*seq, i));
        }

        return str;
}

/* parser
 * purpose: 
 *
 * Parameters: 
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
                        allocCheck(toInt);
                        *toInt = strtol(startOfInt, &endOfInt, 10);
                        startOfInt = NULL;
                        makingInt = false;
                        Seq_addhi(*values, toInt);
                }
        }
        int corrLength = Seq_length(corrSeq);
        char* corrLine = seqToStr(&corrSeq, corrLength);
        
        Seq_free(&corrSeq);
        
        free(*line);
        *line = corrLine;

        return corrLength;
}

/* printOutput
 * purpose: prints out the pgm file
 *
 * Parameters: 
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
        for (int i = 0; i < rows; i++)
        {

                for (int j = 0; j < cols; j++) {
                        printf("%c", 
                           *(int *)Seq_get(*(Seq_T *)Seq_get(*original, i), j));
                        }
        }
}

/* decorrupt
 * purpose: Reads through the corrupted file. Then it takes the parsed contents
 *          of each line, and populates a table where the key is an atom of
 *          non digit characters from a line and the value is a sequence of the
 *          digit sequences where the corrupted non digit sequence shows up.
 *          Once you find the same non digit sequence multiple times, you have
 *          the corrupted non digit sequence. For each digit sequence you find
 *          with the corrupted non digit sequence, you add the digit sequence
 *          to its associated sequence of digit sequences.
 *
 * Parameters: 
 *      FILE *fd:                       File stream pointer to the corrupted 
 *                                      file.
 *      Table_T *tableOfAtoms:          A pointer to the table of atoms and 
 *                                      associated sequence of sequences.
 * Parameters: tableOfAtoms 
 * returns: 
 *      Seq_T*, a pointer to the original raster of the image. This is a 
 *      sequence of sequences of ints. 
 */
Seq_T *decorrupt(FILE *fd, Table_T *tableOfAtoms)
{
        Seq_T *original = NULL;
        char *line = NULL;
        size_t length = 0;

        /* Loops until the end of the file/input stream. */
        length = readaline(fd, &line);
        while (length > 0) {
                Seq_T *values = malloc(sizeof(*values));
                *values = Seq_new(1);
                length = parser(&line, values, length);

                const char *tempAtom = Atom_new(line, length);
                if (Table_get(*tableOfAtoms, tempAtom) == NULL) {
                        Seq_T *valuesSeq = malloc(sizeof(*valuesSeq));
                        allocCheck(valuesSeq);
                        *valuesSeq = Seq_seq(values, NULL);
                        Table_put(*tableOfAtoms, tempAtom, valuesSeq);
                }
                else {
                        if (original == NULL)
                                original = (Seq_T *)Table_get(*tableOfAtoms, 
                                                                      tempAtom);
                        Seq_addhi(*original, values);
                }
                values = NULL;

                free(line);
                length = readaline(fd, &line);
        }
        free(line);
        return original;
}

/* restore
 * purpose: Restore a file from its corrupted version into an uncorrupted pgm 
 *          that can be read by Pnmrdr
 *
 * Parameters: 
 *      FILE *fd:       A pointer to the opened file to be read through 
 *                      restored
 */
void restore(FILE *fd)
{
        /*
         * original is the the original raster of the uncorrupted pgm file
         */
        Seq_T *original = NULL;
        Table_T tableOfAtoms = Table_new(2, NULL, NULL);

        original = decorrupt(fd, &tableOfAtoms);

        printOutput(original);


        /* because original is pointing to a value inside the table, and this
         * value needs to be accessed in the printOutput function, the memory used 
         * by the table and its contents are freed here 
        */
        Table_map(tableOfAtoms, vfree, NULL);
        
        Table_free(&tableOfAtoms);
}

/* open_or_abort
 * purpose: opens file stream for use in program
 * 
 *
 * Parameters: 
 *      char *fname:    name of file to open
 *      char *mode:     mode of file accessing
 * returns: FILE*, a pointer to the file stream
 * Expects:
 *      fname and mode to not be NULL (for its use in this project, they will 
 *      never be)
 * Notes:
 *      Will CRE if the file, named by fname fails to open
 */
FILE *open_or_abort(char *fname, char *mode)
{
        FILE *fp = fopen(fname, mode);
        if (fp == NULL) {
                RAISE(open_abort);
        }
        return fp;
}