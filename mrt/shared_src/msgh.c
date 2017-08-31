
/******************************************************************************

FILE:  msgh.c

PURPOSE:  Messages printing to the terminal

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  Rob Burrell            Original Development

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/
#include <stdarg.h>
#include "shared_resample.h"

/******************************************************************************

MODULE:  MessageHandler

PURPOSE:  Prints a variable argument list to the terminal and echos
  the message to the log file

RETURN VALUE:
Type = int
Value           Description
-----           -----------
TRUE            Always

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         03/00  Rob Burrell            Original Development
         01/01  John Rishea            Standardized formatting

NOTES:

******************************************************************************/
int MessageHandler
(
    char *module,	/* I:  module who called us (optional) */
    char *format,	/* I:  format string for the message */
    ...			/* I:  variable argument list */
)

{
    va_list arguments;		/* variable list */

    /* message holders; MAX_MESSAGE_LENGTH defined in shared_resample.h */
    char va_message[MAX_MESSAGE_LENGTH], message[MAX_MESSAGE_LENGTH];

    /* create a message */
    va_start( arguments, format );
    vsprintf( va_message, format, arguments );
    va_end( arguments );

    /* since a module name is optional ... */
    if ( module != NULL )
	sprintf( message, "%s : %s", module, va_message );
    else
	sprintf( message, "%s", va_message );

    fprintf( stdout, "%s\n", message );
    fflush( stdout );
    LogHandler( message );

    return ( TRUE );
}
