

/* readaline test */
        while (test != 0) {
                test = readaline(fd, &line);
                printf("\"%s\" has %ld character(s)\n", line, test);        
                free(line);
        }