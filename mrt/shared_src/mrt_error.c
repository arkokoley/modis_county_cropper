
/******************************************************************************

FILE:  error.c

PURPOSE:  Handle errors from resample

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         03/00  Rob Burrell            Original Development

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/

/******************************************************************************

MODULE:  ErrorHandler

PURPOSE:  
  Calls the LogHandler with the message
  Write both nonfatal and fatal error messages to the terminal.
  If a fatal error is issued, AbortExit is called.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
MRT_NO_ERROR        Returns MRT_NO_ERROR if the call is not FATAL

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         03/00   Rob Burrell            Original Development
         01/01   John Rishea            Modified string size variables to
                                        use constants defined in resample.h
         01/01   John Rishea            Standardized formatting 
         04/01   Rob Burrell            Added missing msgs for UTM zone
         04/01   Rob Burrell            Added msgs for ellipse field
         12/01   Gail Schmidt           Ellipse field is now datum

NOTES:
  A replacement for c_errmsg and some instances of c_lasmsg in LAS code

  mrt_error.h contains a list of formal messages
  extra_message is used if more information is available

******************************************************************************/
#include "shared_resample.h"

static char *formal_message[NUM_ERROR_CODES] = {
    "No Error",			/* zero, not an error? */
    "General Processing",	/* -1 */
    "ASSERT",			/* system */
    "Environmental Variable Not Found",
    "Memory Allocation Failed",
    "Error waiting for thread termination",
    "Semaphore Error",
    "Mutex Error",
    "None",
    "None",
    "Opening Input Parameter File",	/* -10 *//* general file io */
    "Reading Input Parameter File",
    "Opening Output Parameter File",
    "Writing Output Parameter File",
    "Opening Input Image File",
    "Reading Input Image File",
    "Opening Output Image File",
    "Writing Output Image File",
    "Opening Input Header File",
    "Reading Input Header File",
    "Opening Output Header File",	/* -20 */
    "Writing Output Header File",
    "No Command Line Arguments",	/* command line & prm file */
    "Missing Or Bad Parameter Filename",
    "Unknown Command Line Argument",
    "Bad Or Missing Input Filename Extension",
    "Bad Or Missing Output Filename Extension",
    "Bad Or Missing Resample Type",
    "Bad Or Missing Projection Type",
    "Bad Or Missing INPUT_FILENAME Field",
    "Bad Or Missing SPECTRAL_SUBSET Field",	/* -30 */
    "Bad Or Missing SPATIAL_SUBSET Field",
    "Bad Or Missing OUTPUT_FILENAME Field",
    "Bad Or Missing RESAMPLING_TYPE Field",
    "Bad Or Missing OUTPUT_PROJECTION Field",
    "Bad Or Missing OUTPUT_PROJECTION_PARAMETERS field",
    "Bad Or Missing DATA_TYPE Field",
    "Bad Or Missing PROJECTION_PARAMETERS Field",
    "Bad Or Missing PROJECTION_PARAMETERS Values",
    "Bad Or Missing Spatial Extents Corner",
    "Bad Or Missing NBANDS Field",	/* -40 */
    "Bad Or Missing NBANDS Value",
    "Bad Or Missing BANDNAMES Field",
    "Bad Or Missing BANDNAMES Value",
    "Bad Or Missing DATATYPE Field",
    "Bad Or Missing DATATYPE Value",
    "Bad Or Missing NLINES Field",
    "Bad Or Missing NLINES Value",
    "Bad Or Missing NSAMPLES Field",
    "Bad Or Missing NSAMPLES Value",
    "Bad Or Missing PIXEL_SIZE Field",	/* -50 */
    "Bad Or Missing PIXEL_SIZE Value",
    "Bad Or Missing MINVALUE Field",
    "Bad Or Missing MINVALUE Value",
    "Bad Or Missing MAXVALUE Field",
    "Bad Or Missing MAXVALUE Value",
    "Bad Or Missing BACKGROUND_FILL Field",
    "Bad Or Missing BACKGROUND_FILL Value",
    "Total Bands Found Is Inconsistent With NBANDS",
    "No Bands Selected For Output",
    "Bad or Missing UTM zone field",  /* -60 */
    "Bad or Missing UTM Value",
    "Bad or Missing DATUM Field",
    "Bad or Missing DATUM Value",
    "Bad or Missing SPATIAL_SUBSET_TYPE Field",
    "Bad or Missing BYTE_ORDER Field",
    "Bad or missing BYTE_ORDER Value",
    "None",
    "None",
    "None",

    "Projection Processing Error",	/* -70 *//* gctp & geolib */
    "Open Datum File Error",
    "Open Spheroid File Error",
    "Projection Math Error",
    "Point lies in break",
    "Output file name not specified",
    "Projection transformation failed",
    "Failed to converge after many iterations",
    "Too many iterations for inverse Robinson",
    "Too many iterations in inverse",
    "Input data error",
    "Illegal DMS field",
    "Inconsistant unit and system codes for input",
    "Illegal input system code",
    "Illegal input unit code",
    "Illegal input zone code",
    "Point projects into infinity",
    "Latitude failed to converge after many iterations",
    "Inconsistant unit and system codes for output",
    "Illegal output system code",
    "Illegal output unit code",
    "Illegal output zone code",
    "Transformation cannot be computed at the poles",
    "Point cannot be projected",
    "Point projects into a circle of unacceptable radius",
    "Fifty iterations performed without conversion",
    "Spheroid code reset to default",
    "Equal latitudes for std parallels on opposite sides of equator",
    "Illegal zone number",
    "Error opening state plane parameter file",
    "Illegal source or target unit code",
    "Missing projection parameters",
    "Invalid corner coordinates for input image",
    "Output window falls outside mapping grid"	/* -103 */
};

void AbortExit
(
    int error_code	/* one of the #defined codes in mrt_error.h */
);

int ErrorHandler
(
    int fatal_flag,	/* I:  TRUE for fatal errors, FALSE otherwise */
    char *module,	/* I:  calling module name */
    int error_code,	/* I:  one of the #defined names in resample.h */
    char *extra_message		/* I:  extra info if needed */
)

{
    char error_msg[LARGE_STRING];	/* strings for constructing messages */
    char log_msg[HUGE_STRING];

    /* just in case someone is using a non-formal code */
    if ( error_code < 0 )
	error_code = -error_code;

    /* just in case someone calls us badly */
    if ( error_code >= NUM_ERROR_CODES )
	error_code = MRT_NO_ERROR;

    /* construct fatal string or a warning */
    if ( fatal_flag )
	sprintf( error_msg,
            "Error: %s : %s", module, formal_message[error_code] );
    else
	sprintf( error_msg,
            "Warning: %s : %s", module, formal_message[error_code] );

    /* construct message to the log file */
    if ( extra_message )
	sprintf( log_msg, "%s\n     : %s", error_msg, extra_message );
    else
	strcpy( log_msg, error_msg );

    /* log it */
    LogHandler( log_msg );

    /* print to terminal */
    fprintf( stdout, "%s\n", error_msg );
    fflush( stdout );

    /* extra to terminal ? */
    if ( extra_message )
    {
	fprintf( stdout, "     : %s\n", extra_message );
        fflush( stdout );
    }

    /* if its fatal, bail */
    if ( fatal_flag )
	AbortExit( error_code );

    return ( MRT_NO_ERROR );
}
