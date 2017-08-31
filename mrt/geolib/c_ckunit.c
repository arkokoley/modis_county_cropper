
/*****************************************************************************
NAME:		C_CKUNIT

PURPOSE:	This routine is designed to check the unit code for
		coordinates stored in the LAS ddr and return an
		integer code that corresponds to the unit code and
		is capable of being used by the Projection Transformation
		Package.  The routine will return a bad status (E_GEO_FAIL)
		and an output unit code of -1 for undefined (user defined) 
		unit codes.

		The routine currently supports the following code strings:

			Radians:
			Feet:
			Meters:
			Seconds:	Seconds of arc or arc seconds
			Degrees:	Degrees of arc or arc degrees
			Dms:		Packed DMS (DDDMMMSSS.SS)

		Note also that the input unit code string is converted to
		upper case, thereby allowing the routine to handle case
		combinations of the above.

PROGRAM HISTORY:
	By D. Akkerman, USGS/EROS Data Center, 11/88
        D. Etrheim, CSB, 7/90, Standardized error message handling
	D. Akkerman, CSB, 7/91, Fixed off-by-one error in instr memory
				allocation.
        D. Etrheim, CSB, 10/93, Free allocated space

PROJECT:	LAS

ALGORITHM:

	Convert the input unit code string to upper case.
	Assign the output unit code based on the input unit string.
	If the input unit code is undefined then assign -1 to the output
	  unit code and return a bad status.
	Return a successful status.

******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "shared_resample.h"
#include "tae.h"
#include "worgen.h"
#include "util.h"

int c_ckunit( char *inunit,	/* Input unit code string. (I)                  */
	      long *outunit	/* Output unit code.  Note that this code will
				   be -1 for an undefined code. (O)             */
     )
{

    char *instr;		/* Working input unit string.                   */

    long slen;			/* String length.                               */

/* Convert the input unit code string to upper case.			*/

    slen = ( long ) strlen( inunit );
    if ( ( instr = ( char * ) malloc( slen + 1 ) ) == 0 )	/* +1 for null */
    {
	ErrorHandler( FALSE, "c_ckunit", ERROR_MEMORY, NULL );
	return ( E_GEO_FAIL );
    }
    strcpy( instr, inunit );
    c_low2up( instr, &slen );

/* Assign the output unit code based on the input unit string.		*/

    if ( strcmp( instr, "RADIANS" ) == 0 )
	*outunit = 0;
    else if ( strcmp( instr, "FEET" ) == 0 )
	*outunit = 1;
    else if ( strcmp( instr, "METERS" ) == 0 )
	*outunit = 2;
    else if ( strcmp( instr, "SECONDS" ) == 0 )
	*outunit = 3;
    else if ( strcmp( instr, "DEGREES" ) == 0 )
	*outunit = 4;
    else if ( strcmp( instr, "DMS" ) == 0 )
	*outunit = 5;
    else
    {
	*outunit = -1;
	ErrorHandler( FALSE, "c_ckunit", ERROR_PROJECTION,
		      "Undefined projection unit code" );
	return ( E_GEO_FAIL );
    }

/* Free allocated space */
    free( instr );

    return ( E_GEO_SUCC );
}
