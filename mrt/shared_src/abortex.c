
/******************************************************************************

FILE:  abortex.c

PURPOSE:  Common exit point for resampler

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  Rob Burrell            
         01/01  John Rishea            Standardized formatting
           
HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "shared_resample.h"

/******************************************************************************

MODULE:  AbortExit

PURPOSE:  Print a termination message and return an
  error code to the OS

RETURN VALUE:
Type =  None
Value           Description
-----           -----------

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  Rob Burrell            Original Development
         08/00  Rob Burrell            Close log file

NOTES:

******************************************************************************/

void AbortExit
( 
    int error_code	/* I:  one of the #defined codes in errorhandler.h */
)

{
    fprintf( stdout, "Fatal Error, Terminating...\n" );
    fflush(stdout);
    CloseLogHandler(  );	/* close the log file */

    exit( error_code );
}
