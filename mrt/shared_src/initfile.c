/******************************************************************************

FILE:  initfile.c

PURPOSE:  Initializes GeoTIFF and HDFEOS output files with the 
          projection information.

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  Rob Burrell            Original Development
         01/01  John Rishea            Standardized formatting
         01/01  John Rishea            Moved local prototypes to loc_prot.h

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/

#include "shared_resample.h"


/******************************************************************************

MODULE:  InitOutputFile

PURPOSE:  Initializes GeoTIFF and HDFEos output files with the
  projection information.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status          See mrt_error.h for a complete list of error codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00   Rob Burrell            Original Development
         01/01   John Rishea            Standardized formatting

NOTES:

******************************************************************************/
int InitOutputFile
(
    FileDescriptor *input,	/* I:  need this for HDF band names */
    FileDescriptor *output,	/* I/O:  the output file to initialize */
    ModisDescriptor *modis	/* I:  session info */
)

{
    int status = MRT_NO_ERROR;	/* error return code */

    switch ( output->filetype )
    {
	case RAW_BINARY:
	    /* no init for multi, write the header later */
	    return ( MRT_NO_ERROR );

	case HDFEOS:
	    if ( CreateHdfEosField( input, output, modis ) < 0 )
		status = ERROR_OPEN_OUTPUTIMAGE;
	    return status;

	case GEOTIFF:
	    return ( SetTIFFTags( output, modis ) );

	default:
	    ErrorHandler( TRUE, "InitOutputFile", ERROR_GENERAL,
                "Bad File Type" );
	    break;
    }

    return ( ERROR_GENERAL );
}
