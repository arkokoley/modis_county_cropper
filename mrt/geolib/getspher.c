
/*--------------------------------------------------------------------------
ROUTINE: c_get_spheroid

PURPOSE:
        Find and return the spheroid datum that is associated with the given 
        semi-major and semi-minor axes.

PROGRAM HISTORY
VERSION     PROGRAMMER      DATE        REASON
-------     ----------      ----        ------
1.0         T. Beckmann     Jul 1999    Original development

INPUTS:
        semi_major - ellipsoid semi-major axis
        semi_minor - ellipsoid semi-minor axis
        
OUTPUTS:
        datum - spheroid datum associated with the semi-major and semi-minor
                axes
        datum_valid - returned datum is valid flag (1=valid)
        returns - E_GEO_SUCC when a valid datum is found
                  E_GEO_FAIL when no valid datum is found
                  
ALGORITHM:
        Calculate the inverse flattening coefficient
        Construct the path to the spheroid datum text file
        Open the spheroid text file
        Search the spheroid text file for a datum that is within the tolerances
        Close the spheroid text file
        If a matching datum was found, return it and E_GEO_SUCC
        Otherwise, return E_GEO_FAIL
        
NOTES:
- Some of the spheroid datums have nearly identical parameters (GRS80 and 
  WGS84).  The first one found that is within tolerances is returned.

--------------------------------------------------------------------------*/

#include <stdlib.h>				/* getenv prototype */
#include <stdio.h>				/* fopen prototype */
#include <math.h>				/* fabs prototype */
#include <string.h>				/* string function prototypes */
#include "shared_resample.h"
#include "las.h"				/* las typedefs */
#include "geompak.h"			/* prototype for this routine */

#define BUFFER_SIZE 200

/* size of buffer for reading lines of text from the spheroid.txt file */

#define SPHEROID_FILE "spheroid.txt"

/* name of spheroid data text file */

#define SEMI_MAJOR_TOLERANCE 1.0
#define INV_FLAT_TOLERANCE   0.01

/* tolerances allowed for finding a matching datum */

int c_get_spheroid( double semi_major,	/* I: ellipsoid semi-major axis */
	double semi_minor,      /* I: ellipsoid semi-minor axis */
	long *datum,            /* O: matching ellipsoid datum number */
	long *datum_valid       /* O: valid flag for the datum (1=valid) */
        )
{
	FILE *file;					/* pointer to the spheroid.txt file */
	double inv_flat;			/* inverse flattening parameter */
	double read_inv_flat;		/* inverse flattening param read from spheroid.txt */
	double read_major;			/* semi-major axis read from spheroid.txt */
	long read_datum;			/* datum read from the spheroid.txt file */
	int count;					/* temporary count variable */
	int items_parsed;			/* count of items parsed from the line */
	int match = 0;				/* flag to indicate a matching datum has been found */
	char *buf_ptr;				/* temporary pointer into buffer */
	char buffer[BUFFER_SIZE];	/* buffer for reading data from the file */
	char filename[CMLEN];		/* buffer for the path to spheroid.txt */
	char *las_table;			/* pointer to LASTABLE environment variable */

	/* assume the datum is invalid in case we encounter a failure along
	   the way */
	*datum = 0;
	*datum_valid = 0;

	/* calculate the inverse flattening */
	if( semi_minor == 0.0 )
		inv_flat = 0.0;
	else if ( fabs( semi_minor - semi_major ) > 1 )
		inv_flat = 1.0 / ( 1.0 - semi_minor / semi_major );
	else
		inv_flat = 0.0;

	/* get the path to the spheroid data file */

        las_table = getenv( "MRT_DATA_DIR" );
        if( las_table == NULL ) {
           las_table = getenv( "MRTDATADIR" );
           if ( las_table == NULL ) {
              ErrorHandler( FALSE, "c_get_spheroid", ERROR_ENV,
                    "MRT_DATA_DIR or MRTDATADIR not defined");
              return ( E_GEO_FAIL );
           }
        }

/*    las_table = "../data"; */

	strcpy( filename, las_table );
	if ( filename[strlen( filename ) - 1] != '/' )
		strcat( filename, "/" );
	strcat( filename, SPHEROID_FILE );

	/* open the spheroid file */
	file = fopen( filename, "rt" );
	if ( file == NULL )
	{
		MessageHandler( "c_get_spheroid", "Unable to open %s", filename );
		ErrorHandler( FALSE, "c_get_spheroid", ERROR_OPEN_SPHEREFILE, NULL );
		return ( E_GEO_FAIL );
	}

	/* search the spheroid data file for a matching spheroid */
	while ( ( match == 0 ) && ( fgets( buffer, BUFFER_SIZE, file ) != NULL ) )
	{
		/* extract the datum number from the line */
		if ( sscanf( buffer, "%ld", &read_datum ) != 1 )
		{
			MessageHandler( "c_get_spheroid", "Unable to parse %s", buffer );
			ErrorHandler( FALSE, "c_get_spheroid", ERROR_GENERAL, NULL );
			continue;
		}
		/* skip over the text description */
		buf_ptr = buffer;
		count = 2;				/* skip 2 ':' delimiters */
		while ( ( *buf_ptr != '\0' ) && ( count > 0 ) )
		{
			if ( *buf_ptr == ':' )
				count--;
			buf_ptr++;
		}
		if ( count > 0 )
		{
			MessageHandler( "c_get_spheroid", "Unable to parse %s", buffer );
			ErrorHandler( FALSE, "c_get_spheroid", ERROR_GENERAL, NULL );
			continue;
		}

		/* extract the major axis and inverse flattening */
		items_parsed = sscanf( buf_ptr, "%lf:%lf:", &read_major, &read_inv_flat );
		if ( items_parsed == 1 )
		{
			/* may only have one match for spheroid (flattening left empty) */
			read_inv_flat = 0.0;
		}
		else if ( items_parsed != 2 )
		{
			MessageHandler( "c_get_spheroid", "Unable to parse %s", buffer );
			ErrorHandler( FALSE, "c_get_spheroid", ERROR_GENERAL, NULL );
			continue;
		}
/*		printf("Checking  %lf, %lf:  %lf, %lf %d\n", semi_major, inv_flat, read_major, read_inv_flat, read_datum ); */

		/* check for a match */
		if ( ( fabs( read_major - semi_major ) < SEMI_MAJOR_TOLERANCE ) &&
			 ( fabs( read_inv_flat - inv_flat ) < INV_FLAT_TOLERANCE ) )
		{
			match = 1;
		}
	}

	/* close the spheroid file */
	fclose( file );

	/* if there was a matching spheroid, return the datum with the valid
	   flag set. (already defaulted to invalid, so no action if a match
	   was not found) */
	if ( match )
	{
		*datum = read_datum;
		*datum_valid = 1;
	}

	return ( E_GEO_SUCC );
}
