/******************************************************************************

FILE:  projutil.c

PURPOSE:  Misc projection modules

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  Rob Burrell            Original Development

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/
#include "shared_resample.h"
#include "worgen.h"

/******************************************************************************

MODULE:  DMS2Deg

PURPOSE:  Convert projection parameters from DMS to degrees

RETURN VALUE:
Type = int
Value           Description
-----           -----------
TRUE			success
FALSE			failure

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  Rob Burrell            Original Development
         04/01  Rob Burrell            Added TM projection

NOTES:

******************************************************************************/
int DMS2Deg
    ( 
	int projection_type,           /* I: see shared_resample.h for a list of
                                             types */
	double *projection_parameters  /* I/O: parameters to convert */
    )
{
	switch ( projection_type )
	{
		case PROJ_GEO:
		case PROJ_IGH:
			/* nothing to do? */
			break;

		case PROJ_HAM:
		case PROJ_ISIN:
		case PROJ_MOL:
		case PROJ_SIN:
			dmsdeg( projection_parameters[4],
                                &projection_parameters[4] );
			break;

		case PROJ_ER:
		case PROJ_LA:
		case PROJ_MERC:
		case PROJ_PS:
			dmsdeg( projection_parameters[4],
                                &projection_parameters[4] );
			dmsdeg( projection_parameters[5],
                                &projection_parameters[5] );
			break;

		case PROJ_UTM:
			dmsdeg( projection_parameters[0],
                                &projection_parameters[0] );
			dmsdeg( projection_parameters[1],
                                &projection_parameters[1] );
			break;

		case PROJ_AEA:
		case PROJ_LCC:
			dmsdeg( projection_parameters[2],
                                &projection_parameters[2] );
			dmsdeg( projection_parameters[3],
                                &projection_parameters[3] );
			dmsdeg( projection_parameters[4],
                                &projection_parameters[4] );
			dmsdeg( projection_parameters[5],
                                &projection_parameters[5] );
			break;

		case PROJ_TM:
			dmsdeg( projection_parameters[4],
                                &projection_parameters[4] );
			dmsdeg( projection_parameters[5],
                                &projection_parameters[5] );
			break;

		default:
			ErrorHandler( TRUE, "DMS2Deg", ERROR_GENERAL,
                            "Bad Projection Type" );
			return ( FALSE );
	}

	return ( TRUE );
}

/******************************************************************************

MODULE:  dmsdeg

PURPOSE:  Converts a single DMS value to decimal degrees

RETURN VALUE:
Type = none
Value           Description
-----           -----------

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  Rob Burrell            Original Development

NOTES:

******************************************************************************/
void dmsdeg
    ( 
	double inputvalue, 	/* I:  DMS value */
	double *outputvalue	/* O:  degree value */
    )
{
	int deg;		/* degrees */
	int min;		/* minutes */
	double sec;		/* decimal seconds */

	double tempstore;

	tempstore = inputvalue;

	if( inputvalue < 0.0 )
		inputvalue = -inputvalue;

	/* nothing to do? */
	if ( inputvalue == 0.0 )
		return;

	/* stomp the degrees */
	deg = ( int ) ( inputvalue / 1000000.0 );

	/* remove degrees and stomp minutes */
	min = ( int ) ( inputvalue / 1000.0 - deg * 1000.0 );

	/* remove degrees and minutes */
	sec = inputvalue - ( deg * 1000000.0 + min * 1000.0 );

	/* fix it up */
	*outputvalue = sec / 3600.0 + min / 60.0 + deg;

	if( tempstore < 0.0 )
		*outputvalue = -*outputvalue;
}

/******************************************************************************

MODULE:  Deg2DMS

PURPOSE:  Convert parameters from decimal degrees to DMS

RETURN VALUE:
Type = int
Value           Description
-----           -----------
TRUE			success
FALSE			failure

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  Rob Burrell            Original Development
         04/01  Rob Burrell            Added TM projection

NOTES:

******************************************************************************/
int Deg2DMS
    ( 
	int projection_type, 	        /* I:  see shared_resample.h for a list of
                                               types */
	double *projection_parameters	/* I/O:  parameters to convert */
    )
{
	int status = FALSE;		    /* error status */
	double temp1, temp2, temp3, temp4;  /* local temp storage for
                                               conversion */

	switch ( projection_type )
	{
		case PROJ_UTM:
                        if ( c_degdms( &projection_parameters[0], &temp1,
                                       "DEG", "LON" ) != E_GEO_FAIL &&
                             c_degdms( &projection_parameters[1], &temp2,
                                       "DEG", "LAT" ) != E_GEO_FAIL )
			{
				projection_parameters[0] = temp1;
				projection_parameters[1] = temp2;
				status = TRUE;
			}
			break;

		case PROJ_GEO:
		case PROJ_IGH:
			status = TRUE;
			/* nothing to do? */
			break;

		case PROJ_ISIN:
		case PROJ_HAM:
		case PROJ_MOL:
		case PROJ_SIN:
                        if ( c_degdms( &projection_parameters[4], &temp1,
                                       "DEG", "LON" ) != E_GEO_FAIL )
			{
				projection_parameters[4] = temp1;
				status = TRUE;
			}
			break;

		case PROJ_ER:
		case PROJ_LA:
		case PROJ_MERC:
		case PROJ_PS:
                        if ( c_degdms( &projection_parameters[4], &temp1,
                                       "DEG", "LON" ) != E_GEO_FAIL &&
                             c_degdms( &projection_parameters[5], &temp2,
                                       "DEG", "LAT" ) != E_GEO_FAIL )
			{
				projection_parameters[4] = temp1;
				projection_parameters[5] = temp2;
				status = TRUE;
			}
			break;

		case PROJ_AEA:
		case PROJ_LCC:
                        if ( c_degdms( &projection_parameters[2], &temp1,
                                       "DEG", "LAT" ) != E_GEO_FAIL &&
                             c_degdms( &projection_parameters[3], &temp2,
                                       "DEG", "LAT" ) != E_GEO_FAIL &&
                             c_degdms( &projection_parameters[4], &temp3,
                                       "DEG", "LON" ) != E_GEO_FAIL &&
                             c_degdms( &projection_parameters[5], &temp4,
                                       "DEG", "LAT" ) != E_GEO_FAIL )
			{
				projection_parameters[2] = temp1;
				projection_parameters[3] = temp2;
				projection_parameters[4] = temp3;
				projection_parameters[5] = temp4;
				status = TRUE;
			}
			break;

		case PROJ_TM:
                        if ( c_degdms( &projection_parameters[4], &temp1,
                                       "DEG", "LON" ) != E_GEO_FAIL &&
                             c_degdms( &projection_parameters[5], &temp2,
                                       "DEG", "LAT" ) != E_GEO_FAIL )
			{
				projection_parameters[4] = temp1;
				projection_parameters[5] = temp2;
				status = TRUE;
			}
			break;

		default:
			ErrorHandler( TRUE, "Deg2DMS", ERROR_GENERAL,
                                      "Bad Projection Type" );
	}

	return ( status );
}

/******************************************************************************

MODULE:  GetUTMZone

PURPOSE:  Calculate a UTM zone, taken from geolib

RETURN VALUE:
Type = long
Value           Description
-----           -----------
zone			the UTM zone

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  Rob Burrell            Original Development
         12/00  Rob Burrell            Add lat to generate a negative zone

NOTES:

******************************************************************************/
long GetUTMZone
    ( 
	double lon,    /* I:  longitude */
	double lat     /* I:  latitude */ 
    )
{
	int sign = 1;

	if( lat < 0.0 )
		sign = -1;	

	return ( sign * ( long ) ( ( ( lon + 180.0 ) / 6.0 ) + 1.0 ) );
}

