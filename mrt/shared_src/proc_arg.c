/******************************************************************************

FILE:  proc_arg.c

PURPOSE:  Process command-line arguments

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         04/00  John Weiss             Original Development
         01/01  John Rishea            Standardized formatting
         01/01  John Rishea            Moved some local prototypes to loc_prot.h

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/
#if defined(__CYGWIN__) || defined(WIN32)
#include <getopt.h>		/* getopt  prototype */
#else
#include <unistd.h>		/* getopt  prototype */
#endif
#include "shared_mosaic.h"

/************ LOCAL PROTOTYPES ********************/

static int GetOutputProjectionParameters
(
    ModisDescriptor *P,         /* O:  session info */
    char *str                   /* I:  string to be parsed */
);

static ProjectionType GetOutputProjectionType
(
    char *str   /* I:  the string to parse */
);

static ResamplingType GetResamplingType
(
    char *str   /* I:  the string to parse */
);

static SpatialSubsetType GetSpatialSubsetType
(
    char *str   /* I:  the string to parse */
);

static int GetSpatialSubset
(
    ModisDescriptor *P,          /* O:  session info */
    char *str                   /* I:  string to be parsed */
);

static int GetSpectralSubset
(
    ModisDescriptor *P,         /* O:  session info */
    char *str                   /* I:  string to be parsed */
);

int GetPixelSizeArg
(
    ModisDescriptor *P,		/* O:  session info */
    char *str			/* I:  string to be parsed */
);

int GetUTMZoneArg
(
    ModisDescriptor *P,		/* O:  session info */
    char *str			/* I:  string to be parsed */
);

/******************************************************************************

MODULE:  ProcessArguments

PURPOSE:  Capture command line arguments and initialize
  the modis descriptor

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status          See mrt_error.h for a complete list of values

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         04/00  John Weiss             Original Development
         04/00  John Weiss             More options
         05/00  John Weiss             Remove output filetype switch
         01/01  John Rishea            Standardized formatting

NOTES:

******************************************************************************/
int ProcessArguments
(
    int argc,			/* I:  number of arguments */
    char *argv[],		/* I:  argument strings */
    ModisDescriptor *P          /* O:  session info */
)

{
    int c, i;
    char errorstr[SMALL_STRING];  /* SMALL_STRING defined in shared_resample.h */ 

    /* check usage */
    if ( argc < 3 )
    {
	Usage(  );
	return ERROR_NOCOMMANDLINE_ARGUMENT;
    }

    opterr = 0;		/* do not print error messages to stdout */
    while ( ( c = getopt( argc, argv, "fg:h:i:j:l:o:p:r:s:a:t:u:x:" ) ) != -1 )
    {
	switch ( c )
	{
            case 'a':   /* spatial subset type */
                P->aswitch = TRUE;
                P->ParamsPresent[SPATIAL_SUBSET_TYPE] = 1;
                P->spatial_subset_type = GetSpatialSubsetType( optarg );
                if ( P->spatial_subset_type == BAD_SPATIAL_SUBSET_TYPE )
                {
                    sprintf( errorstr, "Bad spatial subset type (%s)", optarg );
                    ErrorHandler( FALSE, "ProcessArguments",
                        ERROR_SPATIAL_SUBSET_TYPE, errorstr );
                    Usage(  );
                    return ERROR_SPATIAL_SUBSET_TYPE;
                }
                break;

	    case 'f':	/* format conversion (no resampling) */
		P->resampling_type = NO_RESAMPLE;
		break;

	    case 'g':	/* log file name, should be processed in
                           InitLogHandler() */
		break;

	    case 'h':	/* .hdf file name, should be processed in Hdf2Hdr() */
		sprintf( errorstr, "-h argument should call Hdf2Hdr(). "
                    "Please submit bug report." );
		ErrorHandler( TRUE, "ProcessArguments", ERROR_GENERAL,
                    errorstr );
		Usage(  );
		return ERROR_GENERAL;

	    case 'i':	/* input file name */
                P->iswitch = TRUE;
                P->ParamsPresent[INPUT_FILENAME] = 1;
		P->input_filename = strdup( optarg );
                if ( P->input_filename == NULL )
                {
		    sprintf( errorstr,
                        "strdup mem for input_filename not allocated." );
		    ErrorHandler( TRUE, "ProcessArguments", ERROR_MEMORY,
                        errorstr );
		    Usage(  );
		    return ERROR_MEMORY;
                }
		break;

            case 'j':   /* output projection parameters */
                P->jswitch = TRUE;
                P->ParamsPresent[OUTPUT_PROJ_PARMS] = 1;
                i = GetOutputProjectionParameters( P, optarg );
		break;

            case 'l':   /* spatial subsetting */
                P->lswitch = TRUE;
                P->ParamsPresent[SPATIAL_SUBSET_UL] = 1;
                P->ParamsPresent[SPATIAL_SUBSET_LR] = 1;
                i = GetSpatialSubset( P, optarg );
                if ( i != 4 )
		{
                    sprintf( errorstr, "Bad spatial subset (%s)", optarg );
                    ErrorHandler( FALSE, "ProcessArguments",
                        ERROR_SPATIALSUB_FIELD, errorstr );
		    Usage(  );
                    return ERROR_SPATIALSUB_FIELD;
		}
		break;

	    case 'o':	/* output file name */
                P->oswitch = TRUE;
                P->ParamsPresent[OUTPUT_FILENAME] = 1;
		P->output_filename = strdup( optarg );
                if ( P->output_filename == NULL )
                {
		    sprintf( errorstr,
                        "strdup mem for output_filename not allocated." );
		    ErrorHandler( TRUE, "ProcessArguments", ERROR_MEMORY,
                        errorstr );
		    Usage(  );
		    return ERROR_MEMORY;
                }
		break;

            case 'p':	/* parameter filename */
		P->parameter_filename = strdup( optarg );
                if ( P->parameter_filename == NULL )
                {
		    sprintf( errorstr,
                        "strdup mem for parameter_filename not allocated." );
		    ErrorHandler( TRUE, "ProcessArguments", ERROR_MEMORY,
                        errorstr );
		    Usage(  );
		    return ERROR_MEMORY;
                }
                break;

	    case 'r':	/* resampling type */
                P->rswitch = TRUE;
                P->ParamsPresent[RESAMPLING_TYPE] = 1;
		P->resampling_type = GetResamplingType( optarg );
		if ( P->resampling_type == BAD_RESAMPLING_TYPE )
		{
		    sprintf( errorstr, "Bad resampling type (%s)", optarg );
		    ErrorHandler( FALSE, "ProcessArguments",
                        ERROR_RESAMPLE_TYPE, errorstr );
		    Usage(  );
		    return ERROR_RESAMPLE_TYPE;
		}
		break;

	    case 's':	/* spectral subsetting - can't really finish until
                we get the input file and band information in the read_prm
                routine */
                P->sswitch = TRUE;
                P->ParamsPresent[SPECTRAL_SUBSET] = 1;
                if ( GetSpectralSubset( P, optarg ) == 0 )
		{
		    sprintf( errorstr, "Bad spectral subset (%s)", optarg );
		    ErrorHandler( FALSE, "ProcessArguments",
                        ERROR_SPECTRALSUB_FIELD, errorstr );
		    Usage(  );
		    return ERROR_SPECTRALSUB_FIELD;
		}
		break;

	    case 't':	/* output projection type */
                P->tswitch = TRUE;
                P->ParamsPresent[OUTPUT_PROJ_TYPE] = 1;
                P->output_projection_type = GetOutputProjectionType( optarg );
		if ( P->output_projection_type == BAD_PROJECTION_TYPE )
		{
		    sprintf( errorstr, "Bad projection type (%s)", optarg );
		    ErrorHandler( FALSE, "ProcessArguments",
                        ERROR_PROJECTION_TYPE, errorstr );
		    Usage(  );
		    return ERROR_PROJECTION_TYPE;
		}
                break;

	    case 'u':	/* output UTM zone */
                P->uswitch = TRUE;
                P->ParamsPresent[UTM_ZONE] = 1;
                i = GetUTMZoneArg( P, optarg );
		break;

	    case 'x':	/* output pixel size */
                P->xswitch = TRUE;
                P->ParamsPresent[PIXEL_SIZE] = 1;
                i = GetPixelSizeArg( P, optarg );
		break;

	    case '?':		/* error */
		sprintf( errorstr, "Unknown option (%s)", argv[optind - 1] );
		ErrorHandler( FALSE, "ProcessArguments", ERROR_UNKNOWN_ARGUMENT,
		    errorstr );
		Usage(  );
		return ERROR_UNKNOWN_ARGUMENT;
	}
    }

    /* error-handling */
    if ( optind != argc )
    {
	sprintf( errorstr, "Unknown argument (%s)", argv[optind - 1] );
	ErrorHandler( FALSE, "ProcessArguments", ERROR_UNKNOWN_ARGUMENT,
            errorstr );
	Usage(  );
	return ERROR_UNKNOWN_ARGUMENT;
    }

    /* must specify at least a parameter filename */
    if ( P->parameter_filename == NULL )
    {
	ErrorHandler( FALSE, "ProcessArguments", ERROR_NOPARAMFILE_NAME, NULL );
	Usage(  );
	return ERROR_NOPARAMFILE_NAME;
    }

    return MRT_NO_ERROR;
}

/******************************************************************************

MODULE:  ProcessArgumentsHDF2RB

PURPOSE:  Capture command line arguments and initialize the modis descriptor
    for the HDF to Raw Binary converter

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status          See mrt_error.h for a complete list of values

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         11/05  Gail Schmidt           Original Development

NOTES:

******************************************************************************/
int ProcessArgumentsHDF2RB
(
    int argc,			/* I:  number of arguments */
    char *argv[],		/* I:  argument strings */
    ModisDescriptor *P          /* O:  session info */
)

{
    int c, i;
    char errorstr[SMALL_STRING];  /* SMALL_STRING defined in shared_resample.h */ 

    /* check usage */
    if ( argc < 3 )
    {
        HDF2RB_Usage(  );
        return ERROR_NOCOMMANDLINE_ARGUMENT;
    }

    /* format conversion (no resampling) */
    P->resampling_type = NO_RESAMPLE;
    P->ParamsPresent[RESAMPLING_TYPE] = 1;

    /* read the command-line arguments */
    opterr = 0;		/* do not print error messages to stdout */
    while ( ( c = getopt( argc, argv, "g:i:l:o:p:s:" ) ) != -1 )
    {
        switch ( c )
        {
            case 'g':   /* log file name, should be processed in
                           InitLogHandler() */
                break;

            case 'i':   /* input file name */
                P->iswitch = TRUE;
                P->ParamsPresent[INPUT_FILENAME] = 1;
                P->input_filename = strdup( optarg );
                if ( P->input_filename == NULL )
                {
                    sprintf( errorstr,
                        "strdup mem for input_filename not allocated." );
                    ErrorHandler( TRUE, "ProcessArgumentsHDF2RB", ERROR_MEMORY,
                        errorstr );
                    HDF2RB_Usage(  );
                    return ERROR_MEMORY;
                }
                break;

            case 'l':   /* spatial subsetting (only input line/samp allowed) */
                P->lswitch = TRUE;
                P->ParamsPresent[SPATIAL_SUBSET_UL] = 1;
                P->ParamsPresent[SPATIAL_SUBSET_LR] = 1;
                i = GetSpatialSubset( P, optarg );
                if ( i != 4 )
                {
                    sprintf( errorstr, "Bad spatial subset (%s)", optarg );
                    ErrorHandler( FALSE, "ProcessArgumentsHDF2RB",
                        ERROR_SPATIALSUB_FIELD, errorstr );
                    HDF2RB_Usage(  );
                    return ERROR_SPATIALSUB_FIELD;
                }
                break;

            case 'o':   /* output file name */
                P->oswitch = TRUE;
                P->ParamsPresent[OUTPUT_FILENAME] = 1;
                P->output_filename = strdup( optarg );
                if ( P->output_filename == NULL )
                {
                    sprintf( errorstr,
                        "strdup mem for output_filename not allocated." );
                    ErrorHandler( TRUE, "ProcessArgumentsHDF2RB", ERROR_MEMORY,
                        errorstr );
                    HDF2RB_Usage(  );
                    return ERROR_MEMORY;
                }
                break;

            case 'p':	/* parameter filename */
                P->parameter_filename = strdup( optarg );
                if ( P->parameter_filename == NULL )
                {
                    sprintf( errorstr,
                        "strdup mem for parameter_filename not allocated." );
                    ErrorHandler( TRUE, "ProcessArgumentsHDF2RB", ERROR_MEMORY,
                        errorstr );
                    HDF2RB_Usage(  );
                    return ERROR_MEMORY;
                }
                break;

            case 's':   /* spectral subsetting - can't really finish until
                we get the input file and band information in the read_prm
                routine */
                P->sswitch = TRUE;
                P->ParamsPresent[SPECTRAL_SUBSET] = 1;
                if ( GetSpectralSubset( P, optarg ) == 0 )
                {
                    sprintf( errorstr, "Bad spectral subset (%s)", optarg );
                    ErrorHandler( FALSE, "ProcessArgumentsHDF2RB",
                        ERROR_SPECTRALSUB_FIELD, errorstr );
                    HDF2RB_Usage(  );
                    return ERROR_SPECTRALSUB_FIELD;
                }
                break;

            case '?':   /* error */
                sprintf( errorstr, "Unknown option (%s)", argv[optind - 1] );
                ErrorHandler( FALSE, "ProcessArgumentsHDF2RB",
                    ERROR_UNKNOWN_ARGUMENT, errorstr );
                HDF2RB_Usage(  );
                return ERROR_UNKNOWN_ARGUMENT;
        }
    }

    /* error-handling */
    if ( optind != argc )
    {
        sprintf( errorstr, "Unknown argument (%s)", argv[optind - 1] );
        ErrorHandler( FALSE, "ProcessArgumentsHDF2RB", ERROR_UNKNOWN_ARGUMENT,
            errorstr );
        HDF2RB_Usage(  );
        return ERROR_UNKNOWN_ARGUMENT;
    }

    /* must specify at least a parameter filename */
    if ( P->parameter_filename == NULL )
    {
        ErrorHandler( FALSE, "ProcessArgumentsHDF2RB", ERROR_NOPARAMFILE_NAME,
            NULL );
        HDF2RB_Usage(  );
        return ERROR_NOPARAMFILE_NAME;
    }

    return MRT_NO_ERROR;
}

/******************************************************************************

MODULE:  GetOutputProjectionType

PURPOSE:  Parse a projection string and return the type

RETURN VALUE:
Type = ProjectionType
Value           Description
-----           -----------
ProjectionType  See shared_resample.h for a complete list of types

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  John Weiss             Original Development
         01/01  John Rishea            Standardized formatting
         11/02  Gail Schmidt           Support Albers Equal Area
         07/03  Gail Schmidt           Support Equirectangular

NOTES:

******************************************************************************/
ProjectionType GetOutputProjectionType
(
    char *str	/* I:  the string to parse */
)

{
    strupr( str );

    if ( !strcmp( str, "ISIN" ) || !strcmp( str, "INTEGERIZED_SINUSOIDAL") )
        return PROJ_ISIN;
    else if ( !strcmp( str, "AEA" ) || !strcmp( str, "ALBERS_EQUAL_AREA") )
        return PROJ_AEA;
    else if ( !strcmp( str, "ER" ) || !strcmp( str, "EQUIRECTANGULAR") )
        return PROJ_ER;
    else if ( !strcmp( str, "GEO" ) || !strcmp( str, "GEOGRAPHIC") )
        return PROJ_GEO;
    else if ( !strcmp( str, "HAM" ) || !strcmp( str, "HAMMER") )
        return PROJ_HAM;
    else if ( !strcmp( str, "IGH" ) ||
              !strcmp( str, "INTERRUPTED_GOODE_HOMOLOSINE") )
        return PROJ_IGH;
    else if ( !strcmp( str, "LA" ) || !strcmp( str, "LAMBERT_AZIMUTHAL") )
        return PROJ_LA;
    else if ( !strcmp( str, "LCC" ) ||
              !strcmp( str, "LAMBERT_CONFORMAL_CONIC") )
        return PROJ_LCC;
    else if ( !strcmp( str, "MERCAT" ) || !strcmp( str, "MERCATOR") )
        return PROJ_MERC;
    else if ( !strcmp( str, "MOL" ) || !strcmp( str, "MOLLWEIDE") )
        return PROJ_MOL;
    else if ( !strcmp( str, "PS" ) || !strcmp( str, "POLAR_STEREOGRAPHIC") )
        return PROJ_PS;
    else if ( !strcmp( str, "SIN" ) || !strcmp( str, "SINUSOIDAL") )
        return PROJ_SIN;
    else if ( !strcmp( str, "TM" ) || !strcmp( str, "TRANSVERSE_MERCATOR") )
        return PROJ_TM;
    else if ( !strcmp( str, "UTM" ) ||
              !strcmp( str, "UNIVERSAL_TRANSVERSE_MERCATOR") )
        return PROJ_UTM;
    else
	return BAD_PROJECTION_TYPE;
}

/******************************************************************************

MODULE:  GetResamplingType

PURPOSE:  Parse a string and return the resample method

RETURN VALUE:
Type = ResamplingType
Value           Description
-----           -----------
ResamplingType  See shared_resample.h for a complete list of types

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  John Weiss             Original Development
         01/01  John Rishea            Standardized formatting
 
NOTES:

******************************************************************************/
ResamplingType GetResamplingType
(
    char *str	/* I:  the string to parse */
)

{
    strupr( str );

    if ( !strcmp( str, "NN" ) )
	return NN;
    else if ( !strcmp( str, "BI" ) )
	return BI;
    else if ( !strcmp( str, "CC" ) )
	return CC;
    else if ( !strcmp( str, "NONE" ) )
	return NO_RESAMPLE;
    else
	return BAD_RESAMPLING_TYPE;
}

/******************************************************************************

MODULE:  GetSpatialSubsetType

PURPOSE:  Parse a string and return the spatial subsetting type

RETURN VALUE:
Type = SpatialSubsetType
Value           Description
-----           -----------
ResamplingType  See shared_resample.h for a complete list of types

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         02/02  Gail Schmidt           Original Development
 
NOTES:

******************************************************************************/
SpatialSubsetType GetSpatialSubsetType
(
    char *str	/* I:  the string to parse */
)

{
    strupr( str );

    if ( !strcmp( str, "INPUT_LAT_LONG" ) )
	return INPUT_LAT_LONG;
    else if ( !strcmp( str, "INPUT_LINE_SAMPLE" ) )
	return INPUT_LINE_SAMPLE;
    else if ( !strcmp( str, "OUTPUT_PROJ_COORDS" ) )
	return OUTPUT_PROJ_COORDS;
    else
	return BAD_SPATIAL_SUBSET_TYPE;
}

/******************************************************************************

MODULE:  GetOutputProjectionParameters

PURPOSE:  Parse a projection parameter string

RETURN VALUE:
Type = int
Value           Description
-----           -----------
n               number of parameters

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  John Weiss             Original Development
         01/01  John Rishea            Standardized formatting

NOTES:

******************************************************************************/
int GetOutputProjectionParameters
(
    ModisDescriptor *P,		/* O:  session info */
    char *str			/* I:  string to be parsed */
)

{
    int i, n, count, len, pos = 0;
    double pp;

    /* read the projection parameters */
    for ( i = 0; i < 15; i++ )
    {
	count = sscanf( str + pos, "%lf%n", &pp, &len );
	pos += len;
	if ( count < 1 )
	    break;
	P->output_projection_parameters[i] = pp;
    }
    n = i;

    /* zero all unsupplied projection parameters */
    for ( ; i < 15; i++ )
	P->output_projection_parameters[i] = 0.0;

    return n;
}

/******************************************************************************

MODULE:  GetSpectralSubset

PURPOSE:  read a list of spectral subset values and store them for actual
    use in the read parameter file routine

RETURN VALUE:
Type = int
Value           Description
-----           -----------
n               number of things found
	
HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  John Weiss             Original Development
         01/01  John Rishea            Standardized formatting

NOTES:

******************************************************************************/
int GetSpectralSubset
(
    ModisDescriptor *P,		/* O:  session info */
    char *str			/* I:  string to be parsed */
)

{
    int i, count = 0;
    char errmsg[SMALL_STRING];

    /* check for valid values */
    for ( i = 0; i < (int) strlen( str ); i++ )
    {
        if ( str[i] != '0' && str[i] != '1' && str[i] != ' ' )
        {
            sprintf( errmsg, "Error processing spectral subset (%s) for "
                "resampler. Only '0's and '1's are allowed.", optarg );
            ErrorHandler( FALSE, "GetSpectralSubset", ERROR_GENERAL,
                errmsg );
            return 0;
        }
        else if ( str[i] == '0' || str[i] == '1' )
            count++;
    }

    /* Copy the spectral subset string to the tmpspectralsubset string in
       the ModisDescriptor */
    P->tmpspectralsubset = strdup (str);
    if (P->tmpspectralsubset == NULL)
    {
        ErrorHandler( FALSE, "GetSpectralSubset", ERROR_GENERAL,
           "Error copying the spectral subset string to the MODIS descriptor");
        return 0;
    }

    return count;
}

/******************************************************************************

MODULE:  GetSpectralSubsetMosaic

PURPOSE:  Read a list of spectral subset values for mosaicking
          Supports "x x x x ... x" or "xxxxxx".

RETURN VALUE:
Type = int
Value           Description
-----           -----------
n               number of things found

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         07/02  Gail Schmidt           Original Development

NOTES:

******************************************************************************/
int GetSpectralSubsetMosaic
(
    MosaicDescriptor *Mosaic,   /* O:  session info */
    char *str,                  /* I:  string to be parsed */
    int spectral_subset         /* I:  was spectral subsetting specified? */
)

{
    int    n;
    size_t len, i;
    size_t band;

    /* process bands if spectral subsetting was specified */
    if ( spectral_subset )
    {
        /* read the band selections (0 or 1) */
        len = strlen( str );
        for ( band = 0, i = 0; i < len; i++ )
        {
            if ( str[i] == '1' )
                Mosaic->bandinfo[band++].selected = 1;
            else if ( str[i] == '0' )
                Mosaic->bandinfo[band++].selected = 0;
        }
        n = band;

        /* deselect any remaining bands */
        for ( ; band < Mosaic->nbands; band++ )
            Mosaic->bandinfo[band].selected = 0;
    }

    /* by default select all bands */
    else
    {
        for ( i = 0; i < Mosaic->nbands; i++ )
            Mosaic->bandinfo[i].selected = 1;
        n = Mosaic->nbands;
    }

    return n;
}

/******************************************************************************

MODULE:  GetSpatialSubset

PURPOSE:  read a list of spatial subset values (UL and LR corners)

RETURN VALUE:
Type = int
Value           Description
-----           -----------
count           Number of points found

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  John Weiss             Original Development
         01/01  John Rishea            Standardized formatting
         02/01  Gail Schmidt           Remove FixCornerPoints call since
                                       these values will be processed while
                                       reading the input parameter file
         02/02  Gail Schmidt           Update to support output proj coords
                                       for spatial subsetting

NOTES:

******************************************************************************/
int GetSpatialSubset
(
    ModisDescriptor *P,		/* O:  session info */
    char *str			/* I:  string to be parsed */
)

{
    int count;
    int ul0, ul1, lr0, lr1;

    if ( strchr(str, '.') == NULL )     /* ints => line/sample */
    {
        count = sscanf( str, "%i %i %i %i", &ul0, &ul1, &lr0, &lr1 );
        P->ll_spac_sub_gring_corners[UL][0] = ul0;
        P->ll_spac_sub_gring_corners[UL][1] = ul1;
        P->ll_spac_sub_gring_corners[LR][0] = lr0;
        P->ll_spac_sub_gring_corners[LR][1] = lr1;
    }
    else                                /* floats => lat/lon or proj coords */
    {
        count = sscanf( str, "%lf %lf %lf %lf",
            &( P->ll_spac_sub_gring_corners[UL][0] ),
            &( P->ll_spac_sub_gring_corners[UL][1] ),
            &( P->ll_spac_sub_gring_corners[LR][0] ),
            &( P->ll_spac_sub_gring_corners[LR][1] ) );
    }

    return count;
}

/******************************************************************************

MODULE:  GetParameterFilename

PURPOSE:  get parameter filename from command-line arguments

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status		See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  John Weiss             Original Development
         01/01  John Rishea            Standardized formatting

NOTES:

******************************************************************************/
int GetParameterFilename
(
    int argc,			/* I:  number of arguments */
    char *argv[],		/* I:  argument strings */
    char *parameter_filename,   /* O:  filename */
    size_t param_filename_len   /* I:  length of filename buffer, one less */
                                /*      than allocated */
)

{
    int i;

    /* check usage */
    if ( argc < 3 ) {
	ErrorHandler( FALSE, "GetParameterFilename",
            ERROR_NOCOMMANDLINE_ARGUMENT, NULL );
	Usage(  );
	return ERROR_NOCOMMANDLINE_ARGUMENT;
    }

    for ( i = 1; i < argc - 1; i++ ) {
	if ( !strcmp( argv[i], "-p" ) ) {
           strncpy( parameter_filename, argv[i + 1], param_filename_len );
           parameter_filename[param_filename_len] = 0;
           return MRT_NO_ERROR;
        }
    }

    /* must specify at least a parameter filename */
    ErrorHandler( FALSE, "GetParameterFilename", ERROR_NOPARAMFILE_NAME, NULL );

    Usage(  );
    return ERROR_NOPARAMFILE_NAME;
}


/******************************************************************************

MODULE:  GetUTMZoneArg

PURPOSE:  get UTM zone from command-line arguments

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status		See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         04/01  John Weiss             Original Development

NOTES:

******************************************************************************/
int GetUTMZoneArg
(
    ModisDescriptor *P,		/* O:  session info */
    char *str			/* I:  string to be parsed */
)

{
    int zone;
    char s[SMALL_STRING];

    /* scan UTM zone field */
    if ( sscanf( str, "%i", &zone ) < 1 )
    {
	sprintf( s, "Incorrect -u UTM_zone command-line argument (bad or "
                    "missing value).\n" );
        ErrorHandler( TRUE, "GetUTMZoneArg", ERROR_UTMZONE_FIELD, s );
        return ERROR_UTMZONE_FIELD;
    }

    if (-60 <= zone && zone <= 60)
        P->output_zone_code = zone;
    else
    {
	sprintf( s, "Incorrect -u UTM_zone command-line argument (value "
                    "out of range).\n" );
        ErrorHandler( TRUE, "GetUTMZoneArg", ERROR_UTMZONE_VALUE, s );
        return ERROR_UTMZONE_VALUE;
    }

    return 0;
}


/******************************************************************************

MODULE:  GetPixelSizeArg

PURPOSE:  get pixel size from command-line arguments

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status		See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         04/01  John Weiss             Original Development

NOTES:

******************************************************************************/
int GetPixelSizeArg
(
    ModisDescriptor *P,		/* O:  session info */
    char *str			/* I:  string to be parsed */
)

{
    size_t i;
    double pixel_size;
    char s[SMALL_STRING];

    /* short-circuit resampling if user wants format conversion */
    if ( P->resampling_type == NO_RESAMPLE )
    {
        for ( i = 0; i < P->nbands; i++ )
            P->bandinfo[i].output_pixel_size = P->bandinfo[i].pixel_size;
        return 0;
    }

    /* scan output pixel size field */
    if ( sscanf( str, "%lf", &pixel_size ) < 1 )
    {
        sprintf( s, "Incorrect -x pixel_size command-line argument "
                    "(bad or missing value).\n" );
	ErrorHandler( TRUE, "GetPixelSizeArg", ERROR_PIXELSIZE_VALUE, s );
	return ERROR_PIXELSIZE_VALUE;
    }

    /* store specified output pixel size in all bands */
    for ( i = 0; i < P->nbands; i++ )
	P->bandinfo[i].output_pixel_size = pixel_size;

    return 0;
}

/******************************************************************************

MODULE:  CheckFormatConversion

PURPOSE:  See if user issued "-f" format conversion on command line

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status		See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         07/01  John Weiss             Original Development

NOTES:

******************************************************************************/
int CheckFormatConversion
(
    int argc,                   /* I:  number of arguments */
    char *argv[],               /* I:  argument strings */
    ModisDescriptor *modis	/* O:  session info */
)

{
    int i;

    for ( i = 1; i < argc; i++ )
    {
	if ( ( ! strcmp( argv[i], "-f" ) ) ||
	    (  i < argc - 1 && ! strcmp( argv[i], "-r" ) &&
            ! strcmp( argv[i + 1], "NONE" ) ) )
	{
	    modis->resampling_type = NO_RESAMPLE;
            modis->ParamsPresent[RESAMPLING_TYPE] = 1;
            return TRUE;
	}
    }

    return FALSE;
}

