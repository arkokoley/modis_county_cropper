
/*******************************************************************************
NAME			      print_proj

PURPOSE	     Report projection parameters to the user

PROGRAM HISTORY
PROGRAMMER		DATE		REASON
----------		----		------
D. Steinwand		Aug, 1988	Original Development
D. Steinwand		Feb, 1989	Misc updates
D. Steinwand		Aug, 1989	Added Hammer & Robinson
S. Nelson		Jun, 1993	Added projections from the new
					"C" version of GCTP.
G. Schmidt              Nov, 2002       Added Albers Equal Area
G. Schmidt              July, 2003      Added equirectangular

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
#include "shared_resample.h"
#include "proj.h"

void print_proj_type( char text[75]	/* Projection name */);
void print_proj_gen( double val, char *text );
void print_proj_dms( double angle, char *text );
void print_proj_spheroid( double major, double e2 );
void print_proj_rad( double radius );
void print_false_ne( double east, double north );
void print_proj_zone( long zone );
void print_proj_nad( double val );

void print_proj
(
    long proj,		/* Projection ID as defined in shared_resample.h */
    long zone,		/* Zone number for state plane and UTM */
    double par[15] 	/* Array of 15 projection parameters */
)
{
    char msg[100];

    switch ( proj )
    {
	case GEO:
	    MessageHandler( NULL, "GEOGRAPHIC COORDINATES: None" );
	    break;

	case UTM:
	    print_proj_type( "UTM" );
	    print_proj_zone( zone );
	    break;

	case ALBERS:
	    print_proj_type( "ALBERS EQUAL AREA" );
	    print_proj_spheroid( par[0], par[1] );
	    print_proj_dms( par[2], "Latitude of 1st Std. Parallel" );
	    print_proj_dms( par[3], "Latitude of 2nd Std. Parallel" );
	    print_proj_dms( par[4], "Longitude of Origin          " );
	    print_proj_dms( par[5], "Latitude of Origin           " );
	    print_false_ne( par[6], par[7] );
	    break;

        case EQRECT:
            print_proj_type( "EQUIRECTANGULAR" );
            print_proj_rad( par[0] );
            print_proj_dms( par[4], "Latitude of 1st Std. Parallel" );
            print_proj_dms( par[5], "Latitude of True Scale       " );
            print_false_ne( par[6], par[7] );
            break;

	case LAMCC:
	    print_proj_type( "LAMBERT CONFORMAL CONIC" );
	    print_proj_spheroid( par[0], par[1] );
	    print_proj_dms( par[2], "Latitude of 1st Std. Parallel" );
	    print_proj_dms( par[3], "Latitude of 2nd Std. Parallel" );
	    print_proj_dms( par[4], "Longitude of Origin          " );
	    print_proj_dms( par[5], "Latitude of Origin           " );
	    print_false_ne( par[6], par[7] );
	    break;

        case MERCAT:
            print_proj_type( "MERCATOR" );
            print_proj_spheroid( par[0], par[1] );
            print_proj_dms( par[4], "Latitude of 1st Std. Parallel" );
            print_proj_dms( par[5], "Latitude of True Scale       " );
            print_false_ne( par[6], par[7] );
            break;

	case PS:
	    print_proj_type( "POLAR STEREOGRAPHIC" );
	    print_proj_spheroid( par[0], par[1] );
	    print_proj_dms( par[4], "Longitude of Y-Axis          " );
	    print_proj_dms( par[5], "Latitude of True Scale       " );
	    print_false_ne( par[6], par[7] );
	    break;

	case TM:
	    print_proj_type( "TRANSVERSE MERCATOR" );
	    print_proj_spheroid( par[0], par[1] );
	    print_proj_gen( par[2], "Scale Factor at C. Meridian  " );
	    print_proj_dms( par[4], "Longitude of C. Meridian     " );
	    print_proj_dms( par[5], "Latitude of Origin           " );
	    print_false_ne( par[6], par[7] );
	    break;

	case LAMAZ:
	    print_proj_type( "LAMBERT AZIMUTHAL EQUAL-AREA" );
	    print_proj_rad( par[0] );
	    print_proj_dms( par[4], "Longitude of Center          " );
	    print_proj_dms( par[5], "Latitude  of Center          " );
	    print_false_ne( par[6], par[7] );
	    break;

	case SNSOID:
	    print_proj_type( "SINUSOIDAL" );
	    print_proj_rad( par[0] );
	    print_proj_dms( par[4], "Longitude of C. Meridian     " );
	    print_false_ne( par[6], par[7] );
	    break;

	case GOOD:
	    print_proj_type( "GOODE'S HOMOLOSINE EQUAL AREA" );
	    print_proj_rad( par[0] );
	    break;

	case MOLL:
	    print_proj_type( "MOLLWEIDE" );
	    print_proj_rad( par[0] );
	    print_proj_dms( par[4], "Longitude of C. Meridian     " );
	    print_false_ne( par[6], par[7] );
	    break;

	case HAMMER:
	    print_proj_type( "HAMMER" );
	    print_proj_rad( par[0] );
	    print_proj_dms( par[4], "Longitude of C. Meridian     " );
	    print_false_ne( par[6], par[7] );
	    break;

        case ISINUS:
            print_proj_type( "INTEGERIZED SINUSOIDAL" );
            print_proj_rad( par[0] );
            print_proj_dms( par[4], "Longitude of C. Meridian     " );
            print_false_ne( par[6], par[7] );
            print_proj_gen( par[8], "Number of Latitudinal Zones  " );
            print_proj_gen( par[10], "Right Justify Columns Flag   " );
            break;

        default:
            sprintf( msg, "Unrecognized projection: %ld", proj );
            MessageHandler( NULL, msg );
            break;
    }
}

/* Print header with projection name 
 ----------------------------------*/
void print_proj_type( char text[75]	/* Projection name */
     )
{
    char outmsg[100];		/* buffer for output message */

    sprintf( outmsg, "%s PROJECTION PARAMETERS:", text );
    MessageHandler( NULL, outmsg );
}

/* Report the string (text) with the numeric value (val) 
 -------------------------------------------------------*/
void print_proj_gen( double val, char *text )
{
    char outmsg[100];		/* buffer for output message */

    sprintf( outmsg, " %s = %f", text, val );
    MessageHandler( NULL, outmsg );
}

/* Reports a TEXT = DMS value parameter 
 ---------------------------------*/
void print_proj_dms( double angle, char *text )
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

    sprintf( outmsg, " %s = %ld %ld %lf", text, deg, min, sec );
    MessageHandler( NULL, outmsg );
}

/* Reports an ellipsoidal earth model 
 ------------------------------------*/
void print_proj_spheroid( double major, double e2 )
{
    char outmsg[100];		/* buffer for output message */

    if ( e2 == 0 )
    {
	print_proj_rad( major );
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
void print_proj_rad( double radius )
{
    char outmsg[100];		/* buffer for output message */

    sprintf( outmsg, " Radius of Sphere              = %f meters", radius );
    MessageHandler( NULL, outmsg );
}

/* Reports false northings and false eastings
 -------------------------------------------*/
void print_false_ne( double east, double north )
{
    char outmsg[100];		/* buffer for output message */

    sprintf( outmsg, " False Easting                 = %f meters", east );
    MessageHandler( NULL, outmsg );
    sprintf( outmsg, " False Northing                = %f meters", north );
    MessageHandler( NULL, outmsg );
}

/* Report projection zone code
 ----------------------------*/
void print_proj_zone( long zone )
{
    char outmsg[100];		/* buffer for output message */

    sprintf( outmsg, " Zone Code = %ld", zone );
    MessageHandler( NULL, outmsg );
}

/* Report the NAD value
 ---------------------*/
void print_proj_nad( double val )
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
