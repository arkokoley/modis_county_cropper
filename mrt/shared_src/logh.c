
/******************************************************************************

FILE:  logh.c

PURPOSE:  Session logging to a file

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         03/00  Rob Burrell            Original Development

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/
#if defined(__CYGWIN__) || defined(WIN32)
#include <getopt.h>             /* getopt  prototype */
#else
#include <unistd.h>             /* getopt  prototype */
#endif
#include "shared_resample.h"

/* NOTE:
**   templogname is a global variable used in this function and C_TRANS.C.
**   In C_TRANS.C it is used by the init() function which is found in
**   Modis/geolib/gctp/report.c
 */

char templogname[SMALL_STRING];		/* temporary log file name */
static int loginitialized = FALSE;	/* init flag */
/* Create a global variable for the log filename.  By default this is
   resample.log, however the user can change the log filename using the
   command line option -g.  gls */
static char LOG_FILENAME[FILENAME_LENGTH+1] = "resample.log";

/******************************************************************************

MODULE:  InitLogHandler

PURPOSE:  Create a temporary log file and set the LOG_FILENAME if it was
  specified on the command line.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
TRUE            Success
FALSE           Failure to write

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         08/00   Rob Burrell            Original Development
         01/01   John Rishea            Standardized formatting
         03/01   John Weiss             Replace tmpnam() with mktemp()
         06/01   Gail Schmidt           Allow the resample.log file to be
                                        specified via a command-line option
         06/07   Gail Schmidt           Changed mktemp to mkstemp due to a
                                        possible race condition that may occur

NOTES:
  Creating a temporary log file and then appending it to the resample
  log file. This should avoid the possibility that two processes might
  write to the log file at the same time.

******************************************************************************/
int InitLogHandler
(
    int argc,                   /* I:  number of arguments */
    char *argv[]                /* I:  argument strings */
)

{
    int i;
    FILE *templog = NULL;    /* filename of the temporary log file */

    /* if it is already open */
    if ( loginitialized )
        return ( FALSE );

    /* JMW 06/26/01 - Bug fix: cannot use getopt() here and also in
       ProcessArguments(). */
    /* if -g was specified on the command line, then set the global
       LOG_FILENAME to the command line filename specified. */
    for ( i = 1; i < argc - 1; i++ )
    {
        if ( ! strcmp( argv[i], "-g" ) && strlen( argv[i + 1] ) > 0 ) {
            strncpy( LOG_FILENAME, argv[i + 1], FILENAME_LENGTH );
            LOG_FILENAME[FILENAME_LENGTH] = 0;
        }
    }

    /* create a temporary name */
    strcpy(templogname, "tmpXXXXXX");
#ifdef WIN32
    if (_mktemp_s(templogname, strlen(templogname)+1) != 0)
#else
    if (mkstemp(templogname) == -1)
#endif
    {
        fprintf( stdout, "Error: %s : %s\n", "InitLogHandler",
            "cannot generate unique templogname with mkstemp().  Check the "
            "directory name to make sure the path exists." );
        fflush( stdout );
        return ( FALSE );
    }

    /* open a temporary log file in the current directory */
    templog = fopen( templogname, "w" );
    if ( !templog )
    {
        fprintf( stdout, "Error: %s : %s\n", "InitLogHandler",
            ERRORMSG_LOGFILE_OPEN );
        fflush( stdout );
        return ( FALSE );
    }

    /* let someone else open and close */
    fclose( templog );
    loginitialized = TRUE;

    return ( TRUE );
}

/******************************************************************************

MODULE:  CloseLogHandler

PURPOSE:  Copy the templog file to LOG_FILENAME

RETURN VALUE:
Type = int
Value           Description
-----           -----------
TRUE            Success
FALSE           Failure to write

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         08/00   Rob Burrell            Original Development
         01/01   John Rishea            Standardized formatting
 
NOTES:

******************************************************************************/
int CloseLogHandler
(
    void
)

{
    FILE *templog = NULL,	/* file ptrs to input temp and output files */
         *logfile = NULL;
    char buffer[HUGE_STRING];	/* string buffer for reading/writing */
                                /* HUGE_STRING defined in shared_resample.h */

    /* if no log initialized */
    if ( !loginitialized )
        return ( FALSE );

    /* it is now closed */
    loginitialized = FALSE;

    /* open resampler's log file */
    logfile = fopen( LOG_FILENAME, "a" );
    if ( !logfile )
    {
        fprintf( stdout, "Error: %s : %s\n", "CloseLogHandler",
            ERRORMSG_LOGFILE_OPEN );
        fflush( stdout );
        return ( FALSE );
    }

    /* open our temp log file */
    templog = fopen( templogname, "r" );
    if ( !templog )
    {
        fprintf( stdout, "Error: %s : %s\n", "CloseLogHandler",
            ERRORMSG_LOGFILE_OPEN );
        fflush( stdout );
        fclose( logfile );
        return ( FALSE );
    }

    /* append the contents of the temp log */
    while ( fgets( buffer, 254, templog ) )
        fputs( buffer, logfile );

    fclose( logfile );
    fclose( templog );
    remove( templogname );

    return ( TRUE );
}

/******************************************************************************

MODULE:  LogHandler

PURPOSE:  Formats and prints messages to a logfile

RETURN VALUE:
Type = int
Value           Description
-----           -----------
TRUE            Success
FALSE           Failure to write

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         03/00  Rob Burrell            Original Development
         08/00  Rob Burrell            Mods to use temp log file 
         01/01  John Rishea            Standardized formatting

NOTES:

******************************************************************************/
int LogHandler
(
    char *message	/* I:  message to be written to the log */
)

{
    FILE *logfile = NULL;

    logfile = fopen( templogname, "a" );
    if ( !logfile )
    {
        /* rather than create a potential loop by calling the ErrorHandler,
           just print a message and return */
        fprintf( stdout, "Error: %s : %s\n", "LogHandler",
            ERRORMSG_LOGFILE_OPEN );
        fflush( stdout );
        return ( FALSE );
    }

    fprintf( logfile, "%s\n", message );
    fclose( logfile );

    return ( TRUE );
}
