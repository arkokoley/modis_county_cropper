
/*****************************************************************************
	NAME:  C_DEGDMS

	PURPOSE:  To convert total degrees, total minutes, or total seconds to 
		  packed degress,minutes,seconds.

	PROGRAM HISTORY:
	PROGRAMMER	DATE	REASON
	----------	----	------
	B.Ailts		9/87	Original development 
	B. Ailts	12/87	Place bridge routine is seperate source file
				change include file directory specifications
				replace DESC arguements with char. strings
	B. Ailts	05/88	Change return status to E_GEO_SUCC and E_GEO_FAIL
				Changed newlas.h to worgen.h
				Standardized error messages
				Changed the name from c_deg2dms to c_degdms
	D. Steinwand 	07/89	Corrected some numerical problems--some
				angles were comming out as xxx060000.0
				(some degrees, 60 minutes, etc)
				Added find_deg, find_min, find_sec routines
	D. Etrheim	07/90	Standardized error message handling
 	S. Nelson	08/92	Changed "static" statements on find_deg,
				find_min, find_sec functions

	COMPUTER HARDWARE AND/OR SOFTWRE LIMITATIONS:
		Must be run under TAE

	PROJECT:	LAS

	ALGORITHM:
       	Receive an angle in seconds, minutes, or degrees
	Convert it to DMS.  
	The angle is then checked to be sure it is within the limits 
	of its use(LAT, LON, or DEGREES).

	ALGORITHM REFERENCES		none

******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "shared_resample.h"
#include "tae.h"
#include "worgen.h"
#include "geompak.h"

static int find_deg( double angle	/* Angle in total degrees */
     );

static int find_min( double angle	/* Angle in total degrees */
     );

static double find_sec( double angle	/* Angle in total degrees */
     );

int c_degdms( double *deg, double *dms, char *code, char *check )
{				/* degdms */

    double tsec;
    double MAXDMS;
    double MAXMIN = 60060;
    double MAXSEC = 60;
    double MINDMS;
    double MINMIN = -60060;
    double MINSEC = -60;
    double tempmin;
    double tempsec;

    long tdeg;
    long tmin;
    long sign;
    long status = E_GEO_SUCC;

    if ( strcmp( check, "LAT" ) == 0 )
    {
	MAXDMS = 90000000;
	MINDMS = -90000000;
    }
    else if ( strcmp( check, "LON" ) == 0 )
    {
	MAXDMS = 180000000;
	MINDMS = -180000000;
    }
    else
    {
	MAXDMS = 360000000;
	MINDMS = 0;
    }

    if ( strcmp( code, "DMS" ) != 0 )
    {
	if ( strcmp( code, "MIN" ) == 0 )
	{
	    *deg = *deg / 60.0;
	}
	if ( strcmp( code, "SEC" ) == 0 )
	{
	    *deg = *deg / 3600.0;
	}
	tdeg = ( long ) find_deg( *deg );
	tmin = ( long ) find_min( *deg );
	tsec = find_sec( *deg );
	sign = 1;
	if ( *deg < 0 )
	    sign = -1;
	tdeg = abs( tdeg );
	*dms = ( ( tdeg * 1000000 ) + ( tmin * 1000 ) + tsec ) * sign;

/*  Check to be sure coordinate is valid
----------------------------------------*/
	if ( ( *dms > MAXDMS ) || ( *dms < MINDMS ) )
	{
	    ErrorHandler( FALSE, "c_degdms", ERROR_PROJECTION,
			  "Invalid coordinate value" );
	    status = E_GEO_FAIL;
	}
    }
    else
    {
	*dms = *deg;

/*  Check to be sure coordinate is valid
----------------------------------------*/
	if ( ( *dms > MAXDMS ) || ( *dms < MINDMS ) )
	{
	    ErrorHandler( FALSE, "c_degdms", ERROR_PROJECTION,
			  "Invalid coordinate value" );
	    status = E_GEO_FAIL;
	}

	if ( ( strcmp( check, "LAT" ) != 0 ) && ( strcmp( check, "LON" ) != 0 ) )
	{
	    if ( *dms <= 0 )
	    {
		ErrorHandler( FALSE, "c_degdms", ERROR_PROJECTION,
			      "Invalid coordinate value" );
		status = E_GEO_FAIL;
	    }
	}

/*  parse out the minutes value from DMS and check against MAXMIN
-----------------------------------------------------------------*/
	tempmin = *dms - ( ( ( int ) ( *dms / 1000000 ) ) * 1000000 );
	if ( ( tempmin > MAXMIN ) || ( tempmin < MINMIN ) )
	{
	    ErrorHandler( FALSE, "c_degdms", ERROR_PROJECTION,
			  "Invalid coordinate value" );
	    status = E_GEO_FAIL;
	}

/*  parse out the seconds value from DMS and check against MAXSEC
-----------------------------------------------------------------*/
	tempsec = *dms - ( ( ( int ) ( *dms / 1000 ) ) * 1000 );
	if ( ( tempsec > MAXSEC ) || ( tempsec < MINSEC ) )
	{
	    ErrorHandler( FALSE, "c_degdms", ERROR_PROJECTION,
			  "Invalid coordinate value" );
	    status = E_GEO_FAIL;
	}
    }

    return ( status );
}				/* degdms */

/****************************************************************************
NAME:			find_deg, find_min, find_sec	

PURPOSE:  Extracts deg, min, or sec portions of an angle

PROGRAM HISTORY:
VERSION	 DATE	AUTHOR	   CODE/CONT   REASON
-------	 ----	------	   ---------   -----------------------------
  5.0	 5/89	D. Steinwand  CSB      LAS 5.0 (original) Development

COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:   must be run under TAE

PROJECT:  LAS

ALGORITHM DESCRIPTION:
	Extract portion of angle
	Return
*****************************************************************************/
static int find_deg( double angle	/* Angle in total degrees */
     )
{
    long sign;			/* Sign of angle */
    long deg;			/* Degrees portion of angle */
    long minute;		/* Minutes portion of angle */
    double sec;			/* Seconds portion of angle */

    deg = ( long ) angle;
    sign = 1;
    if ( deg < 0 )
	sign = -1;
    deg = ( long ) fabs( angle );
    minute = ( long ) ( ( fabs( angle ) - deg ) * 60.0 );
    sec = ( ( ( fabs( angle ) - deg ) * 60.0 ) - minute ) * 60.0;
    if ( sec >= 59.999 )
	minute++;
    if ( minute >= 60 )
	deg++;
    deg *= sign;
    return ( deg );
}

static int find_min( double angle	/* Angle in total degrees */
     )
{
    double sec;			/* Seconds portion of angle */
    long minute;		/* Minutes portion of angle */

    angle = fabs( angle );
    angle -= ( long ) angle;
    minute = ( long ) ( angle * 60.0 );
    sec = ( ( angle * 60.0 ) - minute ) * 60.0;
    if ( sec > 59.999 )
	minute++;
    if ( minute >= 60 )
	minute -= 60;
    return ( minute );
}

static double find_sec( double angle	/* Angle in total degrees */
     )
{
    long temp_angle;

    angle = fabs( angle );
    angle -= ( long ) angle;
    angle *= 60.0;
    angle -= ( long ) angle;
    angle *= 60.0;
    if ( angle > 59.999 )
	angle -= 60.0;
    temp_angle = ( long ) ( angle * 1000.0 );	/* Truncate to 0.001 sec */
    angle = temp_angle / 1000.0;
    return ( angle );
}
