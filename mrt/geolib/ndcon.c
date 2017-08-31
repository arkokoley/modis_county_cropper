
/**********************************************************************
*                                                                     *
* NAME:   			NDCON
*                                                                     *
* PURPOSE:    COMPUTATION PROGRAM TO CONVERT (OR TRANSFORM)           *
*             POSITIONAL DATA (E.G., LATITUDES AND LONGITUDES) FROM   *
*             THE NORTH AMERICAN DATUM OF 1927 (NAD 27) TO THE        *
*             NORTH AMERICAN DATUM OF 1983 (NAD 83).  THIS PROGRAM    *
*             CAN COMPUTE FROM FROM EITHER DATUM TO THE OTHER.        *
*                                                                     *
*             THE ACTUAL COMPUTATION IS PERFORMED AS AN INTERPOLATION *
*             FROM A REGULARLY-SPACED GRID OF POINTS OBTAINED FROM THE*
*             FITTING OF A MINIMUM-CURVATURE SURFACE TO THE ACTUAL    *
*             SHIFT DATA RESULTING FROM THE NAD 83 ADJUSTMENT.        *
*                                                                     *
*             THE INTERPOLATION IS ACCOMPLISHED BY LOCALLY FITTING    *
*             A CURVED POLYNOMIAL SURFACE TO THE FOUR DATA POINTS     *
*             DEFINING THE SQUARE WHICH SURROUND THE (X,Y) PAIR       *
*             WHERE THE INTERPOLATION IS TO TAKE PLACE.               *
*                                                                     *
*             THE POLYNOMIAL SURFACE IS DEFINED BY:                   *
*                                                                     *
*                         A+BX+CY+DXY=Z                               *
*                                                                     *
* VERSION DATE:  APRIL 1, 1991                                        *
*                                                                     *
*        AUTHOR:   WARREN T. DEWHURST, PH.D.                          *
*                    LIEUTENANT COMMANDER, NOAA                       *
*                  ALICE R. DREW                                      *
*                    SENIOR GEODESIST, HORIZONTAL NETWORK BRANCH      *
*                  NATIONAL GEODETIC SURVEY, NOS, NOAA                *
*                  ROCKVILLE, MD   20852                              *
*								      *
* version 2.10 - 1/20/92					      *
*       added option to select HPGN grids and compute NAD 83 - HPGN   *
*       conversions - jmb					      *
  Version 2.2  - 2/93
	Converted to C programming language - T. Mittan
  Version 2.3  - 6/94
	Debugged and setup for LAS software - S. Nelson
  Version 2.4  - 11/94
	fixed error converting points over Hawaii.  The
        error occurred in INTRP.
	Changed out of bounds err message from the continuous US range
	to a more general message giving the point that was out of range.
					    - S. Nelson
  Version 2.5  - 2/95
	Increased array size for luarea and areas.
	Added status checks for "fread" statements and altered error
	messages.
  
  Version ?.?  - 8/95
        Changed WORCONFIG to LASTABLES
	- J. Fenno
	
  Version ?.?  - 02/01
        Changed LASTABLES to MRT_DATA_DIR
        - J. Weiss
	
***********************************************************************

***********************************************************************
*                                                                     *
*                  DISCLAIMER                                         *
*                                                                     *
*   THIS PROGRAM AND SUPPORTING INFORMATION IS FURNISHED BY THE       *
* GOVERNMENT OF THE UNITED STATES OF AMERICA, AND IS ACCEPTED AND     *
* USED BY THE RECIPIENT WITH THE UNDERSTANDING THAT THE UNITED STATES *
* GOVERNMENT MAKES NO WARRANTIES, EXPRESS OR IMPLIED, CONCERNING THE  *
* ACCURACY, COMPLETENESS, RELIABILITY, OR SUITABILITY OF THIS         *
* PROGRAM, OF ITS CONSTITUENT PARTS, OR OF ANY SUPPORTING DATA.       *
*                                                                     *
*   THE GOVERNMENT OF THE UNITED STATES OF AMERICA SHALL BE UNDER NO  *
* LIABILITY WHATSOEVER RESULTING FROM ANY USE OF THIS PROGRAM.  THIS  *
* PROGRAM SHOULD NOT BE RELIED UPON AS THE SOLE BASIS FOR SOLVING A   *
* PROBLEM WHOSE INCORRECT SOLUTION COULD RESULT IN INJURY TO PERSON   *
* OR PROPERTY.                                                        *
*                                                                     *
*   THIS PROGRAM IS PROPERTY OF THE GOVERNMENT OF THE UNITED STATES   *
* OF AMERICA.  THEREFORE, THE RECIPIENT FURTHER AGREES NOT TO ASSERT  *
* PROPRIETARY RIGHTS THEREIN AND NOT TO REPRESENT THIS PROGRAM TO     *
* ANYONE AS BEING OTHER THAN A GOVERNMENT PROGRAM.                    *
*                                                                     *
**********************************************************************/

/* Windows hack to get at PATH_MAX */
#if defined(WIN32) && !defined(_POSIX_)
#define _POSIX_
#include <limits.h>
#undef _POSIX_
#else
#include <limits.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "worgen.h"
#include "geompak.h"
#include "loclprot.h"

#define PATHLEN  PATH_MAX
#define MXAREA 8
#define MXDEF 8
#define TRUE 1
#define FALSE 0
#define ERROR -1
#define OK 0

/*
** LOCAL PROTOTYPES
*/

static void coeff

    ( double *tee1,
      double *tee2,
      double *tee3,
      double *tee4, double *ay, double *bee, double *cee, double *dee );

static void dgrids( void );

static void fgrid

    ( double *xpt,
      double *ypt,
      double *dx,
      double *dy,
      double *xmax,
      double *xmin,
      double *ymax,
      double *ymin,
      double *xgrid, double *ygrid, long *irow, long *jcol, long *nogo );

static void intrp

    ( long *iarea,
      long *irow,
      long *jcol,
      double *xgrid,
      double *ygrid,
      double *xpt,
      double *ypt,
      double *xpt2, double *ypt2, double *dlos, double *dlas, long *nogo );

static void ngrids( long *nodata );

static void openfl

    ( char *afile,
      long *itemp,
      long *nogo,
      double *dx,
      double *dy,
      double *xmax1, double *xmin1, double *ymax1, double *ymin1, long *nc1 );

static void surf

    ( double *xgrid,
      double *ygrid,
      double *zee,
      double *ay, double *bee, double *cee, double *dee, long *irow, long *jcol );

static void to83

    ( long *nogo,
      char *resp,
      double *xpt,
      double *ypt, double *xpt2, double *ypt2, double *dlos, double *dlas );

static void transf

    ( long *nogo,
      char *resp, double *xpt, double *ypt, double *xpt2, double *ypt2, long *key );

static char message[256];	/* Error message                        */
static char errkey[256];	/* Error key                            */
static double dx[MXAREA];
static double dy[MXAREA];
static double xmax[MXAREA];
static double xmin[MXAREA];
static double ymax[MXAREA];
static double ymin[MXAREA];
static char areas[MXDEF][20];
static long griderr;
static long nc[MXAREA], narea;
static long init = TRUE;

static FILE /* *napar, */ *luarea[2 * MXAREA + 1];

int ndcon( double *incoor,	/* input coordinates                    */
	   long *conv,		/* 1 = 27 - 83, 2 = 27 - 83             */
	   double *outcoor,	/* output coordinates                   */
	   char *errkey2,	/* error key                            */
	   char *message2       /* error message                        */
     )
{
    long key = 0L, ifile, nodata;
    long nogo;
    char resp[15];

/* INITIALIZE VARIABLES
  --------------------*/

/* end of conversions -- close files
   ---------------------------------*/
    if ( *conv == -1 )
    {
	for ( ifile = 0; ifile < 2 * narea; ifile++ )
	    fclose( luarea[ifile] );
	return ( OK );
    }

/* convert from NAD27 to NAD83
   ---------------------------*/
    else if ( *conv == 1 )
    {
	key = 1;
    }

/* convert from NAD83 to NAD27
  ---------------------------*/
    else if ( *conv == 2 )
    {
	key = -1;
    }

/* only NAD83 and NAD27 are supported at this time.  The p.c.
   version of nadcon has options involving hpgn files.  These files
   are not supported in this version.
  ---------------------------------------------------------------*/
    else
    {
	strcpy( errkey, "ndcon-conv" );
	strcpy( message, "Unsupported conversion type" );
	return ( ERROR );
    }

/* OPEN NADCON DATA FILES (LATITUDE AND LONGITUDE GRIDS)
 -----------------------------------------------------*/

    griderr = FALSE;
    if ( init )
    {
	ngrids( &nodata );
	if ( nodata )
	{
	    strcpy( message2, message );
	    strcpy( errkey2, errkey );
	    return ( ERROR );
	}
/*
        if( napar != NULL ) {
	   fclose( napar );
           napar = NULL;
        }
*/
	init = FALSE;
    }

/* LOOP (ONCE FOR EACH CONVERSION)
 -------------------------------*/

    transf( &nogo, resp, &incoor[0], &incoor[1], &outcoor[0], &outcoor[1], &key );
    strcpy( message2, message );
    strcpy( errkey2, errkey );
    if ( nogo )
	return ( ERROR );

    return ( OK );
}

/*********************************************************************
NAME:				COEFF

PURPOSE:  GENERATES COEFFICIENTS FOR SURFACE FUNCTION
*********************************************************************/

static void coeff
    ( double *tee1,
      double *tee2,
      double *tee3, double *tee4, double *ay, double *bee, double *cee, double *dee )
{
    *ay = *tee1;
    *bee = *tee3 - *tee1;
    *cee = *tee2 - *tee1;
    *dee = *tee4 - *tee3 - *tee2 + *tee1;
}

/********************************************************************
NAME			DGRIDS

PURPOSE:  This subroutine opens the NADCON grids using the default grid
	  names and locations.  The default names of the grid areas are
	  given in DAREAS and the default base file locations are in DFILES
 ******************************************************************/

static void dgrids( void )
{
    double xmax1, xmin1, ymax1, ymin1;
    double dx1, dy1;
    long idef, itemp, nc1;
    char afile[PATHLEN];
    char dfiles[MXAREA][PATHLEN];
    char dareas[MXDEF][15];
    char *ptr;
    char lastables[PATHLEN];
    long nogo;
    int i;

/* Get directory containing files
  -----------------------------*/
    ptr = (char *)getenv("MRT_DATA_DIR");
    if( ptr == NULL ) {
       ptr = (char *)getenv("MRTDATADIR");
       if (ptr == NULL) {
          strcpy(errkey,"ndcon-worcon");
          strcpy(message,"MRT_DATA_DIR nor MRTDATADIR not defined");
          nogo = TRUE;
          griderr = TRUE;
          return;
       }
    }

    lastables[PATHLEN - 1] = 0;
    strncpy(lastables, ptr, PATHLEN - 1);
    strncat(lastables, "/", PATHLEN - 1);

    for( i = 0; i < MXDEF; ++i )
       {
       dfiles[i][PATHLEN - 1] = 0;
       strncpy( dfiles[i], lastables, PATHLEN - 1 );
       }

    strncat( dfiles[0], "conus", PATHLEN - 1);
    strncat( dfiles[1], "hawaii", PATHLEN - 1);
    strncat( dfiles[2], "prvi", PATHLEN - 1);
    strncat( dfiles[3], "stlrnc", PATHLEN - 1);
    strncat( dfiles[4], "stgeorge", PATHLEN - 1);
    strncat( dfiles[5], "stpaul", PATHLEN - 1);
    strncat( dfiles[6], "alaska", PATHLEN - 1);
    dfiles[7][0] = 0;

    strcpy( dareas[0], "Conus" );
    strcpy( dareas[1], "Hawaii" );
    strcpy( dareas[2], "P.R. and V.I." );
    strcpy( dareas[3], "St. Laurence I." );
    strcpy( dareas[4], "St. George I." );
    strcpy( dareas[5], "St. Paul I." );
    strcpy( dareas[6], "Alaska" );
    strcpy( dareas[7], " " );

/* DFILES contains the default locations (pathname) of the grid files
* without the .las and .los extensions. (For example 'conus' would
* indicate that the conus.las and conus.los grid files are in the
* current working directory.)  The length of each entry in DFILES may
* be up to CMLEN characters.  DAREAS contains the default names of these
* areas.  The names are used internally in the program and in the
* program output.  They may be no longer than 15 characters.  They
* must correspond on a one-to-one basis with the file locations in
* the DFILES array.  That is, the first area name in DAREAS must
* be the name that you wish for the first data file set in the
* DFILES array.  You may, of course, have the arrays the same if
* the location of the data file is no longer than 15 characters.
* The locations of the grid files may be different for each
* installation.  If the pathnames are not correct DFILES (and, possibly,
* DAREAS) may be changed and the program recompiled.
  --------------------------------------------------------------------*/
    for ( idef = 0; idef < MXDEF - 1; idef++ )
    {
        if( *dfiles[idef] == 0 )
           break;
	strcpy( afile, dfiles[idef] );

/* Try to open a set of default files.
   Do not print error messages for non-existing files.
  -------------------------------------------------*/

	itemp = narea;
	openfl( afile, &itemp, &nogo, &dx1, &dy1, &xmax1, &xmin1,
		&ymax1, &ymin1, &nc1 );

	if ( !nogo )
	{

/* Set of files opened OK and variables read
  ----------------------------------------*/

	    narea = itemp;
	    strcpy( areas[narea], dareas[idef] );
	    dx[narea] = dx1;
	    dy[narea] = dy1;
	    xmax[narea] = xmax1;
	    xmin[narea] = xmin1;
	    ymax[narea] = ymax1;
	    ymin[narea] = ymin1;
	    nc[narea] = nc1;

	    narea++;
	}
    }
    return;
}

/*********************************************************************
NAME:				FGRID

PURPOSE:  IDENTIFIES THE LOCAL GRID SQUARE FOR INTRP

   This subroutine is designed to identify the grid square in which a
   particular point is located and get the corner coordinates
   converted into the index coordinate system.
************************************************************************/

static void fgrid
    ( double *xpt,
      double *ypt,
      double *dx,
      double *dy,
      double *xmax,
      double *xmin,
      double *ymax,
      double *ymin,
      double *xgrid, double *ygrid, long *irow, long *jcol, long *nogo )
{
    *nogo = FALSE;

/* Check to see it the point is outside the area of the gridded data
  -----------------------------------------------------------------*/

    if ( ( *xpt >= *xmax ) || ( *xpt <= *xmin ) || ( *ypt >= *ymax ) ||
	 ( *ypt <= *ymin ) )
    {
	*nogo = TRUE;
	return;
    }

/* Calculate the coordinate values for the point to be interpolated
   in terms of grid indices*
  ----------------------------------------------------------------*/

    *xgrid = ( *xpt - *xmin ) / *dx + 1.0;
    *ygrid = ( *ypt - *ymin ) / *dy + 1.0;

    *irow = ( long ) ( *ygrid );
    *jcol = ( long ) ( *xgrid );

    return;
}

/*********************************************************************
NAME:				INTRP

PURPOSE:  DETERMINE SURFACE FUNCTION FOR THIS GRID SQUARE
          AND INTERPOLATE A VALUE, ZEE, FOR XPT, YPT
*********************************************************************/

static long iareal = 0;
static long irowl = 0;
static long jcoll = 0;
static double ay1, bee1, cee1, dee1, ay2, bee2, cee2, dee2;

static void intrp
    ( long *iarea,
      long *irow,
      long *jcol,
      double *xgrid,
      double *ygrid,
      double *xpt,
      double *ypt,
      double *xpt2, double *ypt2, double *dlos, double *dlas, long *nogo )
{
    float buf[2];
    long num_elem;
    double tee1, tee2, tee3, tee4, zee;
    FILE *ifile;

/*
      num_elem = nc[2 * (*iarea)];
*/
    num_elem = nc[*iarea];

/* LATITUDE
  ---------*/

    if ( ( *irow != irowl ) || ( *jcol != jcoll ) || ( *iarea != iareal ) )
    {
	/* Lower boundary */

	ifile = luarea[2 * ( *iarea )];
	fseek( ifile, ( ( *irow ) * 16 * ( num_elem + 1 ) + 4 * *jcol ), 0 );
	if ( fread( buf, sizeof( float ), 2, ifile ) != 2 )
	{
	    strcpy( errkey, "ndcon-fread" );
	    strcpy( message, "Error reading data files" );
	    *nogo = TRUE;
	    return;
	}
	tee1 = ( double ) ( buf[0] );
	tee3 = ( double ) ( buf[1] );

	/* Upper boundary */

	fseek( ifile, ( ( *irow + 1 ) * 16 * ( num_elem + 1 ) + 4 * *jcol ), 0 );

/*
	fseek(ifile,(16*(num_elem + 1) + 4 * (*jcol - 2)),1);
*/
	if ( fread( buf, sizeof( float ), 2, ifile ) != 2 )
	{
	    strcpy( errkey, "ndcon-fread" );
	    strcpy( message, "Error reading data files" );
	    *nogo = TRUE;
	    return;
	}
	tee2 = ( double ) ( buf[0] );
	tee4 = ( double ) ( buf[1] );
	coeff( &tee1, &tee2, &tee3, &tee4, &ay1, &bee1, &cee1, &dee1 );

    }

    surf( xgrid, ygrid, &zee, &ay1, &bee1, &cee1, &dee1, irow, jcol );
    *dlas = zee;

/* LONGITUDE
  ----------*/

    if ( ( *irow != irowl ) || ( *jcol != jcoll ) || ( *iarea != iareal ) )
    {
	/* Lower boundary */

	ifile = luarea[2 * ( *iarea ) + 1];
	fseek( ifile, ( ( *irow ) * 16 * ( num_elem + 1 ) + 4 * *jcol ), 0 );
	if ( fread( buf, sizeof( float ), 2, ifile ) != 2 )
	{
	    strcpy( errkey, "ndcon-fread" );
	    strcpy( message, "Error reading data files" );
	    *nogo = TRUE;
	    return;
	}
	tee1 = ( double ) ( buf[0] );
	tee3 = ( double ) ( buf[1] );

	/* Upper boundary */

	fseek( ifile, ( ( *irow + 1 ) * 16 * ( num_elem + 1 ) + 4 * *jcol ), 0 );

/*
	fseek(ifile,(16*(num_elem + 1) + 4 * (*jcol - 2)),1);
*/
	if ( fread( buf, sizeof( float ), 2, ifile ) == 0 )
	{
	    strcpy( errkey, "ndcon-fread" );
	    strcpy( message, "Error reading data files" );
	    *nogo = TRUE;
	    return;
	}
	tee2 = ( double ) ( buf[0] );
	tee4 = ( double ) ( buf[1] );

	coeff( &tee1, &tee2, &tee3, &tee4, &ay2, &bee2, &cee2, &dee2 );
    }

    surf( xgrid, ygrid, &zee, &ay2, &bee2, &cee2, &dee2, irow, jcol );
    *dlos = zee;

/* COMPUTE THE NAD 83 VALUES
 --------------------------*/

    *ypt2 = *ypt + *dlas / 3600.0;

/* Longitude is positive west in this subroutine
  --------------------------------------------*/

    *xpt2 = *xpt - *dlos / 3600.0;

    irowl = *irow;
    jcoll = *jcol;
    iareal = *iarea;
    return;
}

/************************************************************************
NAME:				NGRIDS

PURPOSE:  This subroutine opens the NADCON grids which contain datum shifts.
	  A total of two files are necessary for each area; 1 for each
	  latitude and longitude shift table (gridded data set) expressed
	  in arc seconds.
*************************************************************************/

static void ngrids( long *nodata )
{

/* Initialize variables
  -------------------*/

    *nodata = FALSE;
    narea = 0;

/*  open default names in the subroutine DGRIDS.
    -------------------------------------------*/
    dgrids();

    if ( narea == 0 )
    {
	*nodata = TRUE;
	/* ERROR No grid files were opened -- program ending!
	   ------------------------------------------------- */
	if ( griderr != TRUE )
	{
	    griderr = TRUE;
	    strcpy( errkey, "ndcon-data" );
	    strcpy( message, "Data files unavailable for conversion" );
	}
    }
    return;
}

/***************************************************************************
NAME:				OPENFL

 Given base name of gridded data files, open the two data files
***************************************************************************/
static void openfl
    ( char *afile,
      long *itemp,
      long *nogo,
      double *dx,
      double *dy,
      double *xmax1, double *xmin1, double *ymax1, double *ymin1, long *nc1 )
{
    float dx1, dy1, dx2, dy2;
    float x01, y01, angle1, x02, y02, angle2;
    long ila, ilo;
    char alas[PATHLEN], alos[PATHLEN];
    char rident[56];
    char pgm[8];
    long nr1, nz1, nc2, nr2, nz2;
    FILE *ifile;

/* Initialize 
  ----------*/
    *nogo = FALSE;

/* Form complete names of grid files
   --------------------------------*/
    alas[PATHLEN - 1] = 0;
    strncpy( alas, afile, PATHLEN - 1 );
    strncat( alas, ".las", PATHLEN - 1 );

    alos[PATHLEN - 1] = 0;
    strncpy( alos, afile, PATHLEN - 1 );
    strncat( alos, ".los", PATHLEN - 1 );

/* DIRECT ACCESS GRID FILES
   Each file is opened once to get the grid variables.
   The file is then closed and reopened to ensure that
   the record length is correct
 ----------------------------------------------------*/

/* Seconds of latitude grid file 
  -----------------------------*/

    ila = 2 * *itemp;
    if ( ( ifile = fopen( alas, "r" ) ) == NULL )
	goto L910;
    else
	luarea[ila] = ifile;
    if ( fread( rident, sizeof( char ), 56, ifile ) != 56 )
    {
	strcpy( errkey, "ndcon-fread" );
	strcpy( message, "Error reading data files" );
	*nogo = TRUE;
	griderr = TRUE;
	return;
    }
    if ( fread( pgm, sizeof( char ), 8, ifile ) != 8 )
    {
	strcpy( errkey, "ndcon-fread" );
	strcpy( message, "Error reading data files" );
	*nogo = TRUE;
	griderr = TRUE;
	return;
    }
    if ( fread( nc1, sizeof( long ), 1, ifile ) != 1 )
    {
	strcpy( errkey, "ndcon-fread" );
	strcpy( message, "Error reading data files" );
	*nogo = TRUE;
	griderr = TRUE;
	return;
    }
    if ( fread( &nr1, sizeof( long ), 1, ifile ) != 1 )
    {
	strcpy( errkey, "ndcon-fread" );
	strcpy( message, "Error reading data files" );
	*nogo = TRUE;
	griderr = TRUE;
	return;
    }
    if ( fread( &nz1, sizeof( long ), 1, ifile ) != 1 )
    {
	strcpy( errkey, "ndcon-fread" );
	strcpy( message, "Error reading data files" );
	*nogo = TRUE;
	griderr = TRUE;
	return;
    }
    if ( fread( &x01, sizeof( float ), 1, ifile ) != 1 )
    {
	strcpy( errkey, "ndcon-fread" );
	strcpy( message, "Error reading data files" );
	*nogo = TRUE;
	griderr = TRUE;
	return;
    }
    if ( fread( &dx1, sizeof( float ), 1, ifile ) != 1 )
    {
	strcpy( errkey, "ndcon-fread" );
	strcpy( message, "Error reading data files" );
	*nogo = TRUE;
	griderr = TRUE;
	return;
    }
    if ( fread( &y01, sizeof( float ), 1, ifile ) != 1 )
    {
	strcpy( errkey, "ndcon-fread" );
	strcpy( message, "Error reading data files" );
	*nogo = TRUE;
	griderr = TRUE;
	return;
    }
    if ( fread( &dy1, sizeof( float ), 1, ifile ) != 1 )
    {
	strcpy( errkey, "ndcon-fread" );
	strcpy( message, "Error reading data files" );
	*nogo = TRUE;
	griderr = TRUE;
	return;
    }
    if ( fread( &angle1, sizeof( float ), 1, ifile ) != 1 )
    {
	strcpy( errkey, "ndcon-fread" );
	strcpy( message, "Error reading data files" );
	*nogo = TRUE;
	griderr = TRUE;
	return;
    }

/* Seconds of longitude grid file 
 -------------------------------*/

    ilo = 2 * *itemp + 1;
    if ( ( ifile = fopen( alos, "r" ) ) == NULL )
	goto L920;
    else
	luarea[ilo] = ifile;
    if ( fread( rident, sizeof( char ), 56, ifile ) != 56 )
    {
	strcpy( errkey, "ndcon-fread" );
	strcpy( message, "Error reading data files" );
	*nogo = TRUE;
	griderr = TRUE;
	return;
    }
    if ( fread( pgm, sizeof( char ), 8, ifile ) != 8 )
    {
	strcpy( errkey, "ndcon-fread" );
	strcpy( message, "Error reading data files" );
	*nogo = TRUE;
	griderr = TRUE;
	return;
    }
    if ( fread( &nc2, sizeof( long ), 1, ifile ) != 1 )
    {
	strcpy( errkey, "ndcon-fread" );
	strcpy( message, "Error reading data files" );
	*nogo = TRUE;
	griderr = TRUE;
	return;
    }
    if ( fread( &nr2, sizeof( long ), 1, ifile ) != 1 )
    {
	strcpy( errkey, "ndcon-fread" );
	strcpy( message, "Error reading data files" );
	*nogo = TRUE;
	griderr = TRUE;
	return;
    }
    if ( fread( &nz2, sizeof( long ), 1, ifile ) != 1 )
    {
	strcpy( errkey, "ndcon-fread" );
	strcpy( message, "Error reading data files" );
	*nogo = TRUE;
	griderr = TRUE;
	return;
    }
    if ( fread( &x02, sizeof( float ), 1, ifile ) != 1 )
    {
	strcpy( errkey, "ndcon-fread" );
	strcpy( message, "Error reading data files" );
	*nogo = TRUE;
	griderr = TRUE;
	return;
    }
    if ( fread( &dx2, sizeof( float ), 1, ifile ) != 1 )
    {
	strcpy( errkey, "ndcon-fread" );
	strcpy( message, "Error reading data files" );
	*nogo = TRUE;
	griderr = TRUE;
	return;
    }
    if ( fread( &y02, sizeof( float ), 1, ifile ) != 1 )
    {
	strcpy( errkey, "ndcon-fread" );
	strcpy( message, "Error reading data files" );
	*nogo = TRUE;
	griderr = TRUE;
	return;
    }
    if ( fread( &dy2, sizeof( float ), 1, ifile ) != 1 )
    {
	strcpy( errkey, "ndcon-fread" );
	strcpy( message, "Error reading data files" );
	*nogo = TRUE;
	griderr = TRUE;
	return;
    }
    if ( fread( &angle2, sizeof( float ), 1, ifile ) != 1 )
    {
	strcpy( errkey, "ndcon-fread" );
	strcpy( message, "Error reading data files" );
	*nogo = TRUE;
	griderr = TRUE;
	return;
    }

/* Check to see if the two files have the same variables 
 ------------------------------------------------------*/

    if ( ( nc2 != *nc1 ) || ( nr2 != nr1 ) || ( nz2 != nz1 ) || ( x02 != x01 ) ||
	 ( dx2 != dx1 ) || ( y02 != y01 ) || ( dy2 != dy1 ) || ( angle2 != angle1 ) )
	goto L960;

/* Calculate values used in this program 
 --------------------------------------*/

    *xmin1 = ( double ) ( x01 );
    *ymin1 = ( double ) ( y01 );
    *xmax1 = ( double ) ( x01 ) + ( *nc1 - 1 ) * ( double ) ( dx1 );
    *ymax1 = ( double ) ( y01 ) + ( nr1 - 1 ) * ( double ) ( dy1 );
    *dx = fabs( dx1 );
    *dy = fabs( dy1 );

    return;

/*----------------------------
  WARNING AND ERROR MESSAGES
  -----------------------------*/

/* Grid files do not exist 
  -----------------------*/

  L910:*nogo = TRUE;

    if( luarea[ila] != NULL ) {
       fclose( luarea[ila] );
       luarea[ila] = NULL;
    }
    return;

  L920:*nogo = TRUE;

    if( luarea[ila] != NULL ) {
       fclose( luarea[ila] );
       luarea[ila] = NULL;
    }
    if( luarea[ilo] != NULL ) {
       fclose( luarea[ilo] );
       luarea[ilo] = NULL;
    }
    return;

/* Grid files do not agree
  -----------------------*/

  L960:*nogo = TRUE;

    if( luarea[ila] != NULL ) {
       fclose( luarea[ila] );
       luarea[ila] = NULL;
    }
    if( luarea[ilo] != NULL ) {
       fclose( luarea[ilo] );
       luarea[ilo] = NULL;
    }
    strcpy( message, "Contents of data files corrupt" );
    strcpy( errkey, "ndcon-file" );
    griderr = TRUE;
    return;
}

/*****************************************************************************
NAME:				SURF

PURPOSE:  INTERPOLATES THE Z VALUE
	  Calculated the value of the grid at the point XPT, YPT.  The
	  interpolation is done in the index coordinate system for convenience.
******************************************************************************/
static void surf
    ( double *xgrid,
      double *ygrid,
      double *zee,
      double *ay, double *bee, double *cee, double *dee, long *irow, long *jcol )
{
    double zee2, zee3;

    zee2 = ( *xgrid - ( double ) ( *jcol ) );
    zee3 = ( *ygrid - ( double ) ( *irow ) );
    *zee = *ay + zee2 * *bee + zee3 * ( *cee + *dee * zee2 );

}

/****************************************************************************
NAME:					TO83

PURPOSE: This subroutine predicts the NAD 83 latitude and longitude values
  	 given the NAD 27 latitude and longitude values in degree decimal
  	 format.  In addition, the program returns the shift values between
  	 The datums in both arc secs and meters.
  
  	 All of the predictions are based upon a straight-forward interpolation
  	 of a gridded data set of datum shifts.  The datum shifts are assumed
  	 to be provided in the files opened in the NGRIDS subroutine.  The
  	 common AREAS contains the names of the valid areas while the common
  	 GDINFO contains the grid variables.  NAREA is the number of areas
  	 which had data files opened.  A total of two files are necessary for
  	 each area: one latitude and one longitude shift table (gridded data
  	 set) expressed in arc seconds.

  	 For this subroutine, it is important to remember that the
  	 input longitude is assumed to be positive east and the
  	 output longitude will be positive east.
  
         Author:     Warren T. Dewhurst, PH. D.
                     National Geodetic Survey
                     November 1, 1989             

*****************************************************************************/

static void to83
    ( long *nogo,
      char *resp,
      double *xpt,
      double *ypt, double *xpt2, double *ypt2, double *dlos, double *dlas )
{
    double xgrid = 0.0, ygrid = 0.0;
    double dx0, dy0, xmax0, xmin0, ymax0, ymin0;
    long irow = 0L, jcol = 0L, iarea;

/* INITIALIZE
 -----------*/

    *nogo = FALSE;

/* READ WHERE TO GET THE DATA AND HOW IT IS ORGANIZED
   Check to see which set of gridded files xpt,ypt is in.
  ------------------------------------------------------*/

    for ( iarea = 0; iarea < narea; iarea++ )
    {
	dx0 = dx[iarea];
	dy0 = dy[iarea];
	xmax0 = xmax[iarea];
	xmin0 = xmin[iarea];
	ymax0 = ymax[iarea];
	ymin0 = ymin[iarea];
	fgrid( xpt, ypt, &dx0, &dy0, &xmax0, &xmin0, &ymax0, &ymin0, &xgrid,
	       &ygrid, &irow, &jcol, nogo );

	if ( !*nogo )
	    break;
    }

    /* Not in any of the grid areas
       ---------------------------- */
    if ( !*nogo )
    {
	/* Point in area number iarea and named AREAS(iarea) 
	   ------------------------------------------------ */
	strcpy( resp, areas[iarea] );
	intrp( &iarea, &irow, &jcol, &xgrid, &ygrid, xpt, ypt, xpt2,
	       ypt2, dlos, dlas, nogo );
	return;
    }

    /* Write out grid areas for the out-of-bounds error message
       -------------------------------------------------------- */

    sprintf( message, "Point %7.3f,%8.3f out of bounds", *ypt, *xpt );
    strcpy( errkey, "ndcon-range" );

    return;
}

/***************************************************************************
NAME:					TRANSF

PURPOSE:  This subroutine computes either the forward or inverse coordinate
	  transformation depending upon the value of the integer variable 'key'
	  1/20/92 - IF the HPGN option is chosen, statements in this subroutine
	  which refer to NAD 27 apply to NAD 83; 
	  statements which refer to NAD 83 apply to HPGN -jmb 

**************************************************************************/

static void transf
    ( long *nogo,
      char *resp, double *xpt, double *ypt, double *xpt2, double *ypt2, long *key )
{
    double small;
    double xtemp, ytemp, xdif, ydif;
    double dlos, dlas;
    double thrsix = 3600.0;
    long itmax;
    long num;

    itmax = 10;
    small = 1.0e-9;

/* FOR NAD 27 TO NAD 83
 --------------------*/
    if ( *key == 1 )

	to83( nogo, resp, xpt, ypt, xpt2, ypt2, &dlos, &dlas );

    else if ( *key == -1 )
    {

/* SET THE XPT,YPT TO TEMPORARY VALUES
   (REMEMBER, XPT AND YPT ARE REALLY NAD 83 VALUES)
  -----------------------------------------------*/

	xtemp = *xpt;
	ytemp = *ypt;

/* PRETEND THAT THESE TEMPORARY VALUES ARE REALLY NAD 27 VALUES
   FOR A FIRST GUESS AND COMPUTE PSEUDO-NAD 83 COORDINATES
 ------------------------------------------------------------*/

	for ( num = 0;; )
	{
	    num++;

/* CHECK THE NUMBER OF ITERATIONS
  ------------------------------*/

	    if ( num >= itmax )
	    {
		sprintf( message,
			 "Maximum iterations exceeded at point %7.3f,%8.3f", *ypt,
			 *xpt );
		strcpy( errkey, "ndcon-iterat" );
		*nogo = TRUE;
		break;
	    }

	    to83( nogo, resp, &xtemp, &ytemp, xpt2, ypt2, &dlos, &dlas );

/* Check if points lie in gridded area
 -----------------------------------*/
	    if ( *nogo )
	    {
		*xpt2 = xtemp;
		*ypt2 = ytemp;
		break;
	    }

/* COMPARE TO ACTUAL NAD 83 COORDINATES
  ------------------------------------*/

	    xdif = *xpt - *xpt2;
	    ydif = *ypt - *ypt2;

/* COMPUTE A NEW GUESS UNLESS THE DIFFERENCES ARE LESS THAN small
   WHERE small IS DEFINED (ABOVE) TO BE;  small = 1.0D-9
  -------------------------------------------------------------*/

	    if ( num == 1 )
	    {
		if ( fabs( xdif ) > small )
		    xtemp = *xpt - dlos / thrsix;
		if ( fabs( ydif ) > small )
		    ytemp = *ypt - dlas / thrsix;
	    }
	    else
	    {
		if ( fabs( xdif ) > small )
		    xtemp = xtemp - ( *xpt2 - *xpt );
		if ( fabs( ydif ) > small )
		    ytemp = ytemp - ( *ypt2 - *ypt );
	    }

	    if ( ( fabs( ydif ) <= small ) && ( fabs( xdif ) <= small ) )
	    {

/* IF CONVERGED THEN LEAVE LOOP
  ----------------------------*/

		*xpt2 = xtemp;
		*ypt2 = ytemp;
		break;
	    }
	}
    }
    return;
}
