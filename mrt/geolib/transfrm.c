
/******************************************************************************

NAME:                           TRANSFORM

PURPOSE:  Transform coordinates from one datum to another by appropriate method.

PROGRAM HISTORY:
VERSION  DATE   AUTHOR     CODE/CONT   REASON
-------  ----   ------     ---------   -----------------------------
  1.0    ?????  J. Story     USGS      Datum conversion subroutines (FORTRAN)
  5.0    10/89  D. Steinwand  CSB      LAS 5.0 C conversion
  7.0    1/98   J. Willems    CSB      Implemenatation of Datum Transformation

PARAMETERS:

       INCOOR:     input point longitude-latitude in radians
       INDTMPARM:  input datum information:
       OUTDTMPARM: output datum information:
       DTMFLAG:    Flag specifying type of conversion
       OUTCOOR:    output point longitude-latitude in radians

ALGORITHM:
	If no transformation
	  Exit
        If molodensky
	  call molodtrans
        If NAD 27 to NAD 83 or NAD 83 to NAD 27 conversion
	  convert coordinate to degrees for NADCON
 	  call NADCON
	  if NADCON errored because the point was out of range
	    call molodtrans
        If NAD 27 to and other datum
	  convert coordinate to degrees for NADCON
          if NADCON is successful for converting from NAD27 to NAD83
	    fill parameter array to convert from NAD 83
            call molodensky to convert from NAD 83 to output datum
          if NADCON errored because the point was out of range
            call molodtrans to transfer from NAD27 to output datum
        If any datum to NAD 27 
          fill parameter array to convert to NAD 83
          call molodensky to convert to NAD 83 from input datum
          convert coordinate to degrees for NADCON
          call NADCON to convert from NAD 83 to NAD 27
          if NADCON errored because the point was out of range
            call molodtrans to convert from NAD 83 to NAD 27
           
ALGORITHM REFERENCES:
       See MOLODENSKY
*****************************************************************************/

#include <string.h>
#include "shared_resample.h"

#include "tae.h"
#include "worgen.h"
#include "datum.h"
#include "cproj.h"
#include "geompak.h"
#include "loclprot.h"

#define WGS84 317		/* Datum number for WGS 84 */
#define WGS84SMAJ 6378137	/* Semi-major axis value for WGS 84 */
#define WGS84SMIN 6356752.3142	/* Semi-minor axis value for WGS 84 */
#define NAD83SMAJ 6378137	/* Semi-major axis value for NAD 83 */
#define NAD83SMIN 6356752.31414	/* Semi-minor axis value for NAD 83 */
#define NAD27to83 1		/* Conversion flag for NADCON: NAD27 to NAD83 */
#define NAD83to27 2		/* Conversion flag for NADCON: NAD83 to NAD27 */

/*
** LOCAL PROTOTYPES
*/

static int molodensky( double inrads[2],	/* input point longitude-latitude in radians */
		       double userpars[7],	/* user specified transformation parameters */
		       double otrads[2]	/* output point longitude-latitude in radians */
     );

static void bwolf
    ( double inrad[2],

      double abin[2], double pars[7], double otrad[2], double abot[2] );

static void llhxyz

    ( double *x,
      double *y, double *z, double a, double b, double phi, double lam, double h );

static void xyzllh

    ( double x,
      double y, double z, double *a, double *b, double *phi, double *lam, double h );

static int molodtrans( struct DATUMDEF indtmparm,	/* I: Input datum parameter array */
		       struct DATUMDEF outdtmparm,	/* I: Output datum parameter array */
		       double incoor[2],	/* I: Input coordinates to be transformed */
		       double outcoor[2]	/* O: Output coordinates after molodensky */
     );

/*------------------------------------------------------------------------*/

int transform( double incoor[2],           /* I: input point in radians */
	       struct DATUMDEF indtmparm,  /* I: input datum information */
	       struct DATUMDEF outdtmparm, /* I: output datum information */
	       long datumflag,        /* I: Flag specifying which conversion */
	       double outcoor[2]      /* O: output point in radians */
     )
{
    long ndconflg = 0;		/* Conversion flag for nadcon */

    char errkey[CMLEN];		/* Error key returned from nadcon */
    char message[CMLEN];	/* Error message returned from nadcon */

    double indeg[2];		/* Temporary input degree coordinates for nadcon */
    double outdeg[2];		/* Temporary output degree coordinates for nadcon */
    double inrads[2];		/* Temporary input radian coordinates for nadcon */
    double outrads[2];		/* Temporary output radian coordinates for nadcon */

    struct DATUMDEF nad83parm;	/* parameter array for molodensky after NADCON */

/* Call transformation functions depending on type of transformation
  -----------------------------------------------------------------*/
    if ( datumflag == NO_TRANS )
    {
	ErrorHandler( FALSE, "transform", ERROR_PROJECTION,
		      "Datum flag does not specify conversion" );
	return ( E_GEO_SUCC );
    }
    else if ( datumflag == MOLODENSKY )
    {
	if ( molodtrans( indtmparm, outdtmparm, incoor, outcoor ) != E_GEO_SUCC )
	    return ( E_GEO_FAIL );
    }
    else if ( ( datumflag == NAD27_TO_83 ) || ( datumflag == NAD83_TO_27 ) )
    {
	if ( datumflag == NAD27_TO_83 )
	    ndconflg = NAD27to83;
	else
	    ndconflg = NAD83to27;

	/* NADCON needs values in degrees
	   ------------------------------- */
	indeg[0] = incoor[0] * R2D;
	indeg[1] = incoor[1] * R2D;

	/* Call NADCON to transfer to NAD83
	   -------------------------------- */
	if ( ( ndcon( indeg, &ndconflg, outdeg, errkey, message ) )
                                                                   == E_GEO_SUCC )
	{
	    outcoor[0] = outdeg[0] * D2R;
	    outcoor[1] = outdeg[1] * D2R;
	}
	else
	{
	    /* If point is out of nadcon's range, use molodensky transformations
	       ------------------------------------------------------------------ */
	    if ( strcmp( errkey, "nadcon-range" ) )
	    {
		if ( molodtrans( indtmparm, outdtmparm, incoor, outcoor )
                                                                   != E_GEO_SUCC )
		    return ( E_GEO_FAIL );
	    }
	    else
	    {
		ErrorHandler( FALSE, "transform", ERROR_PROJECTION, message );
		return ( E_GEO_FAIL );
	    }
	}
    }
    else if ( datumflag == NAD27_TO_GEN )
    {
	/* Use nadcon for 27 to 83, and then molodensky for 83 to out datum
	   ---------------------------------------------------------------- */
	ndconflg = NAD27to83;
	indeg[0] = incoor[0] * R2D;
	indeg[1] = incoor[1] * R2D;
	if ( ( ndcon( indeg, &ndconflg, outdeg, errkey, message ) )
                                                                  == E_GEO_SUCC )
	{
	    /* Set values to be passed to molodensky transformations
	       ------------------------------------------------------ */
	    inrads[0] = outdeg[0] * D2R;
	    inrads[1] = outdeg[1] * D2R;
	    nad83parm.datumnum = NAD83;
	    nad83parm.smajor = NAD83SMAJ;
	    nad83parm.sminor = NAD83SMIN;
	    nad83parm.xshift = 0;
	    nad83parm.yshift = 0;
	    nad83parm.zshift = 0;
	    if ( molodtrans( nad83parm, outdtmparm, inrads, outcoor )
                                                                 != E_GEO_SUCC )
		return ( E_GEO_FAIL );
	}
	else
	{
	    /* If point is out of nadcon's range, use molodensky transformations
	       ------------------------------------------------------------------ */
	    if ( strcmp( errkey, "nadcon-range" ) )
	    {
		if ( molodtrans( indtmparm, outdtmparm, incoor, outcoor )
                                                                 != E_GEO_SUCC )
		    return ( E_GEO_FAIL );
	    }
	    else
	    {
		ErrorHandler( FALSE, "tranform", ERROR_PROJECTION, message );
		return ( E_GEO_FAIL );
	    }
	}
    }
    else if ( datumflag == GEN_TO_27 )
    {
	/* Use molodensky for out datum to 83, and nadcon for 83 to 27 
	   ----------------------------------------------------------- */
	nad83parm.datumnum = NAD83;
	nad83parm.smajor = NAD83SMAJ;
	nad83parm.sminor = NAD83SMIN;
	nad83parm.xshift = 0;
	nad83parm.yshift = 0;
	nad83parm.zshift = 0;
	if ( molodtrans( indtmparm, nad83parm, incoor, outrads ) != E_GEO_SUCC )
	    return ( E_GEO_FAIL );

	ndconflg = NAD83to27;
	indeg[0] = outrads[0] * R2D;
	indeg[1] = outrads[1] * R2D;
	if ( ( ndcon( indeg, &ndconflg, outdeg, errkey, message ) )
                                                                  == E_GEO_SUCC )
	{
	    outcoor[0] = outdeg[0] * D2R;
	    outcoor[1] = outdeg[1] * D2R;
	}
	else
	{
	    /* If point is out of nadcon's range, use molodensky transformations
	       ------------------------------------------------------------------ */
	    if ( strcmp( errkey, "nadcon-range" ) )
	    {
		if ( molodtrans( nad83parm, outdtmparm, outrads, outcoor ) !=
		     E_GEO_SUCC )
		    return ( E_GEO_FAIL );
	    }
	    else
	    {
		ErrorHandler( FALSE, "transform", ERROR_PROJECTION, message );
		return ( E_GEO_FAIL );
	    }
	}
    }
    else
    {
	ErrorHandler( FALSE, "transform", ERROR_PROJECTION,
		      "Datum flag does not represent a conversion" );
	return ( E_GEO_FAIL );
    }
    return ( E_GEO_SUCC );
}

/******************************************************************************

NAME:                           molodtrans

PURPOSE:  Initializes parameters and calls molodensky transformation.

PARAMETERS: 	INDTMPARM
		OUTDTMPARM
		INCOOR
		OUTCOOR

*****************************************************************************/

static int molodtrans( struct DATUMDEF indtmparm,	/* I: Input datum parameter array */
		       struct DATUMDEF outdtmparm,	/* I: Output datum parameter array */
		       double incoor[2],	/* I: Input coordinates to be transformed */
		       double outcoor[2]	/* O: Output coordinates after molodensky */
     )
{
    long wgsout = FALSE;	/* Flag to specify if WGS 84 is output datum */
    long wgsin = FALSE;		/* Flag to specify if WGS 84 is input datum */

    double todtmparm[7];	/* Datum parameters for to WGS 84 */
    double fromdtmparm[7];	/* Datum parameters for from WGS 84 */
    double tempcoor[2];		/* Temporary coordinate for WSG conversion */

    if ( indtmparm.datumnum == WGS84 )
	wgsin = TRUE;
    else if ( outdtmparm.datumnum == WGS84 )
	wgsout = TRUE;

/* Convert to WGS84 
 -----------------*/
    if ( !wgsin )
    {
	/* Set Shifts */
	todtmparm[0] = indtmparm.xshift;
	todtmparm[1] = indtmparm.yshift;
	todtmparm[2] = indtmparm.zshift;
	todtmparm[3] = indtmparm.smajor;
	todtmparm[4] = indtmparm.sminor;
	todtmparm[5] = WGS84SMAJ;
	todtmparm[6] = WGS84SMIN;
	if ( molodensky( incoor, todtmparm, outcoor ) != E_GEO_SUCC )
	{
	    ErrorHandler( FALSE, "molodtrans", ERROR_PROJECTION,
			  "Error returned from datum conversion" );
	    return ( E_GEO_FAIL );
	}
    }
    else
    {
	outcoor[0] = incoor[0];
	outcoor[1] = incoor[1];
    }

/* Convert from WGS84 to Output datum 
 -----------------------------------*/
    if ( !wgsout )
    {
	tempcoor[0] = outcoor[0];
	tempcoor[1] = outcoor[1];
	fromdtmparm[0] = -( outdtmparm.xshift );
	fromdtmparm[1] = -( outdtmparm.yshift );
	fromdtmparm[2] = -( outdtmparm.zshift );
	fromdtmparm[3] = WGS84SMAJ;
	fromdtmparm[4] = WGS84SMIN;
	fromdtmparm[5] = outdtmparm.smajor;
	fromdtmparm[6] = outdtmparm.sminor;
	if ( molodensky( tempcoor, fromdtmparm, outcoor ) != E_GEO_SUCC )
	{
	    ErrorHandler( FALSE, "molodtrans", ERROR_PROJECTION,
			  "Error returned from datum conversion" );
	    return ( E_GEO_FAIL );
	}
    }
    return ( E_GEO_SUCC );
}

/******************************************************************************

NAME:				molodensky

PURPOSE:  Performs molodensky transformation using user specified shift 
	  and ellipsoid parameters.

PARAMETERS:
 
       INRADS:   input point longitude-latitude in radians
       USERPARS: user specified transformation parameters:
                 element 0 = delta X, Molodensky shift
                    "    1 =   "   Y,      "       "
                    "    2 =   "   Z,      "       "
                    "    3 = a1, semi-major axis of input ellipsoid
                    "    4 = b1, semi-minor  "   "    "       "
                    "    5 = a2, semi-major axis of output ellipsoid
                    "    6 = b2, semi-minor  "   "    "       "
       OTRADS:   output point longitude-latitude in radians

ALGORITHM REFERENCES:
	MOLODENSKY was derrived from DATUMCHG which was written by Dan 
	Steinwand based on routines written by Jim Story, USGS, Reston, VA
*****************************************************************************/

static int molodensky( double inrads[2],	/* input point longitude-latitude in radians */
		       double userpars[7],	/* user specified transformation parameters */
		       double otrads[2]	/* output point longitude-latitude in radians */
     )
{
    double ellipse_in[2];	/* Input ellipse semi major and minor */
    double ellipse_out[2];	/* Output ellipse semi major and minor */
    double bursapar[7];		/* Parameters for the Bursa-Wolf trans. */
    long i;			/* Increment for loop */

    for ( i = 0; i < 7; i++ )
	bursapar[i] = 0.0;

/* Check input datum and load parameters
  -------------------------------------*/
    if ( ( userpars[3] < 6.0 ) || ( userpars[4] < 6.0 ) )
    {
	return ( E_GEO_FAIL );
    }
    ellipse_in[0] = userpars[3];
    ellipse_in[1] = userpars[4];

/* Check output datum and load parameters
  -------------------------------------*/
    if ( ( userpars[5] < 6.0 ) || ( userpars[6] < 6.0 ) )
    {
	return ( E_GEO_FAIL );
    }
    ellipse_out[0] = userpars[5];
    ellipse_out[1] = userpars[6];

/* Compute molodensky transformation
  ---------------------------------*/
    bursapar[0] = userpars[0];
    bursapar[1] = userpars[1];
    bursapar[2] = userpars[2];
    bwolf( inrads, ellipse_in, bursapar, otrads, ellipse_out );
    return ( E_GEO_SUCC );
}

/******************************************************************************

NAME:				bwolf

PURPOSE:  Applies Bursa-Wolf transformation
 
PARAMETERS:
 
       INRAD:  INPUT LONG - LAT IN RADIANS
       ABIN:    INPUT ELLIPSOID PARAMETERS A - B
       PARS:    DX  IN METERS
                DY  IN METERS
                DZ  IN METERS
                WX  IN RADIANS
                WY  IN RADIANS
                WZ  IN RADIANS
                DS  DIMENSIONLESS
       OTRAD:  OUTPUT LONG - LAT IN RADIANS
       ABOT:    OUTPUT ELLIPSOID PARAMETERS A - B

ALGORITHM REFERENCES:
       BWOLF was written by Dan Steinwand as part of DATUMCHG.
*****************************************************************************/
static void bwolf
    ( double inrad[2],
      double abin[2], double pars[7], double otrad[2], double abot[2] )
{
    double h, x, y, z, scale, xp, yp, zp;

/* Set ellipsoid height to zero
  ----------------------------*/
    h = 0.0;

/* Convert geodetic to geocentric
  ------------------------------*/
    llhxyz( &x, &y, &z, abin[0], abin[1], inrad[1], inrad[0], h );

/* Transform geocentric coordinates
  --------------------------------*/
    scale = 1.0 + pars[6];
    xp = ( x - pars[5] * y + pars[4] * z ) * scale + pars[0];
    yp = ( y + pars[5] * x - pars[3] * z ) * scale + pars[1];
    zp = ( z - pars[4] * x + pars[3] * y ) * scale + pars[2];

/* Convert geocentric to geodetic
  ------------------------------*/
    xyzllh( xp, yp, zp, &( abot[0] ), &( abot[1] ), &( otrad[1] ), &( otrad[0] ),
	    h );

}

static void llhxyz
    ( double *x,
      double *y, double *z, double a, double b, double phi, double lam, double h )
{
    double e2;
    double sinp, cosp, sinl, cosl;
    double rn;

    e2 = 1.0 - b * b / a / a;

    sinp = sin( phi );
    cosp = cos( phi );
    sinl = sin( lam );
    cosl = cos( lam );

/* Radius of curvature in the prime vertical
  -----------------------------------------*/
    rn = a / sqrt( 1.0 - e2 * sinp * sinp );

    *x = ( rn + h ) * cosp * cosl;
    *y = ( rn + h ) * cosp * sinl;
    *z = ( rn * ( 1.0 - e2 ) + h ) * sinp;
}

static void xyzllh
    ( double x,
      double y, double z, double *a, double *b, double *phi, double *lam, double h )
{
    double e2;
    double r;
    double sinp, cosp;
    double den, den2;
    double drdp, dzdp;
    double rc, zc;
    double dh, dphi;
    double scalar;

    e2 = 1.0 - ( *b * *b / *a / *a );

/* Compute longitude
  -----------------*/
    *lam = atan2( y, x );

    r = sqrt( x * x + y * y );

/* Initial approximations for linearized simultaneous equation solution.
  ---------------------------------------------------------------------*/
    h = 0.0;
    *phi = atan( z / ( 1.0 - e2 ) / r );

/* Iterative loop
  --------------*/
    for ( ;; )
    {
	sinp = sin( *phi );
	cosp = cos( *phi );
	den2 = 1.0 - e2 * sinp * sinp;
	den = sqrt( den2 );

/* Partial derivatives
  -------------------*/
	drdp = *a * sinp / den * ( e2 * cosp * cosp / den2 - 1.0 ) - h * sinp;
	dzdp =
	    *a * ( 1.0 - e2 ) * cosp / den * ( sinp * sinp / den2 + 1.0 ) + h * cosp;
	rc = *a * cosp / den + h * cosp;
	zc = *a * ( 1.0 - e2 ) * sinp / den + h * sinp;
	scalar = 1.0 / ( drdp * sinp - cosp * dzdp );

	dphi = scalar * ( sinp * ( r - rc ) - cosp * ( z - zc ) );
	dh = scalar * ( drdp * ( z - zc ) - dzdp * ( r - rc ) );

	*phi = *phi + dphi;
	h = h + dh;

/* Convergence criteria
  --------------------*/
	if ( fabs( dh ) <= 0.002 )
	    break;
	if ( fabs( dphi ) <= 1.0e-10 )
	    break;
    }
}
