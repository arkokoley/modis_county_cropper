
/******************************************************************************

NAME:                           COORD2GEO

PURPOSE:  Transform input coordinates from projection to geographic coordinates.

PROGRAM HISTORY:
VERSION  DATE   AUTHOR     CODE/CONT   REASON
-------  ----   ------     ---------   -----------------------------
  7.0    1/98   J. Willems    CSB      Implementation of Datum Transformation

PROJECT     LAS

ALGORITHM
        if input projection is geographic
          convert to radians
        if input projection is not geographic
          convert to meters
          use GCTP to do forward transformation to geo
ALGORITHM REFERENCES:

*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "shared_resample.h"

#include "tae.h"
#include "worgen.h"
#include "proj.h"
#include "cproj.h"
#include "geompak.h"

extern long ( *inv_trans[MAXPROJ + 1] ) (  );	/* inverse function pointer array */

int coord2geo( long inproj,	/* I: Input projection code */
	       long inunit,	/* I: Input projection units code */
	       double incoor[2],	/* O: Coordinate in geo and radians */
	       double *inx,	/* I: Input X projection coordinate */
	       double *iny	/* I: Input Y projection coordinate */
     )
{
    double factor;		/* conversion factor */
    double meterx;		/* Input x coordinate in meters */
    double metery;		/* Input y coordinate in meters */
    double tempx, tempy;	/* Temporary variables to hold values for inx & iny */

    long iflg = 0;		/* error flag */


/* if coordinate is in geographic, just need to convert units to radians
 ----------------------------------------------------------------------*/

    if ( inproj == GEO )
    {
	/* If DMS, convert to degress 
	   --------------------------- */
	if ( inunit == DMS )
	{
	    tempx = *inx;
	    if ( c_decdeg( &tempx, "DMS", "LON" ) != E_GEO_SUCC )
	    {
		ErrorHandler( FALSE, "coord2geo", ERROR_PROJECTION,
			      "Could not convert to Degrees" );
	    }
	    tempy = *iny;
	    if ( c_decdeg( &tempy, "DMS", "LAT" ) != E_GEO_SUCC )
	    {
		ErrorHandler( FALSE, "coord2geo", ERROR_PROJECTION,
			      "Could not convert to Degrees" );
		return ( E_GEO_FAIL );
	    }
	    /* Convert from degrees to radians */
	    incoor[0] = ( tempx * D2R );
	    incoor[1] = ( tempy * D2R );
	}
	else
	{
	    /* Use GCTP funciton to get factor to convert from radians
	       -------------------------------------------------------- */
	    iflg = untfz( inunit, RADIAN, &factor );
	    if ( iflg != 0 )
	    {
		ErrorHandler( FALSE, "coord2geo", ERROR_PROJECTION,
			      "Error returned from unit factor" );
		return ( E_GEO_FAIL );
	    }
	    /* Convert to radians by multiplying by factor
	       -------------------------------------------- */
	    incoor[0] = ( *inx * factor );
	    incoor[1] = ( *iny * factor );
	}
    }
    else
    {
	if ( inunit != METER )
	{
	    /* convert coord to meters, using GCTP function.  Assign it for input
	       to inv_trans
	       ------------------------------------------------------------------- */
	    iflg = untfz( inunit, METER, &factor );
	    if ( iflg != 0 )
	    {
		ErrorHandler( FALSE, "coord2geo", ERROR_PROJECTION,
			      "Error returned from unit factor" );
		return ( E_GEO_FAIL );
	    }
	    meterx = ( *inx * factor );
	    metery = ( *iny * factor );
	}
	else
	{
	    meterx = *inx;
	    metery = *iny;
	}

	/* Do inverse transformation to convert to geographic and radians
	   --------------------------------------------------------------- */

	iflg = inv_trans[inproj] ( meterx, metery, &incoor[0], &incoor[1] );
	if ( iflg == GCTP_ERROR )
	    ErrorHandler( FALSE, "coord2geo", ERROR_PROJECTION,
			  "Error returned from inverse transformation" );

	return ( iflg );

    }

    return ( E_GEO_SUCC );
}
