
/******************************************************************************

FILE:  gctp_call

PURPOSE:  To call GCTP directly. GCTP parameters are set up first, then GCTP
          is called using the input and output file information.

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         04/02  Gail Schmidt           Original development
         11/02  Gail Schmidt           ISIN_ERANGE is changed to GCTP_ERANGE to
                                       also support SIN and other projs
         02/03  Gail Schmidt           Added support for GCTP_IN_BREAK returned
                                       from Interrupted Goode's
         01/07  Gail Schmidt           Modified the module to allow the input
                                       and output sphere codes to be specified
                                       for UTM projections.

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:
  This is a temporary fix to allow the user to call GCTP directly and specify
  their own output semi-major/minor or radius values.  The current version of
  Geolib will not allow a sphere value of -1, which allows GCTP to use the
  specified values in the projection parameters.

******************************************************************************/

#include "shared_resample.h"
#include "worgen.h"
#include "cproj.h"

int gctp_call
(
    long in_proj_code,
    long in_zone_code,       /* not used except for UTM */
    long in_sphere_code,     /* not used except for UTM */
    double in_proj_parms[],
    long in_units,
    double in_x,
    double in_y,
    long out_proj_code,
    long out_zone_code,      /* not used except for UTM */
    long out_sphere_code,    /* not used except for UTM */
    double out_proj_parms[],
    long out_units,
    double *out_x,
    double *out_y
)

{
    long iflg = 0;		/* error flag */
    long in_spheroid;           /* input sphere value */
    long out_spheroid;          /* output sphere value */
    long ipr;                   /* printout flag */
    char fn27[CMLEN];		/* name of NAD 1927 parameter file */
    char fn83[CMLEN];		/* name of NAD 1983 parameter file */
    char *ptr;			/* point to mrttables */
    char mrttables[CMLEN];	/* storage for mrttables */
    double in_coords[2];        /* input x/y coords */
    double out_coords[2];       /* output x/y coords */

    /* Place State Plane directory in fn27, fn83 */
    ptr = (char *)getenv( "MRT_DATA_DIR" );
    if( ptr == NULL ) {
       ptr = (char *)getenv( "MRTDATADIR" );
       if (ptr == NULL) {
          ErrorHandler( FALSE, "gctp_call", ERROR_ENV,
            "MRT_DATA_DIR nor MRTDATADIR not defined" );
          return( E_GEO_FAIL );
       }
    }
 
    strcpy(mrttables, ptr);
    sprintf( fn27, "%s/nad27sp", mrttables );
    sprintf( fn83, "%s/nad83sp", mrttables );

    /* If not processing UTM, then use the input projection parameters for
       the spheroid information. Otherwise use the spheroid passed to the
       routine. The projection parameters for UTM don't allow the spheroid
       to be specified. */
    if (in_proj_code != UTM)
        in_spheroid = -1;
    else
        in_spheroid = in_sphere_code;

    /* If not processing UTM, then use the output projection parameters for
       the spheroid information. Otherwise use the spheroid passed to the
       routine. The projection parameters for UTM don't allow the spheroid
       to be specified. */
    if (out_proj_code != UTM)
        out_spheroid = -1;
    else
        out_spheroid = out_sphere_code;

    /* Set up the input coordinates */
    in_coords[0] = in_x;
    in_coords[1] = in_y;
    ipr = -1;    /* don't print error messages */

    /* Call GCTP */
    gctp( in_coords, &in_proj_code, &in_zone_code, in_proj_parms, &in_units,
        &in_spheroid, &ipr, NULL, &ipr, NULL, out_coords, &out_proj_code,
        &out_zone_code, out_proj_parms, &out_units, &out_spheroid, fn27,
        fn83, &iflg );
    if ( iflg == GCTP_ERANGE || iflg == GCTP_IN_BREAK )
        return iflg;
    else if ( iflg != 0 )
    {
        ErrorHandler( TRUE, "GCTP_CALL", ERROR_GENERAL,
            "Error projecting input coordinates to output coordinates." );
    }

    *out_x = out_coords[0];
    *out_y = out_coords[1];

    return E_GEO_SUCC;
}
