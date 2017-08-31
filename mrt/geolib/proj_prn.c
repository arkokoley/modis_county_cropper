
/*******************************************************************************
NAME			      proj_print

PURPOSE	     Report projection parameter to the user

PROGRAM HISTORY
PROGRAMMER		DATE		REASON
----------		----		------
D. Steinwand		Aug, 1988	Original Development
D. Steinwand		Feb, 1989	Misc updates
D. Steinwand		Aug, 1989	Added Hammer & Robinson
S. Nelson		Jun, 1993	Added new projections from the "C"
					version of GCTP
S. Nelson		Feb, 1994	Corrected error in print_proj_nad

PROJECT     LAS

ALGORITHM 
	Determine map projection type (code)
	Report projection parameters to file
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
#include "geompak.h"

/*
** LOCAL PROTOTYPES 
*/

static void prt_proj_type( char text[75],	/* Projection name */
			   FILE *fp	/* pointer to print file */
     );

static void prt_proj_gen( double val,	/* Value to be printed */
			  char text[75],	/* Message to be printed */
			  FILE *fp	/* pointer to print file */
     );

static void prt_proj_dms( double angle,	/* Angle in DDDMMMSSS.SS format */
			  char text[75],	/* Message to be printed */
			  FILE *fp	/* Pointer to print file */
     );

static void prt_proj_spheroid( double major,	/* Semi-major axis of ellipsoid */
			       double e2,	/* Eccentricity squared, semi-minor axis, or blank */
			       FILE *fp	/* Pointer to print file */
     );

static void prt_proj_rad( double radius,	/* Radius of reference sphere (earth) */
			  FILE *fp	/* Pointer to printer file */
     );

static void prt_false_ne( double east,	/* False easting */
			  double north,	/* False northing */
			  FILE *fp	/* Pointer to printer file */
     );

static void prt_proj_zone( long zone,	/* UTM or State Plane Coordinate System zone code */
			   FILE *fp	/* Pointer to printer file */
     );

static void prt_proj_nad( double val,	/* Value to be printed */
			  FILE *fp	/* pointer to print file */
     );

/*-------------------------------------------------------------------*/

void proj_print( long proj,	/* Projection ID as defined in proj.h */
		 long zone,	/* Zone number for state plane and UTM */
		 double par[15],	/* Array of 15 projection parameters */
		 char *fname,	/* File name of output */
		 char *mess	/* Input/Output projection space mesage */
     )
{
    FILE *fp;			/* Pointer to printer file */

    fp = fopen( fname, "a" );

    fprintf( fp, "\n%s\n\n", mess );

    switch ( proj )
    {

	case GEO:
	    fprintf( fp, "GEOGRAPHIC COORDINATES\n" );
	    break;

	case UTM:
	    prt_proj_type( "UTM", fp );
	    prt_proj_zone( zone, fp );
	    break;

	case SPCS:
	    prt_proj_type( "STATE PLANE", fp );
	    prt_proj_zone( zone, fp );
	    prt_proj_nad( par[0], fp );
	    prt_spcs_zone( zone, fp );
	    break;

	case ALBERS:
	    prt_proj_type( "ALBERS CONICAL EQUAL-AREA", fp );
	    prt_proj_spheroid( par[0], par[1], fp );
	    prt_proj_dms( par[2], "Latitude of 1st Std. Parallel", fp );
	    prt_proj_dms( par[3], "Latitude of 2nd Std. Parallel", fp );
	    prt_proj_dms( par[4], "Longitude of Origin          ", fp );
	    prt_proj_dms( par[5], "Latitude of Origin           ", fp );
	    prt_false_ne( par[6], par[7], fp );
	    break;

	case LAMCC:
	    prt_proj_type( "LAMBERT CONFORMAL CONIC", fp );
	    prt_proj_spheroid( par[0], par[1], fp );
	    prt_proj_dms( par[2], "Latitude of 1st Std. Parallel", fp );
	    prt_proj_dms( par[3], "Latitude of 2nd Std. Parallel", fp );
	    prt_proj_dms( par[4], "Longitude of Origin          ", fp );
	    prt_proj_dms( par[5], "Latitude of Origin           ", fp );
	    prt_false_ne( par[6], par[7], fp );
	    break;

	case MERCAT:
	    prt_proj_type( "MERCATOR", fp );
	    prt_proj_spheroid( par[0], par[1], fp );
	    prt_proj_dms( par[5], "Latitude of True Scale       ", fp );
	    prt_proj_dms( par[4], "Central Longitude            ", fp );
	    prt_false_ne( par[6], par[7], fp );
	    break;

	case PS:
	    prt_proj_type( "POLAR STEREOGRAPHIC", fp );
	    prt_proj_spheroid( par[0], par[1], fp );
	    prt_proj_dms( par[4], "Longitude of Y-Axis          ", fp );
	    prt_proj_dms( par[5], "Latitude of True Scale       ", fp );
	    prt_false_ne( par[6], par[7], fp );
	    break;

	case POLYC:
	    prt_proj_type( "POLYCONIC", fp );
	    prt_proj_spheroid( par[0], par[1], fp );
	    prt_proj_dms( par[4], "Longitude of Origin          ", fp );
	    prt_proj_dms( par[5], "Latitude of Origin           ", fp );
	    prt_false_ne( par[6], par[7], fp );
	    break;

	case EQUIDC:
	    prt_proj_type( "EQUIDISTANT CONIC", fp );
	    prt_proj_spheroid( par[0], par[1], fp );
	    if ( par[8] == 0.0 )
		prt_proj_dms( par[2], "Latitude of Std. Parallel    ", fp );
	    else
	    {
		prt_proj_dms( par[2], "Latitude of 1st Std. Parallel", fp );
		prt_proj_dms( par[3], "Latitude of 2nd Std. Parallel", fp );
	    }
	    prt_proj_dms( par[4], "Longitude of Origin          ", fp );
	    prt_proj_dms( par[5], "Latitude of Origin           ", fp );
	    prt_false_ne( par[6], par[7], fp );
	    break;

	case TM:
	    prt_proj_type( "TRANSVERSE MERCATOR", fp );
	    prt_proj_spheroid( par[0], par[1], fp );
	    prt_proj_gen( par[2], "Scale Factor at C. Meridian  ", fp );
	    prt_proj_dms( par[4], "Longitude of C. Meridian     ", fp );
	    prt_proj_dms( par[5], "Latitude of Origin           ", fp );
	    prt_false_ne( par[6], par[7], fp );
	    break;

	case STEREO:
	    prt_proj_type( "STEREOGRAPHIC", fp );
	    prt_proj_rad( par[0], fp );
	    prt_proj_dms( par[4], "Longitude of Center          ", fp );
	    prt_proj_dms( par[5], "Latitude  of Center          ", fp );
	    prt_false_ne( par[6], par[7], fp );
	    break;

	case LAMAZ:
	    prt_proj_type( "LAMBERT AZIMUTHAL EQUAL-AREA", fp );
	    prt_proj_rad( par[0], fp );
	    prt_proj_dms( par[4], "Longitude of Center          ", fp );
	    prt_proj_dms( par[5], "Latitude  of Center          ", fp );
	    prt_false_ne( par[6], par[7], fp );
	    break;

	case AZMEQD:
	    prt_proj_type( "AZIMUTHAL EQUIDISTANT", fp );
	    prt_proj_rad( par[0], fp );
	    prt_proj_dms( par[4], "Longitude of Center          ", fp );
	    prt_proj_dms( par[5], "Latitude  of Center          ", fp );
	    prt_false_ne( par[6], par[7], fp );
	    break;

	case GNOMON:
	    prt_proj_type( "GNOMONIC", fp );
	    prt_proj_rad( par[0], fp );
	    prt_proj_dms( par[4], "Longitude of Center          ", fp );
	    prt_proj_dms( par[5], "Latitude  of Center          ", fp );
	    prt_false_ne( par[6], par[7], fp );
	    break;

	case ORTHO:
	    prt_proj_type( "ORTHOGRAPHIC", fp );
	    prt_proj_rad( par[0], fp );
	    prt_proj_dms( par[4], "Longitude of Center          ", fp );
	    prt_proj_dms( par[5], "Latitude  of Center          ", fp );
	    prt_false_ne( par[6], par[7], fp );
	    break;

	case GVNSP:
	    prt_proj_type( "GENERAL VERT. NEAR-SIDE PERSPECTIVE", fp );
	    prt_proj_rad( par[0], fp );
	    prt_proj_gen( par[2], "Height of Perspective Point  ", fp );
	    prt_proj_dms( par[4], "Longitude of Center          ", fp );
	    prt_proj_dms( par[5], "Latitude  of Center          ", fp );
	    prt_false_ne( par[6], par[7], fp );
	    break;

	case SNSOID:
	    prt_proj_type( "SINUSOIDAL", fp );
	    prt_proj_rad( par[0], fp );
	    prt_proj_dms( par[4], "Longitude of C. Meridian     ", fp );
	    prt_false_ne( par[6], par[7], fp );
	    break;

	case EQRECT:
	    prt_proj_type( "EQUIRECTANGULAR", fp );
	    prt_proj_rad( par[0], fp );
	    prt_proj_dms( par[5], "Latitude of True Scale       ", fp );
	    prt_proj_dms( par[4], "Longitude of C. Meridian     ", fp );
	    prt_false_ne( par[6], par[7], fp );
	    break;

	case MILLER:
	    prt_proj_type( "MILLER CYLINDRICAL", fp );
	    prt_proj_rad( par[0], fp );
	    prt_proj_dms( par[4], "Longitude of C. Meridian     ", fp );
	    prt_false_ne( par[6], par[7], fp );
	    break;

	case VGRINT:
	    prt_proj_type( "VAN DER GRINTEN I", fp );
	    prt_proj_rad( par[0], fp );
	    prt_proj_dms( par[4], "Longitude of C. Meridian     ", fp );
	    prt_proj_dms( par[5], "Latitude of Origin           ", fp );
	    prt_false_ne( par[6], par[7], fp );
	    break;

	case HOM:
	    prt_proj_type( "OBLIQUE MERCATOR (HOTINE)", fp );
	    prt_proj_spheroid( par[0], par[1], fp );
	    prt_proj_gen( par[2], "Scale at Center              ", fp );
	    prt_proj_dms( par[5], "Latitude of Origin           ", fp );
	    if ( par[12] == 0.0 )
	    {
		prt_proj_dms( par[8], "Longitude of 1st Point       ", fp );
		prt_proj_dms( par[9], "Latitude of 1st Point        ", fp );
		prt_proj_dms( par[10], "Longitude of 2nd Point       ", fp );
		prt_proj_dms( par[11], "Latitude of 2nd Point        ", fp );
	    }
	    else
	    {
		prt_proj_dms( par[3], "Azimuth of Central Line      ", fp );
		prt_proj_dms( par[4], "Longitude of Origin         ", fp );
	    }
	    prt_false_ne( par[6], par[7], fp );
	    break;

	case ROBIN:
	    prt_proj_type( "ROBINSON", fp );
	    prt_proj_rad( par[0], fp );
	    prt_proj_dms( par[4], "Longitude of C. Meridian     ", fp );
	    prt_false_ne( par[6], par[7], fp );
	    break;

	case SOM:
	    prt_proj_type( "SPACE OBLIQUE MERCATOR", fp );
	    prt_proj_spheroid( par[0], par[1], fp );
	    if ( par[12] == 0.0 )
	    {
		prt_proj_dms( par[3], "Inclination of Orbit         ", fp );
		prt_proj_dms( par[4], "Longitude of Ascending Orbit ", fp );
		prt_proj_gen( par[8], "Period of Sat. Revolution    ", fp );
		prt_proj_gen( par[9], "Landsat Ratio                ", fp );
		prt_proj_gen( par[10], "Landsat End of Path Flag     ", fp );
	    }
	    else
	    {
		prt_proj_gen( par[2], "Satellite Number             ", fp );
		prt_proj_gen( par[3], "Path Number                  ", fp );
	    }
	    prt_false_ne( par[6], par[7], fp );
	    break;

	case ALASKA:
	    prt_proj_type( "ALASKA CONFORMAL", fp );
	    prt_proj_spheroid( par[0], par[1], fp );
	    prt_false_ne( par[6], par[7], fp );
	    break;

	case GOOD:
	    prt_proj_type( "GOODE'S HOMOLOSINE EQUAL AREA", fp );
	    prt_proj_rad( par[0], fp );
	    break;

	case MOLL:
	    prt_proj_type( "MOLLWEIDE", fp );
	    prt_proj_rad( par[0], fp );
	    prt_proj_dms( par[4], "Longitude of C. Meridian     ", fp );
	    prt_false_ne( par[6], par[7], fp );
	    break;

	case IMOLL:
	    prt_proj_type( "INTERRUPTED MOLLWEIDE EQUAL AREA", fp );
	    prt_proj_rad( par[0], fp );
	    break;

	case HAMMER:
	    prt_proj_type( "HAMMER", fp );
	    prt_proj_rad( par[0], fp );
	    prt_proj_dms( par[4], "Longitude of C. Meridian     ", fp );
	    prt_false_ne( par[6], par[7], fp );
	    break;

	case WAGIV:
	    prt_proj_type( "WAGNER IV", fp );
	    prt_proj_rad( par[0], fp );
	    prt_proj_dms( par[4], "Longitude of C. Meridian     ", fp );
	    prt_false_ne( par[6], par[7], fp );
	    break;

	case WAGVII:
	    prt_proj_type( "WAGNER VII", fp );
	    prt_proj_rad( par[0], fp );
	    prt_proj_dms( par[4], "Longitude of C. Meridian     ", fp );
	    prt_false_ne( par[6], par[7], fp );
	    break;

	case OBEQA:
	    prt_proj_type( "OBLATED EQUAL-AREA", fp );
	    prt_proj_rad( par[0], fp );
	    prt_proj_dms( par[4], "Longitude of Center          ", fp );
	    prt_proj_dms( par[5], "Latitude  of Center          ", fp );
	    prt_proj_gen( par[2], "Oval Shape Parameter m       ", fp );
	    prt_proj_gen( par[3], "Oval Shape Parameter n       ", fp );
	    prt_proj_dms( par[8], "Oval Rotation Angle          ", fp );
	    prt_false_ne( par[6], par[7], fp );
	    break;

        case ISINUS:
	    prt_proj_type( "INTEGERIZED SINUSOIDAL", fp );
            prt_proj_rad( par[0], fp );
            prt_proj_dms( par[4], "Longitude of C. Meridian     ", fp );
            prt_false_ne( par[6], par[7], fp );
            prt_proj_gen( par[8], "Number of Latitudinal Zones  ", fp );
            prt_proj_gen( par[10], "Right Justify Columns Flag   ", fp );
            break;
    }

    fprintf( fp, "\n" );
    fclose( fp );
}

/* Print header with projection name 
 ----------------------------------*/
static void prt_proj_type( char text[75],	/* Projection name */
			   FILE *fp	/* pointer to print file */
     )
{
    fprintf( fp, "%s PROJECTION PARAMETERS:\n", text );
}

/* Report the string (text) with the numeric value (val) 
 -------------------------------------------------------*/
static void prt_proj_gen( double val,	/* Value to be printed */
			  char text[75],	/* Message to be printed */
			  FILE *fp	/* pointer to print file */
     )
{
    fprintf( fp, " %s = %f\n", text, val );
}

/* Reports a TEXT = DMS value parameter 
 ---------------------------------*/
static void prt_proj_dms( double angle,	/* Angle in DDDMMMSSS.SS format */
			  char text[75],	/* Message to be printed */
			  FILE *fp	/* Pointer to print file */
     )
{
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

    fprintf( fp, " %s = %ld %ld %f\n", text, deg, min, sec );
}

/* Reports an ellipsoidal earth model 
 ------------------------------------*/
static void prt_proj_spheroid( double major,	/* Semi-major axis of ellipsoid */
			       double e2,	/* Eccentricity squared, semi-minor axis, or blank */
			       FILE *fp	/* Pointer to print file */
     )
{
    if ( e2 == 0 )
    {
	prt_proj_rad( major, fp );
	return;
    }
    else
    {
	fprintf( fp, " Semi-major Axis of Ellipsoid  = %f meters\n", major );
	if ( e2 < 1 )
	    fprintf( fp, " Eccentricity Squared          = %.12f\n", e2 );
	else
	    fprintf( fp, " Semi-minor Axis               = %f\n", e2 );
    }
}

/* Reports a spherical earth model
 --------------------------------*/
static void prt_proj_rad( double radius,	/* Radius of reference sphere (earth) */
			  FILE *fp	/* Pointer to printer file */
     )
{
    fprintf( fp, " Radius of Sphere              = %f meters\n", radius );
}

/* Reports false northings and false eastings
 -------------------------------------------*/
static void prt_false_ne( double east,	/* False easting */
			  double north,	/* False northing */
			  FILE *fp	/* Pointer to printer file */
     )
{
    fprintf( fp, " False Easting                 = %f meters\n", east );
    fprintf( fp, " False Northing                = %f meters\n", north );
}

/* Report projection zone code
 ----------------------------*/
static void prt_proj_zone( long zone,	/* UTM or State Plane Coordinate System zone code */
			   FILE *fp	/* Pointer to printer file */
     )
{
    fprintf( fp, " Zone Code = %ld\n", zone );
}

/* Report the string (text) with the numeric value (val) 
 -------------------------------------------------------*/
static void prt_proj_nad( double val,	/* Value to be printed */
			  FILE *fp	/* pointer to print file */
     )
{
    long nadval;

    if ( ( val - 0.0 ) < 0.00001 )
	nadval = 27;
    else
	nadval = 83;
    fprintf( fp, " Datum     = NAD %ld\n", nadval );
    return;
}
