
/******************************************************************************

FILE:  tif_oc.c

PURPOSE:  Open and close TIFF files

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  Rob Burrell            Original Development

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/
#include "geotiffio.h"
#include "xtiffio.h"
#include "shared_resample.h"
#include "worgen.h"

/******************************************************************************

MODULE:  OpenGeoTIFFFile

PURPOSE:  Open a TIFF for writing

RETURN VALUE: FileDescriptor
Type = 
Value           Description
-----           -----------
file			Open TIFF file
NULL			Failure

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  Rob Burrell            Original Development
         01/01  John Rishea            Standardized formatting
         01/01  John Rishea            Removed test code
         07/01  Gail Schmidt           Changed the strpbrk to a strrchr to
                                       find the last occurrance of a '.' in
                                       the filename. Also found the end of
                                       the pathname to insure that the
                                       extension was being pulled from the
                                       filename and not the pathname.

NOTES:

******************************************************************************/
FileDescriptor *OpenGeoTIFFFile
(
    ModisDescriptor *modis,	/* I:  session info */
    FileOpenType mode,		/* I:  must be write */
    int bandnum,		/* I:  band number for file name */
    int *status			/* O:  error code */
)

{
    FileDescriptor *file = NULL;	/* file to write */
    char filename[LARGE_STRING], 	/* name construction strings */
         tempname[LARGE_STRING],        /* LARGE_STRING defined in shared_resample.h */ 
         *tmpptr = NULL,                /* pointer to the filename after
                                           finding the end of the path */
         *nameptr = NULL; 
    GeoTIFFFD *geotiff = NULL;		/* GEOTIFF file pointer */

    /* read is not supported */
    if ( mode == FILE_READ_MODE )
    {
	ErrorHandler( TRUE, "OpenGeoTIFFFile", ERROR_GENERAL,
	    "GeoTIFF input not supported" );
	*status = ERROR_GENERAL;
    }
    else
    {
	/* make a copy of the name and find the extension */
	strcpy( tempname, modis->output_filename );

        /* find the end of the path since some pathnames may have a
           '.' in them. check Unix/Linux then Windows. */
        tmpptr = strrchr( tempname, '/' );
        if ( tmpptr == NULL )
        {   /* check Windows */
            tmpptr = strrchr( tempname, '\\' );
            if ( tmpptr == NULL )
            {
                /* no pathname specified so just use the filename */
                tmpptr = tempname;
            }
        }

        /* find the extension */
        nameptr = strrchr( tmpptr, '.' );
        if ( nameptr != NULL )
            nameptr[0] = '\0';

	/* add the bandnumber/pixelsize to the name */
	/* extension should have the band name */
	sprintf( filename, "%s.%s.tif", tempname,
            modis->bandinfo[bandnum].name );

	/* replace white space with underscores in filename */
	SpaceToUnderscore( filename );

	/* create a new descriptor using the new name */
	file = CreateFileDescriptor( modis, bandnum, mode, filename );

	if ( file )
	{
	    /* allocate the geotiff pointer */
	    geotiff = ( GeoTIFFFD * ) calloc( 1, sizeof( GeoTIFFFD ) );

            /* if memory not allocated, error */
	    if ( !geotiff )
	    {
		DestroyFileDescriptor( file );
		ErrorHandler( TRUE, "OpenGeoTIFFFILE", ERROR_MEMORY,
		    "Allocating GeoTIFFFD" );
		return ( NULL );
	    }

	    /* open the file for writing */
	    geotiff->tif = XTIFFOpen( filename, "w" );
	   
            /* if memory not allocated, error */ 
            if ( !geotiff->tif )
	    {
		free( geotiff );
		DestroyFileDescriptor( file );
		sprintf( tempname, "Unable to open %s", filename );
		ErrorHandler( TRUE, "OpenGeoTIFFFILE", ERROR_OPEN_OUTPUTIMAGE,
		    tempname );
		return ( NULL );
	    }

	    /* set up initial GeoTIFF info */
	    geotiff->gtif = GTIFNew( geotiff->tif );
	    if ( !geotiff->gtif )
	    {
		XTIFFClose( geotiff->tif );
		free( geotiff );
		DestroyFileDescriptor( file );
		sprintf( tempname, "Unable to initialize GeoTIFF %s", filename );
		ErrorHandler( TRUE, "OpenGeoTIFFFILE", ERROR_OPEN_OUTPUTIMAGE,
		    tempname );
		return ( NULL );
	    }
	    file->fileptr = geotiff;
	}
    }
    return ( file );
}

/******************************************************************************

MODULE:  CloseGeoTIFFFile

PURPOSE:  Close a TIFF

RETURN VALUE:
Type = int
Value           Description
-----           -----------
TRUE			Success
FALSE			Failure

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  Rob Burrell            Original Development
         01/01  John Rishea            Standardized formatting

NOTES:

******************************************************************************/
int CloseGeoTIFFFile
(
    FileDescriptor *filedescriptor	/* file to close */
)

{
    GeoTIFFFD *geotiff = NULL;		/* cast pointer */

    switch ( filedescriptor->fileopentype )
    {
	case FILE_READ_MODE:
	    DestroyFileDescriptor( filedescriptor );
	    ErrorHandler( TRUE, "CloseGeoTIFFFile", ERROR_GENERAL,
		"Bad Open Type" );
	    return ( FALSE );

	case FILE_WRITE_MODE:
	    geotiff = ( GeoTIFFFD * ) filedescriptor->fileptr;
	    GTIFWriteKeys( geotiff->gtif );
	    GTIFFree( geotiff->gtif );
	    XTIFFClose( geotiff->tif );
	    DestroyFileDescriptor( filedescriptor );
	    break;

	default:
	    ErrorHandler( TRUE, "CloseGeoTIFFFile", ERROR_GENERAL,
		"Bad Open Type" );
	    return ( FALSE );
    }
    return ( TRUE );
}
