
/******************************************************************************

FILE:  fileoc.c

PURPOSE:  Open and close files depending on type

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  Rob Burrell            Original Development
         06/00  John Weiss             Support for input/output HDF-EOS files
         06/00  Rob Burrell            Support for GeoTIFF output files
         06/00  John Weiss             Remove multi-file specific routines
         01/01  John Rishea            Standardized formatting 
         01/01  John Rishea            Moved local prototypes to loc_prot.h

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/
#include "shared_mosaic.h"


/******************************************************************************

MODULE:  OpenInImage

PURPOSE:  Case for opening input images (multi-file and HDF-EOS)

RETURN VALUE:
Type = FileDescriptor
Value           Description
-----           -----------
file            A new file descriptor
NULL            Failure

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00   Rob Burrell            Original Development
         01/01   John Rishea            Standardized formatting

NOTES:

******************************************************************************/
FileDescriptor *OpenInImage
(
    ModisDescriptor *modis,	/* I:  session info */
    int bandnum,		/* I:  band number to open */
    int *status			/* O:  Error status */
)

{
    /* call specific input file opening routine */
    switch ( modis->input_filetype )
    {
	case RAW_BINARY:
	    return ( OpenMultiFile( modis, FILE_READ_MODE, bandnum, status ) );

	case HDFEOS:
	    return ( NULL );

	case GEOTIFF:
	    ErrorHandler( TRUE, "OpenInImage", ERROR_GENERAL,
		"GeoTIFF not supported for input" );
	    return ( NULL );

	default:
	    ErrorHandler( TRUE, "OpenInImage", ERROR_GENERAL,
		"Bad filetype passed" );
	    return ( NULL );
    }
}

/******************************************************************************

MODULE:  OpenInImageMosaic

PURPOSE:  Case for opening input images (multi-file and HDF-EOS) for
          mosaicking

RETURN VALUE:
Type = FileDescriptor
Value           Description
-----           -----------
file            A new file descriptor
NULL            Failure

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         07/02  Gail Schmidt           Original Development

NOTES:

******************************************************************************/
FileDescriptor *OpenInImageMosaic
(
    MosaicDescriptor *mosaic,   /* I:  session info */
    int in_bandnum,             /* I:  number of the input band */
    int out_bandnum,            /* I:  number of the output band */
    int *status                 /* O:  Error status */
)

{
    /* call specific input file opening routine */
    switch ( mosaic->filetype )
    {
        case RAW_BINARY:
            return ( OpenMultiFileMosaic( mosaic, FILE_READ_MODE, in_bandnum,
                out_bandnum, status ) );

        case HDFEOS:
            return ( NULL );

        default:
            ErrorHandler( TRUE, "OpenInImageMosaic", ERROR_GENERAL,
                "Bad filetype passed" );
            return ( NULL );
    }
}

/******************************************************************************

MODULE:  OpenOutImage

PURPOSE:  Case for opening output images (multi-file, HDF-EOS, and GeoTIFF)

RETURN VALUE:
Type = FileDescriptor
Value           Description
-----           -----------
file            A new file descriptor
NULL            Failure

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00   Rob Burrell            Original Development
         01/01   John Rishea            Standardized formatting

NOTES:

******************************************************************************/
FileDescriptor *OpenOutImage
(
    ModisDescriptor *modis,     /* I:  session info */
    int bandnum,                /* I:  band number to open */
    int *status                 /* O:  Error status */
)

{
    /* call specific output file opening routine */
    switch ( modis->output_filetype )
    {
        case RAW_BINARY:
            return ( OpenMultiFile( modis, FILE_WRITE_MODE, bandnum, status ) );

        case HDFEOS:
            return ( NULL );

        case GEOTIFF:
            return ( OpenGeoTIFFFile( modis, FILE_WRITE_MODE, bandnum,
                status ) );

        default:
            ErrorHandler( TRUE, "OpenOutImage", ERROR_GENERAL,
                "Bad filetype passed" );
            return ( NULL );
    }
}

/******************************************************************************

MODULE:  OpenOutImageMosaic

PURPOSE:  Case for opening output images (multi-file, HDF-EOS) for mosaicking.

RETURN VALUE:
Type = FileDescriptor
Value           Description
-----           -----------
file            A new file descriptor
NULL            Failure

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         07/02   Gail Schmidt           Original Development
 
NOTES:

******************************************************************************/
FileDescriptor *OpenOutImageMosaic
(
    MosaicDescriptor *mosaic,	/* I:  session info */
    int in_bandnum,             /* I:  number of the input band */
    int out_bandnum,            /* I:  number of the output band */
    int *status			/* O:  Error status */
)

{
    /* call specific output file opening routine */
    switch ( mosaic->filetype )
    {
	case RAW_BINARY:
	    return ( OpenMultiFileMosaic( mosaic, FILE_WRITE_MODE, in_bandnum,
                out_bandnum, status ) );

	case HDFEOS:
	    return ( NULL );

	default:
	    ErrorHandler( TRUE, "OpenOutImageMosaic", ERROR_GENERAL,
		"Bad filetype passed" );
	    return ( NULL );
    }
}

/******************************************************************************

MODULE:  CloseFile

PURPOSE:  Case for closing files (multi-file, HDF-EOS, and GeoTIFF)

RETURN VALUE:
Type = int
Value           Description
-----           -----------
TRUE            Success
FALSE           Failure

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00   Rob Burrell            Original Development
         01/01   John Rishea            Standardized formatting

NOTES:

******************************************************************************/
int CloseFile
(
    FileDescriptor *filedescriptor      /* I:  file to close */
)

{
    /* which type of file to close */
    switch ( filedescriptor->filetype )
    {
        case RAW_BINARY:
            return ( CloseMultiFile( filedescriptor ) );

        case HDFEOS:
            return ( CloseHdfEosFile( filedescriptor ) );

        case GEOTIFF:
            return ( CloseGeoTIFFFile( filedescriptor ) );

        default:
            ErrorHandler( TRUE, "CloseFile", ERROR_GENERAL,
                "Bad filetype passed" );
            return ( FALSE );
    }
}
