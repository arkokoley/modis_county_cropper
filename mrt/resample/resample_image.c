#include "resample.h"

/******************************************************************************

MODULE:  ResampleImage

PURPOSE:  Manage selection of resampling files and methods

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status		See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00   Rob Burrell           Original Development
         01/01   John Rishea           Standardized formatting
         01/01   John Rishea           Removed test code
         05/01   John Weiss            Removed HDF SDS code
         07/01   Gail Schmidt          Check error status returned from
                                       the resampler

NOTES:

******************************************************************************/
int ResampleImage
(
    ModisDescriptor *modis	/* I:  session info */
)

{
    size_t inband = 0;		/* loop index for bands */
    size_t curband = 0;		/* loop index for bands */
    int inmulti_band = 0;	/* index for band num in the input image */
    int outmulti_band = 0;	/* index for band num in the output image */
    double curr_resolution = 0.0;  /* determines when to open a new input
                                      file */
    double last_resolution = 0.0;  /* determines when to increment
                                      inmulti_band */
    int status = MRT_NO_ERROR, stat;   /* loop breaker for errors */
    int change_resolution;	/* determines when to open a new output file */
    FileDescriptor *input = NULL, *output = NULL;
    HdfEosFD *input_hdfptr = NULL, *output_hdfptr = NULL;
    double temp_parms[15];      /* temporary projection parameters */
    int last_band = TRUE;       /* are we processing the last band? */

    /* first, setup the output band info so we can fill things in as we go */
    /* no output files written yet, this counter is for multi file headers */
    modis->nfiles_out = 0;

    /* worst case is nbands with 1 per file */
    modis->output_file_info =
	( OutFileType * ) calloc( modis->nbands, sizeof( OutFileType ) );
    if ( !modis->output_file_info )
	ErrorHandler( TRUE, "ResampleImage", ERROR_MEMORY,
            "Creating output_file_info" );

    /* HDF-EOS files get opened just once and stay open */
    if ( modis->input_filetype == HDFEOS )
	input_hdfptr = OpenHdfEosFile( modis->input_filename,
            modis->output_filename, FILE_READ_MODE, &status );

    if ( modis->output_filetype == HDFEOS )
	output_hdfptr = OpenHdfEosFile( modis->input_filename,
            modis->output_filename, FILE_WRITE_MODE, &status );

    /* print out the projection information if this is not a datum
       conversion, otherwise geolib prints the information for datum
       conversion processing. */
    if ( modis->output_datum_code == E_NODATUM &&
         modis->resampling_type != NO_RESAMPLE )
    {
        /* print the input projection information */
        memcpy( temp_parms, modis->in_projection_info->proj_coef,
            15 * sizeof( double ) );
        print_proj( modis->in_projection_info->proj_code,
            modis->in_projection_info->zone_code,
            temp_parms );

        /* print the output projection information */
        MessageHandler(NULL, "");  /* print white-line between */
        memcpy( temp_parms, modis->out_projection_info->proj_coef,
            15 * sizeof( double ) );
        print_proj( modis->out_projection_info->proj_code,
            modis->out_projection_info->zone_code,
            temp_parms );
    }

    /* loop through all the image bands */
    for ( inband = 0; inband < modis->nbands && status == MRT_NO_ERROR;
	  inband++, inmulti_band++ )
    {
	/* Keep track of the band number in the current input image.
	 * Changing bands implies a new input file for
	 * multifile input, and new grid for HDF-EOS. */
	if ( last_resolution != modis->bandinfo[inband].pixel_size )
	{
	    inmulti_band = 0;
	    last_resolution = modis->bandinfo[inband].pixel_size;
	}

	/* always start by assuming no change in resolution */
	change_resolution = FALSE;

	/* if band is not selected, continue to next band */
	if ( !modis->bandinfo[inband].selected )
	    continue;

	/* open input file/grid/band/field */
	switch ( modis->input_filetype )
	{
	    case RAW_BINARY:
		/* open input file */
		input = OpenInImage( modis, inband, &status );
		if ( !input )
		    return ( status );

		/* track rez changes for HDF-EOS grids */
		if ( curr_resolution != modis->bandinfo[inband].pixel_size )
		{
		    curr_resolution = modis->bandinfo[inband].pixel_size;
		    change_resolution = TRUE;
		}

		/* clear buffers to avoid reading data from previous band */
		ClobberFileBuffers( input );

		break;

	    case HDFEOS:
		/* create a file descriptor */
		input = MakeHdfEosFD( modis, input_hdfptr, FILE_READ_MODE,
                    inband, &status );
		if ( !input )
		    return ( status );
		fflush (stdout);

		/* still at the same res? */
		if ( curr_resolution != modis->bandinfo[inband].pixel_size )
		{
		    /* no, open another grid */
		    change_resolution = TRUE;
		}

		/* set the correct band to read */
		curr_resolution = modis->bandinfo[inband].pixel_size;
		GetHdfEosField( modis, input_hdfptr, inband );

		break;

	    default:
		ErrorHandler( TRUE, "ResampleImage", ERROR_GENERAL,
		    "Bad File Type" );
		status = ERROR_GENERAL;
		break;
	}

	if ( status != MRT_NO_ERROR )
	{
	    ErrorHandler( TRUE, "ResampleImage", ERROR_OPEN_INPUTIMAGE,
		"problem opening input file" );
	}

	/* now open corresponding output file/grid/band/field */
	switch ( modis->output_filetype )
	{
	    case RAW_BINARY:
		output = OpenOutImage( modis, inband, &status );
		outmulti_band++;
		output->bandnum = outmulti_band;
		break;

	    case HDFEOS:
		output = MakeHdfEosFD( modis, output_hdfptr, FILE_WRITE_MODE,
		    inband, &status );

		if ( change_resolution )
		{
		    /* open new grid (old grid is closed if necessary) */
		    output->bandnum = outmulti_band = 1;
                    if ( CreateHdfEosGrid( input, output, modis ) != 0 )
			    status = ERROR_OPEN_OUTPUTIMAGE;
		}
		else
		{
		    outmulti_band++;
		    output->bandnum = outmulti_band;
		}
		break;

	    case GEOTIFF:
		output = OpenOutImage( modis, inband, &status );
		break;

	    default:
		ErrorHandler( TRUE, "ResampleImage", ERROR_GENERAL,
		    "Bad File Type" );
		status = ERROR_GENERAL;
		break;
	}

	if ( status != MRT_NO_ERROR )
	{
	    ErrorHandler( TRUE, "ResampleImage", ERROR_OPEN_OUTPUTIMAGE,
		"problem opening output file" );
	}

        /* is this the last band?  loop through the rest of the bands to
           see if there are any other bands selected.  if this is the last
           band then let the resampling processes know so that they can
           clean up the ISIN shift buffers. */
        last_band = TRUE;
        for (curband = inband+1; curband < modis->nbands; curband++ )
        {
            if ( modis->bandinfo[curband].selected )
            {
                /* if we find a band that is selected then break out of the
                   loop */
                last_band = FALSE;
                break;
            }
        }

	/* reproject band/field */
	switch ( modis->resampling_type )
	{
	    case NN:
		status = NNResample( modis, input, output, last_band );
		break;

	    case BI:
		status = BIResample( modis, input, output, last_band );
		break;

	    case CC:
		status = CCResample( modis, input, output, last_band );
		break;

	    case NO_RESAMPLE:
		status = NoResample( modis, input, output );
		break;

	    default:
		ErrorHandler( TRUE, "ResampleImage", ERROR_GENERAL,
		    "Bad Resample Type" );
		status = ERROR_GENERAL;
		break;
	}

        if ( status != MRT_NO_ERROR )
        {
            ErrorHandler( TRUE, "ResampleImage", ERROR_GENERAL,
                "Error occurred in the resample process" );
        }

	/* close input file */
	switch ( modis->input_filetype )
	{
	    case RAW_BINARY:
		CloseFile( input );
		break;

	    case HDFEOS:
		DestroyFileDescriptor( input );
		break;
	}
	input = NULL;

	/* store output file info, for updating screen and log file, and
           writing RB headers */
	modis->output_file_info[modis->nfiles_out].nlines = output->nrows;
	modis->output_file_info[modis->nfiles_out].nsamples = output->ncols;
	modis->output_file_info[modis->nfiles_out].pixel_size =
            output->output_pixel_size;
	modis->output_file_info[modis->nfiles_out].nbands = outmulti_band;
	modis->nfiles_out++;

	/* close raw binary and GeoTiff output files */
	switch ( modis->output_filetype )
	{
	    case RAW_BINARY:
		CloseFile( output );
		break;

	    case HDFEOS:
		DestroyFileDescriptor( output );
		break;

	    case GEOTIFF:
		CloseFile( output );
		break;
	}
	output = NULL;
    }

    /* close input HDF-EOS file */
    switch ( modis->input_filetype )
    {
	case HDFEOS:
	    if ( !input )
		input = MakeHdfEosFD( modis, input_hdfptr, FILE_READ_MODE, 0,
                    &stat );
	    CloseFile( input );
	    break;
    }

    /* close output HDF-EOS file */
    switch ( modis->output_filetype )
    {
	case HDFEOS:
	    if ( !output )
		output = MakeHdfEosFD( modis, output_hdfptr, FILE_WRITE_MODE,
                    0, &stat );
	    CloseFile( output );
            break;
    }

    return ( status );
}
