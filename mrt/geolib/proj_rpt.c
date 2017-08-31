
/*******************************************************************************
NAME			      proj_report

PURPOSE	     Report projection parameter to the user

PROGRAM HISTORY
PROGRAMMER		DATE		REASON
----------		----		------
D. Steinwand		Aug, 1988	Original Development
D. Steinwand		Feb, 1989	Misc updates
D. Steinwand		Aug, 1989	Added Hammer & Robinson
S. Nelson		Jun, 1993	Added projections from the new
					"C" version of GCTP.

PROJECT     LAS

ALGORITHM 
	Determine map projection type (code)
	Report projection parameters to the desired output device
	return

ALGORITHM REFERENCES

1.  Snyder, John P., "Map Projections--A Working Manual", U.S. Geological
    Survey Professional Paper 1395 (Supersedes USGS Bulletin 1532), United
    State Government Printing Office, Washington D.C., 1987. 

2.  "Software Documentation for GCTP General Cartographic Transformation
    Package", U.S. Geological Survey National Mapping Division, May 1982.

3.  Clarie, Charles N, "State Plane Coordinates by Automatic Data Processing",
    U.S. Department of Commerce, Environmental Science Services Admin.,
    Coast and Geodetic Survey, Publication 62-4, 1973.
*******************************************************************************/

#include <stdio.h>
#include "worgen.h"
#include "proj.h"
#include "util.h"
#include "geompak.h"
#include "loclprot.h"

int MessageHandler
(
    char *module,	/* I:  module who called us (optional) */
    char *format,	/* I:  format string for the message */
    ...                 /* I:  variable argument list */
);

void proj_report( long proj,	/* Projection ID as defined in proj.h */
		  long zone,	/* Zone number for state plane and UTM */
		  double par[15],	/* Array of 15 projection parameters */
		  char *mess	/* Input/Output projection message buffer */
     )
{
    MessageHandler( NULL, mess );

    switch ( proj )
    {

	case GEO:
	    MessageHandler( NULL, "GEOGRAPHIC COORDINATES" );
	    break;

	case UTM:
	    proj_type( "UTM" );
	    proj_zone( zone );
	    break;

	case SPCS:
	    proj_type( "STATE PLANE" );
	    proj_zone( zone );
	    proj_nad( par[0] );
	    spcs_zone( zone );
	    break;

	case ALBERS:
	    proj_type( "ALBERS CONICAL EQUAL-AREA" );
	    proj_spheroid( par[0], par[1] );
	    proj_dms( par[2], "Latitude of 1st Std. Parallel" );
	    proj_dms( par[3], "Latitude of 2nd Std. Parallel" );
	    proj_dms( par[4], "Longitude of Origin          " );
	    proj_dms( par[5], "Latitude of Origin           " );
	    false_ne( par[6], par[7] );
	    break;

	case LAMCC:
	    proj_type( "LAMBERT CONFORMAL CONIC" );
	    proj_spheroid( par[0], par[1] );
	    proj_dms( par[2], "Latitude of 1st Std. Parallel" );
	    proj_dms( par[3], "Latitude of 2nd Std. Parallel" );
	    proj_dms( par[4], "Longitude of Origin          " );
	    proj_dms( par[5], "Latitude of Origin           " );
	    false_ne( par[6], par[7] );
	    break;

	case MERCAT:
	    proj_type( "MERCATOR" );
	    proj_spheroid( par[0], par[1] );
	    proj_dms( par[5], "Latitude of True Scale       " );
	    proj_dms( par[4], "Central Longitude            " );
	    false_ne( par[6], par[7] );
	    break;

	case PS:
	    proj_type( "POLAR STEREOGRAPHIC" );
	    proj_spheroid( par[0], par[1] );
	    proj_dms( par[4], "Longitude of Y-Axis          " );
	    proj_dms( par[5], "Latitude of True Scale       " );
	    false_ne( par[6], par[7] );
	    break;

	case POLYC:
	    proj_type( "POLYCONIC" );
	    proj_spheroid( par[0], par[1] );
	    proj_dms( par[4], "Longitude of Origin          " );
	    proj_dms( par[5], "Latitude of Origin           " );
	    false_ne( par[6], par[7] );
	    break;

	case EQUIDC:
	    proj_type( "EQUIDISTANT CONIC" );
	    proj_spheroid( par[0], par[1] );
	    if ( par[8] == 0.0 )
		proj_dms( par[2], "Latitude of Std. Parallel    " );
	    else
	    {
		proj_dms( par[2], "Latitude of 1st Std. Parallel" );
		proj_dms( par[3], "Latitude of 2nd Std. Parallel" );
	    }
	    proj_dms( par[4], "Longitude of Origin          " );
	    proj_dms( par[5], "Latitude of Origin           " );
	    false_ne( par[6], par[7] );
	    break;

	case TM:
	    proj_type( "TRANSVERSE MERCATOR" );
	    proj_spheroid( par[0], par[1] );
	    proj_gen( par[2], "Scale Factor at C. Meridian  " );
	    proj_dms( par[4], "Longitude of C. Meridian     " );
	    proj_dms( par[5], "Latitude of Origin           " );
	    false_ne( par[6], par[7] );
	    break;

	case STEREO:
	    proj_type( "STEREOGRAPHIC" );
	    proj_rad( par[0] );
	    proj_dms( par[4], "Longitude of Center          " );
	    proj_dms( par[5], "Latitude  of Center          " );
	    false_ne( par[6], par[7] );
	    break;

	case LAMAZ:
	    proj_type( "LAMBERT AZIMUTHAL EQUAL-AREA" );
	    proj_rad( par[0] );
	    proj_dms( par[4], "Longitude of Center          " );
	    proj_dms( par[5], "Latitude  of Center          " );
	    false_ne( par[6], par[7] );
	    break;

	case AZMEQD:
	    proj_type( "AZIMUTHAL EQUIDISTANT" );
	    proj_rad( par[0] );
	    proj_dms( par[4], "Longitude of Center          " );
	    proj_dms( par[5], "Latitude  of Center          " );
	    false_ne( par[6], par[7] );
	    break;

	case GNOMON:
	    proj_type( "GNOMONIC" );
	    proj_rad( par[0] );
	    proj_dms( par[4], "Longitude of Center          " );
	    proj_dms( par[5], "Latitude  of Center          " );
	    false_ne( par[6], par[7] );
	    break;

	case ORTHO:
	    proj_type( "ORTHOGRAPHIC" );
	    proj_rad( par[0] );
	    proj_dms( par[4], "Longitude of Center          " );
	    proj_dms( par[5], "Latitude  of Center          " );
	    false_ne( par[6], par[7] );
	    break;

	case GVNSP:
	    proj_type( "GENERAL VERT. NEAR-SIDE PERSPECTIVE" );
	    proj_rad( par[0] );
	    proj_gen( par[2], "Height of Perspective Point  " );
	    proj_dms( par[4], "Longitude of Center          " );
	    proj_dms( par[5], "Latitude  of Center          " );
	    false_ne( par[6], par[7] );
	    break;

	case SNSOID:
	    proj_type( "SINUSOIDAL" );
	    proj_rad( par[0] );
	    proj_dms( par[4], "Longitude of C. Meridian     " );
	    false_ne( par[6], par[7] );
	    break;

	case EQRECT:
	    proj_type( "EQUIRECTANGULAR" );
	    proj_rad( par[0] );
	    proj_dms( par[5], "Latitude of True Scale       " );
	    proj_dms( par[4], "Longitude of C. Meridian     " );
	    false_ne( par[6], par[7] );
	    break;

	case MILLER:
	    proj_type( "MILLER CYLINDRICAL" );
	    proj_rad( par[0] );
	    proj_dms( par[4], "Longitude of C. Meridian     " );
	    false_ne( par[6], par[7] );
	    break;

	case VGRINT:
	    proj_type( "VAN DER GRINTEN I" );
	    proj_rad( par[0] );
	    proj_dms( par[4], "Longitude of C. Meridian     " );
	    proj_dms( par[5], "Latitude of Origin           " );
	    false_ne( par[6], par[7] );
	    break;

	case HOM:
	    proj_type( "OBLIQUE MERCATOR (HOTINE)" );
	    proj_spheroid( par[0], par[1] );
	    proj_gen( par[2], "Scale at Center              " );
	    proj_dms( par[5], "Latitude of Origin           " );
	    if ( par[12] == 0.0 )
	    {
		proj_dms( par[8], "Longitude of 1st Point       " );
		proj_dms( par[9], "Latitude of 1st Point        " );
		proj_dms( par[10], "Longitude of 2nd Point       " );
		proj_dms( par[11], "Latitude of 2nd Point        " );
	    }
	    else
	    {
		proj_dms( par[3], "Azimuth of Central Line      " );
		proj_dms( par[4], "Longitude of Origin         " );
	    }
	    false_ne( par[6], par[7] );
	    break;

	case ROBIN:
	    proj_type( "ROBINSON" );
	    proj_rad( par[0] );
	    proj_dms( par[4], "Longitude of C. Meridian     " );
	    false_ne( par[6], par[7] );
	    break;

	case SOM:
	    proj_type( "SPACE OBLIQUE MERCATOR" );
	    proj_spheroid( par[0], par[1] );
	    if ( par[12] == 0.0 )
	    {
		proj_dms( par[3], "Inclination of Orbit         " );
		proj_dms( par[4], "Longitude of Ascending Orbit " );
		proj_gen( par[8], "Period of Sat. Revolution    " );
		proj_gen( par[9], "Landsat Ratio                " );
		proj_gen( par[10], "Landsat End of Path Flag     " );
	    }
	    else
	    {
		proj_gen( par[2], "Satellite Number             " );
		proj_gen( par[3], "Path Number                  " );
	    }
	    false_ne( par[6], par[7] );
	    break;

	case ALASKA:
	    proj_type( "ALASKA CONFORMAL" );
	    proj_spheroid( par[0], par[1] );
	    false_ne( par[6], par[7] );
	    break;

	case GOOD:
	    proj_type( "GOODE'S HOMOLOSINE EQUAL AREA" );
	    proj_rad( par[0] );
	    break;

	case MOLL:
	    proj_type( "MOLLWEIDE" );
	    proj_rad( par[0] );
	    proj_dms( par[4], "Longitude of C. Meridian     " );
	    false_ne( par[6], par[7] );
	    break;

	case IMOLL:
	    proj_type( "INTERRUPTED MOLLWEIDE EQUAL AREA" );
	    proj_rad( par[0] );
	    break;

	case HAMMER:
	    proj_type( "HAMMER" );
	    proj_rad( par[0] );
	    proj_dms( par[4], "Longitude of C. Meridian     " );
	    false_ne( par[6], par[7] );
	    break;

	case WAGIV:
	    proj_type( "WAGNER IV" );
	    proj_rad( par[0] );
	    proj_dms( par[4], "Longitude of C. Meridian     " );
	    false_ne( par[6], par[7] );
	    break;

	case WAGVII:
	    proj_type( "WAGNER VII" );
	    proj_rad( par[0] );
	    proj_dms( par[4], "Longitude of C. Meridian     " );
	    false_ne( par[6], par[7] );
	    break;

	case OBEQA:
	    proj_type( "OBLATED EQUAL-AREA" );
	    proj_rad( par[0] );
	    proj_dms( par[4], "Longitude of Center          " );
	    proj_dms( par[5], "Latitude  of Center          " );
	    proj_gen( par[2], "Oval Shape Parameter m       " );
	    proj_gen( par[3], "Oval Shape Parameter n       " );
	    proj_dms( par[8], "Oval Rotation Angle          " );
	    false_ne( par[6], par[7] );
	    break;

        case ISINUS:
            proj_type( "INTEGERIZED SINUSOIDAL" );
            proj_rad( par[0] );
            proj_dms( par[4], "Longitude of C. Meridian     " );
            false_ne( par[6], par[7] );
            proj_gen( par[8], "Number of Latitudinal Zones  " );
            proj_gen( par[10], "Right Justify Columns Flag   " );
            break;

    }
}

/* Print header with projection name 
 ----------------------------------*/
void proj_type( char text[75]	/* Projection name */
     )
{
    char outmsg[100];		/* buffer for output message */

    sprintf( outmsg, "%s PROJECTION PARAMETERS:", text );
    MessageHandler( NULL, outmsg );
}

/* Report the string (text) with the numeric value (val) 
 -------------------------------------------------------*/
void proj_gen( double val, char *text )
{
    char outmsg[100];		/* buffer for output message */

    sprintf( outmsg, " %s = %f", text, val );
    MessageHandler( NULL, outmsg );
}

/* Reports a TEXT = DMS value parameter 
 ---------------------------------*/
void proj_dms( double angle, char *text )
{
    char outmsg[100];		/* buffer for output message */
    long deg, min;		/* Degrees & minutes portion of angle */
    double sec;			/* Seconds portion of angle */
    long sign;			/* Sign of angle */

/* extract the degress, minutes, and seconds portions of angle
  -----------------------------------------------------------*/
    if ( angle < 0 )
	sign = -1;
    else
	sign = 1;
    angle *= sign;
    deg = ( long ) ( angle / 1000000 );
    angle -= ( deg * 1000000 );
    min = ( long ) ( angle / 1000 );
    sec = angle - ( min * 1000 );
    deg *= sign;

    sprintf( outmsg, " %s = %ld %ld %f", text, deg, min, sec );
    MessageHandler( NULL, outmsg );
}

/* Reports an ellipsoidal earth model 
 ------------------------------------*/
void proj_spheroid( double major, double e2 )
{
    char outmsg[100];		/* buffer for output message */

    if ( e2 == 0 )
    {
	proj_rad( major );
	return;
    }
    else
    {
	sprintf( outmsg, " Semi-major Axis of Ellipsoid  = %f meters", major );
	MessageHandler( NULL, outmsg );
	if ( e2 < 1 )
	    sprintf( outmsg, " Eccentricity Squared          = %.12f", e2 );
	else
	    sprintf( outmsg, " Semi-minor Axis               = %f", e2 );
	MessageHandler( NULL, outmsg );
    }
}

/* Reports a spherical earth model
 --------------------------------*/
void proj_rad( double radius )
{
    char outmsg[100];		/* buffer for output message */

    sprintf( outmsg, " Radius of Sphere              = %f meters", radius );
    MessageHandler( NULL, outmsg );
}

/* Reports false northings and false eastings
 -------------------------------------------*/
void false_ne( double east, double north )
{
    char outmsg[100];		/* buffer for output message */

    sprintf( outmsg, " False Easting                 = %f meters", east );
    MessageHandler( NULL, outmsg );
    sprintf( outmsg, " False Northing                = %f meters", north );
    MessageHandler( NULL, outmsg );
}

/* Report projection zone code
 ----------------------------*/
void proj_zone( long zone )
{
    char outmsg[100];		/* buffer for output message */

    sprintf( outmsg, " Zone Code = %ld", zone );
    MessageHandler( NULL, outmsg );
}

/* Report the NAD value
 ---------------------*/
void proj_nad( double val )
{
    char outmsg[100];		/* buffer for output message */
    long nadval;

    if ( val == 0 )
	nadval = 27;
    else
	nadval = 83;
    sprintf( outmsg, " Datum     = NAD %2ld", nadval );
    MessageHandler( NULL, outmsg );
}
