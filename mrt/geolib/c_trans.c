/*******************************************************************************
NAME			      C_TRANSINIT

PURPOSE	     This function initializes projection and datum information
	     to be used by c_trans when doing the actual point 
             transforamations.

PROGRAM HISTORY
PROGRAMMER		DATE		REASON
----------		----		------
J. Willems		Apr. 1998	Based on c_projon.  Modified for datum
					conversions.
R. Burrell		04-June-2000    Add call to initialize GCTP report
                                        output.

PROJECT     LAS

ALGORITHM
	Check validity of parameters
	Pack spheroid codes
	Initialize GCTP transformations
	Report user entered parameters to user selected device
	Return

ALGORITHM REFERENCES

1.  Snyder, John P., "Map Projections--A Working Manual", U.S. Geological
    Survey Profesional Paper 1395 (Supersedes USGS Bulletin 1532), United
    State Government Printing Office, Washington D.C., 1987. 

2.  "Software Documentation for GCTP General Cartographic Transformation
    Package", U.S. Geological Survey National Mapping Division, May 1982.

3.  Clarie, Charles N, "State Plane Coordinates by Automatic Data Processing",
    U.S. Department of Commerce, Environmental Science Services Admin.,
    Coast and Geodetic Survey, Publication 62-4, 1973.
*******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "shared_resample.h"

#include "tae.h"
#include "worgen.h"
#include "datum.h"
#include "proj.h"
#include "cproj.h"
#include "isin.h"
#include "geompak.h"
#include "loclprot.h"

#define GEOLIB_UNUSED_ARG(a) do {/* null */} while (&a == 0)

extern char templogname[];

static struct DATUMDEF indtmparm;	/* Array of in datum parameters */
static struct DATUMDEF outdtmparm;	/* Array of out datum parameters */
long ( *for_trans[MAXPROJ + 1] ) (  );	/* forward function pointer array */
long ( *inv_trans[MAXPROJ + 1] ) (  );	/* inverse function pointer array */
long datumflag;			/* Flag for type of datum conversion */

int c_transinit( long *inproj,	/* I: Input projection number */
		 long *inunit,	/* I: Input projection unit code */
		 long *inzone,	/* I: Input projection zone number */
		 long *indtmnum,	/* I: Input datum number */
		 double inparm[15],	/* I: 15 input projection parameters */
		 long *outproj,	/* I: Output projection number */
		 long *outunit,	/* I: Output projection unit code */
		 long *outzone,	/* I: Output projection zone number */
		 long *outdtmnum,	/* I: Output datum number */
		 double outparm[15],	/* I: 15 output projection parameters */
		 long prtprm[2],	/* I: Report destination.  A TRUE/FALSE
                                              toggle is used: prtprm[GEO_TERM],
                                              prtprm[GEO_FILE] */
		 char *fname    /* I: File name if printing to file, NULL if
                                      not */
     )
{
    long iflg = 0;		/* error flag */

    char fn27[CMLEN];		/* name of NAD 1927 parameter file */
    char fn83[CMLEN];		/* name of NAD 1983 parameter file */
    char *ptr;			/* Point to lastables */
    char lastables[CMLEN];	/* Storage for lastables */

    /* fname is not used.  Use the GEOLIB_UNUSED_ARG macro to quiet the
     * compiler.
     */
    GEOLIB_UNUSED_ARG(fname);

/* Do preliminary error checking on parameters where possible
 -----------------------------------------------------------*/
    if ( ( *inproj < 0 ) || ( *inproj > MAXPROJ ) )
	return ( proj_err( 1001 ) );

    if ( ( *outproj < 0 ) || ( *outproj > MAXPROJ ) )
	return ( proj_err( 1002 ) );

    if ( ( *inunit < 0 ) || ( *inunit > MAXUNIT ) )
	return ( proj_err( 1003 ) );

    if ( ( *outunit < 0 ) || ( *outunit > MAXUNIT ) )
	return ( proj_err( 1004 ) );

    if ( ( *inproj != GEO ) && ( *inunit != METER ) && ( *inunit != FEET ) )
	return ( proj_err( 1005 ) );

    if ( ( *outproj != GEO ) && ( *outunit != METER ) && ( *outunit != FEET ) )
	return ( proj_err( 1006 ) );

/* Check for valid DMS angles 
 ---------------------------*/
    if ( ( *inproj != SOM ) && ( check_dms( inparm[3] ) != E_GEO_SUCC ) )
	return ( proj_err( 1116 ) );

    if ( check_dms( inparm[4] ) != E_GEO_SUCC )
	return ( proj_err( 1116 ) );

    if ( check_dms( inparm[5] ) != E_GEO_SUCC )
	return ( proj_err( 1116 ) );

    if ( ( *outproj != SOM ) && ( check_dms( outparm[3] ) != E_GEO_SUCC ) )
	return ( proj_err( 1116 ) );

    if ( check_dms( outparm[4] ) != E_GEO_SUCC )
	return ( proj_err( 1116 ) );

    if ( check_dms( outparm[5] ) != E_GEO_SUCC )
	return ( proj_err( 1116 ) );

    if ( ( *inproj == ALBERS ) || ( *inproj == LAMCC ) ||
         ( *inproj == EQUIDC ) )
	if ( check_dms( inparm[2] ) != E_GEO_SUCC )
	    return ( proj_err( 1116 ) );

    if ( ( *outproj == ALBERS ) || ( *outproj == LAMCC ) ||
         ( *outproj == EQUIDC ) )
	if ( check_dms( outparm[2] ) != E_GEO_SUCC )
	    return ( proj_err( 1116 ) );

/* Pack spheroid and datum codes into the parameter arrays
 --------------------------------------------------------*/
    indtmparm.datumnum = *indtmnum;
    outdtmparm.datumnum = *outdtmnum;

    if ( packdatum( *inproj, *outproj, &indtmparm, &outdtmparm,
		    inparm, outparm, &datumflag ) != E_GEO_SUCC )
	return ( E_GEO_FAIL );

/* Place State Plane directory in fn27, fn83
------------------------------------------*/
    ptr = (char *)getenv("MRT_DATA_DIR");
    if( ptr == NULL ) {
       ptr = (char *)getenv("MRTDATADIR");
       if (ptr == NULL) {
          ErrorHandler( FALSE, "c_transinit", ERROR_ENV,
             "MRT_DATA_DIR or MRTDATADIR not defined");
          return(E_GEO_FAIL);
       }
    }
 
    strcpy(lastables, ptr);

    sprintf( fn27, "%s/nad27sp", lastables );
    sprintf( fn83, "%s/nad83sp", lastables );

/* Initialize inverse transformation for c_trans.  We will pass the input 
   sphere number so gctp will use the semi-major and minor axis values that
   are defined for that sphere.  The projection parameter values will only
   be read if the sphere number is set to -1.  However, it was decided that
   this would not be done so the same values are used as in the datum 
   conversions.
 -------------------------------------------------------------------------*/

    /* either report nothing or everything */
    if ( prtprm[GEO_TERM] == GEO_FALSE && prtprm[GEO_FILE] == GEO_FALSE )
        init( -1, -1, NULL, NULL );
    else 
        init( -1, 2, NULL, templogname );

    inv_init( *inproj, *inzone, inparm, indtmparm.spherenum, fn27, fn83, &iflg,
	      inv_trans );
    if ( iflg != 0 )
    {
	ErrorHandler( FALSE, "c_transinit", ERROR_PROJECTION,
		      "Error returned from inverse initialization" );
	return ( E_GEO_FAIL );
    }

/* Initialize forward transformation for c_trans
 -----------------------------------------------*/
    for_init( *outproj, *outzone, outparm, outdtmparm.spherenum, fn27, fn83,
              &iflg, for_trans );
    if ( iflg != 0 )
    {
	ErrorHandler( FALSE, "c_transinit", ERROR_PROJECTION,
		      "Error returned from forward initialization" );
	return ( E_GEO_FAIL );
    }

/* Write projection parameters to output device
 ---------------------------------------------*/
/*    if ( prtprm[GEO_TERM] == GEO_TRUE )
    {
	proj_report( *inproj, *inzone, inparm, "Input Projection:" );
	proj_report( *outproj, *outzone, outparm, "Output Projection:" );
    }
    if ( prtprm[GEO_FILE] == GEO_TRUE )
    {
	proj_print( *inproj, *inzone, inparm, fname, "Input Projection:" );
	proj_print( *outproj, *outzone, outparm, fname, "Output Projection:" );
    }
*/

    return ( E_GEO_SUCC );
}

/*******************************************************************************
NAME			      C_TRANS

PURPOSE	    Converts coordinates from one map projection and/or datum 
            to another 

PROGRAM HISTORY
PROGRAMMER		DATE		REASON
----------		----		------
J. Willems		Apr. 1998	Based on c_proj.  Modified for datum
					transformations.
G. Schmidt              Nov. 2001       Check for ISIN_ERANGE to be returned
                                        from coord2geo and return it from
                                        c_trans as well.
G. Schmidt              Nov. 2002       Changed ISIN_ERANGE to GCTP_ERANGE.

PROJECT     LAS

ALGORITHM 
	Swap SOM point system
	Convert points to geographic
	Do datum Transformation
	Convert to output projection and unit
	Swap point if out is SOM

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

int c_trans( long *inproj,	/* Input projection code */
	     long *inunit,	/* Input projection units code */
	     long *outproj,	/* Output projection code */
	     long *outunit,	/* Output projection units code */
	     double *inx,	/* Input X projection coordinate */
	     double *iny,	/* Input Y projection coordinate */
	     double *outx,	/* Output X projection coordinate */
	     double *outy       /* Output Y projection coordinate */
     )
{
    int status;			/* routine return status */

    double incoor[2];		/* Input coordinates */
    double outcoor[2];		/* Output coordinates */
    double tempx;		/* Working value for x */
    double tempy;		/* Working value for y */
    double temp;		/* Temporary coordinate value for SOM swapping */

/* Swap X & Y if the input projection is SOM
  -----------------------------------------*/
    if ( *inproj == SOM )
    {
	tempy = *inx;
	tempx = -( *iny );
    }
    else
    {
	tempy = *iny;
	tempx = *inx;
    }

/* Convert input coordinate to geographic coords, in radians
 ----------------------------------------------------------*/
    if ( ( status = coord2geo( *inproj, *inunit, incoor, &tempx, &tempy ) ) ==
	 E_GEO_FAIL )
    {
	ErrorHandler( FALSE, "c_trans", ERROR_PROJECTION,
		  "Error returned from coordinate conversion" );
	return ( status );
    }

/* If the point was out of valid range, then exit
 -----------------------------------------------*/
    if ( status == GCTP_ERANGE )
    {
        return ( status );
    }

/* Do datum transformation
  -----------------------*/
    if ( datumflag != NO_TRANS )
    {
	if ( transform(incoor, indtmparm, outdtmparm, datumflag, outcoor) !=
	     E_GEO_SUCC )
	{
	    ErrorHandler( FALSE, "c_trans", ERROR_PROJECTION,
			  "Error returned from datum conversion" );
	    return ( E_GEO_FAIL );
	}
    }
    else
    {
	outcoor[0] = incoor[0];
	outcoor[1] = incoor[1];
    }

/*  Convert output coords from radians to ouput projection and out unit
  --------------------------------------------------------------------*/
    if ( geo2coord( *outproj, *outunit, outcoor, outx, outy ) != E_GEO_SUCC )
    {
	ErrorHandler( FALSE, "c_trans", ERROR_PROJECTION,
		      "Error returned from coordinate conversion" );
	return ( E_GEO_FAIL );
    }

/* If the output projection is SOM, swap the X & Y coordinates
  -----------------------------------------------------------*/
    if ( *outproj == SOM )
    {
	temp = *outx;
	*outx = *outy;
	*outy = -temp;
    }

/* Check the return status and return to calling function
  ------------------------------------------------------*/
    return ( E_GEO_SUCC );
}
