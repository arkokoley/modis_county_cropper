
/******************************************************************************

FILE:  bi_res.c

PURPOSE:  Bi-linear resampling

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  Rob Burrell            
         10/00  John Weiss             Handle background fill values using
                                       "majority rule".
         11/01  Gail Schmidt           If ISIN_ERANGE error is returned from
                                       c_trans, then that pixel should be
                                       assigned a background fill value.
         04/02  Gail Schmidt           Changed data pointers from floats to
                                       doubles
         10/02  Gail Schmidt           Handle ISIN shift
         11/02  Gail Schmidt           ISIN_ERANGE is changed to GCTP_ERANGE to
                                       also support SIN and other projs
         02/03  Gail Schmidt           Added support for IN_BREAK returned from
                                       Interrupted Goode's
         02/05  Gail Schmidt           Modified to resample from the center of
                                       a pixel since resample weights are set
				       up for center of the pixel
         01/07  Gail Schmidt           Modified the call to GCTP to send in
                                       the spherecode which will be used for
                                       UTM only

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:
  1. Based on the LAS bi_resample module.  Removed all the static 
     variables from the original and simplified things a bit.
  2. When resampling, we want to get the pixel value that relates to the
     center of the output pixel for filling the output pixel.
  3. When mapping the output pixel back to input space, remember that the
     UL coordinates refer to the outer extent of the image.  Thus, when
     determining the closest pixel(s), no rounding should occur.  Pixel 1
     covers 0.0 to 0.9999; pixel 2 covers 1.0 to 1.9999; etc.  Rounding
     would produce inappropriate results for the "nearest pixel".  If the
     UL corner values referred to the center of the pixel, then rounding
     would be appropriate.

******************************************************************************/

#include "resample.h"
#include "worgen.h"
#include "cproj.h"
#include "mrt_dtype.h"

/************************************
 * 
 * Local prototypes
 * 
 ************************************/

double GetBIInterpValue( double x, double y, double background,
  FileDescriptor *input, int is_isin, double *delta_s_start,
  double *delta_s_slope );

/******************************************************************************

MODULE:  BiResample

PURPOSE:  Performs Bi-Linear resampling

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status          See mrt_error.h for a complete list

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00   Rob Burrell            Original Development
         04/02   Gail Schmidt           Added call to GCTP directly

NOTES:

******************************************************************************/

int BIResample
(
    ModisDescriptor *modis,     /* I: session info */
    FileDescriptor *input,      /* I: input file info */
    FileDescriptor *output,     /* I: output file info */
    int last_band               /* I: is this the last band to be processed */
)

{
    ProjInfo *inproj, *outproj;	/* input/output projection data for geolib */
    int status = MRT_NO_ERROR;	/* return status error code */
    size_t i, j, k;		/* loop & progress indices */
    int is_isin;                /* is the input projection ISIN? */
    double row, col;		/* input file row/col position */
    double *buffer = NULL;      /* output buffer */
    double inx, iny, outx, outy;	/* input/output coordinates */
    long prtprm[2];		/* logging flags for geolib */
    double background;		/* background fill value */
    double upleft_x, upleft_y;  /* upper left projection coordinates */
    double delta_s;             /* shift for ISIN shift calculation */
    double delta_s_end;         /* ending shift for ISIN shift calculation */
    long count = 0;             /* count of delta_s values */
    double min_delta = 99.0;    /* minimum delta_s value */
    double max_delta = 0.0;     /* maximum delta_s value */
    double sum_delta = 0.0;     /* sum of delta_s values for average */

    /* static variables to be reused for bands of the same resolution */
    static double *delta_s_start = NULL;
                                /* array of starting shifts for the ISIN
                                   shift calculation */
    static double *delta_s_slope = NULL;
                                /* array of slope values for the delta_s */
    static double previous_pixel_size = 0.0;
                                /* value of the previous band's pixel size */

    /* store background fill value in scalar var for faster access */
    background = input->background_fill;

    /* get the projection coordinate corners for input, from lat/long */
    GetInputImageCorners( modis, input );

    /* get the projection coordinate corners for output, from lat/long */
    GetOutputImageCorners( modis, output, input );

    /* calculate the number of rows/cols for output */
    GetOutputExtents( output );

    /* calculate the output coords in lat/long. store both the lat/long
     * and projection coords (output) in the modis descriptor. */
    StoreOutputExtents( modis, output );

    /* make things more readable */
    inproj = modis->in_projection_info;
    outproj = modis->out_projection_info;

    /* initialize the output file with the projection information */
    status = InitOutputFile( input, output, modis );
    if ( status != MRT_NO_ERROR )
        return ( status );

    /* set the value for the upper left x and y coordinates from the original
       coordinates in the file */
    if ( modis->use_bound )
    {
        /* this is an ISIN bounding tile, so use the original corner coords */
        upleft_x = modis->orig_coord_corners[UL][0];
        upleft_y = modis->orig_coord_corners[UL][1];
    }
    else
    {
        /* this is not an ISIN bounding tile, so use the corner coords listed
           in the input file descriptor */
        upleft_x = input->coord_corners[UL][0];
        upleft_y = input->coord_corners[UL][1];
    }

    /* is the input projection ISIN? */
    if ( inproj->proj_code == ISINUS )
        is_isin = TRUE;
    else
        is_isin = FALSE;

    /* if the input projection is ISIN then calculate the delta_s slopes
       for each line. if the resolution of this band is the same as the
       previous band, then the delta_s calculations have already been done. */
    if ( ( inproj->proj_code == ISINUS ) &&
         ( input->pixel_size != previous_pixel_size ) )
    {
        MessageHandler( "\nBIResample",
            "Calculating ISIN shifts for input image" );
        fprintf( stdout, "%% complete (" MRT_SIZE_T_FMT " rows): 0%%",
                 input->nrows );
        fflush( stdout );

        /* initialize space to store nrows starting shift values and
           shift slopes */
        if ( delta_s_start != NULL )
            free( delta_s_start );
        delta_s_start = ( double * ) calloc( input->nrows, sizeof( double ) );
        if ( delta_s_start == NULL )
        {
            ErrorHandler( TRUE, "BIResample", ERROR_MEMORY,
               "Error allocating space for the delta_s_start buffer" );
        }

        if ( delta_s_slope != NULL )
            free( delta_s_slope );
        delta_s_slope = ( double * ) calloc( input->nrows, sizeof( double ) );
        if ( delta_s_slope == NULL )
        {
            free( delta_s_start );
            ErrorHandler( TRUE, "BIResample", ERROR_MEMORY,
               "Error allocating space for the delta_s_slope buffer" );
        }

        /* loop through the rows calculating the starting shift and slope
           for each row. if a starting or ending shift value is not available
           (bounding tiles) then set starting shift and slope to 0.0. */
        k = 0;
        for ( i = 0; i < input->nrows; i++ )
        {
            /* update status? */
            if ( 100 * i / input->nrows > k )
            {
                k = 100 * i / input->nrows;
                if ( k % 10 == 0 )
                {
                    fprintf( stdout, " " MRT_SIZE_T_FMT "%%", k );
                    fflush( stdout );
                }
            }

            /* calculate the delta_s for sample 0. keep for later
               calculations. */
            status = calc_isin_shift ( i, 0, inproj, upleft_x, upleft_y,
                input->pixel_size, modis->output_datum_code, &delta_s );
            if ( status == GCTP_ERANGE || status == IN_BREAK )
            {   /* The value was out of range for the projection. */
                delta_s_start[i] = -99.0;
            }
            else if ( status != E_GEO_SUCC )
            {
                free( delta_s_start );
                free( delta_s_slope );
                ErrorHandler( FALSE, "BIResample", ERROR_GENERAL,
                    "Error calculating the ISIN delta_s_start value." );
                return( status );
            }
            else
            {
                delta_s_start[i] = delta_s;
            }

            /* calculate the delta_s for sample ncols-1 */
            status = calc_isin_shift ( i, input->ncols - 1, inproj, upleft_x,
                upleft_y, input->pixel_size, modis->output_datum_code,
                &delta_s );
            if ( status == GCTP_ERANGE || status == IN_BREAK )
            {   /* The value was out of range for the projection. */
                delta_s_end = -99.0;
            }
            else if ( status != E_GEO_SUCC )
            {
                free( delta_s_start );
                free( delta_s_slope );
                ErrorHandler( FALSE, "BIResample", ERROR_GENERAL,
                    "Error calculating the ISIN delta_s_end value." );
                return( status );
            }
            else
            {
                delta_s_end = delta_s;
            }

            /* calculate the slope for the current row */
            if ( delta_s_start[i] == -99.0 || delta_s_end == -99.0 )
            {
                /* if either the start and/or end delta_s values are not
                   available then set slope to 0.0. reset delta_s_start to
                   0.0 if it is not available. */
                delta_s_slope[i] = 0.0;

                /* if delta_s_start is -99.0 then reset to 0.0 */
                if ( delta_s_start[i] == -99.0 )
                    delta_s_start[i] = 0.0;
            }
            else
            {
                delta_s_slope[i] = ( delta_s_end - delta_s_start[i] ) /
                    ( input->ncols - 1 );

                /* generate delta_s statistics. delta's commonly fall
                   between 3.25 and -3.25. */
                if ( delta_s_end < min_delta )
                    min_delta = delta_s_end;
                if ( delta_s_start[i] < min_delta )
                    min_delta = delta_s_start[i];
                if ( delta_s_end > max_delta )
                    max_delta = delta_s_end;
                if ( delta_s_start[i] > max_delta )
                    max_delta = delta_s_start[i];
                sum_delta += delta_s_end + delta_s_start[i];
                count += 2;
            }
        }  /* end for i = input->nrows */

        fprintf( stdout, " 100%%\n" );
        fflush( stdout );

        /* output ISIN shift stats */
        MessageHandler( "\nBIResample", "ISIN Shift Statistics:" );
        if ( count != 0 )
        {
            MessageHandler( NULL, "  min delta shift = %f", min_delta );
            MessageHandler( NULL, "  max delta shift = %f", max_delta );
            MessageHandler( NULL, "  avg delta shift = %f", sum_delta / count );        }
        else
        {
            MessageHandler( NULL, "  min delta shift = 0.0" );
            MessageHandler( NULL, "  max delta shift = 0.0" );
            MessageHandler( NULL, "  avg delta shift = 0.0" );
        }

        /* set the previous pixel size to the current pixel size */
        previous_pixel_size = input->pixel_size;
    }  /* end ISINUS */

    if ( modis->output_datum_code != E_NODATUM )
    {   /* Don't call c_transinit or c_trans if a datum shift is not
           going to happen. */
        /* no printing from geolib */
        prtprm[GEO_TERM] = GEO_FALSE;
        prtprm[GEO_FILE] = GEO_FALSE;

        /* initialize the inverse projection */
        status = c_transinit( &outproj->proj_code, &outproj->units,
            &outproj->zone_code, &outproj->datum_code, outproj->proj_coef,
	    &inproj->proj_code, &inproj->units, &inproj->zone_code,
	    &inproj->datum_code, inproj->proj_coef, prtprm, NULL );

        /* check projection return value */
        if ( status != MRT_NO_ERROR )
        {
            free( delta_s_start );
            delta_s_start = NULL;
            free( delta_s_slope );
            delta_s_slope = NULL;
            ErrorHandler( FALSE, "BIResample", ERROR_GENERAL,
               "Error in initializing the inverse projection (c_transinit)" );
            return ( status );
        }
    }

    /* allocate the local output buffer */
    buffer = ( double * ) calloc( output->ncols, sizeof( double ) );
    if ( buffer == NULL )
    {
        free( delta_s_start );
        delta_s_start = NULL;
        free( delta_s_slope );
        delta_s_slope = NULL;
        ErrorHandler( TRUE, "BIResample", ERROR_MEMORY,
           "Error allocating space for the row buffer" );
    }

    MessageHandler( "\nBIResample", "processing band %s",
        modis->bandinfo[input->bandnum].name );

    /* initialize status to terminal */
    fprintf( stdout, "%% complete (" MRT_SIZE_T_FMT " rows): 0%%",
             output->nrows );
    fflush( stdout );
    k = 0;

    /* loop through output rows */
    for ( i = 0; i < output->nrows; i++ )
    {
	/* update status ? */
	if ( 100 * i / output->nrows > k )
	{
	    k = 100 * i / output->nrows;
	    if ( k % 10 == 0 )
	    {
		fprintf( stdout, " " MRT_SIZE_T_FMT "%%", k );
		fflush( stdout );
	    }
	}

	/* since we're on a grid, get output northing once. pass the center
           of the pixel rather than the UL outer extent. */
	outy = output->coord_corners[UL][1] - i * output->output_pixel_size -
               output->output_pixel_size * 0.5;

	/* loop through output cols */
	for ( j = 0; j < output->ncols; j++ )
	{
	    /* easting for this pixel. pass the center of the pixel rather than
               the UL outer extent. */
	    outx = output->coord_corners[UL][0] +
              j * output->output_pixel_size + output->output_pixel_size * 0.5;

	    /* get input coords */
            if ( modis->output_datum_code != E_NODATUM )
            {
                /* Possible datum shift */
                status = c_trans( &outproj->proj_code, &outproj->units,
                    &inproj->proj_code, &inproj->units, &outx, &outy, &inx,
                    &iny );
                if ( status == GCTP_ERANGE || status == IN_BREAK )
                {   /* The value was out of range for the projection so
                       just set it as a background pixel. */
                    buffer[j] = background;
                    continue;
                }
                else if ( status != E_GEO_SUCC )
                {
                    ErrorHandler( FALSE, "BIResample", ERROR_GENERAL,
                        "Error converting output projection coordinates to "
                        "input projection coordinates." );
                    free( delta_s_start );
                    delta_s_start = NULL;
                    free( delta_s_slope );
                    delta_s_slope = NULL;
                    free( buffer );
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
                    outx, outy, inproj->proj_code, inproj->zone_code,
                    inproj->sphere_code, inproj->proj_coef, inproj->units,
                    &inx, &iny );
                if ( status == GCTP_ERANGE || status == IN_BREAK )
                {   /* The value was out of range for the projection so
                       just set it as a background pixel. */
                    buffer[j] = background;
                    continue;
                }
                else if ( status != E_GEO_SUCC )
                {
                    ErrorHandler( FALSE, "BIResample", ERROR_GENERAL,
                        "Error converting output projection coordinates to "
                        "input projection coordinates." );
                    free( delta_s_start );
                    delta_s_start = NULL;
                    free( delta_s_slope );
                    delta_s_slope = NULL;
                    free( buffer );
                    return( status );
                }
	    }

	    /* get input row/col */
	    col = ( inx - upleft_x ) / input->pixel_size;
	    row = ( upleft_y - iny ) / input->pixel_size;

	    /* resample from input */
	    buffer[j] = GetBIInterpValue( col, row, background, input,
                is_isin, delta_s_start, delta_s_slope );
	}

	/* write the resampled row to output */
	if ( !WriteRow( output, i, buffer ) )
        {
            ErrorHandler( FALSE, "BIResample", ERROR_GENERAL,
                "Error writing the resampled row to the output file.");
            free( delta_s_start );
            delta_s_start = NULL;
            free( delta_s_slope );
            delta_s_slope = NULL;
            free( buffer );
            return( E_GEO_FAIL );
        }
    }

    fprintf( stdout, " 100%%\n" );
    fflush( stdout );

    /* free up the allocated memory. don't free the static variables until
       the last band. */
    free( buffer );
    if ( inproj->proj_code == ISINUS && last_band == TRUE )
    {
        free( delta_s_start );
        delta_s_start = NULL;
        free( delta_s_slope );
        delta_s_slope = NULL;
    }

    return ( MRT_NO_ERROR );
}

/******************************************************************************

MODULE:  GetBIInterpValue

PURPOSE:  Bi-Linear resampling kernel

RETURN VALUE:
Type = double
Value           Description
-----           -----------
pixel           Interpolated pixel value

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  Rob Burrell            Original Development
         10/00  John Weiss             Handle background fill values correctly

NOTES:

******************************************************************************/

double GetBIInterpValue
(
    double x,                /* I: input sample */
    double y,                /* I: input line */
    double background,	     /* I: background fill */
    FileDescriptor *input,   /* I: input file data */
    int is_isin,             /* I: is this the ISIN projection */
    double *delta_s_start,   /* I: array of starting shifts */
    double *delta_s_slope    /* I: array of shifts slopes for shifts */
)

{
    register size_t x1, x2;	/* integer pixel locations in image */
    register size_t y1, y2;

    int count = 0;		/* counter for background fill values */

    double v1, v2, v3, v4;      /* the four pixel values */
    double w1, w2, w3, w4;      /* the four pixel weights */
    double v = 0.0, w = 0.0;    /* total value and total weight */
    double delta_s;             /* x shift for ISIN shift calculation */
    int delta_x1, delta_x2;     /* x1, x2 values with the deltas added */
    double delta_x;             /* x value with the delta added */

    /* if we're outside, just return */
    if ( x < 0.0 || y < 0.0  || x >= input->ncols || y >= input->nrows )
	return input->background_fill;

    x1 = ( int ) x;		/* truncate to upper left pixel */
    y1 = ( int ) y;

    /* stop blurring around edges */
    y2 = y1 + 1;
    if ( y2 >= input->nrows ) y2 = y1;

    x2 = x1 + 1;
    if ( x2 >= input->ncols ) x2 = x1;

    /* get the shift values for y2 in relation to y1 (as long as they are
       not the same value). if y1 and y2 are the same value then the
       delta_s value will be 0.0. For non-ISIN projections, delta_s will also
       be 0.0. */
    if ( !is_isin )
    {
        /* Not ISIN so delta_s is 0 */
        delta_s = 0.0;
    }
    else if ( y1 == y2 )
    {
        /* We have not moved to the next line so delta_s is 0 */
        delta_s = 0.0;
    }
    else
    {
        delta_s = delta_s_start[y1] + x * delta_s_slope[y1];
    }

    /* get 4 values for bilinear resample. use the delta_s values for v3, v4
       and round to closest integer. */
    v1 = ReadBufferValue( x1, y1, input );
    v2 = ReadBufferValue( x2, y1, input );

    delta_x = x + delta_s;  /* actual value of x with shift for next line */
    delta_x1 = (int) delta_x;  /* round to closest int */
    delta_x2 = (int) (delta_x + 1.0);
    v3 = ReadBufferValue( delta_x1, y2, input );
    v4 = ReadBufferValue( delta_x2, y2, input );

    /* Potential solution for a double precision problem most noticable
     * in Cygwin.  It is currently commented out due to lack of proper
     * testing.  The solution was provided by Tamara Rockvam.
     *
     * If all 4 pixels are the same value, just return the value...
     *
     * if ((v1 == v2) && (v1 == v3) && (v1 == v4))
     *    return v1;
     */

    /* get 4 associated weights. use delta_s values for w3, w4 and round
       to closest integer. */
    w1 = ( 1.0 - ( x - x1 ) ) * ( 1.0 - ( y - y1 ) );
    w2 = ( 1.0 - ( x2 - x ) ) * ( 1.0 - ( y - y1 ) );
    w3 = ( 1.0 - ( delta_x - delta_x1 ) ) * ( 1.0 - ( y2 - y ) );
    w4 = ( 1.0 - ( delta_x2 - delta_x ) ) * ( 1.0 - ( y2 - y ) );

    /* check for majority (note: >= 50%) of background fill values */
    /* note kinda kludgy solution (substitute nearby value for background) */
    if ( background == 0.0 )    /* no background value */
        return w1 * v1 + w2 * v2 + w3 * v3 + w4 * v4;

    /* handle background values */
    if ( v1 == background )
    {
        ++count;
    }
    else
    {
        v += w1 * v1;
        w += w1;
    }

    if ( v2 == background )
    {
        if ( ++count >= 2 ) return background;
    }
    else
    {
        v += w2 * v2;
        w += w2;
    }

    if ( v3 == background )
    {
        if ( ++count >= 2 ) return background;
    }
    else
    {
        v += w3 * v3;
        w += w3;
    }

    if ( v4 == background )
    {
        if ( ++count >= 2 ) return background;
    }
    else
    {
        v += w4 * v4;
        w += w4;
    }

    return v / w;
}

