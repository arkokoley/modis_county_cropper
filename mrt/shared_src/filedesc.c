/******************************************************************************

FILE:  filedesc.c

PURPOSE:  Creates and destroys file descriptors

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  Rob Burrell            Original Development
         01/01  John Rishea            Moved local prototype to loc_prot.h

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/
#include "shared_mosaic.h"

/******************************************************************************

MODULE:  CreateFileDescriptor

PURPOSE:  Creates a file descriptor using information from the MODIS descriptor

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

NOTES:

******************************************************************************/
FileDescriptor *CreateFileDescriptor
(
    ModisDescriptor *modis,	/* I:  session info */
    int bandnum,		/* I:  the number of the band */
    FileOpenType fileopentype,	/* I:  reading or writing */
    char *filename		/* I:  the file name */
)

{
    FileDescriptor *file = NULL;	/* return value, new descriptor */

    /* allocate a descriptor */
    file = ( FileDescriptor * ) calloc( 1, sizeof( FileDescriptor ) );

    if ( !file )
	ErrorHandler( TRUE, "CreateFileDescriptor", ERROR_MEMORY,
            "File Descriptor" );

    /* copy the file name */
    file->filename = strdup( filename );
    if ( !file->filename )
	ErrorHandler( TRUE, "CreateFileDescriptor", ERROR_MEMORY, "File Name" );

    /* get the info from the modis file */
    file->fileopentype = fileopentype;

    /* input file */
    if ( fileopentype == FILE_READ_MODE )
    {
	file->filetype = modis->input_filetype;
	file->datatype = modis->bandinfo[bandnum].input_datatype;
        file->file_endianness = modis->input_file_endian;
    }
    else  /* output file */
    {
	file->filetype = modis->output_filetype;
	file->datatype = modis->bandinfo[bandnum].output_datatype;
    }

    /* calculate the size of the data needed for a pixel */
    file->datasize = DFKNTsize( file->datatype );

    /* get the size for this band, the band number, and the size of a pixel */
    file->nrows = modis->bandinfo[bandnum].nlines;
    file->ncols = modis->bandinfo[bandnum].nsamples;
    file->bandnum = bandnum;
    file->pixel_size = modis->bandinfo[bandnum].pixel_size;
    file->output_pixel_size = modis->bandinfo[bandnum].output_pixel_size;
    file->background_fill = modis->bandinfo[bandnum].background_fill;

    /* create an internal read/write buffer for this descriptor */
    file->rowbuffer = ( void * ) calloc( file->ncols, file->datasize );
    if ( !file->rowbuffer )
	ErrorHandler( TRUE, "CreateFileDescriptor", ERROR_MEMORY, "Row Buffer" );

    /* if we're reading, create the read buffers */
    if ( fileopentype == FILE_READ_MODE )
	CreateFileBuffers( file );

    return ( file );
}

/******************************************************************************

MODULE:  CreateFileDescriptorMosaic

PURPOSE:  Creates a file descriptor using information from the MOSAIC
          descriptor

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
FileDescriptor *CreateFileDescriptorMosaic
(
    MosaicDescriptor *mosaic,   /* I:  session info */
    int in_bandnum,             /* I:  number of the input band */
    int out_bandnum,            /* I:  number of the output band */
    FileOpenType fileopentype,  /* I:  reading or writing */
    char *filename              /* I:  the file name */
)

{
    FileDescriptor *file = NULL;        /* return value, new descriptor */

    /* allocate a descriptor */
    file = ( FileDescriptor * ) calloc( 1, sizeof( FileDescriptor ) );
    if ( !file )
        ErrorHandler( TRUE, "CreateFileDescriptorMosaic", ERROR_MEMORY,
            "File Descriptor" );

    /* copy the file name */
    file->filename = strdup( filename );
    if ( !file->filename )
        ErrorHandler( TRUE, "CreateFileDescriptorMosaic", ERROR_MEMORY,
            "File Name" );

    /* get the info from the modis file */
    file->fileopentype = fileopentype;

    file->filetype = mosaic->filetype;
    file->datatype = mosaic->bandinfo[in_bandnum].input_datatype;
    file->file_endianness = mosaic->input_file_endian;
    file->bandnum = out_bandnum;

    /* calculate the size of the data needed for a pixel */
    file->datasize = DFKNTsize( file->datatype );

    /* get the size for this band, the band number, and the size of a pixel */
    file->nrows = mosaic->bandinfo[in_bandnum].nlines;
    file->ncols = mosaic->bandinfo[in_bandnum].nsamples;
    file->pixel_size = mosaic->bandinfo[in_bandnum].pixel_size;
    file->output_pixel_size = mosaic->bandinfo[in_bandnum].pixel_size;
    file->background_fill = mosaic->bandinfo[in_bandnum].background_fill;

    /* create an internal read/write buffer for this descriptor */
    file->rowbuffer = ( void * ) calloc( file->ncols, file->datasize );
    if ( !file->rowbuffer )
        ErrorHandler( TRUE, "CreateFileDescriptorMosaic", ERROR_MEMORY,
        "Error allocating space for the row buffer" );

    /* if we're reading, create the read buffers */
    if ( fileopentype == FILE_READ_MODE )
        CreateFileBuffers( file );

    return ( file );
}

/******************************************************************************

MODULE:  DestroyFileDescriptor

PURPOSE:  Release all the memory used by a descriptor

RETURN VALUE:
Type = int
Value           Description
-----           -----------
TRUE            Always success

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00   Rob Burrell            Original Development
         01/01   John Rishea            Standardized formatting 

NOTES:

******************************************************************************/
int DestroyFileDescriptor
(
    FileDescriptor *file	/* I:  the descriptor to destroy */
)

{
    boolean DestroyFileBuffers( FileDescriptor *file );

    /* throw away read buffers */
    DestroyFileBuffers( file );

    /* free internal buffer and filename */
    if ( file->rowbuffer )
	free( file->rowbuffer );
    if ( file->filename )
	free( file->filename );
    /* free the descriptor itself */
    free( file );

    return ( TRUE );
}
