
/******************************************************************************

FILE:  rb_oc.c

PURPOSE:  Open and close raw binary files

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  Rob Burrell            Original Development
         06/00  John Weiss             support for input/output HDF-EOS files
         06/00  Rob Burrell            support for output GeoTIFF files

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/
#include "shared_mosaic.h"
#include <errno.h>

/******************************************************************************

MODULE:  OpenMultiFile

PURPOSE:  Open a multifile file for reading or writing

RETURN VALUE:
Type = FileDescriptor
Value           Description
-----           -----------
file            Success
NULL            Failure

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00   Rob Burrell            Original Development
         01/01   John Rishea            Standardized formatting
	 07/01   Gail Schmidt           Changed the strpbrk to a strrchr to
                                        find the last occurrance of a '.' in
                                        the filename. Also found the end of
                                        the pathname to insure that the
                                        extension was being pulled from the
                                        filename and not the pathname.

NOTES:

******************************************************************************/
FileDescriptor *OpenMultiFile
(
    ModisDescriptor *modis,	/* I:  session info */
    FileOpenType mode,		/* I:  reading or writing */
    int bandnum,		/* I:  band number to read or write */
    int *status			/* O:  error status */
)

{
    FileDescriptor *file = NULL;	/* new file descriptor */
    char filename[LARGE_STRING],	/* the name we must open */
         tempname[LARGE_STRING],	/* temp storage for a name */
                                        /* LARGE_STRING defined in shared_resample.h*/ 
         *tmpptr = NULL,                /* pointer to the filename after
                                           finding the end of the path */
         *nameptr = NULL;		/* strpbrk pointer */

    /* open file for reading */
    if ( mode == FILE_READ_MODE )
    {
	/* get a copy since we're going to modify */
	strcpy( tempname, modis->input_filename );

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

	/* extension should have the band name */
	sprintf( filename, "%s.%s.dat", tempname,
            modis->bandinfo[bandnum].name );

	/* replace white space with underscores in filename */
	SpaceToUnderscore( filename );

	/* create a descriptor using this band */
	file = CreateFileDescriptor( modis, bandnum, mode, filename );
	if ( file )
	{
	    /* open the file */
            errno = 0;
	    file->fileptr = fopen( filename, "rb" );
	    if ( !file->fileptr )
	    {
		sprintf( tempname, "Unable to open %s: %s\n", filename,
                      strerror(errno) );
		ErrorHandler( TRUE, "OpenMultiFile", ERROR_OPEN_INPUTIMAGE,
		    tempname );
		*status = ERROR_OPEN_INPUTIMAGE;
		DestroyFileDescriptor( file );
		file = NULL;
	    }
	}
    }

    else			/* open for writing */
    {
	/* get a copy since we're going to modify */
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
	    nameptr[0] = '\0';	/* find the extension */

	/* create a new name with the band name */
	sprintf( filename, "%s.%s.dat", tempname,
            modis->bandinfo[bandnum].name );

	/* replace white space with underscores in filename */
	SpaceToUnderscore( filename );

	/* create a descriptor using this band */
	file = CreateFileDescriptor( modis, bandnum, mode, filename );
	if ( file )
	{
	    /* open the file */
            errno = 0;
	    file->fileptr = fopen( filename, "wb" );
	    if ( !file->fileptr )
	    {
		sprintf( tempname, "Unable to open %s: %s", filename,
                         strerror(errno) );
		ErrorHandler( TRUE, "OpenMultiFile", ERROR_OPEN_OUTPUTIMAGE,
		    tempname );
		*status = ERROR_OPEN_OUTPUTIMAGE;
		DestroyFileDescriptor( file );
		file = NULL;
	    }
	}
    }

    return ( file );
}

/******************************************************************************

MODULE:  OpenMultiFileMosaic

PURPOSE:  Open a multifile file for reading or writing for mosaicking

RETURN VALUE:
Type = FileDescriptor
Value           Description
-----           -----------
file            Success
NULL            Failure

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         07/02   Gail Schmidt           Original Development

NOTES:

******************************************************************************/
FileDescriptor *OpenMultiFileMosaic
(
    MosaicDescriptor *mosaic,	/* I:  session info */
    FileOpenType mode,		/* I:  reading or writing */
    int in_bandnum,             /* I:  number of the input band */
    int out_bandnum,            /* I:  number of the output band */
    int *status			/* O:  error status */
)

{
    FileDescriptor *file = NULL;	/* new file descriptor */
    char filename[LARGE_STRING],	/* the name we must open */
         tempname[LARGE_STRING],	/* temp storage for a name */
                                        /* LARGE_STRING defined in shared_resample.h*/ 
         *tmpptr = NULL,                /* pointer to the filename after
                                           finding the end of the path */
         *nameptr = NULL;		/* strpbrk pointer */

    /* open file for reading */
    if ( mode == FILE_READ_MODE )
    {
	/* get a copy since we're going to modify */
	strcpy( tempname, mosaic->filename );

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

	/* extension should have the band name */
	sprintf( filename, "%s.%s.dat", tempname,
            mosaic->bandinfo[in_bandnum].name );

	/* replace white space with underscores in filename */
	SpaceToUnderscore( filename );

	/* create a descriptor using this band */
	file = CreateFileDescriptorMosaic( mosaic, in_bandnum, out_bandnum,
            mode, filename );
	if ( file )
	{
	    /* open the file */
            errno = 0;
	    file->fileptr = fopen( filename, "rb" );
	    if ( !file->fileptr )
	    {
		sprintf( tempname, "Unable to open %s: %s\n", filename,
                     strerror(errno) );
		ErrorHandler( TRUE, "OpenMultiFileMosaic",
                    ERROR_OPEN_INPUTIMAGE, tempname );
		*status = ERROR_OPEN_INPUTIMAGE;
		DestroyFileDescriptor( file );
		file = NULL;
	    }
	}
    }

    else			/* open for writing */
    {
	/* get a copy since we're going to modify */
	strcpy( tempname, mosaic->filename );

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
	    nameptr[0] = '\0';	/* find the extension */

	/* create a new name with the band name */
	sprintf( filename, "%s.%s.dat", tempname,
            mosaic->bandinfo[in_bandnum].name );

	/* replace white space with underscores in filename */
	SpaceToUnderscore( filename );

	/* create a descriptor using this band */
	file = CreateFileDescriptorMosaic( mosaic, in_bandnum, out_bandnum,
            mode, filename );
	if ( file )
	{
	    /* open the file */
            errno = 0;
	    file->fileptr = fopen( filename, "wb" );
	    if ( !file->fileptr )
	    {
		sprintf( tempname, "Unable to open %s: %s", filename,
                      strerror(errno) );
		ErrorHandler( TRUE, "OpenMultiFileMosaic",
                    ERROR_OPEN_OUTPUTIMAGE, tempname );
		*status = ERROR_OPEN_OUTPUTIMAGE;
		DestroyFileDescriptor( file );
		file = NULL;
	    }
	}
    }

    return ( file );
}

/******************************************************************************

MODULE:  CloseMultiFile

PURPOSE:  Closes a multifile file

RETURN VALUE:
Type = int
Value           Description
-----           -----------
TRUE            Always

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00   Rob Burrell            Original Development
         01/01   John Rishea            Standardized formatting
         01/01   John Rishea            Combined cases in switch stmt 
NOTES:

******************************************************************************/
int CloseMultiFile
(
    FileDescriptor *filedescriptor	/* I:  the file to close */
)

{
    /* nice to have a case if we eventually use write buffers
     * since we'll need to flush them before closing */
    switch ( filedescriptor->fileopentype )
    {
	case FILE_READ_MODE:

	case FILE_WRITE_MODE:
	    fclose( ( FILE * ) filedescriptor->fileptr );
	    DestroyFileDescriptor( filedescriptor );
	    break;

	default:
	    ErrorHandler( TRUE, "CloseMultiFile", ERROR_GENERAL,
                "Bad Open Type" );
	    return ( FALSE );
    }

    return ( TRUE );
}
