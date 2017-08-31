
/******************************************************************************

FILE:  Hdf2Hdr

PURPOSE:  Read an HDF-EOS file and write multifile .hdr (used in GUI)

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         07/00  John M. Weiss          Original Development
         01/01  John Rishea            Standardized formatting
         01/01  John Rishea            Added check for memory allocation
         01/01  John Rishea            Removed debugging code sections
         01/01  John Rishea            Removed 2 unused variables -- 
                                       curr_resolution and firstband

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/
#include <stdlib.h>		/* exit()    prototype */
#include <stdio.h>		/* sprintf() prototype */
#include <string.h>		/* strdup()  prototype */
#include <time.h>		/* clock()   prototype */
#include "resample.h"

int Hdf2Hdr ( char *filename )
{
    int errval, i, j;
    size_t k;
    char *str = NULL,
         *ext = NULL;
    ModisDescriptor *P;		/* Modis descriptor struct */
    FILE *fp;
   
    /* handle case of too few or too many command line arguments */
    if ( filename == NULL || strlen( filename ) == 0 )
    {
	fprintf( stdout, "Usage: resample -h file.hdf\n" );
        fflush( stdout );
	return -1;
    }

    /* determine input file type from input filename extension */
    str = strdup( filename );
    if( str == NULL )
    {
        fprintf( stdout, "Hdf2Hdr: unable to allocate memory (str).\n");
        fflush( stdout );
	return -2;
    } 

    /* ensure that input filename extension is ".hdf" */
    ext = strrchr( str, '.' );
    if ( ext == NULL || strcasecmp( ext, ".hdf" ) != 0 )
    {
	fprintf( stdout, "Error: %s extension is not .hdf.\n", filename );
        fflush( stdout );
	return -3;
    }

    /* try to open and close file */
    fp = fopen( filename, "r" );
    if ( fp == NULL )
    {
	fprintf( stdout, "Error: unable to open %s.\n", filename );
        fflush( stdout );
	return -4;
    }
    fclose( fp );

    /* attempt to allocate space for a Modis descriptor */
    P = ( ModisDescriptor * ) calloc( 1, sizeof( ModisDescriptor ) );
    if ( P == NULL ) 
    {
        fprintf( stdout, "Hdf2Hdr: unable to allocate memory (P).\n");
        fflush( stdout );
	return -5;
    }

    /* initialize ModisDescriptor */
    InitializeModisDescriptor( P );

    P->input_filename = strdup( filename );
   
    /* if memory for input_filename can't be allocated, error */
    if( P->input_filename == NULL )
    {
        fprintf( stdout,
            "Hdf2Hdr: unable to allocate memory (P->input_filename).\n");
        fflush( stdout );
	return -6;
    } 

    P->input_filetype = HDFEOS;

    /* open input file and read file info (nbands, nlines, nsamples, etc.) */
    errval = ReadHDFHeader( P );
    if ( errval != MRT_NO_ERROR )
    {
	fprintf( stdout,
            "Hdf2Hdr: could not read HDF-EOS file info correctly.\n" );
        fflush( stdout );
	return -7;
    }

    /* fill in output file info fields */
    P->output_file_info = ( OutFileType * )
        calloc( P->nbands, sizeof( OutFileType ) );
    if ( P->output_file_info == NULL )
    {
        fprintf( stdout,
            "Hdf2Hdr: unable to allocate memory (P->output_file_info).\n");
        fflush( stdout );
	return -8;
    } 
    
    /* store output .hdr filename */
    P->output_filename = strdup( "TmpHdr.hdr" );
    if ( P->output_filename == NULL )
    {
        fprintf( stdout,
            "Hdf2Hdr: unable to allocate memory (P->output_filename).\n");
        fflush( stdout );
	return -9;
    } 

    /* fill in the projection information */
    P->output_projection_type = P->input_projection_type;
    for ( i = 0; i < 15; i++ )
	P->output_projection_parameters[i] = P->input_projection_parameters[i];
    if (P->input_projection_type == PROJ_UTM)
        P->output_zone_code = P->input_zone_code;

    /* fill in other fields */
    for ( i = 0; i < 4; i++ )
    {
	for ( j = 0; j < 2; j++ )
	{
	    P->ll_extents_corners[i][j] = P->input_image_extent[i][j];
	}
    }

    P->output_filetype = RAW_BINARY;
    P->nfiles_out = 0;
    for ( k = 0; k < P->nbands; k++ )
    {
	P->bandinfo[k].output_datatype = P->bandinfo[k].input_datatype;
	P->output_file_info[P->nfiles_out].nlines = P->bandinfo[k].nlines;
	P->output_file_info[P->nfiles_out].nsamples = P->bandinfo[k].nsamples;
	P->output_file_info[P->nfiles_out].pixel_size =
            P->bandinfo[k].pixel_size;
	P->output_file_info[P->nfiles_out].nbands = P->nbands;
	P->nfiles_out++;
    }
    P->output_datum_code = P->input_datum_code;

    /* write header file (raw binary format) */
    WriteHeaderFile( P );

    /* output the .hdr file name to stdout, required for the GUI */
    printf( "%s\n", P->output_filename );

    return 0;
}
