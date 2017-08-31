/******************************************************************************

FILE:  mosaic.c

PURPOSE:  Stitch all the input files (ISIN and SIN data) together into one
    product

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         03/02  Gail Schmidt           Original Development
         11/02  Gail Schmidt           Support SIN data in addition to ISIN

HARDWARE AND/OR SOFTWARE LIMITATIONS:
  None

PROJECT:    MODIS Reprojection Tool

NOTES:  Only SIN and ISIN data products can be mosaicked, since it will be
        assumed that no overlap will occur.

******************************************************************************/
#if defined(__CYGWIN__) || defined(WIN32)
#include <getopt.h>             /* getopt  prototype */
#else
#include <unistd.h>             /* getopt  prototype */
#endif

#include <time.h>               /* clock()   prototype */
#include "worgen.h"
#include "mosaic.h"
#include "mrt_dtype.h"

/* Local prototypes */
static int ExpandEnvironment( char *line, size_t max_linelen );
static MRT_UINT64 EstimateFileSize( MosaicDescriptor *mosaicfile );
int getInputFileNamesFromFile( FILE * ifile, char ***str, int *n );
void freeInputFileNameList( char **filelist, int nfiles );

int main
(
    int argc,
    char *argv[]
)

{
    int i, j;
    int num_infiles;         /* number of input files in the argument list */
    int numh_tiles;          /* number of horiz tiles in the mosaic */
    int numv_tiles;          /* number of vert tiles in the mosaic */
    int nspectral_bands;     /* number of spectral bands selected */
    int **tile_array = NULL; /* 2D array of size [numv_tiles][numh_tiles]
                                specifying which input file represents
                                that tile location (space is allocated
                                for this array in SortProducts) */
    int determine_tiles;     /* does the user want the tiles of each file? */
    int write_tmphdr;        /* does the user want the mosaic info written
                                to TmpHdr.hdr? */
    int spectral_subset;     /* did the user specify spectral subsetting? */
    int status = MRT_NO_ERROR;   /* function return status */
    time_t startdate, enddate;  /* start and end date struct */
    char errmsg[SMALL_STRING];  /* error message string */
    char bandstr[SMALL_STRING]; /* string for the spectral subset bands */
    char input_filenames[NUM_ISIN_TILES][FILENAME_LENGTH+1];
                             /* input files to be mosaicked (allocate enough
                                space for the number of possible ISIN tiles) */
    char output_filename[FILENAME_LENGTH+1];
                             /* output filename for the mosaicked image */
    char tmpstr[LARGE_STRING];  /* temporary string for tile positions */
    char tmpstr2[SMALL_STRING]; /* temporary string for tile positions */
    MosaicDescriptor *infiles = NULL;
                             /* file descriptor array for the input files */
    MosaicDescriptor mosaicfile;
                             /* file descriptor for the output mosaic file */

    /* Set up a log file and process the -g command line option if it exists */
    InitLogHandler( argc, argv );

    /* Get the starting date and time */
    startdate = time( NULL );

    /* Check the command-line arguments and verify that all the required
       arguments were specified.  Also read the input filenames and the
       output filename. */
    if ( CheckMosaicArgs( argc, argv, input_filenames, &num_infiles,
        output_filename, bandstr, &determine_tiles, &write_tmphdr,
        &spectral_subset ) != MOSAIC_SUCCESS )
    {
        ErrorHandler( FALSE, "mosaic", ERROR_GENERAL,
            "Error processing the arguments for the mosaic tool" );
        CloseLogHandler( );
        return EXIT_FAILURE;
    }

    /* If -t was specified, then determine the tiles for each input file.
       We are done after this. */
    if ( determine_tiles )
    {
        ReadTiles( input_filenames, num_infiles );
        CloseLogHandler( );
        return EXIT_SUCCESS;
    }

    /* Create a MosaicDescriptor for each input file */
    infiles = calloc( num_infiles, sizeof( MosaicDescriptor ) );
    if ( infiles == NULL )
    {
        ErrorHandler( FALSE, "mosaic", ERROR_GENERAL,
            "Error allocating memory for the MosaicDescriptors" );
        CloseLogHandler( );
        return EXIT_FAILURE;
    }

    /* Initialize the MosaicDescriptor for each input file */
    for ( i = 0; i < num_infiles; i++ )
        InitializeMosaicDescriptor( &infiles[i] );

    /* Read the metadata information for each input file */
    for ( i = 0; i < num_infiles; i++ )
    {
        infiles[i].filename = strdup ( input_filenames[i] );
#ifdef DEBUG
        printf ("Reading metadata for #%d: %s\n", i, infiles[i].filename);
#endif
        ReadMetaMosaic( &infiles[i] );

        /* Get the ISIN tile information */
        if ( infiles[i].projection_type == PROJ_ISIN ||
             infiles[i].projection_type == PROJ_SIN )
        {
            switch ( infiles[i].filetype )
            {
                case RAW_BINARY:
                    status = read_tile_number_rb( infiles[i].filename,
                        &infiles[i].horiz, &infiles[i].vert );
                    break;

                case HDFEOS:
                    status = read_tile_number_hdf( infiles[i].filename,
                        &infiles[i].horiz, &infiles[i].vert );
                    break;
            }

            if ( status != MRT_NO_ERROR )
            {
                sprintf( errmsg, "Error reading tile numbers for %s. "
                    "Horizontal and vertical tile numbers are read from the "
                    "embedded metadata for HDF-EOS files.  For raw binary "
                    "files the filename must contain h##v## in the filename "
                    "so that the tile numbers can be known.\n",
                    infiles[i].filename );
                ErrorHandler( FALSE, "mosaic", ERROR_GENERAL, errmsg );
                CloseLogHandler( );
                return EXIT_FAILURE;
            }
        }
    }

#ifdef DEBUG
    for ( i = 0; i < num_infiles; i++ )
    {
        printf ("Input file #%d-->\n", i);
        printf ("   filename - %s\n", infiles[i].filename);
        printf ("   tiles (h,v) - %d, %d\n", infiles[i].horiz, infiles[i].vert);
        printf ("   filetype - %d\n", infiles[i].filetype);
        printf ("   nbands   - %d\n", infiles[i].nbands);
        printf ("   projection_type - %d\n", infiles[i].projection_type);
        printf ("   datum_code - %d\n", infiles[i].datum_code);
        printf ("   zone_code - %d\n", infiles[i].zone_code);
        printf ("   UL(x/y) - %f %f\n", infiles[i].proj_image_extent[UL][0],
            infiles[i].proj_image_extent[UL][1] );
        printf ("   UR(x/y) - %f %f\n", infiles[i].proj_image_extent[UR][0],
            infiles[i].proj_image_extent[UR][1] );
        printf ("   LL(x/y) - %f %f\n", infiles[i].proj_image_extent[LL][0],
            infiles[i].proj_image_extent[LL][1] );
        printf ("   LR(x/y) - %f %f\n", infiles[i].proj_image_extent[LR][0],
            infiles[i].proj_image_extent[LR][1] );
        printf ("   UL(lat/long) - %f %f\n", infiles[i].ll_image_extent[UL][0],
            infiles[i].ll_image_extent[UL][1] );
        printf ("   UR(lat/long) - %f %f\n", infiles[i].ll_image_extent[UR][0],
            infiles[i].ll_image_extent[UR][1] );
        printf ("   LL(lat/long) - %f %f\n", infiles[i].ll_image_extent[LL][0],
            infiles[i].ll_image_extent[LL][1] );
        printf ("   LR(lat/long) - %f %f\n", infiles[i].ll_image_extent[LR][0],
            infiles[i].ll_image_extent[LR][1] );
    }
#endif

    /* Verify that the data types, resolutions, dimensions, etc. are
       the same for each SDS in each file. The intent is to mosaic tiles
       of the same data products, however if everything matches up there
       is no reason different data products can't be mosaicked. Only ISIN
       and SIN products can be mosaicked, since it is assumed the files
       will not have any overlap. */
    if ( CompareProducts( num_infiles, infiles ) != MOSAIC_SUCCESS )
    {
        ErrorHandler( FALSE, "mosaic", ERROR_GENERAL,
            "Error: The input files must be of the same data product" );
        CloseLogHandler( );
        return EXIT_FAILURE;
    }

    /* Determine the order of the input tiles to create the output image
       and create the mosaicfile descriptor */
    if ( SortProducts( num_infiles, infiles, output_filename, &mosaicfile,
         &numh_tiles, &numv_tiles, &tile_array ) != MOSAIC_SUCCESS )
    {
        ErrorHandler( FALSE, "mosaic", ERROR_GENERAL,
            "Error: The input files must be of the same data product" );
        CloseLogHandler( );
        return EXIT_FAILURE;
    }

#ifdef DEBUG
    printf ("   filename - %s\n", mosaicfile.filename);
    printf ("   nbands   - %d\n", mosaicfile.nbands);
    printf ("   projection_type - %d\n", mosaicfile.projection_type);
    printf ("   datum_code - %d\n", mosaicfile.datum_code);
    printf ("   zone_code - %d\n", mosaicfile.zone_code);
    printf ("   UL(x/y) - %f %f\n", mosaicfile.proj_image_extent[UL][0],
        mosaicfile.proj_image_extent[UL][1] );
    printf ("   UR(x/y) - %f %f\n", mosaicfile.proj_image_extent[UR][0],
        mosaicfile.proj_image_extent[UR][1] );
    printf ("   LL(x/y) - %f %f\n", mosaicfile.proj_image_extent[LL][0],
        mosaicfile.proj_image_extent[LL][1] );
    printf ("   LR(x/y) - %f %f\n", mosaicfile.proj_image_extent[LR][0],
        mosaicfile.proj_image_extent[LR][1] );
    printf ("   UL(lat/long) - %f %f\n", mosaicfile.ll_image_extent[UL][0],
        mosaicfile.ll_image_extent[UL][1] );
    printf ("   UR(lat/long) - %f %f\n", mosaicfile.ll_image_extent[UR][0],
        mosaicfile.ll_image_extent[UR][1] );
    printf ("   LL(lat/long) - %f %f\n", mosaicfile.ll_image_extent[LL][0],
        mosaicfile.ll_image_extent[LL][1] );
    printf ("   LR(lat/long) - %f %f\n", mosaicfile.ll_image_extent[LR][0],
        mosaicfile.ll_image_extent[LR][1] );
#endif

    /* If -h was specified, then output the mosaic information to TmpHdr.hdr.
       We are done after this. */
    if ( write_tmphdr )
    {
        if ( OutputHdrMosaic( &mosaicfile, "TmpHdr.hdr" ) != MOSAIC_SUCCESS )
        {
            CloseLogHandler( );
            return EXIT_FAILURE;
        }

        CloseLogHandler( );
        return EXIT_SUCCESS;
    }

    /* Now that the write header and tile text stuff is done, print a log file
       header */
    MessageHandler( NULL,
       "*******************************************************************"
       "***********\n");
    MessageHandler( NULL, "%s (%s)", MOSAIC_NAME, RESAMPLER_VERSION );
    MessageHandler( NULL, "Start Time:  %s", ctime( &startdate ) );
    MessageHandler( NULL,
    "------------------------------------------------------------------\n" );

    /* Log the input filenames, output filename, and mosaic array */
    MessageHandler( NULL, "Input filenames (%d):", num_infiles);
    for ( i = 0; i < num_infiles; i++ )
        MessageHandler( NULL,"   %s", input_filenames[i]);
    MessageHandler( NULL, "Output filename: %s", output_filename);
    MessageHandler( NULL, "Mosaic Array:");
    for ( i = 0; i < numv_tiles; i++ )
    {
        sprintf( tmpstr, " " );
        for ( j = 0; j < numh_tiles; j++ )
        {
            sprintf( tmpstr2, "  file[%2d]", tile_array[i][j] );
            strcat( tmpstr, tmpstr2 );
        }
        MessageHandler( NULL, "%s", tmpstr );
    }

    /* If -s was specified, then read the spectral subset bands, otherwise
       all bands will be processed by default */
    nspectral_bands = GetSpectralSubsetMosaic( &mosaicfile, bandstr,
        spectral_subset );
    if ( nspectral_bands == 0 )
    {
        sprintf( errmsg, "Error processing spectral subset (%s) "
            "for mosaic tool", bandstr );
        ErrorHandler( FALSE, "mosaic", ERROR_GENERAL, errmsg );
        CloseLogHandler( );
        return EXIT_FAILURE;
    }

    if( mosaicfile.filetype == HDFEOS ) {
       MRT_UINT64 estfs = EstimateFileSize( &mosaicfile );
       if( estfs > 2147483647ULL ) {
          char eststr[256];
          size_t commas, cloop, dloop, eststrlen;
          char *ptr;
          sprintf( eststr, MRT_UINT64_FMT, estfs );
          /* Add commas in place.  Could have used memmove, I suppose */
          eststrlen = strlen( eststr );
          ptr = eststr + eststrlen - 3;
          commas = (eststrlen - 1) / 3;
          for(  cloop = 0; cloop < commas; ++cloop ) {
             for( dloop = (cloop + 1) * 3 + cloop + 1; dloop > 0; --dloop ) {
                *(ptr + dloop) = *(ptr + dloop - 1);
             }
             *ptr = ',';
             ptr -= 3;
          }
          sprintf( errmsg, "Estimated HDF file output size of %s bytes is "
               "greater than than the HDF v4 limit of 2G.", eststr );
          ErrorHandler( FALSE, "mosaic", ERROR_GENERAL, errmsg );
          CloseLogHandler( );
          return EXIT_FAILURE;
       }
       /* Just state a warning if close. 150K? */
       if( estfs + 153600ULL > 2147483647ULL ) {
          sprintf( errmsg, "Warning: The estimated HDF file output is "
               "very close to the HDF v4 limit of 2G.  The resulting HDF "
               "file might be unreadable or an error may occur later in "
               "processing.  Processing will continue..." );
          ErrorHandler( FALSE, "mosaic", ERROR_GENERAL, errmsg );
       }
    }

    /* Mosaic the tiles together */
    if ( MosaicTiles ( numh_tiles, numv_tiles, tile_array, num_infiles,
        infiles, &mosaicfile ) != MOSAIC_SUCCESS )
    {
        sprintf( errmsg, "Error in the mosaic process" );
        ErrorHandler( FALSE, "mosaic", ERROR_GENERAL, errmsg );
        CloseLogHandler( );
        return EXIT_FAILURE;
    }

    /* Mosaic the metadata and output the extents of the new image */
    if ( mosaicfile.filetype == RAW_BINARY )
    {
        /* Output a header file for the raw binary data */
        OutputHdrMosaic( &mosaicfile, mosaicfile.filename );
    }
    else
    {
        /* Output the metadata for the HDF-EOS file. The metadata structures
           from the original HDF-EOS files will be appended to the metadata
           using the "Old" prefix. */
        if ( AppendMetadataMosaic( numh_tiles, numv_tiles, tile_array,
            infiles, &mosaicfile ) != MOSAIC_SUCCESS )
        {
            sprintf( errmsg, "Error outputting the mosaic metadata (HDF-EOS)" );
            ErrorHandler( FALSE, "mosaic", ERROR_GENERAL, errmsg );
            CloseLogHandler( );
            return EXIT_FAILURE;
        }
    }

    /* dump useful output file info */
    PrintOutputFileInfoMosaic( &mosaicfile );

    /* stop timer and print elapsed time */
    enddate = time( NULL );
    MessageHandler( NULL, "End Time:  %s", ctime( &enddate ) );
    MessageHandler( NULL, "Finished mosaicking!\n" );
    MessageHandler( NULL,"******************************************************************************\n");

    CloseLogHandler( );
    return EXIT_SUCCESS;
}


/******************************************************************************

MODULE:  CheckMosaicArgs

PURPOSE:  Capture command line arguments and verify they are valid

RETURN VALUE:
Type = int
Value           Description
-----           -----------
MOSAIC_SUCCESS  Successful completion
MOSAIC_ERROR    Error in processing


HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         03/02  Gail Schmidt           Original Development

NOTES:

******************************************************************************/
int CheckMosaicArgs
(
    int argc,              /* I: number of arguments */
    char *argv[],          /* I: argument strings */
    char ifilenames[][FILENAME_LENGTH+1],
                           /* O: input filenames specified in the arguments */
    int *num_files,        /* O: number of input filenames specified */
    char ofilename[],      /* O: output filename specified in the arguments */
    char bandstr[],        /* O: string of spectral subset bands */
    int *determine_tiles,  /* O: was -t switch specified to determine the
                                 tiles for each input filename? */
    int *write_tmphdr,     /* O: was -h switch specified to write the
                                 raw binary header info for the mosaic? */
    int *spectral_subset   /* O: was -s switch specified for spectral
                                 subsetting? */
)

{
    int c;
    int i;                       /* looping variable */
    char errmsg[SMALL_STRING];   /* error message */
    int hswitch = FALSE;         /* output TmpHdr.hdr for mosaic */
    int iswitch = FALSE;         /* input files specified */
    int oswitch = FALSE;         /* output files specified */
    int tswitch = FALSE;         /* tile switch specified */
    FileType input_filetype,     /* input filetype */
             output_filetype;    /* output filetype */

    /* Initialize determine tiles, write tmphdr, and spectral subset to false */
    *determine_tiles = FALSE;
    *write_tmphdr = FALSE;
    *spectral_subset = FALSE;

    opterr = 0;         /* do not print error messages to stdout */
    while ( ( c = getopt( argc, argv, "i:o:g:s:th" ) ) != -1 )
    {   /* the -t (get tile info) and -h (output to TmpHdr.hdr) switches
           don't have any arguments */
        switch( c )
        {
            case 'i':   /* input filenames */
                *num_files = GetInputFilenames( optarg, ifilenames );
                if ( *num_files <= 0 )
                {
                    sprintf( errmsg, "Error processing input_filenames from "
                        "(%s) for the mosaic tool", optarg );
                    ErrorHandler( FALSE, "CheckMosaicArgs", ERROR_GENERAL,
                        errmsg );
                    MosaicUsage( );
                    return MOSAIC_ERROR;
                }
                iswitch = TRUE;
                break;

            case 'o':   /* output filename */
                strcpy( ofilename, optarg );
                if ( ofilename == NULL )
                {
                    sprintf( errmsg, "Error processing output_filename (%s) "
                        "for mosaic tool", optarg );
                    ErrorHandler( FALSE, "CheckMosaicArgs", ERROR_GENERAL,
                        errmsg );
                    MosaicUsage( );
                    return MOSAIC_ERROR;
                }
                oswitch = TRUE;
                break;

            case 's':   /* spectral subsetting */
                strcpy( bandstr, optarg );
                for ( i = 0; i < (int) strlen( bandstr ); i++ )
                {
                    if ( bandstr[i] != '0' && bandstr[i] != '1' &&
                         bandstr[i] != ' ' )
                    {
                        sprintf( errmsg, "Error processing spectral subset "
                            "(%s) for mosaic tool. Only '0's and '1's are "
                            "allowed.", optarg );
                        ErrorHandler( FALSE, "CheckMosaicArgs", ERROR_GENERAL,
                            errmsg );
                        MosaicUsage( );
                        return MOSAIC_ERROR;
		    }
                }
                *spectral_subset = TRUE;
                break;

            case 't':   /* determine the tile numbers */
                *determine_tiles = TRUE;
                tswitch = TRUE;
                break;

            case 'h':   /* output overall mosaic information as a raw binary
                           header to TmpHdr.hdr */
                *write_tmphdr = TRUE;
                hswitch = TRUE;
                break;

            case 'g':   /* log file name, should be processed in
                           InitLogHandler() */
                break;
        }
    }

    /* check usage - either input filenames and output filename or input
       filenames and tile switch must be specified. Or the hswitch may be
       specified by itself. */
    if ( !hswitch && ( !iswitch || ( !oswitch && !tswitch ) ) )
    {
        MosaicUsage( );
        return MOSAIC_ERROR;
    }

    /* verify that the input file type and output file types are the same */
    if ( oswitch )
    {
        GetInputFileExt( ifilenames[0], &input_filetype );
        GetInputFileExt( ofilename, &output_filetype );
        if ( input_filetype != output_filetype )
        {
            ErrorHandler( FALSE, "CheckMosaicArgs", ERROR_GENERAL,
                "Input and output file types (HDF-EOS or raw binary) must "
                "match" ); 
            return MOSAIC_ERROR;
        }
    }

    return MOSAIC_SUCCESS;
}


/******************************************************************************

MODULE:  GetInputFilenames

PURPOSE:  Read the input_filenames file and read the input files listed in
          the input_filenames file for mosaicking

RETURN VALUE:
Type = int
Value           Description
-----           -----------
>0              Number of input filenames (successful completion)
=0              Error in processing

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         03/02  Gail Schmidt           Original Development

NOTES:

******************************************************************************/
int GetInputFilenames
(
    char *infile,          /* I: input filenames file to be read */
    char ifilenames[][FILENAME_LENGTH+1]
                           /* O: input filenames specified in the arguments
                                 (can't be more than NUM_ISIN_TILES strings) */
) {
   int r, i, n = 0;            /* loop and response variables */
   char errmsg[SMALL_STRING];  /* error messages */
   int nfiles = 0;             /* number of input files in argstr */
   FILE *ifile = NULL;         /* input file pointer */
   char **filelist;

   /* Open the infile for reading */
   ifile = fopen( infile, "r" );
   if ( ifile == NULL ) {
      sprintf( errmsg, "Error opening %s for reading\n", infile );
      ErrorHandler( TRUE, "GetInputFilenames", ERROR_OPEN_INPUTPAR, errmsg );
   }

   /* Determine the number of input filenames.  Assume at least one input
      file. */
   n = getInputFileNamesFromFile( ifile, &filelist, &nfiles );
   if( n ) {
      fclose( ifile );
      if( n == 1 ) {
         sprintf( errmsg, "Error reading %s\n", infile );
         ErrorHandler( TRUE, "GetInputFilenames", ERROR_GENERAL, errmsg );
      } else if( n == 2 ) {
         sprintf( errmsg, "Syntax error while reading %s: No ending quote "
                          "found.\n", infile );
         ErrorHandler( TRUE, "GetInputFilenames", ERROR_GENERAL, errmsg );
      } else {
         sprintf( errmsg, "Error reading %s: Memory allocation error.\n",
                          infile );
         ErrorHandler( TRUE, "GetInputFilenames", ERROR_GENERAL, errmsg );
      }
   return 0;
   }

   /* For now, copy files names into the existing ifilenames array. */
   for( i = 0; i < nfiles; ++i ) {
      /* Verify that the number of files is not larger than the number
         allowed.  If it is, then don't read any more filenames. */
      if ( nfiles >= NUM_ISIN_TILES ){
         sprintf( errmsg, "Warning: Only %d input files are supported by "
             "the mosaic tool, thus only the first %d files will be "
             "mosaicked.  All other files will be ignored.", NUM_ISIN_TILES,
             NUM_ISIN_TILES );
         ErrorHandler( FALSE, "GetInputFilenames", ERROR_GENERAL,
                       errmsg );
         break;
      }

      strncpy( ifilenames[i], filelist[i], FILENAME_LENGTH );
      ifilenames[i][FILENAME_LENGTH] = 0;
      r = ExpandEnvironment( ifilenames[i], FILENAME_LENGTH );
      if( r == 2 ) {
         freeInputFileNameList( filelist, nfiles );
         sprintf( errmsg,
                  "Error expanding environment variables in the file %s\n",
                  infile );
         ErrorHandler( TRUE, "GetInputFilenames", ERROR_OPEN_INPUTPAR,
                       errmsg );
      } else if( r != 0 ) {
         freeInputFileNameList( filelist, nfiles );
         sprintf( errmsg,
                  "Environment variable %s found in file %s, but" \
                  "was not defined in the environment\n",
                  ifilenames[i], infile );
         ErrorHandler( TRUE, "GetInputFilenames", ERROR_OPEN_INPUTPAR,
                       errmsg );
      }
   }
   freeInputFileNameList( filelist, nfiles );
   if( nfiles >= NUM_ISIN_TILES )
      nfiles = NUM_ISIN_TILES;

   return nfiles;
}


/******************************************************************************

MODULE:  ReadMetaMosaic

PURPOSE:  Read the metadata for each file (HDF-EOS or raw binary)

RETURN VALUE:
Type = int
Value           Description
-----           -----------
MOSAIC_SUCCESS  Successful completion
MOSAIC_ERROR    Error in processing

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         03/02  Gail Schmidt           Original Development

NOTES:

******************************************************************************/
int ReadMetaMosaic
(
    MosaicDescriptor *mosaic     /* I/O:  session info */
)

{
    int status = MRT_NO_ERROR;      /* function return status */
    char errmsg[SMALL_STRING];  /* error message string */

    /* Determine file type from filename extension */
    if ( GetInputFileExt( mosaic->filename, &mosaic->filetype ) != MRT_NO_ERROR )
    {
        sprintf( errmsg, "Error determining the file type for %s\n",
            mosaic->filename );
        ErrorHandler( TRUE, "ReadMetaMosaic", ERROR_GENERAL, errmsg );
        return MOSAIC_ERROR;
    }

    /* Open file and read file info (nbands, nlines, nsamples, etc.) */
    switch ( mosaic->filetype )
    {
        case RAW_BINARY:
            status = ReadHeaderFileMosaic( mosaic );
            break;

        case HDFEOS:
            status = ReadHDFHeaderMosaic( mosaic );
            break;
    }

    if ( status != MRT_NO_ERROR )
    {
        sprintf( errmsg, "Error reading metadata for %s\n", mosaic->filename );
        ErrorHandler( TRUE, "ReadMetaMosaic", ERROR_GENERAL, errmsg );
        return MOSAIC_ERROR;
    }

    return MOSAIC_SUCCESS;
}


/******************************************************************************

MODULE:  ReadTiles

PURPOSE:  Determine the ISIN tiles for each filename (HDF-EOS or raw binary)
          and output them to tiles.txt.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
MOSAIC_SUCCESS  Successful completion
MOSAIC_ERROR    Error in processing

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/02  Gail Schmidt           Original Development

NOTES:

******************************************************************************/
int ReadTiles
(
    char filenames[][FILENAME_LENGTH+1], /* I: files to be mosaicked */
    int num_files                        /* I: number of files to be mosaicked */
)

{
    int status = MRT_NO_ERROR;  /* function return status */
    int i;                      /* looping variable */
    char errmsg[SMALL_STRING];  /* error message string */
    FileType filetype;          /* hdf-eos or raw binary file? */
    int horiz;                  /* horizontal position of tile */
    int vert;                   /* vertical position of tile */
    FILE *ofile = NULL;         /* output file for tile locations */

    /* Determine file type from filename extension of the first file. It
       is assumed that all input files are of the same format. */
    if ( GetInputFileExt( filenames[0], &filetype ) != MRT_NO_ERROR )
    {
        sprintf( errmsg, "Error determining the file type for %s\n",
            filenames[0] );
        ErrorHandler( TRUE, "ReadTiles", ERROR_GENERAL, errmsg );
    }

    /* Open the tile.txt output file */
    ofile = fopen ("tile.txt", "w");
    if ( ofile == NULL )
    {
        sprintf( errmsg, "Error opening tile.txt for writing" );
        ErrorHandler( TRUE, "ReadTiles", ERROR_GENERAL, errmsg );
    }

    /* Loop through the input filenames, read the tiles, and output the
       tiles to tile.txt */
    for ( i = 0; i < num_files; i++ )
    {
        /* Open file and read tile info */
        switch ( filetype )
        {
            case RAW_BINARY:
                status = read_tile_number_rb( filenames[i], &horiz, &vert );
                break;

            case HDFEOS:
                status = read_tile_number_hdf( filenames[i], &horiz, &vert );
                break;
        }

        if ( status != MRT_NO_ERROR )
        {
            sprintf( errmsg, "Error reading tile numbers for %s. Horizontal "
                "and vertical tile numbers are read from the embedded "
                "metadata for HDF-EOS files.  For raw binary files the "
                "filename must contain h##v## in the filename so that the "
                "tile numbers can be known.\n", filenames[i] );
            ErrorHandler( TRUE, "ReadTiles", ERROR_GENERAL, errmsg );
        }

        /* Output the tile info to the output file tile.txt */
        fprintf( ofile, "%d, %d\n", horiz, vert );
    }

    /* Close tile.txt */
    fclose( ofile );

    return MOSAIC_SUCCESS;
}


/******************************************************************************

MODULE:  CompareProducts

PURPOSE:  Compare the different input files to make sure they match for
          mosaicking. The number of lines, samples, pixel size, data type,
          etc. must match for each band in the file. The number of bands
          must also match. Each file must be in the SIN or ISIN projection,
          since it will be assumed that no overlap between files will exist.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
MOSAIC_SUCCESS  Successful completion
MOSAIC_ERROR    Error in processing

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         04/02  Gail Schmidt           Original Development

NOTES:

******************************************************************************/
int CompareProducts
(
    int num_infiles,            /* I: number of input files */
    MosaicDescriptor infiles[]  /* I: file descriptor array for the input
                                      files */
)

{
    int i;                        /* looping variables */
    size_t j;                     /* looping variables */
    char errmsg[SMALL_STRING];    /* error message string */

    /* Check each file for consistency */
    for ( i = 0; i < num_infiles; i++ )
    {
        /* Check the file type */
        if ( infiles[0].filetype != infiles[i].filetype )
        {
            sprintf( errmsg, "Error: Filetypes don't match for %s and %s\n",
                infiles[0].filename, infiles[i].filename );
            ErrorHandler( FALSE, "CompareProducts", ERROR_GENERAL, errmsg );
            return MOSAIC_ERROR;
        }

        /* Check the number of bands */
        if ( infiles[0].nbands != infiles[i].nbands )
        {
            sprintf( errmsg, "Error: The number of bands doesn't match for %s "
                "(" MRT_SIZE_T_FMT " bands) and %s (" MRT_SIZE_T_FMT
                " bands)\n", infiles[0].filename,
                infiles[0].nbands, infiles[i].filename, infiles[i].nbands );
            ErrorHandler( FALSE, "CompareProducts", ERROR_GENERAL, errmsg );
            return MOSAIC_ERROR;
        }

        /* Check the specific band information */
        for ( j = 0; j < infiles[i].nbands; j++ )
        {
            /* Check the number of lines in each band */
            if ( infiles[0].bandinfo[j].nlines !=
                 infiles[i].bandinfo[j].nlines )
            {
                sprintf( errmsg, "Error: The number of lines in band "
                    MRT_SIZE_T_FMT
                    " doesn't match for %s (%d) and %s (%d)\n",
                    j+1, infiles[0].filename, infiles[0].bandinfo[j].nlines,
                    infiles[i].filename, infiles[i].bandinfo[j].nlines );
                ErrorHandler( FALSE, "CompareProducts", ERROR_GENERAL, errmsg );
                return MOSAIC_ERROR;
            }

            /* Check the number of samples in each band */
            if ( infiles[0].bandinfo[j].nsamples !=
                 infiles[i].bandinfo[j].nsamples )
            {
                sprintf( errmsg, "Error: The number of samples in band "
                    MRT_SIZE_T_FMT
                    " doesn't match for %s (%d) and %s (%d)\n",
                    j+1, infiles[0].filename, infiles[0].bandinfo[j].nsamples,
                    infiles[i].filename, infiles[i].bandinfo[j].nsamples );
                ErrorHandler( FALSE, "CompareProducts", ERROR_GENERAL, errmsg );
                return MOSAIC_ERROR;
            }

            /* Check the data type */
            if ( infiles[0].bandinfo[j].input_datatype !=
                 infiles[i].bandinfo[j].input_datatype )
            {
                sprintf( errmsg, "Error: The data type in band " MRT_SIZE_T_FMT
                    " doesn't match for %s and %s\n",
                    j+1, infiles[0].filename, infiles[i].filename );
                ErrorHandler( FALSE, "CompareProducts", ERROR_GENERAL, errmsg );
                return MOSAIC_ERROR;
            }

            /* Compare the pixel size */
            if ( fabs( infiles[0].bandinfo[j].pixel_size -
                       infiles[i].bandinfo[j].pixel_size ) > 0.000001 )
            {
                sprintf( errmsg, "Error: The pixel size in band "
                    MRT_SIZE_T_FMT
                    " doesn't match for %s (%f) and %s (%f)\n", j+1,
                    infiles[0].filename, infiles[0].bandinfo[j].pixel_size,
                    infiles[i].filename, infiles[i].bandinfo[j].pixel_size );
                ErrorHandler( FALSE, "CompareProducts", ERROR_GENERAL, errmsg );
                return MOSAIC_ERROR;
            }

            /* Compare the rank */
            if ( infiles[0].bandinfo[j].rank !=
                 infiles[i].bandinfo[j].rank )
            {
                sprintf( errmsg, "Error: The dimensionality in band "
                    MRT_SIZE_T_FMT
                    " doesn't match for %s (%dD) and %s (%dD)\n",
                    j+1, infiles[0].filename, infiles[0].bandinfo[j].rank,
                    infiles[i].filename, infiles[i].bandinfo[j].rank );
                ErrorHandler( FALSE, "CompareProducts", ERROR_GENERAL, errmsg );
                return MOSAIC_ERROR;
            }
        }

        /* Verify the projection type is SIN or ISIN */
        if ( infiles[i].projection_type != PROJ_ISIN &&
             infiles[i].projection_type != PROJ_SIN )
        {
            sprintf( errmsg,
                "Error: The projection type for %s is not SIN or ISIN\n",
                infiles[i].filename );
            ErrorHandler( FALSE, "CompareProducts", ERROR_GENERAL, errmsg );
            return MOSAIC_ERROR;
        }

        /* Check the 15 projection parameters */
        for ( j = 0; j < 15; j++ )
        {
            if ( infiles[0].projection_parameters[j] !=
                 infiles[i].projection_parameters[j] )
            {
                sprintf( errmsg, "Error: The projection parameters don't "
                    "match for %s and %s\n", infiles[0].filename,
                    infiles[i].filename );
                ErrorHandler( FALSE, "CompareProducts", ERROR_GENERAL, errmsg );
                return MOSAIC_ERROR;
            }
        }

        /* Check the datum code */
        if ( infiles[0].datum_code != infiles[i].datum_code )
        {
            sprintf( errmsg, "Error: Datum codes don't match for %s and %s\n",
                infiles[0].filename, infiles[i].filename );
            ErrorHandler( FALSE, "CompareProducts", ERROR_GENERAL, errmsg );
            return MOSAIC_ERROR;
        }
    }

    return MOSAIC_SUCCESS;
}

/******************************************************************************

MODULE:  SortProducts

PURPOSE:  Sort the different input files based on their tile numbers for
          mosaicking and create a descriptor for the mosaicked product.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
MOSAIC_SUCCESS  Successful completion
MOSAIC_ERROR    Error in processing

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/02  Gail Schmidt           Original Development

NOTES:

******************************************************************************/
int SortProducts
(
    int num_infiles,            /* I: number of input files */
    MosaicDescriptor infiles[], /* I: file descriptor array for the input
                                      files */
    char output_filename[],     /* I: name of the output file */
    MosaicDescriptor *mosaicfile, /* O: file descriptor for the output mosaic
                                        file */
    int *numh_tiles,            /* O: number of horiz tiles in the mosaic */
    int *numv_tiles,            /* O: number of vert tiles in the mosaic */
    int ***tile_array           /* O: 2D array of size [numv_tiles][numh_tiles]
                                      specifying which input file represents
                                      that tile location (space is allocated
                                      for this array) */
)

{
    size_t i;
    int ni, j, h, v;             /* looping variables */
    int minh = 99, maxh = -9;    /* min/max horizontal tile locations */
    int minv = 99, maxv = -9;    /* min/max vertical tile locations */
    int bounding_coord;          /* is this a bounding coordinate image? */
    int modis_tile;              /* is this a bounding coordinate image? */
    char errmsg[SMALL_STRING];   /* error message string */
    int **tmp_tile_array;        /* temporary pointer for the tile_array so we
                                    can clean up the syntax */
    int index_minh = 0,
        index_maxh = 0;          /* index of minh and maxh */
    int index_minv = 0,
        index_maxv = 0;          /* index of minv and maxv */
    int status = MRT_NO_ERROR;       /* error code status */

    /* Find the min/max horizontal and vertical tile locations */
    for ( ni = 0; ni < num_infiles; ni++ )
    {
        /* Horizontal tile */
        if ( infiles[ni].horiz < minh )
        {
            minh = infiles[ni].horiz;
            index_minh = ni;
        }
        if ( infiles[ni].horiz > maxh )
        {
            maxh = infiles[ni].horiz;
            index_maxh = ni;
        }

        /* Vertical tile */
        if ( infiles[ni].vert < minv )
        {
            minv = infiles[ni].vert;
            index_minv = ni;
        }
        if ( infiles[ni].vert > maxv )
        {
            maxv = infiles[ni].vert;
            index_maxv = ni;
        }
    }

    /* Check that a min/max was found */
    if ( minh == -9 || maxh == -9 )
    {
        sprintf( errmsg, "Error: The minimum (%d) or maximum (%d) horizontal "
            "tile locations are not valid.\n", minh, maxh );
        ErrorHandler( FALSE, "SortProducts", ERROR_GENERAL, errmsg );
        return MOSAIC_ERROR;
    }
    else if ( minv == -9 || maxv == -9 )
    {
        sprintf( errmsg, "Error: The minimum (%d) or maximum (%d) vertical "
            "tile locations are not valid.\n", minv, maxv );
        ErrorHandler( FALSE, "SortProducts", ERROR_GENERAL, errmsg );
        return MOSAIC_ERROR;
    }

    /* Determine the number of horizontal and vertical tiles in the mosaic */
    *numh_tiles = maxh - minh + 1;
    *numv_tiles = maxv - minv + 1;

    /* Allocate space for the 2D tile array */
    *tile_array = ( int ** ) calloc ( *numv_tiles, sizeof( int * ) );
    if ( *tile_array == NULL )
    {
        sprintf( errmsg, "Error allocating vertical tile space for the "
            "tile_array\n" );
        ErrorHandler( FALSE, "SortProducts", ERROR_GENERAL, errmsg );
        return MOSAIC_ERROR;
    }
    tmp_tile_array = *tile_array;

    for ( v = 0; v < *numv_tiles; v++ )
    {
        tmp_tile_array[v] = ( int * ) calloc ( *numh_tiles, sizeof( int ) );
        if ( tmp_tile_array[v] == NULL )
        {
            sprintf( errmsg, "Error allocating horizontal tile space for the "
                "tile_array\n" );
            ErrorHandler( FALSE, "SortProducts", ERROR_GENERAL, errmsg );
            return MOSAIC_ERROR;
        }
    }

    /* Initialize the tile locations to -9 */
    for ( v = 0; v < *numv_tiles; v++ )
        for ( h = 0; h < *numh_tiles; h++ )
            tmp_tile_array[v][h] = -9;

    /* Determine which filename represents each tile location in the mosaic */
    for ( ni = 0; ni < num_infiles; ni++ )
    {
        v = infiles[ni].vert - minv;
        h = infiles[ni].horiz - minh;
        tmp_tile_array[v][h] = ni;
    }

    /* Initialize the mosaic file descriptor */
    InitializeMosaicDescriptor( mosaicfile );

    /* Create a file descriptor for the output mosaic */
    /* copy the first input file for everything except the corner coords */
    CopyMosaicDescriptor( &infiles[0], output_filename, mosaicfile );

    /* Get the bounding coords.  Must search for the minimum lat/long value,
       instead of just using the min/max h/v tiles. */
    mosaicfile->north_bound = -90.0;
    mosaicfile->south_bound = 90.0;
    mosaicfile->east_bound = -180.0;
    mosaicfile->west_bound = 180.0;
    for ( ni = 0; ni < num_infiles; ni++ )
    {
        if ( infiles[ni].north_bound > mosaicfile->north_bound )
            mosaicfile->north_bound = infiles[ni].north_bound;
        if ( infiles[ni].south_bound < mosaicfile->south_bound )
            mosaicfile->south_bound = infiles[ni].south_bound;
        if ( infiles[ni].east_bound > mosaicfile->east_bound )
            mosaicfile->east_bound = infiles[ni].east_bound;
        if ( infiles[ni].west_bound < mosaicfile->west_bound )
            mosaicfile->west_bound = infiles[ni].west_bound;
    }

    /* Get the image extents in meters, based on the min/max h/v tiles */
    mosaicfile->proj_image_extent[UL][0] =
        infiles[index_minh].proj_image_extent[UL][0];
    mosaicfile->proj_image_extent[UL][1] = 
        infiles[index_minv].proj_image_extent[UL][1];

    mosaicfile->proj_image_extent[UR][0] =
        infiles[index_maxh].proj_image_extent[UR][0];
    mosaicfile->proj_image_extent[UR][1] = 
        infiles[index_minv].proj_image_extent[UR][1];

    mosaicfile->proj_image_extent[LL][0] =
        infiles[index_minh].proj_image_extent[LL][0];
    mosaicfile->proj_image_extent[LL][1] = 
        infiles[index_maxv].proj_image_extent[LL][1];

    mosaicfile->proj_image_extent[LR][0] =
        infiles[index_maxh].proj_image_extent[LR][0];
    mosaicfile->proj_image_extent[LR][1] = 
        infiles[index_maxv].proj_image_extent[LR][1];

    /* Convert the image extents from meters to lat/long for the four
       mosaic corner points */
    bounding_coord = FALSE;
    modis_tile = FALSE;
    for ( j = 0; j < 4; j++ )
    {
        /* Convert corner points from projection coordinates to lat/long.
           If GCTP_ERANGE is returned from GetInputGeoCorner, then use the
           bounding rectangle coordinates for all corner points. */
        status = GetInputGeoCornerMosaic( mosaicfile,
             mosaicfile->proj_image_extent[j][0],
             mosaicfile->proj_image_extent[j][1],
             &mosaicfile->ll_image_extent[j][0],
             &mosaicfile->ll_image_extent[j][1] );
        if ( status == GCTP_ERANGE || status == IN_BREAK )
        {
            /* If any corners fall outside the bounds, then use the bounding
               rectangle coordinates for all corner points. If dealing with
               SIN or ISIN then just bound the longitude at -180 and use the
               latitude from GCTP. */
            bounding_coord = TRUE;
            if (mosaicfile->projection_type == PROJ_SIN ||
                mosaicfile->projection_type == PROJ_ISIN)
            {
                MessageHandler( "SortProducts", 
                    "Corner point falls outside the bounds of "
                    "the input projection.  The rectangle will be bounded "
                    "at -180 or 180 degrees latitude." );

                /* If there was an issue inverse mapping this coordinate
                   then the coordinate likely wrapped around to the other
                   side of the Earth.  We want to bound at 180 and use the
                   opposite sign of the wrap-around longitude value. */
                modis_tile = TRUE;
                if (mosaicfile->ll_image_extent[j][1] > 0)
                    mosaicfile->ll_image_extent[j][1] = -BOUND_LONG;
                else
                    mosaicfile->ll_image_extent[j][1] = BOUND_LONG;
            }
            else
            {
                MessageHandler( "SortProducts", 
                    "At least one corner point falls outside the bounds of "
                    "the input projection.  The bounding rectangular "
                    "coordinates from the metadata will be used for the "
                    "lat/long in the header." );
                break;  /* just break out and use bounding coords */
            }
        }
    }

    /* If a bounding coordinate exists then use the bounding coord
       lat/long values available in the HDF metadata */
    if ( bounding_coord && !modis_tile )
    {
        /* If bounding coords are not available (i.e. raw binary file) then
           output an error message.  The lat/long values cannot be found. */
        if ( mosaicfile->north_bound == 0.0 &&
             mosaicfile->south_bound == 0.0 &&
             mosaicfile->east_bound == 0.0 &&
             mosaicfile->west_bound == 0.0 )
        {
            ErrorHandler( TRUE, "SortProducts", ERROR_GENERAL,
                "Error when converting the corners from projection x/y to "
                "latitude/longitude.  This image set contains a bounding "
                "coordinate and the lat/long bounding coordinate values are "
                "not available for the data products.  Bounding coordinates "
                "are not supported in raw binary data files." );
        }

        mosaicfile->ll_image_extent[UL][0] = mosaicfile->north_bound; 
        mosaicfile->ll_image_extent[UL][1] = mosaicfile->west_bound;
        mosaicfile->ll_image_extent[UR][0] = mosaicfile->north_bound; 
        mosaicfile->ll_image_extent[UR][1] = mosaicfile->east_bound;
        mosaicfile->ll_image_extent[LL][0] = mosaicfile->south_bound; 
        mosaicfile->ll_image_extent[LL][1] = mosaicfile->west_bound;
        mosaicfile->ll_image_extent[LR][0] = mosaicfile->south_bound; 
        mosaicfile->ll_image_extent[LR][1] = mosaicfile->east_bound;
    }

    /* Set the number of lines and samples to be for the entire mosaic */
    for ( i = 0; i < mosaicfile->nbands; i++ )
    {
        mosaicfile->bandinfo[i].nlines *= *numv_tiles;
        mosaicfile->bandinfo[i].nsamples *= *numh_tiles;
    }

    /* Done sorting */
    return MOSAIC_SUCCESS;
}

/******************************************************************************

MODULE:  MosaicTiles

PURPOSE:  Mosaic the tiles together, filling in empty tiles with the
          background fill value in the first tile.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
MOSAIC_SUCCESS  Successful completion
MOSAIC_ERROR    Error in processing

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         07/02  Gail Schmidt           Original Development

NOTES:

******************************************************************************/
int MosaicTiles
(
    int numh_tiles,      /* I: number of horiz tiles in the mosaic */
    int numv_tiles,      /* I: number of vert tiles in the mosaic */
    int **tile_array,    /* I: 2D array of size [numv_tiles][numh_tiles]
                               specifying which input file represents that
                               tile location */
    int num_infiles,     /* I: number of input files */
    MosaicDescriptor infiles[],
                         /* I: file descriptor array for the input files */
    MosaicDescriptor *mosaicfile
                         /* I: file descriptor for the output mosaic file */
)

{
    int status = MRT_NO_ERROR;          /* return status error code */
    int v, h;                       /* looping variables */
    size_t curband, k,              /* looping variables */
        currow, curcol;
    size_t nrows = 0;               /* number of rows for current band */
    int outmulti_band = 0;          /* index for band num in the output image */
    int outcol;                     /* output column value for current row */
    int curr_infile;                /* location in infiles of the current
                                       tile */
    char errstr[SMALL_STRING];      /* string for error messages */
    int change_resolution;          /* determines when to open a new output
                                       file */
    FileDescriptor **input = NULL,  /* [array of] raw binary file descriptors
                                       for one row of input */
                   *output = NULL;  /* raw binary file descriptor for output */
    HdfEosFD **input_hdfptr = NULL, /* [array of] HDF-EOS file pointers for
                                       each input tile */
             *output_hdfptr = NULL; /* HDF-EOS file pointer for output */
    double curr_resolution = 0.0;   /* determines when to open a new input
                                       file */
    double *buffer = NULL;          /* output buffer */

    /* allocate space in input to hold one row (numh_tiles) file descriptors */
    input = ( FileDescriptor ** )
        calloc( numh_tiles, sizeof( FileDescriptor * ) );
    if ( input == NULL )
    {
        ErrorHandler( TRUE, "MosaicTiles", ERROR_MEMORY,
            "Error allocating memory for the horizontal input tiles" );
    }

    /* allocate space to hold all the input file descriptors */
    input_hdfptr = ( HdfEosFD ** )calloc( num_infiles, sizeof( HdfEosFD * ) );
    if ( input_hdfptr == NULL )
    {
        ErrorHandler( TRUE, "MosaicTiles", ERROR_MEMORY,
            "Error allocating memory for the horizontal input tiles" );
    }

    /* open the output HDF-EOS file (stays open until all input files are
       read and output) */
    if ( mosaicfile->filetype == HDFEOS )
    {
        output_hdfptr = OpenHdfEosFile( "", mosaicfile->filename, 
            FILE_WRITE_MODE, &status );
        if ( output_hdfptr == NULL )
        {
            sprintf( errstr, "Error opening output image %s.",
                mosaicfile->filename );
            ErrorHandler( TRUE, "MosaicTiles", ERROR_MEMORY, errstr );
        }
    }

    /* loop through the bands in the mosaicfile */
    for ( curband = 0; curband < mosaicfile->nbands; curband++ )
    {
        /* always start by assuming no change in resolution */
        change_resolution = FALSE;

        /* if band is not selected, continue to next band */
        if ( !mosaicfile->bandinfo[curband].selected )
            continue;

        /* allocate space for the output row buffer */
        buffer = ( double * ) calloc( mosaicfile->bandinfo[curband].nsamples,
            sizeof( double ) );
        if ( buffer == NULL )
        {
            ErrorHandler( TRUE, "MosaicTiles", ERROR_MEMORY,
                "Error allocating space for the input/output row buffer" );
        }

        MessageHandler( "\nMosaic", "processing band %s",
            mosaicfile->bandinfo[curband].name );

        /* track rez changes for HDF-EOS grids */
        if ( curr_resolution != mosaicfile->bandinfo[curband].pixel_size )
        {
            curr_resolution = mosaicfile->bandinfo[curband].pixel_size;
            change_resolution = TRUE;
        }

        /* open corresponding output file/grid/band/field */
        switch ( mosaicfile->filetype )
        {
            case RAW_BINARY:
                output = OpenOutImageMosaic( mosaicfile, curband, outmulti_band,
                    &status );
                break;

            case HDFEOS:
                output = MakeHdfEosFDMosaic( mosaicfile, output_hdfptr,
                    FILE_WRITE_MODE, curband, outmulti_band, &status );
                if ( output == NULL )
                {
                    ErrorHandler( TRUE, "MosaicTiles", ERROR_OPEN_OUTPUTIMAGE,
                        "Error creating the output file descriptor" );
                }

                if ( change_resolution )
                {
                    /* open new grid (old grid is closed if necessary) */
                    output->bandnum = outmulti_band = 1;
                    if ( CreateHdfEosGridMosaic( mosaicfile, curband, output )
                        != 0 )
                    {
                        ErrorHandler( TRUE, "MosaicTiles",
                            ERROR_OPEN_OUTPUTIMAGE,
                            "Error while creating grid in output file" );
                    }
                }

                if ( CreateHdfEosFieldMosaic( mosaicfile, curband, output )
                    != 0 )
                {
                    ErrorHandler( TRUE, "MosaicTiles",
                        ERROR_OPEN_OUTPUTIMAGE,
                        "Error while creating field in output file" );
                }
                break;

            default:
                ErrorHandler( TRUE, "MosaicTiles", ERROR_GENERAL,
                    "Bad File Type" );
                break;
        }

        /* loop through the vertical tiles */
        for ( v = 0; v < numv_tiles; v++ )
        {
            /* open all the horizontal tiles for this vertical set */
            for ( h = 0; h < numh_tiles; h++ )
            {
                /* open the corresponding file if one exists */
                if ( tile_array[v][h] != -9 )
                {
                    /* open input file/grid/band/field */
                    curr_infile = tile_array[v][h];

                    switch ( mosaicfile->filetype )
                    {
                        case RAW_BINARY:
                            /* open input file */
                            input[h] = OpenInImageMosaic( &infiles[curr_infile],
                                curband, curband, &status );
                            if ( input[h] == NULL )
                            {
                                sprintf( errstr,
                                    "Error opening raw binary image: %s\n",
                                    infiles[curr_infile].filename );
                                ErrorHandler( TRUE, "MosaicTiles",
                                    ERROR_OPEN_INPUTIMAGE, errstr );
                            }
                            nrows = input[h]->nrows;

                            /* clear buffers to avoid reading data from
                               previous band */
                            ClobberFileBuffers( input[h] );
                            break;

                        case HDFEOS:
                            /* open the input HDF-EOS file */
                            input_hdfptr[curr_infile] = OpenHdfEosFile(
                                infiles[curr_infile].filename, "",
                                FILE_READ_MODE, &status );
                            if ( input_hdfptr[curr_infile] == NULL )
                            {
                                sprintf( errstr,
                                    "Error opening input image %s.",
                                    infiles[curr_infile].filename );
                                ErrorHandler( TRUE, "MosaicTiles",
                                    ERROR_MEMORY, errstr );
                            }

                            /* create a file descriptor */
                            input[h] = MakeHdfEosFDMosaic(
                                &infiles[curr_infile],
                                input_hdfptr[curr_infile], FILE_READ_MODE,
                                curband, curband, &status );
                            if ( input[h] == NULL )
                            {
                                sprintf( errstr,
                                    "Error opening HDF-EOS image: %s\n",
                                    infiles[curr_infile].filename );
                                ErrorHandler( TRUE, "MosaicTiles",
                                    ERROR_OPEN_INPUTIMAGE, errstr );
                            }
                            nrows = input[h]->nrows;

                            GetHdfEosFieldMosaic( input_hdfptr[curr_infile],
                                curband );
                            break;

                        default:
                            ErrorHandler( TRUE, "MosaicTiles", ERROR_GENERAL,
                                "Bad Input File Type" );
                            break;
                    }

                    if ( status != MRT_NO_ERROR )
                    {
                        sprintf( errstr, "Error opening input file: %s\n",
                            infiles[curr_infile].filename );
                        ErrorHandler( TRUE, "MosaicTiles",
                            ERROR_OPEN_INPUTIMAGE, errstr );
                    }
                }
            }  /* for h */

            /* initialize status to terminal */
            fprintf( stdout, "%% complete (" MRT_SIZE_T_FMT " rows): 0%%",
                     nrows );
            fflush( stdout );
            k = 0;

            /* loop through the rows reading the current row for each image
               then output the mosaicked row to the output file */
            for ( currow = 0; currow < nrows; currow++ )
            {
                /* update status */
                if ( 100 * currow / nrows > k )
                {
                    k = 100 * currow / nrows;
                    if ( k % 10 == 0 )
                    {
                        fprintf( stdout, " " MRT_SIZE_T_FMT "%%", k );
                        fflush( stdout );
                    }
                }

                /* initialize the output column value */
                outcol = 0;

                /* loop through the horizontal tiles, read the input data and
                   write it to output */
                for ( h = 0; h < numh_tiles; h++ )
                {
                    /* if this tile exists, then read it from the correct
                       input file and fill in the output file */
                    if ( tile_array[v][h] != -9 )
                    {
                        for ( curcol = 0; curcol < input[h]->ncols; curcol++ )
                        {
                            /* read the input file */
                            buffer[outcol++] = ReadBufferValue( curcol, currow,
                                input[h] );
                        }
                    }

                    /* otherwise fill this tile with background fill values;
                       use information from the first input file */
                    else
                    {
                        FillBufferBackground( &mosaicfile->bandinfo[curband],
                            numh_tiles, &buffer[outcol] );
                        outcol += mosaicfile->bandinfo[curband].nsamples /
                            numh_tiles;
                    }
                } /* for h */

                /* write the mosaic buffer to the output file */
                if ( !WriteRow( output, currow + v * nrows, buffer ) )
                {
                    free( buffer );
                    ErrorHandler( TRUE, "MosaicTiles", ERROR_GENERAL,
                        "Error writing the mosaicked row to the output file.");
                }
            }  /* for currow */

            fprintf( stdout, " 100%%\n" );
            fflush( stdout );

            /* close all the horizontal files */
            for ( h = 0; h < numh_tiles; h++ )
            {
                if ( tile_array[v][h] != -9 )
                {
                    /* close input file */
                    switch ( mosaicfile->filetype )
                    {
                        case RAW_BINARY:
                            CloseFile( input[h] );
                            break;

                        case HDFEOS:
                            CloseHdfEosFile( input[h] );
                            curr_infile = tile_array[v][h];
                            CloseHdfEos( input_hdfptr[curr_infile] );
                            break;
                    }
                }
            }   /* for h */
        }   /* for v */

        /* close raw binary and HDF-EOS output files and update the band
           number */
        switch ( mosaicfile->filetype )
        {
            case RAW_BINARY:
                CloseFile( output );
                outmulti_band++;
                break;

            case HDFEOS:
                DestroyFileDescriptor( output );
                if ( !change_resolution )
                    outmulti_band++;
                break;
        }
        output = NULL;

        /* free the buffer for this band */
        if ( buffer != NULL )
        {
           free( buffer );
           buffer = NULL;
        }
    }   /* for curband */

    /* close output HDF-EOS file */
    if ( mosaicfile->filetype == HDFEOS )
    {
        CloseHdfEos( output_hdfptr );
    }

    /* free space for the input FileDescriptor pointers */
    free( input );
    input = NULL;
    free( input_hdfptr );
    input_hdfptr = NULL;

    return MOSAIC_SUCCESS;
}


/******************************************************************************

MODULE:  FillBufferBackground

PURPOSE:  Fill the buffer with background fill values for the current image.

RETURN VALUE:
Type = None

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         07/02  Gail Schmidt           Original Development

NOTES:

******************************************************************************/
void FillBufferBackground
(
    BandType *bandinfo,     /* I: mosaic band information for current band */
    int numh_tiles,         /* I: number of horizontal tiles */
    double *buffer          /* O: output buffer */
)

{
    int curcol;

    /* loop through the sample values filling with the background fill
       NOTE: the mosaic band information will have nsamples for the entire
       mosaic so it must be divided by the number of horizontal tiles to get
       the number of samples in an individual tile */
    for ( curcol = 0; curcol < bandinfo->nsamples / numh_tiles; curcol++ )
    {
        buffer[curcol] = bandinfo->background_fill;
    }

    return;
}


/******************************************************************************

MODULE:  ExpandEnvironment

PURPOSE: Expand environment variables found as $(envvar) within the string. 

RETURN VALUE:
Type = 0 successful
       1 various errors
       2 environment variable could not be found.

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         09/07  Maverick Merritt       Original Development

NOTES:

******************************************************************************/
int ExpandEnvironment( char *line, size_t max_linelen ) {
    int i, j;
    int found;
    int err = 0;
    char *tptr;
    char *tmpstr;

    tmpstr = (char *)malloc( (max_linelen + 1) * sizeof(char) );
    if( !tmpstr ) {
       return 1;
    }

    for( i = 0; line[i] != '\0' && !err; ++i ) {
       if( line[i] == '$' && line[i+1] == '(' ) {
          j = i + 1;
          if( line[j + 1] == '\0' ) {
             err = 1;
             break;
          }
          found = FALSE;
          for( j = j + 1; line[j] != '\0' && err == 0; ++j )
             if( line[j] == ')' ) {
                found = TRUE;
                break;
             }
          if( found ) {
             char *envptr;
             size_t envnamelen = j - i + 1;
             strncpy( tmpstr, &line[i+2], envnamelen - 3 );
             tmpstr[envnamelen - 3] = '\0';
             envptr = getenv(tmpstr);
             if( envptr == NULL ) {
                strcpy( line, tmpstr );
                err = 2;
             } else {
                size_t envlen = strlen(envptr);
                if(strlen(line) - envnamelen + envlen < max_linelen) {
                   strncpy( tmpstr, line, i );
                   tmpstr[i] = 0;
                   strcat( tmpstr, envptr );
                   strcat( tmpstr, &line[i + envnamelen] );
                   strcpy( line, tmpstr );
                   i = i + envlen;
                } else
                   err = 1;
             }
          } else
             err = 1;
       }
    }

    /* The following code tries to determine what path seperator to use.  This
     * is primarily used for the test scripts that will, more than likely,
     * use an environment variable to state the beginning PATH to the input
     * and output file names.  Given this, then, if the file name has a colon,
     * use the backslash, and if failing that, use the first separator to fill
     * in the rest.
     */
     if( !err ) {
        tptr = strchr( line, ':' );
        if( tptr ) {
           for( i = 0; line[i] != '\0'; ++i )
              if( line[i] == '/' )
                 line[i] = '\\';
        } else {
           char tchar = 0;
           for( i = 0; line[i] != '\0'; ++i )
              if( !tchar ) {
                 if( line[i] == '/' ) {
                    tchar = '/';
                    found = TRUE;
                 } else if( line[i] == '\\' ) {
                    tchar = '\\';
                    found = TRUE;
                 }
              } else {
                 /* The following code could be a little more efficeint. */
                 if( line[i] == '/' || line[i] == '\\' )
                    line[i] = tchar;
              }
        }
    }

    free(tmpstr);
    return err;  
}


/******************************************************************************

MODULE:  EstimateFileSize

PURPOSE: Estimate the output file size.  This does not take into account
         the extra file overhead related to the file format, such as the
         metadata added to HDF and Geotiff.

RETURN VALUE:
Type = The estimated file size of the output file in a 64-bit unsigned.

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         09/07  Maverick Merritt       Original Development

NOTES:

******************************************************************************/
MRT_UINT64 EstimateFileSize( MosaicDescriptor *mosaicfile ){
   MRT_UINT64 efs = 0;
   MRT_UINT64 bs;
   size_t curband;

   /* The output file's nlines and nsamples for each band is the same for
    * all output bands in the array.  The nlines and nsamples indicate the
    * total data for the output image.
    */
   
   for( curband = 0; curband < mosaicfile->nbands; ++curband ) {
      if( !mosaicfile->bandinfo[curband].selected )
         continue;

      bs = mosaicfile->bandinfo[curband].nlines *
            mosaicfile->bandinfo[curband].nsamples;
      switch ( mosaicfile->bandinfo[curband].output_datatype ) {
         case DFNT_INT8:
            bs *= sizeof(MRT_INT8);
            break;
         case DFNT_UINT8:
            bs *= sizeof(MRT_UINT8);
            break;
         case DFNT_INT16:
            bs *= sizeof(MRT_INT16);
            break;
         case DFNT_UINT16:
            bs *= sizeof(MRT_UINT16);
            break;
         case DFNT_INT32:
            bs *= sizeof(MRT_INT32);
            break;
         case DFNT_UINT32:
            bs *= sizeof(MRT_UINT32);
            break;
         case DFNT_FLOAT32:
            bs *= sizeof(MRT_FLOAT4);
            break;
      }
      efs += bs;
   }
   return efs;
}

/******************************************************************************

MODULE:  freeInputFileNameList

PURPOSE: Frees allocated memory produced by the getInputFileNamesFromFile()
         function.

RETURN VALUE:
Type =

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         09/01  Maverick Merritt       Original Development

NOTES:

******************************************************************************/
void freeInputFileNameList( char **filelist, int nfiles ) {
   int i;
   if( filelist ) {
      for( i = 0; i < nfiles; ++i )
         free( filelist[i] );
      free( filelist );
   }
}

/******************************************************************************

MODULE:  getInputFileNamesFromFile

PURPOSE: Gets filenames from a file and stores then in a dynamic array that
         needs to be freed by the calling function, if there was no error.
         Can handle quoted or non-quoted filenames.  Non-quoted filenames
         cannot contain any spaces.  Multiple filenames can be specified per
         line.

RETURN VALUE:
Type = 0 successful
       1 error while reading file.
       2 syntax error, no closing quote found, filename to big.
	   3 memory allocation error.
	   4 internal error.

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         09/01  Maverick Merritt       Original Development

NOTES: Use freeInputFileNameList() to free the allocated memory.

******************************************************************************/
int getInputFileNamesFromFile( FILE * ifile, char ***filelist, int *n ) {
   char rtmpstr[HUGE_STRING];
   int nfiles = 0;
   char *tptr, *ptr, *endqptr;
   char **flist = 0;
   size_t len;

   while( 1 ) {               /* End of file will break loop */
      ptr = fgets( rtmpstr, HUGE_STRING, ifile );
      if( ptr == NULL ) {
         if( feof(ifile) ) {
            *filelist = flist;
            *n = nfiles;
            return 0;
         }
         freeInputFileNameList( flist, nfiles );
         return 1;            /* Read error */
      }
      ptr = rtmpstr;
      while( 1 ) {             /* End of line will break loop */
         while( isspace(*ptr) && *ptr != 0 ) ++ptr;
         if( *ptr == 0 )       /* End of line, break */
            break;
         if( *ptr == '"' ) {
            tptr = ptr;
            ++ptr;
            while( *ptr != '"' && *ptr != 0 ) ++ptr;
            if( *ptr == 0 )    /* No closing quote, error */
               return 2;
            endqptr = ptr;
            --ptr;
            while( ptr != tptr && isspace(*ptr) ) --ptr;
            if( ptr == tptr ) {
               ptr = endqptr + 1;
               continue;      /* all spaces or empty string */
            }
            ++ptr; ++tptr;
            while( isspace(*tptr) && *tptr != 0 ) ++tptr;
            len = ptr - tptr;
            ptr = endqptr + 1;
         } else {
            tptr = ptr;
            while( !isspace(*ptr) && *ptr != 0 && *ptr != '"' ) ++ptr;
            len = ptr - tptr;
         }
         if( len > 0 ) {
            char *str;
            if( len + 1 > FILENAME_LENGTH ) {
               freeInputFileNameList( flist, nfiles );
               return 2;
            }
            if( flist == 0 ) {
               flist = (char **)malloc( sizeof(char *) );
               if( flist == 0 )
                  return 3;
            } else {
               char **tmplist = flist;
               flist = (char **)realloc( flist, (nfiles + 1) * sizeof(char *) );
               if( flist == 0 ) {
                  flist = tmplist;
                  freeInputFileNameList( flist, nfiles );
                  return 3;
               }
            }
            str = (char *)malloc( (len + 1) * sizeof(char) );
            if( str == 0 ) {
               freeInputFileNameList( flist, nfiles );
               return 3;
            }
            strncpy( str, tptr, len );
            str[len] = 0;
            flist[nfiles] = str;
            ++nfiles;
         }
      }
   }
   /* Should not get here */
   freeInputFileNameList( flist, nfiles );
   return 4;
}

