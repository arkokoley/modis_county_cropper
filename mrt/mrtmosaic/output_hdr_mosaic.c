#include <stdlib.h>		/* exit()    prototype */
#include <stdio.h>		/* sprintf() prototype */
#include <string.h>		/* strdup()  prototype */
#include <time.h>		/* clock()   prototype */
#include "mosaic.h"

/******************************************************************************

FILE:  OutputHdrMosaic

PURPOSE:  Write mosaic information to the output .hdr file

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/02  Gail Schmidt           Original development
         07/02  Gail Schmidt           Allow the output filename to be
                                       specified

HARDWARE AND/OR SOFTWARE LIMITATIONS:
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/

int OutputHdrMosaic
(
    MosaicDescriptor *mosaic,
    char *output_filename
)

{
    int i, j;
    size_t k;
    ModisDescriptor *P;		/* Modis descriptor struct */
   
    /* attempt to allocate space for a Modis descriptor */
    P = ( ModisDescriptor * ) calloc( 1, sizeof( ModisDescriptor ) );
    if ( P == NULL ) 
    {
        ErrorHandler( FALSE, "Hdf2HdrMosaic", ERROR_MEMORY,
            "Unable to allocate memory");
        return MOSAIC_ERROR;
    }

    /* initialize ModisDescriptor */
    InitializeModisDescriptor( P );

    P->input_filename = strdup( mosaic->filename );
   
    /* if memory for input_filename can't be allocated, error */
    if( P->input_filename == NULL )
    {
        ErrorHandler( FALSE, "Hdf2HdrMosaic", ERROR_MEMORY,
            "Unable to allocate memory");
        return MOSAIC_ERROR;
    } 

    /* fill in output file info fields */
    P->output_file_info = ( OutFileType * )
        calloc( mosaic->nbands, sizeof( OutFileType ) );
    if ( P->output_file_info == NULL )
    {
        ErrorHandler( FALSE, "Hdf2HdrMosaic", ERROR_MEMORY,
            "Unable to allocate memory");
        return MOSAIC_ERROR;
    } 
    
    /* store output .hdr filename */
    P->output_filename = strdup( output_filename );
    if ( P->output_filename == NULL )
    {
        ErrorHandler( FALSE, "Hdf2HdrMosaic", ERROR_MEMORY,
            "Unable to allocate memory");
        return MOSAIC_ERROR;
    } 

    /* fill in the projection information */
    P->output_projection_type = mosaic->projection_type;
    for ( i = 0; i < 15; i++ )
    {
	P->output_projection_parameters[i] = mosaic->projection_parameters[i];
    }
    P->output_zone_code = mosaic->zone_code;
    P->coord_origin = UL;

    /* fill in image extents */
    for ( i = 0; i < 4; i++ )
    {
	for ( j = 0; j < 2; j++ )
	{
	    P->ll_extents_corners[i][j] = mosaic->ll_image_extent[i][j];
	    P->proj_extents_corners[i][j] = mosaic->proj_image_extent[i][j];
	}
    }

    P->nbands = mosaic->nbands;
    P->bandinfo = calloc( P->nbands, sizeof( BandType ) );
    if ( P->bandinfo == NULL )
    {
        ErrorHandler( FALSE, "Hdf2HdrMosaic", ERROR_MEMORY,
            "Cannot allocate memory for band selection." );
        return MOSAIC_ERROR;
    }

    P->output_filetype = RAW_BINARY;
    P->nfiles_out = 0;
    for ( k = 0; k < P->nbands; k++ )
    {
	P->bandinfo[k].name = strdup( mosaic->bandinfo[k].name );
	P->bandinfo[k].selected = 1;
	P->bandinfo[k].output_datatype = mosaic->bandinfo[k].input_datatype;
        P->bandinfo[k].min_value = mosaic->bandinfo[k].min_value;
        P->bandinfo[k].max_value = mosaic->bandinfo[k].max_value;
        P->bandinfo[k].background_fill = mosaic->bandinfo[k].background_fill;
        P->bandinfo[k].nlines = mosaic->bandinfo[k].nlines;
        P->bandinfo[k].nsamples = mosaic->bandinfo[k].nsamples;

	P->output_file_info[P->nfiles_out].nlines = mosaic->bandinfo[k].nlines;
	P->output_file_info[P->nfiles_out].nsamples =
            mosaic->bandinfo[k].nsamples;
	P->output_file_info[P->nfiles_out].pixel_size =
            mosaic->bandinfo[k].pixel_size;
	P->output_file_info[P->nfiles_out].nbands = mosaic->nbands;
	P->nfiles_out++;

        P->output_file_info[k].nlines = mosaic->bandinfo[k].nlines;
        P->output_file_info[k].nsamples = mosaic->bandinfo[k].nsamples;
    }
    P->output_datum_code = mosaic->datum_code;

    /* write header file (raw binary format) */
    WriteHeaderFile( P );

    return ( MOSAIC_SUCCESS );
}
