/******************************************************************************

FILE:  read_prm.c

PURPOSE:  Read parameter files

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  John Weiss             Original Development
         01/01  John Rishea            Standardized formatting
         01/01  John Rishea            Moved local prototypes to loc_prot.h
         04/01  Rob Burrell            Add ellipse codes for UTM and Geo
         12/01  Gail Schmidt           Changed to support datums for all
                                       projections
         01/02  Gail Schmidt           If command-line switches are used,
                                       then don't read that parameter from
                                       the parameter file.
         05/02  Gail Schmidt           Either the first two proj parms or the
                                       datum can be specified, but not both.
         03/03  Gail Schmidt           Bumped up the size of a line and file
                                       buffer to read all the input mosaic
                                       files

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/
#include <ctype.h>		/* toupper */
#include "datum.h"
#include "worgen.h"
#include "shared_resample.h"
#include "mrt_dtype.h"

int AdjustGeoPixelSizeProj
(
   ModisDescriptor *modis     /* I/O:  session info */
);
int AdjustInputGeoPixelSizeProj
(
   ModisDescriptor *modis     /* I/O:  session info */
);

/**************************************
* Local Prototype                     *
**************************************/
static int GetQuotedValue( char *str, char **pv, int *pvn );

/******************************************************************************

MODULE:  ReadParameterFile

PURPOSE:  Function to process parameter file data.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status		See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  John Weiss             Original Development
         05/00  John Weiss             Assume no commas separating fields
         05/00  John Weiss             Change SIN to initialize PROJ_ISIN
                                       instead of PROJ_SIN.
         05/00  John Weiss             Change SIN back to SIN, add ISIN.
         06/00  Rob Burrell            conversion for row/col subsetting
         06/00  John Weiss             Add HDF-EOS file support.
         07/00  Rob Burrell            Error handling returns
         08/00  John Weiss             Allow default values for spectral and
                                       spatial subsetting.
         09/13  Rob Burrell	       Add output pixel size
         10/00  Rob Burrell            Fix Geographic output pixel size
         10/00  John Weiss             Upgrade to v.1.04 spec.
         01/01  John Rishea            Standardized formatting
         01/01  John Rishea            Added memory allocation chk for strdup
         02/01  John Weiss             Remove output pixel size units.
         02/02  Gail Schmidt           FixCornerPoints must be called before
                                       AdjustGeoPixelSize or determining the
                                       center of a UTM scene, so the call to
                                       FixCornerPoints has been moved to within
                                       the CheckOutputFields routine.
         11/02  Gail Schmidt           Add support for Albers Equal Area.
         11/02  Gail Schmidt           Sphere-based projections cannot specify
                                       datums. Default datum is No Datum.
         12/02  Gail Schmidt           Geographic can have a datum specified

NOTES:

******************************************************************************/

/* include files */
#include <stdio.h>		/* fopen, fprintf, fscanf, etc. */
#include <stdlib.h>		/* malloc */
#include <string.h>		/* strupr, strdup, strcmp, etc. */
#include "shared_resample.h"

/* constants */
#define FILE_BUFSIZ (64*1024)
#define LINE_BUFSIZ (5*1024)

/* function prototypes */
int CleanupLine  /* defined in read_hdr.c */
(
    char *line  
);		

int CheckOutputFields
(
    char *HeaderStrings[],
    ModisDescriptor *P,
    int *defpixsiz
);

int GetInputFilename
(
    char *str,
    ModisDescriptor *P
);

int ClearInputFilenameFromBuffer
(
    char *str
);

int GetOutputFilename
(
    char *str,
    ModisDescriptor *P
);

int ClearOutputFilenameFromBuffer
(
    char *str
);

int GetOutputFileExt
(
    ModisDescriptor *P 
);

int GetSpectralSubset
(
    char *str,
    ModisDescriptor *P 
);

int ClearSpectralSubsetFromBuffer
(
    char *str
);

void GetSpectralSubsetCmdLine
(
    ModisDescriptor *P 
);

int GetSpatialSubsetType
(
    char *str,
    ModisDescriptor *P
);

int ClearSpatialSubsetTypeFromBuffer
(
    char *str
);

int GetSpatialSubsetting
(
    CornerType corner,
    char *str,
    ModisDescriptor *P
);

int ClearSpatialSubsetFromBuffer
(
    char *str
);

int FixCornerPoints
(
    ModisDescriptor *P
);

int GetOutputResamplingType
(
    char *str,
    ModisDescriptor *P 
);

int ClearOutputResampTypeFromBuffer
(
    char *str
);

int GetOutputProjectionType
(
    char *str,
    ModisDescriptor *P 
);

int ClearOutputProjTypeFromBuffer
(
    char *str
);

int GetOutputProjectionParameters
(
    char *str,
    ModisDescriptor *P 
);

int ClearOutputProjParamsFromBuffer
(
    char *str
);

int GetOutputPixelSize
(
    char *str,
    ModisDescriptor *P 
);

int ClearOutputPixelSizeFromBuffer
(
    char *str
);

int GetOutputUTMZone
(
    char *str,
    ModisDescriptor *P
);

int ClearOutputUTMZoneFromBuffer
(
    char *str
);

int GetOutputDatum
(
    char *str,
    ModisDescriptor *P
);

void PrintModisDescriptor
(
    ModisDescriptor *P 
);

/**************************************************************************/

int ReadParameterFile
(
    ModisDescriptor *P,    /* I/O:  session info */
    int *defpixsiz         /* O: hack to handle Geographic proj pixel sizes */
)

{
    int valid_string;
    int err, n = 0;
    int errval = MRT_NO_ERROR;
    FILE *fp = NULL;
    char filebuf[FILE_BUFSIZ] = { 0 },
	 linebuf[LINE_BUFSIZ],
	 token[LINE_BUFSIZ];
    char error_str[LINE_BUFSIZ];
    char *bufptr = NULL,
         *capbuf = NULL;
    char *HeaderStrings[] = {
	"INPUT_FILENAME",
	"SPECTRAL_SUBSET",
        "SPATIAL_SUBSET_TYPE",
	"SPATIAL_SUBSET_UL_CORNER",
	"SPATIAL_SUBSET_LR_CORNER",
	"OUTPUT_FILENAME",
	"RESAMPLING_TYPE",
	"OUTPUT_PROJECTION_TYPE",
	"OUTPUT_PROJECTION_PARAMETERS",
	"OUTPUT_PIXEL_SIZE",
        "UTM_ZONE",
        "DATUM"};
    /* There are loops in this code that loop through the following
     * enumeration, starting at "INPUT_FILENAME" while the counter
     * is less than NSTRINGS.  Just be carefull adding items to the
     * enumeration below.
     */
    typedef enum {
        INPUT_FILENAME, SPECTRAL_SUBSET, SPATIAL_SUBSET_TYPE,
        SPATIAL_SUBSET_UL, SPATIAL_SUBSET_LR, OUTPUT_FILENAME,
        RESAMPLING_TYPE, OUTPUT_PROJ_TYPE, OUTPUT_PROJ_PARMS,
        PIXEL_SIZE, UTM_ZONE, DATUM, NSTRINGS } ParamType;
    ParamType iparam;
    /* these enums must also be changed in the shared_resample.h file */

    /*******************************************************************/

    /* open parameter file for reading the INPUT_FILENAME, which should
       be the first line in the file */
    if ( ( fp = fopen( P->parameter_filename, "r" ) ) == NULL )
    {
	sprintf( error_str, "Unable to open parameter file %s",
            P->parameter_filename );
	ErrorHandler( TRUE, "ReadParameterFile", ERROR_OPEN_INPUTPAR,
            error_str );
	return ERROR_OPEN_INPUTPAR;
    }

    /*******************************************************************/

    /* read first line of file, clean it up, and store in buffer */
    while ( fgets( linebuf, LINE_BUFSIZ, fp ) )
    {
        if ( (err = CleanupLine( linebuf )) == 0 )
            strcat( filebuf, linebuf );
        else if( err > 0 ) {
           if( err == 1 )
              sprintf( error_str, "Internal buffer not big enough for " \
                       "line found in %s", P->parameter_filename );
           else if( err == 2 )
              sprintf( error_str, "Environment variable [%s] found in file "\
                       "%s, but the variable is not defined in environment.",
                       linebuf, P->parameter_filename );
           else
              strcpy( error_str, "Unknown Internal Error!!!" );
           ErrorHandler( TRUE, "ReadHeaderFile", ERROR_READ_INPUTHEADER,
               error_str );
        }
    }
    fclose( fp );

    /*******************************************************************/

    /* determine input filename: INPUT_FILENAME = filename */

    /* make copy of filebuf */
    capbuf = strdup( filebuf );

    /* error if memory for capbuf was not successfully allocated */
    if ( capbuf == NULL)
    {
        sprintf( error_str, "Unable to allocate memory for capbuf" );
        ErrorHandler( TRUE, "ReadParameterFile", ERROR_MEMORY, error_str );
        return ERROR_MEMORY;
    }

    /* capitalize capbuf to make field recognition easier */ 
    strupr( capbuf );

    /* determine input filename: INPUT_FILENAME = filename */
    if ( (bufptr = strstr( capbuf, "INPUT_FILENAME" ) ) == NULL )
    {
        /* first free memory allocated for capbuf */
        free( capbuf );

        /* now display error msg and exit */
	sprintf( error_str, "Missing or invalid INPUT_FILENAME field" );
	ErrorHandler( TRUE, "ReadParameterFile", ERROR_INPUTNAME_FIELD,
            error_str );
	return ERROR_INPUTNAME_FIELD;
    }

    /* if the input filename has not been specified on the command
       line, then read it from the parameter file */
    if (!P->iswitch)
    {
        if ( ( n = GetInputFilename ( filebuf + (bufptr - capbuf) +
            strlen( "INPUT_FILENAME" ), P ) ) < 1 )
        {
            /* first free memory allocated for capbuf */
            free( capbuf );

            /* now display error  msg and exit */
	    sprintf( error_str, "Missing or invalid INPUT_FILENAME field" );
	    ErrorHandler( TRUE, "ReadParameterFile", ERROR_INPUTNAME_FIELD,
                error_str );
	    return ERROR_INPUTNAME_FIELD;
        }
    }

    /* determine input file type from input filename extension */
    errval = GetInputFileExt( P->input_filename, &P->input_filetype );

    /* determine output filename: OUTPUT_FILENAME = filename */
    if ( (bufptr = strstr( capbuf, "OUTPUT_FILENAME" ) ) == NULL )
    {
        /* first free memory allocated for capbuf */
        free( capbuf );

        /* now display error msg and exit */
	sprintf( error_str, "Missing or invalid OUTPUT_FILENAME field" );
	ErrorHandler( TRUE, "ReadParameterFile", ERROR_OUTPUTNAME_FIELD,
            error_str );
	return ERROR_OUTPUTNAME_FIELD;
    }

    /* if the output filename has not been specified on the command
       line, then read it from the parameter file */
    if (!P->oswitch)
    {
        if ( ( n = GetOutputFilename ( filebuf + (bufptr - capbuf) +
            strlen( "OUTPUT_FILENAME" ), P ) ) < 1 )
        {
            /* first free memory allocated for capbuf */
            free( capbuf );

            /* now display error msg and exit */
	    sprintf( error_str, "Missing or invalid OUTPUT_FILENAME field" );
	    ErrorHandler( TRUE, "ReadParameterFile", ERROR_OUTPUTNAME_FIELD,
                error_str );
	    return ERROR_OUTPUTNAME_FIELD;
        }
        P->oswitch = TRUE;
    }

    /* determine input file type from output filename extension */
    GetOutputFileExt( P );

    free( capbuf );

    /*******************************************************************/

    /* open input file and read file info (nbands, nlines, nsamples, etc.) */
    switch ( P->input_filetype )
    {
	case RAW_BINARY:
	    errval = ReadHeaderFile( P );
	    break;

	case HDFEOS:
	    errval = ReadHDFHeader( P );
	    break;
    }

    if ( errval != MRT_NO_ERROR )
    {
	ErrorHandler( TRUE, "ReadParameterFile", ERROR_READ_INPUTPAR,
	    "Input data file error" );
	return errval;
    }

    /*******************************************************************/

    /* read tokens from file buffer */
    bufptr = filebuf;
    while ( sscanf( bufptr, "%s%n", token, &n ) > 0 )
    {
	/* got next token, so advance file buffer pointer */
	bufptr += n;

	/* check for valid strings */
	valid_string = 0;
	for ( iparam = INPUT_FILENAME; iparam < NSTRINGS; iparam++ )
	{
	    /* check for match to fieldname */
	    if ( strcasecmp( token, HeaderStrings[iparam] ) == 0 )
	    {
		/* got a fieldname match, check it out */
		P->ParamsPresent[iparam] = 1;
		valid_string = 1;
		break;
	    }
	}

	/* make sure we have a valid field name */
	if ( !valid_string )
	{
	    sprintf( error_str, "Invalid field %s", token );
            ErrorHandler( TRUE, "ReadParameterFile", ERROR_READ_INPUTPAR,
                error_str );
	    return ERROR_READ_INPUTPAR;
	}

        /* parse remainder of fields, but don't use them if their respective
           command-line switches have been used */
	switch ( iparam )
	{
	    case INPUT_FILENAME:
                /* determine input filename: INPUT_FILENAME = ... */
                /* The input filename has already been processed, so there is
                   no need to process it again.  Besides this would override
                   the input filename command-line switch, if used. -gs */
                /* n = GetInputFilename( bufptr, P ); */
                n = ClearInputFilenameFromBuffer( bufptr );
                break;

	    case SPECTRAL_SUBSET:
                /* determine spectral subsetting: SPECTRAL_SUBSET = ... */
                if (!P->sswitch)
                    n = GetSpectralSubset( bufptr, P );
                else
                    /* spectral subsetting was specified on the command line
                       and now that the input file has been read and we know
                       the number of bands, etc. we can process the spectral
                       subset string */
                    GetSpectralSubsetCmdLine( P );
                    n = ClearSpectralSubsetFromBuffer( bufptr );
		break;

            case SPATIAL_SUBSET_TYPE:
                /* determine which type of spatial subsetting has been
                   specified */
                if (!P->aswitch)
                    n = GetSpatialSubsetType( bufptr, P );
                else
                    n = ClearSpatialSubsetTypeFromBuffer( bufptr );
                break;

	    case SPATIAL_SUBSET_UL:
                /* determine spatial subsetting:
                   SPATIAL_SUBSET_UL_CORNER = ... */
                if (!P->lswitch)
                    n = GetSpatialSubsetting( UL, bufptr, P );
                else
                    n = ClearSpatialSubsetFromBuffer( bufptr );
		break;

	    case SPATIAL_SUBSET_LR:
                /* determine spatial subsetting:
                   SPATIAL_SUBSET_LR_CORNER = ... */
                if (!P->lswitch)
                    n = GetSpatialSubsetting( LR, bufptr, P );
                else
                    n = ClearSpatialSubsetFromBuffer( bufptr );
		break;

	    case OUTPUT_FILENAME:
                /* determine output filename: OUTPUT_FILENAME = ... */
                if (!P->oswitch)
                    n = GetOutputFilename( bufptr, P );
                else
                    n = ClearOutputFilenameFromBuffer( bufptr );

                /* get the output file extension */
		GetOutputFileExt( P );
		break;

	    case RESAMPLING_TYPE:
                /* determine resampling type: RESAMPLING_TYPE = ... */
                if (!P->rswitch)
                    n = GetOutputResamplingType( bufptr, P );
                else
                    n = ClearOutputResampTypeFromBuffer( bufptr );
		break;

	    case OUTPUT_PROJ_TYPE:
                /* determine output projection type:
                   OUTPUT_PROJECTION_TYPE = ... */
                if (!P->tswitch)
                    n = GetOutputProjectionType( bufptr, P );
                else
                    n = ClearOutputProjTypeFromBuffer( bufptr );
		break;

	    case OUTPUT_PROJ_PARMS:
                /* determine output projection parameters:
                   OUTPUT_PROJECTION_PARAMETERS = ... */
                if (!P->jswitch)
                    n = GetOutputProjectionParameters( bufptr, P );
                else
                    n = ClearOutputProjParamsFromBuffer( bufptr );
		break;

	    case PIXEL_SIZE:
                /* determine output pixel size: OUTPUT_PIXEL_SIZE = ... */
                if (!P->xswitch)
                    n = GetOutputPixelSize( bufptr, P );
                else
                    n = ClearOutputPixelSizeFromBuffer( bufptr );
		break;

            case UTM_ZONE:
                /* determine UTM zone: UTM_ZONE = ... */
                if (!P->uswitch)
                    n = GetOutputUTMZone( bufptr, P );
                else
                    n = ClearOutputUTMZoneFromBuffer( bufptr );
		break;

            case DATUM:
                /* determine datum: DATUM = ... */
                /* note: currently no command-line option for the datum
                   code */
                n = GetOutputDatum( bufptr, P );
		break;
	}

	/* make sure we got a valid field */
	if ( n < 1 )
	{
	    sprintf( error_str, "Invalid field %s", token );
	    ErrorHandler( TRUE, "ReadParameterFile", ERROR_READ_INPUTPAR,
                error_str );
	    return ERROR_READ_INPUTPAR;
	}

	/* advance file buffer pointer to next field */
	bufptr += n;
    }

    /*******************************************************************/

    /* check to see if all required fields are filled in,
     * and use default values for missing optional fields.
     * also finish the spatial subsetting (FixCornerPoints).
     */
    CheckOutputFields ( HeaderStrings, P, defpixsiz );

    /*******************************************************************/

    /* finish up */
    return 0;
}


/******************************************************************************

FILE:  ReadParameterFileHDF2RB

PURPOSE:  Read parameter files

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         11/05  Gail Schmidt           Original Development

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/
int ReadParameterFileHDF2RB
(
    ModisDescriptor *P     /* I/O:  session info */
)

{
    int valid_string;
    int err, n = 0;
    int errval = MRT_NO_ERROR;
    FILE *fp = NULL;
    char filebuf[FILE_BUFSIZ] = { 0 },
	 linebuf[LINE_BUFSIZ],
	 token[LINE_BUFSIZ];
    char error_str[LINE_BUFSIZ];
    char *bufptr = NULL,
         *capbuf = NULL;
    char *HeaderStrings[] = {
	"INPUT_FILENAME",
	"SPECTRAL_SUBSET",
        "SPATIAL_SUBSET_TYPE",
	"SPATIAL_SUBSET_UL_CORNER",
	"SPATIAL_SUBSET_LR_CORNER",
	"OUTPUT_FILENAME",
	"RESAMPLING_TYPE",
	"OUTPUT_PROJECTION_TYPE",
	"OUTPUT_PROJECTION_PARAMETERS",
	"OUTPUT_PIXEL_SIZE",
        "UTM_ZONE",
        "DATUM"};
    typedef enum {
        INPUT_FILENAME, SPECTRAL_SUBSET, SPATIAL_SUBSET_TYPE,
        SPATIAL_SUBSET_UL, SPATIAL_SUBSET_LR, OUTPUT_FILENAME,
        RESAMPLING_TYPE, OUTPUT_PROJ_TYPE, OUTPUT_PROJ_PARMS,
        PIXEL_SIZE, UTM_ZONE, DATUM, NSTRINGS } ParamType;
    ParamType iparam;
    /* these enums must also be changed in the shared_resample.h file */

    /*******************************************************************/

    /* open parameter file for reading the INPUT_FILENAME, which should
       be the first line in the file */
    if ( ( fp = fopen( P->parameter_filename, "r" ) ) == NULL )
    {
	sprintf( error_str, "Unable to open parameter file %s",
            P->parameter_filename );
	ErrorHandler( TRUE, "ReadParameterFileHDF2RB", ERROR_OPEN_INPUTPAR,
            error_str );
	return ERROR_OPEN_INPUTPAR;
    }

    /*******************************************************************/

    /* read first line of file, clean it up, and store in buffer */
    while ( fgets( linebuf, LINE_BUFSIZ, fp ) )
    {
        if ( (err = CleanupLine( linebuf )) == 0 )
            strcat( filebuf, linebuf );
        else if( err > 0 ) {
           if( err == 1 )
              sprintf( error_str, "Internal buffer not big enough for " \
                       "line found in %s", P->parameter_filename );
           else if( err == 2 )
              sprintf( error_str, "Environment variable found in file %s, "\
                       "but the variable is not defined in environment.",
                       P->parameter_filename );
           else
              strcpy( error_str, "Unknown Internal Error!!!" );
           ErrorHandler( TRUE, "ReadHeaderFile", ERROR_READ_INPUTHEADER,
               error_str );
        }
    }
    fclose( fp );

    /*******************************************************************/

    /* determine input filename: INPUT_FILENAME = filename */

    /* make copy of filebuf */
    capbuf = strdup( filebuf );

    /* error if memory for capbuf was not successfully allocated */
    if ( capbuf == NULL)
    {
        sprintf( error_str, "Unable to allocate memory for capbuf" );
        ErrorHandler( TRUE, "ReadParameterFileHDF2RB", ERROR_MEMORY,
            error_str );
        return ERROR_MEMORY;
    }

    /* capitalize capbuf to make field recognition easier */ 
    strupr( capbuf );

    /* first determine input filename: INPUT_FILENAME = filename */
    if ( (bufptr = strstr( capbuf, "INPUT_FILENAME" ) ) == NULL )
    {
        /* first free memory allocated for capbuf */
        free( capbuf );

        /* now display error msg and exit */
	sprintf( error_str, "Missing or invalid INPUT_FILENAME field" );
	ErrorHandler( TRUE, "ReadParameterFileHDF2RB", ERROR_INPUTNAME_FIELD,
            error_str );
	return ERROR_INPUTNAME_FIELD;
    }

    /* if the input filename has not been specified on the command
       line, then read it from the parameter file */
    if (!P->iswitch)
    {
        if ( ( n = GetInputFilename ( filebuf + (bufptr - capbuf) +
            strlen( "INPUT_FILENAME" ), P ) ) < 1 )
        {
            /* first free memory allocated for capbuf */
            free( capbuf );

            /* now display error  msg and exit */
	    sprintf( error_str, "Missing or invalid INPUT_FILENAME field" );
	    ErrorHandler( TRUE, "ReadParameterFileHDF2RB",
                ERROR_INPUTNAME_FIELD, error_str );
	    return ERROR_INPUTNAME_FIELD;
        }
    }

    free( capbuf );

    /*******************************************************************/

    /* determine input file type from input filename extension */
    errval = GetInputFileExt( P->input_filename, &P->input_filetype );

    /*******************************************************************/

    /* open input file and read file info (nbands, nlines, nsamples, etc.) */
    switch ( P->input_filetype )
    {
	case HDFEOS:
	    errval = ReadHDFHeaderHDF2RB( P );
	    break;

        default:
            ErrorHandler( TRUE, "ReadParameterFileHDF2RB",
                ERROR_READ_INPUTPAR, "Input files can only be HDF format" );
	    return ERROR_INPUTNAME_FIELD;
    }

    if ( errval != MRT_NO_ERROR )
    {
	ErrorHandler( TRUE, "ReadParameterFileHDF2RB", ERROR_READ_INPUTPAR,
	    "Input filename error" );
	return errval;
    }

    /*******************************************************************/

    /* read tokens from file buffer */
    bufptr = filebuf;
    while ( sscanf( bufptr, "%s%n", token, &n ) > 0 )
    {
	/* got next token, so advance file buffer pointer */
	bufptr += n;

	/* check for valid strings */
	valid_string = 0;
	for ( iparam = INPUT_FILENAME; iparam < NSTRINGS; iparam++ )
	{
            /* skip over certain parameters since they aren't used in the
               HDF2RB conversion */
            if ( iparam == SPATIAL_SUBSET_TYPE || iparam == RESAMPLING_TYPE ||
                 iparam == OUTPUT_PROJ_TYPE || iparam == OUTPUT_PROJ_PARMS ||
                 iparam == PIXEL_SIZE || iparam == UTM_ZONE || iparam == DATUM )
                continue;

	    /* check for match to fieldname */
	    if ( strcasecmp( token, HeaderStrings[iparam] ) == 0 )
	    {
		/* got a fieldname match, check it out */
		P->ParamsPresent[iparam] = 1;
		valid_string = 1;
		break;
	    }
	}

	/* make sure we have a valid field name */
	if ( !valid_string )
	{
	    sprintf( error_str, "Invalid field %s", token );
            ErrorHandler( TRUE, "ReadParameterFileHDF2RB", ERROR_READ_INPUTPAR,
                error_str );
	    return ERROR_READ_INPUTPAR;
	}

        /* parse remainder of fields, but don't use them if their respective
           command-line switches have been used */
	switch ( iparam )
	{
	    case INPUT_FILENAME:
                /* determine input filename: INPUT_FILENAME = ... */
                /* The input filename has already been processed, so there is
                   no need to process it again.  Besides this would override
                   the input filename command-line switch, if used. -gs */
                /* n = GetInputFilename( bufptr, P ); */
                n = ClearInputFilenameFromBuffer( bufptr );
                break;

	    case SPECTRAL_SUBSET:
                /* determine spectral subsetting: SPECTRAL_SUBSET = ... */
                if (!P->sswitch)
                    n = GetSpectralSubset( bufptr, P );
                else
                    /* spectral subsetting was specified on the command line
                       and now that the input file has been read and we know
                       the number of bands, etc. we can process the spectral
                       subset string */
                    GetSpectralSubsetCmdLine( P );
                    n = ClearSpectralSubsetFromBuffer( bufptr );
		break;

	    case SPATIAL_SUBSET_UL:
                /* determine spatial subsetting:
                   SPATIAL_SUBSET_UL_CORNER = ... */
                if (!P->lswitch)
                    n = GetSpatialSubsetting( UL, bufptr, P );
                else
                    n = ClearSpatialSubsetFromBuffer( bufptr );
		break;

	    case SPATIAL_SUBSET_LR:
                /* determine spatial subsetting:
                   SPATIAL_SUBSET_LR_CORNER = ... */
                if (!P->lswitch)
                    n = GetSpatialSubsetting( LR, bufptr, P );
                else
                    n = ClearSpatialSubsetFromBuffer( bufptr );
		break;

	    case OUTPUT_FILENAME:
                /* determine output filename: OUTPUT_FILENAME = ... */
                if (!P->oswitch)
                    n = GetOutputFilename( bufptr, P );
                else
                    n = ClearOutputFilenameFromBuffer( bufptr );

                /* get the output file extension */
		GetOutputFileExt( P );

                /* output file must be raw binary */
                if ( P->output_filetype != RAW_BINARY )
                {
                    ErrorHandler( TRUE, "ReadParameterFileHDF2RB",
                        ERROR_READ_INPUTPAR,
                        "Output files can only be raw binary format" );
                    return ERROR_INPUTNAME_FIELD;
                }
		break;
	}

	/* make sure we got a valid field */
	if ( n < 1 )
	{
	    sprintf( error_str, "Invalid field %s", token );
	    ErrorHandler( TRUE, "ReadParameterFileHDF2RB", ERROR_READ_INPUTPAR,
                error_str );
	    return ERROR_READ_INPUTPAR;
	}

	/* advance file buffer pointer to next field */
	bufptr += n;
    }

    /*******************************************************************/

    /* check to see if all required fields are filled in,
     * and use default values for missing optional fields.
     * also finish the spatial subsetting (FixCornerPoints).
     */
    CheckOutputFieldsHDF2RB ( HeaderStrings, P );

    /*******************************************************************/

    /* finish up */
    return 0;
}

/******************************************************************************

MODULE:  CopyInputParametersToOutput

PURPOSE: If the user has chosen to do a translation, we need to make
         the input and output projection parameters the same.

RETURN VALUE:
Type = void

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         07/01  Rob Burrell            Original Development

NOTES:

******************************************************************************/
void CopyInputParametersToOutput( ModisDescriptor *P )
{
    int i;

    P->output_projection_type = P->input_projection_type;

    for ( i = 0; i < 15; i++ )
         P->output_projection_parameters[i] = P->input_projection_parameters[i];

    P->output_datum_code = P->input_datum_code;
    P->output_zone_code = P->input_zone_code;

    /* we don't want to reset the datum so specify that it was set */
    P->ParamsPresent[DATUM] = 1;
}

/************************************************************************/

/*
 * CheckOutputFields - make sure all required fields are present
 *                     in the parameter file and verify that the
 *                     parameter combinations are valid
 *                     If the user is reprojecting to a sphere-based
 *                     projection, then an output datum is not allowed
 *                     to be specified.
 */

int CheckOutputFields
(
    char *HeaderStrings[],
    ModisDescriptor *P, 
    int *defpixsiz
)

{
    ParamType iparam;
    char s[LINE_BUFSIZ];
    size_t i;

    if( P->resampling_type == NO_RESAMPLE )
    {
        P->input_datum_code = E_NODATUM;
    	CopyInputParametersToOutput( P );
    }

    /* default to no spectral subsetting */
    if ( !P->ParamsPresent[SPECTRAL_SUBSET] )
    {
        P->ParamsPresent[SPECTRAL_SUBSET] = 1;
	for ( i = 0; i < P->nbands; i++ )
	    P->bandinfo[i].selected = 1;
    }

    /* default to no spatial subsetting if UL is not provided */
    if ( !P->ParamsPresent[SPATIAL_SUBSET_UL] )
    {
        /* default to no spatial subsetting */
        P->is_subset = FALSE;
        P->ParamsPresent[SPATIAL_SUBSET_UL] = 1;

        /* default to input lat/long for the spatial subset type */
        P->spatial_subset_type = INPUT_LAT_LONG;
        P->ParamsPresent[SPATIAL_SUBSET_TYPE] = 1;

        /* use the input UL corner */
        P->ll_spac_sub_gring_corners[UL][0] = P->input_image_extent[UL][0];
        P->ll_spac_sub_gring_corners[UL][1] = P->input_image_extent[UL][1];
    }

    /* default to no spatial subsetting if LR is not provided */
    if ( !P->ParamsPresent[SPATIAL_SUBSET_LR] )
    {
        /* default to no spatial subsetting */
        P->is_subset = FALSE;
        P->ParamsPresent[SPATIAL_SUBSET_LR] = 1;

        /* default to input lat/long for the spatial subset type */
        P->spatial_subset_type = INPUT_LAT_LONG;
        P->ParamsPresent[SPATIAL_SUBSET_TYPE] = 1;

        /* use the input LR corner */
        P->ll_spac_sub_gring_corners[LR][0] = P->input_image_extent[LR][0];
        P->ll_spac_sub_gring_corners[LR][1] = P->input_image_extent[LR][1];
    }

    /* if no spatial subset type was specified, then this is an error */
    if ( !P->ParamsPresent[SPATIAL_SUBSET_TYPE] )
    {
        sprintf( s, "The spatial subset type must be specified in the "
                    "parameter file.\n" );
        ErrorHandler( TRUE, "CheckOutputFields", ERROR_SPATIAL_SUBSET_TYPE, s );
    }

    /* default to nearest neighbor resampling, if the resampling type
       hasn't already been set */
    if ( !P->ParamsPresent[RESAMPLING_TYPE] )
    {
        P->ParamsPresent[RESAMPLING_TYPE] = 1;
	P->resampling_type = NN;
    }

    /* if no datum was specified, then set to NODATUM */
    if ( !P->ParamsPresent[DATUM] )
    {
        P->output_datum_code = E_NODATUM;
        P->ParamsPresent[DATUM] = 1;
    }

    /* sphere-based projections cannot have an output datum specified */
    if ( ( P->output_projection_type == PROJ_LA  ||
           P->output_projection_type == PROJ_SIN ||
           P->output_projection_type == PROJ_IGH ||
           P->output_projection_type == PROJ_MOL ||
           P->output_projection_type == PROJ_HAM ||
           P->output_projection_type == PROJ_ISIN ) &&
           P->output_datum_code != E_NODATUM )
    {
        sprintf( s, "When reprojecting to sphere-based projections a "
                    "datum conversion is not allowed. The projection "
                    "parameters need to be specified using the radius "
                    "of the output sphere as the first parameter and "
                    "the datum must be NODATUM.\n" );
        ErrorHandler( TRUE, "CheckOutputFields", ERROR_DATUM_FIELD, s );
    }

    /* if no datum was specified (or the NoDatum option was specified) and
       the first two projection parameters were also not specified, then
       this is an error (doesn't apply to UTM or GEO). UTM proj parms will
       be handled later in this routine. */
    if ( ( !P->ParamsPresent[DATUM] ||
           P->output_datum_code == E_NODATUM ) &&
           P->output_projection_parameters[0] == 0.0 &&
           P->output_projection_parameters[1] == 0.0 &&
           ( P->output_projection_type != PROJ_UTM &&
             P->output_projection_type != PROJ_GEO ) )
    {
        sprintf( s, "Either the first two PROJECTION_PARAMETERS (which "
                    "represent the semi-major and semi-minor axes of the "
                    "output spheroid) or a datum must be specified.\n" );
        ErrorHandler( TRUE, "CheckOutputFields", ERROR_OUTPROJPARAMS_FIELD, s );
    }

    /* if a datum was specified and the first two projection parameters
       were also specified, then this is an error (doesn't apply to UTM
       and GEO projections). UTM proj parms will be handled later in this
       routine. */
    if ( P->output_datum_code != E_NODATUM &&
         P->output_projection_parameters[0] != 0.0 &&
         ( P->output_projection_type != PROJ_UTM &&
           P->output_projection_type != PROJ_GEO ) )
    {
        sprintf( s, "Either the datum may be specified or the first two "
            "projection parameters may be specified, but not both. When "
            "specifying your own radius of the sphere or semi-major and "
            "semi-minor axes, use the NODATUM option." );
        ErrorHandler( TRUE, "CheckOutputFields", ERROR_DATUM_FIELD, s );
    }

    /* for UTM GCTP will not process without datum information so if
       the output_datum_code is NODATUM (not very likely), then make the
       output_datum_code equal to that of the input_datum_code, which is
       by default WGS84. this will allow GCTP to process the data, but will
       not implement a datum conversion. if the input and output projections
       are UTM and no input datum is available, then the input sphere value
       will be used in the GCTP call. thus leave the input and output datum
       values as E_NODATUM. */
    if ( P->output_projection_type == PROJ_UTM &&
         P->output_datum_code == E_NODATUM )
    {
        if ( P->input_datum_code == E_NODATUM &&
             P->input_projection_type != PROJ_UTM)
            P->output_datum_code = P->input_datum_code = E_WGS84;
        else
            P->output_datum_code = P->input_datum_code;

	sprintf( s,
            "For UTM projections, NoDatum processing will default the "
            "output datum code to be the same as the input datum code. If "
            "the input projection is UTM, then the input sphere value will "
            "be used if the input datum value is not available.\n" );
	ErrorHandler( FALSE, "CheckOutputFields", ERROR_DATUM_FIELD, s );
    }

    /* if the output datum has been specified and the output projection is
       an ellipse-based projection, then set the input datum code to WGS84
       (default value) if it is not already set. Also zero out the first
       two projection parameters. */
    if ( ( P->output_projection_type == PROJ_GEO  ||
           P->output_projection_type == PROJ_UTM  ||
           P->output_projection_type == PROJ_AEA  ||
           P->output_projection_type == PROJ_LCC  ||
           P->output_projection_type == PROJ_MERC ||
           P->output_projection_type == PROJ_PS   ||
           P->output_projection_type == PROJ_TM ) &&
         P->output_datum_code != E_NODATUM )
    {
        /* if the projection type is ISIN or SIN and the spherecode is
           not specified (-1) an erroneous MODIS sphere (1) or the MODIS
           sphere code (31), then this data product is referenced to the
           WGS84 datum */
        if ( P->input_projection_type == PROJ_ISIN ||
             P->input_projection_type == PROJ_SIN )
        {
            if ( P->input_datum_code == -1 || P->input_datum_code == 1 ||
                 P->input_datum_code == 31 )
            {
                P->input_datum_code = E_WGS84;
            }
        }
        else if ( P->input_datum_code == E_NODATUM )
            P->input_datum_code = E_WGS84;
    }

    /* if not doing a datum conversion (i.e. NODATUM has been specified)
       then GCTP requires the input datum to also be NODATUM and the
       radius of the sphere or the semi-major and semi-minor input proj
       parms must be specified. GCTP requires that if no datum is specified,
       then the proj parms information is available for both the input and
       output projections. */
    if ( P->output_datum_code == E_NODATUM &&
         P->input_datum_code != E_NODATUM )
    {
        if ( P->input_projection_parameters[0] != 0.0 &&
             P->input_projection_parameters[1] != 0.0 )
        {
            sprintf( s, "If the output datum is not specified then the "
                        "input datum is also not used. Therefore the input "
                        "projection parameters need to be specified using "
                        "the radius of the input sphere or the "
                        "semi-major/minor axes as the first (one/two) "
                        "input projection parameter(s).\n" );
            ErrorHandler( TRUE, "CheckOutputFields", ERROR_DATUM_FIELD, s );
        }
        else
        {
            /* set the input datum code to NODATUM */
            P->input_datum_code = E_NODATUM;
        }
    }

    /* the projection parameters are now verified, so even if they weren't
       specified mark them as specified */
    P->ParamsPresent[OUTPUT_PROJ_PARMS] = 1;

    /* finish spatial subsetting */
    FixCornerPoints( P );

    /* if the UTM_ZONE was not specified and the first two projection
       parameters were not specified, then default to the center of the
       scene for UTM. NOTE: Must be done after FixCornerPoints is called. */
    if ( P->output_projection_type == PROJ_UTM &&
         ( !P->ParamsPresent[UTM_ZONE] ||          /* no UTM zone specified */
            P->output_zone_code == 0 ) &&
	 P->output_projection_parameters[0] == 0.0 &&
	 P->output_projection_parameters[1] == 0.0 )
    {
	/* center longitude */
	P->output_projection_parameters[0] =
	    ( P->ll_spac_sub_gring_corners[UL][1] +
              P->ll_spac_sub_gring_corners[LR][1] ) / 2.0;

	/* center latitude */
	P->output_projection_parameters[1] =
	    ( P->ll_spac_sub_gring_corners[UL][0] +
              P->ll_spac_sub_gring_corners[LR][0] ) / 2.0;

	sprintf( s,
            "Defaulting to scene center for first two UTM projection "
            "parameters: %f, %f\n", P->output_projection_parameters[0],
            P->output_projection_parameters[1] );
	ErrorHandler( FALSE, "CheckOutputFields", ERROR_OUTPROJPARAMS_FIELD,
            s );
    }

    /* default to same input and output pixel sizes */
    /* note: for Geographic (only proj that doesn't use meters),
     * adjust pixel size using input projection */
    if ( !P->ParamsPresent[PIXEL_SIZE] )
    {
        P->ParamsPresent[PIXEL_SIZE] = 1;
        *defpixsiz = 1;

	for ( i = 0; i < P->nbands; i++ )
	    P->bandinfo[i].output_pixel_size = P->bandinfo[i].pixel_size;

        /* if the input is Geographic then convert to meters. NOTE: the
           corner points must be valid and in lat/long before calling
           AdjustInputGeoPixelSizeProj (i.e. FixCornerPoints must have
           already been called) */
        if ( P->input_projection_type == PROJ_GEO &&
             P->output_projection_type != PROJ_GEO )
            AdjustInputGeoPixelSizeProj ( P );

        /* if the output is Geographic then convert to degrees. NOTE:
           the corner points must be valid and in lat/long before calling
           AdjustGeoPixelSizeProj (i.e. FixCornerPoints must have already
           been called) */
	if ( P->output_projection_type == PROJ_GEO &&
             P->input_projection_type != PROJ_GEO )
	    AdjustGeoPixelSizeProj( P );
    }

    /* optional UTM zone field - if present, overrides projection parameters */
    if ( !P->ParamsPresent[UTM_ZONE] )
        P->ParamsPresent[UTM_ZONE] = 1;

    /* check that all fields are present */
    for ( iparam = INPUT_FILENAME; iparam < NSTRINGS; iparam++ )
    {
        if ( !P->ParamsPresent[iparam] )
    	{
	    sprintf( s, "Missing required field %s in parameter file.",
                HeaderStrings[iparam] );
	    ErrorHandler( TRUE, "CheckOutputFields", ERROR_READ_INPUTPAR, s );
	    return ERROR_READ_INPUTPAR;
    	}
    }

    return 0;
}

/************************************************************************/

/*
 * CheckOutputFieldsHDF2RB - make sure all required fields are present
 *     in the parameter file and verify that the parameter combinations are
 *     valid.
 */

int CheckOutputFieldsHDF2RB
(
    char *HeaderStrings[],
    ModisDescriptor *P
)

{
    ParamType iparam;
    char s[LINE_BUFSIZ];
    size_t i;

    if( P->resampling_type == NO_RESAMPLE )
    {
    	CopyInputParametersToOutput( P );
    }

    /* default to no spectral subsetting */
    if ( !P->ParamsPresent[SPECTRAL_SUBSET] )
    {
        P->ParamsPresent[SPECTRAL_SUBSET] = 1;
	for ( i = 0; i < P->nbands; i++ )
	    P->bandinfo[i].selected = 1;
    }

    /* default to no spatial subsetting if UL or LR are not provided */
    if ( !P->ParamsPresent[SPATIAL_SUBSET_UL] ||
         !P->ParamsPresent[SPATIAL_SUBSET_LR] )
    {
        /* default to no spatial subsetting */
        P->ParamsPresent[SPATIAL_SUBSET_UL] = 1;
        P->ParamsPresent[SPATIAL_SUBSET_LR] = 1;

        /* fill the lat/long spatial corners with invalid values */
        P->ll_spac_sub_gring_corners[UL][0] = -999.0;
        P->ll_spac_sub_gring_corners[UL][1] = -999.0;
        P->ll_spac_sub_gring_corners[UR][0] = -999.0;
        P->ll_spac_sub_gring_corners[UR][1] = -999.0;
        P->ll_spac_sub_gring_corners[LL][0] = -999.0;
        P->ll_spac_sub_gring_corners[LL][1] = -999.0;
        P->ll_spac_sub_gring_corners[LR][0] = -999.0;
        P->ll_spac_sub_gring_corners[LR][1] = -999.0;
    }

    /* finish spatial subsetting */
    FixCornerPointsHDF2RB( P );

    /* default to same input and output pixel sizes */
    for ( i = 0; i < P->nbands; i++ )
        P->bandinfo[i].output_pixel_size = P->bandinfo[i].pixel_size;

    /* check that all fields are present */
    for ( iparam = INPUT_FILENAME; iparam < NSTRINGS; iparam++ )
    {
        /* skip over the check for certain parameters since they aren't
           used in the HDF2RB conversion */
        if (iparam == SPATIAL_SUBSET_TYPE || iparam == RESAMPLING_TYPE ||
            iparam == OUTPUT_PROJ_TYPE || iparam == OUTPUT_PROJ_PARMS ||
            iparam == PIXEL_SIZE || iparam == UTM_ZONE || iparam == DATUM)
            continue;

        if ( !P->ParamsPresent[iparam] )
    	{
	    sprintf( s, "Missing required field %s in parameter file.",
                HeaderStrings[iparam] );
	    ErrorHandler( TRUE, "CheckOutputFieldsHDF2RB", ERROR_READ_INPUTPAR,
                s );
	    return ERROR_READ_INPUTPAR;
    	}
    }

    return 0;
}

/************************************************************************/

/* determine input filename */
int GetInputFilename( char *str, ModisDescriptor *P ) {
   int  r, n = 0;
   char s[LINE_BUFSIZ];
   char *ptr = NULL;

   r = GetQuotedValue( str, &ptr, &n );
   if( r != 0 ) {
      if( r == 1 ) {
         sprintf( s, "Bad or missing INPUT_FILENAME field." );
         ErrorHandler( TRUE, "ReadParameterFile", ERROR_INPUTNAME_FIELD, s );
      } else if ( r == 2 ) {
         sprintf( s, "Bad INPUT_FILENAME field.  No ending quote found." );
         ErrorHandler( TRUE, "ReadParameterFile", ERROR_INPUTNAME_FIELD, s );
      } else if ( r == 3 ) {
         sprintf(s,"Bad INPUT_FILENAME field.  Empty filename within quotes.");
         ErrorHandler( TRUE, "ReadParameterFile", ERROR_INPUTNAME_FIELD, s );
      } else if ( r == 4 ) {
         sprintf( s, "Bad INPUT_FILENAME field.  Erroneous quote found." );
         ErrorHandler( TRUE, "ReadParameterFile", ERROR_INPUTNAME_FIELD, s );
      } else {
         sprintf(s, "Unable to allocate memory for input_filename.");
         ErrorHandler( TRUE, "ReadParameterFile", ERROR_MEMORY, s );
      }
   }

   if ( P->input_filename == NULL )
      P->input_filename = ptr;
   else
     free( ptr );

    /* return number of characters parsed */
    return n;
}

/************************************************************************/

/* empty the rest of the buffer for the INPUT_FILENAME parameter */
int ClearInputFilenameFromBuffer( char *str ) {
    int  r, n = 0;
    char s[LINE_BUFSIZ];

    r = GetQuotedValue( str, NULL, &n );
    if ( r != 0 ) {
        sprintf( s, "Error clearing the INPUT_FILENAME field." );
        ErrorHandler( TRUE, "ReadParameterFile", ERROR_INPUTNAME_FIELD, s );
        return ERROR_INPUTNAME_FIELD;
    }

    /* return number of characters parsed */
    return n;
}

/************************************************************************
The purpose of this function is to replace the 'sscanf( str, " = %s%n", s, &n )'
function call, but can handle quoted values.  In addition, if quoted, whitespce
is removed from the fron and back of the value (e.g. '" A Value "', is returned
as 'A Value').  If not quoted, this function acts like the sscanf().  If NULL
is passed in for the pv argument, then n is calculated, but no memory is
allocated.

Response:
  1 - Bad or missing field.
  2 - No closing quote.
  3 - Empty quoted string.
  4 - Erroneous or no starting quote.
  5 - Memory allocation error.
*/

int GetQuotedValue( char *str, char **pv, int *pvn ) {
    int len = 0, n = 0;
    char *bptr = NULL;
    char *tptr, *lastptr, *ptr = str;

    if( pv != NULL )
       *pv = NULL;
    *pvn = 0;

    /* Short term: Replace sscanf so that we can handle quoted values.
     * To mimic 'sscanf( str, " = %s%n", s, &n )', the varibale n is the
     * length of the total string, including whitespace and equal sign.
     * Long term: A better way of parsing should be devised overall.
     */

    /* Spin through white space. */
    while( isspace(*ptr) && *ptr != 0 ) ++ptr, ++n;
    /* Must be the equal sign. */
    if( *ptr != '=' ) {
       /* Bad or missing field */
       return 1;
    }
    ++ptr, ++n;
    /* Spin through white space. */
    while( isspace(*ptr) && *ptr != 0 ) ++ptr, ++n;
    /* If we have a quote, then we must have an ending quote and read
     * everything in between.  If no quote is present, then read until
     * until eol or whitespace.
     */
    if( *ptr == '"' ) {
       tptr = ptr;
       ++ptr; ++n;
       while( *ptr != '"' && *ptr != 0 ) ++ptr, ++n;
       if( *ptr == 0 ) {
          /* No closing quote, error */
          return 2;
       }
       ++n;
       lastptr = ptr + 1;
       --ptr;
       while( ptr != tptr && isspace(*ptr) ) --ptr;
       /* If whitespace or empty quoted string, then error. */
       if( ptr == tptr ) {
          return 3;
       }
       ++ptr; ++tptr;
       while( isspace(*tptr) && *tptr != 0 ) ++tptr;
       len = ptr - tptr;
    } else {
       tptr = ptr;
       while( !isspace(*ptr) && *ptr != 0 && *ptr != '"' ) ++ptr, ++n;
       if( *ptr == '"' ) {
          return 4;
       }
       lastptr = ptr;
       len = ptr - tptr;
    }
    if( pv != NULL ) {
       if( len > 0 ) {
          bptr = (char *)malloc( (len + 1) * sizeof(char) );

          /* if memory for input_filename can't be allocated, fatal error */
          if ( bptr == NULL ) {
             return 5;
          }
          strncpy( bptr, tptr, len );
          bptr[len] = 0;
       } else {
          return 1;
       }
    }

    /* sscanf also returns the number of white space after the value. */
    ptr = lastptr;
    while( isspace(*ptr) && *ptr != 0 ) ++ptr, ++n;

    if( pv != NULL )
       *pv = bptr;
    *pvn = n;
 
    return 0;
}


/************************************************************************/

/* determine output filename */
int GetOutputFilename( char *str, ModisDescriptor *P ) {
   int  r, n = 0;
   char s[LINE_BUFSIZ];
   char *ptr = NULL;

   r = GetQuotedValue( str, &ptr, &n );
   if( r != 0 ) {
      if( r == 1 ) {
         sprintf( s, "Bad or missing OUTPUT_FILENAME field." );
         ErrorHandler( TRUE, "ReadParameterFile", ERROR_OUTPUTNAME_FIELD, s );
      } else if ( r == 2 ) {
         sprintf( s, "Bad OUTPUT_FILENAME field.  No ending quote found." );
         ErrorHandler( TRUE, "ReadParameterFile", ERROR_OUTPUTNAME_FIELD, s );
      } else if ( r == 3 ) {
         sprintf(s,"Bad OUTPUT_FILENAME field.  Empty filename within quotes.");
         ErrorHandler( TRUE, "ReadParameterFile", ERROR_OUTPUTNAME_FIELD, s );
      } else if ( r == 4 ) {
         sprintf( s, "Bad OUTPUT_FILENAME field.  Erroneous quote found." );
         ErrorHandler( TRUE, "ReadParameterFile", ERROR_OUTPUTNAME_FIELD, s );
      } else if ( r != 0 ) {
         sprintf(s, "Unable to allocate memory for output_filename.");
         ErrorHandler( TRUE, "ReadParameterFile", ERROR_MEMORY, s );
      }
   }

   if ( P->output_filename == NULL )
      P->output_filename = ptr;
   else
     free( ptr );

   /* return number of characters parsed */
   return n;
}

/************************************************************************/

/* empty the rest of the buffer for the OUTPUT_FILENAME parameter */
int ClearOutputFilenameFromBuffer( char *str ) {
    int  r, n = 0;
    char s[LINE_BUFSIZ];

    r = GetQuotedValue( str, NULL, &n );
    if ( r != 0 ) {
        sprintf( s, "Error clearing the OUTPUT_FILENAME field." );
        ErrorHandler( TRUE, "ReadParameterFile", ERROR_OUTPUTNAME_FIELD, s );
        return ERROR_OUTPUTNAME_FIELD;
    }

    /* return number of characters parsed */
    return n;
}

/************************************************************************/

/* determine input file type */
int GetInputFileExt
(
    char input_filename[],     /* I: input filename */
    FileType *input_filetype   /* O: file type of the filename */
)

{
    char filename[LINE_BUFSIZ];
    char *ext = NULL;

    /* determine input file type from input filename extension */
    strcpy( filename, input_filename );
    ext = strrchr( filename, '.' );
    if (ext == NULL)
    {
        *input_filetype = BAD_FILE_TYPE;
        ErrorHandler( TRUE, "ReadParameterFile", ERROR_INPUT_EXTENSION,
            "Missing input_filetype extension. Valid extensions are "
            ".hdf or .hdr" );
        return ERROR_OUTPUT_EXTENSION;
    }

    strupr( ext );

    if ( strcmp( ext, ".HDR" ) == 0 )
	*input_filetype = RAW_BINARY;
    else if ( strcmp( ext, ".HDF" ) == 0 )
	*input_filetype = HDFEOS;
    else
    {
	*input_filetype = BAD_FILE_TYPE;
	ErrorHandler( TRUE, "ReadParameterFile", ERROR_INPUT_EXTENSION,
	    "Incorrect input_filetype extension" );
	return ERROR_INPUT_EXTENSION;
    }

    return MRT_NO_ERROR;
}

/************************************************************************/

/* determine output file type */

int GetOutputFileExt
(
    ModisDescriptor *P 
)

{
    char filename[LINE_BUFSIZ];
    char *ext = NULL;

    /* determine output file type from output filename extension */
    strcpy( filename, P->output_filename );
    ext = strrchr( filename, '.' );
    if (ext == NULL)
    {
        P->output_filetype = BAD_FILE_TYPE;
        ErrorHandler( TRUE, "ReadParameterFile", ERROR_OUTPUT_EXTENSION,
            "Missing output_filetype extension. Valid extensions are "
            ".hdf, .hdr, or .tif" );
        return ERROR_OUTPUT_EXTENSION;
    }

    strupr( ext );

    if ( strcmp( ext, ".HDR" ) == 0 )
	P->output_filetype = RAW_BINARY;
    else if ( strcmp( ext, ".HDF" ) == 0 )
	P->output_filetype = HDFEOS;
    else if ( strcmp( ext, ".TIF" ) == 0 )
	P->output_filetype = GEOTIFF;
    else
    {
	P->output_filetype = BAD_FILE_TYPE;
	ErrorHandler( TRUE, "ReadParameterFile", ERROR_OUTPUT_EXTENSION,
	    "Incorrect output_filetype extension" );
	return ERROR_OUTPUT_EXTENSION;
    }

    return MRT_NO_ERROR;
}

/************************************************************************/

/* determine spectral subsetting: SPECTRAL_SUBSET = ... */

int GetSpectralSubset
(
    char *str, 
    ModisDescriptor *P 
)

{
    size_t i;
    int    n, len, select;
    char   s[LINE_BUFSIZ];
    char   strval[LINE_BUFSIZ];

    /* read the open paren */
    sscanf( str, " = (%n", &len );
    if ( len < 1 )
    {
	sprintf( s,
            "Incorrect SPECTRAL_SUBSET field (bad or missing open paren).\n" );
	ErrorHandler( TRUE, "ReadParameterFile", ERROR_SPECTRALSUB_FIELD, s );
	return ERROR_SPECTRALSUB_FIELD;
    }
    str += len;

    /* read the values, without assuming correct number of bands */
    i = 0;
    while ( sscanf( str, "%s%n", strval, &n ) > 0 &&
            strcmp( strval, ")" ) != 0 )
    {
	if ( sscanf( strval, "%i", &select ) < 1 )
    	{
	    sprintf( s,
                "Incorrect SPECTRAL_SUBSET field (bad or missing value %s).\n",
                strval );
	    ErrorHandler( TRUE, "ReadParameterFile", ERROR_SPECTRALSUB_FIELD,
                s );
	    return ERROR_SPECTRALSUB_FIELD;
    	}

	if ( i < P->nbands )
	    P->bandinfo[i].selected = select;

	i++;
	str += n;
	len += n;
    }

    /* issues warning if wrong number of bands */
    if ( i != P->nbands )
    {
	sprintf( s, "Only " MRT_SIZE_T_FMT " out of " MRT_SIZE_T_FMT \
            " bands were addressed (0 or 1) in the " \
            "SPECTRAL_SUBSET field. The remaining bands will not be " \
            "processed (assumed to be 0).", i, P->nbands );
	ErrorHandler( FALSE, "ReadParameterFile", MRT_NO_ERROR, s );
    }

    /* do not select any unspecified bands */
    while ( i < P->nbands )
	P->bandinfo[i++].selected = 0;

    /* return number of characters parsed */
    return len + n;
}

/************************************************************************/

/* clear the buffer for SPECTRAL_SUBSET parameter */

int ClearSpectralSubsetFromBuffer
(
    char *str
)

{
    int i, n, len, select;
    char s[LINE_BUFSIZ];
    char strval[LINE_BUFSIZ];

    /* read the open paren */
    sscanf( str, " = (%n", &len );
    if ( len < 1 )
    {
        sprintf( s, "Error clearing the SPECTRAL_SUBSET field (bad or "
                    "missing open paren).\n" );
        ErrorHandler( TRUE, "ReadParameterFile", ERROR_SPECTRALSUB_FIELD, s );
        return ERROR_SPECTRALSUB_FIELD;
    }
    str += len;

    /* read the values, without assuming correct number of bands */
    i = 0;
    while ( sscanf( str, "%s%n", strval, &n ) > 0 &&
            strcmp( strval, ")" ) != 0 )
    {
        if ( sscanf( strval, "%i", &select ) < 1 )
        {
            sprintf( s, "Error clearing the SPECTRAL_SUBSET field (bad or "
                        "missing value %s).\n", strval );
            ErrorHandler( TRUE, "ReadParameterFile", ERROR_SPECTRALSUB_FIELD,
                s );
            return ERROR_SPECTRALSUB_FIELD;
        }

        i++;
        str += n;
        len += n;
    }

    /* return number of characters parsed */
    return len + n;
}

/************************************************************************/

/* determine spectral subsetting using command-line specified arguments */

void GetSpectralSubsetCmdLine(ModisDescriptor *P)
{
    size_t i;
    int    count, len, pos = 0;
    int    band;
    char  *str;

    /* we are going to parse the spectral subset string that was specified
       via the command-line arguments and stored temporarily in the MODIS
       descriptor.  the string has already been checked for valid values
       in the proc_arg routine. */
    str = P->tmpspectralsubset;

    /* read the band selections */
    for ( i = 0; i < P->nbands; i++ )
    {
	count = sscanf( str + pos, "%i%n", &band, &len );
	pos += len;
	if ( count < 1 )
	    break;
	P->bandinfo[i].selected = band;
    }

    /* deselect any remaining bands */
    for ( ; i < P->nbands; i++ )
        P->bandinfo[i].selected = 0;
}

/************************************************************************/

/* determine spatial subsetting type */

int GetSpatialSubsetType
(
    char *str,
    ModisDescriptor *P 
)

{
    int n;
    char s[LINE_BUFSIZ];
    char errmsg[LARGE_STRING];

    if ( sscanf( str, " = %s%n", s, &n ) != 1 || n < 1 )
    {
	sprintf( s,
            "Incorrect SPATIAL_SUBSET_TYPE field (bad or missing value %s).\n",
            s );
        ErrorHandler( TRUE, "ReadParameterFile", ERROR_SPATIAL_SUBSET_TYPE, s );
        return ERROR_SPATIAL_SUBSET_TYPE;
    }
    strupr( s );

    if ( strcmp( s, "INPUT_LAT_LONG" ) == 0 )
        P->spatial_subset_type = INPUT_LAT_LONG;
    else if ( strcmp( s, "INPUT_LINE_SAMPLE" ) == 0 )
        P->spatial_subset_type = INPUT_LINE_SAMPLE;
    else if ( strcmp( s, "OUTPUT_PROJ_COORDS" ) == 0 )
        P->spatial_subset_type = OUTPUT_PROJ_COORDS;
    else
    {
        P->spatial_subset_type = BAD_SPATIAL_SUBSET_TYPE;
	sprintf( errmsg, "Incorrect SPATIAL_SUBSET_TYPE field (bad value %s).\n"
            "     : Valid types are INPUT_LAT_LONG, INPUT_LINE_SAMPLE, "
            "and OUTPUT_PROJ_COORDS.\n", s );
        ErrorHandler( TRUE, "ReadParameterFile", ERROR_SPATIAL_SUBSET_TYPE,
            errmsg );
        return ERROR_SPATIAL_SUBSET_TYPE;
    }

    /* return number of characters parsed */
    return n;
}

/************************************************************************/

/* clear the buffer for the spatial subsetting type */

int ClearSpatialSubsetTypeFromBuffer
(
    char *str
)

{
    int n;
    char s[LINE_BUFSIZ];

    if ( sscanf( str, " = %s%n", s, &n ) != 1 || n < 1 )
    {
        sprintf( s,
            "Incorrect SPATIAL_SUBSET_TYPE field (bad or missing value %s).\n",
            s );
        ErrorHandler( TRUE, "ReadParameterFile", ERROR_SPATIAL_SUBSET_TYPE, s );        return ERROR_SPATIAL_SUBSET_TYPE;
    }

    /* return number of characters parsed */
    return n;
}

/*************************************************************************/

/* determine spatial subsetting: SPATIAL_SUBSET = ...
     * SPATIAL_SUBSET_UL_CORNER = ( lat long )          < float values >
     * SPATIAL_SUBSET_LR_CORNER = ( lat long )
     *              - or -
     * SPATIAL_SUBSET_UL_CORNER = ( line sample )       < int values >
     * SPATIAL_SUBSET_LR_CORNER = ( line sample )
     *              - or -
     * OUTPUT_PROJECTION_UL_CORNER = ( proj_x proj_y )  < float values >
     * OUTPUT_PROJECTION_LR_CORNER = ( proj_x proj_y )
*/

int GetSpatialSubsetting
(
    CornerType corner,
    char *str,
    ModisDescriptor *P
)

{
    int n;
    double val1, val2;
    char sval1[LINE_BUFSIZ], sval2[LINE_BUFSIZ];

    /* read corner coords */
    if ( sscanf( str, " = ( %s %s )%n", sval1, sval2, &n ) < 2 )
    {
	ErrorHandler( TRUE, "ReadParameterFile", ERROR_SPATIALSUB_FIELD, NULL );
	return ERROR_SPATIALSUB_FIELD;
    }

    /* read the corner value. this may be an integer or a float, so store
       as a float. */
    if ( sscanf( sval1, "%lf", &val1 ) < 1 ||
         sscanf( sval2, "%lf", &val2 ) < 1 )
    {
        ErrorHandler( TRUE, "ReadParameterFile", ERROR_SPATIALSUB_FIELD,
            "Missing lat/lon, row/column, or proj. x/y values" );
        return ERROR_SPATIALSUB_FIELD;
    }

    /* store corner point for spatial subsetting */
    P->ll_spac_sub_gring_corners[corner][0] = val1;
    P->ll_spac_sub_gring_corners[corner][1] = val2;

    /* return number of characters parsed */
    return n;
}

/*************************************************************************/

/* clear the buffer for SPATIAL_SUBSET parameter */

int ClearSpatialSubsetFromBuffer
(
    char *str
)

{
    int n;
    char s[LINE_BUFSIZ];
    char sval1[LINE_BUFSIZ], sval2[LINE_BUFSIZ];

    /* read corner coords */
    if ( sscanf( str, " = ( %s %s )%n", sval1, sval2, &n ) < 2 )
    {
        sprintf( s, "Error clearing the SPATIAL_SUBSET field" );
        ErrorHandler( TRUE, "ReadParameterFile", ERROR_SPATIALSUB_FIELD, s );
        return ERROR_SPATIALSUB_FIELD;
    }

    /* return number of characters parsed */
    return n;
}

/************************************************************************/

/* finish spatial subsetting */

int FixCornerPoints(ModisDescriptor *P)
{
    int    errval;
    size_t i;
    int    npixels = 0, maxpixels = 0, maxsamples = 0, maxlines = 0;

    if ( P->spatial_subset_type == INPUT_LAT_LONG )
    {   /* lat/lon values were used for spatial subsetting */
	if ( ( errval = GetInputRectangle( P ) ) != MRT_NO_ERROR )
	{
	    ErrorHandler( TRUE, "ReadParameterFile", errval,
		"Problem in GetInputRectangle" );
	    return errval;
	}
    }
    else if ( P->spatial_subset_type == INPUT_LINE_SAMPLE )
    {   /* line/sample values were used for spatial subsetting */
	/* store four corner points for spatial subsetting */
	P->ll_spac_sub_gring_corners[UR][0] =
            P->ll_spac_sub_gring_corners[UL][0];
	P->ll_spac_sub_gring_corners[UR][1] =
            P->ll_spac_sub_gring_corners[LR][1];
	P->ll_spac_sub_gring_corners[LL][0] =
            P->ll_spac_sub_gring_corners[LR][0];
	P->ll_spac_sub_gring_corners[LL][1] =
            P->ll_spac_sub_gring_corners[UL][1];

	if ( ( errval = ConvertRowCol2Deg( P ) ) != MRT_NO_ERROR )
	{
	    ErrorHandler( TRUE, "ReadParameterFile", errval,
		"Error converting row/col to degrees" );
	    return errval;
	}
    }
    else if ( P->spatial_subset_type == OUTPUT_PROJ_COORDS )
    {   /* output projection coordinates were specified, so use those
           exact coordinates for the output spatial subsetting */
        if ( ( errval = GetRectangle( P ) )
               != MRT_NO_ERROR )
        {
            ErrorHandler( TRUE, "ReadParameterFile", errval,
                "Error converting output projection coordinates to input "
                "coordinates" );
            return errval;
        }
    }

    /* for now, set highest rez line/sample spatial subsetting to entire
       image */
    for ( i = 0; i < P->nbands; i++ )
    {
	npixels = P->bandinfo[i].nsamples * P->bandinfo[i].nlines;
	if ( npixels > maxpixels )
	{
	    maxsamples = P->bandinfo[i].nsamples;
	    maxlines = P->bandinfo[i].nlines;
	    maxpixels = npixels;
	}
    }

    P->input_corner_points[UL][0] = 0;
    P->input_corner_points[UL][1] = 0;
    P->input_corner_points[UR][0] = 0;
    P->input_corner_points[UR][1] = maxsamples - 1;
    P->input_corner_points[LL][0] = maxlines - 1;
    P->input_corner_points[LL][1] = 0;
    P->input_corner_points[LR][0] = maxlines - 1;
    P->input_corner_points[LR][1] = maxsamples - 1;

    return 0;
}

/************************************************************************/

/* finish spatial subsetting for HDF to raw binary conversion */

int FixCornerPointsHDF2RB
(
    ModisDescriptor *P
)

{
    size_t i;
    int    npixels = 0, maxpixels = 0;
    double pixel_size = 0.0;

    /* get highest rez pixel size */
    for ( i = 0; i < P->nbands; i++ )
    {
	npixels = P->bandinfo[i].nsamples * P->bandinfo[i].nlines;
	if ( npixels > maxpixels )
	{
	    maxpixels = npixels;
            pixel_size = P->bandinfo[i].pixel_size;
	}
    }

    /* determine the output spatial subset projection corners */
    if ( P->ll_spac_sub_gring_corners[UL][0] == -999.0 ||
         P->ll_spac_sub_gring_corners[UL][1] == -999.0 )
    {   /* process the entire image */
        P->proj_extents_corners[UL][0] = P->orig_coord_corners[UL][0];
        P->proj_extents_corners[UL][1] = P->orig_coord_corners[UL][1];
        P->proj_extents_corners[LR][0] = P->orig_coord_corners[LR][0];
        P->proj_extents_corners[LR][1] = P->orig_coord_corners[LR][1];
    }
    else
    {   /* use the user-specified starting/ending line/sample values */
        P->proj_extents_corners[UL][0] = P->orig_coord_corners[UL][0] +
            P->ll_spac_sub_gring_corners[UL][1] * pixel_size;
        P->proj_extents_corners[UL][1] = P->orig_coord_corners[UL][1] -
            P->ll_spac_sub_gring_corners[UL][0] * pixel_size;
        P->proj_extents_corners[LR][0] = P->orig_coord_corners[UL][0] +
            P->ll_spac_sub_gring_corners[LR][1] * pixel_size;
        P->proj_extents_corners[LR][1] = P->orig_coord_corners[UL][1] -
            P->ll_spac_sub_gring_corners[LR][0] * pixel_size;
    }

    /* use UL and LR corners to get UR and LL corners */
    P->proj_extents_corners[UR][0] = P->proj_extents_corners[LR][0];
    P->proj_extents_corners[UR][1] = P->proj_extents_corners[UL][1];
    P->proj_extents_corners[LL][0] = P->proj_extents_corners[UL][0];
    P->proj_extents_corners[LL][1] = P->proj_extents_corners[LR][1];

    return 0;
}

/************************************************************************/

/* determine output resampling type */

int GetOutputResamplingType
(
    char *str,
    ModisDescriptor *P 
)

{
    int n;
    char s[LINE_BUFSIZ];

    if ( sscanf( str, " = %s%n", s, &n ) != 1 || n < 1 )
    {
	sprintf( s,
            "Incorrect RESAMPLING_TYPE field (bad or missing value %s).\n",
            s );
        ErrorHandler( TRUE, "ReadParameterFile", ERROR_RESAMPLE_TYPE, s );
        return ERROR_RESAMPLE_TYPE;
    }
    strupr( s );

    /* short-circuit resampling if user wants format conversion */
    if ( P->resampling_type == NO_RESAMPLE )
	return n;

    if ( strcmp( s, "NN" ) == 0 || strcmp( s, "NEAREST_NEIGHBOR" ) == 0 )
        P->resampling_type = NN;
    else if ( strcmp( s, "BI" ) == 0 || strcmp( s, "BILINEAR" ) == 0 )
        P->resampling_type = BI;
    else if ( strcmp( s, "CC" ) == 0 || strcmp( s, "CUBIC" ) == 0 ||
              strcmp( s, "CUBIC_CONVOLUTION" ) == 0 )
        P->resampling_type = CC;
    else if ( strcmp( s, "NONE" ) == 0 || strcmp( s, "NO_RESAMPLE" ) == 0 )
        P->resampling_type = NO_RESAMPLE;
    else
    {
        P->resampling_type = BAD_RESAMPLING_TYPE;
	sprintf( s, "Incorrect RESAMPLING_TYPE field (bad value %s).\n", s );
        ErrorHandler( TRUE, "ReadParameterFile", ERROR_RESAMPLE_TYPE, s );
        return ERROR_RESAMPLE_TYPE;
    }

    /* return number of characters parsed */
    return n;
}

/************************************************************************/

/* clear the buffer for RESAMPLING_TYPE */

int ClearOutputResampTypeFromBuffer
(
    char *str
)

{
    int n;
    char s[LINE_BUFSIZ];

    if ( sscanf( str, " = %s%n", s, &n ) != 1 || n < 1 )
    {
        sprintf( s, "Error clearing the RESAMPLING_TYPE field");
        ErrorHandler( TRUE, "ReadParameterFile", ERROR_RESAMPLE_TYPE, s );
        return ERROR_RESAMPLE_TYPE;
    }

    /* return number of characters parsed */
    return n;
}

/************************************************************************/

/* determine output projection type */

int GetOutputProjectionType
(
    char *str,
    ModisDescriptor *P 
)

{
    int n;
    char s[LINE_BUFSIZ];

    if ( sscanf( str, " = %s%n", s, &n ) != 1 || n < 1 )
    {
	sprintf( s,
            "Incorrect PROJECTION_TYPE field (bad or missing value %s).\n",
            s );
	ErrorHandler( TRUE, "ReadParameterFile", ERROR_PROJECTION_TYPE, s );
	return ERROR_PROJECTION_TYPE;
    }
    strupr( s );

    if ( strcmp( s, "AEA" ) == 0 || strcmp( s, "ALBERS_EQUAL_AREA" ) == 0 )
	P->output_projection_type = PROJ_AEA;

    else if ( strcmp( s, "ER" ) == 0 || strcmp( s, "EQUIRECTANGULAR" ) == 0 )
	P->output_projection_type = PROJ_ER;

    else if ( strcmp( s, "GEO" ) == 0 || strcmp( s, "GEOGRAPHIC" ) == 0 )
	P->output_projection_type = PROJ_GEO;

    else if ( strcmp( s, "HAM" ) == 0 || strcmp( s, "HAMMER" ) == 0 )
	P->output_projection_type = PROJ_HAM;

    else if ( strcmp( s, "IGH" ) == 0 ||
              strcmp( s, "INTERRUPTED_GOODE_HOMOLOSINE" ) == 0 )
	P->output_projection_type = PROJ_IGH;

    else if ( strcmp( s, "ISIN" ) == 0 ||
              strcmp( s, "INTEGERIZED_SINUSOIDAL" ) == 0 )
	P->output_projection_type = PROJ_ISIN;

    else if ( strcmp( s, "LA" ) == 0 || strcmp( s, "LAMBERT_AZIMUTHAL" ) == 0 )
	P->output_projection_type = PROJ_LA;

    else if ( strcmp( s, "LCC" ) == 0 ||
              strcmp( s, "LAMBERT_CONFORMAL_CONIC" ) == 0 )
	P->output_projection_type = PROJ_LCC;

    else if ( strcmp( s, "MOL" ) == 0 || strcmp( s, "MOLLWEIDE" ) == 0 )
	P->output_projection_type = PROJ_MOL;

    else if ( strcmp( s, "MERCAT" ) == 0 || strcmp( s, "MERCATOR" ) == 0 )
	P->output_projection_type = PROJ_MERC;

    else if ( strcmp( s, "PS" ) == 0 ||
              strcmp( s, "POLAR_STEREOGRAPHIC" ) == 0 )
	P->output_projection_type = PROJ_PS;

    else if ( strcmp( s, "SIN" ) == 0 || strcmp( s, "SINUSOIDAL" ) == 0 )
	P->output_projection_type = PROJ_SIN;

    else if ( strcmp( s, "TM" ) == 0 ||
              strcmp( s, "TRANSVERSE_MERCATOR" ) == 0 )
	P->output_projection_type = PROJ_TM;

    else if ( strcmp( s, "UTM" ) == 0 ||
              strcmp( s, "UNIVERSAL_TRANSVERSE_MERCATOR" ) == 0 )
	P->output_projection_type = PROJ_UTM;

    else
    {
	sprintf( s, "Incorrect PROJECTION_TYPE field (bad value %s).\n", s );
	ErrorHandler( TRUE, "ReadParameterFile", ERROR_PROJECTION_TYPE, s );
	return ERROR_PROJECTION_TYPE;
    }

    /* return number of characters parsed */
    return n;
}

/************************************************************************/

/* clear the buffer for PROJECTION_TYPE */

int ClearOutputProjTypeFromBuffer
(
    char *str
)

{
    int n;
    char s[LINE_BUFSIZ];

    if ( sscanf( str, " = %s%n", s, &n ) != 1 || n < 1 )
    {
        sprintf( s, "Error clearing PROJECTION_TYPE field ");
        ErrorHandler( TRUE, "ReadParameterFile", ERROR_PROJECTION_TYPE, s );
        return ERROR_PROJECTION_TYPE;
    }

    /* return number of characters parsed */
    return n;
}

/************************************************************************/

/* determine output projection parameters:
   PROJECTION_PARAMETERS = ( p1 p2 ... p15 ) */

int GetOutputProjectionParameters
(
    char *str,
    ModisDescriptor *P 
)

{
    int i, n, len;
    double pp;
    char s[LINE_BUFSIZ];
    char strval[LINE_BUFSIZ];

    /* read the open paren */
    sscanf( str, " = (%n", &len );
    if ( len < 1 )
    {
	sprintf( s, "Incorrect PROJECTION_PARAMETERS field (bad or missing "
            "open paren).\n" );
	ErrorHandler( TRUE, "ReadParameterFile", ERROR_PROJPARAMS_VALUE, s );
	return ERROR_PROJPARAMS_VALUE;
    }
    str += len;

    /* read the projection parameters, without assuming the correct number of
       values */
    i = 0;
    while ( sscanf( str, "%s%n", strval, &n ) > 0 &&
            strcmp( strval, ")" ) != 0 )
    {
	if ( sscanf( strval, "%lf", &pp ) < 1 )
    	{
	    sprintf( s, "Incorrect PROJECTION_PARAMETERS field (bad or "
                "missing value %s).\n", strval );
	    ErrorHandler( TRUE, "ReadParameterFile", ERROR_PROJPARAMS_VALUE,
                s );
	    return ERROR_PROJPARAMS_VALUE;
    	}

	if ( i < 15 )
	    P->output_projection_parameters[i] = pp;

	i++;
	str += n;
	len += n;
    }

    /* issues warning if wrong number of parameters */
    if ( i != 15 )
    {
	sprintf( s,
           "Incorrect number of parameters in PROJECTION_PARAMETERS field.\n" );
	ErrorHandler( FALSE, "ReadParameterFile", ERROR_PROJPARAMS_VALUE, s );
    }

    /* return number of characters parsed */
    return len + n;
}

/************************************************************************/

/* clear the buffer for PROJECTION_PARAMETERS = ( p1 p2 ... p15 ) */

int ClearOutputProjParamsFromBuffer
(
    char *str
)

{
    int i, n, len;
    char s[LINE_BUFSIZ];
    char strval[LINE_BUFSIZ];

    /* read the open paren */
    sscanf( str, " = (%n", &len );
    if ( len < 1 )
    {
        sprintf( s, "Error clearing PROJECTION_PARAMETERS field (bad "
                    "or missing open paren).\n" );
        ErrorHandler( TRUE, "ReadParameterFile", ERROR_PROJPARAMS_VALUE, s );
        return ERROR_PROJPARAMS_VALUE;
    }
    str += len;

    /* read the projection parameters, without assuming the correct number of
       values */
    i = 0;
    while ( sscanf( str, "%s%n", strval, &n ) > 0 &&
            strcmp( strval, ")" ) != 0 )
    {
        i++;
        str += n;
        len += n;
    }

    /* return number of characters parsed */
    return len + n;
}

/*******************************************************************/

/* determine pixel size: OUTPUT_PIXEL_SIZE = ... 
 *          02/23	Rob Burrell	 Use a units parameter */

int GetOutputPixelSize
(
    char *str,
    ModisDescriptor *P 
)

{
    size_t i;
    int    n;
    double pixel_size;
    char s[LINE_BUFSIZ];

    /* scan output pixel size field */
    if ( sscanf( str, " = %lf%n", &pixel_size, &n ) < 1 )
    {
	sprintf( s,
            "Incorrect OUTPUT_PIXEL_SIZE field (bad or missing value).\n" );
	ErrorHandler( TRUE, "ReadParameterFile", ERROR_PIXELSIZE_VALUE, s );
	return ERROR_PIXELSIZE_VALUE;
    }

    /* short-circuit resampling if user wants format conversion */
    if ( P->resampling_type == NO_RESAMPLE )
    {
	for ( i = 0; i < P->nbands; i++ )
	    P->bandinfo[i].output_pixel_size = P->bandinfo[i].pixel_size;
    }
    else	/* store specified output pixel size in all bands */
    {
	for ( i = 0; i < P->nbands; i++ )
	    P->bandinfo[i].output_pixel_size = pixel_size;
    }

    /* return value is number of characters parsed */
    return n;
}

/*******************************************************************/

/* clear the buffer for OUTPUT_PIXEL_SIZE = ... */

int ClearOutputPixelSizeFromBuffer
(
    char *str
)

{
    int n;
    double pixel_size;
    char s[LINE_BUFSIZ];

    /* scan output pixel size field */
    if ( sscanf( str, " = %lf%n", &pixel_size, &n ) < 1 )
    {
        sprintf( s, "Error clearing OUTPUT_PIXEL_SIZE field (bad or "
                    "missing value).\n" );
        ErrorHandler( TRUE, "ReadParameterFile", ERROR_PIXELSIZE_VALUE, s );
        return ERROR_PIXELSIZE_VALUE;
    }

    /* return value is number of characters parsed */
    return n;
}

/*******************************************************************/

/* determine UTM zone: UTM_ZONE = ... */

int GetOutputUTMZone
(
    char *str,
    ModisDescriptor *P
)

{
    int n, zone;
    char s[LINE_BUFSIZ];

    /* scan output pixel size field */
    if ( sscanf( str, " = %i%n", &zone, &n ) < 1 )
    {
        sprintf( s, "Incorrect UTM_ZONE field (bad or missing value).\n" );
        ErrorHandler( TRUE, "ReadParameterFile", ERROR_UTMZONE_FIELD, s );
        return ERROR_UTMZONE_FIELD;
    }

    if (-60 <= zone && zone <= 60)
        P->output_zone_code = zone;
    else
    {
        sprintf( s, "Incorrect UTM_ZONE field (bad or missing value).\n" );
        ErrorHandler( TRUE, "ReadParameterFile", ERROR_UTMZONE_VALUE, s );
        return ERROR_UTMZONE_VALUE;
    }

    /* return value is number of characters parsed */
    return n;
}

/*******************************************************************/

/* clear the buffer for UTM_ZONE = ... */

int ClearOutputUTMZoneFromBuffer
(
    char *str
)

{
    int n, zone;
    char s[LINE_BUFSIZ];

    /* scan output pixel size field */
    if ( sscanf( str, " = %i%n", &zone, &n ) < 1 )
    {
        sprintf( s, "Error clearing UTM_ZONE field (bad or missing value).\n" );        ErrorHandler( TRUE, "ReadParameterFile", ERROR_UTMZONE_FIELD, s );
        return ERROR_UTMZONE_FIELD;
    }

    /* return value is number of characters parsed */
    return n;
}

/******************************************************************************

MODULE:  GetOutputDatum

PURPOSE:  Read the datum from a parameter file

RETURN VALUE:
Type = int
Value           Description
-----           -----------
n               Number of characters parsed

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         04/01  Rob Burrell            Add ellipse codes for UTM and Geo
         12/01  Gail Schmidt           Changed to support datums for all
                                       projections
  
NOTES:

******************************************************************************/
int GetOutputDatum
(
    char *str,
    ModisDescriptor *P
)

{
    int n;
    char datumtype[LINE_BUFSIZ];
    char errmsg[LARGE_STRING];

    if ( sscanf( str, " = %s%n", datumtype, &n ) != 1 || n < 1 )
    {
        sprintf( errmsg,
            "Incorrect DATUM field (bad or missing value %s).",
            datumtype );
        ErrorHandler( FALSE, "ReadParameterFile", ERROR_DATUM_FIELD,
            errmsg );
        return ERROR_DATUM_FIELD;
    }
    strupr( datumtype );

    if ( strcmp( datumtype, "NAD27" ) == 0 )
        P->output_datum_code = E_NAD27;
    else if ( strcmp( datumtype, "NAD83" ) == 0 )
        P->output_datum_code = E_NAD83;
    else if ( strcmp( datumtype, "WGS66" ) == 0 )
        P->output_datum_code = E_WGS66;
    else if ( strcmp( datumtype, "WGS72" ) == 0 )
        P->output_datum_code = E_WGS72;
    else if ( strcmp( datumtype, "WGS84" ) == 0 )
        P->output_datum_code = E_WGS84;
    else if ( strcmp( datumtype, "NODATUM" ) == 0 )
        P->output_datum_code = E_NODATUM;
    else
    {
        sprintf( errmsg, "Incorrect DATUM_CODE field (bad value %s).\n"
            "     : Valid datum types are NAD27, NAD83, WGS66, WGS72, WGS84, "
            "and NODATUM.\n", datumtype );
        ErrorHandler( FALSE, "ReadParameterFile", ERROR_DATUM_VALUE,
            errmsg );
        return ERROR_DATUM_VALUE;
    }

    /* return number of characters parsed */
    return n;
}
