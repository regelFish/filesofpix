#ifndef UTILITIES_H
#define UTILITIES_H

#include <atom.h>
#include <table.h>
#include <seq.h>
#include <str.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <except.h>
#include <readaline.h>

void restore(FILE *fd);

FILE *open_or_abort(char *fname, char *mode);

#endif