
/*******************************************************************************
NAME: C_DTMSTR

PURPOSE: Returns a title string of the input datum code

PROGRAM HISTORY:
PROGRAMMER        DATE          REASON
----------        ----          ------
J. Janssen        8/98          Original Development
T. Ratliff        10/99         The routine was giving a fatal error if the
                                datum code didn't exist, it should only
                                generate non-fatal errors.

COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
        C_DTMSTR must be run under TAE.

PROJECT: LAS

ALGORITHM:
        Return the datum title based on the input datum code

******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "shared_resample.h"
#include "tae.h"
#include "worgen.h"
#include "datum.h"
#include "geompak.h"

int c_dtmstr( long *dtm_code,	/* I: Datum number we are searching for */
	      char *dtm_str     /* O: Datum name */
     )
{
    struct DATUMDEF dtmparm;	/* Datum structure */

    switch ( *dtm_code )
    {
	case 125:
	    strcpy( dtm_str, "(125)ASTRONOMICAL STATION 1952:INT'L 1909" );
	    break;
	case 127:
	    strcpy( dtm_str, "(127)AUSTRALIAN GEODETIC 1966:AUSTR. NAT'L" );
	    break;
	case 128:
	    strcpy( dtm_str, "(128)AUSTRALIAN GEODETIC 1984:AUSTR. NAT'L" );
	    break;
	case 141:
	    strcpy( dtm_str, "(141)CHATHAM ISLAND ASTRO 1971:INT'L 1909" );
	    break;
	case 145:
	    strcpy( dtm_str, "(145)DECEPTION ISLAND:CLARKE 1880" );
	    break;
	case 149:
	    strcpy( dtm_str, "(149)ESTONIA:BESSEL 1841(NAMIBIA)" );
	    break;
	case 167:
	    strcpy( dtm_str, "(167)OBSERV. METEOR. 1939:INT'L 1909" );
	    break;
	case 176:
	    strcpy( dtm_str, "(176)PROV. SOUTH CHILEAN 1963:INT'L 1909" );
	    break;
	case 195:
	    strcpy( dtm_str, "(195)KERTAU 1948:EVEREST" );
	    break;
	case 298:
	    strcpy( dtm_str, "(298)SOUTH ASIA:SOUTHEAST ASIA" );
	    break;
	case 306:
	    strcpy( dtm_str, "(306)TANANARIVE OBSERV. 1925:INT'L 1909" );
	    break;
	case 313:
	    strcpy( dtm_str, "(313)ASTRO TERN ISLAND 1961:INT'L 1909" );
	    break;
	case 318:
	    strcpy( dtm_str, "(318)WAKE ISLAND ASTRO 1952:INT'L 1909" );
	    break;
	default:
	    if ( ( *dtm_code > 261 ) && ( *dtm_code < 271 ) )
		sprintf( dtm_str, "(%ld)%s", *dtm_code,
			 "SOUTH AMERICAN 1956:INT'L 1909" );
	    else
	    {
		/* Fill the datum structure 
		   --------------------------- */
		dtmparm.datumnum = *dtm_code;
		if ( c_getdatum( &dtmparm ) != E_GEO_SUCC )
		{
		    MessageHandler( "c_dtmstr", "Datum code %ld: Does not exist",
				    *dtm_code );
		    ErrorHandler( FALSE, "c_dtmstr", ERROR_GENERAL, NULL );
		    return ( E_GEO_FAIL );
		}

		/* Create the datum string
		   -------------------------- */
		if ( dtmparm.datumnum < 100 )
		    sprintf( dtm_str, "(%ld)%s", *dtm_code, dtmparm.spherename );
		else
		    sprintf( dtm_str, "(%ld)%s:%s", *dtm_code, dtmparm.datumname,
			     dtmparm.spherename );
	    }
    }

    return ( E_GEO_SUCC );
}
