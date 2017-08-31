
/*******************************************************************************
NAME			      PSUPPORT

PURPOSE	     Package-specific support routines

PROGRAM HISTORY
PROGRAMMER		DATE		REASON
----------		----		------
D. Steinwand		Nov, 1990	Original Development

*******************************************************************************/
#include <stdio.h>
#include "worgen.h"
#include "geompak.h"
#include "loclprot.h"

/* Print header with projection name 
 ----------------------------------*/
void report_type( char text[75]	/* Projection name */
     )
{
    printf( "\n%s PROJECTION PARAMETERS:\n", text );
}

/* Report the string (text) with the numeric value (val) 
 -------------------------------------------------------*/
void report_gen( double val, char *text )
{
    printf( " %s =    %f\n", text, val );
}

/* Reports a TEXT = DMS value parameter 
 ---------------------------------*/
void report_deg( double angle, char *text )
{

/* Convert from radians to degrees and report angle in decimal degrees
  -------------------------------------------------------------------*/
    angle *= 57.2957795131;
    printf( " %s = %12.7f degrees\n", text, angle );
}

/* Reports an ellipsoidal earth model 
 ------------------------------------*/
void report_spheroid( double major, double e2 )
{

    if ( e2 == 0 )
    {
	report_rad( major );
	return;
    }
    else
    {
	printf( " Semi-major axis of ellipsoid  =    %f meters\n", major );
	if ( e2 < 1 )
	    printf( " Eccentricity squared          =    %.12f\n", e2 );
	else
	    printf( " Semi-minor axis               =    %f\n", e2 );
    }
}

/* Reports a spherical earth model
 --------------------------------*/
void report_rad( double radius )
{
    printf( " Radius of Sphere              =    %f meters\n", radius );
}

/* Report projection zone code
 ----------------------------*/
void report_zone( long zone )
{
    printf( " Zone Code = %ld\n", zone );
}

/* Produce a line-feed for nicely spaced output
 ---------------------------------------------*/
void report_lf( void )
{
    printf( "\n" );
}
