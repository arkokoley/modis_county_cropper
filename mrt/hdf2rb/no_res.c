/******************************************************************************

FILE:  no_res.c

PURPOSE:  No resampling, just copy input to output.

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/10  John Weiss             Enhancement requested by customer(s).

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:
  Based on the NNResample() module.

******************************************************************************/
#include "shared_resample.h"
#include "worgen.h"
#include "mrt_dtype.h"

/******************************************************************************

MODULE:  NoResample

PURPOSE:  Simple translation, no resampling 

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status          See mrt_error.h for a complete list

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/01  John Weiss             Original Development
         07/01  Rob Burrell	       Fixed spatial subsetting

NOTES:

******************************************************************************/

int NoResample
(
    ModisDescriptor *modis,	/* I: session info */
    FileDescriptor *input,	/* I: input file info */
    FileDescriptor *output	/* I: output file info */
)

{
    int status = MRT_NO_ERROR;	/* return status error code */
    size_t i, j, k;		/* loop & progress indices */
    size_t row, col;		/* input file row/col position */
    double *buffer = NULL;      /* output buffer */
    double outx, outy;	        /* input/output coordinates */
    double upleft_x, upleft_y;  /* upper left projection coordinates */

    /* get the projection coordinate corners for the input file */
    GetInputImageCorners( modis, input );

    /* set the projection coordinate corners for the output file to
       be the same as the input file (use the spatial subset coords).
       note: for ISIN bounding tiles, this will be the bounding rectangular
       coordinates instead of the original ISIN tile corners. -gls */
    /* GetOutputImageCorners( modis, output, input ); */
    for ( i = 0; i < 4; i++ )
    {
        output->coord_corners[i][0] =
            modis->proj_spac_sub_gring_corners[i][0];
        output->coord_corners[i][1] =
            modis->proj_spac_sub_gring_corners[i][1];
    }

    /* calculate the number of rows/cols for output */
    /* GetOutputExtents( output ); */
    output->nrows = (int) ((((fabs)
       (output->coord_corners[UL][1] - output->coord_corners[LL][1])) /
        output->output_pixel_size) + 0.5);
    output->ncols = (int) ((((fabs)
       (output->coord_corners[UL][0] - output->coord_corners[UR][0])) /
        output->output_pixel_size) + 0.5);

    /* reallocate based on window */
    if ( output->rowbuffer )
        free ( output->rowbuffer );

    /* new buffer, things may have changed */
    output->rowbuffer = ( void * ) calloc ( output->ncols, output->datasize );
    if ( !output->rowbuffer )
    {
        ErrorHandler( TRUE, "NoResample", ERROR_MEMORY,
            "Error allocating memory for the output Row Buffer" );
    }

    /* get the geographic coordinates for the output corners */
    StoreOutputExtents( modis, output );

    /* initialize the output file with the projection information */
    status = InitOutputFile( input, output, modis );
    if ( status != MRT_NO_ERROR )
	return ( status );

    /* allocate for the row buffer */
    buffer = ( double * ) calloc( output->ncols, sizeof( double ) );
    if ( buffer == NULL )
    {
        ErrorHandler( TRUE, "NoResample", ERROR_MEMORY,
            "Error allocating space for the input row buffer" );
    }

    MessageHandler( "\nNoResample", "processing band %s",
        modis->bandinfo[input->bandnum].name );

    /* initialize status to terminal */
    fprintf( stdout, "%% complete (" MRT_SIZE_T_FMT " rows): 0%%",
             output->nrows );
    fflush( stdout );
    k = 0;

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

	/* since we're on a grid, get output northing once */
	outy = output->coord_corners[UL][1] - i * output->output_pixel_size;
	row = (size_t) (( upleft_y - outy ) / input->pixel_size + 0.5);

	/* loop through output cols */
	for ( j = 0; j < output->ncols; j++ )
	{
	    /* easting for this pixel */
	    outx = output->coord_corners[UL][0] +
                   j * output->output_pixel_size; 

	    /* get input row/col */
	    col = (int)(( outx - upleft_x ) / input->pixel_size + 0.5);

	    /* resample from input */
	    buffer[j] = ReadBufferValue( col, row, input );
	}

	/* write the resampled row to output */
        if ( !WriteRow( output, i, buffer ) )
        {
            ErrorHandler( FALSE, "NoResample", ERROR_GENERAL,
                "Error writing the resampled row to the output file.");
            free( buffer );
            return( E_GEO_FAIL );
        }
    }

    fprintf( stdout, " 100%%\n" );
    fflush( stdout );

    free( buffer );

    return ( MRT_NO_ERROR );
}


/******************************************************************************

MODULE:  NoResampleHDF2RB

PURPOSE:  Simply dump input product to output file (using the user-specified
    spatial subsetting)

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status          See mrt_error.h for a complete list

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         11/05  Gail Schmidt           Original Development

NOTES:

******************************************************************************/

int NoResampleHDF2RB
(
    ModisDescriptor *modis,	/* I: session info */
    FileDescriptor *input,	/* I: input file info */
    FileDescriptor *output	/* I: output file info */
)

{
    size_t i, j, k;		/* loop & progress indices */
    size_t row, col;		/* input file row/col position */
    double *buffer = NULL;      /* output buffer */
    double outx, outy;	        /* input/output coordinates */
    double upleft_x, upleft_y;  /* upper left projection coordinates */

    /* set the projection coordinate corners for the output file */
    for ( i = 0; i < 4; i++ )
    {
        output->coord_corners[i][0] = modis->proj_extents_corners[i][0];
        output->coord_corners[i][1] = modis->proj_extents_corners[i][1];
    }

    /* calculate the number of rows/cols for output */
    output->nrows = (int) ((((fabs)
       (output->coord_corners[UL][1] - output->coord_corners[LL][1])) /
        output->output_pixel_size) + 0.5);
    output->ncols = (int) ((((fabs)
       (output->coord_corners[UL][0] - output->coord_corners[UR][0])) /
        output->output_pixel_size) + 0.5);

    /* reallocate based on window */
    if ( output->rowbuffer )
        free ( output->rowbuffer );

    /* new buffer, things may have changed */
    output->rowbuffer = ( void * ) calloc ( output->ncols, output->datasize );
    if ( !output->rowbuffer )
    {
        ErrorHandler( TRUE, "NoResampleHDF2RB", ERROR_MEMORY,
            "Error allocating memory for the output Row Buffer" );
    }

    /* allocate for the row buffer */
    buffer = ( double * ) calloc( output->ncols, sizeof( double ) );
    if ( buffer == NULL )
    {
        ErrorHandler( TRUE, "NoResampleHDF2RB", ERROR_MEMORY,
            "Error allocating space for the input row buffer" );
    }

    MessageHandler( "\nNoResample", "processing band %s",
        modis->bandinfo[input->bandnum].name );

    /* initialize status to terminal */
    fprintf( stdout, "%% complete (" MRT_SIZE_T_FMT " rows): 0%%",
             output->nrows );
    fflush( stdout );
    k = 0;

    /* set the value for the upper left x and y coordinates from the original
       coordinates in the file */
    upleft_x = modis->orig_coord_corners[UL][0];
    upleft_y = modis->orig_coord_corners[UL][1];

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

	/* since we're on a grid, get output northing once */
	outy = output->coord_corners[UL][1] - i * output->output_pixel_size;
	row = (int) (( upleft_y - outy ) / input->pixel_size + 0.5);

	/* loop through output cols */
	for ( j = 0; j < output->ncols; j++ )
	{
	    /* easting for this pixel */
	    outx = output->coord_corners[UL][0] +
                   j * output->output_pixel_size; 

	    /* get input row/col */
	    col = (int)(( outx - upleft_x ) / input->pixel_size + 0.5);

	    /* resample from input */
	    buffer[j] = ReadBufferValue( col, row, input );
	}

	/* write the resampled row to output */
        if ( !WriteRow( output, i, buffer ) )
        {
            ErrorHandler( FALSE, "NoResampleHDF2RB", ERROR_GENERAL,
                "Error writing the resampled row to the output file.");
            free( buffer );
            return( E_GEO_FAIL );
        }
    }

    fprintf( stdout, " 100%%\n" );
    fflush( stdout );

    free( buffer );

    return ( MRT_NO_ERROR );
}

