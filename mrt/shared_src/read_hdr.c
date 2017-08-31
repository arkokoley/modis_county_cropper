/******************************************************************************

FILE:  read_hdr.c

PURPOSE:  Multifile header reading

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  John Weiss             Original Development
         04/01  Rob Burrell            Add GetInputEllipseCode for UTM and Geo

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/

/* include files */
#include <stdio.h>		/* fopen, fprintf, fscanf, etc. */
#include <stdlib.h>		/* malloc */
#include <string.h>		/* strdup, strcmp, etc. */
#if !defined(__CYGWIN__) && !defined(__APPLE__) && !defined(WIN32)
#include <values.h>
#endif
#include <math.h>
#include "mrt_dtype.h"
#include "worgen.h"
#include "shared_mosaic.h"

/* constants */
#define FILE_BUFSIZ (64*1024)
#define LINE_BUFSIZ (5*1024)

/* local function prototypes */
static int SetBackgroundFill ( int nbands, BandType *bandinfo, int have_min,
    int have_max );
static int CheckFields ( int nstrings, int StringsPresent[],
    ModisDescriptor *P );
static int CheckFieldsMosaic ( int nstrings, int StringsPresent[],
    MosaicDescriptor *P );
static int GetByteOrder ( char *tmpstr, MrtEndianness *input_byte_order );


/* Create some defines to make the code a little easier
 * to read.  The enums in this list must match those in the
 * HeaderStrings[] array below.
 */
typedef enum { MRT_PROJECTION_TYPE,         /* 0 */
               MRT_PROJECTION_PARAMETERS,   /* 1 */
               MRT_UL_CORNER_LATLON,
               MRT_UR_CORNER_LATLON,
               MRT_LL_CORNER_LATLON,
               MRT_LR_CORNER_LATLON,        /* 5 */
               MRT_NBANDS,
               MRT_BANDNAMES,
               MRT_DATA_TYPE,
               MRT_NLINES,
               MRT_NSAMPLES,                /* 10 */
               MRT_PIXEL_SIZE,
               MRT_MIN_VALUE,
               MRT_MAX_VALUE,
               MRT_BACKGROUND_FILL,
               MRT_DATUM,                   /* 15 */
               MRT_UTM_ZONE,
               MRT_BYTE_ORDER } HeaderStringsEnums;

static char *HeaderStrings[] = {
        "PROJECTION_TYPE",         /* 0 */
        "PROJECTION_PARAMETERS",   /* 1 */
        "UL_CORNER_LATLON",
        "UR_CORNER_LATLON",
        "LL_CORNER_LATLON",
        "LR_CORNER_LATLON",        /* 5 */
        "NBANDS",
        "BANDNAMES",
        "DATA_TYPE",
        "NLINES",
        "NSAMPLES",                /* 10 */
        "PIXEL_SIZE",
        "MIN_VALUE",
        "MAX_VALUE",
        "BACKGROUND_FILL",
        "DATUM",                   /* 15 */
        "UTM_ZONE",
        "BYTE_ORDER"
    };
#define MRT_NUMBER_OF_HEADER_STRINGS  (sizeof(HeaderStrings)/sizeof(HeaderStrings[0]))

/******************************************************************************

MODULE:  ReadHeaderFile

PURPOSE:  Read a multifile header and fills in ModisDescriptor

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status		See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  John Weiss             Original Development
         05/00  John Weiss             added TOTAL_BANDS field
         05/00  John Weiss             deleted FILENAME, METAFILENAME fields
         05/00  John Weiss             Swapped order of TOTAL_BANDS and NFILES
                                       fields.
         07/00  Rob Burrell	       Error handling return values
         10/00  John Weiss             Upgrade to v.1.04 spec.
         01/01  John Rishea            Standardized formatting
         01/01  John Rishea            Removed test code
         01/01  John Rishea            Moved local prototype to loc_prot.h
         01/01  John Rishea            Updated error codes 
         04/01  John Weiss             Background fill defaults
         01/02  Gail Schmidt           Support for input datums and removed
                                       ellipsoid support
         11/02  Gail Schmidt           Added support for Albers Equal Area.
         07/03  Gail Schmidt           Added support for Equirectangular
  
NOTES:

******************************************************************************/

int ReadHeaderFile
(
    ModisDescriptor *P		/* I/O:  session info */
)

{
    int valid_string;
    int nstrings = 0;
    int err, i, n = 0;
    FILE *fp = NULL;
    char filebuf[FILE_BUFSIZ] = { 0 }, linebuf[LINE_BUFSIZ], token[LINE_BUFSIZ];
    char error_str[LINE_BUFSIZ];
    char *bufptr = NULL,
         *capbuf = NULL;
    int StringsPresent[MRT_NUMBER_OF_HEADER_STRINGS] = { 0 };

    /*******************************************************************/

    /* count fieldname strings */
    nstrings = MRT_NUMBER_OF_HEADER_STRINGS;

    /* open header file for reading */
    if ( ( fp = fopen( P->input_filename, "r" ) ) == NULL )
    {
	sprintf( error_str, "Unable to open %s", P->input_filename );
	ErrorHandler( TRUE, "ReadHeaderFile", ERROR_OPEN_INPUTHEADER,
            error_str );
	return ERROR_OPEN_INPUTHEADER;
    }

    /*******************************************************************/

    /* read each line of file, clean it up, and store in buffer */
    while ( fgets( linebuf, LINE_BUFSIZ, fp ) )
    {
	if ( (err = CleanupLine( linebuf )) == 0 )
	    strcat( filebuf, linebuf );
        else if( err > 0 ) {
           if( err == 1 )
              sprintf( error_str, "Internal buffer not big enough for " \
                       "line found in %s", P->input_filename );
           else if( err == 2 )
              sprintf( error_str, "Environment variable [%s] found in file "\
                       "%s, but the variable is not defined in environment.", 
                       linebuf, P->input_filename );
           else
              strcpy( error_str, "Unknown Internal Error!!!" );
           ErrorHandler( TRUE, "ReadHeaderFile", ERROR_READ_INPUTHEADER,
               error_str );
        }
    }
    fclose( fp );

    /*******************************************************************/

    /* store capitalized version of filebuf, to make NBANDS field
       recognition easier */
    capbuf = strdup( filebuf );

    /* if memory for capbuf can't be allocated, fatal error. JR */
    if ( capbuf == NULL )
    {
        /* display error msg and return error */
	sprintf( error_str, "Unable to allocate strdup memory for capbuf." );
	ErrorHandler( TRUE, "ReadHeaderFile", ERROR_MEMORY, error_str );
	return ERROR_MEMORY;
    }

    strupr( capbuf );

    /* first determine number of bands: NBANDS = ... */
    if ( (bufptr = strstr( capbuf, "NBANDS" ) ) == NULL )
    {
        /* free the capbuf */
        free ( capbuf );

        /* display error msg and return error */
	sprintf( error_str, "Missing or invalid NBANDS field" );
	ErrorHandler( TRUE, "ReadHeaderFile", ERROR_READ_INPUTHEADER,
            error_str );
	return ERROR_READ_INPUTHEADER;
    }

    if ( ( n = GetNBands( bufptr + strlen( "NBANDS" ), &P->nbands,
        &P->bandinfo ) ) < 1 )
    {
        /* free the capbuf */
        free ( capbuf );

        /* display error msg and return error */
	sprintf( error_str, "Missing or invalid NBANDS field" );
	ErrorHandler( TRUE, "ReadHeaderFile", ERROR_READ_INPUTHEADER,
            error_str );
	return ERROR_READ_INPUTHEADER;
    }

    free( capbuf );

    /*******************************************************************/

    /* read tokens from file buffer */
    bufptr = filebuf;
    while ( sscanf( bufptr, "%s%n", token, &n ) > 0 )
    {
	/* got next token, so advance file buffer pointer */
	bufptr += n;

	/* check for valid strings */
	valid_string = 0;
	for ( i = 0; i < nstrings; i++ )
	{
	    /* check for match to fieldname */
	    if ( strcasecmp( token, HeaderStrings[i] ) == 0 )
	    {
		/* check for multiply defined fields */
		if ( StringsPresent[i] )
		{
		    sprintf( error_str, "Multiply defined header field %s",
                        token );
		    ErrorHandler( TRUE, "ReadHeaderFile",
                        ERROR_READ_INPUTHEADER, error_str );
		    return ERROR_READ_INPUTHEADER;
		}

		/* got a fieldname match, check it out */
		StringsPresent[i] = 1;
		valid_string = 1;
		break;
	    }
	}

	/* make sure we have a valid field name */
	if ( !valid_string )
	{
	    sprintf( error_str, "Invalid header field %s", token );
	    ErrorHandler( TRUE, "ReadHeaderFile", ERROR_READ_INPUTHEADER,
                error_str );
	    return ERROR_READ_INPUTHEADER;
	}

	/* parse remainder of field */
	switch ( i )
	{
	    case MRT_PROJECTION_TYPE:
                /* determine input projection type: PROJECTION_TYPE = ... */
		n = GetProjectionType( bufptr, &P->input_projection_type );
		break;

	    case MRT_PROJECTION_PARAMETERS:
                /* determine input projection parameters:
                   PROJECTION_PARAMETERS = ... */
		n = GetProjectionParameters( bufptr,
                    P->input_projection_parameters );
		break;

	    case MRT_UL_CORNER_LATLON:
                /* determine input corner coordinates:
                   UL_CORNER_LATLON = ... */
		n = GetCornerPoints( UL, bufptr, P->input_image_extent,
                    P->ll_extents_corners );
		break;

	    case MRT_UR_CORNER_LATLON:
                /* determine input corner coordinates:
                   UR_CORNER_LATLON = ... */
		n = GetCornerPoints( UR, bufptr, P->input_image_extent,
                    P->ll_extents_corners );
		break;

	    case MRT_LL_CORNER_LATLON:
                /* determine input corner coordinates:
                   LL_CORNER_LATLON = ... */
		n = GetCornerPoints( LL, bufptr, P->input_image_extent,
                    P->ll_extents_corners );
		break;

	    case MRT_LR_CORNER_LATLON:
                /* determine input corner coordinates:
                   LR_CORNER_LATLON = ... */
		n = GetCornerPoints( LR, bufptr, P->input_image_extent,
                    P->ll_extents_corners );
		break;

	    case MRT_NBANDS:
                /* determine number of bands: NBANDS = ... */
		n = GetNBands( bufptr, &P->nbands, &P->bandinfo );
		break;

	    case MRT_BANDNAMES:
                /* determine band names: BANDNAMES = ... */
		n = GetBandNames( bufptr, P->nbands, P->bandinfo );
		break;

	    case MRT_DATA_TYPE:
                /* determine datatype of each band: DATA_TYPE = ... */
		n = GetDataTypes( bufptr, P->nbands, P->bandinfo );
		break;

	    case MRT_NLINES:
                /* determine number of lines per band: NLINES = ... */
		n = GetNLines( bufptr, P->nbands, P->bandinfo );
		break;

	    case MRT_NSAMPLES:
                /* determine number of samples per band: NSAMPLES = ... */
		n = GetNSamples( bufptr, P->nbands, P->bandinfo );
		break;

	    case MRT_PIXEL_SIZE:
                /* determine pixel size of each band: PIXEL_SIZE = ... */
		n = GetPixelSize( bufptr, P->nbands, P->bandinfo );
		break;

	    case MRT_MIN_VALUE:
                /* determine minimum value in each band: MIN_VALUE = ... */
		n = GetMinValue( bufptr, P->nbands, P->bandinfo );
		break;

	    case MRT_MAX_VALUE:
                /* determine maximum value in each band: MAX_VALUE = ... */
		n = GetMaxValue( bufptr, P->nbands, P->bandinfo );
		break;

	    case MRT_BACKGROUND_FILL:
                /* determine background fill value in each band:
                   BACKGROUND_FILL = ... */
		n = GetBackgroundFill( bufptr, P->nbands, P->bandinfo );
		break;

            case MRT_DATUM:
                /* determine datum value: DATUM = ... */
                n = GetInputDatum( bufptr, &P->input_datum_code );
                break;

            case MRT_UTM_ZONE:
                /* determine UTM zone value: UTM_ZONE = ... */
                n = GetUtmZone( bufptr, &P->input_zone_code );
                break;

            case MRT_BYTE_ORDER:
                /* determine Raw Binary data endianness: BYTE_ORDER = ... */
                n = GetByteOrder( bufptr, &P->input_file_endian );
                break;
	}

	/* make sure we got a valid field */
	if ( n < 1 )
	{
	    sprintf( error_str, "Invalid header field %s", token );
	    ErrorHandler( TRUE, "ReadHeaderFile", ERROR_READ_INPUTHEADER,
                error_str );
	    return ERROR_READ_INPUTHEADER;
	}

	/* advance file buffer pointer to next field */
	bufptr += n;
    }

    /*******************************************************************/

    /* check to see if all required fields are filled in,
     * and use default values for missing optional fields
     */
    CheckFields ( nstrings, StringsPresent, P );

    /*******************************************************************/

    /* finish up */
    return 0;
}

/******************************************************************************

MODULE:  ReadHeaderFileMosaic

PURPOSE:  Read a multifile header and fills in MosaicDescrptor

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status          See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         04/02  Gail Schmidt           Original Development
         01/07  Gail Schmidt           Modified the call to GCTP to send in
                                       the sphere code which will be
                                       used for UTM only

NOTES:

******************************************************************************/

int ReadHeaderFileMosaic
(
    MosaicDescriptor *mosaic            /* I/O:  session info */
)

{
    int valid_string;
    int nstrings = 0;
    int err, i, j, n = 0;
    FILE *fp = NULL;
    char filebuf[FILE_BUFSIZ] = { 0 }, linebuf[LINE_BUFSIZ], token[LINE_BUFSIZ];
    char error_str[LINE_BUFSIZ];
    char *bufptr = NULL,
         *capbuf = NULL;
    int StringsPresent[MRT_NUMBER_OF_HEADER_STRINGS] = { 0 };
    ProjInfo *inproj, *geoproj;  /* input/geographic projection data structs */
    ProjInfo projinfo;           /* local for geographic projection */
    int status = MRT_NO_ERROR;       /* error code status */
    long prtprm[2];              /* geolib terminal printing flags */

    /*******************************************************************/

    /* count fieldname strings */
    nstrings = MRT_NUMBER_OF_HEADER_STRINGS;

    /* open header file for reading */
    if ( ( fp = fopen( mosaic->filename, "r" ) ) == NULL )
    {
        sprintf( error_str, "Unable to open %s", mosaic->filename );
        ErrorHandler( TRUE, "ReadHeaderFileMosaic", ERROR_OPEN_INPUTHEADER,
            error_str );
        return ERROR_OPEN_INPUTHEADER;
    }

    /*******************************************************************/

    /* read each line of file, clean it up, and store in buffer */
    while ( fgets( linebuf, LINE_BUFSIZ, fp ) )
    {
        if ( (err = CleanupLine( linebuf )) == 0 )
            strcat( filebuf, linebuf );
        else if( err > 0 ) {
           if( err == 1 )
              sprintf( error_str, "Internal buffer not big enough for "\
                       "line found in %s", mosaic->filename );
           else if( err == 2 )
              sprintf( error_str, "Environment variable [%s] found in file "\
                       "%s, but the variable is not defined in environment.",
                       linebuf, mosaic->filename );
           else
              strcpy( error_str, "Unknown Internal Error!!!" );
           ErrorHandler( TRUE, "ReadHeaderFile", ERROR_READ_INPUTHEADER,
               error_str );
        }
    }
    fclose( fp );

    /*******************************************************************/

    /* store capitalized version of filebuf, to make NBANDS field
       recognition easier */
    capbuf = strdup( filebuf );

    /* if memory for capbuf can't be allocated, fatal error. JR */
    if ( capbuf == NULL )
    {
        /* display error msg and return error */
        sprintf( error_str, "Unable to allocate strdup memory for capbuf." );
        ErrorHandler( TRUE, "ReadHeaderFileMosaic", ERROR_MEMORY, error_str );
        return ERROR_MEMORY;
    }

    strupr( capbuf );

    /* first determine number of bands: NBANDS = ... */
    if ( (bufptr = strstr( capbuf, "NBANDS" ) ) == NULL )
    {
        /* free the capbuf */
        free ( capbuf );

        /* display error msg and return error */
        sprintf( error_str, "Missing or invalid NBANDS field" );
        ErrorHandler( TRUE, "ReadHeaderFileMosaic", ERROR_READ_INPUTHEADER,
            error_str );
        return ERROR_READ_INPUTHEADER;
    }

    if ( ( n = GetNBands( bufptr + strlen( "NBANDS" ), &mosaic->nbands,
        &mosaic->bandinfo ) ) < 1 )
    {
        /* free the capbuf */
        free ( capbuf );

        /* display error msg and return error */
        sprintf( error_str, "Missing or invalid NBANDS field" );
        ErrorHandler( TRUE, "ReadHeaderFileMosaic", ERROR_READ_INPUTHEADER,
            error_str );
        return ERROR_READ_INPUTHEADER;
    }

    free( capbuf );

    /*******************************************************************/

    /* read tokens from file buffer */
    bufptr = filebuf;
    while ( sscanf( bufptr, "%s%n", token, &n ) > 0 )
    {
        /* got next token, so advance file buffer pointer */
        bufptr += n;

        /* check for valid strings */
        valid_string = 0;
        for ( i = 0; i < nstrings; i++ )
        {
            /* check for match to fieldname */
            if ( strcasecmp( token, HeaderStrings[i] ) == 0 )
            {
                /* check for multiply defined fields */
                if ( StringsPresent[i] )
                {
                    sprintf( error_str, "Multiply defined header field %s",
                        token );
                    ErrorHandler( TRUE, "ReadHeaderFileMosaic",
                        ERROR_READ_INPUTHEADER, error_str );
                    return ERROR_READ_INPUTHEADER;
                }

                /* got a fieldname match, check it out */
                StringsPresent[i] = 1;
                valid_string = 1;
                break;
            }
        }

        /* make sure we have a valid field name */
        if ( !valid_string )
        {
            sprintf( error_str, "Invalid header field %s", token );
            ErrorHandler( TRUE, "ReadHeaderFileMosaic", ERROR_READ_INPUTHEADER,
                error_str );
            return ERROR_READ_INPUTHEADER;
        }

        /* parse remainder of field */
        switch ( i )
        {
            case MRT_PROJECTION_TYPE:
                /* determine input projection type: PROJECTION_TYPE = ... */
                n = GetProjectionType( bufptr, &mosaic->projection_type );
                break;

            case MRT_PROJECTION_PARAMETERS:
                /* determine input projection parameters:
                   PROJECTION_PARAMETERS = ... */
                n = GetProjectionParameters( bufptr,
                    mosaic->projection_parameters );
                break;

            case MRT_UL_CORNER_LATLON:
                /* determine input corner coordinates:
                   UL_CORNER_LATLON = ... */
                n = GetCornerPoints( UL, bufptr, mosaic->ll_image_extent,
                    mosaic->proj_image_extent );
                break;

            case MRT_UR_CORNER_LATLON:
                /* determine input corner coordinates:
                   UR_CORNER_LATLON = ... */
                n = GetCornerPoints( UR, bufptr, mosaic->ll_image_extent,
                    mosaic->proj_image_extent );
                break;

            case MRT_LL_CORNER_LATLON:
                /* determine input corner coordinates:
                   LL_CORNER_LATLON = ... */
                n = GetCornerPoints( LL, bufptr, mosaic->ll_image_extent,
                    mosaic->proj_image_extent );
                break;

            case MRT_LR_CORNER_LATLON:
                /* determine input corner coordinates:
                   LR_CORNER_LATLON = ... */
                n = GetCornerPoints( LR, bufptr, mosaic->ll_image_extent,
                    mosaic->proj_image_extent );
                break;

            case MRT_NBANDS:
                /* determine number of bands: NBANDS = ... */
                n = GetNBands( bufptr, &mosaic->nbands, &mosaic->bandinfo );
                break;

            case MRT_BANDNAMES:
                /* determine band names: BANDNAMES = ... */
                n = GetBandNames( bufptr, mosaic->nbands, mosaic->bandinfo );
                break;

            case MRT_DATA_TYPE:
                /* determine datatype of each band: DATA_TYPE = ... */
                n = GetDataTypes( bufptr, mosaic->nbands, mosaic->bandinfo );
                break;

            case MRT_NLINES:
                /* determine number of lines per band: NLINES = ... */
                n = GetNLines( bufptr, mosaic->nbands, mosaic->bandinfo );
                break;

            case MRT_NSAMPLES:
                /* determine number of samples per band: NSAMPLES = ... */
                n = GetNSamples( bufptr, mosaic->nbands, mosaic->bandinfo );
                break;

            case MRT_PIXEL_SIZE:
                /* determine pixel size of each band: PIXEL_SIZE = ... */
                n = GetPixelSize( bufptr, mosaic->nbands, mosaic->bandinfo );
                break;

            case MRT_MIN_VALUE:
                /* determine minimum value in each band: MIN_VALUE = ... */
                n = GetMinValue( bufptr, mosaic->nbands, mosaic->bandinfo );
                break;

            case MRT_MAX_VALUE:
                /* determine maximum value in each band: MAX_VALUE = ... */
                n = GetMaxValue( bufptr, mosaic->nbands, mosaic->bandinfo );
                break;

            case MRT_BACKGROUND_FILL:
                /* determine background fill value in each band:
                   BACKGROUND_FILL = ... */
                n = GetBackgroundFill( bufptr, mosaic->nbands,
                    mosaic->bandinfo );
                break;

            case MRT_DATUM:
                /* determine datum value: DATUM = ... */
                n = GetInputDatum( bufptr, &mosaic->datum_code );
                break;

            case MRT_UTM_ZONE:
                /* determine UTM zone value: UTM_ZONE = ... */
                n = GetUtmZone( bufptr, &mosaic->zone_code );
                break;

            case MRT_BYTE_ORDER:
                /* determine Raw Binary data endianness: BYTE_ORDER = ... */
                n = GetByteOrder( bufptr, &mosaic->input_file_endian );
                break;
        }

        /* make sure we got a valid field */
        if ( n < 1 )
        {
            sprintf( error_str, "Invalid header field %s", token );
            ErrorHandler( TRUE, "ReadHeaderFileMosaic", ERROR_READ_INPUTHEADER,
                error_str );
            return ERROR_READ_INPUTHEADER;
        }

        /* advance file buffer pointer to next field */
        bufptr += n;
    }

    /*******************************************************************/

    /* check to see if all required fields are filled in,
     * and use default values for missing optional fields
     */
    CheckFieldsMosaic ( nstrings, StringsPresent, mosaic );

    /*******************************************************************/

    /* No printing */
    prtprm[GEO_TERM] = GEO_FALSE;
    prtprm[GEO_FILE] = GEO_FALSE;

    /* Setup to use Geographic coordinates; projection parameters for
       Geographic are all zeros; units are in degrees */
    projinfo.proj_code = GEO;
    projinfo.units = DEGREE;
    projinfo.zone_code = 0;
    for ( i = 0; i < NUM_PROJECTION_PARAMS; i++ )
        projinfo.proj_coef[i] = 0.0;

    geoproj = &projinfo;
    inproj = GetInputProjectionMosaic( mosaic );

    /* Since these are input lat/longs, then use the input datum code */
    geoproj->datum_code = inproj->datum_code;

    /* Convert from Geographic to the input projection */
    if ( inproj->datum_code != E_NODATUM )
    {   /* Don't call c_transinit or c_trans if a datum shift is not
           going to happen. */
        status = c_transinit( &geoproj->proj_code, &geoproj->units,
            &geoproj->zone_code, &geoproj->datum_code, geoproj->proj_coef,
            &inproj->proj_code, &inproj->units, &inproj->zone_code,
            &inproj->datum_code, inproj->proj_coef, prtprm, NULL );
        if ( status != MRT_NO_ERROR )
        {
            DestroyProjectionInfo( inproj );
            ErrorHandler( TRUE, "ReadHeaderFileMosaic", ERROR_GENERAL,
                "Error in c_transinit when converting the corners from "
                "latitude/longitude to projection x/y" );
        }
    }

    /* Loop through all 4 corners and get the input projection values for
       the lat/long values */
    for ( j = 0; j < 4; j++ )
    {
        /* Convert latitude/longitude corners to x/y projection coordinates */
        if ( inproj->datum_code != E_NODATUM )
        {   /* Don't call c_transinit or c_trans if a datum shift is not
               going to happen. */
            status = c_trans( &geoproj->proj_code, &geoproj->units,
                &inproj->proj_code, &inproj->units,
                &mosaic->ll_image_extent[j][1],
                &mosaic->ll_image_extent[j][0],
                &mosaic->proj_image_extent[j][0],
                &mosaic->proj_image_extent[j][1] );
            if (status != E_GEO_SUCC)
            {
                DestroyProjectionInfo( inproj );
                ErrorHandler( TRUE, "ReadHeaderFileMosaic", ERROR_GENERAL,
                   "Error in c_trans when converting the corners from "
                   "latitude/longitude to projection x/y" );
            }
        }
        else
        {
            /* Call GCTP directly to allow the semi-major and semi-minor
               to be specified directly.  Both the input and output
               sphere values need to be -1, and thus the projection
               parameters for both input and output will be used. */
            status = gctp_call( geoproj->proj_code, geoproj->zone_code, -1,
                geoproj->proj_coef, geoproj->units,
                mosaic->ll_image_extent[j][1], mosaic->ll_image_extent[j][0],
                inproj->proj_code, inproj->zone_code, inproj->sphere_code,
                inproj->proj_coef, inproj->units,
                &mosaic->proj_image_extent[j][0],
                &mosaic->proj_image_extent[j][1] );
            if ( status != E_GEO_SUCC )
            {
                DestroyProjectionInfo( inproj );
                ErrorHandler( TRUE, "ReadHeaderFileMosaic", ERROR_GENERAL,
                   "Error in gctp when converting the corners from "
                   "latitude/longitude to projection x/y" );
            }
        }
    }

    DestroyProjectionInfo( inproj );

    /*******************************************************************/

    /* finish up */
    return 0;
}

/***********************************************************************
 * int CleanupLine(
 *    char *line     (B) String or line to clean up.  The string is
 *                       modified.
 *    )
 *
 * Return values:
 *
 * -1   ignore (empty line)
 *  0   success, add to file buffer.
 *  1   potential buffer overrun
 *  2   environment variable did not exist.
 *
 */
int CleanupLine(char *line)
{
    size_t i, j;
    int infile = FALSE;       /* is this the INPUT_FILENAME line? */
    int outfile = FALSE;      /* is this the OUTPUT_FILENAME line? */
    int found;
    char tmpstr[LINE_BUFSIZ];
    char *capbuf = NULL;
    char *tptr;
    int err = 0;

    /* check for blank line */
    if ( line == NULL || strlen( line ) == 0 )
        return -1;

    /* is this the INPUT_FILENAME line? */
    capbuf = strdup( line );
    strupr( capbuf );
    if ( ( strstr( capbuf, "INPUT_FILENAME" ) ) != NULL )
        infile = TRUE;
    if ( ( strstr( capbuf, "OUTPUT_FILENAME" ) ) != NULL )
        outfile = TRUE;
    free( capbuf );

    /* Expand environment variables: Expects to find "$(ENV_VAR)" where ENV_VAR
     * is the environment variable that one is looking for. 
     */
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
                if(strlen(line) - envnamelen + envlen < LINE_BUFSIZ) {
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
     if( !err && (infile == TRUE || outfile == TRUE) ) {
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

    j = 0;
    /* preprocess line */
    for ( i = 0; line[i] != '\0' && !err; i++ )
    {
        switch ( line[i] )
        {
            case ',':           /* replace commas with spaces */
               if( j + 1 < LINE_BUFSIZ ) {
                  tmpstr[j++] = ' ';
               } else
                  err = 1;
               break;

            case '=':           /* put spaces around = and parens */
               if( j + 3 < LINE_BUFSIZ ) {
                  tmpstr[j++] = ' ';
                  tmpstr[j++] = '=';
                  tmpstr[j++] = ' ';
               } else
                 err = 1;
               break;

            case '(':           /* put space after open paren */
                if (!infile)
                {
                    /* '(' in the INPUT_FILENAME line might be part of
                       the directory or filename so don't process it */
                    if( j + 3 < LINE_BUFSIZ ) {
                       tmpstr[j++] = ' ';
                       tmpstr[j++] = '(';
                       tmpstr[j++] = ' ';
                    } else
                       err = 1;
                }
                else {
                    if( j + 1 < LINE_BUFSIZ )
                       tmpstr[j++] = '(';
                    else
                       err = 1;
                }
                break;

            case ')':   /* put space before close paren, and terminate line */
                if (!infile)
                {
                    /* ')' in the INPUT_FILENAME line might be part of
                       the directory or filename so don't process it */
                    if( j + 4 < LINE_BUFSIZ ) {
                       tmpstr[j++] = ' ';
                       tmpstr[j++] = ')';
                       tmpstr[j++] = ' ';
                       tmpstr[j++] = line[i + 1] = '\0';       /* we be done */
                    } else
                       err = 1;
                }
                else {
                    if( j + 1 < LINE_BUFSIZ )
                       tmpstr[j++] = ')';
                    else
                       err = 1;
                }
                break;

            case '#':/* get rid of comments and newlines, and terminate line */
            case '\n':
                if( j + 2 < LINE_BUFSIZ ) {
                   tmpstr[j++] = ' ';
                   tmpstr[j++] = line[i + 1] = '\0';       /* we be done */
                } else
                   err = 1;
                break;

            default:            /* otherwise just copy input char */
                if( j + 1 < LINE_BUFSIZ ) {
                   tmpstr[j++] = line[i];
                } else
                   err = 1;
                break;
        }
    }

    /* store cleaned up line and return its length */
    if( !err )
       strcpy( line, tmpstr );
    return err;
}

/*******************************************************************/

static int CheckFields
(
    int nstrings,
    int StringsPresent[],
    ModisDescriptor *P
)

{
    int i;
    char s[LINE_BUFSIZ];
    struct DATUMDEF dtm_info;     /* information about the datum */
    long parmflg;

    /* check if we need default bandnames */
    if ( !StringsPresent[MRT_BANDNAMES] )
    {
	for ( i = 0; i < nstrings; i++ )
	{
	    sprintf( s, "band%02i", i + 1 );
	    P->bandinfo[i].name = strdup( s );
            if ( P->bandinfo[i].name == NULL )
            {
	        sprintf( s,
                    "Unable to allocate strdup memory for bandinfo.name. " );
	        ErrorHandler( TRUE, "CheckFields", ERROR_MEMORY, s );
	        return ERROR_MEMORY;
            }
	}
	StringsPresent[MRT_BANDNAMES] = 1;
    }

    /* use min value if background fill is not present */
    if ( !StringsPresent[MRT_BACKGROUND_FILL] )
    {
        SetBackgroundFill( P->nbands, P->bandinfo,
            StringsPresent[MRT_MIN_VALUE],
            StringsPresent[MRT_MAX_VALUE] );
	StringsPresent[MRT_BACKGROUND_FILL] = 1;
    }

    /* min and max values in valid range */
    if ( !StringsPresent[MRT_MIN_VALUE] ) StringsPresent[MRT_MIN_VALUE] = 1;
    if ( !StringsPresent[MRT_MAX_VALUE] ) StringsPresent[MRT_MAX_VALUE] = 1;

    /* UTM_ZONE and DATUM are optional fields. default values
       were set in the initialization process. */
    if ( !StringsPresent[MRT_DATUM] )
    {
        /* No Datum is the default datum value for all data files */
        StringsPresent[MRT_DATUM] = 1;
        P->input_datum_code = E_NODATUM;
    }
    if ( !StringsPresent[MRT_UTM_ZONE] )
        StringsPresent[MRT_UTM_ZONE] = 1;

    /* check that all fields are present */
    for ( i = 0; i < nstrings; i++ )
    {
	if ( !StringsPresent[i] )
    	{
	    sprintf( s, "Missing required field %s in header file.",
                HeaderStrings[i] );
	    ErrorHandler( TRUE, "CheckFields", ERROR_READ_INPUTHEADER, s );
	    return ERROR_READ_INPUTHEADER;
    	}
    }

    /* verify that either the UTM_ZONE or the first projection parameter
       is specified, when dealing with a UTM projection */
    if ( P->input_projection_type == PROJ_UTM )
    {
        if ( P->input_zone_code == 0 &&
             P->input_projection_parameters[0] == 0.0 )
        {
            sprintf( s, "Either the UTM_ZONE or the first two projection "
                     "parameters need to be specified in the input header." );
            ErrorHandler( TRUE, "CheckFields", ERROR_READ_INPUTHEADER, s );
            return ERROR_READ_INPUTHEADER;
        }
    }

    /* if a datum was specified and the first two projection parameters
       were also specified, then verify that they match up (doesn't apply
       to UTM and GEO projections) */
    if ( P->input_datum_code != E_NODATUM &&
         P->input_projection_parameters[0] != 0.0 &&
         ( P->input_projection_type != PROJ_UTM &&
           P->input_projection_type != PROJ_GEO ) )
    {
        /* get the datum info (semi-major, semi-minor axes) for the
           input datum */
        dtm_info.datumnum = P->input_datum_code;
        if (c_getdatum( &dtm_info ) != E_GEO_SUCC)
        {
            ErrorHandler( TRUE, "CheckFields", ERROR_PROJECTION,
                "Could not retrieve input datum information" );
            return ERROR_READ_INPUTHEADER;
        }

        /* verify the input datum info matches with the input projection
           parameters */
        if ( proj_check( &dtm_info, P->input_projection_parameters,
             &parmflg ) != E_GEO_SUCC )
        {
            sprintf( s, "Value of the first projection parameter (%f) does "
                "not match the semi-major axis of the specified datum "
                "(%d:%f)", P->input_projection_parameters[0],
                P->input_datum_code, dtm_info.smajor );
            ErrorHandler( FALSE, "CheckFields", ERROR_READ_INPUTHEADER, s );
            sprintf( s, "Value of the second projection parameter (%f) may "
                "not match the semi-minor axis of the specified datum "
                "(%d:%f)", P->input_projection_parameters[1],
                P->input_datum_code, dtm_info.sminor );
            ErrorHandler( TRUE, "CheckFields", ERROR_READ_INPUTHEADER, s );
            return ERROR_READ_INPUTHEADER;
        }
    }

    return 0;
}

/************************************************************************/

static int CheckFieldsMosaic
(
    int nstrings,
    int StringsPresent[],
    MosaicDescriptor *P
)

{
    int i;
    char s[LINE_BUFSIZ];

    /* check if we need default bandnames */
    if ( !StringsPresent[MRT_BANDNAMES] )
    {
        for ( i = 0; i < nstrings; i++ )
        {
            sprintf( s, "band%02i", i + 1 );
            P->bandinfo[i].name = strdup( s );
            if ( P->bandinfo[i].name == NULL )
            {
                sprintf( s,
                    "Unable to allocate strdup memory for bandinfo.name. " );
                ErrorHandler( TRUE, "CheckFieldsMosaic", ERROR_MEMORY, s );
                return ERROR_MEMORY;
            }
        }
        StringsPresent[MRT_BANDNAMES] = 1;
    }

    /* use min value if background fill is not present */
    if ( !StringsPresent[MRT_BACKGROUND_FILL] )
    {
        SetBackgroundFill( P->nbands, P->bandinfo,
            StringsPresent[MRT_MIN_VALUE],
            StringsPresent[MRT_MAX_VALUE] );
        StringsPresent[MRT_BACKGROUND_FILL] = 1;
    }

    /* min and max values in valid range */
    if ( !StringsPresent[MRT_MIN_VALUE] ) StringsPresent[MRT_MIN_VALUE] = 1;
    if ( !StringsPresent[MRT_MAX_VALUE] ) StringsPresent[MRT_MAX_VALUE] = 1;

    /* UTM_ZONE and DATUM are optional fields. default values
       were set in the initialization process. */
    if ( !StringsPresent[MRT_DATUM] )
    {
        /* No Datum is the default datum value for all data files */
        StringsPresent[MRT_DATUM] = 1;
        P->datum_code = E_NODATUM;
    }
    if ( !StringsPresent[MRT_UTM_ZONE] ) StringsPresent[MRT_UTM_ZONE] = 1;

    /* check that all fields are present */
    for ( i = 0; i < nstrings; i++ )
    {
        if ( !StringsPresent[i] )
        {
            sprintf( s, "Missing required field %s in header file.",
                HeaderStrings[i] );
            ErrorHandler( TRUE, "CheckFieldsMosaic", ERROR_READ_INPUTHEADER,
                s );
            return ERROR_READ_INPUTHEADER;
        }
    }

    /* verify that either the UTM_ZONE or the first projection parameter
       is specified, when dealing with a UTM projection */
    if ( P->projection_type == PROJ_UTM )
    {
        if ( P->zone_code == 0 && P->projection_parameters[0] == 0.0 )
        {
            sprintf( s, "Either the UTM_ZONE or the first two projection "
                "parameters need to be specified in the input header." );
            ErrorHandler( TRUE, "CheckFieldsMosaic", ERROR_READ_INPUTHEADER,
                s );
            return ERROR_READ_INPUTHEADER;
        }
    }

    return 0;
}

/************************************************************************/

/* determine input projection type */

int GetProjectionType
(
    char *tmpstr,
    ProjectionType *input_projection_type
)

{
    int n;
    char projtype[LINE_BUFSIZ];

    if ( sscanf( tmpstr, " = %s%n", projtype, &n ) != 1 || n < 1 )
    {
        sprintf( projtype,
            "Incorrect PROJECTION_TYPE field (bad or missing value %s).",
            projtype );
        ErrorHandler( TRUE, "ReadHeaderFile", ERROR_PROJECTION_TYPE, projtype );        return ERROR_PROJECTION_TYPE;
    }
    strupr( projtype );

    if ( strcmp( projtype, "ISIN" ) == 0 ||
         strcmp( projtype, "INTEGERIZED_SINUSOIDAL" ) == 0 )
        *input_projection_type = PROJ_ISIN;

    else if ( strcmp( projtype, "AEA" ) == 0 ||
              strcmp( projtype, "ALBERS_EQUAL_AREA" ) == 0 )
        *input_projection_type = PROJ_AEA;

    else if ( strcmp( projtype, "ER" ) == 0 ||
              strcmp( projtype, "EQUIRECTANGULAR" ) == 0 )
        *input_projection_type = PROJ_ER;

    else if ( strcmp( projtype, "GEO" ) == 0 ||
              strcmp( projtype, "GEOGRAPHIC" ) == 0 )
        *input_projection_type = PROJ_GEO;

    else if ( strcmp( projtype, "HAM" ) == 0 ||
              strcmp( projtype, "HAMMER" ) == 0 )
        *input_projection_type = PROJ_HAM;

    else if ( strcmp( projtype, "IGH" ) == 0 ||
              strcmp( projtype, "INTERRUPTED_GOODE_HOMOLOSINE" ) == 0 )
        *input_projection_type = PROJ_IGH;

    else if ( strcmp( projtype, "LA" ) == 0 ||
              strcmp( projtype, "LAMBERT_AZIMUTHAL" ) == 0 )
        *input_projection_type = PROJ_LA;

    else if ( strcmp( projtype, "LCC" ) == 0 ||
              strcmp( projtype, "LAMBERT_CONFORMAL_CONIC" ) == 0 )
        *input_projection_type = PROJ_LCC;

    else if ( strcmp( projtype, "MERCAT" ) == 0 ||
              strcmp( projtype, "MERCATOR" ) == 0 )
        *input_projection_type = PROJ_MERC;

    else if ( strcmp( projtype, "MOL" ) == 0 ||
              strcmp( projtype, "MOLLWEIDE" ) == 0 )
        *input_projection_type = PROJ_MOL;

    else if ( strcmp( projtype, "PS" ) == 0 ||
              strcmp( projtype, "POLAR_STEREOGRAPHIC" ) == 0 )
        *input_projection_type = PROJ_PS;

    else if ( strcmp( projtype, "SIN" ) == 0 ||
              strcmp( projtype, "SINUSOIDAL" ) == 0 )
        *input_projection_type = PROJ_SIN;

    else if ( strcmp( projtype, "TM" ) == 0 ||
              strcmp( projtype, "TRANSVERSE_MERCATOR" ) == 0 )
        *input_projection_type = PROJ_TM;

    else if ( strcmp( projtype, "UTM" ) == 0 ||
              strcmp( projtype, "UNIVERSAL_TRANSVERSE_MERCATOR" ) == 0 )
        *input_projection_type = PROJ_UTM;

    else
    {
        sprintf( projtype, "Incorrect PROJECTION_TYPE field (bad value %s).",
            projtype );
        ErrorHandler( TRUE, "ReadHeaderFile", ERROR_PROJPARAMS_VALUE,
            projtype );
        return ERROR_PROJPARAMS_VALUE;
    }

    /* return number of characters parsed */
    return n;
}

/**************************************************************************/

/*
 * determine input projection parameters:
 *
 * PROJECTION_PARAMETERS = ( p1 p2 ... p15 )
 */
int GetProjectionParameters
(
    char *tmpstr,
    double input_projection_parameters[]
)

{
    int i, n, len;
    double pp;
    char s[LINE_BUFSIZ];

    /* read the open paren */
    sscanf( tmpstr, " = (%n", &len );
    if ( len < 1 )
    {
        sprintf( s,
        "Incorrect PROJECTION_PARAMETERS field (bad or missing open paren)." );
        ErrorHandler( TRUE, "ReadHeaderFile", ERROR_PROJPARAMS_VALUE, s );
        return ERROR_PROJPARAMS_VALUE;
    }
    tmpstr += len;

    /* read the projection parameters */
    for ( i = 0; i < 15; i++ )
    {
        if ( sscanf( tmpstr, "%lf%n", &pp, &n ) < 1 )
        {
            sprintf( s,
            "Incorrect PROJECTION_PARAMETERS field (bad or missing value)." );
            ErrorHandler( TRUE, "ReadHeaderFile", ERROR_PROJPARAMS_VALUE, s );
            return ERROR_PROJPARAMS_VALUE;
        }
        input_projection_parameters[i] = pp;
        len += n;
        tmpstr += n;
    }

    /* read the close paren */
    sscanf( tmpstr, " )%n", &n );
    if ( n < 1 )
    {
        sprintf( s,
        "Incorrect PROJECTION_PARAMETERS field (bad or missing close paren)." );
        ErrorHandler( TRUE, "ReadHeaderFile", ERROR_PROJPARAMS_VALUE, s );
        return ERROR_PROJPARAMS_VALUE;
    }

    /* return number of characters parsed */
    return len + n;
}

/**************************************************************************/

/*
 * determine spatial extents:
 *
 * UL_CORNER = ( ULlat ULlon )
 * UR_CORNER = ( URlat URlon )
 * LL_CORNER = ( LLlat LLlon )
 * LR_CORNER = ( LRlat LRlon )
 */
int GetCornerPoints
(
    CornerType corner,
    char *tmpstr,
    double input_image_extent[4][2],
    double ll_extents_corners[4][2]
)

{
    int n;
    double lat, lon;
    char s[LINE_BUFSIZ];

    /* read corner coords */
    if ( sscanf( tmpstr, " = ( %lf %lf )%n", &lat, &lon, &n ) < 2 )
    {
        sprintf( s,
            "ERROR (ReadHeaderFile): Incorrect CORNER_LATLON field.\n" );
        ErrorHandler( TRUE, "ReadHeaderFile", ERROR_READ_CORNERS, s );
        return ERROR_READ_CORNERS;
    }

    input_image_extent[corner][0] = lat;
    input_image_extent[corner][1] = lon;
    ll_extents_corners[corner][0] = lat;
    ll_extents_corners[corner][1] = lon;

    /* return number of characters parsed */
    return n;
}

/************************************************************************/

/* determine number of bands: NBANDS = ... */

int GetNBands
(
    char *tmpstr,
    size_t *nbands,
    BandType **bandinfo
)

{
    size_t i;
    int    n;
    char s[LINE_BUFSIZ];

    if ( sscanf( tmpstr, " = " MRT_SIZE_T_FMT "%n", nbands, &n ) < 1 )
    {
        sprintf( s, "ERROR (ReadHeaderFile): Incorrect NBANDS field.\n" );
        ErrorHandler( TRUE, "ReadHeaderFile", ERROR_NBANDS_FIELD, s );
        return ERROR_NBANDS_FIELD;
    }

    if ( *nbands < 1 )
    {
        sprintf( s,
           "ERROR (ReadHeaderFile): Incorrect NBANDS field (bad value).\n" );
        ErrorHandler( TRUE, "ReadHeaderFile", ERROR_NBANDS_VALUE, s );
        return ERROR_NBANDS_VALUE;
    }

    if ( *bandinfo == NULL )            /* make sure we don't allocate twice */
    {
        *bandinfo = calloc( *nbands, sizeof( BandType ) );
        if ( *bandinfo == NULL )
        {
            sprintf( s,
              "ERROR (ReadHeaderFile): cannot allocate memory for "
              MRT_SIZE_T_FMT " bands.\n", *nbands );
            ErrorHandler( TRUE, "ReadHeaderFile", ERROR_MEMORY, s );
            return ERROR_MEMORY;
        }
    }

    /* by default, select all bands */
    for ( i = 0; i < *nbands; i++ )
        (*bandinfo)[i].selected = 1;

    /* return number of characters parsed */
    return n;
}

/************************************************************************/

/* determine band names: BANDNAMES = ... */
int GetBandNames
(
    char *tmpstr,
    int nbands,
    BandType *bandinfo
)

{
    int i, n, len;
    char s[LINE_BUFSIZ];

    /* read the open paren */
    sscanf( tmpstr, " = (%n", &len );
    if ( len < 1 )
    {
        sprintf( s, "Incorrect BANDNAMES field (bad or missing open paren)." );
        ErrorHandler( TRUE, "ReadHeaderFile", ERROR_BANDNAMES_VALUE, s );
        return ERROR_BANDNAMES_VALUE;
    }
    tmpstr += len;

    /* read the values */
    for ( i = 0; i < nbands; i++ )
    {
        if ( sscanf( tmpstr, "%s%n", s, &n ) < 1 )
        {
            sprintf( s, "Incorrect BANDNAMES field (bad or missing value)." );
            ErrorHandler( TRUE, "ReadHeaderFile", ERROR_BANDNAMES_VALUE, s );
            return ERROR_BANDNAMES_VALUE;
        }

        bandinfo[i].name = strdup( s );

        /* if memory can't be allocated for bandinfo.name, fatal error. JR */
        if ( bandinfo[i].name == NULL )
        {
            sprintf( s, "Unable to allocate strdup memory for bandinfo.name." );            ErrorHandler( TRUE, "ReadHeaderFile", ERROR_MEMORY, s );
            return ERROR_MEMORY;
        }

        len += n;
        tmpstr += n;
    }

    /* read the close paren */
    sscanf( tmpstr, " )%n", &n );
    if ( n < 1 )
    {
        sprintf( s, "Incorrect BANDNAMES field (bad or missing close paren)." );        ErrorHandler( TRUE, "ReadHeaderFile", ERROR_BANDNAMES_VALUE, s );
        return ERROR_BANDNAMES_VALUE;
    }

    /* return number of characters parsed */
    return len + n;
}

/************************************************************************/

/* determine input and output data type: DATA_TYPE = ... */
int GetDataTypes
(
    char *tmpstr,
    int nbands,
    BandType *bandinfo
)

{
    int i, n, len;
    char s[LINE_BUFSIZ];

    /* read the open paren */
    sscanf( tmpstr, " = (%n", &len );
    if ( len < 1 )
    {
        sprintf( s, "Incorrect DATATYPE field (bad or missing open paren)." );
        ErrorHandler( TRUE, "ReadHeaderFile", ERROR_DATATYPE_VALUE, s );
        return ERROR_DATATYPE_VALUE;
    }
    tmpstr += len;

    /* read the values */
    for ( i = 0; i < nbands; i++ )
    {
        if ( sscanf( tmpstr, "%s%n", s, &n ) < 1 )
        {
            sprintf( s, "Incorrect DATATYPE field (bad or missing value)." );
            ErrorHandler( TRUE, "ReadHeaderFile", ERROR_DATATYPE_VALUE, s );
            return ERROR_DATATYPE_VALUE;
        }
        strupr( s );

        if ( strcmp( s, "INT8" ) == 0 )
            bandinfo[i].input_datatype = bandinfo[i].output_datatype =
                DFNT_INT8;
        else if ( strcmp( s, "UINT8" ) == 0 )
            bandinfo[i].input_datatype = bandinfo[i].output_datatype =
                DFNT_UINT8;
        else if ( strcmp( s, "INT16" ) == 0 )
            bandinfo[i].input_datatype = bandinfo[i].output_datatype =
                DFNT_INT16;
        else if ( strcmp( s, "UINT16" ) == 0 )
            bandinfo[i].input_datatype = bandinfo[i].output_datatype =
                DFNT_UINT16;
        else if ( strcmp( s, "INT32" ) == 0 )
            bandinfo[i].input_datatype = bandinfo[i].output_datatype =
                DFNT_INT32;
        else if ( strcmp( s, "UINT32" ) == 0 )
            bandinfo[i].input_datatype = bandinfo[i].output_datatype =
                DFNT_UINT32;
        else if ( strcmp( s, "FLOAT32" ) == 0 )
            bandinfo[i].input_datatype = bandinfo[i].output_datatype =
                DFNT_FLOAT32;
        else
        {
            bandinfo[i].input_datatype = bandinfo[i].output_datatype =
                BAD_DATA_TYPE;
            sprintf( s, "Incorrect DATATYPE field (bad value)." );
            ErrorHandler( TRUE, "ReadHeaderFile", ERROR_DATATYPE_VALUE, s );
            return ERROR_DATATYPE_VALUE;
        }

        len += n;
        tmpstr += n;
    }

    /* read the close paren */
    sscanf( tmpstr, " )%n", &n );
    if ( n < 1 )
    {
        sprintf( s, "Incorrect DATATYPE field (bad or missing close paren)." );
        ErrorHandler( TRUE, "ReadHeaderFile", ERROR_DATATYPE_VALUE, s );
        return ERROR_DATATYPE_VALUE;
    }

    /* return number of characters parsed */
    return len + n;
}

/************************************************************************/

/* determine number of lines: NLINES = ... */

int GetNLines
(
    char *tmpstr,
    int nbands,
    BandType *bandinfo
)

{
    int i, n, len, nlines;
    char s[LINE_BUFSIZ];

    /* read the open paren */
    sscanf( tmpstr, " = (%n", &len );
    if ( len < 1 )
    {
        sprintf( s, "Incorrect NLINES field (bad or missing open paren)." );
        ErrorHandler( TRUE, "ReadHeaderFile", ERROR_NLINES_VALUE, s );
        return ERROR_NLINES_VALUE;
    }
    tmpstr += len;

    /* read the values */
    for ( i = 0; i < nbands; i++ )
    {
        if ( sscanf( tmpstr, "%i%n", &nlines, &n ) < 1 )
        {
            sprintf( s, "Incorrect NLINES field (bad or missing value)." );
            ErrorHandler( TRUE, "ReadHeaderFile", ERROR_NLINES_VALUE, s );
            return ERROR_NLINES_VALUE;
        }
        bandinfo[i].nlines = nlines;
        len += n;
        tmpstr += n;
    }

    /* read the close paren */
    sscanf( tmpstr, " )%n", &n );
    if ( n < 1 )
    {
        sprintf( s, "Incorrect NLINES field (bad or missing close paren)." );
        ErrorHandler( TRUE, "ReadHeaderFile", ERROR_NLINES_VALUE, s );
        return ERROR_NLINES_VALUE;
    }

    /* return number of characters parsed */
    return len + n;
}

/************************************************************************/

/* determine number of samples: NSAMPLES = ... */

int GetNSamples
(
    char *tmpstr,
    int nbands,
    BandType *bandinfo
)

{
    int i, n, len, nsamples;
    char s[LINE_BUFSIZ];

    /* read the open paren */
    sscanf( tmpstr, " = (%n", &len );
    if ( len < 1 )
    {
        sprintf( s, "Incorrect NSAMPLES field (bad or missing open paren)." );
        ErrorHandler( TRUE, "ReadHeaderFile", ERROR_NSAMPLES_VALUE, s );
        return ERROR_NSAMPLES_VALUE;
    }
    tmpstr += len;

    /* read the values */
    for ( i = 0; i < nbands; i++ )
    {
        if ( sscanf( tmpstr, "%i%n", &nsamples, &n ) < 1 )
        {
            sprintf( s, "Incorrect NSAMPLES field (bad or missing value)." );
            ErrorHandler( TRUE, "ReadHeaderFile", ERROR_NSAMPLES_VALUE, s );
            return ERROR_NSAMPLES_VALUE;
        }
        bandinfo[i].nsamples = nsamples;
        len += n;
        tmpstr += n;
    }

    /* read the close paren */
    sscanf( tmpstr, " )%n", &n );
    if ( n < 1 )
    {
        sprintf( s, "Incorrect NSAMPLES field (bad or missing close paren)." );
        ErrorHandler( TRUE, "ReadHeaderFile", ERROR_NSAMPLES_VALUE, s );
        return ERROR_NSAMPLES_VALUE;
    }

    /* return number of characters parsed */
    return len + n;
}

/************************************************************************/

/* determine pixel size: PIXEL_SIZE = ... */

int GetPixelSize
(
    char *tmpstr,
    int nbands,
    BandType *bandinfo
)

{
    int i, n, len;
    double pixel_size;
    char s[LINE_BUFSIZ];

    /* read the open paren */
    sscanf( tmpstr, " = (%n", &len );
    if ( len < 1 )
    {
        sprintf( s, "Incorrect PIXELSIZE field (bad or missing open paren)." );
        ErrorHandler( TRUE, "ReadHeaderFile", ERROR_PIXELSIZE_VALUE, s );
        return ERROR_PIXELSIZE_VALUE;
    }
    tmpstr += len;

    /* read the values */
    for ( i = 0; i < nbands; i++ )
    {
        if ( sscanf( tmpstr, "%lf%n", &pixel_size, &n ) < 1 )
        {
            sprintf( s, "Incorrect PIXELSIZE field (bad or missing value)." );
            ErrorHandler( TRUE, "ReadHeaderFile", ERROR_PIXELSIZE_VALUE, s );
            return ERROR_PIXELSIZE_VALUE;
        }
        bandinfo[i].pixel_size = pixel_size;
        len += n;
        tmpstr += n;
    }

    /* read the close paren */
    sscanf( tmpstr, " )%n", &n );
    if ( n < 1 )
    {
        sprintf( s, "Incorrect PIXELSIZE field (bad or missing close paren)." );        ErrorHandler( TRUE, "ReadHeaderFile", ERROR_PIXELSIZE_VALUE, s );
        return ERROR_PIXELSIZE_VALUE;
    }

    /* return number of characters parsed */
    return len + n;
}

/************************************************************************/

/* determine minimum value: MIN_VALUE = ... */

int GetMinValue
(
    char *tmpstr,
    int nbands,
    BandType *bandinfo
)

{
    int i, n, len;
    double min_value;
    char s[LINE_BUFSIZ];

    /* read the open paren */
    sscanf( tmpstr, " = (%n", &len );
    if ( len < 1 )
    {
        sprintf( s, "Incorrect MINVALUE field (bad or missing open paren)." );
        ErrorHandler( TRUE, "ReadHeaderFile", ERROR_MINVALUE_VALUE, s );
        return ERROR_MINVALUE_VALUE;
    }
    tmpstr += len;

    /* read the values */
    for ( i = 0; i < nbands; i++ )
    {
        if ( sscanf( tmpstr, "%lf%n", &min_value, &n ) < 1 )
        {
            sprintf( s, "Incorrect MINVALUE field (bad or missing value)." );
            ErrorHandler( TRUE, "ReadHeaderFile", ERROR_MINVALUE_VALUE, s );
            return ERROR_MINVALUE_VALUE;
        }
        bandinfo[i].min_value = min_value;
        len += n;
        tmpstr += n;
    }

    /* read the close paren */
    sscanf( tmpstr, " )%n", &n );
    if ( n < 1 )
    {
        sprintf( s, "Incorrect MINVALUE field (bad or missing close paren)." );
        ErrorHandler( TRUE, "ReadHeaderFile", ERROR_MINVALUE_VALUE, s );
        return ERROR_MINVALUE_VALUE;
    }

    /* return number of characters parsed */
    return len + n;
}

/***********************************************************************/

/* determine maximum value: MAX_VALUE = ... */

int GetMaxValue
(
    char *tmpstr,
    int nbands,
    BandType *bandinfo
)

{
    int i, n, len;
    double max_value;
    char s[LINE_BUFSIZ];

    /* read the open paren */
    sscanf( tmpstr, " = (%n", &len );
    if ( len < 1 )
    {
        sprintf( s, "Incorrect MAXVALUE field (bad or missing open paren)." );
        ErrorHandler( TRUE, "ReadHeaderFile", ERROR_MAXVALUE_VALUE, s );
        return ERROR_MAXVALUE_VALUE;
    }
    tmpstr += len;

    /* read the values */
    for ( i = 0; i < nbands; i++ )
    {
        if ( sscanf( tmpstr, "%lf%n", &max_value, &n ) < 1 )
        {
            sprintf( s, "Incorrect MAXVALUE field (bad or missing value)." );
            ErrorHandler( TRUE, "ReadHeaderFile", ERROR_MAXVALUE_VALUE, s );
            return ERROR_MAXVALUE_VALUE;
        }
        bandinfo[i].max_value = max_value;
        len += n;
        tmpstr += n;
    }

    /* read the close paren */
    sscanf( tmpstr, " )%n", &n );
    if ( n < 1 )
    {
        sprintf( s, "Incorrect MAXVALUE field (bad or missing close paren)." );
        ErrorHandler( TRUE, "ReadHeaderFile", ERROR_MAXVALUE_VALUE, s );
        return ERROR_MAXVALUE_VALUE;
    }

    /* return number of characters parsed */
    return len + n;
}

/***********************************************************************/

/* determine background fill value: BACKGROUND_FILL = ... */

int GetBackgroundFill
(
    char *tmpstr,
    int nbands,
    BandType *bandinfo
)

{
    int i, n, len;
    double background_fill;
    char s[LINE_BUFSIZ];

    /* read the open paren */
    sscanf( tmpstr, " = (%n", &len );
    if ( len < 1 )
    {
        sprintf( s,
            "Incorrect BACKGROUND_FILL field (bad or missing open paren)." );
        ErrorHandler( TRUE, "ReadHeaderFile", ERROR_FILLVALUE_VALUE, s );
        return ERROR_FILLVALUE_VALUE;
    }
    tmpstr += len;

    /* read the values */
    for ( i = 0; i < nbands; i++ )
    {
        if ( sscanf( tmpstr, "%lf%n", &background_fill, &n ) < 1 )
        {
            sprintf( s,
                "Incorrect BACKGROUND_FILL field (bad or missing value)." );
            ErrorHandler( TRUE, "ReadHeaderFile", ERROR_FILLVALUE_VALUE, s );
            return ERROR_FILLVALUE_VALUE;
        }
        bandinfo[i].background_fill = background_fill;
        len += n;
        tmpstr += n;
    }

    /* read the close paren */
    sscanf( tmpstr, " )%n", &n );
    if ( n < 1 )
    {
        sprintf( s,
            "Incorrect BACKGROUND_FILL field (bad or missing close paren)." );
        ErrorHandler( TRUE, "ReadHeaderFile", ERROR_FILLVALUE_VALUE, s );
        return ERROR_FILLVALUE_VALUE;
    }

    /* return number of characters parsed */
    return len + n;
}


/***********************************************************************/

/* set background fill value when BACKGROUND_FILL field is not present */
/* if possible, use value outside valid range, else use min value */

int SetBackgroundFill
(
    int nbands,
    BandType *bandinfo,
    int have_min,
    int have_max
)

{
    int i;

    /* for each band, supply missing background fill value */
    /* use value outside valid range if background fill is not present */
    for ( i = 0; i < nbands; i++ )
    {
        switch ( bandinfo[i].input_datatype )
        {
            case DFNT_INT8:
                if ( have_min && bandinfo[i].min_value > (float)MRT_INT8_MIN )
                    bandinfo[i].background_fill =
                        bandinfo[i].min_value - 1.0;
                else if ( have_max &&
                          bandinfo[i].max_value < (float)MRT_INT8_MAX )
                    bandinfo[i].background_fill =
                        bandinfo[i].max_value + 1.0;
                else
                    bandinfo[i].background_fill = (float)MRT_INT8_MIN;
                break;

            case DFNT_UINT8:
                if ( have_min && bandinfo[i].min_value > 0.0 )
                    bandinfo[i].background_fill =
                        bandinfo[i].min_value - 1.0;
                else if ( have_max &&
                          bandinfo[i].max_value < (float)MRT_UINT8_MAX )
                    bandinfo[i].background_fill =
                        bandinfo[i].max_value + 1.0;
                else
                    bandinfo[i].background_fill = 0.0;
                break;

            case DFNT_INT16:
                if ( have_min && bandinfo[i].min_value > (float)MRT_INT16_MIN )
                    bandinfo[i].background_fill =
                        bandinfo[i].min_value - 1.0;
                else if ( have_max &&
                          bandinfo[i].max_value < (float)MRT_INT16_MAX )
                    bandinfo[i].background_fill =
                        bandinfo[i].max_value + 1.0;
                else
                    bandinfo[i].background_fill = (float)MRT_INT16_MIN;
                break;

            case DFNT_UINT16:
                if ( have_min && bandinfo[i].min_value > 0.0 )
                    bandinfo[i].background_fill =
                        bandinfo[i].min_value - 1.0;
                else if ( have_max &&
                          bandinfo[i].max_value < (float)MRT_UINT16_MAX )
                    bandinfo[i].background_fill =
                        bandinfo[i].max_value + 1.0;
                else
                    bandinfo[i].background_fill = 0.0;
                break;

            case DFNT_INT32:
                if ( have_min && bandinfo[i].min_value > (float)MRT_INT32_MIN )
                    bandinfo[i].background_fill =
                        bandinfo[i].min_value - 1.0;
                else if ( have_max &&
                          bandinfo[i].max_value < (float)MRT_INT32_MAX )
                    bandinfo[i].background_fill =
                        bandinfo[i].max_value + 1.0;
                else
                    bandinfo[i].background_fill = (float)MRT_INT32_MIN;
                break;

            case DFNT_UINT32:
                if ( have_min && bandinfo[i].min_value > 0.0 )
                    bandinfo[i].background_fill =
                        bandinfo[i].min_value - 1.0;
                else if ( have_max &&
                          bandinfo[i].max_value < (float)MRT_UINT32_MAX )
                    bandinfo[i].background_fill =
                        bandinfo[i].max_value + 1.0;
                else
                    bandinfo[i].background_fill = 0.0;
                break;

            case DFNT_FLOAT32:
                if ( have_min && bandinfo[i].min_value > -MRT_FLOAT4_MAX + 1.0 )
                    bandinfo[i].background_fill =
                        bandinfo[i].min_value - 1.0;
                else if ( have_max &&
                          bandinfo[i].max_value < MRT_FLOAT4_MAX - 1.0 )
                    bandinfo[i].background_fill =
                        bandinfo[i].max_value + 1.0;
                else
                    bandinfo[i].background_fill = -MRT_FLOAT4_MAX;
                break;
        }
    }

    return 0;
}

/******************************************************************************

MODULE:  GetInputDatum

PURPOSE:  Read the datum from a header file

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
int GetInputDatum
(
    char *tmpstr,
    int *input_datum_code
)

{
    int n;
    char datumtype[LINE_BUFSIZ];

    if ( sscanf( tmpstr, " = %s%n", datumtype, &n ) != 1 || n < 1 )
    {
        sprintf( datumtype, "Incorrect DATUM field (bad or missing value %s).",
            datumtype );
        ErrorHandler( FALSE, "ReadHeaderFile", ERROR_DATUM_FIELD,
            datumtype );
        return ERROR_DATUM_FIELD;
    }
    strupr( datumtype );

    if ( strcmp( datumtype, "NAD27" ) == 0 )
        *input_datum_code = E_NAD27;
    else if ( strcmp( datumtype, "NAD83" ) == 0 )
        *input_datum_code = E_NAD83;
    else if ( strcmp( datumtype, "WGS66" ) == 0 )
        *input_datum_code = E_WGS66;
    else if ( strcmp( datumtype, "WGS72" ) == 0 )
        *input_datum_code = E_WGS72;
    else if ( strcmp( datumtype, "WGS84" ) == 0 )
        *input_datum_code = E_WGS84;
    else if ( strcmp( datumtype, "NODATUM" ) == 0 )
        *input_datum_code = E_NODATUM;
    else
    {
        sprintf( datumtype, "Incorrect DATUM field (bad value %s).",
            datumtype );
        ErrorHandler( FALSE, "ReadHeaderFile", ERROR_DATUM_VALUE, datumtype );
        return ERROR_DATUM_VALUE;
    }

    /* return number of characters parsed */
    return n;
}

/******************************************************************************

MODULE:  GetUtmZone

PURPOSE:  Read the UTM zone from a header file

RETURN VALUE:
Type = int
Value           Description
-----           -----------
n               Number of characters parsed

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         07/01  Rob Burrell            Add UTM zones for UTM
  
NOTES:

******************************************************************************/
int GetUtmZone
(
    char *tmpstr,
    int *input_zone_code
)

{
    int n, utmzone;
    char errmsg[LINE_BUFSIZ];

    if ( sscanf( tmpstr, " = %d%n", &utmzone, &n ) != 1 || n < 1 )
    {
        sprintf( errmsg, "Incorrect UTM_ZONE field (bad or missing value %d).",
            utmzone );
        ErrorHandler( FALSE, "ReadHeaderFile", ERROR_UTMZONE_FIELD, errmsg );
        return ERROR_UTMZONE_FIELD;
    }

    if ( utmzone < -60 || utmzone > 60 )
    {
        sprintf( errmsg, "Incorrect UTM_ZONE field (bad value %d).", utmzone );
        ErrorHandler( FALSE, "ReadHeaderFile", ERROR_UTMZONE_VALUE, errmsg );
        return ERROR_UTMZONE_VALUE;
    }
    else
        *input_zone_code = utmzone;

    /* return number of characters parsed */
    return n;
}

/******************************************************************************

MODULE:  GetByteOrder

PURPOSE:  Read the Byte Order a header file

RETURN VALUE:
Type = int
Value           Description
-----           -----------
n               Number of characters parsed

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         07/07  Maverick Merritt       Initial development
  
NOTES:

******************************************************************************/
static int GetByteOrder
(
    char *tmpstr,
    MrtEndianness *input_byte_order
)

{
    int n;
    char byte_order[LINE_BUFSIZ];
    char errmsg[LINE_BUFSIZ];

    if ( sscanf( tmpstr, " = %s%n", byte_order, &n ) != 1 || n < 1 )
    {
        sprintf( errmsg,
            "Incorrect BYTE_ORDER field (bad or missing value %s).",
            byte_order );
        ErrorHandler( FALSE, "ReadHeaderFile", ERROR_BYTEORDER_FIELD, errmsg );
        return ERROR_BYTEORDER_FIELD;
    }

    strupr( byte_order );

    if( strcmp(byte_order, "BIG_ENDIAN") == 0 )
       *input_byte_order = MRT_BIG_ENDIAN;
    else if( strcmp(byte_order, "LITTLE_ENDIAN") == 0 )
       *input_byte_order = MRT_LITTLE_ENDIAN;
    else
    {
        sprintf( errmsg, "Incorrect BYTE_ORDER value (bad value %s).",
            byte_order );
        ErrorHandler( FALSE, "ReadHeaderFile", ERROR_BYTEORDER_VALUE, errmsg );
        return ERROR_BYTEORDER_VALUE;
    }

    /* return number of characters parsed */
    return n;
}

