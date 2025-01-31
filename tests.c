

/* readaline test */
        while (test != 0) {
                test = readaline(fd, &line);
                printf("\"%s\" has %ld character(s)\n", line, test);        
                free(line);
        }







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
        //free(*line);
        *line = corrLine;
        //printf("corrline after:%s\n", corrLine);
        //printf("line after: %s\n", *line);


        return corrLength;