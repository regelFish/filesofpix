/* 
 * utilities.h
 * by Lawer Nyako {lnyako01}, Rigoberto Rodriguez-Anton {rrodri08}, 01/30/2025
 * filesofpix
 * 
 * summary: Holds the interface for restore and open_or_abort. Restore takes a
 *          filestream pointing to a corrupted pgm as an input, and outputs an
 *          uncorrupted raw pgm to stdout. 
 *          open_or_abort is a function that opens a filestream with the desired
 *          mode while performing a check to makes sure the file is open. 
 * 
 */

#ifndef UTILITIES_H
#define UTILITIES_H

#include <atom.h>
#include <table.h>
#include <seq.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <except.h>
#include <readaline.h>

void restore(FILE *fd);

FILE *open_or_abort(char *fname, char *mode);

#endif