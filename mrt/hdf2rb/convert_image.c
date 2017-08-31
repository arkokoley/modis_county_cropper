#include "hdf2rb.h"

/******************************************************************************

MODULE:  ConvertImageHDF2RB

PURPOSE:  Do the format conversion for HDF to raw binary

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status		See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         11/05   Gail Schmidt          Original Development

NOTES:

******************************************************************************/
int ConvertImageHDF2RB
(
    ModisDescriptor *modis	/* I:  session info */
)

{
    size_t inband = 0;		/* loop index for bands */
    int inmulti_band = 0;	/* index for band num in the input image */
    int outmulti_band = 0;	/* index for band num in the output image */
    double last_resolution = 0.0;  /* determines when to increment
                                      inmulti_band */
    int status = MRT_NO_ERROR, stat;   /* loop breaker for errors */
    FileDescriptor *input = NULL, *output = NULL;
    HdfEosFD *input_hdfptr = NULL;

    /* first, setup the output band info so we can fill things in as we go */
    /* no output files written yet, this counter is for raw binary headers */
    modis->nfiles_out = 0;

    /* worst case is nbands with 1 per file */
    modis->output_file_info =
	( OutFileType * ) calloc( modis->nbands, sizeof( OutFileType ) );
    if ( !modis->output_file_info )
	ErrorHandler( TRUE, "ConvertImageHDF2RB", ERROR_MEMORY,
            "Creating output_file_info" );

    /* HDF-EOS files get opened just once and stay open */
    if ( modis->input_filetype == HDFEOS )
	input_hdfptr = OpenHdfEosFile( modis->input_filename,
            modis->output_filename, FILE_READ_MODE, &status );

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

	/* if band is not selected, continue to next band */
	if ( !modis->bandinfo[inband].selected )
	    continue;

	/* open input file/grid/band/field */
	switch ( modis->input_filetype )
	{
	    case HDFEOS:
		/* create a file descriptor */
		input = MakeHdfEosFD( modis, input_hdfptr, FILE_READ_MODE,
                    inband, &status );
	        if ( !input || status != MRT_NO_ERROR )
        	{
        	    ErrorHandler( TRUE, "ConvertImageHDF2RB",
                        ERROR_OPEN_INPUTIMAGE, "problem opening input file" );
        	}

		/* set the correct band to read */
		GetHdfEosField( modis, input_hdfptr, inband );

		break;

	    default:
		ErrorHandler( TRUE, "ConvertImageHDF2RB", ERROR_GENERAL,
		    "Bad File Type" );
		status = ERROR_GENERAL;
		break;
	}

	/* now open corresponding output file/grid/band/field */
	switch ( modis->output_filetype )
	{
	    case RAW_BINARY:
		output = OpenOutImage( modis, inband, &status );
		outmulti_band++;
		output->bandnum = outmulti_band;
		break;

	    default:
		ErrorHandler( TRUE, "ConvertImageHDF2RB", ERROR_GENERAL,
		    "Bad File Type" );
		status = ERROR_GENERAL;
		break;
	}

	if ( status != MRT_NO_ERROR )
	{
	    ErrorHandler( TRUE, "ConvertImageHDF2RB", ERROR_OPEN_OUTPUTIMAGE,
		"problem opening output file" );
	}

	/* format convert band/field */
	status = NoResampleHDF2RB( modis, input, output );
        if ( status != MRT_NO_ERROR )
        {
            ErrorHandler( TRUE, "ConvertImageHDF2RB", ERROR_GENERAL,
                "Error occurred in the resample process" );
        }

	/* close input file */
	switch ( modis->input_filetype )
	{
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

    return ( status );
}

