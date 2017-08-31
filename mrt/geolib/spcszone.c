
/*******************************************************************************
NAME			      spcs_zone 

PURPOSE	     Print the state or area which corresponds to the state plane
	     coordinate system zone id entered by the user.

PROGRAM HISTORY
PROGRAMMER		DATE		REASON
----------		----		------
D. Steinwand		Aug, 1988	Original Development
					

PROJECT     LAS

ALGORITHM 

	Find the appropriate zone code
	Print the state or area information

ALGORITHM REFERENCES

1.  Clarie, Charles N, "State Plane Coordinates by Automatic Data Processing",
    U.S. Department of Commerce, Environmental Science Services Admin.,
    Coast and Geodetic Survey, Publication 62-4, 1973.

2.  "Software Documentation for GCTP General Cartographic Transformation
    Package", U.S. Geological Survey National Mapping Division, May 1982.

3.  Snyder, John P., "Map Projections--A Working Manual", U.S. Geological
    Survey Professional Paper 1395 (Supersedes USGS Bulletin 1532), United
    State Government Printing Office, Washington D.C., 1987. 
*******************************************************************************/
#include "worgen.h"
#include "util.h"
#include "geompak.h"
#include "loclprot.h"

int MessageHandler
(
    char *module,	/* I:  module who called us (optional) */
    char *format,	/* I:  format string for the message */
    ...                 /* I:  variable argument list */
);

void spcs_zone( long zone	/* State Plane Coordinate System zone id */
     )
{
    switch ( zone )
    {

	case 101:
	    MessageHandler( NULL, " Alabama--East" );
	    break;
	case 102:
	    MessageHandler( NULL, " Alabama--West" );
	    break;
	case 201:
	    MessageHandler( NULL, " Arizona--East" );
	    break;
	case 202:
	    MessageHandler( NULL, " Arizona--Central" );
	    break;
	case 203:
	    MessageHandler( NULL, " Arizona--West" );
	    break;
	case 301:
	    MessageHandler( NULL, " Arkansas--North" );
	    break;
	case 302:
	    MessageHandler( NULL, " Arkansas--South" );
	    break;
	case 401:
	    MessageHandler( NULL, " California--Zone 1" );
	    break;
	case 402:
	    MessageHandler( NULL, " California--Zone 2" );
	    break;
	case 403:
	    MessageHandler( NULL, " California--Zone 3" );
	    break;
	case 404:
	    MessageHandler( NULL, " California--Zone 4" );
	    break;
	case 405:
	    MessageHandler( NULL, " California--Zone 5" );
	    break;
	case 406:
	    MessageHandler( NULL, " California--Zone 6" );
	    break;
	case 407:
	    MessageHandler( NULL, " California--Zone 7" );
	    break;
	case 501:
	    MessageHandler( NULL, " Colorado--North" );
	    break;
	case 502:
	    MessageHandler( NULL, " Colorado--Central" );
	    break;
	case 503:
	    MessageHandler( NULL, " Colorado--South" );
	    break;
	case 600:
	    MessageHandler( NULL, " Connecticut" );
	    break;
	case 700:
	    MessageHandler( NULL, " Delaware" );
	    break;
	case 901:
	    MessageHandler( NULL, " Florida--East" );
	    break;
	case 902:
	    MessageHandler( NULL, " Florida--West" );
	    break;
	case 903:
	    MessageHandler( NULL, " Florida--North" );
	    break;
	case 1001:
	    MessageHandler( NULL, " Georgia--East" );
	    break;
	case 1002:
	    MessageHandler( NULL, " Georgia--West" );
	    break;
	case 1101:
	    MessageHandler( NULL, " Idaho--East" );
	    break;
	case 1102:
	    MessageHandler( NULL, " Idaho--Central" );
	    break;
	case 1103:
	    MessageHandler( NULL, " Idaho--West" );
	    break;
	case 1201:
	    MessageHandler( NULL, " Illinois--East" );
	    break;
	case 1202:
	    MessageHandler( NULL, " Illinois--West" );
	    break;
	case 1301:
	    MessageHandler( NULL, " Indiana--East" );
	    break;
	case 1302:
	    MessageHandler( NULL, " Indiana--West" );
	    break;
	case 1401:
	    MessageHandler( NULL, " Iowa--North " );
	    break;
	case 1402:
	    MessageHandler( NULL, " Iowa--South" );
	    break;
	case 1501:
	    MessageHandler( NULL, " Kansas--North" );
	    break;
	case 1502:
	    MessageHandler( NULL, " Kansas--South" );
	    break;
	case 1601:
	    MessageHandler( NULL, " Kentucky--North" );
	    break;
	case 1602:
	    MessageHandler( NULL, " Kentucky--South" );
	    break;
	case 1701:
	    MessageHandler( NULL, " Louisiana--North" );
	    break;
	case 1702:
	    MessageHandler( NULL, " Louisiana--South" );
	    break;
	case 1703:
	    MessageHandler( NULL, " Louisiana--Offshore" );
	    break;
	case 1801:
	    MessageHandler( NULL, " Maine--East " );
	    break;
	case 1802:
	    MessageHandler( NULL, " Maine--West" );
	    break;
	case 1900:
	    MessageHandler( NULL, " Maryland & District of Columbia" );
	    break;
	case 2001:
	    MessageHandler( NULL, " Massachusetts--Mainland" );
	    break;
	case 2002:
	    MessageHandler( NULL, " Massachusetts--Island" );
	    break;
	case 2101:
	    MessageHandler( NULL, " Michigan--East (TM)" );
	    break;
	case 2102:
	    MessageHandler( NULL, " Michigan--Central (TM)" );
	    break;
	case 2103:
	    MessageHandler( NULL, " Michigan--West (TM)" );
	    break;
	case 2111:
	    MessageHandler( NULL, " Michigan--North (Lambert)" );
	    break;
	case 2112:
	    MessageHandler( NULL, " Michigan--Central (Lambert)" );
	    break;
	case 2113:
	    MessageHandler( NULL, " Michigan--South (Lambert)" );
	    break;
	case 2201:
	    MessageHandler( NULL, " Minnesota--North" );
	    break;
	case 2202:
	    MessageHandler( NULL, " Minnesota--Central" );
	    break;
	case 2203:
	    MessageHandler( NULL, " Minnesota--South" );
	    break;
	case 2301:
	    MessageHandler( NULL, " Mississippi--East" );
	    break;
	case 2302:
	    MessageHandler( NULL, " Mississippi--West" );
	    break;
	case 2401:
	    MessageHandler( NULL, " Missouri--East" );
	    break;
	case 2402:
	    MessageHandler( NULL, " Missouri--Central" );
	    break;
	case 2403:
	    MessageHandler( NULL, " Missouri--West" );
	    break;
	case 2500:
	    MessageHandler( NULL, " Montana" );
	    break;
	case 2501:
	    MessageHandler( NULL, " Montana--North" );
	    break;
	case 2502:
	    MessageHandler( NULL, " Montana--Central" );
	    break;
	case 2503:
	    MessageHandler( NULL, " Montana--South" );
	    break;
	case 2601:
	    MessageHandler( NULL, " Nebraska--North" );
	    break;
	case 2602:
	    MessageHandler( NULL, " Nebraska--South" );
	    break;
	case 2701:
	    MessageHandler( NULL, " Nevada--East" );
	    break;
	case 2702:
	    MessageHandler( NULL, " Nevada--Central" );
	    break;
	case 2703:
	    MessageHandler( NULL, " Nevada--West " );
	    break;
	case 2800:
	    MessageHandler( NULL, " New Hampshire" );
	    break;
	case 2900:
	    MessageHandler( NULL, " New Jersey " );
	    break;
	case 3001:
	    MessageHandler( NULL, " New Mexico--East" );
	    break;
	case 3002:
	    MessageHandler( NULL, " New Mexico--Central" );
	    break;
	case 3003:
	    MessageHandler( NULL, " New Mexico--West" );
	    break;
	case 3101:
	    MessageHandler( NULL, " New York--East" );
	    break;
	case 3102:
	    MessageHandler( NULL, " New York--Central" );
	    break;
	case 3103:
	    MessageHandler( NULL, " New York--West" );
	    break;
	case 3104:
	    MessageHandler( NULL, " New York--Long Island" );
	    break;
	case 3200:
	    MessageHandler( NULL, " North Carolina" );
	    break;
	case 3301:
	    MessageHandler( NULL, " North Dakota--North" );
	    break;
	case 3302:
	    MessageHandler( NULL, " North Dakota--South" );
	    break;
	case 3401:
	    MessageHandler( NULL, " Ohio--North" );
	    break;
	case 3402:
	    MessageHandler( NULL, " Ohio--South" );
	    break;
	case 3501:
	    MessageHandler( NULL, " Oklahoma--North" );
	    break;
	case 3502:
	    MessageHandler( NULL, " Oklahoma--South" );
	    break;
	case 3601:
	    MessageHandler( NULL, " Oregon--North" );
	    break;
	case 3602:
	    MessageHandler( NULL, " Oregon--South" );
	    break;
	case 3701:
	    MessageHandler( NULL, " Pennsylvania--North" );
	    break;
	case 3702:
	    MessageHandler( NULL, " Pennsylvania--South" );
	    break;
	case 3800:
	    MessageHandler( NULL, " Rhode Island " );
	    break;
	case 3901:
	    MessageHandler( NULL, " South Carolina--North" );
	    break;
	case 3902:
	    MessageHandler( NULL, " South Carolina--South" );
	    break;
	case 4001:
	    MessageHandler( NULL, " South Dakota--North" );
	    break;
	case 4002:
	    MessageHandler( NULL, " South Dakota--South" );
	    break;
	case 4100:
	    MessageHandler( NULL, " Tennessee" );
	    break;
	case 4201:
	    MessageHandler( NULL, " Texas--North" );
	    break;
	case 4202:
	    MessageHandler( NULL, " Texas--North Central" );
	    break;
	case 4203:
	    MessageHandler( NULL, " Texas--Central" );
	    break;
	case 4204:
	    MessageHandler( NULL, " Texas--South" );
	    break;
	case 4205:
	    MessageHandler( NULL, " Texas--South Central" );
	    break;
	case 4301:
	    MessageHandler( NULL, " Utah--North" );
	    break;
	case 4302:
	    MessageHandler( NULL, " Utah--Central" );
	    break;
	case 4303:
	    MessageHandler( NULL, " Utah--South" );
	    break;
	case 4400:
	    MessageHandler( NULL, " Vermont" );
	    break;
	case 4501:
	    MessageHandler( NULL, " Virginia--North" );
	    break;
	case 4502:
	    MessageHandler( NULL, " Virginia--South" );
	    break;
	case 4601:
	    MessageHandler( NULL, " Washington--North" );
	    break;
	case 4602:
	    MessageHandler( NULL, " Washington--South" );
	    break;
	case 4701:
	    MessageHandler( NULL, " West Virginia--North" );
	    break;
	case 4702:
	    MessageHandler( NULL, " West Virginia--South" );
	    break;
	case 4801:
	    MessageHandler( NULL, " Wisconsin--North" );
	    break;
	case 4802:
	    MessageHandler( NULL, " Wisconsin--Central" );
	    break;
	case 4803:
	    MessageHandler( NULL, " Wisconsin--South" );
	    break;
	case 4901:
	    MessageHandler( NULL, " Wyoming--East" );
	    break;
	case 4902:
	    MessageHandler( NULL, " Wyoming--East Central" );
	    break;
	case 4903:
	    MessageHandler( NULL, " Wyoming--West Central" );
	    break;
	case 4904:
	    MessageHandler( NULL, " Wyoming--West" );
	    break;
	case 5001:
	    MessageHandler( NULL, " Alaska--Zone 1" );
	    break;
	case 5002:
	    MessageHandler( NULL, " Alaska--Zone 2" );
	    break;
	case 5003:
	    MessageHandler( NULL, " Alaska--Zone 3" );
	    break;
	case 5004:
	    MessageHandler( NULL, " Alaska--Zone 4" );
	    break;
	case 5005:
	    MessageHandler( NULL, " Alaska--Zone 5" );
	    break;
	case 5006:
	    MessageHandler( NULL, " Alaska--Zone 6" );
	    break;
	case 5007:
	    MessageHandler( NULL, " Alaska--Zone 7" );
	    break;
	case 5008:
	    MessageHandler( NULL, " Alaska--Zone 8" );
	    break;
	case 5009:
	    MessageHandler( NULL, " Alaska--Zone 9" );
	    break;
	case 5010:
	    MessageHandler( NULL, " Alaska--Zone 10" );
	    break;
	case 5101:
	    MessageHandler( NULL, " Hawaii--Zone 1" );
	    break;
	case 5102:
	    MessageHandler( NULL, " Hawaii--Zone 2" );
	    break;
	case 5103:
	    MessageHandler( NULL, " Hawaii--Zone 3" );
	    break;
	case 5104:
	    MessageHandler( NULL, " Hawaii--Zone 4" );
	    break;
	case 5105:
	    MessageHandler( NULL, " Hawaii--Zone 5" );
	    break;
	case 5201:
	    MessageHandler( NULL,
			    " Puerto Rico & Virgin Islands--St. John, St. Thomas" );
	    break;
	case 5202:
	    MessageHandler( NULL, " Virgin Islands--St. Croix" );
	    break;
	case 5300:
	    MessageHandler( NULL, " American Samoa" );
	    break;
	case 5400:
	    MessageHandler( NULL, " Guam" );
	    break;
    }
}
