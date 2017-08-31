#include <time.h>		/* clock()   prototype */
#include "hdf2rb.h"
#include "shared_resample.h"

/******************************************************************************

MODULE:  hdf2rb

PURPOSE:  Convert an input HDF (MODIS land gridded product) to raw binary,
    allowing the user to do spatial or spectral subsetting.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status          See mrt_error.h for a complete list of error codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         11/05  Gail Schmidt           Original Development
 
NOTES:
   This software is built mainly for the users that want to do a simple
   conversion from HDF to raw binary.  The MRT does format conversions, but
   the bounding ISIN/SIN tiles are a problem due to the major reliance on
   lat/long for the MRT.  This tool is provided as a quick and dirty
   work-around to allow users to do a simple format conversion rather than
   rewriting the MRT to remove the reliance on lat/long throughout the
   software.  A large amount of the existing MRT software is being used for
   this tool.  It would be cleaner to write the tool from scratch, but time
   does not allow for that at this point.

******************************************************************************/

int main
(
    int argc,
    char *argv[]
)

{
    int errval;	                    /* error status */
    ModisDescriptor *modis = NULL;  /* session info */
    char str[SMALL_STRING];         /* misc string handling */
                                    /* SMALL_STRING defined in resample.h */
    time_t startdate, enddate;      /* start and end date struct */

    /* check usage */
    if ( argc < 3 )
    {
        HDF2RB_Usage();
        return ERROR_GENERAL;
    }

    /* set up a log file and process the -g command line option if it exists */
    InitLogHandler( argc, argv );

    /* print a log file header */
    MessageHandler( NULL,
       "*******************************************************************"
       "***********\n");
    MessageHandler( NULL, "%s (%s)", HDF2RB_NAME, HDF2RB_VERSION );
    startdate = time( NULL );
    MessageHandler( NULL, "Start Time:  %s", ctime( &startdate ) );
    MessageHandler( NULL,
    "------------------------------------------------------------------\n" );
    fflush( stdout );

    /* allocate Modis descriptor */
    modis = calloc( 1, sizeof( ModisDescriptor ) );
    if ( modis == NULL)
    {
        errval = ERROR_MEMORY;   /* assign memory error to errval */
        sprintf( str, "Unable to allocate ModisDescriptor memory (errval = %i)",
            errval ); 
        ErrorHandler( TRUE, "hdf2rb", errval, str );
    }

    /* initialize ModisDescriptor with default values */
    InitializeModisDescriptor( modis );

    /* read command-line arguments */
    errval = ProcessArgumentsHDF2RB( argc, argv, modis );
    if ( errval != MRT_NO_ERROR )
    {
        PrintModisDescriptor( modis );

        /* free dynamically allocated memory */
        free ( modis );

        /* now display error message and call ErrorHandler */
        sprintf( str, "Parameter file error (errval = %i)", errval );
        ErrorHandler( TRUE, "hdf2rb", errval, str );
    }

    /* read parameter file (don't override command-line args) and check for
       errors */
    errval = ReadParameterFileHDF2RB( modis );
    if ( errval != MRT_NO_ERROR )
    {
        PrintModisDescriptorHDF2RB( modis );

        /* free dynamically allocated memory */
        free ( modis );

        /* now display error message and call ErrorHandler */
        sprintf( str, "Parameter file error (errval = %i)", errval );
        ErrorHandler( TRUE, "hdf2rb", errval, str );
    }

    /* let's see what we've accomplished */
    PrintModisDescriptorHDF2RB( modis );

    /* do the format conversion */
    errval = ConvertImageHDF2RB( modis );
    if ( errval != MRT_NO_ERROR )
    {
        /* free dynamically allocated memory */
        free ( modis );

        /* now display error message and call ErrorHandler */
	sprintf( str, "Resampler error (errval = %i)", errval );
	ErrorHandler( TRUE, "hdf2rb", errval, str );
    }

    /* write output header file for raw binary format */
    WriteHeaderFileHDF2RB( modis );

    /* dump useful output file info */
    PrintOutputFileInfoHDF2RB( modis );

    /* stop timer and print elapsed time */
    enddate = time( NULL );
    MessageHandler( NULL, "End Time:  %s", ctime( &enddate ) );
    MessageHandler( NULL, "Finished processing!\n" );
    MessageHandler( NULL, "******************************************************************************\n");
    CloseLogHandler();

    /* indicate successful completion of processing */
    return errval;
}

