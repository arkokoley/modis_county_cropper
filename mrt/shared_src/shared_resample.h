/******************************************************************************

FILE:  shared_resample.h

PURPOSE:  Type definitions and const values

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  Rob Burrell            
         01/01  John Rishea            Added #defines for char str lengths
         01/01  John Rishea            Standardized formatting
         04/01  Rob Burrell            Small change to Cubic params
         04/01  Rob Burrell            Add ellipse enum types
         04/02  Gail Schmidt           Changed data pointers from floats to
                                       doubles
         11/05  Gail Schmidt           Added #defines for HDF2RB

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
    __DJGPP__ is included for DOS

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/

#ifndef _SHARED_RESAMPLE_H_
#define _SHARED_RESAMPLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mrt_isoc.h"

/* #define DEBUG 1 */

#define RESAMPLER_NAME    "MODIS Reprojection Tool"
#define RESAMPLER_VERSION "v4.1 March 2009"

#define HDF2RB_NAME    "HDF to Raw Binary Tool"
#define HDF2RB_VERSION "v4.1 March 2009"

/* character string lengths */
#define SMALL_STRING		256
#define LARGE_STRING		1024
#define HUGE_STRING		4096
#define FILENAME_LENGTH         LARGE_STRING

#include "mrt_error.h"		/* error codes and protos */
#include "cproj.h"              /* gctp projection consts */
#include "datum.h"              /* gctp datum consts */

/* this will stop the inclusion of hdf conflicts in geotiff.c */
#ifndef _TIFF_
#include "hdf.h"
#include "mfhdf.h"
#include "HdfEosDef.h"
#endif

/* Uncomment to produce verbose stdout/stderr */
/* #define MODIS_DEBUG */

/* resample logging defs */
#define ERRORMSG_LOGFILE_OPEN "Unable to open log file"
#define MAX_MESSAGE_LENGTH 1024
#define EMPTY_VALUE 0.0f
#define MAX_BUFFER_SIZE 33554432	/* 32 MB */
#define NUM_PROJECTION_PARAMS 15

/* Cubic resampler constants */
/* subpixel steps to include in the kernel weights */
#define SUBPIXEL_STEPS 32
#define DOUBLE_SUBPIXEL_STEPS 32.0

/* This is one more than the number of SUBPIXEL steps 
 * since the fractional line number is rounded to the
 * nearest 1/32 subpixel in the resampler.  To avoid 
 * having to check for rounding up to the next whole 
 * line number, the extra subpixel step is included in 
 * the table. */
#define SUBPIXEL_TABLE_ENTRIES (SUBPIXEL_STEPS + 1)

/* normal cubic convolution kernel size */
#define LINES_IN_KERNEL 4
#define TOP_NUM_LINES (LINES_IN_KERNEL - (LINES_IN_KERNEL/2) - 1)
#define SAMPLES_IN_KERNEL 4
#define LEFT_NUM_SAMPLES (SAMPLES_IN_KERNEL - (SAMPLES_IN_KERNEL/2) - 1)
#define KERNEL_SIZE ((LINES_IN_KERNEL) * (SAMPLES_IN_KERNEL))

#define CC_ALPHA -0.5

/* the roundoff is 1/2 the subpixel steps (half-rounding) */
#define ROUND_OFF (1.0/(DOUBLE_SUBPIXEL_STEPS*2.0))

/* longitude value to use for bounding tiles (close to 180, but not quite
   because we don't want to be on the -180/180 degree line */
#define BOUND_LONG 179.900

/*---------------
-----------------
 Type definitions
-----------------
-----------------*/
#ifndef WIN32
#ifndef FALSE
typedef enum
{
    FALSE, TRUE
}
boolean;

#else
typedef enum
{
    false, true
}
boolean;
#endif
#endif

/* Structure holding projection parameters   */
typedef struct
{
    long proj_code;		/* Projection code number                    */
    long units;			/* Projection units code number              */
    long zone_code;		/* Projection zone code (UTM or State Plane) */
    long datum_code;		/* Projection datum code                     */
    long sphere_code;		/* Projection sphere code (only for UTM)     */
    double proj_coef[15];	/* 15 projection coefficients                */
}
ProjInfo;						


/* how a file has been opened */
typedef enum
{
    BAD_FILE_MODE, FILE_WRITE_MODE, FILE_READ_MODE
}
FileOpenType;


/* type of in/out file */
typedef enum
{
    BAD_FILE_TYPE, RAW_BINARY, HDFEOS, GEOTIFF
}
FileType;


/* type of data read/write */
/* JMW 05/04/01 - no longer used */
/* use HDF DFNT_... types instead (except for BAD_DATA_TYPE value) */
/* Actually, nothing is used below except BAD_DATA_TYPE. */
typedef enum
{
    BAD_DATA_TYPE, MRT_ASCII_DATA_TYPE, MRT_INT8_DATA_TYPE,
    MRT_UINT8_DATA_TYPE, MRT_INT16_DATA_TYPE, MRT_UINT16_DATA_TYPE,
    MRT_INT32_DATA_TYPE, MRT_UINT32_DATA_TYPE, MRT_FLOAT32_DATA_TYPE
}
DataType;


/* internal projection types. if these change then make sure to change
   the ProjectionStrings in print_md.c. */
typedef enum
{
    BAD_PROJECTION_TYPE,
    PROJ_AEA, PROJ_ER, PROJ_GEO, PROJ_HAM, PROJ_IGH, PROJ_ISIN, PROJ_LA,
    PROJ_LCC, PROJ_MERC, PROJ_MOL, PROJ_PS, PROJ_SIN, PROJ_TM, PROJ_UTM
}
ProjectionType;

/* parameters in the parameter file (must match the order of HeaderStrings
   in read_prm.c) */
typedef enum
{
    INPUT_FILENAME, SPECTRAL_SUBSET, SPATIAL_SUBSET_TYPE, SPATIAL_SUBSET_UL,
    SPATIAL_SUBSET_LR, OUTPUT_FILENAME, RESAMPLING_TYPE, OUTPUT_PROJ_TYPE,
    OUTPUT_PROJ_PARMS, PIXEL_SIZE, UTM_ZONE, DATUM, NSTRINGS
}
ParamType;

/* When a raw binary file is read in, what did the header file say about what
 * endianness the raw binary data is in.  If the file read in is not raw
 * binary, then MRT_UNKNOWN_APPLICABLE should be used.
 */
typedef enum
{
    MRT_UNKNOWN_ENDIAN, MRT_BIG_ENDIAN, MRT_LITTLE_ENDIAN
} MrtEndianness;


/* internal datum types, lets keep it simple and
 * use the same values as GCTP, Geolib, etc.  
 * See datum.txt in the data directory.
 * See sphdz.c in GCTP */
/*#define E_CLARKE66     0
#define E_GRS80        8
#define E_WGS84       12
#define E_STDSPHERE   19
#define E_MODISSPHERE 31
*/
#define E_NAD27 225
#define E_NAD83 219
#define E_WGS66 315
#define E_WGS72 316
#define E_WGS84 317
#define E_NODATUM -1


/* resample methods */
typedef enum
{
    BAD_RESAMPLING_TYPE, NN, BI, CC, NO_RESAMPLE
}
ResamplingType;


/* image corner array positions (double array[??][2] */
typedef enum
{
    UL, UR, LL, LR
}
CornerType;


/* spatial subsetting options */
typedef enum
{
    BAD_SPATIAL_SUBSET_TYPE, INPUT_LAT_LONG, INPUT_LINE_SAMPLE,
    OUTPUT_PROJ_COORDS
}
SpatialSubsetType;


/* read buffer queue element definition */
typedef struct QueueType_tag
{
    struct QueueType_tag *next, *prev;  /* a doubly linked list */
    size_t row;                         /* row number, faster lookup at the
                                           top */
    double *data;                       /* data buffer */
}
QueueType;


/* read buffer queue definition (queue header) */
typedef struct QueueHdrType_tag
{
    QueueType *first, *last;  /* first and last elements */
    int numbuffers;           /* number of buffers in this queue */
    QueueType **in_cache;     /* pointers to elements,
                               * one for each row in the input file
                               * NULL => row_is_not_in_memory */
}
QueueHdrType;


/* describes the input/output file (band) */
typedef struct FileDescriptor_tag
{
    char *filename;            
    FileOpenType fileopentype;   /* opened for reading or writing */
    FileType filetype;           /* hdf, geotiff, raw binary */
    /* On read, what is Raw Binary endianness */
    MrtEndianness file_endianness;
    int datatype;                /* float, int, char, etc */
    int datasize;                /* sizeof( float, int, or char ) */
    void *fileptr;               /* HdfEosFD, GeoTIFFFD, or FILE* */
    void *rowbuffer;             /* read/write buffer */
    QueueHdrType queuetop;       /* read buffer
                                    (fileopentype == FILE_READ_MODE) */
    size_t nrows, ncols;         /* physical dimensions */
    double coord_corners[4][2];  /* projection corner coordinates */
    double pixel_size;           /* size of a pixel in projection coordinates */
    double output_pixel_size;    /* size of an output pixel in projection
                                    coordinates */
    double background_fill;      /* background fill value */
    int bandnum;                 /* current band to read/write */
}
FileDescriptor;


/* tag for an HDF-EOS file descriptor in the FileDescriptor fileptr field */
typedef struct
{
    int fid;			/* file id */
    int gid;			/* current grid id */
    int ngrids;			/* number of grids (1 grid per resolution) */
    char *gridlist;		/* comma-separated list of grid names */
    char *currgrid;		/* current grid name */
    char *fieldlist;		/* comma-separated list of fields in current
                                   grid */
    char *currfield;		/* current field name */
    /* JMW 06/13/01 - rank and dimension info for 3-D/4-D data sets */
    int rank;			/* dimensionality */
    int pos[4];			/* position of YDim, XDim, 3rdDim, 4thDim in
                                   dimension list */
    int dim3, dim4;		/* current slice and cube */
}
HdfEosFD;


/* tag for a GeoTIFF file descriptor in the FileDescriptor fileptr field */
#ifdef _TIFF_
typedef struct
{
    TIFF *tif;			/* TIFF-level descriptor */
    GTIF *gtif;			/* GeoKey-level descriptor */
}
GeoTIFFFD;
#endif


/* BandType struct to describe one image band */
typedef struct
{
    /* band name */
    char *name;

    /* image dimensions */
    int nlines, nsamples;

    /* data types (byte, int, etc.) */
    int input_datatype, output_datatype;

    /* pixel size, in meters */
    double pixel_size;
    double output_pixel_size;

    /* JMW 06/13/01 - rank and dimension info for 3-D/4-D data sets */
    int fieldnum;	/* number of field this slice is in */
    int rank;		/* dimensionality */
    int pos[4];		/* position of YDim, XDim, 3rdDim, 4thDim in dimension
                           list */

    /* store SDS attributes:
     * valid range, background fill value, scale factor, offset 
     *
     * could use void pointers to handle different data types,
     * but all float math anyway, so just use doubles
     */
    double min_value, max_value, background_fill;
    double scale_factor, offset;

    /* has band been selected for processing? */
    int selected;
}
BandType;


/* Output file information for raw binary headers */
typedef struct
{
    int nlines;            /* number of rows for this file */
    int nsamples;          /* number of cols for this file */
    double pixel_size;     /* pixel size in projection space */
    int nbands;            /* number of bands for this file */
}
OutFileType;


/* ModisDescriptor struct */
typedef struct
{
    /* various filenames */
    char *parameter_filename, *input_filename, *output_filename;

    /* input grid (Vgroup) name */
    char input_gridname[LARGE_STRING];

    /* file types (raw binary, HDF-EOS, GeoTiff) */
    FileType input_filetype, output_filetype;

    /* On read, what is Raw Binary endianness */
    MrtEndianness input_file_endian;

    /* number of bands in input image */
    size_t nbands;

    /* array[nbands] of info about each input band */
    BandType *bandinfo;

    /* input image size (raster corner points - lat/long in decimal degrees
       in input space).  filled with invalid values for HDF2RB. */
    double input_image_extent[4][2];

    /* input bounding rectangle in degrees (input space) */
    double north_bound;
    double south_bound;
    double east_bound;
    double west_bound;

    /* were the bounding rectangle coordinates used? */
    int use_bound;

    /* original projection corner coordinates, in the case of this being an
       ISIN bounding tile.  always filled for HDF2RB. */
    double orig_coord_corners[4][2];

    /* what type of subsetting is being specified? */
    SpatialSubsetType spatial_subset_type;

    /* is this really a subset or just the corners of the input image? */
    int is_subset;

    /* input spatial subset corner points (UL, UR, LL, LR in lat/long in
       input space).  filled with starting line/samp and ending line/samp,
       if specified by user.  otherwise filled with invalid values for
       HDF2RB. */
    double ll_spac_sub_gring_corners[4][2];

    /* input spatial subset corner points (UL, UR, LL, LR in x/y in input
       space) */
    double proj_spac_sub_gring_corners[4][2];

    /* highest rez input spatial subset corner points (UL, UR, LL, LR in
       row/col).  not filled for HDF2RB. */
    int input_corner_points[4][2];

    /* output image size (raster corners in lat/lon in output space).  not
       filled for HDF2RB. */
    double ll_extents_corners[4][2];
	
    /* output image size (corners x/y in output projection units) */
    double proj_extents_corners[4][2];

    /* coordinate system origin: UL, UR, LL, LR */
    CornerType coord_origin;

    /* projection types (ISIN, GEO, UTM, etc.) */
    ProjectionType input_projection_type, output_projection_type;

    /* resampling type (NN, BI, CC) */
    ResamplingType resampling_type;

    /* array of 15 projection parameters */
    double input_projection_parameters[15];
    double output_projection_parameters[15];

    /* datum codes (NAD27, NAD83, WGS66, WGS72, WGS84, NODATUM) */
    int input_datum_code, output_datum_code;
    int input_sphere_code;  /* required for UTM projections since the
                               projection parameters can't store the
                               sphere/ellipse information */

    /* UTM zone codes */
    int input_zone_code, output_zone_code;

    /* projection info structures for Geolib */
    ProjInfo *in_projection_info, *out_projection_info;

    /* file information for handling raw binary and GeoTIFF outputs */
    OutFileType *output_file_info;

    /* I'm going to insist that the output units be global, don't
     * want to have to worry about setting different units for
     * different bands */
    int output_units;
	
    /* the number of output files (raw binary and GeoTIFF) */
    int nfiles_out;

    /* command-line argument switches */
    int aswitch;  /* spatial subset type */
    int iswitch;  /* input filename */
    int jswitch;  /* output projection parameters */
    int lswitch;  /* spatial subsetting (both UL and LR) */
    int oswitch;  /* output filename */
    int rswitch;  /* resampling type */
    int sswitch;  /* spectral subsetting */
    int tswitch;  /* output projection type */
    int uswitch;  /* output UTM zone */
    int xswitch;  /* output pixel size */

    /* specify that a parameter was present either on the
       command line or in the parameter file. note that the
       specific parameters line up with the ParamType order */
    int ParamsPresent[NSTRINGS];

    /* temporary spectral subset string from the command-line options */
    char *tmpspectralsubset;
}
ModisDescriptor;

/*****************************************************************************/

#include "loc_prot.h"           /* local prototypes */

#endif /* _SHARED_RESAMPLE_H_ */
