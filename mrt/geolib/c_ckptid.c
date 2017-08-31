
/****************************************************************************
NAME:				c_ckptid

PURPOSE:  Checks point id to see if it has been used before

PROGRAM HISTORY:
VERSION	 DATE	AUTHOR	   CODE/CONT   REASON
-------	 ----	------	   ---------   -----------------------------
  5.0	 4/89	D. Steinwand  CSB      LAS 5.0 Original development
	 8/90   L. Huewe      CSB      Modified to remove all blanks from
				       the point id string.
  5.1   10/94	D. Etrheim    CSB      Modified to make the tie point id
				       array to be dynamiclly allocated.

ALGORITHM DESCRIPTION:
	Convert pt_id to lower case
	Remove any white space from the pt_id
	If the resulting string is a null, return FALSE
	Check the static point id array to see if the current id is there
	If there, return FALSE
	Put the current point id in the point id array
	Increment the counter
	Return TRUE
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "shared_resample.h"

#include "tae.h"
#include "worgen.h"
#include "util.h"
#include "geompak.h"

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

int c_ckptid( char *pt_id	/* Tie point identifier */
     )
{
    static char ( *id )[20];	/* All point id's entered so far */
    static int count = 0;	/* Point id count */
    static long maxtie;		/* maximum limit on number of tie points */
    long i;			/* Loop counter */
    long twenty = 20;		/* Constant of 20 */
    char *string;		/* Input string with blanks removed */
    long len;

    len = ( long ) strlen( pt_id );
    c_up2low( pt_id, &len );
    string = rem_blanks( pt_id, &len );
    if ( *string == '\0' )
	return ( FALSE );
    if ( count == 0 )
    {
	maxtie = MAX_TIE_POINTS;
	id = ( char ( * )[20] ) malloc( maxtie * twenty );
	if ( id == NULL )
	    ErrorHandler( TRUE, "c_ckptid", ERROR_MEMORY, NULL );
    }
    else if ( count == maxtie )
    {
	maxtie += MAX_TIE_POINTS;
	id = ( char ( * )[20] ) realloc( id, maxtie * twenty );
	if ( id == NULL )
	    ErrorHandler( TRUE, "c_ckptid", ERROR_MEMORY, NULL );
    }

    for ( i = 0; i < count; i++ )
	if ( strcmp( id[i], string ) == 0 )
	    return ( FALSE );
    strcpy( id[count], string );
    strcpy( pt_id, string );
    free( string );
    count++;

    return ( TRUE );
}

/******************************************************************************
NAME:	REM_BLANKS

PURPOSE:
	REM_BLANKS returns the input character with all white space removed.
	This includes leading, trailing and internal white sapce.

PROGRAM HISTORY:
	By L. Huewe, USGS/EROS Data Center, 8/90

COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:	
		Run under TAE

PROJECT:	LAS

ALGORITHM:

	Allocate buffer space for the output string.
	For each character in the input string
	    If the character is not a blank
		Copy it to the output string.
	Null terminate the output string.
	Return to the calling routine.

******************************************************************************/
char *rem_blanks( char *str, long *len )
{
    char *newstr, *ptr, *newptr;

    newstr = ( char * ) malloc( *len + 1 );
    if ( newstr == NULL )
	ErrorHandler( TRUE, "c_ckptid(rem_blanks)", ERROR_MEMORY, NULL );

/*
   Copy all non-blank characters from the input string to the output string.
*/
    for ( newptr = newstr, ptr = str; ( ( ptr < str + *len ) && ( *ptr != '\0' ) );
	  ptr++ )
    {
	if ( isspace( (int)*ptr ) == 0 )
	{
	    *newptr = *ptr;
	    newptr++;
	}
    }
    *( newptr ) = '\0';
    return ( newstr );
}
