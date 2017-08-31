
/******************************************************************************

FILE:  struplow.c

PURPOSE:  Replacements for non-ansi routines

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  John Weiss             Original Development
         05/00  John Weiss			   Place in separate file

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/
#ifndef __CYGWIN__
#include <ctype.h>

/******************************************************************************

MODULE:  strlwr

PURPOSE:  non-ansi string conversion to lower case

RETURN VALUE:
Type = none
Value           Description
-----           -----------

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  John Weiss             Original Development
         01/01  John Rishea            Standardized formatting
 
NOTES:

******************************************************************************/
void strlwr
(
    char *str		/* I/O:  string to convert */
)

{
    while ( *str )
    {
	*str = tolower( *str );
	str++;
    }
}

/******************************************************************************

MODULE:  strupr

PURPOSE:  non-ansi string conversion to upper case

RETURN VALUE:
Type = none
Value           Description
-----           -----------

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  John Weiss             Original Development
         01/01  John Rishea            Standardized formatting

NOTES:

******************************************************************************/
void strupr
(
    char *str		/* I/O:  string to convert */
)

{
    while ( *str )
    {
	*str = toupper( *str );
	str++;
    }
}

#endif
