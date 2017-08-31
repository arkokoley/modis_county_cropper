/******************************************************************************

MODULE:  calc_isin_shift

PURPOSE: Calculate the ISIN shift for a particular line/sample value.

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         11/02  Gail Schmidt           Original Development
         11/02  Gail Schmidt           ISIN_ERANGE is changed to GCTP_ERANGE to
                                       also support SIN and other projs
         02/05  Gail Schmidt           Modified to resample from the center of
                                       a pixel
         01/07  Gail Schmidt           Modified the call to GCTP to send in
                                       a value of -1 for the sphere code since
                                       sphere codes aren't used for ISIN

HARDWARE AND/OR SOFTWARE LIMITATIONS:
  None

PROJECT:    MODIS Reprojection Tool

NOTES:
  ISIN data has a shift between lines in the image. The delta_s calculated
    for a particular line is the pixel shift for that line, in relation to
    the current line. The delta_s is linear along any given line.
  1. (lisin, sisin) --> (xisin, yisin) --> (lat_orig, long_orig)
  2. (lisin + 1, sisin) --> (xisin, yisin_prime) --> (lat_prime, long_prime)
  3. (lat_prime, long_orig) --> (xisin_prime, yisin_double_prime) -->
     (lisin_prime, sisin_prime)
  4. delta_s = sisin_prime - sisin
     OR delta_s = (xisin - xisin_prime) / input->pixel_size

  NOTE: yisin_double_prime should be equal to yisin_prime.
  NOTE: The resample algorithm works from the center of a pixel rather than
        the UL extents, so the input line/sample will be shifted to represent
        the center of a pixel instead of the UL extent.  This will allow the
        ISIN shift calculations to match up with the center of the pixel
        resampling that is done later.

******************************************************************************/
#include "resample.h"
#include "worgen.h"
#include "cproj.h"

int calc_isin_shift
(
    int lisin,                  /* I: input projection line coord */
    int sisin,                  /* I: input projection sample coord */
    ProjInfo *inproj,           /* I: input projection data for geolib */
    double upleft_x,            /* I: upper left input projection coord */
    double upleft_y,            /* I: upper left input projection coord */
    double input_pixel_size,    /* I: pixel size for input image */
    int output_datum_code,      /* I: output datum code */
    double *delta_s             /* O: shift for the ISIN shift calculation */
)

{
    char errmsg[256];           /* error message string */
    ProjInfo *geoproj, geoinfo; /* geographic projection data for geolib */
    int status = MRT_NO_ERROR;      /* return status error code */
    int i;                      /* looping variable */
    long prtprm[2];             /* logging flags for geolib */
    double sisin_prime;         /* s-prime ISIN value (ISIN sample value with
                                   line + 1) */
    double xisin, yisin,        /* x/y ISIN values */
           xisin_prime,
           yisin_prime,
           yisin_double_prime;
    double lat_orig, long_orig;    /* original lat/long */
    double lat_prime, long_prime;  /* lat/long for next line */

    /* Setup Geographic coordinates */
    geoinfo.proj_code = GEO;
    geoinfo.units = DEGREE;
    geoinfo.zone_code = 0;
    for ( i = 0; i < NUM_PROJECTION_PARAMS; i++ )
        geoinfo.proj_coef[i] = 0.0;
    geoproj = &geoinfo;
    geoproj->datum_code = inproj->datum_code;  /* always convert to
                                                  input lat/long */
    /* no printing from geolib */
    prtprm[GEO_TERM] = GEO_FALSE;
    prtprm[GEO_FILE] = GEO_FALSE;

    /** 1. (lisin, sisin) --> (xisin, yisin) --> (lat_orig, long_orig) **/

    /* get the input projection coords for the center of the pixel */
    xisin = upleft_x + sisin * input_pixel_size + 0.5 * input_pixel_size;
    yisin = upleft_y - lisin * input_pixel_size - 0.5 * input_pixel_size;

    /* go from input projection coords to input lat/long */
    if ( output_datum_code != E_NODATUM )
    {
        /* initialize the inverse projection */
        status = c_transinit( &inproj->proj_code, &inproj->units,
            &inproj->zone_code, &inproj->datum_code, inproj->proj_coef,
            &geoproj->proj_code, &geoproj->units, &geoproj->zone_code,
            &geoproj->datum_code, geoproj->proj_coef, prtprm, NULL );
        if ( status != MRT_NO_ERROR )
        {
            ErrorHandler( FALSE, "calc_isin_shift", ERROR_GENERAL,
               "Error in initializing the inverse projection (c_transinit)" );
            return ( status );
        }

        /* do the inverse projection */
        status = c_trans( &inproj->proj_code, &inproj->units,
            &geoproj->proj_code, &geoproj->units, &xisin, &yisin, &long_orig,
            &lat_orig );
        if ( status == GCTP_ERANGE )
        {   /* if this is a bounding tile pixel value then just return
               and let the calling routine handle the issue */
            return( status );
        }
        else if ( status != E_GEO_SUCC )
        {
            ErrorHandler( FALSE, "calc_isin_shift", ERROR_GENERAL,
                "Error converting input projection coordinates to "
                "lat/long coordinates for delta_s calculation." );
            return( status );
        }
    }
    else
    {
        /* Call GCTP directly to allow the semi-major and semi-minor
           to be specified directly.  Both the input and output
           sphere values need to be -1, and thus the projection
           parameters for both input and output will be used. */
        status = gctp_call( inproj->proj_code, inproj->zone_code,
            -1, inproj->proj_coef, inproj->units, xisin, yisin,
            geoproj->proj_code, geoproj->zone_code, -1, geoproj->proj_coef,
            geoproj->units, &long_orig, &lat_orig );
        if ( status == GCTP_ERANGE )
        {   /* if this is a bounding tile pixel value then just return
               and let the calling routine handle the issue */
            return( status );
        }
        else if ( status != E_GEO_SUCC )
        {
            ErrorHandler( FALSE, "calc_isin_shift", ERROR_GENERAL,
                "Error converting input projection coordinates to "
                "lat/long coordinates for delta_s calculation." );
            return( status );
        }
    }

    /** 2. (lisin + 1, sisin) --> (xisin, yisin_prime) --> 
           (lat_prime, long_prime) **/

    /* calculate projection coords for (lisin + 1, sisin). xisin is
       already available from earlier. yisin_prime is yisin + pixel_size. */
    yisin_prime = yisin + input_pixel_size;

    /* go from input projection coords to input lat/long */
    if ( output_datum_code != E_NODATUM )
    {
        /* initialize the inverse projection */
        status = c_transinit( &inproj->proj_code, &inproj->units,
            &inproj->zone_code, &inproj->datum_code, inproj->proj_coef,
            &geoproj->proj_code, &geoproj->units, &geoproj->zone_code,
            &geoproj->datum_code, geoproj->proj_coef, prtprm, NULL );
        if ( status != MRT_NO_ERROR )
        {
            ErrorHandler( FALSE, "calc_isin_shift", ERROR_GENERAL,
               "Error in initializing the inverse projection (c_transinit)" );
            return ( status );
        }

        /* do the inverse projection */
        status = c_trans( &inproj->proj_code, &inproj->units,
            &geoproj->proj_code, &geoproj->units, &xisin, &yisin_prime,
            &long_prime, &lat_prime );
        if ( status == GCTP_ERANGE )
        {   /* if this is a bounding tile pixel value then just return
               and let the calling routine handle the issue */
            return( status );
        }
        else if ( status != E_GEO_SUCC )
        {
            ErrorHandler( FALSE, "calc_isin_shift", ERROR_GENERAL,
                "Error converting input projection coordinates to "
                "lat/long coordinates for delta_s calculation." );
            return( status );
        }
    }
    else
    {
        /* Call GCTP directly to allow the semi-major and semi-minor
           to be specified directly.  Both the input and output
           sphere values need to be -1, and thus the projection
           parameters for both input and output will be used. */
        status = gctp_call( inproj->proj_code, inproj->zone_code,
            -1, inproj->proj_coef, inproj->units, xisin, yisin_prime,
            geoproj->proj_code, geoproj->zone_code, -1, geoproj->proj_coef,
            geoproj->units, &long_prime, &lat_prime );
        if ( status == GCTP_ERANGE )
        {   /* if this is a bounding tile pixel value then just return
               and let the calling routine handle the issue */
            return( status );
        }
        else if ( status != E_GEO_SUCC )
        {
            ErrorHandler( FALSE, "calc_isin_shift", ERROR_GENERAL,
                "Error converting input projection coordinates to "
                "lat/long coordinates for delta_s calculation." );
            return( status );
        }
    }

    /** 3. (lat_prime, long_orig) --> (xisin_prime, yisin_double_prime) -->
           (lisin_prime, sisin_prime) **/

    /* go from input lat/long to input projection coords */
    if ( output_datum_code != E_NODATUM )
    {
        /* initialize the forward projection */
        status = c_transinit( &geoproj->proj_code, &geoproj->units,
            &geoproj->zone_code, &geoproj->datum_code, geoproj->proj_coef,
            &inproj->proj_code, &inproj->units, &inproj->zone_code,
            &inproj->datum_code, inproj->proj_coef, prtprm, NULL );
        if ( status != MRT_NO_ERROR )
        {
            ErrorHandler( FALSE, "calc_isin_shift", ERROR_GENERAL,
               "Error in initializing the forward projection (c_transinit)" );
            return ( status );
        }

        /* do the forward projection */
        status = c_trans( &geoproj->proj_code, &geoproj->units,
            &inproj->proj_code, &inproj->units, &long_orig, &lat_prime,
            &xisin_prime, &yisin_double_prime );
        if ( status == GCTP_ERANGE )
        {   /* if this is a bounding tile pixel value then just return
               and let the calling routine handle the issue */
            return( status );
        }
        else if ( status != E_GEO_SUCC )
        {
            ErrorHandler( FALSE, "calc_isin_shift", ERROR_GENERAL,
                "Error converting lat/long coordinates to input "
                "projection coordinates for delta_s calculation." );
            return( status );
        }
    }
    else
    {
        /* Call GCTP directly to allow the semi-major and semi-minor
           to be specified directly.  Both the input and output
           sphere values need to be -1, and thus the projection
           parameters for both input and output will be used. */
        status = gctp_call( geoproj->proj_code, geoproj->zone_code,
            -1, geoproj->proj_coef, geoproj->units, long_orig, lat_prime,
            inproj->proj_code, inproj->zone_code, -1, inproj->proj_coef,
            inproj->units, &xisin_prime, &yisin_double_prime );
        if ( status == GCTP_ERANGE )
        {   /* if this is a bounding tile pixel value then just return
               and let the calling routine handle the issue */
            return( status );
        }
        else if ( status != E_GEO_SUCC )
        {
            ErrorHandler( FALSE, "calc_isin_shift", ERROR_GENERAL,
                "Error converting lat/long coordinates to input "
                "projection coordinates for delta_s calculation." );
            return( status );
        }
    }

    /* sanity check (NOTE: yisin_double_prime should be equal to
       yisin_prime) */
    if ( fabs ( yisin_double_prime - yisin_prime ) > .000005 )
    {
        sprintf( errmsg, "Error: yisin_double_prime (%.9f) should be equal "
            "to yisin_prime (%.9f).\n", yisin_double_prime, yisin_prime );
        ErrorHandler( TRUE, "calc_isin_shift", ERROR_GENERAL, errmsg );
    } 

    /* get the line/sample values for the input projection coords */
/*    lisin_prime = ( upleft_y - yisin_double_prime ) / input_pixel_size; */
    sisin_prime = ( xisin_prime - upleft_x ) / input_pixel_size;

    /** 4. delta_s = sisin_prime - sisin
           OR delta_s = (xisin - xisin_prime) / input_pixel_size **/
    *delta_s = sisin_prime - sisin;

    return ( status );
}
