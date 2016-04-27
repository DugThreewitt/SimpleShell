/* ************************************************************************ *
 * func.h 
 * function prototypes for mysh.c
 * Dug Threewitt - dstgh6
 * CS2750 - Mon/Wed
 * *********************************************************************** */

#include <string.h>

#ifndef FUNC_H_
#define FUNC_H_

int makeargv ( const char * s, const char * delimiters, char *** argvp );

void freemakeargv ( char ** argv );

char * readLine();
#endif
