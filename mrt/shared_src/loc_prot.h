/******************************************************************************

FILE:  loc_prot.h

PURPOSE:  Function prototypes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  Rob Burrell            Initial development            
         01/01  John Rishea            Standardized formatting
         01/01  John Rishea            Added prototypes
 
HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/
#ifndef _LOC_PROT_H_
#define _LOC_PROT_H_

/************************************
 * 
 * function prototypes from GeoLIB 
 * 
 ************************************/
int c_transinit
( 
    long *inproj,	/* I: Input projection number */
    long *inunit,	/* I: Input projection unit code */
    long *inzone,	/* I: Input projection zone number */
    long *indtmnum,	/* I: Input datum number */
    double inparm[15],	/* I: 15 input projection parameters */
    long *outproj,	/* I: Output projection number */
    long *outunit,	/* I: Output projection unit code */
    long *outzone,	/* I: Output projection zone number */
    long *outdtmnum,	/* I: Output datum number */
    double outparm[15],	/* I: 15 output projection parameters */
    long prtprm[2],	/* I: Report destination.
                         * A TRUE/FALSE toggle is used:
                         * prtprm[GEO_TERM], prtprm[GEO_FILE]
                         */
    char *fname 	/* I: File name if printing to file, NULL if not */
);

int c_trans
( 
    long *inproj,		/* Input projection code */
    long *inunit,		/* Input projection units code */
    long *outproj,		/* Output projection code */
    long *outunit,		/* Output projection units code */
    double *inx,		/* Input X projection coordinate */
    double *iny,		/* Input Y projection coordinate */
    double *outx,		/* Output X projection coordinate */
    double *outy                /* Output Y projection coordinate */
);

int c_get_spheroid
(
    double semi_major,          /* I: ellipsoid semi-major axis */
    double semi_minor,          /* I: ellipsoid semi-minor axis */
    long *datum,                /* O: matching ellipsoid datum number */
    long *datum_valid           /* O: valid flag for the datum (1=valid) */
);

int gctp_call
(
    long in_proj_code,
    long in_zone_code,       /* not used except for UTM */
    long in_sphere_code,     /* not used except for UTM */
    double in_proj_parms[],
    long in_units,
    double in_x,
    double in_y,
    long out_proj_code,
    long out_zone_code,      /* not used except for UTM */
    long out_sphere_code,    /* not used except for UTM */
    double out_proj_parms[],
    long out_units,
    double *out_x,
    double *out_y
);

void print_proj
(
    long proj,		/* Projection ID as defined in proj.h */
    long zone,		/* Zone number for state plane and UTM */
    double par[15] 	/* Array of 15 projection parameters */
);

/************************************
 * 
 * Non-ANSI "common" prototypes 
 * 
 ************************************/

#if !defined(__CYGWIN__) && !defined(WIN32)
/*#if (MRT_OSTYPE == MRT_SOLARIS) || (MRT_OSTYPE == MRT_IRIX) || (MRT_OSTYPE == MRT_LINUX)*/

void strlwr
(
    char *str           /* I/O:  string to convert */
);

void strupr
(
    char *str           /* I/O:  string to convert */
);

#endif

char *SpaceToUnderscore ( char *str );

void InitializeModisDescriptor
(
    ModisDescriptor * P     /* I/O:  the descriptor to initialize */
);

/************************************
 * 
 * Message and error handling 
 * 
 ************************************/
int MessageHandler
(
    char *module,	/* I:  module who called us (optional) */
    char *format,	/* I:  format string for the message */
    ...                 /* I:  variable argument list */
);

int InitLogHandler
(
    int argc,                   /* I:  number of arguments */
    char *argv[]                /* I:  argument strings */
);

int LogHandler
(
    char *message	/* I:  message to be written to the log */
);

int CloseLogHandler
(
    void 
);

int ErrorHandler
( 
    int fatal_flag,	/* I:  TRUE for fatal errors, FALSE otherwise */
    char *module,	/* I:  calling module name */
    int error_code,	/* I:  one of the #defined names in resample.h */
    char *extra_message	/* I:  extra info if needed */
);

/************************************
 * 
 * Projection and coordinates
 * 
 ************************************/
int Deg2DMS
( 
    int projection_type,		/* I:  see resample.h for a list of types */
    double *projection_parameters	/* I/O:  parameters to convert */
);

void DestroyProjectionInfo
(
    ProjInfo * projinfo      /* I:  structure to free */
);

int DMS2Deg
( 
    int projection_type, 			/* I:  see resample.h for a list of types */
    double *projection_parameters	/* I/O:  parameters to convert */
);

int GetInputFileExt
(
    char input_filename[],     /* I: input filename */
    FileType *input_filetype   /* O: file type of the filename */
);

int GetInputImageCorners
(
    ModisDescriptor * modis,    /* I:  session info */
    FileDescriptor * file       /* I/O:  file for which we need corners */
);

int GetOutputImageCorners
(
    ModisDescriptor * modis,    /* I:  session info */
    FileDescriptor * outfile,   /* I/O:  file for which we need corners */
    FileDescriptor * infile     /* I:  input file information */
);

int GetOutputImageCornersBound
(
    ModisDescriptor * modis,    /* I:  session info */
    FileDescriptor * infile,    /* I:  input file information */
    FileDescriptor * outfile    /* I/O:  file for which we need corners */
);

ProjInfo *GetInputProjection
(
    ModisDescriptor * modis     /* I:  session info */
);

void GetOutputExtents
( 
    FileDescriptor * file	/* I/O:  file to find extents */
);

ProjInfo *GetOutputProjection
(
    ModisDescriptor * modis     /* I:  session info */
);

int StoreOutputExtents
(
    ModisDescriptor * modis,    /* I:  session info */
    FileDescriptor * file       /* I/O:  file from which we store extents */
);

int CheckProjectionParams
(
    ModisDescriptor *modis      /* I:  session info */
);

void CopyInputParametersToOutput
(
    ModisDescriptor *modis
);

void CheckDateline
(
    ModisDescriptor *modis    /* I/O: structure which contains the subset
                                      corner points */
);

long GetUTMZone
(
    double lon,         /* I:  longitude */
    double lat          /* I:  latitude */
);

void Usage
(
    void
);

int FixCornerPoints
(
    ModisDescriptor *P
);

int c_degdms
(
    double *deg,                /* I:  decimal degrees */
    double *dms,                /* O:  DMS form of deg */
    char *code,                 /* I:  "DEG" string */
    char *check                 /* I:  "LAT" or "LON" */
);

void dmsdeg
(
    double inputvalue,          /* I:  DMS value */
    double *outputvalue         /* O:  degree value */
);

int GetInputGeoCorner
(
    ModisDescriptor *modis,     /* I:  session info */
    double inputx,              /* I:  easting */
    double inputy,              /* I:  northing */
    double *outputlat,          /* O:  latitude */
    double *outputlon           /* O:  longitude */
);

int GetOutputProjCorner
( 
    ModisDescriptor *modis,     /* I:  session info */
    double inputlat,            /* I:  latitude */
    double inputlon,            /* I:  longitude */
    double *outputx,            /* O:  easting */
    double *outputy             /* O:  northing */
);

void GetInputProjCorner
(
    ModisDescriptor *modis,     /* I:  session info */
    double inputlat,            /* I:  latitude */
    double inputlon,            /* I:  longitude */
    double *outputx,            /* O:  easting */
    double *outputy             /* O:  northing */
);

int WalkInputBoundary
(
    ModisDescriptor *modis,    /* I: session info */
    FileDescriptor *infile,    /* I: input file */
    double incorners[4][2],    /* I: corner points in input space */ 
    double *minx,              /* O: minimum x value in output space */
    double *maxx,              /* O: maximum x value in output space */
    double *miny,              /* O: minimum y value in output space */
    double *maxy               /* O: maximum y value in output space */
);

/************************************
 * 
 * File IO
 * 
 ************************************/
FileDescriptor *CreateFileDescriptor
(
    ModisDescriptor * modis,    /* I:  session info */
    int bandnum,                /* I:  the number of the band */
    FileOpenType fileopentype,  /* I:  reading or writing */
    char *filename              /* I:  the file name */
);

int DestroyFileDescriptor
(
    FileDescriptor * file	/* I:  the descriptor to destroy */
);

int CreateFileBuffers
(
    FileDescriptor *file        /* I/O:  file for which buffers are created */
);

int CreateHdfEosField
(
    FileDescriptor *input,      /* input file descriptor */
    FileDescriptor *output,     /* output file descriptor */
    ModisDescriptor *modis      /* session info */
);

int CreateHdfEosGrid
(
    FileDescriptor *input,      /* input file descriptor */
    FileDescriptor *output,     /* output file descriptor */
    ModisDescriptor *modis      /* session info */
);

int InitOutputFile
( 
    FileDescriptor * input,     /* I:  need this for HDF band names */
    FileDescriptor * output,    /* I/O:  the output file to initialize */
    ModisDescriptor * modis     /* I:  session info */
);

int ReadRow
(
    FileDescriptor * file,  /* I:  input file descriptor */
    int row,                /* I:  row number to read */
    double *buffer          /* I/O:  buffer to hold data */
);

double ReadBufferValue
( 
    size_t col,             /* I:  column to find */
    size_t row,             /* I:  row to find */
    FileDescriptor * file   /* I:  file to read from */
);

int ReadHDFHeader
( 
    ModisDescriptor *modis     /* I/O:  session info */
);

int WriteHeaderFile
(
    ModisDescriptor * P		/* I:  session info */
);

int WriteRow
( 
    FileDescriptor * file,  /* I:  output file descriptor */
    int row,                /* I:  row number to write */
    double *buffer          /* I:  buffer of data to write */
);

MrtEndianness GetMachineEndianness(void);

int ReadRowHdfEos
(
    FileDescriptor *file,       /* I/O:  file to read */
    int row                     /* I:  row number to read */
);

int ReadRowMultiFile
(
    FileDescriptor *file,       /* I/O:  file to read from */
    int row                     /* I:  row number to read */
);

int WriteRowGeoTIFF
(
    FileDescriptor *file,       /* I:  file to write */
    int row                     /* I:  row number to write */
);

int WriteRowHdfEos
(
    FileDescriptor *file,       /* I:  file to write */
    int row                     /* I:  row number to write */
);

int WriteRowMultiFile
(
    FileDescriptor *file        /* I:  file to write data */
);

int CloseGeoTIFFFile
(
    FileDescriptor *filedescriptor      /* file to close */
);

void CloseHdfEos
(
    HdfEosFD *hdfptr         /* I: hdf file pointer to cleanup and close */
);

int CloseHdfEosFile
(
    FileDescriptor *filedescriptor      /* I:  file to close */
);

int CloseMultiFile
(
    FileDescriptor *filedescriptor      /* I:  the file to close */
);

FileDescriptor *OpenGeoTIFFFile
(
    ModisDescriptor *modis,     /* I:  session info */
    FileOpenType mode,          /* I:  must be write */
    int bandnum,                /* I:  band number for file name */
    int *status                 /* O:  error code */
);

FileDescriptor *OpenMultiFile
(
    ModisDescriptor *modis,     /* I:  session info */
    FileOpenType mode,          /* I:  reading or writing */
    int bandnum,                /* I:  band number to read or write */
    int *status                 /* O:  error status */
);

int SetTIFFTags
(
    FileDescriptor *output,     /* I:  file info */
    ModisDescriptor *modis      /* I:  session info */
);

int GetParameterFilename
(
    int argc,                   /* I:  number of arguments */
    char *argv[],               /* I:  argument strings */
    char *parameter_filename,   /* O:  filename */
    size_t param_filename_len   /* I:  length of filename buffer, one less */
                                /*     than allocated. */
);

int CheckFormatConversion
(
    int argc,                   /* I:  number of arguments */
    char *argv[],               /* I:  argument strings */
    ModisDescriptor *modis	/* O:  session info */
);

void PrintModisDescriptor
(
    ModisDescriptor *P      /* I:  session info */
);

void PrintOutputFileInfo
(
    ModisDescriptor *P
);

int ProcessArguments
(
    int argc,               /* I:  number of arguments */
    char *argv[],           /* I:  argument strings */
    ModisDescriptor *P      /* O:  session info */
);

int proj_check
(
    struct DATUMDEF *dtmparm,
    double projparm[15],
    long *parmflg
);

int ReadParameterFile
(
    ModisDescriptor *P,     /* I/O:  session info */
    int *defpixsiz          /* O:  hack to handle Geographic projection pixel sizes */
);

int AppendMetadata
(
    ModisDescriptor *modis	/* I:  session info */
);

int read_bounding_coords
(
    int32 sd_id,             /* I: the SDS file id # for the old HDF file */
    ModisDescriptor *modis,  /* I/O: file descriptor (bounding coords are
                                     updated in this structure) */
    int *all_coords_present  /* O: were all bounding coordinates present? */
);

int read_metadata
(
    int32 sd_id,             /* I: the SDS id # for the HDF file */
    char *attr,              /* I: the name handle of the attribute to read */
    ModisDescriptor *modis,  /* I/O: file descriptor (bounding coords are
                                     updated in this structure) */
    int *all_coords_present  /* O: were all bounding coordinates present? */
);


/************************************
 * 
 * .prm and .hdr user input processing  
 * 
 ************************************/

int CleanupLine
(
    char *line
);

int GetProjectionType
(
    char *tmpstr,
    ProjectionType *input_projection_type
);

int GetProjectionParameters
(
    char *tmpstr,
    double input_projection_parameters[]
);

int GetCornerPoints
(
    CornerType corner,
    char *tmpstr,
    double input_image_extent[4][2],
    double ll_extents_corners[4][2]
);

int GetNBands
(
    char *tmpstr,
    size_t *nbands,
    BandType **bandinfo
);

int GetBandNames
(
    char *tmpstr,
    int nbands,
    BandType *bandinfo
);

int GetDataTypes
(
    char *tmpstr,
    int nbands,
    BandType *bandinfo
);

int GetNLines
(
    char *tmpstr,
    int nbands,
    BandType *bandinfo
);

int GetNSamples
(
    char *tmpstr,
    int nbands,
    BandType *bandinfo
);

int GetPixelSize
(
    char *tmpstr,
    int nbands,
    BandType *bandinfo
);

int GetMinValue
(
    char *tmpstr,
    int nbands,
    BandType *bandinfo
);

int GetMaxValue
(
    char *tmpstr,
    int nbands,
    BandType *bandinfo
);

int GetBackgroundFill
(
    char *tmpstr,
    int nbands,
    BandType *bandinfo
);

int GetInputDatum
(
    char *tmpstr,
    int *input_datum_code
);

int GetUtmZone
(
    char *tmpstr,
    int *input_zone_code
);

int ConvertRowCol2Deg
(
    ModisDescriptor *modis      /* I:  session info */
);
int GetInputRectangle
(
    ModisDescriptor *modis      /* I:  session info */
);
int GetRectangle
(
    ModisDescriptor *modis      /* I:  session info */
);
int ReadHeaderFile
(
    ModisDescriptor *P  /* I/O:  session info */
);


/************************************
 * 
 * prototypes for resample.c  
 * 
 ************************************/

void ClobberFileBuffers
(
    FileDescriptor *file    /* I: file for which we are clobbering buffers */
);

int CloseFile
(
    FileDescriptor *filedescriptor      /* I:  file to close */
);

int GetHdfEosField
(
    ModisDescriptor *modis,     /* I:   session info */
    HdfEosFD *hdfptr,           /* I/O: file to get field number */
    int fieldnum                /* I:   field to find */
);

FileDescriptor *MakeHdfEosFD
(
    ModisDescriptor *modis,     /* I:  session info */
    HdfEosFD *hdfptr,           /* I:  HDF file descriptor */
    FileOpenType mode,          /* I:  read or write */
    int bandnum,                /* I:  band number to read or write */
    int *status                 /* I/O:  error status code */
);

HdfEosFD *OpenHdfEosFile
(
    char input_filename[],      /* I:  input filename for reading */
    char output_filename[],     /* I:  output filename for writing */
    FileOpenType mode,          /* I:  reading or writing */
    int *status                 /* I/O:  error code status */
);

FileDescriptor *OpenInImage
(
    ModisDescriptor *modis,     /* I:  session info */
    int bandnum,                /* I:  band number to open */
    int *status                 /* O:  Error status */
);

FileDescriptor *OpenOutImage
(
    ModisDescriptor *modis,     /* I:  session info */
    int bandnum,                /* I:  band number to open */
    int *status                 /* O:  Error status */
);

void Usage
(
    void
);


/************************************
 * 
 * prototypes for HDF2RB functions
 * 
 ************************************/

void PrintModisDescriptorHDF2RB
(
    ModisDescriptor *P   /* I:  session info */
);

int ReadHDFHeaderHDF2RB
(
    ModisDescriptor *modis	/* I/O:  session info */
);

int ProcessArgumentsHDF2RB
(
    int argc,			/* I:  number of arguments */
    char *argv[],		/* I:  argument strings */
    ModisDescriptor *P          /* O:  session info */
);

int ReadParameterFileHDF2RB
(
    ModisDescriptor *P     /* I/O:  session info */
);

int CheckOutputFieldsHDF2RB
(
    char *HeaderStrings[],
    ModisDescriptor *P
);

int FixCornerPointsHDF2RB
(
    ModisDescriptor *P
);

int WriteHeaderFileHDF2RB
(
    ModisDescriptor *P	/* I:  session info */
);

void PrintOutputFileInfoHDF2RB
(
    ModisDescriptor *P
);

void HDF2RB_Usage
(
    void
);
#endif
