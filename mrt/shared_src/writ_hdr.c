/******************************************************************************

FILE:  writ_hdr.c

PURPOSE:  Write raw binary headers

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  John Weiss             Original Development

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/
#include "shared_resample.h"
#include "mrt_dtype.h"

/******************************************************************************

MODULE:  WriteHeaderFile

PURPOSE:  Write a raw binary header

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status		See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  John Weiss             Original Development
         06/00  Rob Burrell            Output info from ModisDescriptor
         06/00  Rob Burrell            ParamsDMS2Deg fix
         07/00  Rob Burrell            Error return values
         01/01  John Rishea            Standardized formatting
         02/01  John Weiss             Add UTM zone to header
         04/01  Rob Burrell            Added TM projection
         07/01  Gail Schmidt           UTM_ZONE and ELLIPSOID_CODE no longer
                                       are commented fields
         12/01  Gail Schmidt           ELLIPSOID_CODE is now DATUM for all
                                       projections
         07/03  Gail Schmidt           Added Equirectangular projection

NOTES:

******************************************************************************/
int WriteHeaderFile
(
    ModisDescriptor *P	/* I:  session info */
)

{
    FILE *fpo = NULL;		/* output file pointer */
    char str[SMALL_STRING];	/* temp string */
    char *ext = NULL;		/* file extension pointer */
    size_t i, j, tbands;	/* index and bands */
    int sfprint;		/* flag to print scale factor, offset */

	/*******************************************************************/

    /* open header file for writing */
    strcpy( str, P->output_filename );
    ext = strrchr( str, '.' );
    if ( ext != NULL )
	ext[0] = '\0';
    strcat( str, ".hdr" );
    fpo = fopen( str, "w" );

    /* check if header file was successfully opened */
    if ( fpo == NULL )
    {
	sprintf( str, "Unable to open %s", P->output_filename );
	ErrorHandler( TRUE, "WriteHeaderFile", ERROR_OPEN_OUTPUTHEADER, str );

	return ERROR_OPEN_OUTPUTHEADER;
    }

	/*******************************************************************/

    /* write output projection type: PROJECTION_TYPE = ... */

    fprintf( fpo, "\nPROJECTION_TYPE = " );

    switch ( P->output_projection_type )
    {
	case PROJ_ISIN:
	    fprintf( fpo, "INTEGERIZED_SINUSOIDAL\n" );
	    break;
	case PROJ_AEA:
	    fprintf( fpo, "ALBERS_EQUAL_AREA\n" );
	    break;
	case PROJ_ER:
	    fprintf( fpo, "EQUIRECTANGULAR\n" );
	    break;
	case PROJ_GEO:
	    fprintf( fpo, "GEOGRAPHIC\n" );
	    break;
	case PROJ_HAM:
	    fprintf( fpo, "HAMMER\n" );
	    break;
	case PROJ_IGH:
	    fprintf( fpo, "INTERRUPTED_GOODE_HOMOLOSINE\n" );
	    break;
	case PROJ_LA:
	    fprintf( fpo, "LAMBERT_AZIMUTHAL\n" );
	    break;
	case PROJ_LCC:
	    fprintf( fpo, "LAMBERT_CONFORMAL_CONIC\n" );
	    break;
	case PROJ_MERC:
	    fprintf( fpo, "MERCATOR\n" );
	    break;
	case PROJ_MOL:
	    fprintf( fpo, "MOLLWEIDE\n" );
	    break;
	case PROJ_PS:
	    fprintf( fpo, "POLAR_STEREOGRAPHIC\n" );
	    break;
	case PROJ_SIN:
	    fprintf( fpo, "SINUSOIDAL\n" );
	    break;
	case PROJ_TM:
	    fprintf( fpo, "TRANSVERSE_MERCATOR\n" );
	    break;
	case PROJ_UTM:
	    fprintf( fpo, "UTM\n" );

            /* write UTM zone (as comment) */
            fprintf( fpo, "\nUTM_ZONE = %i\n", P->output_zone_code );
	    break;

	default:
	    ErrorHandler( TRUE, "WriteHeaderFile", ERROR_PROJECTION_TYPE,
		"Bad output projection type" );
	    return ERROR_PROJECTION_TYPE;
    }

	/*******************************************************************/

    /*
     * write 15 output projection parameters:
     *
     * PROJECTION_PARAMETERS = ( p1 p2 ... p15 )
     */

    fprintf( fpo, "\nPROJECTION_PARAMETERS = (" );

    for ( i = 0; i < NUM_PROJECTION_PARAMS; i++ )
    {
	if ( i % 3 == 0 )
	    fprintf( fpo, "\n" );

	fprintf( fpo, "%24.9f", P->output_projection_parameters[i] );
    }
    fprintf( fpo, " )\n" );

	/*******************************************************************/

    /* write coordinate system origin (as comment) */

    fprintf( fpo, "\n# COORDINATE_ORIGIN = " );
    switch ( P->coord_origin )
    {
	case UL:
	    fprintf( fpo, "UL\n" );
	    break;
	case UR:
	    fprintf( fpo, "UR\n" );
	    break;
	case LL:
	    fprintf( fpo, "LL\n" );
	    break;
	case LR:
	    fprintf( fpo, "LR\n" );
	    break;
    }

	/*******************************************************************/

    /*
     * write spatial extents (lat/lon)
     *
     * UL_CORNER_LATLON = ( ULlat ULlon )
     * UR_CORNER_LATLON = ( URlat URlon )
     * LL_CORNER_LATLON = ( LLlat LLlon )
     * LR_CORNER_LATLON = ( LRlat LRlon )
     */

    fprintf( fpo, "\n# Corner point values refer to the outer extent of the "
                  "pixel\n" );
    fprintf( fpo, "UL_CORNER_LATLON = ( %.9f %.9f )\n",
	P->ll_extents_corners[0][0], P->ll_extents_corners[0][1] );

    fprintf( fpo, "UR_CORNER_LATLON = ( %.9f %.9f )\n",
	P->ll_extents_corners[1][0], P->ll_extents_corners[1][1] );

    fprintf( fpo, "LL_CORNER_LATLON = ( %.9f %.9f )\n",
	P->ll_extents_corners[2][0], P->ll_extents_corners[2][1] );

    fprintf( fpo, "LR_CORNER_LATLON = ( %.9f %.9f )\n",
	P->ll_extents_corners[3][0], P->ll_extents_corners[3][1] );

	/*******************************************************************/

    /*
     * write spatial extents (projection coordinates)
     *
     * UL_CORNER_XY = ( ULlat ULlon )
     * UR_CORNER_XY = ( URlat URlon )
     * LL_CORNER_XY = ( LLlat LLlon )
     * LR_CORNER_XY = ( LRlat LRlon )
     */

    fprintf( fpo, "\n# UL_CORNER_XY = ( %.9f %.9f )\n",
	P->proj_extents_corners[0][0], P->proj_extents_corners[0][1] );

    fprintf( fpo, "# UR_CORNER_XY = ( %.9f %.9f )\n",
	P->proj_extents_corners[1][0], P->proj_extents_corners[1][1] );

    fprintf( fpo, "# LL_CORNER_XY = ( %.9f %.9f )\n",
	P->proj_extents_corners[2][0], P->proj_extents_corners[2][1] );

    fprintf( fpo, "# LR_CORNER_XY = ( %.9f %.9f )\n",
	P->proj_extents_corners[3][0], P->proj_extents_corners[3][1] );

	/*******************************************************************/

    /* compute number of files and number of bands in each one */
    /* note: assume bands of same spatial rez are contiguous */

    /* count total output bands */
    tbands = 0;
    for ( i = 0; i < P->nbands; i++ )
	if ( P->bandinfo[i].selected )
	    tbands++;

    /* error if no selected bands */
    if ( tbands == 0 )
    {
	ErrorHandler( TRUE, "WriteHeaderFile", ERROR_NO_BANDS,
            "No bands selected" );
	return ERROR_NO_BANDS;
    }

	/*******************************************************************/

    /* write total number of bands: NBANDS = n */

    fprintf( fpo, "\nNBANDS = " MRT_SIZE_T_FMT "\n", tbands );

	/*******************************************************************/

    /* write band names */

    fprintf( fpo, "BANDNAMES = (" );
    for ( i = 0; i < P->nbands; i++ )
    {
	if ( P->bandinfo[i].selected )
	{
	    strcpy( str, P->bandinfo[i].name );
	    SpaceToUnderscore( str );
	    fprintf( fpo, " %s", str );
        }
    }
    fprintf( fpo, " )\n" );

	/*******************************************************************/

    /* write output data type: DATA_TYPE = ... */

    fprintf( fpo, "DATA_TYPE = (" );

    for ( i = 0; i < P->nbands; i++ )
    {
	if ( P->bandinfo[i].selected )
	    switch ( P->bandinfo[i].output_datatype )
	    {
		case DFNT_INT8:
		    fprintf( fpo, " INT8" );
		    break;
		case DFNT_UINT8:
		    fprintf( fpo, " UINT8" );
		    break;
		case DFNT_INT16:
		    fprintf( fpo, " INT16" );
		    break;
		case DFNT_UINT16:
		    fprintf( fpo, " UINT16" );
		    break;
		case DFNT_INT32:
		    fprintf( fpo, " INT32" );
		    break;
		case DFNT_UINT32:
		    fprintf( fpo, " UINT32" );
		    break;
		case DFNT_FLOAT32:
		    fprintf( fpo, " FLOAT32" );
		    break;
		default:
		    ErrorHandler( TRUE, "WriteHeaderFile", ERROR_DATA_TYPE,
			"Bad output data type" );
		    return ERROR_DATA_TYPE;
	    }
    }
    fprintf( fpo, " )\n" );

	/*******************************************************************/

    /* write NLINES field */
    fprintf( fpo, "NLINES = (" );

    for ( i = j = 0; i < P->nbands; i++ )
    {
	if ( P->bandinfo[i].selected )
	    fprintf( fpo, " %i", P->output_file_info[j++].nlines );
    }
    fprintf( fpo, " )\n" );

	/*******************************************************************/

    /* write NSAMPLES field */

    fprintf( fpo, "NSAMPLES = (" );

    for ( i = j = 0; i < P->nbands; i++ )
    {
	if ( P->bandinfo[i].selected )
	    fprintf( fpo, " %i", P->output_file_info[j++].nsamples );
    }
    fprintf( fpo, " )\n" );

	/*******************************************************************/

    /* write PIXEL_SIZE field */

    fprintf( fpo, "PIXEL_SIZE = (" );

    for ( i = j = 0; i < P->nbands; i++ )
    {
	if ( P->bandinfo[i].selected )
	{
            /* output the GEO projections in degrees and other projections
               in meters */
	    if ( P->output_projection_type == PROJ_GEO )
		fprintf( fpo, " %.12f", P->output_file_info[j++].pixel_size );
	    else
		fprintf( fpo, " %.12f", P->output_file_info[j++].pixel_size );
	}
    }
    fprintf( fpo, " )\n" );

	/*******************************************************************/

    /* write MIN_VALUE field */
    {
	fprintf( fpo, "MIN_VALUE = (" );

	for ( i = 0; i < P->nbands; i++ )
	{
	    if ( P->bandinfo[i].selected )
		switch ( P->bandinfo[i].output_datatype )
		{
		    case DFNT_INT8:
			fprintf( fpo, " %d",
                                 ( MRT_INT8 ) P->bandinfo[i].min_value );
			break;
		    case DFNT_UINT8:
			fprintf( fpo, " %u",
			    ( MRT_UINT8 ) P->bandinfo[i].min_value );
			break;
		    case DFNT_INT16:
			fprintf( fpo, " %d",
                            ( MRT_INT16 ) P->bandinfo[i].min_value );
			break;
		    case DFNT_UINT16:
			fprintf( fpo, " %u",
			    ( MRT_UINT16 ) P->bandinfo[i].min_value );
			break;
		    case DFNT_INT32:
			fprintf( fpo, " " MRT_INT32_FMT,
                            ( MRT_INT32 ) P->bandinfo[i].min_value );
			break;
		    case DFNT_UINT32:
			fprintf( fpo, " " MRT_UINT32_FMT,
			    ( MRT_UINT32 ) P->bandinfo[i].min_value );
			break;
		    case DFNT_FLOAT32:
			fprintf( fpo, " %f",
                            ( float ) P->bandinfo[i].min_value );
			break;
		    default:
			sprintf( str, "Bad datatype min value for band "
                                 MRT_SIZE_T_FMT, i );
			ErrorHandler( TRUE, "WriteHeaderFile", ERROR_DATA_TYPE,
			    str );
			return ERROR_DATA_TYPE;
		}
	}
	fprintf( fpo, " )\n" );
    }

	/*******************************************************************/

    /* write MAX_VALUE field */

#if 0
    /* find first selected band */
    for ( i = 0; i < P->nbands; i++ )
	if ( P->bandinfo[i].selected )
	    break;

    if ( P->bandinfo[i].min_value != P->bandinfo[i].max_value )
#endif
    {
	fprintf( fpo, "MAX_VALUE = (" );

	for ( i = 0; i < P->nbands; i++ )
	{
	    if ( P->bandinfo[i].selected )
		switch ( P->bandinfo[i].output_datatype )
		{
		    case DFNT_INT8:
			fprintf( fpo, " %d",
                            ( MRT_INT8 ) P->bandinfo[i].max_value );
			break;
		    case DFNT_UINT8:
			fprintf( fpo, " %u",
			    ( MRT_UINT8 ) P->bandinfo[i].max_value );
			break;
		    case DFNT_INT16:
			fprintf( fpo, " %d",
                            ( MRT_INT16 ) P->bandinfo[i].max_value );
			break;
		    case DFNT_UINT16:
			fprintf( fpo, " %u",
			    ( MRT_UINT16 ) P->bandinfo[i].max_value );
			break;
		    case DFNT_INT32:
			fprintf( fpo, " " MRT_INT32_FMT,
                            ( MRT_INT32 ) P->bandinfo[i].max_value );
			break;
		    case DFNT_UINT32:
			fprintf( fpo, " " MRT_UINT32_FMT,
			    ( MRT_UINT32 ) P->bandinfo[i].max_value );
			break;
		    case DFNT_FLOAT32:
			fprintf( fpo, " %f",
                            ( float ) P->bandinfo[i].max_value );
			break;
		    default:
			sprintf( str, "Bad datatype max value for band "
                                 MRT_SIZE_T_FMT, i );
			ErrorHandler( TRUE, "WriteHeaderFile", ERROR_DATA_TYPE,
			    str );
			return ERROR_DATA_TYPE;
		}
	}
	fprintf( fpo, " )\n" );
    }

	/*******************************************************************/

    /* write BACKGROUND_FILL field */

#if 0
    /* find first selected band */
    for ( i = 0; i < P->nbands; i++ )
	if ( P->bandinfo[i].selected )
	    break;

    if ( P->bandinfo[i].background_fill != P->bandinfo[i].min_value )
#endif
    {
	fprintf( fpo, "BACKGROUND_FILL = (" );

	for ( i = 0; i < P->nbands; i++ )
	{
	    if ( P->bandinfo[i].selected )
		switch ( P->bandinfo[i].output_datatype )
		{
		    case DFNT_INT8:
			fprintf( fpo, " %d",
			    ( MRT_INT8 ) P->bandinfo[i].background_fill );
			break;
		    case DFNT_UINT8:
			fprintf( fpo, " %u",
			    ( MRT_UINT8 ) P->bandinfo[i].background_fill );
			break;
		    case DFNT_INT16:
			fprintf( fpo, " %d",
			    ( MRT_INT16 ) P->bandinfo[i].background_fill );
			break;
		    case DFNT_UINT16:
			fprintf( fpo, " %u",
			    ( MRT_UINT16 ) P->bandinfo[i].background_fill );
			break;
		    case DFNT_INT32:
			fprintf( fpo, " " MRT_INT32_FMT,
			    ( MRT_INT32 ) P->bandinfo[i].background_fill );
			break;
		    case DFNT_UINT32:
			fprintf( fpo, " " MRT_UINT32_FMT,
			    ( MRT_UINT32 ) P->bandinfo[i].background_fill );
			break;
		    case DFNT_FLOAT32:
			fprintf( fpo, " %f",
			    ( float ) P->bandinfo[i].background_fill );
			break;
		    default:
			sprintf( str, "Bad datatype fill value for band "
                                 MRT_SIZE_T_FMT, i );
			ErrorHandler( TRUE, "WriteHeaderFile", ERROR_DATA_TYPE,
			    str );
			return ERROR_DATA_TYPE;
		}
	}

	fprintf( fpo, " )\n" );
    }

	/*******************************************************************/

    /* write the datum value */

    fprintf( fpo, "\nDATUM = " );
    switch( P->output_datum_code )
    {
        case E_NAD27:
            fprintf( fpo, "NAD27\n" );
            break;
        case E_NAD83:
            fprintf( fpo, "NAD83\n" );
            break;
        case E_WGS66:
            fprintf( fpo, "WGS66\n" );
            break;
        case E_WGS72:
            fprintf( fpo, "WGS72\n" );
            break;
        case E_WGS84:
            fprintf( fpo, "WGS84\n" );
            break;
        case E_NODATUM:
        default:
            if (P->input_projection_type != PROJ_UTM ||
                P->output_projection_type != PROJ_UTM)
            {
                fprintf( fpo, "NODATUM\n" );
            }
            else
            {
                fprintf( fpo, "sphere_%d\n", P->input_sphere_code );
            }
            break;
    }

        /*******************************************************************/

    /* write BYTE_ORDER field */
    fprintf( fpo, "\nBYTE_ORDER = " );
    switch( GetMachineEndianness() )
    {
        case MRT_BIG_ENDIAN:
            fprintf( fpo, "big_endian\n" );
            break;
        case MRT_LITTLE_ENDIAN:
            fprintf( fpo, "little_endian\n" );
            break;
        default:
            ErrorHandler( TRUE, "WriteHeaderFile", ERROR_DATA_TYPE,
                          "Bad endian value for BYTE_ORDER" );
            return ERROR_DATA_TYPE;
    }

        /*******************************************************************/

    /* write SCALE_FACTOR field */

    sfprint = 0;
    for ( i = 0; i < P->nbands; i++ )
	if ( P->bandinfo[i].selected )
	    if ( P->bandinfo[i].scale_factor != 0.0 )
	    {
		sfprint = 1;
		break;
	    }

    if ( sfprint )
    {
	fprintf( fpo, "\n# SCALE_FACTOR = (" );

	for ( i = 0; i < P->nbands; i++ )
	    if ( P->bandinfo[i].selected )
		fprintf( fpo, " %f", P->bandinfo[i].scale_factor );
	fprintf( fpo, " )\n" );
    }

	/*******************************************************************/

    /* write OFFSET field */

    if ( sfprint )
    {
	fprintf( fpo, "# OFFSET = (" );

	for ( i = 0; i < P->nbands; i++ )
	    if ( P->bandinfo[i].selected )
		fprintf( fpo, " %f", P->bandinfo[i].offset );
	fprintf( fpo, " )\n" );
    }

	/*******************************************************************/

    /* finish up */

    fclose( fpo );
    return MRT_NO_ERROR;
}


/******************************************************************************

MODULE:  WriteHeaderFileHDF2RB

PURPOSE:  Write a raw binary header for the HDF to raw binary converter

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status		See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         11/05  Gail Schmidt           Original Development

NOTES:

******************************************************************************/
int WriteHeaderFileHDF2RB
(
    ModisDescriptor *P	/* I:  session info */
)

{
    FILE *fpo = NULL;		/* output file pointer */
    char str[SMALL_STRING];	/* temp string */
    char *ext = NULL;		/* file extension pointer */
    size_t i, j, tbands;        /* index and bands */
    int sfprint;		/* flag to print scale factor, offset */

	/*******************************************************************/

    /* open header file for writing */
    strcpy( str, P->output_filename );
    ext = strrchr( str, '.' );
    if ( ext != NULL )
	ext[0] = '\0';
    strcat( str, ".hdr" );
    fpo = fopen( str, "w" );

    /* check if header file was successfully opened */
    if ( fpo == NULL )
    {
	sprintf( str, "Unable to open %s", P->output_filename );
	ErrorHandler( TRUE, "WriteHeaderFileHDF2RB", ERROR_OPEN_OUTPUTHEADER,
            str );

	return ERROR_OPEN_OUTPUTHEADER;
    }

	/*******************************************************************/

    /* write output projection type: PROJECTION_TYPE = ... */

    fprintf( fpo, "\nPROJECTION_TYPE = " );

    switch ( P->output_projection_type )
    {
	case PROJ_ISIN:
	    fprintf( fpo, "INTEGERIZED_SINUSOIDAL\n" );
	    break;
	case PROJ_AEA:
	    fprintf( fpo, "ALBERS_EQUAL_AREA\n" );
	    break;
	case PROJ_ER:
	    fprintf( fpo, "EQUIRECTANGULAR\n" );
	    break;
	case PROJ_GEO:
	    fprintf( fpo, "GEOGRAPHIC\n" );
	    break;
	case PROJ_HAM:
	    fprintf( fpo, "HAMMER\n" );
	    break;
	case PROJ_IGH:
	    fprintf( fpo, "INTERRUPTED_GOODE_HOMOLOSINE\n" );
	    break;
	case PROJ_LA:
	    fprintf( fpo, "LAMBERT_AZIMUTHAL\n" );
	    break;
	case PROJ_LCC:
	    fprintf( fpo, "LAMBERT_CONFORMAL_CONIC\n" );
	    break;
	case PROJ_MERC:
	    fprintf( fpo, "MERCATOR\n" );
	    break;
	case PROJ_MOL:
	    fprintf( fpo, "MOLLWEIDE\n" );
	    break;
	case PROJ_PS:
	    fprintf( fpo, "POLAR_STEREOGRAPHIC\n" );
	    break;
	case PROJ_SIN:
	    fprintf( fpo, "SINUSOIDAL\n" );
	    break;
	case PROJ_TM:
	    fprintf( fpo, "TRANSVERSE_MERCATOR\n" );
	    break;
	case PROJ_UTM:
	    fprintf( fpo, "UTM\n" );

            /* write UTM zone (as comment) */
            fprintf( fpo, "\nUTM_ZONE = %i\n", P->output_zone_code );
	    break;

	default:
	    ErrorHandler( TRUE, "WriteHeaderFileHDF2RB", ERROR_PROJECTION_TYPE,
		"Bad output projection type" );
	    return ERROR_PROJECTION_TYPE;
    }

	/*******************************************************************/

    /*
     * write 15 output projection parameters:
     *
     * PROJECTION_PARAMETERS = ( p1 p2 ... p15 )
     */

    fprintf( fpo, "\nPROJECTION_PARAMETERS = (" );

    for ( i = 0; i < NUM_PROJECTION_PARAMS; i++ )
    {
	if ( i % 3 == 0 )
	    fprintf( fpo, "\n" );

	fprintf( fpo, "%24.9f", P->output_projection_parameters[i] );
    }
    fprintf( fpo, " )\n" );

	/*******************************************************************/

    /*
     * write spatial extents (projection coordinates)
     *
     * UL_CORNER_XY = ( ULlat ULlon )
     * UR_CORNER_XY = ( URlat URlon )
     * LL_CORNER_XY = ( LLlat LLlon )
     * LR_CORNER_XY = ( LRlat LRlon )
     */

    fprintf( fpo, "\nUL_CORNER_XY = ( %.9f %.9f )\n",
	P->proj_extents_corners[0][0], P->proj_extents_corners[0][1] );

    fprintf( fpo, "UR_CORNER_XY = ( %.9f %.9f )\n",
	P->proj_extents_corners[1][0], P->proj_extents_corners[1][1] );

    fprintf( fpo, "LL_CORNER_XY = ( %.9f %.9f )\n",
	P->proj_extents_corners[2][0], P->proj_extents_corners[2][1] );

    fprintf( fpo, "LR_CORNER_XY = ( %.9f %.9f )\n",
	P->proj_extents_corners[3][0], P->proj_extents_corners[3][1] );

	/*******************************************************************/

    /* compute number of files and number of bands in each one */
    /* note: assume bands of same spatial rez are contiguous */

    /* count total output bands */
    tbands = 0;
    for ( i = 0; i < P->nbands; i++ )
	if ( P->bandinfo[i].selected )
	    tbands++;

    /* error if no selected bands */
    if ( tbands == 0 )
    {
	ErrorHandler( TRUE, "WriteHeaderFileHDF2RB", ERROR_NO_BANDS,
            "No bands selected" );
	return ERROR_NO_BANDS;
    }

	/*******************************************************************/

    /* write total number of bands: NBANDS = n */

    fprintf( fpo, "\nNBANDS = " MRT_SIZE_T_FMT "\n", tbands );

	/*******************************************************************/

    /* write band names */

    fprintf( fpo, "BANDNAMES = (" );
    for ( i = 0; i < P->nbands; i++ )
    {
	if ( P->bandinfo[i].selected )
	{
	    strcpy( str, P->bandinfo[i].name );
	    SpaceToUnderscore( str );
	    fprintf( fpo, " %s", str );
        }
    }
    fprintf( fpo, " )\n" );

	/*******************************************************************/

    /* write output data type: DATA_TYPE = ... */

    fprintf( fpo, "DATA_TYPE = (" );

    for ( i = 0; i < P->nbands; i++ )
    {
	if ( P->bandinfo[i].selected )
	    switch ( P->bandinfo[i].output_datatype )
	    {
		case DFNT_INT8:
		    fprintf( fpo, " INT8" );
		    break;
		case DFNT_UINT8:
		    fprintf( fpo, " UINT8" );
		    break;
		case DFNT_INT16:
		    fprintf( fpo, " INT16" );
		    break;
		case DFNT_UINT16:
		    fprintf( fpo, " UINT16" );
		    break;
		case DFNT_INT32:
		    fprintf( fpo, " INT32" );
		    break;
		case DFNT_UINT32:
		    fprintf( fpo, " UINT32" );
		    break;
		case DFNT_FLOAT32:
		    fprintf( fpo, " FLOAT32" );
		    break;
		default:
		    ErrorHandler( TRUE, "WriteHeaderFileHDF2RB", ERROR_DATA_TYPE,
			"Bad output data type" );
		    return ERROR_DATA_TYPE;
	    }
    }
    fprintf( fpo, " )\n" );

	/*******************************************************************/

    /* write NLINES field */
    fprintf( fpo, "NLINES = (" );

    for ( i = j = 0; i < P->nbands; i++ )
    {
	if ( P->bandinfo[i].selected )
	    fprintf( fpo, " %i", P->output_file_info[j++].nlines );
    }
    fprintf( fpo, " )\n" );

	/*******************************************************************/

    /* write NSAMPLES field */

    fprintf( fpo, "NSAMPLES = (" );

    for ( i = j = 0; i < P->nbands; i++ )
    {
	if ( P->bandinfo[i].selected )
	    fprintf( fpo, " %i", P->output_file_info[j++].nsamples );
    }
    fprintf( fpo, " )\n" );

	/*******************************************************************/

    /* write PIXEL_SIZE field */

    fprintf( fpo, "PIXEL_SIZE = (" );

    for ( i = j = 0; i < P->nbands; i++ )
    {
	if ( P->bandinfo[i].selected )
	{
            /* output the GEO projections in degrees and other projections
               in meters */
	    if ( P->output_projection_type == PROJ_GEO )
		fprintf( fpo, " %.12f", P->output_file_info[j++].pixel_size );
	    else
		fprintf( fpo, " %.12f", P->output_file_info[j++].pixel_size );
	}
    }
    fprintf( fpo, " )\n" );

	/*******************************************************************/

    /* write MIN_VALUE field */
    {
	fprintf( fpo, "MIN_VALUE = (" );

	for ( i = 0; i < P->nbands; i++ )
	{
	    if ( P->bandinfo[i].selected )
		switch ( P->bandinfo[i].output_datatype )
		{
		    case DFNT_INT8:
			fprintf( fpo, " %d",
                            ( MRT_INT8 ) P->bandinfo[i].min_value );
			break;
		    case DFNT_UINT8:
			fprintf( fpo, " %u",
			    ( MRT_UINT8 ) P->bandinfo[i].min_value );
			break;
		    case DFNT_INT16:
			fprintf( fpo, " %d",
                            ( MRT_INT16 ) P->bandinfo[i].min_value );
			break;
		    case DFNT_UINT16:
			fprintf( fpo, " %u",
			    ( MRT_UINT16 ) P->bandinfo[i].min_value );
			break;
		    case DFNT_INT32:
			fprintf( fpo, " " MRT_INT32_FMT,
                            ( MRT_INT32 ) P->bandinfo[i].min_value );
			break;
		    case DFNT_UINT32:
			fprintf( fpo, " " MRT_UINT32_FMT,
			    ( MRT_UINT32 ) P->bandinfo[i].min_value );
			break;
		    case DFNT_FLOAT32:
			fprintf( fpo, " %f",
                            ( float ) P->bandinfo[i].min_value );
			break;
		    default:
			sprintf( str, "Bad datatype min value for band "
                        MRT_SIZE_T_FMT, i );
			ErrorHandler( TRUE, "WriteHeaderFileHDF2RB",
                                      ERROR_DATA_TYPE, str );
			return ERROR_DATA_TYPE;
		}
	}
	fprintf( fpo, " )\n" );
    }

	/*******************************************************************/

    /* write MAX_VALUE field */

    fprintf( fpo, "MAX_VALUE = (" );

    for ( i = 0; i < P->nbands; i++ )
    {
        if ( P->bandinfo[i].selected )
            switch ( P->bandinfo[i].output_datatype )
            {
                case DFNT_INT8:
                    fprintf( fpo, " %d",
                        ( MRT_INT8 ) P->bandinfo[i].max_value );
                    break;
                case DFNT_UINT8:
                    fprintf( fpo, " %u",
                        ( MRT_UINT8 ) P->bandinfo[i].max_value );
                    break;
                case DFNT_INT16:
                    fprintf( fpo, " %d",
                        ( MRT_INT16 ) P->bandinfo[i].max_value );
                    break;
                case DFNT_UINT16:
                    fprintf( fpo, " %u",
                        ( MRT_UINT16 ) P->bandinfo[i].max_value );
                    break;
                case DFNT_INT32:
                    fprintf( fpo, " " MRT_INT32_FMT,
                        ( MRT_INT32 ) P->bandinfo[i].max_value );
                    break;
                case DFNT_UINT32:
                    fprintf( fpo, " " MRT_UINT32_FMT,
                        ( MRT_UINT32 ) P->bandinfo[i].max_value );
                    break;
                case DFNT_FLOAT32:
                    fprintf( fpo, " %f",
                        ( float ) P->bandinfo[i].max_value );
                    break;
                default:
                    sprintf( str, "Bad datatype max value for band "
                             MRT_SIZE_T_FMT, i );
                    ErrorHandler( TRUE, "WriteHeaderFileHDF2RB",
                                  ERROR_DATA_TYPE, str );
                    return ERROR_DATA_TYPE;
            }
	}
	fprintf( fpo, " )\n" );

	/*******************************************************************/

    /* write BACKGROUND_FILL field */

	fprintf( fpo, "BACKGROUND_FILL = (" );

	for ( i = 0; i < P->nbands; i++ )
	{
	    if ( P->bandinfo[i].selected )
		switch ( P->bandinfo[i].output_datatype )
		{
		    case DFNT_INT8:
			fprintf( fpo, " %d",
			    ( MRT_INT8 ) P->bandinfo[i].background_fill );
			break;
		    case DFNT_UINT8:
			fprintf( fpo, " %u",
			    ( MRT_UINT8 ) P->bandinfo[i].background_fill );
			break;
		    case DFNT_INT16:
			fprintf( fpo, " %d",
			    ( MRT_INT16 ) P->bandinfo[i].background_fill );
			break;
		    case DFNT_UINT16:
			fprintf( fpo, " %u",
			    ( MRT_UINT16 ) P->bandinfo[i].background_fill );
			break;
		    case DFNT_INT32:
			fprintf( fpo, " " MRT_INT32_FMT,
			    ( MRT_INT32 ) P->bandinfo[i].background_fill );
			break;
		    case DFNT_UINT32:
			fprintf( fpo, " " MRT_UINT32_FMT,
			    ( MRT_UINT32 ) P->bandinfo[i].background_fill );
			break;
		    case DFNT_FLOAT32:
			fprintf( fpo, " %f",
			    ( float ) P->bandinfo[i].background_fill );
			break;
		    default:
			sprintf( str, "Bad datatype fill value for band "
                            MRT_SIZE_T_FMT, i );
			ErrorHandler( TRUE, "WriteHeaderFileHDF2RB",
                                      ERROR_DATA_TYPE, str );
			return ERROR_DATA_TYPE;
		}
	}

	fprintf( fpo, " )\n" );

	/*******************************************************************/

    /* write the datum value */

    fprintf( fpo, "\nDATUM = " );
    switch( P->output_datum_code )
    {
        case E_NAD27:
            fprintf( fpo, "NAD27\n" );
            break;
        case E_NAD83:
            fprintf( fpo, "NAD83\n" );
            break;
        case E_WGS66:
            fprintf( fpo, "WGS66\n" );
            break;
        case E_WGS72:
            fprintf( fpo, "WGS72\n" );
            break;
        case E_WGS84:
            fprintf( fpo, "WGS84\n" );
            break;
        case E_NODATUM:
        default:
            if (P->input_projection_type != PROJ_UTM ||
                P->output_projection_type != PROJ_UTM)
            {
                fprintf( fpo, "NODATUM\n" );
            }
            else
            {
                fprintf( fpo, "sphere_%d\n", P->input_sphere_code );
            }
            break;
    }

        /*******************************************************************/

    /* write BYTE_ORDER field */
    fprintf( fpo, "\nBYTE_ORDER = " );
    switch( GetMachineEndianness() )
    {
        case MRT_BIG_ENDIAN:
            fprintf( fpo, "big_endian\n" );
            break;
        case MRT_LITTLE_ENDIAN:
            fprintf( fpo, "little_endian\n" );
            break;
        default:
            ErrorHandler( TRUE, "WriteHeaderFile", ERROR_DATA_TYPE,
                          "Bad datatype fill value for BYTE_ORDER" );
            return ERROR_DATA_TYPE;
    }

        /*******************************************************************/

    /* write SCALE_FACTOR field */

    sfprint = 0;
    for ( i = 0; i < P->nbands; i++ )
	if ( P->bandinfo[i].selected )
	    if ( P->bandinfo[i].scale_factor != 0.0 )
	    {
		sfprint = 1;
		break;
	    }

    if ( sfprint )
    {
	fprintf( fpo, "# SCALE_FACTOR = (" );

	for ( i = 0; i < P->nbands; i++ )
	    if ( P->bandinfo[i].selected )
		fprintf( fpo, " %f", P->bandinfo[i].scale_factor );
	fprintf( fpo, " )\n" );
    }

	/*******************************************************************/

    /* write OFFSET field */

    if ( sfprint )
    {
	fprintf( fpo, "# OFFSET = (" );

	for ( i = 0; i < P->nbands; i++ )
	    if ( P->bandinfo[i].selected )
		fprintf( fpo, " %f", P->bandinfo[i].offset );
	fprintf( fpo, " )\n" );
    }

	/*******************************************************************/

    /* finish up */

    fclose( fpo );
    return MRT_NO_ERROR;
}

