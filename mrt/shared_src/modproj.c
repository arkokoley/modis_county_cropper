/******************************************************************************

FILE:  modproj.c

PURPOSE:  Various projection handling routines

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  Rob Burrell            Original Development
         07/01  Gail Schmidt           Checked the return status from
                                       c_transinit and c_trans
                                       Made some routines error out when
                                       an error occurred, since the calling
                                       routines do not check the error
                                       status.
         11/01  Gail Schmidt           Handle ISIN_ERANGE returned from c_trans
         04/02  Gail Schmidt           Support calling GCTP directly
         11/02  Gail Schmidt           Add support for Albers Equal Area
         11/02  Gail Schmidt           ISIN_ERANGE is changed to GCTP_ERANGE to
                                       also support SIN and other projs
         12/02  Gail Schmidt           Added support for Mercator projection
         02/03  Gail Schmidt           Added support for GCTP_IN_BREAK returned
                                       from Interrupted Goode's
         01/07  Gail Schmidt           Modified the call to GCTP to send in
                                       the sphere code which will be used
                                       for UTM only

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:
  Watch out for the <values.h>, some versions of MSVC
  don't have this file and include things in <math.h>
  instead.

******************************************************************************/
#if !defined(__CYGWIN__) && !defined(__APPLE__) && !defined(WIN32)
#include <values.h>
#endif
#include <math.h>
#include "shared_mosaic.h"
#include "mrt_dtype.h"
#include "worgen.h"
#include "cproj.h"

/******************************************************************************

MODULE:  CheckProjectionParams

PURPOSE:  Attempts to initialize geolib with the given projection parameters

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status		See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  Rob Burrell            Original Development
         06/01  Gail Schmidt           Error check for NULL pointers

NOTES:

******************************************************************************/
int CheckProjectionParams
    (
        ModisDescriptor * modis     /* I:  session info */
    )
{
	ProjInfo *inproj = NULL,        /* input/output projection data for */
                 *outproj = NULL;       /* geolib */
	long prtprm[2];                 /* geolib terminal output flags */
        long status;                    /* return status */

	/* to reduce clutter, use some locals */
	inproj = modis->in_projection_info = GetInputProjection( modis );
        if (inproj == NULL)
        {
            ErrorHandler( TRUE, "CheckProjectionParams", ERROR_GENERAL,
                          "Bad Input Projection Type" );
            return E_GEO_FAIL;
        }

	outproj = modis->out_projection_info = GetOutputProjection( modis );
        if (outproj == NULL)
        {
            ErrorHandler( TRUE, "CheckProjectionParams", ERROR_GENERAL,
                          "Bad Output Projection Type" );
            return E_GEO_FAIL;
        }

        /* initialize and return the value */
        if ( modis->output_datum_code != E_NODATUM )
        {
	    /* print to terminal and to log file */
	    prtprm[GEO_TERM] = GEO_TRUE;
	    prtprm[GEO_FILE] = GEO_TRUE;

	    status = c_transinit( &inproj->proj_code, &inproj->units,
                &inproj->zone_code, &inproj->datum_code, inproj->proj_coef,
		&outproj->proj_code, &outproj->units, &outproj->zone_code,
		&outproj->datum_code, outproj->proj_coef, prtprm, NULL );
            return status;
        }
        else
        {   /* We will call GCTP directly */
            return MRT_NO_ERROR;
        }
}

/******************************************************************************

MODULE:  GetInputProjection

PURPOSE:  Creates a projection data structure for the input projection

RETURN VALUE:
Type = ProjInfo
Value           Description
-----           -----------
ProjInfo        Projection parameters

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  Rob Burrell            Original Development
         12/00  Rob Burrell            Fix datum code, migrate back to
                                       ModisDescriptor when not supplied.
         04/01  Rob Burrell            Use datum codes in shared_resample.h
         07/01  Rob Burrell	       Add all supported projections
         07/01  Rob Burrell            Fix datum stuff
         07/01  Gail Schmidt           Use the specified datum code

NOTES:

******************************************************************************/
ProjInfo *GetInputProjection
    (
        ModisDescriptor *modis     /* I:  session info */
    )
{
    ProjInfo *inprojection = NULL;  /* projection data structure */
/*    long datumnum, datumvalid; */     /* used in spheroid check */
    char errstr[256];               /* string for error messages */

    /* create a projection data structure */
    inprojection = ( ProjInfo * ) calloc( 1, sizeof( ProjInfo ) );
    if ( !inprojection )
        ErrorHandler( TRUE, "GetInputProjection", ERROR_MEMORY, "ProjInfo" );

    /* initialize using the input projection type for case */
    switch ( modis->input_projection_type )
    {
        case PROJ_ISIN:
            inprojection->proj_code = ISINUS;
            inprojection->units = METER;
            inprojection->zone_code = 0;
	    inprojection->datum_code = modis->input_datum_code;
            break;

        case PROJ_UTM:
            inprojection->proj_code = UTM;
            inprojection->units = METER;

            /* if no zone code, create one */
            if ( modis->input_zone_code == 0 )
            {
                modis->input_zone_code =
                    GetUTMZone( modis->input_projection_parameters[0],
                                modis->input_projection_parameters[1] );
            }

            inprojection->zone_code = modis->input_zone_code;
            inprojection->datum_code = modis->input_datum_code;
            break;

        case PROJ_AEA:
            inprojection->proj_code = ALBERS;
            inprojection->units = METER;
            inprojection->zone_code = 0;
            inprojection->datum_code = modis->input_datum_code;
            break;

        case PROJ_ER:
            inprojection->proj_code = EQRECT;
            inprojection->units = METER;
            inprojection->zone_code = 0;
            inprojection->datum_code = modis->input_datum_code;
            break;

        case PROJ_GEO:
            inprojection->proj_code = GEO;
            inprojection->units = DEGREE;
            inprojection->zone_code = 0;
            inprojection->datum_code = modis->input_datum_code;
            break;

        case PROJ_LA:
            inprojection->proj_code = LAMAZ;
            inprojection->units = METER;
            inprojection->zone_code = 0;
            inprojection->datum_code = modis->input_datum_code;
            break;

        case PROJ_HAM:
            inprojection->proj_code = HAMMER;
            inprojection->units = METER;
            inprojection->zone_code = 0;
            inprojection->datum_code = modis->input_datum_code;
            break;

        case PROJ_IGH:
            inprojection->proj_code = GOOD;
            inprojection->units = METER;
            inprojection->zone_code = 0;
            inprojection->datum_code = modis->input_datum_code;
            break;

        case PROJ_LCC:
            inprojection->proj_code = LAMCC;
            inprojection->units = METER;
            inprojection->zone_code = 0;
            inprojection->datum_code = modis->input_datum_code;
            break;

        case PROJ_MERC:
            inprojection->proj_code = MERCAT;
            inprojection->units = METER;
            inprojection->zone_code = 0;
            inprojection->datum_code = modis->input_datum_code;
            break;

        case PROJ_MOL:
            inprojection->proj_code = MOLL;
            inprojection->units = METER;
            inprojection->zone_code = 0;
            inprojection->datum_code = modis->input_datum_code;
            break;

        case PROJ_PS:
            inprojection->proj_code = PS;
            inprojection->units = METER;
            inprojection->zone_code = 0;
            inprojection->datum_code = modis->input_datum_code;
            break;

        case PROJ_SIN:
            inprojection->proj_code = SNSOID;
            inprojection->units = METER;
            inprojection->zone_code = 0;
            inprojection->datum_code = modis->input_datum_code;
            break;

        case PROJ_TM:
            inprojection->proj_code = TM;
            inprojection->units = METER;
            inprojection->zone_code = 0;
            inprojection->datum_code = modis->input_datum_code;
            break;

        default:
            ErrorHandler( TRUE, "GetInputProjection", ERROR_GENERAL,
                          "Bad Projection Type" );
            DestroyProjectionInfo( inprojection );
            inprojection = NULL;
            break;
    }
    inprojection->sphere_code = modis->input_sphere_code;

    /* copy the projection parameters */
    memcpy( inprojection->proj_coef, modis->input_projection_parameters,
            NUM_PROJECTION_PARAMS * sizeof( double ) );

    /* convert from degrees to packed DMS */
    if ( !Deg2DMS( modis->input_projection_type, inprojection->proj_coef ) )
    {
        sprintf( errstr, "Error converting from degrees to DMS\n" );
        ErrorHandler( TRUE, "GetInputProjection", ERROR_GENERAL, errstr );
    }

    return ( inprojection );
}

/******************************************************************************

MODULE:  GetInputProjectionMosaic

PURPOSE:  Creates a projection data structure for the input projection

RETURN VALUE:
Type = ProjInfo
Value           Description
-----           -----------
ProjInfo        Projection parameters

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         04/02  Gail Schmidt           Original Development
         11/02  Gail Schmidt           SIN data will also be supported

NOTES:

******************************************************************************/
ProjInfo *GetInputProjectionMosaic
    (
        MosaicDescriptor *mosaic     /* I:  session info */
    )
{
    ProjInfo *inprojection = NULL;  /* projection data structure */
    char errstr[256];               /* string for error messages */

    /* create a projection data structure */
    inprojection = ( ProjInfo * ) calloc( 1, sizeof( ProjInfo ) );
    if ( !inprojection )
        ErrorHandler( TRUE, "GetInputProjectionMosaic", ERROR_MEMORY,
            "ProjInfo" );

    /* initialize using the input projection type for case */
    switch ( mosaic->projection_type )
    {
        case PROJ_ISIN:
            inprojection->proj_code = ISINUS;
            inprojection->units = METER;
            inprojection->zone_code = 0;
            inprojection->datum_code = mosaic->datum_code;
            break;

        case PROJ_SIN:
            inprojection->proj_code = SNSOID;
            inprojection->units = METER;
            inprojection->zone_code = 0;
            inprojection->datum_code = mosaic->datum_code;
            break;

        default:
            ErrorHandler( TRUE, "GetInputProjectionMosaic", ERROR_GENERAL,
                "Unsupported projection type. Mosaicking only supports the "
                "SIN and ISIN projections on input." );
            DestroyProjectionInfo( inprojection );
            inprojection = NULL;
            break;
    }
    inprojection->sphere_code = mosaic->input_sphere_code;

    /* copy the projection parameters */
    memcpy( inprojection->proj_coef, mosaic->projection_parameters,
            NUM_PROJECTION_PARAMS * sizeof( double ) );

    /* convert from degrees to packed DMS */
    if ( !Deg2DMS( mosaic->projection_type, inprojection->proj_coef ) )
    {
        sprintf( errstr, "Error converting from degrees to DMS\n" );
        ErrorHandler( TRUE, "GetInputProjectionMosaic", ERROR_GENERAL, errstr );    }

    return ( inprojection );
}

/******************************************************************************

MODULE:  GetOutputProjection

PURPOSE:  Creates a projection data structure for the output projection

RETURN VALUE:
Type = ProjInfo
Value           Description
-----           -----------
ProjInfo        Projection parameters

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  Rob Burrell            Original Development
         02/01  Rob Burrell            Use unit parameter
         04/01  Rob Burrell            Added TM projection
         04/01  Rob Burrell            Ellipse codes for Geo and UTM
         06/01  Gail Schmidt           Added a break for the TM projection,
                                       otherwise the code processes the
                                       default statement as well
         06/01  Gail Schmidt           default statement in switch must
                                       return NULL and not continue processing
         01/07  Gail Schmidt           Set the output sphere code to the
                                       input sphere code if both input and
                                       output projections are UTM

NOTES:

******************************************************************************/
ProjInfo *GetOutputProjection
    (
        ModisDescriptor * modis     /* I:  session info */
    )
{
    ProjInfo *outprojection;        /* projection data structure */
    char errstr[256];               /* string for error messages */

    /* allocate a structure */
    outprojection = ( ProjInfo * ) calloc( 1, sizeof( ProjInfo ) );
    if ( !outprojection )
        ErrorHandler( TRUE, "GetOutputProjection", ERROR_MEMORY, "ProjInfo" );

    /* initialize using the output projection type for case */
    switch ( modis->output_projection_type )
    {
        case PROJ_AEA:
            outprojection->proj_code = ALBERS;
            outprojection->units = METER;
            if( modis->output_units >= 0 )
                outprojection->units = modis->output_units;
            outprojection->zone_code = 0;
            outprojection->datum_code = modis->output_datum_code;
            break;

        case PROJ_ER:
            outprojection->proj_code = EQRECT;
            outprojection->units = METER;
            if( modis->output_units >= 0 )
                outprojection->units = modis->output_units;
            outprojection->zone_code = 0;
            outprojection->datum_code = modis->output_datum_code;
            break;

        case PROJ_GEO:
            outprojection->proj_code = GEO;
            outprojection->units = DEGREE;
            if( modis->output_units >= 0 )
                outprojection->units = modis->output_units;
            outprojection->zone_code = 0;
            outprojection->datum_code = modis->output_datum_code;
            break;

        case PROJ_ISIN:
            outprojection->proj_code = ISINUS;
            outprojection->units = METER;
            if( modis->output_units >= 0 )
                outprojection->units = modis->output_units;
            outprojection->zone_code = 0;
            outprojection->datum_code = modis->output_datum_code;
            break;

        case PROJ_LA:
            outprojection->proj_code = LAMAZ;
            outprojection->units = METER;
            if( modis->output_units >= 0 )
                outprojection->units = modis->output_units;
            outprojection->zone_code = 0;
            outprojection->datum_code = modis->output_datum_code;
            break;

        case PROJ_UTM:
            outprojection->proj_code = UTM;
            outprojection->units = METER;
            if( modis->output_units >= 0 )
                outprojection->units = modis->output_units;
                
            /* if no zone code, create one */
            if ( modis->output_zone_code == 0 )
            {
                modis->output_zone_code =
                    GetUTMZone( modis->output_projection_parameters[0], 
                                modis->output_projection_parameters[1] );
            }

            outprojection->zone_code = modis->output_zone_code;
            outprojection->datum_code = modis->output_datum_code;
            break;

        case PROJ_HAM:
            outprojection->proj_code = HAMMER;
            outprojection->units = METER;
            if( modis->output_units >= 0 )
                outprojection->units = modis->output_units;
            outprojection->zone_code = 0;
            outprojection->datum_code = modis->output_datum_code;
            break;

        case PROJ_IGH:
            outprojection->proj_code = GOOD;
            outprojection->units = METER;
            if( modis->output_units >= 0 )
                outprojection->units = modis->output_units;
            outprojection->zone_code = 0;
            outprojection->datum_code = modis->output_datum_code;
            break;

        case PROJ_LCC:
            outprojection->proj_code = LAMCC;
            outprojection->units = METER;
            if( modis->output_units >= 0 )
                outprojection->units = modis->output_units;
            outprojection->zone_code = 0;
            outprojection->datum_code = modis->output_datum_code;
            break;

        case PROJ_MERC:
            outprojection->proj_code = MERCAT;
            outprojection->units = METER;
            if( modis->output_units >= 0 )
                outprojection->units = modis->output_units;
            outprojection->zone_code = 0;
            outprojection->datum_code = modis->output_datum_code;
            break;

        case PROJ_MOL:
            outprojection->proj_code = MOLL;
            outprojection->units = METER;
            if( modis->output_units >= 0 )
                outprojection->units = modis->output_units;
            outprojection->zone_code = 0;
            outprojection->datum_code = modis->output_datum_code;
            break;

        case PROJ_PS:
            outprojection->proj_code = PS;
            outprojection->units = METER;
            if( modis->output_units >= 0 )
                outprojection->units = modis->output_units;
            outprojection->zone_code = 0;
            outprojection->datum_code = modis->output_datum_code;
            break;

        case PROJ_SIN:
            outprojection->proj_code = SNSOID;
            outprojection->units = METER;
            if( modis->output_units >= 0 )
                outprojection->units = modis->output_units;
            outprojection->zone_code = 0;
            outprojection->datum_code = modis->output_datum_code;
            break;

        case PROJ_TM:
            outprojection->proj_code = TM;
            outprojection->units = METER;
            if( modis->output_units >= 0 )
                outprojection->units = modis->output_units;
            outprojection->zone_code = 0;
            outprojection->datum_code = modis->output_datum_code;
            break;

        default:
            ErrorHandler( FALSE, "GetOutputProjection", ERROR_GENERAL,
                          "Bad Projection Type" );
            DestroyProjectionInfo( outprojection );
            outprojection = NULL;
            return ( NULL );  /* must return NULL otherwise the memcpy
                                 will try to write to a NULL ptr - GLS */
    }
    
    /* set up the output sphere. if both input and output projections are
       UTM, then set the output sphere to the input sphere. */
    if (modis->input_projection_type == PROJ_UTM &&
        modis->output_projection_type == PROJ_UTM)
        outprojection->sphere_code = modis->input_sphere_code;
    else
        outprojection->sphere_code = -1;

    /* copy the projection parameters */
    memcpy( outprojection->proj_coef, modis->output_projection_parameters,
            NUM_PROJECTION_PARAMS * sizeof( double ) );

    /* convert from degrees to packed DMS */
    if ( !Deg2DMS( modis->output_projection_type, outprojection->proj_coef ) )
    {
        sprintf( errstr, "Error converting from degrees to DMS\n" );
        ErrorHandler( TRUE, "GetOutputProjection", ERROR_GENERAL, errstr );
    }

    return ( outprojection );
}

/******************************************************************************

MODULE:  GetInputImageCorners

PURPOSE:  Converts the input lat/long image corners to input projection
          coordinates for the input image file descriptor

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status          See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  Rob Burrell            Original Development
         10/00  Rob Burrell	       Fix datum
         02/01  Rob Burrell	       Break out input/output routines

NOTES:

******************************************************************************/
int GetInputImageCorners
    (
        ModisDescriptor *modis,    /* I:  session info */
        FileDescriptor *file       /* I/O:  file for which we need corners */
    )
{
    ProjInfo *geoproj, *outproj;  /* geo/output projection data structures */
    ProjInfo projinfo;            /* local for geographic projection */
    int status = ERROR_GENERAL;   /* error code status */
    long prtprm[2];               /* geolib terminal printing flags */
    int i;                         

    /* Print nothing */
    prtprm[GEO_TERM] = GEO_FALSE;
    prtprm[GEO_FILE] = GEO_FALSE;

    /* setup to use Geographic coordinates */
    projinfo.proj_code = GEO;
    projinfo.units = DEGREE;
    projinfo.zone_code = 0;
    for ( i = 0; i < NUM_PROJECTION_PARAMS; i++ )
        projinfo.proj_coef[i] = 0.0;

    /* keep it clean with a pointer */
    geoproj = &projinfo;

    /* using the input projection information */
    outproj = modis->in_projection_info;
        
    /* since we are converting to input projection coords from input
       lat/long, use the input datum code */
    geoproj->datum_code = outproj->datum_code;
            
    if ( modis->output_datum_code != E_NODATUM )
    {   /* Don't call c_transinit or c_trans if a datum shift is not
           going to happen. */
        status = c_transinit( &geoproj->proj_code, &geoproj->units,
            &geoproj->zone_code, &geoproj->datum_code, geoproj->proj_coef,
            &outproj->proj_code, &outproj->units, &outproj->zone_code,
            &outproj->datum_code, outproj->proj_coef, prtprm, NULL );
        if ( status != MRT_NO_ERROR )
        {
            ErrorHandler( TRUE, "GetInputImageCorners", ERROR_GENERAL,
                "Error in c_transinit" );
        }
    }

    /* for each corner (lat/long), get a projection coordinate */
    for ( i = 0; i < 4; i++ )
    {
        if ( modis->output_datum_code != E_NODATUM )
        {
            /* Possible datum shift */
            status = c_trans( &geoproj->proj_code, &geoproj->units,
                &outproj->proj_code, &outproj->units,
                &modis->input_image_extent[i][1],
                &modis->input_image_extent[i][0], 
                &file->coord_corners[i][0],
                &file->coord_corners[i][1] );
            if ( status != E_GEO_SUCC )
            {
                ErrorHandler( TRUE, "GetInputImageCorners", ERROR_GENERAL,
                    "Error in c_trans" );
            }
        }
        else
        {
            /* Call GCTP directly to allow the semi-major and semi-minor
               to be specified directly.  Both the input and output
               sphere values need to be -1, and thus the projection
               parameters for both input and output will be used. */
            status = gctp_call( geoproj->proj_code, geoproj->zone_code, -1,
                geoproj->proj_coef, geoproj->units,
                modis->input_image_extent[i][1],
                modis->input_image_extent[i][0],
                outproj->proj_code, outproj->zone_code, outproj->sphere_code,
                outproj->proj_coef, outproj->units, &file->coord_corners[i][0],
                &file->coord_corners[i][1] );
            if ( status != E_GEO_SUCC )
            {
                ErrorHandler( TRUE, "GetInputImageCorners", ERROR_GENERAL,
                    "Error converting lat/long coordinates to "
                    "input projection coordinates." );
            }
        }
    }

    return( status );
}

/******************************************************************************

MODULE:  GetOutputImageCorners

PURPOSE:  Uses existing spatial subset corners to determine output projection
          coordinates.  If subset type is output projection coords, then
          we'll use the output projection coords specified by the user.  These
          coords need to be extended if they aren't an integral number of
          lines and samples.

          If subset type is lat/long and they are the original image corners,
          then the input spatial subset corners are used to walk the subset
          boundary in input space.  The boundary coords are projected to
          output projection space.  The minimum bounding rectangle in output
          projection space is returned as the output image corners.

          If subset type is lat/long and the subset is a true subset, then
          the input spatial corners are used (as-is) to project to output
          projection space.  The minimum bounding rectangle in output
          projection space is returned as the output image corners.

          If subset type is line/sample, then it is treated in the same
          manner as the full-image input lat/long subset.

          Bounding tiles (unless a spatial subset was specified by the user)
          are handled slightly differently to make sure we don't process
          data that falls in the discontinuity space.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status          See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         12/05  Gail Schmidt           Original development
4.0      09/07  Gail Schmidt           Modified OUTPUT_PROJ_COORDS to make
                                       sure the LR corner is an integral
                                       number of lines/samples from the UL

NOTES:

******************************************************************************/
int GetOutputImageCorners
    (
        ModisDescriptor *modis,     /* I: session info */
        FileDescriptor *outfile,    /* I/O: file for which we need corners */
        FileDescriptor *infile      /* I: input file */
    )
{
    ProjInfo *inproj, *outproj;   /* input/output projection data structures */
    ProjInfo projinfo;            /* local for geographic projection */
    int status = ERROR_GENERAL;   /* error code status */
    long prtprm[2];               /* geolib terminal printing flags */
    int i;                        /* indexers for loops */
    double incorners[4][2];       /* temp storage for the corners */
    double inx, iny, outx, outy;  /* coordinates for walking around the 
                                     edges of the rectangle */
    double minx, miny, maxx, maxy;/* min/max values for bounding rectangle */
    double xpixels, ypixels;      /* number of pixels in x/y direction */

    if ( modis->spatial_subset_type == OUTPUT_PROJ_COORDS )
    {   /* use the coordinates specified by the user, but make sure the LR
           is an integral number of lines and samples from the UL */
        outfile->coord_corners[UL][0] = modis->proj_extents_corners[UL][0];
        outfile->coord_corners[UL][1] = modis->proj_extents_corners[UL][1];
        outfile->coord_corners[LR][0] = modis->proj_extents_corners[LR][0];
        outfile->coord_corners[LR][1] = modis->proj_extents_corners[LR][1];

        /* These corners may need to be adjusted to be an exact number of
           pixels, based on the output pixel size. */
        xpixels = ( outfile->coord_corners[LR][0] -
            outfile->coord_corners[UL][0] ) / outfile->output_pixel_size;
        ypixels = ( modis->proj_extents_corners[UL][1] -
            modis->proj_extents_corners[LR][1] ) / outfile->output_pixel_size;

        /* Check x direction, adjust LRx if needed. */
        if( xpixels - (int) xpixels > 0.001 )
        {
            /* Not an exact number of pixels in the x direction. Reset the LRx
               pixel so that it is an integral number of pixels. */
            xpixels += 1.0;
            outfile->coord_corners[LR][0] = outfile->coord_corners[UL][0] +
                (int) ( xpixels ) * outfile->output_pixel_size;
        }

        /* Check y direction, adjust LRy if needed. */
        if( ypixels - (int) ypixels > 0.001 )
        {
            /* Not an exact number of pixels in the y direction. Reset the LRy
               value so that it is an integral number of pixels. */
            ypixels += 1.0;
            outfile->coord_corners[LR][1] = outfile->coord_corners[UL][1] -
                (int) ( ypixels ) * outfile->output_pixel_size;
        }

        /* Now set UR and LL based on UL and LR */
        outfile->coord_corners[UR][0] = outfile->coord_corners[LR][0];
        outfile->coord_corners[UR][1] = outfile->coord_corners[UL][1];
        outfile->coord_corners[LL][0] = outfile->coord_corners[UL][0];
        outfile->coord_corners[LL][1] = outfile->coord_corners[LR][1];
        status = MRT_NO_ERROR;
    }
    else if (modis->spatial_subset_type == INPUT_LAT_LONG && modis->is_subset)
    {   /* if subset type is lat/long and the subset is a true subset, then
           use the input lat/long spatial corners, convert to input projection
           projection space, walk the input projection boundary, and find
           the extents of the output projection */

        /* Print nothing */
        prtprm[GEO_TERM] = GEO_FALSE;
        prtprm[GEO_FILE] = GEO_FALSE;

        /* setup to use Geographic coordinates */
        projinfo.proj_code = GEO;
        projinfo.units = DEGREE;
        projinfo.zone_code = 0;
        for ( i = 0; i < NUM_PROJECTION_PARAMS; i++ )
            projinfo.proj_coef[i] = 0.0;

        /* keep it clean with a pointer */
        inproj = &projinfo;

        /* convert from input lat/long to output projection */
        outproj = modis->out_projection_info;
        inproj->datum_code = modis->input_datum_code;

        /* going from lat/long to output projection coords */
        if ( modis->output_datum_code != E_NODATUM )
        {   /* Don't call c_transinit or c_trans if a datum shift is not
               going to happen. */
            status = c_transinit( &inproj->proj_code, &inproj->units,
                &inproj->zone_code, &inproj->datum_code,
                inproj->proj_coef, &outproj->proj_code, &outproj->units,
                &outproj->zone_code, &outproj->datum_code,
                outproj->proj_coef, prtprm, NULL );
            if ( status != MRT_NO_ERROR )
            {
                ErrorHandler( TRUE, "GetOutputImageCorners", ERROR_GENERAL,
                    "Error in c_transinit for lat/long to output coords "
                    "for lat/long spatial subset" );
            }
        }

        /* set things up to find the output rectangle */
        minx = miny = MRT_FLOAT4_MAX;
        maxx = maxy = -MRT_FLOAT4_MAX;

        /* check the four corner points themselves */
        for ( i = 0; i < 4; i++ )
        {
            inx = modis->ll_spac_sub_gring_corners[i][1];  /* longitude */
            iny = modis->ll_spac_sub_gring_corners[i][0];  /* latitude */
            if ( modis->output_datum_code != E_NODATUM )
            {
                /* Possible datum shift */
                status = c_trans( &inproj->proj_code, &inproj->units,
                     &outproj->proj_code, &outproj->units,
                     &inx, &iny, &outx, &outy );
                if ( status != E_GEO_SUCC )
                {
                    ErrorHandler( TRUE, "GetOutputImageCorners", ERROR_GENERAL,
                        "Error in c_trans for input lat/long corner to output "
                        "projection corner points" );
                }
            }
            else
            {
                /* Call GCTP directly to allow the semi-major and semi-minor
                   to be specified directly.  Both the input and output
                   sphere values need to be -1, and thus the projection
                   parameters for both input and output will be used. */
                status = gctp_call( inproj->proj_code, inproj->zone_code,
                    -1, inproj->proj_coef, inproj->units, inx, iny,
                    outproj->proj_code, outproj->zone_code,
                    outproj->sphere_code, outproj->proj_coef, outproj->units,
                    &outx, &outy );
                if ( status != E_GEO_SUCC )
                {
                    ErrorHandler( TRUE, "GetOutputImageCorners", ERROR_GENERAL,
                        "Error converting input lat/long corners to output "
                        "projection corner points" );
                }
            }

            /* Are either of these coords the minimum bounding coords? */
            if( outy < miny ) miny = outy;
            if( outy > maxy ) maxy = outy;
            if( outx < minx ) minx = outx;
            if( outx > maxx ) maxx = outx;
        }

        /* These corners need to be adjusted to be an exact number of
           pixels, using the pixel size. */
        xpixels = ( maxx - minx ) / outfile->output_pixel_size;
        ypixels = ( maxy - miny ) / outfile->output_pixel_size;

        /* Check x direction, adjust maxx if needed. */
        if( xpixels - (int) xpixels > 0.001 )
        {
            /* Not an exact number of pixels in the x direction. Add a
               pixel and reset the maxx value so that it is an even
               number of pixels. */
            xpixels += 1.0;
            maxx = minx + (int) ( xpixels ) * outfile->output_pixel_size;
        }

        /* Check y direction, adjust miny if needed. */
        if( ypixels - (int) ypixels > 0.001 )
        {
            /* Not an exact number of pixels in the y direction. Add a
               pixel and reset the miny value so that it is an even
               number of pixels. */
            ypixels += 1.0;
            miny = maxy - (int) ( ypixels ) * outfile->output_pixel_size;
        }

        outfile->coord_corners[UL][0] = outfile->coord_corners[LL][0] = minx;
        outfile->coord_corners[UR][0] = outfile->coord_corners[LR][0] = maxx;
        outfile->coord_corners[UL][1] = outfile->coord_corners[UR][1] = maxy;
        outfile->coord_corners[LL][1] = outfile->coord_corners[LR][1] = miny;
        status = MRT_NO_ERROR;
    }
    else if (modis->use_bound && !modis->is_subset)
    {   /* handle bounding tiles differently (only if a user-specified subset
           wasn't already specified) */
        status = GetOutputImageCornersBound( modis, infile, outfile );
        if ( status != MRT_NO_ERROR )
        {
            ErrorHandler( TRUE, "GetOutputImageCorners", ERROR_GENERAL,
                "Error determining output image corners for bounding tiles" );
        }
    }
    else
    {   /* If subset type is lat/long and processing the original image 
           corners or subset type is line/sample, then the input spatial
           subset corners are used to walk the subset boundary in input
           space.  The boundary coords are projected to output projection
           space. */

        /* Get the max/min x/y values for the input minimum bounding
           rectangle */
        minx = miny = MRT_FLOAT4_MAX;
        maxx = maxy = -MRT_FLOAT4_MAX;
        for ( i = 0; i < 4; i++)
        {
            if( modis->proj_spac_sub_gring_corners[i][0] < minx )
                minx = modis->proj_spac_sub_gring_corners[i][0];
            if( modis->proj_spac_sub_gring_corners[i][0] > maxx )
                maxx = modis->proj_spac_sub_gring_corners[i][0];
            if( modis->proj_spac_sub_gring_corners[i][1] < miny )
                miny = modis->proj_spac_sub_gring_corners[i][1];
            if( modis->proj_spac_sub_gring_corners[i][1] > maxy )
                maxy = modis->proj_spac_sub_gring_corners[i][1];
        }

        /* Square off the rest of the corners in the input rectangle */
        incorners[LL][0] = incorners[UL][0] = minx;
        incorners[LR][0] = incorners[UR][0] = maxx;
        incorners[UL][1] = incorners[UR][1] = maxy;
        incorners[LL][1] = incorners[LR][1] = miny;

        /* Walk the boundaries of our input projection corners and get the
           min/max x/y values in output projection space */
        if ( WalkInputBoundary( modis, infile, incorners, &minx, &maxx,
            &miny, &maxy ) != MRT_NO_ERROR )
        {
            ErrorHandler( TRUE, "GetOutputImageCorners", ERROR_GENERAL,
                "Error in WalkInputBoundary trying to get the output "
                "projection corners" );
        }

        /* These corners need to be adjusted to be an exact number of
           pixels, using the pixel size. */
        xpixels = ( maxx - minx ) / outfile->output_pixel_size;
        ypixels = ( maxy - miny ) / outfile->output_pixel_size;

        /* Check x direction, adjust maxx if needed. */
        if( xpixels - (int) xpixels > 0.001 )
        {
            /* Not an exact number of pixels in the x direction. Add a
               pixel and reset the maxx value so that it is an even
               number of pixels. */
            xpixels += 1.0;
            maxx = minx + (int) ( xpixels ) * outfile->output_pixel_size;
        }

        /* Check y direction, adjust miny if needed. */
        if( ypixels - (int) ypixels > 0.001 )
        {
            /* Not an exact number of pixels in the y direction. Add a
               pixel and reset the miny value so that it is an even
               number of pixels. */
            ypixels += 1.0;
            miny = maxy - (int) ( ypixels ) * outfile->output_pixel_size;
        }

        outfile->coord_corners[UL][0] = outfile->coord_corners[LL][0] = minx;
        outfile->coord_corners[UR][0] = outfile->coord_corners[LR][0] = maxx;
        outfile->coord_corners[UL][1] = outfile->coord_corners[UR][1] = maxy;
        outfile->coord_corners[LL][1] = outfile->coord_corners[LR][1] = miny;
    }

    return (status);
}

/******************************************************************************

MODULE:  GetOutputImageCornersBound

PURPOSE:  Determines the output image corners for bounding tiles.  These
          are treated slightly different from the regular tiles, since
          the input projection coords are often not valid lat/longs.
          This routine walks the input projection coordinates, checks to
          see if they are in the discontinuity space, then grabs the output
          projection coordinates for the values that don't fall in the
          discontinuity space.

          Converts the spatial subset lat/long corners to output projection
          coordinates for the output image file descriptor.  Given that
          the bounding coordinates in the input file are just that --
          bounding coordinates, we'll use them as-is to determine the
          output corners.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status          See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  Rob Burrell            Original Development
         10/00  Rob Burrell	       Fix datum
         02/01  Rob Burrell            Fix bounding box
         04/01  Rob Burrell	       Fix for fix for bounding box
         11/01  Gail Schmidt           We will find the bounding box in
                                       output space rather than in input
                                       space. With the ISIN bounding problem,
                                       we had values in the input projection
                                       that wouldn't necessarily map to lat/long
                                       or output space.
         02/02  Gail Schmidt           If output projection coords have been
                                       specified, then use those coords instead
                                       of creating a minimum bounding box.
         12/05  Gail Schmidt           Since this routine is only for bounding
                                       tiles now, we'll no longer walk the
                                       edges in lat/long space.  The bounding
                                       coords should be sufficient to provide
                                       an output bounding box.
         12/05  Gail Schmidt           Need to recompute the lat/long once the
                                       projection corners have been calculated
                                       and adjusted to be an exact number of
                                       pixels
4.0      10/07  Gail Schmidt           Use all four input lat/long image
                                       corners for bounding tiles, since we
                                       may not be using squared off bounding
                                       coords from the metadata
                                       Also, if processing a bounding coord,
                                       don't extend the subset to get the
                                       integral number of lines and samples,
                                       since it may push the pixel back into
                                       the discontinuity space

NOTES:

******************************************************************************/
int GetOutputImageCornersBound
    (
        ModisDescriptor *modis,     /* I: session info */
        FileDescriptor *infile,     /* I: input file information */
        FileDescriptor *outfile     /* I/O: file for which we need corners */
    )
{
    int i;                          /* looping index */
    double minx, miny, maxx, maxy;  /* min/max values for bounding rectangle */
    double incorners[4][2];         /* corner points in input space */
    double xpixels, ypixels;        /* number of pixels in x/y direction */

    /* Put the input corners in an array for calling WalkInputBoundary */
    for ( i = 0; i < 4; i++ )
    {
        incorners[i][0] = modis->orig_coord_corners[i][0];
        incorners[i][1] = modis->orig_coord_corners[i][1];
    }

    /* Walk the boundaries of our input projection corners and get the
       min/max x/y values in output projection space */
    if ( WalkInputBoundary( modis, infile, incorners, &minx, &maxx,
        &miny, &maxy ) != MRT_NO_ERROR )
    {
        ErrorHandler( TRUE, "GetOutputImageCornersBound", ERROR_GENERAL,
            "Error in WalkInputBoundary trying to get the output projection "
            "corners" );
    }

    /* These corners need to be adjusted to be an exact number of
       pixels, using the pixel size. */
    xpixels = ( maxx - minx ) / outfile->output_pixel_size;
    ypixels = ( maxy - miny ) / outfile->output_pixel_size;

    /* Check x direction, adjust maxx if needed. */
    if ( xpixels - (int) xpixels > 0.001 )
    {
        /* Not an exact number of pixels in the x direction. Round back to
           the nearest pixel since we are already close to the bounding
           edge. */
        xpixels -= 1.0;
        maxx = minx + (int) ( xpixels ) * outfile->output_pixel_size;
    }

    /* Check y direction, adjust miny if needed. */
    if ( ypixels - (int) ypixels > 0.001 )
    {
        /* Not an exact number of pixels in the y direction. Round back to
           the nearest pixel since we are already close to the bounding
           edge. */
        ypixels -= 1.0;
        miny = maxy - (int) ( ypixels ) * outfile->output_pixel_size;
    }

    outfile->coord_corners[UL][0] = outfile->coord_corners[LL][0] = minx;
    outfile->coord_corners[UR][0] = outfile->coord_corners[LR][0] = maxx;
    outfile->coord_corners[UL][1] = outfile->coord_corners[UR][1] = maxy;
    outfile->coord_corners[LL][1] = outfile->coord_corners[LR][1] = miny;

    return ( MRT_NO_ERROR );
}

/******************************************************************************

MODULE:  GetOutputExtents

PURPOSE:  Using the corner points and pixel size, determine number
          of rows and columns for an output file and set the corner
          coordinates so that the image is rectangular.

RETURN VALUE:
Type = none
Value           Description
-----           -----------

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  Rob Burrell            Original Development
         12/00  Rob Burrell            Fix output extents for southern
                                       hemisphere abs()
         10/01  Gail Schmidt           nrows/ncols calculation was adding
                                       one to the actual number of rows/cols.
                                       An extra row/col is no longer added.
         11/01  Gail Schmidt           Handle the cases where the international
                                       dateline is crossed.
4.0      10/07  Gail Schmidt           International dateline check was not
                                       valid, so it was removed.

NOTES:

******************************************************************************/
void GetOutputExtents
    (
        FileDescriptor *file   /* I/O:  file to find extents */
    )
{
        /* min/max easting/northing values */
        double minx = MRT_FLOAT4_MAX, miny = MRT_FLOAT4_MAX,
               maxx = -MRT_FLOAT4_MAX, maxy = -MRT_FLOAT4_MAX;
        int i;

        /* run through the corners and find the min/max */
        for ( i = 0; i < 4; i++ )
        {
            if ( minx > file->coord_corners[i][0] )
                minx = file->coord_corners[i][0];
            if ( maxx < file->coord_corners[i][0] )
                maxx = file->coord_corners[i][0];

            if ( miny > file->coord_corners[i][1] )
                miny = file->coord_corners[i][1];
            if ( maxy < file->coord_corners[i][1] )
                maxy = file->coord_corners[i][1];
        }

        /* find ncols, abs to handle output west hemi */
        file->ncols = 
          (size_t)((((fabs) ( maxx - minx )) / file->output_pixel_size) + 0.5 );

        /* find nrows, abs to handle output south hemi */
        file->nrows = 
          (size_t)((((fabs) ( maxy - miny )) / file->output_pixel_size) + 0.5 );

        /* reallocate based on window */
        if ( file->rowbuffer )
            free( file->rowbuffer );

        /* new buffer, things may have changed */
        file->rowbuffer = ( void * ) calloc ( file->ncols, file->datasize );
        if ( !file->rowbuffer )
            ErrorHandler( TRUE, "GetOutputExtents", ERROR_MEMORY,
                "Error allocating memory for the output Row Buffer" );
}

/******************************************************************************

MODULE:  StoreOutputExtents

PURPOSE:  Calculate output extents in Geographic and store in the modis
          descriptor. Copy the output extents in projection units to the same.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status          See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         10/00  Rob Burrell            Original Development

NOTES:

******************************************************************************/
int StoreOutputExtents
    (
        ModisDescriptor *modis,    /* I:  session info */
        FileDescriptor *file       /* I/O:  file from which we store extents */
    )
{
    ProjInfo *inproj, *geoproj;     /* input projection data structure */
    ProjInfo projinfo;              /* local for geographic projection */
    int status;                     /* error code status */
    long prtprm[2];                 /* geolib terminal printing flags */
    int i;                         

    /* Print nothing */
    prtprm[GEO_TERM] = GEO_FALSE;
    prtprm[GEO_FILE] = GEO_FALSE;

    /* setup to get the Geographic coordinates for the extents */
    projinfo.proj_code = GEO;
    projinfo.units = DEGREE;
    projinfo.zone_code = 0;
    for ( i = 0; i < NUM_PROJECTION_PARAMS; i++ )
        projinfo.proj_coef[i] = 0.0;

    /* keep it clean with a pointer */
    geoproj = &projinfo;

    /* projection coords and lat/long coords are/will be in output space */
    inproj = GetOutputProjection (modis);
    geoproj->datum_code = modis->output_datum_code;

    if ( modis->output_datum_code != E_NODATUM )
    {   /* Don't call c_transinit or c_trans if a datum shift is not
           going to happen. */
        status = c_transinit( &inproj->proj_code, &inproj->units,
            &inproj->zone_code, &inproj->datum_code, inproj->proj_coef,
            &geoproj->proj_code, &geoproj->units, &geoproj->zone_code,
            &geoproj->datum_code, geoproj->proj_coef, prtprm, NULL );
        if ( status != MRT_NO_ERROR )
        {
            DestroyProjectionInfo( inproj );
            ErrorHandler( TRUE, "StoreOutputExtents", ERROR_GENERAL,
                "Error in c_transinit for output corners" );
        }
    }

    /* for each corner coordinate, determine the output lat/long */
    for ( i = 0; i < 4; i++ )
    {
        if ( modis->output_datum_code != E_NODATUM )
        {
            /* Possible datum shift */
            status = c_trans( &inproj->proj_code, &inproj->units,
                &geoproj->proj_code, &geoproj->units,
                &file->coord_corners[i][0], &file->coord_corners[i][1],
                &modis->ll_extents_corners[i][1],
                &modis->ll_extents_corners[i][0] );
            if ( status != E_GEO_SUCC && status != GCTP_IN_BREAK )
            {
                DestroyProjectionInfo( inproj );
                ErrorHandler( TRUE, "StoreOutputExtents", ERROR_GENERAL,
                    "Error in c_trans for output corner points" );
            }
        }
        else
        {
            /* Call GCTP directly to allow the semi-major and semi-minor
               to be specified directly.  Both the input and output
               sphere values need to be -1, and thus the projection
               parameters for both input and output will be used. */
            status = gctp_call( inproj->proj_code, inproj->zone_code,
                inproj->sphere_code, inproj->proj_coef, inproj->units,
                file->coord_corners[i][0], file->coord_corners[i][1],
                geoproj->proj_code, geoproj->zone_code, -1,
                geoproj->proj_coef, geoproj->units,
                &modis->ll_extents_corners[i][1],
                &modis->ll_extents_corners[i][0] );
            if ( status != E_GEO_SUCC && status != GCTP_IN_BREAK )
            {
                DestroyProjectionInfo( inproj );
                ErrorHandler( TRUE, "StoreOutputExtents", ERROR_GENERAL,
                    "Error converting projection coordinates to "
                    "output lat/long." );
            }
	}
    
        /* save the projection extents too */
        modis->proj_extents_corners[i][0] = file->coord_corners[i][0];
        modis->proj_extents_corners[i][1] = file->coord_corners[i][1];
    }

    DestroyProjectionInfo( inproj );

    return ( status );
}

/******************************************************************************

MODULE:  ConvertRowCol2Deg

PURPOSE:  Given row/col values for the input image, get the input lat/long.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status          See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  Rob Burrell            Original Development

NOTES:

******************************************************************************/
int ConvertRowCol2Deg
    (
        ModisDescriptor *modis     /* I:  session info */
    )
{
    ProjInfo *inproj, *outproj;   /* input/output projection data structures */
    ProjInfo projinfo;            /* local for geographic projection */
    int status = MRT_NO_ERROR;        /* error code status */
    long prtprm[2];               /* geolib terminal printing flags */
    int i;
    size_t nbi;
    double pixel_size = MRT_FLOAT4_MAX; /* find the highest rez band */
    double temp_corner_UL[2],
           temp_corners[4][2];    /* temp storage for corners */

    /* look through the selected bands and find the highest resolution */
    modis->is_subset = TRUE;
    for ( nbi = 0; nbi < modis->nbands; nbi++ )
        if ( modis->bandinfo[nbi].selected )
            if ( modis->bandinfo[nbi].pixel_size < pixel_size )
                pixel_size = modis->bandinfo[nbi].pixel_size;

    /* oops, possibly no selected bands? */
    if ( pixel_size == MRT_FLOAT4_MAX )
    {
        ErrorHandler( TRUE, "ConvertRowCol2Deg", ERROR_GENERAL,
                      "No valid pixel size" );
        return ( ERROR_GENERAL );
    }

    /* no printing */
    prtprm[GEO_TERM] = GEO_FALSE;
    prtprm[GEO_FILE] = GEO_FALSE;

    /* setup to use Geographic coordinates */
    projinfo.proj_code = GEO;
    projinfo.units = DEGREE;
    projinfo.zone_code = 0;
    for ( i = 0; i < NUM_PROJECTION_PARAMS; i++ )
        projinfo.proj_coef[i] = 0.0;

    /* use the geographic info for the output projection */
    outproj = &projinfo;
    inproj = GetInputProjection( modis );

    /* lat/long values should be in input space, so use the input datum code */
    outproj->datum_code = inproj->datum_code;

    /* First, use the extents of the input file to get a handle on things,
       going from Geographic to the input projection */
    if ( modis->output_datum_code != E_NODATUM )
    {   /* Don't call c_transinit or c_trans if a datum shift is not
           going to happen. */
        status = c_transinit( &outproj->proj_code, &outproj->units,
            &outproj->zone_code, &outproj->datum_code, outproj->proj_coef,
            &inproj->proj_code, &inproj->units, &inproj->zone_code,
            &inproj->datum_code, inproj->proj_coef, prtprm, NULL );
        if ( status != MRT_NO_ERROR )
        {
            DestroyProjectionInfo( inproj );
            ErrorHandler( FALSE, "ConvertRowCol2Deg", ERROR_GENERAL,
                "Error in c_transinit for corners" );
            return( status );
        }
    }

    /* get the input image's UL projection corner */
    if ( modis->output_datum_code != E_NODATUM )
    {
        /* Possible datum shift */
        status = c_trans( &outproj->proj_code, &outproj->units,
            &inproj->proj_code, &inproj->units,
            &modis->input_image_extent[UL][1],
            &modis->input_image_extent[UL][0],
            &temp_corner_UL[0], &temp_corner_UL[1] );
        if ( status != E_GEO_SUCC )
        {
            DestroyProjectionInfo( inproj );
            ErrorHandler( FALSE, "ConvertRowCol2Deg", ERROR_GENERAL,
                "Error in c_trans for UL projection corner" );
            return( status );
        }
    }
    else
    {
        /* Call GCTP directly to allow the semi-major and semi-minor
           to be specified directly.  Both the input and output
           sphere values need to be -1, and thus the projection
           parameters for both input and output will be used. */
        status = gctp_call( outproj->proj_code, outproj->zone_code,
            -1, outproj->proj_coef, outproj->units,
            modis->input_image_extent[UL][1],
            modis->input_image_extent[UL][0], inproj->proj_code,
            inproj->zone_code, inproj->sphere_code, inproj->proj_coef,
            inproj->units, &temp_corner_UL[0], &temp_corner_UL[1] );
        if ( status != E_GEO_SUCC )
        {
            DestroyProjectionInfo( inproj );
            ErrorHandler( FALSE, "ConvertRowCol2Deg", ERROR_GENERAL,
                "Error converting lat/long coordinates to "
                "input projection coordinate for UL corner." );
            return( status );
        }
    }

    /* take the actual corners and find the spatial subset corners
       in input projection coordinates. note that the read parameter routine
       stores the values as line[0] and sample[1] in the
       ll_spac_sub_gring_corners field. */
    for ( i = 0; i < 4; i++ )
    {
        temp_corners[i][0] = temp_corner_UL[0] +
            modis->ll_spac_sub_gring_corners[i][1] * pixel_size;
        temp_corners[i][1] = temp_corner_UL[1] -
            modis->ll_spac_sub_gring_corners[i][0] * pixel_size;

        /* save the projection coordinates for the corner points */
        modis->proj_spac_sub_gring_corners[i][0] = temp_corners[i][0];
        modis->proj_spac_sub_gring_corners[i][1] = temp_corners[i][1];
    }

    /* now we can go backwards to get the lat/lon for the subset */
    if ( modis->output_datum_code != E_NODATUM )
    {   /* Don't call c_transinit or c_trans if a datum shift is not
           going to happen. */
        status = c_transinit( &inproj->proj_code, &inproj->units,
            &inproj->zone_code, &inproj->datum_code, inproj->proj_coef,
            &outproj->proj_code, &outproj->units, &outproj->zone_code,
            &outproj->datum_code, outproj->proj_coef, prtprm, NULL );
        if ( status != MRT_NO_ERROR )
        {
            DestroyProjectionInfo( inproj );
            ErrorHandler( FALSE, "ConvertRowCol2Deg", ERROR_GENERAL,
                "Error in c_transinit for corners" );
            return( status );
        }
    }

    /* get the lat/long corners */
    for ( i = 0; i < 4; i++ )
    {
        if ( modis->output_datum_code != E_NODATUM )
        {
            /* Possible datum shift */
            status = c_trans( &inproj->proj_code, &inproj->units,
                &outproj->proj_code, &outproj->units,
                &temp_corners[i][0], &temp_corners[i][1],
                &modis->ll_spac_sub_gring_corners[i][1], 
                &modis->ll_spac_sub_gring_corners[i][0] );
            if ( status != E_GEO_SUCC )
            {
                DestroyProjectionInfo( inproj );
                ErrorHandler( FALSE, "ConvertRowCol2Deg", ERROR_GENERAL,
                          "Error in c_trans for lat/long corners" );
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
                inproj->sphere_code, inproj->proj_coef, inproj->units,
                temp_corners[i][0], temp_corners[i][1], outproj->proj_code,
                outproj->zone_code, -1, outproj->proj_coef, outproj->units,
                &modis->ll_spac_sub_gring_corners[i][1],
                &modis->ll_spac_sub_gring_corners[i][0] );
            if ( status != E_GEO_SUCC )
            {
                DestroyProjectionInfo( inproj );
                ErrorHandler( FALSE, "ConvertRowCol2Deg", ERROR_GENERAL,
                    "Error converting input projection coordinates to "
                    "lat/long coordinates." );
                return( status );
            }
	}
    }

    /* verify we aren't crossing the international dateline */
    CheckDateline( modis );

    DestroyProjectionInfo( inproj );
    return( status );
}

/******************************************************************************

MODULE:  GetInputRectangle

PURPOSE:  Take a spatial subset window specified in lat/long and return the
    input proj x/y values

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status          See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00   Rob Burrell           Original Development
         11/01   Gail Schmidt          Instead of taking UL/LR lat/long,
                                       converting to proj x/y, and creating
                                       the input spatial subset rectangle in
                                       proj x/y, we will now create the input
                                       spatial subset rectangle in lat/long
                                       coords.
         11/01   Gail Schmidt          Supports bounding ISIN tile coords
         12/05   Gail Schmidt          No need to bound the subset box in
                                       lat/long space. The UL and LR are the
                                       bounded coords.

NOTES:

******************************************************************************/
int GetInputRectangle
    (
        ModisDescriptor *modis    /* I:  session info */
    )
{
    ProjInfo *inproj, *geoproj;  /* input/output projection data structures */
    ProjInfo projinfo;           /* local for geographic projection */
    int status = MRT_NO_ERROR;       /* error code status */
    long prtprm[2];              /* geolib terminal printing flags */
    int i;                       /* looping variable */
    double tolerance;            /* tolerance value when comparing two
                                    coordinates to determine if they are the
                                    same value */

    /* no printing */
    prtprm[GEO_TERM] = GEO_FALSE;
    prtprm[GEO_FILE] = GEO_FALSE;

    /* setup to use Geographic coordinates */
    projinfo.proj_code = GEO;
    projinfo.units = DEGREE;
    projinfo.zone_code = 0;
    for ( i = 0; i < NUM_PROJECTION_PARAMS; i++ )
        projinfo.proj_coef[i] = 0.0;

    geoproj = &projinfo;
    inproj = GetInputProjection( modis );

    /* lat/long values are in input space so use the input datum */
    geoproj->datum_code = inproj->datum_code;

    /* Check the UL/LR subset corners to see if they are the same as the
       UL/LR for the granule.  If so, then process the whole granule (this
       isn't really a subset). */
    tolerance = 0.00000001;
    if ( (fabs (modis->input_image_extent[UL][0] -
         modis->ll_spac_sub_gring_corners[UL][0]) < tolerance) &&
         (fabs (modis->input_image_extent[UL][1] -
         modis->ll_spac_sub_gring_corners[UL][1]) < tolerance) &&
         (fabs (modis->input_image_extent[LR][0] -
         modis->ll_spac_sub_gring_corners[LR][0]) < tolerance) &&
         (fabs (modis->input_image_extent[LR][1] -
         modis->ll_spac_sub_gring_corners[LR][1]) < tolerance))
        modis->is_subset = FALSE;

    /* Check to see if we are processing the entire granule.  If so, square
       off the subset values in projection space.  If we are processing a
       format conversion, then also use the UL/LR lat/long subset to get the
       input projection coords, then square things off in input space.
       Otherwise square off the subset values in lat/long space. */
    if (( !modis->is_subset && !modis->use_bound ) ||
        modis->resampling_type == NO_RESAMPLE )
    { /* use the UL and LR input spatial subset corners (in projection
         coordinates) to determine the UR and LL spatial subset corners */
        /* find the proj coords for the UL and LR lat/long corners */
        if ( modis->output_datum_code != E_NODATUM )
        {   /* Don't call c_transinit or c_trans if a datum shift is not
               going to happen. */
            status = c_transinit( &geoproj->proj_code, &geoproj->units,
                &geoproj->zone_code, &geoproj->datum_code, geoproj->proj_coef,
                &inproj->proj_code, &inproj->units, &inproj->zone_code,
                &inproj->datum_code, inproj->proj_coef, prtprm, NULL );
            if ( status != MRT_NO_ERROR )
            {
                DestroyProjectionInfo( inproj );
                ErrorHandler( FALSE, "GetInputRectangle", ERROR_GENERAL,
                    "Error in c_transinit for corners" );
                return( status );
            }

            /* Get the UL corner proj coords */
            status = c_trans( &geoproj->proj_code, &geoproj->units,
                &inproj->proj_code, &inproj->units,
                &modis->ll_spac_sub_gring_corners[UL][1],
                &modis->ll_spac_sub_gring_corners[UL][0],
                &modis->proj_spac_sub_gring_corners[UL][0],
                &modis->proj_spac_sub_gring_corners[UL][1] );
            if ( status != E_GEO_SUCC )
            {
                DestroyProjectionInfo( inproj );
                ErrorHandler( FALSE, "GetInputRectangle", ERROR_GENERAL,
                   "Error in c_trans for UL lat/long corner" );
                return( status );
            }

            /* Get the LR corner proj coords */
            status = c_trans( &geoproj->proj_code, &geoproj->units,
                &inproj->proj_code, &inproj->units,
                &modis->ll_spac_sub_gring_corners[LR][1],
                &modis->ll_spac_sub_gring_corners[LR][0],
                &modis->proj_spac_sub_gring_corners[LR][0],
                &modis->proj_spac_sub_gring_corners[LR][1] );
            if ( status != E_GEO_SUCC )
            {
                DestroyProjectionInfo( inproj );
                ErrorHandler( FALSE, "GetInputRectangle", ERROR_GENERAL,
                   "Error in c_trans for LR lat/long corner" );
                return( status );
            }
        }
        else
        {
            /* Call GCTP directly to allow the semi-major and semi-minor
               to be specified directly.  Both the input and output
               sphere values need to be -1, and thus the projection
               parameters for both input and output will be used. */

            /* Get the UL corner proj coords */
            status = gctp_call( geoproj->proj_code, geoproj->zone_code, -1,
                geoproj->proj_coef, geoproj->units,
                modis->ll_spac_sub_gring_corners[UL][1],
                modis->ll_spac_sub_gring_corners[UL][0],
                inproj->proj_code, inproj->zone_code, inproj->sphere_code,
                inproj->proj_coef, inproj->units,
                &modis->proj_spac_sub_gring_corners[UL][0],
                &modis->proj_spac_sub_gring_corners[UL][1] );
            if ( status != E_GEO_SUCC )
            {
                DestroyProjectionInfo( inproj );
                ErrorHandler( FALSE, "GetInputRectangle", ERROR_GENERAL,
                    "Error converting UL lat/long coordinates to "
                    "output projection coordinates." );
                return( status );
            }

            /* Get the LR corner proj coords */
            status = gctp_call( geoproj->proj_code, geoproj->zone_code, -1,
                geoproj->proj_coef, geoproj->units,
                modis->ll_spac_sub_gring_corners[LR][1],
                modis->ll_spac_sub_gring_corners[LR][0],
                inproj->proj_code, inproj->zone_code, inproj->sphere_code,
                inproj->proj_coef, inproj->units,
                &modis->proj_spac_sub_gring_corners[LR][0],
                &modis->proj_spac_sub_gring_corners[LR][1] );
            if ( status != E_GEO_SUCC )
            {
                DestroyProjectionInfo( inproj );
                ErrorHandler( FALSE, "GetInputRectangle", ERROR_GENERAL,
                    "Error converting LR lat/long coordinates to "
                    "output projection coordinates." );
                return( status );
            }
        }

        /* set the UR and LL projection corner coords based on the UL and
           LR corners to create a rectangle */
        modis->proj_spac_sub_gring_corners[UR][0] =
            modis->proj_spac_sub_gring_corners[LR][0];
        modis->proj_spac_sub_gring_corners[UR][1] =
            modis->proj_spac_sub_gring_corners[UL][1];
        modis->proj_spac_sub_gring_corners[LL][0] =
            modis->proj_spac_sub_gring_corners[UL][0];
        modis->proj_spac_sub_gring_corners[LL][1] =
            modis->proj_spac_sub_gring_corners[LR][1];

        /* find the lat/long for the UR and LL corners */
        if ( modis->output_datum_code != E_NODATUM )
        {   /* Don't call c_transinit or c_trans if a datum shift is not
               going to happen. */
            status = c_transinit( &inproj->proj_code, &inproj->units,
                &inproj->zone_code, &inproj->datum_code, inproj->proj_coef,
                &geoproj->proj_code, &geoproj->units, &geoproj->zone_code,
                &geoproj->datum_code, geoproj->proj_coef, prtprm, NULL );
            if ( status != MRT_NO_ERROR )
            {
                DestroyProjectionInfo( inproj );
                ErrorHandler( FALSE, "GetInputRectangle", ERROR_GENERAL,
                    "Error in c_transinit (input proj to lat/long) for UR "
                    "and LL corners" );
                return( status );
            }

            /* Get the UR corner lat/long */
            status = c_trans( &inproj->proj_code, &inproj->units,
                &geoproj->proj_code, &geoproj->units,
                &modis->proj_spac_sub_gring_corners[UR][0],
                &modis->proj_spac_sub_gring_corners[UR][1],
                &modis->ll_spac_sub_gring_corners[UR][1],
                &modis->ll_spac_sub_gring_corners[UR][0] );
            if ( status != E_GEO_SUCC )
            {
                DestroyProjectionInfo( inproj );
                ErrorHandler( FALSE, "GetInputRectangle", ERROR_GENERAL,
                    "Error in c_trans for UR projection corner" );
                return( status );
            }

            /* Get the LL corner lat/long */
            status = c_trans( &inproj->proj_code, &inproj->units,
                &geoproj->proj_code, &geoproj->units,
                &modis->proj_spac_sub_gring_corners[LL][0],
                &modis->proj_spac_sub_gring_corners[LL][1],
                &modis->ll_spac_sub_gring_corners[LL][1],
                &modis->ll_spac_sub_gring_corners[LL][0] );
            if ( status != E_GEO_SUCC )
            {
                DestroyProjectionInfo( inproj );
                ErrorHandler( FALSE, "GetInputRectangle", ERROR_GENERAL,
                   "Error in c_trans for LL projection corner" );
                return( status );
            }
        }
        else
        {
            /* Call GCTP directly to allow the semi-major and semi-minor
               to be specified directly.  Both the input and output
               sphere values need to be -1, and thus the projection
               parameters for both input and output will be used. */

            /* Get the UR corner lat/long */
            status = gctp_call( inproj->proj_code, inproj->zone_code,
                inproj->sphere_code, inproj->proj_coef, inproj->units,
                modis->proj_spac_sub_gring_corners[UR][0],
                modis->proj_spac_sub_gring_corners[UR][1],
                geoproj->proj_code, geoproj->zone_code, -1,
                geoproj->proj_coef, geoproj->units,
                &modis->ll_spac_sub_gring_corners[UR][1],
                &modis->ll_spac_sub_gring_corners[UR][0] );
            if ( status != E_GEO_SUCC )
            {
                DestroyProjectionInfo( inproj );
                ErrorHandler( FALSE, "GetInputRectangle", ERROR_GENERAL,
                    "Error converting UR input projection coordinates to "
                    "lat/long coordinates." );
                return( status );
            }

            /* Get the LL corner lat/long */
            status = gctp_call( inproj->proj_code, inproj->zone_code,
                inproj->sphere_code, inproj->proj_coef, inproj->units,
                modis->proj_spac_sub_gring_corners[LL][0],
                modis->proj_spac_sub_gring_corners[LL][1],
                geoproj->proj_code, geoproj->zone_code, -1,
                geoproj->proj_coef, geoproj->units,
                &modis->ll_spac_sub_gring_corners[LL][1],
                &modis->ll_spac_sub_gring_corners[LL][0] );
            if ( status != E_GEO_SUCC )
            {
                DestroyProjectionInfo( inproj );
                ErrorHandler( FALSE, "GetInputRectangle", ERROR_GENERAL,
                    "Error converting LL input projection coordinates to "
                    "lat/long coordinates." );
                return( status );
            }
        }
    }
    else
    { /* this is a MODIS subset, either user-specified or bounding coords
         subset.  if it's user-specified, then square off the spatial
         subsetting using the user-specified UL and LR corner in lat/long
         space.  if bounding coords are being used, then use the corners
         as-is. */

        /* set the UR and LL projection corner coords based on the UL and
           LR corners to create a rectangle */
        if ( !modis->use_bound || modis->is_subset )
        {
            modis->ll_spac_sub_gring_corners[UR][0] =
                modis->ll_spac_sub_gring_corners[UL][0];
            modis->ll_spac_sub_gring_corners[UR][1] =
                modis->ll_spac_sub_gring_corners[LR][1];
            modis->ll_spac_sub_gring_corners[LL][0] =
                modis->ll_spac_sub_gring_corners[LR][0];
            modis->ll_spac_sub_gring_corners[LL][1] =
                modis->ll_spac_sub_gring_corners[UL][1];
        }
        else
        {
            /* bounding tile and no user subsetting so just use the bounding
               lat/long that was found when reading the input file */
            for ( i = 0; i < 4; i++ )
            {
                modis->ll_spac_sub_gring_corners[i][0] =
                    modis->input_image_extent[i][0];
                modis->ll_spac_sub_gring_corners[i][1] =
                    modis->input_image_extent[i][1];
            }
        }

        /* determine the input projection coordinates for the four lat/long
           corner points */
        if ( modis->output_datum_code != E_NODATUM )
        {   /* Don't call c_transinit or c_trans if a datum shift is not
               going to happen. */
            status = c_transinit( &geoproj->proj_code, &geoproj->units,
                &geoproj->zone_code, &geoproj->datum_code, geoproj->proj_coef,
                &inproj->proj_code, &inproj->units, &inproj->zone_code,
                &inproj->datum_code, inproj->proj_coef, prtprm, NULL );
            if ( status != MRT_NO_ERROR )
            {
                DestroyProjectionInfo( inproj );
                ErrorHandler( FALSE, "GetInputRectangle", ERROR_GENERAL,
                    "Error in c_transinit for corners" );
                return( status );
            }

            for (i = 0; i < 4; i++)
            {
                status = c_trans( &geoproj->proj_code, &geoproj->units,
                    &inproj->proj_code, &inproj->units,
                    &modis->ll_spac_sub_gring_corners[i][1],
                    &modis->ll_spac_sub_gring_corners[i][0],
                    &modis->proj_spac_sub_gring_corners[i][0],
                    &modis->proj_spac_sub_gring_corners[i][1] );
                if ( status != E_GEO_SUCC )
                {
                    DestroyProjectionInfo( inproj );
                    ErrorHandler( FALSE, "GetInputRectangle", ERROR_GENERAL,
                        "Error in c_trans converting lat/long coordinates to "
                        "output projection coordinates." );
                    return( status );
                }
            }
        }
        else
        {
            /* Call GCTP directly to allow the semi-major and semi-minor
               to be specified directly.  Both the input and output
               sphere values need to be -1, and thus the projection
               parameters for both input and output will be used. */
            for (i = 0; i < 4; i++)
            {
                status = gctp_call( geoproj->proj_code, geoproj->zone_code,
                    -1, geoproj->proj_coef, geoproj->units,
                    modis->ll_spac_sub_gring_corners[i][1],
                    modis->ll_spac_sub_gring_corners[i][0],
                    inproj->proj_code, inproj->zone_code, inproj->sphere_code,
                    inproj->proj_coef, inproj->units,
                    &modis->proj_spac_sub_gring_corners[i][0],
                    &modis->proj_spac_sub_gring_corners[i][1] );
                if ( status != E_GEO_SUCC )
                {
                    DestroyProjectionInfo( inproj );
                    ErrorHandler( FALSE, "GetInputRectangle", ERROR_GENERAL,
                        "Error converting lat/long coordinates to "
                        "output projection coordinates." );
                    return( status );
                }
            }
        }
    }

    /* verify we aren't crossing the international dateline */
    CheckDateline( modis );

    DestroyProjectionInfo( inproj );

    return( status );
}

/******************************************************************************

MODULE:  GetRectangle

PURPOSE:  Get the input rectangle using the specified output projection
    coordinates.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status          See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         02/02   Gail Schmidt          Original Development

NOTES:
  Create a rectangle in output projection space
  Convert the rectangle to lat/long in input projection space
  Convert the input lat/long to input projection coords

******************************************************************************/
int GetRectangle
    (
        ModisDescriptor *modis    /* I:  session info */
    )
{
    ProjInfo *inproj, *outproj;  /* input/output projection data structs */
    ProjInfo geoinfo, *geoproj;  /* geo projection data structs */
    int status = MRT_NO_ERROR;       /* error code status */
    long prtprm[2];              /* geolib terminal printing flags */
    int i;

    /* Using the UL and LR x/y projection coords, create a rectangle for
       the output image.  The x/y projection coords are read into the
       ll_spac_sub_gring_corners field initially, just for storage.  They
       are read as proj x and proj y. */
    modis->is_subset = TRUE;
    modis->proj_extents_corners[UL][0] =
        modis->ll_spac_sub_gring_corners[UL][0];
    modis->proj_extents_corners[UL][1] =
        modis->ll_spac_sub_gring_corners[UL][1];
    modis->proj_extents_corners[LR][0] =
        modis->ll_spac_sub_gring_corners[LR][0];
    modis->proj_extents_corners[LR][1] =
        modis->ll_spac_sub_gring_corners[LR][1];

    modis->proj_extents_corners[UR][0] = modis->proj_extents_corners[LR][0];
    modis->proj_extents_corners[UR][1] = modis->proj_extents_corners[UL][1];
    modis->proj_extents_corners[LL][0] = modis->proj_extents_corners[UL][0];
    modis->proj_extents_corners[LL][1] = modis->proj_extents_corners[LR][1];

    /* Convert the output projection coords to input lat/long */
    /* no printing */
    prtprm[GEO_TERM] = GEO_FALSE;
    prtprm[GEO_FILE] = GEO_FALSE;

    /* setup to use Geographic coordinates */
    geoinfo.proj_code = GEO;
    geoinfo.units = DEGREE;
    geoinfo.zone_code = 0;
    for ( i = 0; i < NUM_PROJECTION_PARAMS; i++ )
        geoinfo.proj_coef[i] = 0.0;
    geoproj = &geoinfo;

    /* get the input and output projection information */
    outproj = GetOutputProjection( modis );
    inproj = GetInputProjection( modis );

    /* we want the lat/long values in input space so use the input datum */
    geoproj->datum_code = inproj->datum_code;

    /* find the UL and LR lat/long corners using the output proj coords */
    if ( modis->output_datum_code != E_NODATUM )
    {   /* Don't call c_transinit or c_trans if a datum shift is not
           going to happen. */
        status = c_transinit( &outproj->proj_code, &outproj->units,
            &outproj->zone_code, &outproj->datum_code, outproj->proj_coef,
            &geoproj->proj_code, &geoproj->units, &geoproj->zone_code,
            &geoproj->datum_code, geoproj->proj_coef, prtprm, NULL );
        if ( status != MRT_NO_ERROR )
        {
            DestroyProjectionInfo( inproj );
            DestroyProjectionInfo( outproj );
            ErrorHandler( FALSE, "GetRectangle", ERROR_GENERAL,
                "Error in c_transinit for output projection corners" );
            return( status );
        }
    }

    /* Going from output projection coords to input lat/long */
    for ( i = 0; i < 4; i++ )
    {
        if ( modis->output_datum_code != E_NODATUM )
        {
            /* Possible datum shift */
            status = c_trans( &outproj->proj_code, &outproj->units,
                &geoproj->proj_code, &geoproj->units,
                &modis->proj_extents_corners[i][0],
                &modis->proj_extents_corners[i][1],
                &modis->ll_spac_sub_gring_corners[i][1],
                &modis->ll_spac_sub_gring_corners[i][0] );
            if ( status != E_GEO_SUCC )
            {
                DestroyProjectionInfo( inproj );
                DestroyProjectionInfo( outproj );
                ErrorHandler( FALSE, "GetRectangle", ERROR_GENERAL,
                   "Error in c_trans for output proj coord to lat/long "
                   "corner" );
                return( status );
            }
        }
        else
        {
            /* Call GCTP directly to allow the semi-major and semi-minor
               to be specified directly.  Both the input and output
               sphere values need to be -1, and thus the projection
               parameters for both input and output will be used. */
            status = gctp_call( outproj->proj_code, outproj->zone_code,
                outproj->sphere_code, outproj->proj_coef, outproj->units,
                modis->proj_extents_corners[i][0],
                modis->proj_extents_corners[i][1],
                geoproj->proj_code, geoproj->zone_code, -1,
                geoproj->proj_coef, geoproj->units,
                &modis->ll_spac_sub_gring_corners[i][1],
                &modis->ll_spac_sub_gring_corners[i][0] );
            if ( status != E_GEO_SUCC )
            {
                DestroyProjectionInfo( inproj );
                DestroyProjectionInfo( outproj );
                ErrorHandler( FALSE, "GetRectangle", ERROR_GENERAL,
                    "Error converting output projection coordinates to "
                    "lat/long coordinates." );
                return( status );
            }
	}
    }

    /* We're done with the output projection information */
    DestroyProjectionInfo( outproj );

    /* Convert from input lat/long to input projection coords */
    if ( modis->output_datum_code != E_NODATUM )
    {   /* Don't call c_transinit or c_trans if a datum shift is not
           going to happen. */
        status = c_transinit( &geoproj->proj_code, &geoproj->units,
            &geoproj->zone_code, &geoproj->datum_code, geoproj->proj_coef,
            &inproj->proj_code, &inproj->units, &inproj->zone_code,
            &inproj->datum_code, inproj->proj_coef, prtprm, NULL );
        if ( status != MRT_NO_ERROR )
        {
            DestroyProjectionInfo( inproj );
            ErrorHandler( FALSE, "GetRectangle", ERROR_GENERAL,
                "Error in c_transinit for lat/long corners" );
            return( status );
        }
    }

    /* Find the input proj coords from the input lat/long corners */
    for ( i = 0; i < 4; i++ )
    {
        if ( modis->output_datum_code != E_NODATUM )
        {
            /* Possible datum shift */
            status = c_trans( &geoproj->proj_code, &geoproj->units,
                &inproj->proj_code, &inproj->units,
                &modis->ll_spac_sub_gring_corners[i][1],
                &modis->ll_spac_sub_gring_corners[i][0],
                &modis->proj_spac_sub_gring_corners[i][0],
                &modis->proj_spac_sub_gring_corners[i][1] );
            if ( status != E_GEO_SUCC )
            {
                DestroyProjectionInfo( inproj );
                ErrorHandler( FALSE, "GetRectangle", ERROR_GENERAL,
                   "Error in c_trans for subset lat/long corner" );
                return( status );
            }
        }
        else
        {
            /* Call GCTP directly to allow the semi-major and semi-minor
               to be specified directly.  Both the input and output
               sphere values need to be -1, and thus the projection
               parameters for both input and output will be used. */
            status = gctp_call( geoproj->proj_code, geoproj->zone_code, -1,
                geoproj->proj_coef, geoproj->units,
                modis->ll_spac_sub_gring_corners[i][1],
                modis->ll_spac_sub_gring_corners[i][0],
                inproj->proj_code, inproj->zone_code, inproj->sphere_code,
                inproj->proj_coef, inproj->units,
                &modis->proj_spac_sub_gring_corners[i][0],
                &modis->proj_spac_sub_gring_corners[i][1] );
            if ( status != E_GEO_SUCC )
            {
                DestroyProjectionInfo( inproj );
                ErrorHandler( FALSE, "GetRectangle", ERROR_GENERAL,
                    "Error converting lat/long coordinates to "
                    "input projection coordinates." );
                return( status );
            }
	}
    }

    /* verify we aren't crossing the international dateline */
    CheckDateline( modis );

    /* We're done with the input projection information */
    DestroyProjectionInfo( inproj );

    return( status );
}

/******************************************************************************

MODULE:  DestroyProjectionInfo

PURPOSE:  Free memory for a projection structure 

RETURN VALUE:
Type = none
Value           Description
-----           -----------

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  Rob Burrell            Original Development

NOTES:

******************************************************************************/
void DestroyProjectionInfo
    (
        ProjInfo *projinfo      /* I:  structure to free */
    )
{
    free( projinfo );
}

/******************************************************************************

MODULE:  GetInputGeoCorner

PURPOSE:  Convert an input projection coordinate to lat/long in input space
          Validate the corner then produces the same input projection
          coordinates using the lat/long from the inverse mapping.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
TRUE            Success
FALSE           Failure

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  Rob Burrell            Original Development
         11/01  Gail Schmidt           Handle GCTP_ERANGE returned from c_trans
         12/02  Gail Schmidt           Only ISIN returns an error if the
                                       projection coord does not map to a valid
                                       lat/long. All other projections simply
                                       wrap around the meridian to provide an
                                       invalid value. This case must be handled
                                       for bounding tiles.
         02/03  Gail Schmidt           Added support for GCTP_IN_BREAK returned
                                       from Interrupted Goode's
         10/07  Gail Schmidt           Since we are working in input space,
                                       there's no need to support datum
                                       transformations.

NOTES:

******************************************************************************/
int GetInputGeoCorner
    ( 
        ModisDescriptor *modis,     /* I:  session info */
        double inputx,              /* I:  easting */
        double inputy,              /* I:  northing */
        double *outputlat,          /* O:  latitude */
        double *outputlon           /* O:  longitude */
    )
{
    ProjInfo *inproj, outproj;     /* input/output projection data structures */
    long prtprm[2];                /* geolib terminal printing flags */
    int status;                    /* error status code */
    int i;
    int erange_set = FALSE;        /* flag to specify if GCTP_ERANGE was
                                      returned from c_trans */
    double inputx2, inputy2;       /* second input x/y values for comparison
                                      to determine wrap-around */
    double tolerance;              /* tolerance value between the input
                                      projection coords and the reprojected
                                      input projection coords */

    /* if the input projection is Geographic, then the input corners
       are already in lat/long */
    if ( modis->input_projection_type == PROJ_GEO )
    {
        *outputlat = inputy;
        *outputlon = inputx;
        return TRUE;
    }

    /* no printing of projection info */
    prtprm[GEO_TERM] = GEO_FALSE;
    prtprm[GEO_FILE] = GEO_FALSE;

    /* setup to use Geographic coordinates; projection parameters for
       Geographic are all zeros; units are in degrees */
    outproj.proj_code = GEO;
    outproj.units = DEGREE;
    outproj.zone_code = 0;
    for ( i = 0; i < NUM_PROJECTION_PARAMS; i++ )
        outproj.proj_coef[i] = 0.0;

    /* create an input projection structure */
    inproj = GetInputProjection( modis );

    /* since we want the lat/long for input space, use the input datum for
       for the output datum as well */
    outproj.datum_code = inproj->datum_code;

    /* Call GCTP directly to allow the semi-major and semi-minor
       to be specified directly.  Both the input and output
       sphere values need to be -1, and thus the projection
       parameters for both input and output will be used. */
    status = gctp_call( inproj->proj_code, inproj->zone_code,
        inproj->sphere_code, inproj->proj_coef, inproj->units,
        inputx, inputy, outproj.proj_code, outproj.zone_code, -1,
        outproj.proj_coef, outproj.units, outputlon, outputlat );
    if ( status == GCTP_ERANGE || status == GCTP_IN_BREAK )
    {   /* The value was out of range for the projection so
           just set it as a background pixel. */
        erange_set = TRUE;
    }
    else if ( status != E_GEO_SUCC )
    {
        DestroyProjectionInfo( inproj );
        ErrorHandler( TRUE, "GetInputGeoCorner", ERROR_GENERAL,
            "Error converting input projection coordinates to "
            "lat/long coordinates." );
    }

    /* Check if the projection values fell into a discontinuity space and
       possibly wrapped around to the other side of the Earth.  If so, then
       this needs to be flagged and erange_set should be set.  The best way
       to catch this is to map the output lat/long back to input space.  If
       the x/y value is not the same as the input x/y, then wrap-around
       occurred. */
    /* Convert the lat/long back to input projection coords */
    status = gctp_call( outproj.proj_code, outproj.zone_code, -1,
        outproj.proj_coef, outproj.units, *outputlon, *outputlat,
        inproj->proj_code, inproj->zone_code, inproj->sphere_code,
        inproj->proj_coef, inproj->units, &inputx2, &inputy2 );
    if ( status == GCTP_ERANGE || status == GCTP_IN_BREAK )
    {   /* The value was out of range for the projection so
           just set it as a background pixel. */
        erange_set = TRUE;
    }
    else if ( status != E_GEO_SUCC )
    {
        DestroyProjectionInfo( inproj );
        ErrorHandler( TRUE, "GetInputGeoCorner", ERROR_GENERAL,
            "Error converting lat/long coordinates to input projection "
            "coordinates." );
    }

    /* Determine if the input projection coords and the reprojected
       input projection coords are within 5m of each other.  If they
       are not then wrap-around occurred. */
    tolerance = 5.0;
    if ( fabs ( inputx - inputx2 ) > tolerance )
    {
        erange_set = TRUE;
    }
    if ( fabs ( inputy - inputy2 ) > tolerance )
    {
        erange_set = TRUE;
    }

    DestroyProjectionInfo( inproj );

    if ( erange_set )
        return ( GCTP_ERANGE );
    else
        return ( TRUE );
}

/******************************************************************************

MODULE:  GetOutputProjCorner

PURPOSE:  Convert an input lat/long coordinate to projection coordinates
          in output space.  Validate the corner then produces the same
          input lat/long using the projection coordinates from the forward
          mapping.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
TRUE            Success
FALSE           Failure

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         10/07  Gail Schmidt           Original Development

NOTES:

******************************************************************************/
int GetOutputProjCorner
    ( 
        ModisDescriptor *modis,     /* I:  session info */
        double inputlat,            /* I:  latitude */
        double inputlon,            /* I:  longitude */
        double *outputx,            /* O:  easting */
        double *outputy             /* O:  northing */
    )
{
    ProjInfo inproj, *outproj;     /* input/output projection data structures */
    long prtprm[2];                /* geolib terminal printing flags */
    int status;                    /* error status code */
    int i;
    int erange_set = FALSE;        /* flag to specify if GCTP_ERANGE was
                                      returned from c_trans */
    double inputlat2, inputlon2;   /* second input lat/long values for
                                      comparison to determine wrap-around */
    double tolerance;              /* tolerance value between the input
                                      lat/long and the reprojected input
                                      lat/long coordinates */

    /* if the output projection is Geographic, then the input corners
       are already in lat/long */
    if ( modis->output_projection_type == PROJ_GEO )
    {
        *outputx = inputlon;
        *outputy = inputlat;
        return TRUE;
    }

    /* no printing of projection info */
    prtprm[GEO_TERM] = GEO_FALSE;
    prtprm[GEO_FILE] = GEO_FALSE;

    /* setup to use Geographic coordinates; projection parameters for
       Geographic are all zeros; units are in degrees */
    inproj.proj_code = GEO;
    inproj.units = DEGREE;
    inproj.zone_code = 0;
    for ( i = 0; i < NUM_PROJECTION_PARAMS; i++ )
        inproj.proj_coef[i] = 0.0;

    /* create an output projection structure */
    outproj = GetOutputProjection( modis );

    /* since we are working with the lat/long from input space, use the
       input datum for */
    inproj.datum_code = modis->input_datum_code; 

    /* initialize and convert */
    if ( modis->output_datum_code != E_NODATUM )
    {   /* Don't call c_transinit or c_trans if a datum shift is not
           going to happen. */
        status = c_transinit( &inproj.proj_code, &inproj.units,
            &inproj.zone_code, &inproj.datum_code, inproj.proj_coef,
            &outproj->proj_code, &outproj->units, &outproj->zone_code,
            &outproj->datum_code, outproj->proj_coef, prtprm, NULL );
        if ( status != MRT_NO_ERROR )
        {
            DestroyProjectionInfo( outproj );
            ErrorHandler( TRUE, "GetOutputProjCorner", ERROR_GENERAL,
                "Error in c_transinit" );
        }
 
        /* Convert from lat/long (geographic) in input space to output
           projection coords */
        status = c_trans( &inproj.proj_code, &inproj.units,
            &outproj->proj_code, &outproj->units,
            &inputlon, &inputlat, outputx, outputy );
        if ( status == GCTP_ERANGE || status == GCTP_IN_BREAK )
        {
            erange_set = TRUE;
        }
        else if ( status != E_GEO_SUCC )
        {
            DestroyProjectionInfo( outproj );
            ErrorHandler( TRUE, "GetOutputProjCorner", ERROR_GENERAL,
                      "Error in c_trans" );
        }
    }
    else
    {
        /* Call GCTP directly to allow the semi-major and semi-minor
           to be specified directly.  Both the input and output
           sphere values need to be -1, and thus the projection
           parameters for both input and output will be used. */
        status = gctp_call( inproj.proj_code, inproj.zone_code,
            -1, inproj.proj_coef, inproj.units,
            inputlon, inputlat, outproj->proj_code, outproj->zone_code,
            outproj->sphere_code, outproj->proj_coef, outproj->units,
            outputx, outputy );
        if ( status == GCTP_ERANGE || status == GCTP_IN_BREAK )
        {   /* The value was out of range for the projection so
               just set it as a background pixel. */
            erange_set = TRUE;
        }
        else if ( status != E_GEO_SUCC )
        {
            DestroyProjectionInfo( outproj );
            ErrorHandler( TRUE, "GetOutputProjCorner", ERROR_GENERAL,
                "Error converting lat/long coordinates to "
                "output projection coordinates." );
        }
    }

    /* Make sure our output projection coords map back to the same lat/long
       with a forward mapping.  This will tell us if the coordinates fall
       in the discontinuity area of the output projection or not.  If they
       do, then this needs to be flagged and erange_set should be set. */

    /* Convert the output projection coords back to input lat/long */
    if ( modis->output_datum_code != E_NODATUM )
    {   /* Don't call c_transinit or c_trans if a datum shift is not
           going to happen. */
        status = c_transinit( &outproj->proj_code, &outproj->units,
            &outproj->zone_code, &outproj->datum_code, outproj->proj_coef,
            &inproj.proj_code, &inproj.units, &inproj.zone_code,
            &inproj.datum_code, inproj.proj_coef, prtprm, NULL );
        if ( status != MRT_NO_ERROR )
        {
            DestroyProjectionInfo( outproj );
            ErrorHandler( TRUE, "GetOutputProjCorner", ERROR_GENERAL,
                "Error in c_transinit" );
        }

        /* Convert from output projection coords in input lat/long */
        status = c_trans( &outproj->proj_code, &outproj->units,
            &inproj.proj_code, &inproj.units, outputx, outputy,
            &inputlon2, &inputlat2 );
        if ( status == GCTP_ERANGE || status == GCTP_IN_BREAK )
        {
            erange_set = TRUE;
        }
        else if ( status != E_GEO_SUCC )
        {
            DestroyProjectionInfo( outproj );
            ErrorHandler( TRUE, "GetOutputProjCorner", ERROR_GENERAL,
                  "Error in c_trans" );
        }
    }
    else
    {
        /* Call GCTP directly to allow the semi-major and semi-minor
           to be specified directly.  Both the input and output
           sphere values need to be -1, and thus the projection
           parameters for both input and output will be used. */
        status = gctp_call( outproj->proj_code, outproj->zone_code,
            outproj->sphere_code, outproj->proj_coef, outproj->units,
            *outputx, *outputy, inproj.proj_code, inproj.zone_code, -1,
            inproj.proj_coef, inproj.units, &inputlon2, &inputlat2 );
        if ( status == GCTP_ERANGE || status == GCTP_IN_BREAK )
        {   /* The value was out of range for the projection so
               just set it as a background pixel. */
            erange_set = TRUE;
        }
        else if ( status != E_GEO_SUCC )
        {
            DestroyProjectionInfo( outproj );
            ErrorHandler( TRUE, "GetOutputProjCorner", ERROR_GENERAL,
                "Error converting output projection coordinates to "
                "input lat/long coordinates." );
        }
    }

    /* Determine if the input lat/long and the reprojected input lat/long are
       within .1 degree of each other.  If they are not then flag as falling
       into the discontinuity space. */
    tolerance = 0.1;
    if ( fabs ( inputlat - inputlat2 ) > tolerance )
    {
        erange_set = TRUE;
    }
    if ( fabs ( inputlon - inputlon2 ) > tolerance )
    {
        erange_set = TRUE;
    }

    DestroyProjectionInfo( outproj );

    if ( erange_set )
        return ( GCTP_ERANGE );
    else
        return ( TRUE );
}

/******************************************************************************

MODULE:  GetInputGeoCornerMosaic

PURPOSE:  Convert an input projection coordinate to lat/long in input space
          (using the input datum).  Validate the corner then produces the
          same input projection coordinates using the lat/long from the
          inverse mapping.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
TRUE            Success
FALSE           Failure

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  Rob Burrell            Original Development
         10/07  Gail Schmidt           Stole from GetInputGeoCorner for
                                       mosaic structures

NOTES:

******************************************************************************/
int GetInputGeoCornerMosaic
    ( 
        MosaicDescriptor *mosaic,   /* I:  session info */
        double inputx,              /* I:  easting */
        double inputy,              /* I:  northing */
        double *outputlat,          /* O:  latitude */
        double *outputlon           /* O:  longitude */
    )
{
    ProjInfo *inproj, outproj;     /* input/output projection data structures */
    long prtprm[2];                /* geolib terminal printing flags */
    int status;                    /* error status code */
    int i;
    int erange_set = FALSE;        /* flag to specify if GCTP_ERANGE was
                                      returned from c_trans */
    double inputx2, inputy2;       /* second input x/y values for comparison
                                      to determine wrap-around */
    double tolerance;              /* tolerance value between the input
                                      projection coords and the reprojected
                                      input projection coords */

    /* no printing of projection info */
    prtprm[GEO_TERM] = GEO_FALSE;
    prtprm[GEO_FILE] = GEO_FALSE;

    /* setup to use Geographic coordinates; projection parameters for
       Geographic are all zeros; units are in degrees */
    outproj.proj_code = GEO;
    outproj.units = DEGREE;
    outproj.zone_code = 0;
    for ( i = 0; i < NUM_PROJECTION_PARAMS; i++ )
        outproj.proj_coef[i] = 0.0;

    /* create an input projection structure */
    inproj = GetInputProjectionMosaic( mosaic );

    /* since we want the lat/long for input space, use the input datum for
       for the output datum as well */
    outproj.datum_code = inproj->datum_code;

    /* Call GCTP directly to allow the semi-major and semi-minor
       to be specified directly.  Both the input and output
       sphere values need to be -1, and thus the projection
       parameters for both input and output will be used. */
    status = gctp_call( inproj->proj_code, inproj->zone_code,
        inproj->sphere_code, inproj->proj_coef, inproj->units,
        inputx, inputy, outproj.proj_code, outproj.zone_code, -1,
        outproj.proj_coef, outproj.units, outputlon, outputlat );
    if ( status == GCTP_ERANGE || status == GCTP_IN_BREAK )
    {   /* The value was out of range for the projection so
           just set it as a background pixel. */
        erange_set = TRUE;
    }
    else if ( status != E_GEO_SUCC )
    {
        DestroyProjectionInfo( inproj );
        ErrorHandler( TRUE, "GetInputGeoCornerMosaic", ERROR_GENERAL,
            "Error converting lat/long coordinates to input "
            "projection coordinates." );
    }

    /* Check if the projection values fell into a discontinuity space and
       possibly wrapped around to the other side of the Earth.  If so, then
       this needs to be flagged and erange_set should be set.  The best way
       to catch this is to map the output lat/long back to input space.  If
       the x/y value is not the same as the input x/y, then wrap-around
       occurred. */
    /* Convert the lat/long back to input projection coords */
    /* Call GCTP directly to allow the semi-major and semi-minor
       to be specified directly.  Both the input and output
       sphere values need to be -1, and thus the projection
       parameters for both input and output will be used. */
    status = gctp_call( outproj.proj_code, outproj.zone_code, -1,
        outproj.proj_coef, outproj.units, *outputlon, *outputlat,
        inproj->proj_code, inproj->zone_code, inproj->sphere_code,
        inproj->proj_coef, inproj->units, &inputx2, &inputy2 );
    if ( status == GCTP_ERANGE || status == GCTP_IN_BREAK )
    {   /* The value was out of range for the projection so
           just set it as a background pixel. */
        erange_set = TRUE;
    }
    else if ( status != E_GEO_SUCC )
    {
        DestroyProjectionInfo( inproj );
        ErrorHandler( TRUE, "GetInputGeoCornerMosaic", ERROR_GENERAL,
            "Error converting input projection coordinates to "
            "lat/long coordinates." );
    }

    /* Determine if the input projection coords and the reprojected
       input projection coords are within 5m of each other.  If they
       are not then wrap-around occurred. */
    tolerance = 5.0;
    if ( fabs ( inputx - inputx2 ) > tolerance )
    {
        erange_set = TRUE;
    }
    if ( fabs ( inputy - inputy2 ) > tolerance )
    {
        erange_set = TRUE;
    }

    DestroyProjectionInfo( inproj );

    if ( erange_set )
        return ( GCTP_ERANGE );
    else
        return ( TRUE );
}

/******************************************************************************

MODULE:  GetInputProjCorner

PURPOSE:  Convert a corner from lat/long in input space to input projection
          coordinates.

RETURN VALUE:
Type = void

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         11/01  Gail Schmidt           Original Development

NOTES:

******************************************************************************/
void GetInputProjCorner
    (
        ModisDescriptor *modis,     /* I:  session info */
        double inputlat,            /* I:  latitude */
        double inputlon,            /* I:  longitude */
        double *outputx,            /* O:  easting */
        double *outputy             /* O:  northing */
    )

{
    ProjInfo *inproj, *geoproj;  /* input/output projection data structures */
    ProjInfo projinfo;           /* local for geographic projection */
    int status = MRT_NO_ERROR;       /* error code status */
    long prtprm[2];              /* geolib terminal printing flags */
    int i;

    /* no printing */
    prtprm[GEO_TERM] = GEO_FALSE;
    prtprm[GEO_FILE] = GEO_FALSE;

    /* setup to use Geographic coordinates; projection parameters for
       Geographic are all zeros; units are in degrees */
    projinfo.proj_code = GEO;
    projinfo.units = DEGREE;
    projinfo.zone_code = 0;
    for ( i = 0; i < NUM_PROJECTION_PARAMS; i++ )
        projinfo.proj_coef[i] = 0.0;

    geoproj = &projinfo;
    inproj = GetInputProjection( modis );

    /* since these are input lat/longs, then use the input datum code */
    geoproj->datum_code = inproj->datum_code;

    /* Call GCTP directly to allow the semi-major and semi-minor
       to be specified directly.  Both the input and output
       sphere values need to be -1, and thus the projection
       parameters for both input and output will be used. */
    status = gctp_call( geoproj->proj_code, geoproj->zone_code,
        -1, geoproj->proj_coef, geoproj->units, inputlon, inputlat,
        inproj->proj_code, inproj->zone_code, inproj->sphere_code,
        inproj->proj_coef, inproj->units, outputx, outputy );
    if ( status != E_GEO_SUCC )
    {
        DestroyProjectionInfo( inproj );
        ErrorHandler( TRUE, "GetInputProjCorner", ERROR_GENERAL,
            "Error converting lat/long coordinates to "
            "input projection coordinates." );
    }

    DestroyProjectionInfo( inproj );
}


/******************************************************************************

MODULE:  AdjustGeoPixelSizeProj

PURPOSE:  Convert input meter pixel size dimension of a pixel near the
          center of the input image to degrees for Geographic output.

RETURN VALUE:
Type = none

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         10/00  Rob Burrell            Original Development

NOTES:

******************************************************************************/
int AdjustGeoPixelSizeProj
    (
        ModisDescriptor *modis     /* I/O:  session info */
    )
{
    ProjInfo *inproj, *geoproj;  /* input/output projection data structures */
    ProjInfo projinfo;           /* local for geographic projection */
    int status = MRT_NO_ERROR;       /* error code status */
    long prtprm[2];              /* geolib terminal printing flags */
    int i;
    size_t nbi;
    double temp_corners[4][2];   /* temp storage for corners */
    double centerx, centery, centerxplus;    /* center pixel in projection
                                                coordinates */
    /* no printing */
    prtprm[GEO_TERM] = GEO_FALSE;
    prtprm[GEO_FILE] = GEO_FALSE;

    /* setup to use Geographic coordinates; projection parameters for
       Geographic are all zeros; units are in degrees */
    projinfo.proj_code = GEO;
    projinfo.units = DEGREE;
    projinfo.zone_code = 0;
    for ( i = 0; i < NUM_PROJECTION_PARAMS; i++ )
        projinfo.proj_coef[i] = 0.0;

    geoproj = &projinfo;
    inproj = GetInputProjection( modis );

    /* since we want input lat/longs, then use the input datum code */
    geoproj->datum_code = inproj->datum_code;

    /* First, use the spatial subset to get a handle on things,
       going from input lat/long to the input projection */
    if ( modis->output_datum_code != E_NODATUM )
    {   /* Don't call c_transinit or c_trans if a datum shift is not
           going to happen. */
        status = c_transinit( &geoproj->proj_code, &geoproj->units,
            &geoproj->zone_code, &geoproj->datum_code, geoproj->proj_coef,
            &inproj->proj_code, &inproj->units, &inproj->zone_code,
            &inproj->datum_code, inproj->proj_coef, prtprm, NULL );
        if ( status != MRT_NO_ERROR )
        {
            DestroyProjectionInfo( inproj );
            ErrorHandler( TRUE, "AdjustGeoPixelSizeProj", ERROR_GENERAL,
                "Error in c_transinit for spatial subset" );
        }
    }

    /* get the input projection coordinates for the subset corners */
    for ( i = 0; i < 4; i++ )
    {
        if ( modis->output_datum_code != E_NODATUM )
        {
            /* Possible datum shift */
            status = c_trans( &geoproj->proj_code, &geoproj->units,
                &inproj->proj_code, &inproj->units,
                &modis->ll_spac_sub_gring_corners[i][1], 
                &modis->ll_spac_sub_gring_corners[i][0],
                &temp_corners[i][0], &temp_corners[i][1] );
            if ( status != E_GEO_SUCC )
            {
                DestroyProjectionInfo( inproj );
                ErrorHandler( TRUE, "AdjustGeoPixelSizeProj", ERROR_GENERAL,
                   "Error in c_trans for proj coords of the subset corners" );
            }
        }
        else
        {
            /* Call GCTP directly to allow the semi-major and semi-minor
               to be specified directly.  Both the input and output
               sphere values need to be -1, and thus the projection
               parameters for both input and output will be used. */
            status = gctp_call( geoproj->proj_code, geoproj->zone_code, -1,
                geoproj->proj_coef, geoproj->units,
                modis->ll_spac_sub_gring_corners[i][1],
                modis->ll_spac_sub_gring_corners[i][0],
                inproj->proj_code, inproj->zone_code, inproj->sphere_code,
                inproj->proj_coef, inproj->units, &temp_corners[i][0],
                &temp_corners[i][1] );
            if ( status != E_GEO_SUCC )
            {
                DestroyProjectionInfo( inproj );
                ErrorHandler( TRUE, "AdjustGeoPixelSizeProj", ERROR_GENERAL,
                    "Error converting lat/long coordinates to "
                    "input projection coordinates." );
            }
	}
    }

    /* find the center pixel for the scene in input projection coordinates */
    centerx = temp_corners[UL][0] +
              (temp_corners[UR][0] - temp_corners[UL][0])/2.0;
    centery = temp_corners[LL][1] +
              (temp_corners[UL][1] - temp_corners[LL][1])/2.0;

    /* initialize transform to go from input projection coords to lat/long */
    if ( modis->output_datum_code != E_NODATUM )
    {   /* Don't call c_transinit or c_trans if a datum shift is not
           going to happen. */
        status = c_transinit( &inproj->proj_code, &inproj->units,
            &inproj->zone_code, &inproj->datum_code, inproj->proj_coef,
            &geoproj->proj_code, &geoproj->units, &geoproj->zone_code,
            &geoproj->datum_code, geoproj->proj_coef, prtprm, NULL );
        if ( status != MRT_NO_ERROR )
        {
            DestroyProjectionInfo( inproj );
            ErrorHandler( TRUE, "AdjustGeoPixelSizeProj", ERROR_GENERAL,
                "Error in c_transinit for center pixel" );
        }
    }

    /* find the center point in input lat/long for each band */
    for ( nbi = 0; nbi < modis->nbands; nbi++ )
    {
        /* this isn't entirely correct, at least from a projection
         * standpoint, but considering the resolution of the instrument
         * it is good enough */
        centerxplus = centerx + modis->bandinfo[nbi].pixel_size;

        if ( modis->output_datum_code != E_NODATUM )
        {
            /* Possible datum shift */
            /* get the input lat/long for the center */        
            c_trans( &inproj->proj_code, &inproj->units,
                 &geoproj->proj_code, &geoproj->units,
                 &centerx, &centery, &temp_corners[0][0],
                 &temp_corners[0][1] );
            if ( status != E_GEO_SUCC )
            {
                DestroyProjectionInfo( inproj );
                ErrorHandler( TRUE, "AdjustGeoPixelSizeProj", ERROR_GENERAL,
                          "Error in c_trans for lat/long of the center pixel" );
            }

            /* get the lat/long for the center plus the pixelsize */
            c_trans( &inproj->proj_code, &inproj->units,
                 &geoproj->proj_code, &geoproj->units,
                 &centerxplus, &centery, &temp_corners[1][0],
                 &temp_corners[1][1] );
            if ( status != E_GEO_SUCC )
            {
                DestroyProjectionInfo( inproj );
                ErrorHandler( TRUE, "AdjustGeoPixelSizeProj", ERROR_GENERAL,
                    "Error in c_trans for lat/long of center pixel plus the "
                    "pixel size" );
            }
        }
        else
        {
            /* Call GCTP directly to allow the semi-major and semi-minor
               to be specified directly.  Both the input and output
               sphere values need to be -1, and thus the projection
               parameters for both input and output will be used. */
            /* get the input lat/long for the center */
            status = gctp_call( inproj->proj_code, inproj->zone_code,
                inproj->sphere_code, inproj->proj_coef, inproj->units,
                centerx, centery, geoproj->proj_code, geoproj->zone_code,
                -1, geoproj->proj_coef, geoproj->units,
                &temp_corners[0][0], &temp_corners[0][1] );
            if ( status != E_GEO_SUCC )
            {
                DestroyProjectionInfo( inproj );
                ErrorHandler( TRUE, "AdjustGeoPixelSizeProj", ERROR_GENERAL,
                    "Error converting output projection coordinates to "
                    "lat/long coordinates." );
            }

            /* get the lat/long for the center plus the pixelsize */
            status = gctp_call( inproj->proj_code, inproj->zone_code,
                inproj->sphere_code, inproj->proj_coef, inproj->units,
                centerxplus, centery, geoproj->proj_code, geoproj->zone_code,
                -1, geoproj->proj_coef, geoproj->units,
                &temp_corners[1][0], &temp_corners[1][1] );
            if ( status != E_GEO_SUCC )
            {
                DestroyProjectionInfo( inproj );
                ErrorHandler( TRUE, "AdjustGeoPixelSizeProj", ERROR_GENERAL,
                    "Error converting output projection coordinates to "
                    "lat/long coordinates." );
            }
	}

        /* save the pixel size */
        modis->bandinfo[nbi].output_pixel_size = temp_corners[1][0] -
            temp_corners[0][0];
    }

    DestroyProjectionInfo( inproj );
    return ( status );
}

/******************************************************************************

MODULE:  AdjustInputGeoPixelSizeProj

PURPOSE:  Convert input degree pixel size dimension of a pixel near the
          center of the input image to meters for non-geographic projections.

RETURN VALUE:
Type = none

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         10/00  Rob Burrell            Original Development

NOTES:

******************************************************************************/
int AdjustInputGeoPixelSizeProj
    (
        ModisDescriptor *modis     /* I/O:  session info */
    )
{
    ProjInfo *outproj, *geoproj; /* output/geographic projection data
                                    structures */
    ProjInfo projinfo;           /* local for geographic projection */
    int status = MRT_NO_ERROR;       /* error code status */
    long prtprm[2];              /* geolib terminal printing flags */
    size_t i;                         
    double temp_corners[4][2];   /* temp storage for proj coord corners */
    double centerlat, centerlon; /* center pixel in input lat/long space */
    double centerlonplus;        /* center longitude of the scene plus the
                                    pixel size */

    /* no printing */
    prtprm[GEO_TERM] = GEO_FALSE;
    prtprm[GEO_FILE] = GEO_FALSE;

    /* setup to use Geographic coordinates; projection parameters for
       Geographic are all zeros; units are in degrees */
    projinfo.proj_code = GEO;
    projinfo.units = DEGREE;
    projinfo.zone_code = 0;
    for ( i = 0; i < NUM_PROJECTION_PARAMS; i++ )
        projinfo.proj_coef[i] = 0.0;

    geoproj = &projinfo;
    outproj = GetOutputProjection( modis );

    /* since we have input lat/longs, then use the input datum code */
    geoproj->datum_code = modis->input_datum_code;

    /* find the center pixel for the scene in output projection coordinates */
    centerlon = modis->ll_spac_sub_gring_corners[UL][1] +
        ( modis->ll_spac_sub_gring_corners[UR][1] -
          modis->ll_spac_sub_gring_corners[UL][1] ) / 2.0;
    centerlat = modis->ll_spac_sub_gring_corners[LL][0] +
        ( modis->ll_spac_sub_gring_corners[UL][0] -
          modis->ll_spac_sub_gring_corners[LL][0] ) / 2.0;

    /* Initialize to go from input lat/long space to output projection coords */
    if ( modis->output_datum_code != E_NODATUM )
    {   /* Don't call c_transinit or c_trans if a datum shift is not
           going to happen. */
        status = c_transinit( &geoproj->proj_code, &geoproj->units,
            &geoproj->zone_code, &geoproj->datum_code, geoproj->proj_coef,
            &outproj->proj_code, &outproj->units, &outproj->zone_code,
            &outproj->datum_code, outproj->proj_coef, prtprm, NULL );
        if ( status != MRT_NO_ERROR )
        {
            DestroyProjectionInfo( outproj );
            ErrorHandler( TRUE, "AdjustInputGeoPixelSizeProj", ERROR_GENERAL,
                "Error in c_transinit for spatial subset" );
        }
    }

    /* find the center point in output proj coords for each band */
    for ( i = 0; i < modis->nbands; i++ )
    {
        /* this isn't entirely correct, at least from a projection
         * standpoint, but considering the resolution of the instrument
         * it is good enough */
        centerlonplus = centerlon + modis->bandinfo[i].pixel_size;

        if ( modis->output_datum_code != E_NODATUM )
        {
            /* Possible datum shift */
            /* get the output proj coords for the center */        
            status = c_trans( &geoproj->proj_code, &geoproj->units,
                &outproj->proj_code, &outproj->units,
                &centerlon, &centerlat, &temp_corners[0][0],
                &temp_corners[0][1] );
            if( status != E_GEO_SUCC )
            {
                DestroyProjectionInfo( outproj );
                ErrorHandler( TRUE, "AdjustInputGeoPixelSizeProj",
                    ERROR_GENERAL,
                    "Error in c_trans for lat/long of the center pixel" );
            }

            /* get the output proj coords for the center plus the pixelsize */
            status = c_trans( &geoproj->proj_code, &geoproj->units,
                &outproj->proj_code, &outproj->units,
                &centerlonplus, &centerlat, &temp_corners[1][0],
                &temp_corners[1][1] );
            if ( status == E_GEO_FAIL )
            {
                DestroyProjectionInfo( outproj );
                ErrorHandler( TRUE, "AdjustInputGeoPixelSizeProj",
                    ERROR_GENERAL,
                    "Error in c_trans for lat/long of center pixel plus the "
                    "pixel size" );
            }
        }
        else
        {
            /* Call GCTP directly to allow the semi-major and semi-minor
               to be specified directly.  Both the input and output
               sphere values need to be -1, and thus the projection
               parameters for both input and output will be used. */
            /* get the output proj coords for the center */
            status = gctp_call( geoproj->proj_code, geoproj->zone_code, -1,
                geoproj->proj_coef, geoproj->units, centerlon, centerlat,
                outproj->proj_code, outproj->zone_code, outproj->sphere_code,
                outproj->proj_coef, outproj->units,
                &temp_corners[0][0], &temp_corners[0][1] );
            if ( status != E_GEO_SUCC )
            {
                DestroyProjectionInfo( outproj );
                ErrorHandler( TRUE, "AdjustInputGeoPixelSizeProj",
                    ERROR_GENERAL, "Error converting lat/long coordinates to "
                    "output projection coordinates." );
            }

            /* get the output proj coords for the center plus the pixelsize */
            status = gctp_call( geoproj->proj_code, geoproj->zone_code, -1,
                geoproj->proj_coef, geoproj->units, centerlonplus, centerlat,
                outproj->proj_code, outproj->zone_code, outproj->sphere_code,
                outproj->proj_coef, outproj->units,
                &temp_corners[1][0], &temp_corners[1][1] );
            if ( status == E_GEO_FAIL )
            {
                DestroyProjectionInfo( outproj );
                ErrorHandler( TRUE, "AdjustInputGeoPixelSizeProj",
                    ERROR_GENERAL, "Error converting lat/long coordinates to "
                    "output projection coordinates." );
            }
	}

        /* save the pixel size */
        modis->bandinfo[i].output_pixel_size = temp_corners[1][0] -
            temp_corners[0][0];
    }

    DestroyProjectionInfo( outproj );
    return ( status );
}

/******************************************************************************

MODULE:  CheckDateline

PURPOSE: Check the subset corner points to make sure they do not cross the
         international dateline.  If the corners straddle the international
         dateline, then find the corner point with a longitude that is
         closest to 180 degrees and change the sign on that longitude value.

RETURN VALUE:
Type = none

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         12/01  Gail Schmidt           Original Development

NOTES:

******************************************************************************/
void CheckDateline
(
    ModisDescriptor *modis    /* I/O: structure which contains the subset
                                      corner points */
)
{
    double east_bound;  /* east bounding longitude for the subset */
    double west_bound;  /* west bounding longitude for the subset */
    double dist_ul;     /* distance of the UL longitude from 180 degrees */
    double dist_lr;     /* distance of the LR longitude from 180 degrees */
    int i;              /* looping variable */

    /* Determine the west-bounding longitude */
    if ( modis->ll_spac_sub_gring_corners[UL][1] <
         modis->ll_spac_sub_gring_corners[LL][1] )
        west_bound = modis->ll_spac_sub_gring_corners[UL][1];
    else
        west_bound = modis->ll_spac_sub_gring_corners[LL][1];

    /* Determine the east-bounding longitude */
    if ( modis->ll_spac_sub_gring_corners[UR][1] >
         modis->ll_spac_sub_gring_corners[LR][1] )
        east_bound = modis->ll_spac_sub_gring_corners[UR][1];
    else
        east_bound = modis->ll_spac_sub_gring_corners[LR][1];

    /* Do the subset longitudes cross the international dateline? If the
       west bounding coord is greater than the east bounding coord, then we
       have crossed the international dateline. */
    if ( west_bound > east_bound )
    {
        /* Find the longitude value that is closest to 180 degrees and
           change its sign to put that corner point on the same side of the
           dateline as the other corner points. */
        dist_ul = 180.0 - fabs( modis->ll_spac_sub_gring_corners[UL][1] );
        dist_lr = 180.0 - fabs( modis->ll_spac_sub_gring_corners[LR][1] );
        if ( dist_ul < dist_lr )
        {
            /* The UL corner is closest to 180 degrees, so change the sign
               for the UL and LL corners' longitude. */
            modis->ll_spac_sub_gring_corners[UL][1] *= -1.0;
            modis->ll_spac_sub_gring_corners[LL][1] *= -1.0;
        }
        else
        {
            /* The LR corner is closest to 180 degrees, so change the sign
               for the UR and LR corners' longitude. */
            modis->ll_spac_sub_gring_corners[UR][1] *= -1.0;
            modis->ll_spac_sub_gring_corners[LR][1] *= -1.0;
        }

        MessageHandler( "CheckDateline",
            "Warning: This tile subset straddles the international dateline. "
            "The east/west corners closest to the dateline will be changed "
            "so that they reside on the same side of the dateline as the "
            "rest of the tile." );
    }

    /* Recalculate the input spatial subset projection coords for the new
       lat/long corner points */
    for ( i = 0; i < 4; i++ )
    {
        GetInputProjCorner( modis, modis->ll_spac_sub_gring_corners[i][0],
            modis->ll_spac_sub_gring_corners[i][1],
            &modis->proj_spac_sub_gring_corners[i][0],
            &modis->proj_spac_sub_gring_corners[i][1] );
    }
}

/******************************************************************************

MODULE:  WalkInputBoundary

PURPOSE:  Walk the extents of the input corners and get the max/min x,y values
          in the output space.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status          See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         12/07  Gail Schmidt           Original development

NOTES:

******************************************************************************/
int WalkInputBoundary
(
    ModisDescriptor *modis,    /* I: session info */
    FileDescriptor *infile,    /* I: input file (uses pixel size) */
    double incorners[4][2],    /* I: corner points in input space */ 
    double *minx,              /* O: minimum x value in output space */
    double *maxx,              /* O: maximum x value in output space */
    double *miny,              /* O: minimum y value in output space */
    double *maxy               /* O: maximum y value in output space */
)
{
    ProjInfo *inproj, *outproj;   /* input/output projection data structures */
    int status = ERROR_GENERAL;   /* error code status */
    long prtprm[2];               /* geolib terminal printing flags */
    double inlat, inlon;          /* input lat/long coordinates for walking
                                     around the edges of the input rectangle */
    double inx, iny;              /* input coordinates for walking around the
                                     edges of the input rectangle */
    double outx, outy;            /* output coordinates of the final output
                                     rectangle */

    /* Print nothing */
    prtprm[GEO_TERM] = GEO_FALSE;
    prtprm[GEO_FILE] = GEO_FALSE;

    /* Get the projection info */
    inproj = modis->in_projection_info;
    outproj = modis->out_projection_info;

    /* Going from input to output projection space, so initialize c_trans
       if there is a datum transformation */
    if ( modis->output_datum_code != E_NODATUM )
    {
        status = c_transinit( &inproj->proj_code, &inproj->units,
            &inproj->zone_code, &inproj->datum_code,
            inproj->proj_coef, &outproj->proj_code, &outproj->units,
            &outproj->zone_code, &outproj->datum_code,
            outproj->proj_coef, prtprm, NULL );
        if ( status != MRT_NO_ERROR )
        {
            ErrorHandler( TRUE, "WalkInputBoundary", ERROR_GENERAL,
                "Error in c_transinit for input coords to output "
                "coords" );
        }
    }

    /* Initialize min/max x/y to find the output rectangle */
    *minx = *miny = MRT_FLOAT4_MAX;
    *maxx = *maxy = -MRT_FLOAT4_MAX;

    /* Let's walk the input projection boundary and convert to output
       projection.  If the input value falls within the discontinuity
       space, then we'll ignore it. */
    inproj = modis->in_projection_info;
    outproj = modis->out_projection_info;

    /* Left side */
    inx = incorners[UL][0];
    iny = incorners[UL][1];
    while ( iny >= incorners[LL][1] )
    {
        /* If this input coordinate doesn't fall within the discontinuity
           space then use it to look at the output projection boundary */
        if ( GetInputGeoCorner( modis, inx, iny, &inlat, &inlon) !=
             GCTP_ERANGE )
        {
            if ( modis->output_datum_code != E_NODATUM )
            {
                /* Possible datum shift */
                status = c_trans( &inproj->proj_code, &inproj->units,
                     &outproj->proj_code, &outproj->units,
                     &inx, &iny, &outx, &outy );
            }
            else
            {
                /* Call GCTP directly to allow the semi-major and semi-minor
                   to be specified directly.  Both the input and output
                   sphere values need to be -1, and thus the projection
                   parameters for both input and output will be used. */
                status = gctp_call( inproj->proj_code, inproj->zone_code,
                    inproj->sphere_code, inproj->proj_coef, inproj->units,
                    inx, iny, outproj->proj_code, outproj->zone_code,
                    outproj->sphere_code, outproj->proj_coef, outproj->units,
                    &outx, &outy );
            }
                     
            /* Only use the point if the output projection value is valid */
            if ( status == E_GEO_SUCC )
            {
                if( outy < *miny ) *miny = outy;
                if( outy > *maxy ) *maxy = outy;
                if( outx < *minx ) *minx = outx;
                if( outx > *maxx ) *maxx = outx;
            }
        }

        /* Decrement the input y value by one pixel */
        iny -= infile->pixel_size;
    }

    /* Right side */
    inx = incorners[UR][0];
    iny = incorners[UR][1];
    while ( iny >= incorners[LR][1] )
    {
        /* If this input coordinate doesn't fall within the discontinuity
           space then use it to look at the output projection boundary */
        if ( GetInputGeoCorner( modis, inx, iny, &inlat, &inlon) !=
             GCTP_ERANGE )
        {
            if ( modis->output_datum_code != E_NODATUM )
            {
                /* Possible datum shift */
                status = c_trans( &inproj->proj_code, &inproj->units,
                     &outproj->proj_code, &outproj->units,
                     &inx, &iny, &outx, &outy );
            }
            else
            {
                /* Call GCTP directly to allow the semi-major and semi-minor
                   to be specified directly.  Both the input and output
                   sphere values need to be -1, and thus the projection
                   parameters for both input and output will be used. */
                status = gctp_call( inproj->proj_code, inproj->zone_code,
                    inproj->sphere_code, inproj->proj_coef, inproj->units,
                    inx, iny, outproj->proj_code, outproj->zone_code,
                    outproj->sphere_code, outproj->proj_coef, outproj->units,
                    &outx, &outy );
            }
                     
            /* Only use the point if the output projection value is valid */
            if ( status == E_GEO_SUCC )
            {
                if( outy < *miny ) *miny = outy;
                if( outy > *maxy ) *maxy = outy;
                if( outx < *minx ) *minx = outx;
                if( outx > *maxx ) *maxx = outx;
            }
        }

        /* Decrement the input y value by one pixel */
        iny -= infile->pixel_size;
    }

    /* Top row */
    inx = incorners[UL][0];
    iny = incorners[UL][1];
    while ( inx <= incorners[UR][0] )
    {
        /* If this input coordinate doesn't fall within the discontinuity
           space then use it to look at the output projection boundary */
        if ( GetInputGeoCorner( modis, inx, iny, &inlat, &inlon) !=
             GCTP_ERANGE )
        {
            if ( modis->output_datum_code != E_NODATUM )
            {
                /* Possible datum shift */
                status = c_trans( &inproj->proj_code, &inproj->units,
                     &outproj->proj_code, &outproj->units,
                     &inx, &iny, &outx, &outy );
            }
            else
            {
                /* Call GCTP directly to allow the semi-major and semi-minor
                   to be specified directly.  Both the input and output
                   sphere values need to be -1, and thus the projection
                   parameters for both input and output will be used. */
                status = gctp_call( inproj->proj_code, inproj->zone_code,
                    inproj->sphere_code, inproj->proj_coef, inproj->units,
                    inx, iny, outproj->proj_code, outproj->zone_code,
                    outproj->sphere_code, outproj->proj_coef, outproj->units,
                    &outx, &outy );
            }
                     
            /* Only use the point if the output projection value is valid */
            if ( status == E_GEO_SUCC )
            {
                if( outy < *miny ) *miny = outy;
                if( outy > *maxy ) *maxy = outy;
                if( outx < *minx ) *minx = outx;
                if( outx > *maxx ) *maxx = outx;
            }
        }

        /* Increment the input x value by one pixel */
        inx += infile->pixel_size;
    }

    /* Bottom row */
    inx = incorners[LL][0];
    iny = incorners[LL][1];
    while ( inx <= incorners[LR][0] )
    {
        /* If this input coordinate doesn't fall within the discontinuity
           space then use it to look at the output projection boundary */
        if ( GetInputGeoCorner( modis, inx, iny, &inlat, &inlon) !=
             GCTP_ERANGE )
        {
            if ( modis->output_datum_code != E_NODATUM )
            {
                /* Possible datum shift */
                status = c_trans( &inproj->proj_code, &inproj->units,
                     &outproj->proj_code, &outproj->units,
                     &inx, &iny, &outx, &outy );
            }
            else
            {
                /* Call GCTP directly to allow the semi-major and semi-minor
                   to be specified directly.  Both the input and output
                   sphere values need to be -1, and thus the projection
                   parameters for both input and output will be used. */
                status = gctp_call( inproj->proj_code, inproj->zone_code,
                    inproj->sphere_code, inproj->proj_coef, inproj->units,
                    inx, iny, outproj->proj_code, outproj->zone_code,
                    outproj->sphere_code, outproj->proj_coef, outproj->units,
                    &outx, &outy );
            }
                     
            /* Only use the point if the output projection value is valid */
            if ( status == E_GEO_SUCC )
            {
                if( outy < *miny ) *miny = outy;
                if( outy > *maxy ) *maxy = outy;
                if( outx < *minx ) *minx = outx;
                if( outx > *maxx ) *maxx = outx;
            }
        }

        /* Increment the input x value by one pixel */
        inx += infile->pixel_size;
    }

    return ( MRT_NO_ERROR );
}
