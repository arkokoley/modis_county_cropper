/******************************************************************************

FILE:  cc_res.c

PURPOSE:  Cubic Convolution resampling

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  Rob Burrell            Original Development
         10/00  John Weiss             Handle background fill values using
                                       "majority rule".
         11/01  Gail Schmidt           If ISIN_ERANGE error is returned from
                                       c_trans, then that pixel should be
                                       assigned a background fill value.
         04/02  Gail Schmidt           Changed data pointers from floats to
                                       doubles
         11/02  Gail Schmidt           ISIN_ERANGE is changed to GCTP_ERANGE to
                                       also support SIN and other projs
         11/02  Gail Schmidt           Handle ISIN shift
         02/03  Gail Schmidt           Added support for IN_BREAK returned from
                                       Interrupted Goode's
         02/05  Gail Schmidt           Modified to resample from the center of
                                       a pixel since resample weights are set
				       up for center of the pixel
         01/07  Gail Schmidt           Modified the call to GCTP to send in
                                       the sphere/datum code which will be
                                       used for UTM only

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:
  1. Based on the LAS cc_resample module.  Removed all the static 
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

/* I really hate globals, even if they only have file scope, but this is
 * the most efficient way to do it without adding another parameter
 * to each interp call
 */
static double *g_weight_table;	/* CC kernel weight table */

/************************************
 * 
 * Local prototypes
 * 
 ************************************/
double *CreateWeightTable( void );
double GetCCInterpValue( double x, double y, double background,
  FileDescriptor *input, int is_isin, double *delta_s_start,
  double *delta_s_slope );

/******************************************************************************

MODULE:  CCResample

PURPOSE:  Performs cubic resampling

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status          See mrt_error.h for a complete list

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  Rob Burrell            Original Development
         04/02  Gail Schmidt           Added call to GCTP directly

NOTES:

******************************************************************************/
int CCResample
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
    double *buffer = NULL;	/* output buffer */
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

    /* get the projection coordinate corners for both files */
    GetInputImageCorners( modis, input );
    GetOutputImageCorners( modis, output, input );

    /* calculate the number of rows/cols for output */
    GetOutputExtents( output );
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
        MessageHandler( "\nCCResample",
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
            ErrorHandler( TRUE, "CCResample", ERROR_MEMORY,
               "Error allocating space for the delta_s_start buffer" );
        }

        if ( delta_s_slope != NULL )
            free( delta_s_slope );
        delta_s_slope = ( double * ) calloc( input->nrows, sizeof( double ) );
        if ( delta_s_slope == NULL )
        {
            free( delta_s_start );
            ErrorHandler( TRUE, "CCResample", ERROR_MEMORY,
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
                ErrorHandler( FALSE, "CCResample", ERROR_GENERAL,
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
                ErrorHandler( FALSE, "CCResample", ERROR_GENERAL,
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
        MessageHandler( "\nCCResample", "ISIN Shift Statistics:" );
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
            ErrorHandler( FALSE, "CCResample", ERROR_GENERAL,
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
        ErrorHandler( TRUE, "CCResample", ERROR_MEMORY,
           "Error allocating space for the row buffer" );
    }

    /* create CC weight table */
    g_weight_table = CreateWeightTable(  );

    MessageHandler( "\nCCResample", "processing band %s",
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
            /* easting for this pixel. pass the center of the pixel rather
               than the UL outer extent. */
            outx = output->coord_corners[UL][0] +
              j * output->output_pixel_size + output->output_pixel_size * 0.5;

	    /* get input coords */
            if ( modis->output_datum_code != E_NODATUM )
            {
                status = c_trans( &outproj->proj_code, &outproj->units,
                    &inproj->proj_code, &inproj->units, &outx, &outy, &inx,
                    &iny );
                if ( status == GCTP_ERANGE || status == IN_BREAK )
                {   /* The value was out of range for the projection so
                       just set it as a background pixel. */
                    buffer[j] = background;
                    continue;
                }
                else if(status != E_GEO_SUCC)
                {
                    ErrorHandler( FALSE, "CCResample", ERROR_GENERAL,
                        "Error converting output projection coordinates to "
                        "input projection coordinates." );
                    free( delta_s_start );
                    delta_s_start = NULL;
                    free( delta_s_slope );
                    delta_s_slope = NULL;
                    free( buffer );
                    free( g_weight_table );
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
                    ErrorHandler( FALSE, "CCResample", ERROR_GENERAL,
                        "Error converting output projection coordinates to "
                        "input projection coordinates." );
                    free( delta_s_start );
                    delta_s_start = NULL;
                    free( delta_s_slope );
                    delta_s_slope = NULL;
                    free( buffer );
                    free( g_weight_table );
                    return( status );
                }
	    }

	    /* get input row/col */
            col = ( inx - upleft_x ) / input->pixel_size;
	    row = ( upleft_y - iny ) / input->pixel_size;

	    /* resample from input */
	    buffer[j] = GetCCInterpValue( col, row, background, input,
                is_isin, delta_s_start, delta_s_slope );
	}

	/* write the resampled row to output */
        if ( !WriteRow( output, i, buffer ) )
        {
            ErrorHandler( FALSE, "CCResample", ERROR_GENERAL,
                "Error writing the resampled row to the output file.");
            free( delta_s_start );
            delta_s_start = NULL;
            free( delta_s_slope );
            delta_s_slope = NULL;
            free( buffer );
            free( g_weight_table );
            return( E_GEO_FAIL );
        }
    }

    fprintf( stdout, " 100%%\n" );
    fflush( stdout );

    /* free up the allocated memory. don't free the static variables until
       the last band. */
    free( buffer );
    free( g_weight_table );
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

MODULE:  GetCCInterpValue

PURPOSE:  Cubic resampling kernel

RETURN VALUE:
Type = double
Value           Description
-----           -----------
pixel           Interpolated pixel value

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         ??/??  ?
         06/00  Rob Burrell            Modified for MODIS
         08/02  Gail Schmidt           Perform calculations using long
                                       double to prevent highly reflective
                                       values from overflowing the double
                                       buffer.

NOTES:  Adapted from LAS

******************************************************************************/

double GetCCInterpValue
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
    int kernel_line, kernel_sample;  /* index into kernel table for
                                        fractional input line/sample */
    int iy;			/* whole value for y */
    double ds, dl;		/* fractional input space line and sample */
    long double total = 0.0;	/* resampling total */
    const double *w_ptr;        /* pointer to current resampling weights */
    int i, j, k;                /* loop counters */
    int count = 0;		/* counter for background fill values */
    double value;		/* current value */
    long double total_weight = 0.0;  /* sum of non-background weights */
    double delta_s;             /* x shift for ISIN shift calculation */
    double delta_x;             /* x value with the delta added */
    int delta_ix;               /* int value for delta_x */
    int kline;                  /* what line of the kernal are we looking
                                   at in comparison to the x,y value
                                   -1  previous line
                                    0  current line
                                    1  next line
                                    2  second line */

    /* if we're outside, just return */
    if ( x < 0.0 || y < 0.0 || x >= input->ncols || y >= input->nrows )
	return input->background_fill;

    /* Convert the input line/sample into whole and fractional line/sample.
       -------------------------------------------------------------------- */
    iy = ( int ) y;
    dl = y - iy;

    /* Get the resampling kernel index for the fractional line */
    kernel_line = ( int ) ( DOUBLE_SUBPIXEL_STEPS * ( ROUND_OFF + dl ) );

    /* Loop through kernel values for resampling.
       ------------------------------------------ */
    for ( i = 0; i < LINES_IN_KERNEL; i++ )
    {
        /* Calculate the delta_s for this line. For non-ISIN projections,
           delta_s will be 0.0. */
        delta_s = 0.0;
        kline = -TOP_NUM_LINES + i;

        if ( !is_isin )
        {
            /* Not ISIN so delta_s is 0 */
            delta_s = 0.0;
        }
        else if ( kline < 0 )
        {
            /* Subtract the delta_s values since we are going back a line.
               delta_s values are calculated assuming we are comparing to
               the next line down. Also, delta_s values are additive so
               we can add delta_s values for adjacent lines to get the overall
               delta_s value. */
            for ( k = kline; k < 0; k++ )
            {
                delta_s -= ( delta_s_start[iy + k] +
                    x * delta_s_slope[iy + k] );
            }
        }
        else if ( kline == 0 )
        {
            /* No delta_s value is needed for the x, y line */
            delta_s = 0.0;
        }
        else
        {
            /* Add the delta_s values.  delta_s values are additive so we
               can add delta_s values for adjacent lines to get the overall
               delta_s value. */
            for ( k = 0; k < kline; k++ )
            {
                delta_s += ( delta_s_start[iy + k] +
                    x * delta_s_slope[iy + k] );
            }
        }

        /* Get the new x value with the delta_s added. Calculate the
           fractional part of the sample vaue. */
        delta_x = x + delta_s;
        delta_ix = ( int ) delta_x;
        ds = delta_x - delta_ix;

        /* Get the resampling kernel index for the fractional sample */
        kernel_sample = ( int ) ( DOUBLE_SUBPIXEL_STEPS * ( ROUND_OFF + ds ) );

        /* Get pointer to weight table kenel to use */
        w_ptr = g_weight_table +
            ( kernel_line * SUBPIXEL_TABLE_ENTRIES + kernel_sample ) *
            KERNEL_SIZE;

        /* Now index to the appropriate line in the kernel */
        w_ptr += i * SAMPLES_IN_KERNEL;

        for ( j = 0; j < SAMPLES_IN_KERNEL; j++ )
        {
	    value = ReadBufferValue( delta_ix - LEFT_NUM_SAMPLES + j,
                iy - TOP_NUM_LINES + i, input );

    	    /* handle background fill values */
	    if ( background != 0.0 && value == background )
	    {
    		/* check for majority (note: >= 50%) of background fill
                   values */
	    	if ( ++count >= 8 )
		    return background;
	    }
	    else	/* valid values */
	    {
		total += *w_ptr * value;
		total_weight += *w_ptr;
	    }

            /* We will use all the weights in the current kernel line */
	    w_ptr++;
	}
    }

    /* adjust for missing background fill values */
    if ( background != 0.0 && total_weight != 0.0 )
        return ( total / total_weight );
    else
        return ( total );
}

/******************************************************************************

MODULE:  cubic_convolution

PURPOSE:  The cubic_convolution function calculates the cubic convolution 
    weight for a point.

RETURN VALUE:
Type = double
Value           Description
-----           -----------
ccw             The cubic convolution weight

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         ??/??  ?
         06/00  Rob Burrell            Modified for MODIS
         08/02  Gail Schmidt           Perform calculations using long
                                       double to prevent highly reflective
                                       values from overflowing the double
                                       buffer.

NOTES:  Adapted from LAS

Park, S.K., R.A. Schowengerdt, Image Reconstruction by Parametric Cubic
Convolution, Computer Vision, Graphics and Image Processing, v.23, no.3,
September 1983.

******************************************************************************/
double cubic_convolution(
    double alpha,       /* I: cubic convolution alpha parameter     */
    double x            /* I: value to perform cubic convolution on */
     )
{
    long double fx,      /* absolute value of x          */
                fx2,     /* x squared                    */
                fx3;     /* absolute value of x cubed    */
    long double ccw;	 /* cubic convolution weight     */

    fx = fabs( x );

    /* Calculate weight for piecewise defined function.
       --------------------------------------------------- */
    if ( fx < 1.0 )
    {
	/* Calculate weight when x is less than 1.
	   ------------------------------------------ */
	fx2 = fx * fx;
	fx3 = fx2 * fx;
	ccw = ( alpha + 2.0 ) * fx3 - ( alpha + 3.0 ) * fx2 + 1.0;
    }
    else if ( fx < 2.0 )
    {
	/* Calculate weight when x is less than 2, but greater than or
	   equal to 1.
	   -------------------------------------------------------------- */
	fx2 = fx * fx;
	fx3 = fx2 * fx;
	ccw = alpha * fx3 - 5.0 * alpha * fx2 + 8.0 * alpha * fx - 4.0 * alpha;
    }
    else
    {
	/* Weight is zero when x is two or more.
	   ---------------------------------------- */
	ccw = 0.0;
    }

    return ( (double) ccw );
}

/******************************************************************************

MODULE:  CreateWeightTable

PURPOSE:  See notes below

RETURN VALUE:
Type = double
Value           Description
-----           -----------
weight_table    The cubic convolution weight table

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         ??/??  ?
         06/00  Rob Burrell            Modified for MODIS

NOTES:  Adapted from LAS

******************************************************************************/

/*****************************************************************************
NAME:           kernel

PURPOSE:        
The kernel module contains the routines needed to create and manage the 
resampling kernel.  The kernel is the weight table used for spatially 
resampling pixels.

ROUTINES:
    get_lines_in_kernel
    get_samples_in_kernel
    num_left_kernel_samples
    num_right_kernel_samples
    num_top_kernel_lines
    num_bottom_kernel_lines
    get_kernel_step_size

DEVELOPMENT HISTORY:
  Baselined from revision 1.14 of kernel.c

NOTES:
- The kernel module will build the resampling weight table for either 
  cubic convolution or mtf resampling.  The kernel is built for the current 
  band being resampled.
- The Kernel_Setup routine also handles the bilinear resampling option by
  setting the kernel size correctly for bilinear resampling.  However, no
  resampling weight table is built for bilinear resampling.

ALGORITHM REFERENCES:
    The algorithm for constructing the kernel from the five
    coefficients is documented in:
       "Cubic Convolution for One-Pass Restoration and Resampling"
       by Stephen E. Reichenbach and Kevin Haake
       Presented at IGARSS '96
    Note: this reference is actually for MTF resampling which is currently
    disabled (although MTF is a generic form of cubic convolution).

*****************************************************************************/

/*****************************************************************************
NAME:           Create_Resampling_Kernel

PURPOSE:
Create_Resampling_Kernel builds the weight table for the resampling kernel.
The weights are setup properly for mtf or cubic convolution resampling, with
or without detector delays taken into account.

RETURN VALUE:
Either E_GEO_SUCC or E_GEO_FAIL are returned, depending on whether the kernel 
creation is successful or not.

NOTES:
The MTF parameters allow for having separate forward and reverse parameters.
This is not currently used, but is included for future growth.

ALGORITHM REFERENCES:
none

*****************************************************************************/

double *CreateWeightTable( void )
{
    int num_left_kernel_samples( void );
    int num_right_kernel_samples( void );
    int num_top_kernel_lines( void );
    int num_bottom_kernel_lines( void );

    int kernel_line,            /* Counters                              */
        kernel_sample,
        subpixel_line,
        subpixel_sample;
    int left_samp,              /* size of the kernel, relative to the   */
        right_samp,             /* center of the kernel                  */
        top_line,
        bottom_line;
    double dx, dy;		/* Sub-pixel spacing for cubic           */
    double x, y;		/* X and Y location of value to calc     */
    double *w_ptr;		/* pointer to walk the weight table       */
    double *weight_table;	/* the allocated space */
    double ccwy;		/* weight for current y (line) component */
    int kernel_table_size;
    double alpha = CC_ALPHA;

    kernel_table_size = SUBPIXEL_TABLE_ENTRIES * SUBPIXEL_TABLE_ENTRIES
	* LINES_IN_KERNEL * SAMPLES_IN_KERNEL;

    w_ptr = weight_table =
	( double * ) calloc( kernel_table_size, sizeof( double ) );

    if ( !w_ptr )
	ErrorHandler( TRUE, "CreateWeightTable", ERROR_MEMORY,
	    "Creating Weight Table" );

    /* Get the kernel dimensions.
       ----------------------------- */
    left_samp = -num_left_kernel_samples(  );
    right_samp = num_right_kernel_samples(  );
    top_line = -num_top_kernel_lines(  );
    bottom_line = num_bottom_kernel_lines(  );

    /* Build the kernel for either cubic convolution or mtf.
       -------------------------------------------------------- */
    for ( subpixel_line = 0; subpixel_line <= SUBPIXEL_STEPS; subpixel_line++ )
    {
	/* Calculate subpixel line step.
	   -------------------------------- */
	dy = subpixel_line / ( double ) ( SUBPIXEL_STEPS );

	for ( subpixel_sample = 0; subpixel_sample <= SUBPIXEL_STEPS;
	    subpixel_sample++ )
	{
	    /* Calculate subpixel sample step.
	       ---------------------------------- */
	    dx = subpixel_sample / ( double ) ( SUBPIXEL_STEPS );

	    for ( kernel_line = top_line; kernel_line <= bottom_line;
                  kernel_line++ )
	    {
		/* Adjust the kernel line for the current subpixel step.  
		   The subpixel step is subtracted since since a
		   subpixel step down makes the weight at the current
		   line position act like it moved up.
		   -------------------------------------------------------- */
		y = ( double ) kernel_line - dy;

		/* Get the weight for the line.
		   ------------------------------- */
		ccwy = cubic_convolution( alpha, y );

		for ( kernel_sample = left_samp; kernel_sample <= right_samp;
		    kernel_sample++ )
		{
		    /* Adjust the kernel sample location for the current 
		       subpixel step.
		       ---------------------------------------------------- */
		    x = ( double ) kernel_sample - dx;

		    /* Get the weight for the current pixel.
		       ---------------------------------------- */
		    *w_ptr++ = ccwy * cubic_convolution( alpha, x );
		}  /* kernel samples */
	    }  /* kernel lines */
	}  /* subpixel samples */
    }  /* subpixel lines */

    return ( weight_table );
}

/*****************************************************************************
NAME:           get_lines_in_kernel, get_samples_in_kernel

PURPOSE:
The get_lines_in_kernel and get_samples_in_kernel routines provide access 
to the number of lines and samples in the resampling kernel.

RETURN VALUE:
Type = int
The number of lines/samples in the resampling kernel is returned

NOTES:
none

ALGORITHM REFERENCES:
none

*****************************************************************************/

/* These routines return the number of pixels to the left, right, top, and
   bottom of the kernel center.
--------------------------------------------------------------------------*/

int num_left_kernel_samples( void )
{
    return ( ( SAMPLES_IN_KERNEL ) - ( ( SAMPLES_IN_KERNEL ) / 2 ) - 1 );
}

int num_right_kernel_samples( void )
{
    return ( SAMPLES_IN_KERNEL / 2 );
}

int num_top_kernel_lines( void )
{
    return ( ( LINES_IN_KERNEL ) - ( ( LINES_IN_KERNEL ) / 2 ) - 1 );
}

int num_bottom_kernel_lines( void )
{
    return ( LINES_IN_KERNEL / 2 );
}
