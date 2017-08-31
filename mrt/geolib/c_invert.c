
/*****************************************************************************
NAME				 INVERT 

PURPOSE    Invert user entered coefficients

PROGRAM HISTORY
VERSION    PROGRAMMER	   DATE     CODE/CONT	REASON
-------    ----------      ----     ---------   ------
  1.0	   R. White        4/82        CSC      Original Development
  4.0      S. Kondal       7/84        SAR     
  5.0      D. Steinwand   10/88        EDC      LAS 5.0 development,
						C conversion
  5.1      D. Steinwand    4/89        EDC      Moved to geolib supports
  5.2      D. Etrheim      7/90        EDC	Standardized error message 
						handling
  5.3	   S. Nelson	   9/93	       EDC	Changed EPSLN in test of
						determinant too near zero.
						Original value of EPSLN was .01
						which caused problems with
						lat/lon values.

PROJECT			LAS

ALGORITHM DESCRIPTION
	Find the determinate
	Find the maximum coefficient value
	If the determinate is too near zero, abort
	Calculate inverse mapping coefficients
*****************************************************************************/
#include <stdio.h>
#include <math.h>
#include "shared_resample.h"
#include "tae.h"
#include "worgen.h"
#include "geompak.h"

#define EPSLNX 0.000001
#define CMAX(A, B)	((A) > (B) ? (A) : (B))	/* assign maximun of a and b */

int c_invert( double incoef[6],	/* Transformation coefficients--input */
	      double coef[6]	/* Inverted transformation coefficients */
     )
{
    double det;			/* Determinate */
    double maximum;		/* Maximum input coefficient value */
    double max1, max2;		/* Temporary variables for max calculation */

/* Find the determinate 
 ---------------------*/
    det = ( incoef[3] * incoef[1] ) - ( incoef[4] * incoef[0] );

/* Find the maximum coefficent value 
 ----------------------------------*/
    max1 = CMAX( fabs( incoef[0] ), fabs( incoef[1] ) );
    max2 = CMAX( fabs( incoef[3] ), fabs( incoef[4] ) );
    maximum = CMAX( max1, max2 );

/* Is the determinant of the coefficients too near zero?
 ----------------------------------------------------------------------*/
    if ( fabs( det ) < ( EPSLNX * maximum ) )
    {
	ErrorHandler( FALSE, "c_invert", ERROR_PROJECTION_MATH,
		      "Determinant too near zero" );
	return ( E_GEO_FAIL );
    }

/* Compute inverse coefficients 
 -----------------------------*/
    coef[0] = -( incoef[4] ) / det;
    coef[1] = incoef[1] / det;
    coef[2] = ( ( incoef[4] * incoef[2] ) - ( incoef[1] * incoef[5] ) ) / det;
    coef[3] = incoef[3] / det;
    coef[4] = -( incoef[0] ) / det;
    coef[5] = ( ( incoef[0] * incoef[5] ) - ( incoef[3] * incoef[2] ) ) / det;

    return ( E_GEO_SUCC );
}
