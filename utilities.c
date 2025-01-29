#include "utilities.h"

Except_T open_abort = { "Could not open file" };

bool isInt(char c) 
{
        if ((c >= 48) && (c <= 57)) {
                return true;
        }
        return false;
}

int parser(char **line, Seq_T *values, int length) 
{
        int corrLength = 0;
        char* corrLine = NULL;
        bool intReal = false;
        char* startOfInt = NULL;
        
        for (int i = 0; i < length; i++)
        {
                char c = (*line)[i];
                if (isInt(c)) {
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
                        if (corrLine == NULL) {
                                corrLine = Str_catv(&c, 0, 0, NULL);
                        }
                        else {
                                corrLine = Str_catv(corrLine, 1, corrLength, 
                                                    &c, 0, 0, NULL);
                        }
                        corrLength++;
                }
                /* Note: This is a bit of defensive programming and only 
                 *  one check is likely needed */
                /* When the current character is not an digit and you have 
                 * encoutered a digit before, convert the "string" of digit 
                 * characters into an int and add it to the sequence of ints*/
                if (!intReal && (startOfInt != NULL)) {
                        char *endptr = *line + i;
                        int toInt = strtol(startOfInt, &endptr, 10);
                        startOfInt = NULL;
                        Seq_addhi(*values, &toInt);
                }
        }
        free(*line);
        line = &corrLine;

        return corrLength;
}



void restore(FILE *fd)
{
        Table_T tableOfAtoms = Table_new(20, NULL, NULL);
        size_t length = -2;

        /* Loops until the end of the file/input stream. */
        while (length != 0) {
                char *line = NULL;

                length = readaline(fd, &line);
                Seq_T values = Seq_new(2);
                length = parser(&line, &values, length);
                
                printf("This line parsed is: %s, with length %ld. And ", 
                                                                 line, length);
                for (int i = 0; i < Seq_length(values); i++) {
                    printf("%d ", *((int *)Seq_get(values, i)));
                }
                printf("\n");
                
                
                /* Add values to table */
                const char *tempAtom = Atom_new(line, length);
                if (Table_get(tableOfAtoms, tempAtom) == NULL) {
                    Table_put(tableOfAtoms, tempAtom, Seq_seq(&values, NULL));
                }
                else {
                    Seq_addhi(Table_get(tableOfAtoms, tempAtom), values);
                }

                free(line);
        }

        (void) fd;
        (void) tableOfAtoms;

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