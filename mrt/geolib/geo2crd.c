
/******************************************************************************

NAME:                           GEO2COORD

PURPOSE:  Transform geographic coordinates in radians to the ouput projection 
          and units.

PROGRAM HISTORY:
VERSION  DATE   AUTHOR     CODE/CONT   REASON
-------  ----   ------     ---------   -----------------------------
  7.0    1/98   J. Willems    CSB      Implemenatation of Datum Transformation

PROJECT     LAS

ALGORITHM
	if output projection is geographic
          if outunit is DMS
	    get units to output units by multiplying by factor
	  otherwise use GCTP call to get factor 
	if out projection is not geographic
	  use GCTP to convert to output projection
	  convert to output units 
PARAMETERS:

*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "shared_resample.h"

#include "tae.h"
#include "worgen.h"
#include "proj.h"
#include "cproj.h"
#include "geompak.h"

extern long ( *for_trans[MAXPROJ + 1] ) (  );	/* forward function pointer array */

int geo2coord( long outproj,	/* I: Output projection code */
	       long outunit,	/* I: Output projection units code */
	       double outcoor[2],	/* I: Coordinates in geographic and radians */
	       double *outx,	/* O: Output X projection coordinate in output unit */
	       double *outy	/* O: Output Y projection coordinate in output unit */
     )
{
    long iflg = 0;		/* error flag */

    double factor;		/* conversion factor */
    double tempcoor[2];		/* Temporary geographic coordinate */

    if ( outproj == GEO )
    {
	/* If out unit DMS, convert to deg, then call support to convert to dms
	   --------------------------------------------------------------------- */
	if ( outunit == DMS )
	{
	    /* convert radians to degrees */
	    tempcoor[0] = ( outcoor[0] * R2D );
	    tempcoor[1] = ( outcoor[1] * R2D );
	    /* convert degrees to dms */
	    if ( c_degdms( &( tempcoor[0] ), outx, "DEG", "LON" ) != E_GEO_SUCC )
	    {
		ErrorHandler( FALSE, "geo2coord", ERROR_PROJECTION,
			      "Could not convert from Degrees to DMS" );
		return ( E_GEO_FAIL );
	    }
	    if ( c_degdms( &( tempcoor[1] ), outy, "DEG", "LAT" ) != E_GEO_SUCC )
	    {
		ErrorHandler( FALSE, "geo2coord", ERROR_PROJECTION,
			      "Could not convert from Degrees to DMS" );
		return ( E_GEO_FAIL );
	    }
	}			/* outunit is DMS */
	else
	{
	    /* Use GCTP function to get factor to convert from radians
	       -------------------------------------------------------- */
	    iflg = untfz( RADIAN, outunit, &factor );
	    if ( iflg != 0 )
	    {
		ErrorHandler( FALSE, "geo2coord", ERROR_PROJECTION,
			      "Error returned from unit factor" );
		return ( E_GEO_FAIL );
	    }
	    /* Convert to outunit by multiplying by factor
	       -------------------------------------------- */
	    *outx = ( outcoor[0] * factor );
	    *outy = ( outcoor[1] * factor );
	}			/* outunit is not DMS */
    }				/* outproj is GEO */
    else
    {
	/* If outproj is not Geo, we need to use GCTP to go to outproj
	   ------------------------------------------------------------ */
	if ( ( iflg = for_trans[outproj] ( outcoor[0], outcoor[1], outx, outy ) ) !=
	     0 )
	{
	    ErrorHandler( FALSE, "geo2coord", ERROR_PROJECTION,
			  "Error returned from forward transformation" );
	    return ( E_GEO_FAIL );
	}

	/* Convert to outunit
	   ------------------- */
	if ( outunit != METER )
	{
	    /* Convert coord from meters, using GCTP function  
	       ----------------------------------------------- */
	    iflg = untfz( METER, outunit, &factor );
	    if ( iflg != 0 )
	    {
		ErrorHandler( FALSE, "geo2coord", ERROR_PROJECTION,
			      "Error returned from unit factor" );
		return ( E_GEO_FAIL );
	    }
	    /* Convert to outunit by multiplying by factor
	       -------------------------------------------- */
	    *outx = ( *outx * factor );
	    *outy = ( *outy * factor );
	}			/* outunit is not METER */
    }				/* outproj is not GEO */
    return ( E_GEO_SUCC );
}
