#include <time.h>		/* clock()   prototype */
#include "resample.h"

/******************************************************************************

MODULE:  main

PURPOSE:  Program entry point

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status          See mrt_error.h for a complete list of error codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  John Weiss             Original Development
         05/00  John Weiss             Merge code from rlb
         01/01  John Rishea            Standardized formatting
         01/01  John Rishea            Added error check for unsuccessful
                                       dynamic memory allocation
         02/01  John Weiss             Add JR's metadata function call.
         06/01  Gail Schmidt           Allow the resample.log file to be
                                       specified via a command-line option
         01/02  Gail Schmidt           Read the command-line parameters before
                                       reading the parameter file and
                                       processing the arguments.
 
NOTES:

******************************************************************************/

int main
(
    int argc,
    char *argv[]
)

{
    int errval;			/* error status */
    int defpixsiz = 0;          /* hack for processing command-line args */
                                /* flag to tell if using default output pixel
                                   size */
    ModisDescriptor *modis = NULL;  /* session info */
    char str[LARGE_STRING+1];   /* misc string handling */
                                /* SMALL_STRING defined in resample.h */
    time_t startdate, enddate;	/* start and end date struct */

    /* check usage */
    if ( argc < 3 )
    {
	Usage();
	return ERROR_GENERAL;
    }

    /* set up a log file and process the -g command line option if it exists.
       the -g option should be processed before Hdf2Hdr is called or any
       other processing takes place. */
    InitLogHandler( argc, argv );

    /* see if we want to call Hdf2Hdr() */
    if ( argc == 3 && strcmp( argv[1], "-h" ) == 0 )
    {
	errval = Hdf2Hdr( argv[2] );
	CloseLogHandler();
	return errval;
    }

    /* print a log file header */
    MessageHandler( NULL,
       "*******************************************************************"
       "***********\n");
    MessageHandler( NULL, "%s (%s)", RESAMPLER_NAME, RESAMPLER_VERSION );
    startdate = time( NULL );
    MessageHandler( NULL, "Start Time:  %s", ctime( &startdate ) );
    MessageHandler( NULL,
    "------------------------------------------------------------------\n" );
    fflush( stdout );

    /* process command-line arguments (to get parameter filename) */
    errval = GetParameterFilename( argc, argv, str, LARGE_STRING );

    /* fatal error handling */
    if ( errval != MRT_NO_ERROR )
    {
	sprintf( str, "No parameter filename (errval = %i)", errval );
	ErrorHandler( TRUE, "main", errval, str );
    }

    /* allocate Modis descriptor */
    modis = calloc( 1, sizeof( ModisDescriptor ) );

    /* fatal error if memory for ModisDescriptor not successfully allocated */ 
    if ( modis == NULL)
    {
        errval = ERROR_MEMORY;   /* assign memory error to errval */
        sprintf( str, "Unable to allocate ModisDescriptor memory (errval = %i)",
            errval ); 
        ErrorHandler( TRUE, "main", errval, str );
    }

    /* initialize ModisDescriptor with default values */
    InitializeModisDescriptor( modis );
    modis->parameter_filename = strdup( str );

    /* if parameter_filename memory can't be allocated, fatal error */
    if ( modis->parameter_filename == NULL )
    {
        /* first free dynamically allocated memory */
        free ( modis );

        /* now display error msg and call ErrorHandler */
	sprintf( str,
            "Could not allocate strdup memory for parameter filename." );
	ErrorHandler( TRUE, "main", ERROR_MEMORY, str );
    }

    /* check for "-f" command-line argument (format conversion) */
    CheckFormatConversion( argc, argv, modis );

    /* read command-line arguments */
    errval = ProcessArguments( argc, argv, modis );

    /* read parameter file (don't override command-line args) and
       check for errors */
    errval = ReadParameterFile( modis, &defpixsiz );

    /* fatal error handling */
    if ( errval != MRT_NO_ERROR )
    {
        PrintModisDescriptor( modis );

        /* free dynamically allocated memory */
        free ( modis );

        /* now display error message and call ErrorHandler */
	sprintf( str, "Parameter file error (errval = %i)", errval );
	ErrorHandler( TRUE, "main()", errval, str );
    }

    /* let's see what we've accomplished */
    PrintModisDescriptor( modis );

    /* check validity of projection parameters and set up the
       in_projection_info and out_projection_info structures for modis */
    errval = CheckProjectionParams( modis );
    fflush( stdout );

    /* fatal error handling */
    /* change this to handle the return code properly */
    if ( errval != MRT_NO_ERROR )
    {
        /* free dynamically allocated memory */
        free ( modis );
 
        /* now display error message and call ErrorHandler */
	sprintf( str, "Projection parameter error (errval = %i)", errval );
	ErrorHandler( TRUE, "main()", errval, str );
    }

    /* invoke the resampler */
    fflush( stdout );
    errval = ResampleImage( modis );

    /* fatal error handling */
    if ( errval != MRT_NO_ERROR )
    {
        /* free dynamically allocated memory */
        free ( modis );

        /* now display error message and call ErrorHandler */
	sprintf( str, "Resampler error (errval = %i)", errval );
	ErrorHandler( TRUE, "main()", errval, str );
    }

    /* write header file (multifile format) */
    if ( modis->output_filetype == RAW_BINARY )
        WriteHeaderFile( modis );

    /* dump useful output file info */
    PrintOutputFileInfo( modis );

    /* stop timer and print elapsed time */
    enddate = time( NULL );
    MessageHandler( NULL, "End Time:  %s", ctime( &enddate ) );
    MessageHandler( NULL, "Finished processing!\n" );
    MessageHandler( NULL, "******************************************************************************\n");
    CloseLogHandler();

    /* if outputting to HDF-EOS, add metadata (if processing HDF-EOS input)
       and attributes */
    if ( modis->output_filetype == HDFEOS )
    {
        errval = AppendMetadata( modis );
        if ( errval != MRT_NO_ERROR )
        {
            /* now display error message and call ErrorHandler */
            sprintf( str, "AppendMetadata error (errval = %i)", errval );
            ErrorHandler( TRUE, "main()", errval, str );
        }
    }

    /* indicate successful completion of processing */
    return errval;
}

