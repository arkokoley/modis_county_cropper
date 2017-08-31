
/******************************************************************************

FILE:  usage.c

PURPOSE:  Print resampler usage info

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         04/00  John Weiss             Original Development

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/
#include <stdio.h>
#include "shared_resample.h"

/******************************************************************************

MODULE:  Usage

PURPOSE:  Print resampler usage info to terminal

RETURN VALUE:
Type = none
Value           Description
-----           -----------

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         04/00  John Weiss             Original Development
         01/01  John Rishea            Standardized formatting
  
NOTES:

******************************************************************************/

void Usage
(
    void 
)

{
    fprintf( stderr, "\n%s %s\n\n", RESAMPLER_NAME, RESAMPLER_VERSION );
    fprintf( stderr, "Usage: resample -p parameter_file [options]\n\n" );
    fprintf( stderr, "Options that override parameter file specifications:\n" );
    fprintf( stderr, "   -i input_file_name\n" );
    fprintf( stderr, "   -o output_file_name\n" );
    fprintf( stderr, "   -r resampling_type [NN BI CC NONE]\n" );
    fprintf( stderr, "   -t projection_type [AEA ER GEO HAM IGH ISIN LA LCC "
        "MERCAT MOL PS SIN TM UTM]\n" );
    fprintf( stderr, "   -j projection_parameter_list \"p1 p2 ... p15\"\n" );
    fprintf( stderr, "   -s spectral_subset \"b1 b2 ... bN\"\n" );
    fprintf( stderr, "   If using the -s switch, the SDSs should be "
        "represented as an\n"
        "   array of 0s and 1s. A '1' specifies to process that SDS;\n"
        "   '0' specifies to skip that SDS. Unspecified SDSs will not be "
        "processed.\n"
        "   If the -s switch is not specified, then all SDSs will be "
        "processed.\n");
    fprintf( stderr, "   -a spatial_subset_type [INPUT_LAT_LONG "
        "INPUT_LINE_SAMPLE OUTPUT_PROJ_COORDS]\n" );
    fprintf( stderr, "   -l spatial_subset \"ULlat ULlong LRlat LRlong\"\n" );
    fprintf( stderr, "               -or-  \"ULline ULsample LRline LRsample (0-based)\"\n" );
    fprintf( stderr, "               -or-  \"ULprojx ULprojy LRprojx LRprojy\"\n" );
    fprintf( stderr, "      NOTE: line/sample must be specified for the\n"
                     "            highest resolution of all SDSs specified\n"
		     "            to be processed in the product.\n");
    fprintf( stderr, "   -u UTM_zone\n" );
    fprintf( stderr, "   -x pixel_size\n" );
    fprintf( stderr, "   -g filename for the log file\n" );
    fprintf( stderr, "\n" );
    fprintf( stderr, "Usage: resample -h file.hdf\n" );
    fprintf( stderr, "       creates raw binary header file TmpHdr.hdr\n" );
    fprintf( stderr, "\n" );
}


/******************************************************************************

MODULE:  MosaicUsage

PURPOSE:  Print mosaic usage info to terminal

RETURN VALUE:
Type = none
Value           Description
-----           -----------

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         03/02  Gail Schmidt           Original Development

NOTES:

******************************************************************************/

void MosaicUsage
(
    void
)
{
    fprintf( stderr,
        "Usage: mrtmosaic -i input_filenames_file -t -h -o output_filename\n" );
    fprintf( stderr,
        "                 -s spectral_subset \"b1 b2 ... bN\"\n" );
    fprintf( stderr,
        "                 -g filename for the log file\n" );
    fprintf( stderr,
        "   where input_filenames_file is a text file which contains the\n"
        "   names of the files to be mosaicked.\n"
        "   If using the -s switch, the SDSs should be represented as an\n"
        "   array of 0s and 1s. A '1' specifies to process that SDS;\n"
        "   '0' specifies to skip that SDS. Unspecified SDSs will not be "
        "processed.\n"
        "   If the -s switch is not specified, then all SDSs will be "
        "processed.\n"
        "   If -t is specified then the tile locations of the input\n"
        "   filenames are output to tile.txt (-o, -s, and -h are not needed).\n"
        "   Raw binary files must specify the tile locations in the filename\n"
        "   to be used with the -t switch (i.e mod09ghk_h02v16.hdr).\n"
        "   If -h is specified then the mosaicked header information will\n"
        "   be output to TmpHdr.hdr (-o, -s, and -t are not needed).\n"
        "   NOTE: Only input Sinusoidal and Integerized Sinusoidal\n"
        "   projections are supported for mosaicking.\n" );
    fprintf( stderr, "\n" );
    fprintf( stderr, "Example: mrtmosaic -i TmpMosaic.prm -s \"1 1 0 1\" "
        "-o mosaic.hdf\n"
        "   This will mosaic the first, second, and fourth SDSs in each of\n"
        "   the specified HDF files in TmpMosaic.prm.\n\n");
}


/******************************************************************************

MODULE:  HDF2RB_Usage

PURPOSE:  Print usage info to terminal

RETURN VALUE:
Type = none
Value           Description
-----           -----------

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         11/05  Gail Schmidt           Original Development
  
NOTES:

******************************************************************************/

void HDF2RB_Usage
(
    void 
)

{
    fprintf( stderr, "\n%s %s\n\n", HDF2RB_NAME, HDF2RB_VERSION );
    fprintf( stderr, "Usage: hdf2rb -p parameter_file [options]\n\n" );
    fprintf( stderr, "Options that override parameter file specifications:\n" );
    fprintf( stderr, "   -i input_file_name (only HDF format is supported)\n" );
    fprintf( stderr, "   -o output_file_name (only raw binary format is "
        "supported)\n" );
    fprintf( stderr, "   -s spectral_subset \"b1 b2 ... bN\"\n" );
    fprintf( stderr, "   If using the -s switch, the SDSs should be "
        "represented as an\n"
        "   array of 0s and 1s. A '1' specifies to process that SDS;\n"
        "   '0' specifies to skip that SDS. Unspecified SDSs will not be "
        "processed.\n"
        "   If the -s switch is not specified, then all SDSs will be "
        "processed.\n");
    fprintf( stderr, "   -l spatial_subset \"ULline ULsample LRline "
        "LRsample (0-based)\"\n" );
    fprintf( stderr, "   Only input line/sample values are supported for "
        "spatial subsetting.  Use line/sample values for highest resolution "
        "selected SDS, if multiple resolution SDSs are to be processed.\n");
    fprintf( stderr, "   -g filename for the log file\n" );
    fprintf( stderr, "\n" );
}
