
/******************************************************************************
NAME: C_DTMNUM

PURPOSE: Retrieve LAS datum number matching the datum name, if no datum name
         is found, a -1 is returned.

PROGRAM HISTORY:
VERSION     DATE      AUTHOR         REASON
-------    ------     ------         ------
  1.0      02/98      J. Willems     Initial development for datum conversion
  2.0      03/98      T. Ratliff     Modified to handle abbrev. names and NAD
                                       datums if North American is specified.
  3.0      01/99      T. Ratliff     Modified to give nonfatal error messages
                                       and to return a -1 in the datum number
                                       field if no datum name found.
  4.0      04/99      D. Lloyd       Used realloc() for memory allocation
                                       and freed allocated memory.
  5.0      05/99      T. Ratliff     Modified to return the datum number,
                                       previous version returned only the
                                       spheroid number.
                                     Rewrote cmp_datums() to better check 
                                       datum names for validity and year.
                                     Wrote the compress() function which will
                                       remove blank spaces from the user datum

PROJECT: LAS

ALGORITHM:
        Remove all blank spaces from the input datum
	Make the input datum all capital letters
        Check if input datum is NAD
        For the 2 datum tables (datum.txt & spheroid.txt)
          Read each line in from the datum table
          While not EOF and the datum is not found
            Get the datum number a datum name from the tables
            Call cmp_datums() to determine if the datum names match
            If the datum names match
              assign datum number for that datum
          End While
          Close the datum table file
        End For
        Free memory
        Return

******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shared_resample.h"
#include "las.h"
#include "datum.h"
#include "util.h"
#include "geompak.h"

static void datum_compress( char * );

static long cmp_datums( char *sdtm,	/* Datum name from tables        */
			char *udtm	/* Datum name from user          */
     );

static long check_char( long *idx,	/* Index into the datum name            */
			int ch1,	/* Char in datum name at idx position   */
			int ch2	/* Char in datum name at idx+1 position */
     );

int c_dtmnum( char *dtm_name,	/* Datum name                         */
	      long *dtm_num     /* Datum number                       */
     )
{
    FILE *file_ptr;		/* Pointer to datum table             */

    char *dtmary[2] = { 0 };	/* Array of dtm_num fields            */
    char filenames[2][CMLEN];	/* Holds the names of the datum files */
    char line[BUFSIZ];		/* Line scanned in from datum table   */
    char *table_loc;		/* Location of the MRT_DATA_DIR       */
    char *temp;			/* Temporary storage of line in table */
    char errmsg[256];

    long filenum;		/* File counter                       */
    long found = FALSE;		/* Flag set if code is found          */
    long i;			/* Increment for loop                 */
    long length;		/* Increment for loop                 */

    /* Set the returning datum number to -1
       This way we don't keep setting it
       all the time.
       --------------------------------------- */
    *dtm_num = -1;

    /* Remove the white space from the 
       datum name.  Must compress here
       so if dtm_name is NAD, the year
       is in the 3rd position.
       ---------------------------------- */
    datum_compress( dtm_name );
    length = ( long ) strlen( dtm_name );

    /* Change the name to capital letters
       ------------------------------------- */
    c_low2up( dtm_name, &length );

    /* Get the directory of the tables
       ---------------------------------- */
    table_loc = getenv("MRT_DATA_DIR");
    if( table_loc == NULL ) {
       table_loc = getenv("MRTDATADIR");
       if (table_loc == NULL) {
          ErrorHandler( FALSE, "c_dtmnum", ERROR_ENV,
             "MRT_DATA_DIR or MRTDATADIR not defined");
          return(E_GEO_FAIL);
       }
    }

/*    table_loc = "../data"; */

    /* Determine if the datum_name has
       the string "NAD" in it
       ---------------------------------- */
    if ( strncmp( dtm_name, "NAD", 3 ) == 0 )
    {
	/* Determine which North American Datum is wanted
	   ------------------------------------------------- */
	if ( strncmp( &dtm_name[3], "27", 2 ) == 0 )
	{
	    *dtm_num = NAD27;
	    found = TRUE;
	}
	else if ( strncmp( &dtm_name[3], "83", 2 ) == 0 )
	{
	    *dtm_num = NAD83;
	    found = TRUE;
	}
    }

    /* There are 2 files to look through to find the
       datum.  They are MRT_DATA_DIR/datum.txt and
       MRT_DATA_DIR/spheroid.txt.  We will look through
       the datum.txt file first.
       ------------------------------------------------- */
    sprintf( filenames[0], "%s/datum.txt", table_loc );
    sprintf( filenames[1], "%s/spheroid.txt", table_loc );
    filenum = 0;
    while ( ( !found ) && ( filenum < 2 ) )
    {
	/* Open the file for read access
	   -------------------------------- */
	if ( ( file_ptr = fopen( filenames[filenum], "r" ) ) == NULL )
	{
            sprintf( errmsg, "Unable to open %s", filenames[filenum]);
	    ErrorHandler( FALSE, "c_dtmnum", ERROR_OPEN_DATUMFILE, errmsg );
	    return ( E_GEO_FAIL );
	}

	/* While we are not at the EOF and
	   we have not found a match
	   ---------------------------------- */
	while ( ( fgets( line, BUFSIZ, file_ptr ) != NULL ) && ( !found ) )
	{
	    /* Get the datum number and datum name from
	       the datum file
	       ------------------------------------------- */
	    for ( i = 0; i < 2; i++ )
	    {
		temp = strtok( i ? NULL : line, ":" );
		dtmary[i] = realloc( dtmary[i], strlen( temp ) + 1 );
		strcpy( dtmary[i], temp );
	    }

	    /* Compare the datum names
	       -------------------------- */
	    if ( cmp_datums( dtmary[1], dtm_name ) )
	    {
		*dtm_num = atoi( dtmary[0] );
		found = TRUE;
	    }
	}
	fclose( file_ptr );
	filenum++;
    }

    /* Free allocated space
       ----------------------- */
    for ( i = 0; i < 2; i++ )
	if ( dtmary[i] != NULL )
	    free( dtmary[i] );

    return ( E_GEO_SUCC );
}

/******************************************************************************
NAME: CMP_DATUMS

PURPOSE: Examines 2 strings to see if they match.

PROGRAM HISTORY:
VERSION     DATE      AUTHOR         REASON
-------    ------     ------         ------
  1.0      05/99      T. Ratliff     Initial development for datum conversion

PROJECT: LAS

ALGORITHM:
******************************************************************************/
static long cmp_datums( char *sdtm,	/* Datum name from tables        */
			char *udtm	/* Datum name from user          */
     )
{
    long idx_sdtm;		/* Index of the ptr variable     */
    long idx_udtm;		/* Index of the udtm variable    */
    long len_sdtm = 0;		/* Length of ptr variable        */
    long len_udtm = 0;		/* Length of udtm variable       */
    long match = TRUE;		/* Returned if matched datums    */
    long nomatch = FALSE;	/* Returned if no datums matched */
    long stop = FALSE;		/* Loop contol variable          */
    long status;		/* Return status of functions    */

    /* Get the length and set the index
       of the user datum
       ----------------------------------- */
    idx_udtm = 0;
    len_udtm = ( long ) strlen( udtm );

    /* Remove the blanks from the system
       datum, then find the length and
       set the index
       ------------------------------------ */
    datum_compress( sdtm );
    idx_sdtm = 0;
    len_sdtm = ( long ) strlen( sdtm );

    /* Compare the characters of the datum's until
       the stop flag is set or we have reached the
       end of one of the datum's
       ---------------------------------------------- */
    while ( ( !stop ) && ( idx_udtm < len_udtm ) && ( idx_sdtm < len_sdtm ) )
    {
	/* If the 2 characters match, increment
	   both indexes
	   --------------------------------------- */
	if ( sdtm[idx_sdtm] == udtm[idx_udtm] )
	{
	    idx_sdtm++;
	    idx_udtm++;
	}
	else if ( udtm[idx_udtm] == '_' )
	    idx_udtm++;
	else
	{
	    status = check_char( &idx_sdtm, sdtm[idx_sdtm], sdtm[idx_sdtm + 1] );
	    if ( status != E_GEO_SUCC )
	    {
		status = check_char( &idx_udtm, udtm[idx_udtm], udtm[idx_udtm + 1] );
		if ( status != E_GEO_SUCC )
		    stop = TRUE;
	    }
	}
    }

    /* If the stop flag was not set AND all characters
       in both datum's have been compared, we have matched
       ------------------------------------------------------- */
    if ( ( !stop ) && ( len_udtm == idx_udtm ) && ( len_sdtm == idx_sdtm ) )
	return ( match );
    else
	return ( nomatch );
}

/******************************************************************************
NAME: CHECK_CHAR

PURPOSE: To determine if the current characters make up a year specification.

PROGRAM HISTORY:
VERSION     DATE      AUTHOR         REASON
-------    ------     ------         ------
  1.0      05/99      T. Ratliff     Initial development for datum conversion

PROJECT: LAS

ALGORITHM:
   If the character in the idx position is a '1'
     If it is, we then check if the character in the idx+1 position is an
       '8', or '9'.
       Increment the position by 2.

NOTE: This function is designed to be added to if future datum names contain
      other year specifications.
******************************************************************************/
static long check_char( long *idx,	/* Index into the datum name            */
			int ch1,	/* Char in datum name at idx position   */
			int ch2	/* Char in datum name at idx+1 position */
     )
{
    /* First we check if we have a '1'.  If we
       do, this could possibly be a year.  If
       it is a year, we want to skip the first
       2 digits
       ------------------------------------------ */
    switch ( ch1 )
    {
	case '1':
	    {
		/* To be certain it is a year, we check the
		   next character.  If it's an 8 or 9, we
		   move the index 2 positions
		   ------------------------------------------- */
		switch ( ch2 )
		{
		    case '8':
			*idx += 2;
			break;
		    case '9':
			*idx += 2;
			break;
		    default:
			return ( E_GEO_FAIL );
		}
		break;
	    }
	default:
	    return ( E_GEO_FAIL );
    }
    return ( E_GEO_SUCC );
}

/******************************************************************************
NAME: COMPRESS

PURPOSE: Removes all blank spaces from a string.

PROGRAM HISTORY:
VERSION     DATE      AUTHOR         REASON
-------    ------     ------         ------
  1.0      05/99      T. Ratliff     Initial development for datum conversion

PROJECT: LAS

ALGORITHM:

******************************************************************************/
static void datum_compress( char *dtm	/* Input datum string           */
     )
{
    long i;			/* Loop control variable        */
    long len;			/* Length of the input datum    */
    char *tmpdtm;		/* Temporary string holder      */
    char *ptr;			/* Pointer to dtm               */

    /* Initialize variables
       ----------------------- */
    i = 0;
    len = ( long ) strlen( dtm );
    tmpdtm = ( char * ) malloc( sizeof( char ) * len );

    strcpy( tmpdtm, "" );

    /* Grab every word that preceeds a blank space
       and concatinate it with tmpdtm
       ---------------------------------------------- */
    while ( ( ptr = strtok( i ? NULL : dtm, " " ) ) != NULL )
    {
	strcat( tmpdtm, ptr );
	i++;
    }

    strcpy( dtm, tmpdtm );
    free( tmpdtm );
}
