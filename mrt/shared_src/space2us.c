/*
        ***** SPACE2US.C *****

Convert all white space characters in a string to underscores.
Required in RESAMPLE and HDF2HDR programs to avoid problem of
band names with white space.

Input:          string to be converted
Output:         converted string
Return value:   pointer to converted string

Author: John M. Weiss, Ph.D.
Written 12/07/00.

Modifications:
*/

#include <ctype.h>

char *SpaceToUnderscore ( char *str )
{
    char *s = str;

    while ( *s )
    {
        if ( isspace( (int) *s ) )
            *s = '_';
	s++;
    }

    return str;
}

