
/******************************************************************************

FILE:  print_md.c

PURPOSE:  Print the contents of a modis descriptor

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  John Weiss             Original Development

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/

#include "shared_mosaic.h"
#include "mrt_dtype.h"

/* static global char string arrays for printing */

static char *ProjectionStrings[] =
{
    "BAD", "AEA", "EQRECT", "GEO", "HAM", "IGH", "ISIN", "LA",
    "LCC", "MERCAT", "MOL", "PS", "SIN", "TM", "UTM"
};

static char *FileTypeStrings[] =
{
    "BAD", "RAW_BINARY", "HDF-EOS", "GEOTIFF"
};

static char *ResamplingTypeStrings[] =
{
    "BAD", "NN", "BI", "CC", "NONE"
};

static char *RawBinaryEndianness[] =
{
   "Not Applicable", "BIG_ENDIAN", "LITTLE_ENDIAN"
};

/******************************************************************************

MODULE:  PrintModisDescriptor

PURPOSE:  Print the contents of the modis descriptor for debug

RETURN VALUE:
Type = none
Value           Description
-----           -----------

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  John Weiss             Original Development
         07/00  Rob Burrell            Output to MessageHandler
         01/01  John Rishea            Standardized formatting
         06/01  Gail Schmidt           Don't print the parameter filename
                                       because it is always TmpParam.prm
                                       Added TM to the ProjectionStrings
                                       list
                                       Output the zone code and ellipsoid
                                       when using UTM for output
         12/01  Gail Schmidt           Datums are now being used for all the
                                       projections
         11/02  Gail Schmidt           Output datums can only be supplied for
                                       sphere based projections. The default
                                       is No Datum.

NOTES:

******************************************************************************/

void PrintModisDescriptor
(
    ModisDescriptor *P   /* I:  session info */
)

{
    int i;
    size_t nbi;
    char msgstr[LARGE_STRING],    /* LARGE_STRING found in shared_resample.h */
         tempstr[SMALL_STRING];   /* SMALL_STRING found in shared_resample.h */
    char datatype[8];

    /* MessageHandler( "PrintModisDescriptor", "ModisDescriptor values:\n" ); */
    MessageHandler( NULL, "" );
    MessageHandler( NULL, "Input image and reprojection info" );
    MessageHandler( NULL, "---------------------------------" );

    MessageHandler( NULL, "input_filename:          %s", P->input_filename );
    MessageHandler( NULL, "output_filename:         %s", P->output_filename );
    MessageHandler( NULL, "input_filetype:          %s",
        FileTypeStrings[P->input_filetype] );
    if( P->input_filetype == RAW_BINARY )
       MessageHandler( NULL, "input_file_endian:       %s",
           RawBinaryEndianness[P->input_file_endian] );
    MessageHandler( NULL, "output_filetype:         %s",
        FileTypeStrings[P->output_filetype] );
    if( P->output_filetype == RAW_BINARY )
       MessageHandler( NULL, "output_file_endian:      %s",
          RawBinaryEndianness[GetMachineEndianness()] );
    MessageHandler( NULL, "input_projection_type:   %s",
        ProjectionStrings[P->input_projection_type] );
    if ( P->input_projection_type == PROJ_UTM )
    {
        MessageHandler( NULL, "input_zone_code:         %d",
            P->input_zone_code );
    }
    switch ( P->input_datum_code )
    {
        case E_NAD27:
            MessageHandler( NULL,
                "input_datum:             NAD27 - Mean for CONUS");
            break;
        case E_NAD83:
            MessageHandler( NULL, "input_datum:             NAD83 - CONUS");
            break;
        case E_WGS66:
            MessageHandler( NULL, "input_datum:             WGS66");
            break;
        case E_WGS72:
            MessageHandler( NULL, "input_datum:             WGS72");
            break;
        case E_WGS84:
            MessageHandler( NULL, "input_datum:             WGS84");
            break;
        case E_NODATUM:
        default:
            if (P->input_projection_type != PROJ_UTM)
            {
                MessageHandler( NULL, "input_datum:             No Datum "
                    "(use projection parameters)");
            }
            else
            {
                MessageHandler( NULL, "input_datum:             No Datum "
                    "(use sphere code)");
                MessageHandler( NULL, "input_sphere:            %d",
                    P->input_sphere_code);
            }
            break;
    }

    MessageHandler( NULL, "output_projection_type:  %s",
        ProjectionStrings[P->output_projection_type] );
    if ( P->output_projection_type == PROJ_UTM )
    {
        MessageHandler( NULL, "output_zone_code:        %d",
            P->output_zone_code );
    }
    switch ( P->output_datum_code )
    {
        case E_NAD27:
            MessageHandler( NULL,
                "output_datum:            NAD27 - Mean for CONUS");
            break;
        case E_NAD83:
            MessageHandler( NULL, "output_datum:            NAD83 - CONUS");
            break;
        case E_WGS66:
            MessageHandler( NULL, "output_datum:            WGS66");
            break;
        case E_WGS72:
            MessageHandler( NULL, "output_datum:            WGS72");
            break;
        case E_WGS84:
            MessageHandler( NULL, "output_datum:            WGS84");
            break;
        case E_NODATUM:
        default:
            if (P->input_projection_type != PROJ_UTM ||
                P->output_projection_type != PROJ_UTM)
            {
                MessageHandler( NULL, "output_datum:            No Datum "
                    "(use projection parameters)");
            }
            else
            {
                MessageHandler( NULL, "output_datum:            No Datum "
                    "(use input sphere code)");
            }
            break;
    }
    MessageHandler( NULL, "resampling_type:         %s",
            ResamplingTypeStrings[P->resampling_type] );

    strcpy( msgstr, "input projection parameters:  " );
    for ( i = 0; i < 15; i++ )
    {
	sprintf( tempstr, "%.2f ", P->input_projection_parameters[i] );
	strcat( msgstr, tempstr );
    }
    MessageHandler( NULL, msgstr );

    strcpy( msgstr, "output projection parameters: " );
    for ( i = 0; i < 15; i++ )
    {
	sprintf( tempstr, "%.2f ", P->output_projection_parameters[i] );
	strcat( msgstr, tempstr );
    }
    MessageHandler( NULL, msgstr );

    if ( !P->use_bound )
        MessageHandler( NULL, "\ninput image corners (lat/lon):" );
    else
        MessageHandler( NULL, "\nbounded input image corners (lat/lon):" );
    MessageHandler( NULL, "    UL:  %.2f %.2f ",
        P->input_image_extent[UL][0], P->input_image_extent[UL][1] );
    MessageHandler( NULL, "    UR:  %.2f %.2f ",
        P->input_image_extent[UR][0], P->input_image_extent[UR][1] );
    MessageHandler( NULL, "    LL:  %.2f %.2f ",
        P->input_image_extent[LL][0], P->input_image_extent[LL][1] );
    MessageHandler( NULL, "    LR:  %.2f %.2f ",
        P->input_image_extent[LR][0], P->input_image_extent[LR][1] );

    MessageHandler( NULL, "\ninput image spatial subset corners (lat/lon):" );
    MessageHandler( NULL, "    UL:  %.2f %.2f ",
        P->ll_spac_sub_gring_corners[UL][0],
        P->ll_spac_sub_gring_corners[UL][1] );
    MessageHandler( NULL, "    UR:  %.2f %.2f ",
        P->ll_spac_sub_gring_corners[UR][0],
        P->ll_spac_sub_gring_corners[UR][1] );
    MessageHandler( NULL, "    LL:  %.2f %.2f ",
        P->ll_spac_sub_gring_corners[LL][0],
        P->ll_spac_sub_gring_corners[LL][1] );
    MessageHandler( NULL, "    LR:  %.2f %.2f ",
        P->ll_spac_sub_gring_corners[LR][0],
        P->ll_spac_sub_gring_corners[LR][1] );

    if ( P->nbands < 1 )
        MessageHandler( NULL, "\nnbands:  %i", P->nbands );
    else
    {
	MessageHandler( NULL,
         "\n     band          select   type lines smpls pixsiz      min    max   fill" );


	for ( nbi = 0; nbi < P->nbands; nbi++ )
	{
	    switch ( P->bandinfo[nbi].input_datatype )
	    {
		case DFNT_INT8:
		    strcpy( datatype, "INT8" );
		    break;
		case DFNT_UINT8:
		    strcpy( datatype, "UINT8" );
		    break;
		case DFNT_INT16:
		    strcpy( datatype, "INT16" );
		    break;
		case DFNT_UINT16:
		    strcpy( datatype, "UINT16" );
		    break;
		case DFNT_INT32:
		    strcpy( datatype, "INT32" );
		    break;
		case DFNT_UINT32:
		    strcpy( datatype, "UINT32" );
		    break;
		case DFNT_FLOAT32:
		    strcpy( datatype, "FLT32" );
		    break;
		default:
		    strcpy( datatype, "BAD" );
		    break;
	    }

	    MessageHandler( NULL,
                "%3" MRT_SIZE_T_PRFX
                ") %-16s %3i %6s %5i %5i %6.4f %6.0f %6.0f %6.0f",
                nbi + 1,
                P->bandinfo[nbi].name,
                P->bandinfo[nbi].selected,
                datatype,
                P->bandinfo[nbi].nlines,
                P->bandinfo[nbi].nsamples,
                P->bandinfo[nbi].pixel_size,
                P->bandinfo[nbi].min_value,
                P->bandinfo[nbi].max_value,
                P->bandinfo[nbi].background_fill );
	}
    }

    MessageHandler( NULL, "" );
}

/******************************************************************************

MODULE:  PrintModisDescriptorHDF2RB

PURPOSE:  Print the contents of the modis descriptor, as used for HDF2RB,
    for debugging and informational purposes.

RETURN VALUE:
Type = none
Value           Description
-----           -----------

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         11/05  Gail Schmidt           Original Development

NOTES:

******************************************************************************/

void PrintModisDescriptorHDF2RB
(
    ModisDescriptor *P   /* I:  session info */
)

{
    int i;
    size_t nbi;
    char msgstr[LARGE_STRING],    /* LARGE_STRING found in shared_resample.h */
         tempstr[SMALL_STRING];   /* SMALL_STRING found in shared_resample.h */
    char datatype[8];

    MessageHandler( NULL, "Input image and reprojection info" );
    MessageHandler( NULL, "---------------------------------" );

    MessageHandler( NULL, "input_filename:          %s", P->input_filename );
    MessageHandler( NULL, "output_filename:         %s", P->output_filename );
    MessageHandler( NULL, "input_filetype:          %s",
        FileTypeStrings[P->input_filetype] );
    if( P->input_filetype == RAW_BINARY )
       MessageHandler( NULL, "input_file_endian:       %s",
          RawBinaryEndianness[P->input_file_endian] );
    MessageHandler( NULL, "output_filetype:         %s",
        FileTypeStrings[P->output_filetype] );
    if( P->output_filetype == RAW_BINARY )
       MessageHandler( NULL, "output_file_endian:      %s",
          RawBinaryEndianness[GetMachineEndianness()] );
    MessageHandler( NULL, "input_projection_type:   %s",
        ProjectionStrings[P->input_projection_type] );
    if ( P->input_projection_type == PROJ_UTM )
    {
        MessageHandler( NULL, "input_zone_code:         %d",
            P->input_zone_code );
    }
    switch ( P->input_datum_code )
    {
        case E_NAD27:
            MessageHandler( NULL,
                "input_datum:             NAD27 - Mean for CONUS");
            break;
        case E_NAD83:
            MessageHandler( NULL, "input_datum:             NAD83 - CONUS");
            break;
        case E_WGS66:
            MessageHandler( NULL, "input_datum:             WGS66");
            break;
        case E_WGS72:
            MessageHandler( NULL, "input_datum:             WGS72");
            break;
        case E_WGS84:
            MessageHandler( NULL, "input_datum:             WGS84");
            break;
        case E_NODATUM:
        default:
            MessageHandler( NULL,
            "input_datum:             No Datum");
            break;
    }

    MessageHandler( NULL, "resampling_type:         %s",
            ResamplingTypeStrings[P->resampling_type] );

    strcpy( msgstr, "input projection parameters:  " );
    for ( i = 0; i < 15; i++ )
    {
	sprintf( tempstr, "%.2f ", P->input_projection_parameters[i] );
	strcat( msgstr, tempstr );
    }
    MessageHandler( NULL, msgstr );

    MessageHandler( NULL, "\ninput image corners (proj. x/y):" );
    MessageHandler( NULL, "    UL:  %.2f %.2f ",
        P->orig_coord_corners[UL][0], P->orig_coord_corners[UL][1] );
    MessageHandler( NULL, "    UR:  %.2f %.2f ",
        P->orig_coord_corners[UR][0], P->orig_coord_corners[UR][1] );
    MessageHandler( NULL, "    LL:  %.2f %.2f ",
        P->orig_coord_corners[LL][0], P->orig_coord_corners[LL][1] );
    MessageHandler( NULL, "    LR:  %.2f %.2f ",
        P->orig_coord_corners[LR][0], P->orig_coord_corners[LR][1] );

    if ( P->ll_spac_sub_gring_corners[UL][0] == -999.0 ||
         P->ll_spac_sub_gring_corners[UL][1] == -999.0 )
    {
        MessageHandler( NULL, "\ninput image spatial subset values "
            "(line/sample):    NONE" );
    }
    else
    {
        MessageHandler( NULL, "\ninput image spatial subset values:" );
        MessageHandler( NULL, "    UL line/sample:  %d %d ",
            (int) P->ll_spac_sub_gring_corners[UL][0],
            (int) P->ll_spac_sub_gring_corners[UL][1] );
        MessageHandler( NULL, "    LR line/sample:  %d %d ",
            (int) P->ll_spac_sub_gring_corners[LR][0],
            (int) P->ll_spac_sub_gring_corners[LR][1] );
    }

/*  Printed as part of output info after the format conversion -->
    MessageHandler( NULL, "\noutput image corners (proj. x/y):" );
    MessageHandler( NULL, "    UL:  %.2f %.2f ",
        P->proj_extents_corners[UL][0], P->proj_extents_corners[UL][1] );
    MessageHandler( NULL, "    UR:  %.2f %.2f ",
        P->proj_extents_corners[UR][0], P->proj_extents_corners[UR][1] );
    MessageHandler( NULL, "    LL:  %.2f %.2f ",
        P->proj_extents_corners[LL][0], P->proj_extents_corners[LL][1] );
    MessageHandler( NULL, "    LR:  %.2f %.2f ",
        P->proj_extents_corners[LR][0], P->proj_extents_corners[LR][1] );
*/

    if ( P->nbands < 1 )
        MessageHandler( NULL, "\nnbands:  %i", P->nbands );
    else
    {
	MessageHandler( NULL, "\n     band          select   type lines "
            "smpls pixsiz      min    max   fill" );


	for ( nbi = 0; nbi < P->nbands; nbi++ )
	{
	    switch ( P->bandinfo[nbi].input_datatype )
	    {
		case DFNT_INT8:
		    strcpy( datatype, "INT8" );
		    break;
		case DFNT_UINT8:
		    strcpy( datatype, "UINT8" );
		    break;
		case DFNT_INT16:
		    strcpy( datatype, "INT16" );
		    break;
		case DFNT_UINT16:
		    strcpy( datatype, "UINT16" );
		    break;
		case DFNT_INT32:
		    strcpy( datatype, "INT32" );
		    break;
		case DFNT_UINT32:
		    strcpy( datatype, "UINT32" );
		    break;
		case DFNT_FLOAT32:
		    strcpy( datatype, "FLT32" );
		    break;
		default:
		    strcpy( datatype, "BAD" );
		    break;
	    }

	    MessageHandler( NULL,
                "%3" MRT_SIZE_T_PRFX
                ") %-16s %3i %6s %5i %5i %6.4f %6.0f %6.0f %6.0f",
                nbi + 1,
                P->bandinfo[nbi].name,
                P->bandinfo[nbi].selected,
                datatype,
                P->bandinfo[nbi].nlines,
                P->bandinfo[nbi].nsamples,
                P->bandinfo[nbi].pixel_size,
                P->bandinfo[nbi].min_value,
                P->bandinfo[nbi].max_value,
                P->bandinfo[nbi].background_fill );
	}
    }

    MessageHandler( NULL, "" );
}

void PrintOutputFileInfo ( ModisDescriptor *P )
{
    size_t i, j;
    char datatype[8];

    MessageHandler( NULL, "\nOutput image info" );
    MessageHandler( NULL,   "-----------------" );

    /* output extents don't get filled in until after resampling */
    MessageHandler( NULL, "output image extents (lat/lon):" );
    MessageHandler( NULL, "    UL:  %.12f %.12f ",
        P->ll_extents_corners[UL][0], P->ll_extents_corners[UL][1] );
    MessageHandler( NULL, "    UR:  %.12f %.12f ",
        P->ll_extents_corners[UR][0], P->ll_extents_corners[UR][1] );
    MessageHandler( NULL, "    LL:  %.12f %.12f ",
        P->ll_extents_corners[LL][0], P->ll_extents_corners[LL][1] );
    MessageHandler( NULL, "    LR:  %.12f %.12f ",
        P->ll_extents_corners[LR][0], P->ll_extents_corners[LR][1] );

    MessageHandler( NULL, "\noutput image extents (X-Y projection units):" );
    MessageHandler( NULL, "    UL:  %.12f %.12f ",
        P->proj_extents_corners[UL][0], P->proj_extents_corners[UL][1] );
    MessageHandler( NULL, "    UR:  %.12f %.12f ",
        P->proj_extents_corners[UR][0], P->proj_extents_corners[UR][1] );
    MessageHandler( NULL, "    LL:  %.12f %.12f ",
        P->proj_extents_corners[LL][0], P->proj_extents_corners[LL][1] );
    MessageHandler( NULL, "    LR:  %.12f %.12f ",
        P->proj_extents_corners[LR][0], P->proj_extents_corners[LR][1] );

    MessageHandler( NULL,
    "\n     band               type lines smpls pixsiz      min    max   fill");

    for ( i = j = 0; i < P->nbands; i++ )
    {
	if ( P->bandinfo[i].selected )
        {
	    switch ( P->bandinfo[i].output_datatype )
	    {
		case DFNT_INT8:
		    strcpy( datatype, "INT8" );
		    break;
		case DFNT_UINT8:
		    strcpy( datatype, "UINT8" );
		    break;
		case DFNT_INT16:
		    strcpy( datatype, "INT16" );
		    break;
		case DFNT_UINT16:
		    strcpy( datatype, "UINT16" );
		    break;
		case DFNT_INT32:
		    strcpy( datatype, "INT32" );
		    break;
		case DFNT_UINT32:
		    strcpy( datatype, "UINT32" );
		    break;
		case DFNT_FLOAT32:
		    strcpy( datatype, "FLT32" );
		    break;
		default:
		    strcpy( datatype, "BAD" );
		    break;
	    }

	    MessageHandler( NULL,
                "%3" MRT_SIZE_T_PRFX
                ") %-16s %6s %5i %5i %6.4f %6.0f %6.0f %6.0f",
                j + 1,
                P->bandinfo[i].name,
                datatype,
                P->output_file_info[j].nlines,
                P->output_file_info[j].nsamples,
                P->output_file_info[j].pixel_size,
                P->bandinfo[i].min_value,
                P->bandinfo[i].max_value,
                P->bandinfo[i].background_fill );
            j++;
        }
    }

    MessageHandler( NULL, "" );
    return;
}

void PrintOutputFileInfoHDF2RB ( ModisDescriptor *P )
{
    size_t i, j;
    char datatype[8];

    MessageHandler( NULL, "\nOutput image info" );
    MessageHandler( NULL,   "-----------------" );

    MessageHandler( NULL, "output image corners (X-Y projection units):" );
    MessageHandler( NULL, "    UL:  %.12f %.12f ",
        P->proj_extents_corners[UL][0], P->proj_extents_corners[UL][1] );
    MessageHandler( NULL, "    UR:  %.12f %.12f ",
        P->proj_extents_corners[UR][0], P->proj_extents_corners[UR][1] );
    MessageHandler( NULL, "    LL:  %.12f %.12f ",
        P->proj_extents_corners[LL][0], P->proj_extents_corners[LL][1] );
    MessageHandler( NULL, "    LR:  %.12f %.12f ",
        P->proj_extents_corners[LR][0], P->proj_extents_corners[LR][1] );

    MessageHandler( NULL,
    "\n     band               type lines smpls pixsiz      min    max   fill");

    for ( i = j = 0; i < P->nbands; i++ )
    {
	if ( P->bandinfo[i].selected )
        {
	    switch ( P->bandinfo[i].output_datatype )
	    {
		case DFNT_INT8:
		    strcpy( datatype, "INT8" );
		    break;
		case DFNT_UINT8:
		    strcpy( datatype, "UINT8" );
		    break;
		case DFNT_INT16:
		    strcpy( datatype, "INT16" );
		    break;
		case DFNT_UINT16:
		    strcpy( datatype, "UINT16" );
		    break;
		case DFNT_INT32:
		    strcpy( datatype, "INT32" );
		    break;
		case DFNT_UINT32:
		    strcpy( datatype, "UINT32" );
		    break;
		case DFNT_FLOAT32:
		    strcpy( datatype, "FLT32" );
		    break;
		default:
		    strcpy( datatype, "BAD" );
		    break;
	    }

	    MessageHandler( NULL,
                "%3" MRT_SIZE_T_PRFX
                ") %-16s %6s %5i %5i %6.4f %6.0f %6.0f %6.0f",
                j + 1,
                P->bandinfo[i].name,
                datatype,
                P->output_file_info[j].nlines,
                P->output_file_info[j].nsamples,
                P->output_file_info[j].pixel_size,
                P->bandinfo[i].min_value,
                P->bandinfo[i].max_value,
                P->bandinfo[i].background_fill );
            j++;
        }
    }

    MessageHandler( NULL, "" );
    return;
}

void PrintOutputFileInfoMosaic ( MosaicDescriptor *mosaic )
{
    size_t i, j;
    char datatype[8];

    MessageHandler( NULL, "\nOutput mosaic image info" );
    MessageHandler( NULL,   "------------------------" );

    MessageHandler( NULL, "output image corners (lat/lon):" );
    MessageHandler( NULL, "    UL:  %.12f %.12f ",
        mosaic->ll_image_extent[UL][0], mosaic->ll_image_extent[UL][1] );
    MessageHandler( NULL, "    UR:  %.12f %.12f ",
        mosaic->ll_image_extent[UR][0], mosaic->ll_image_extent[UR][1] );
    MessageHandler( NULL, "    LL:  %.12f %.12f ",
        mosaic->ll_image_extent[LL][0], mosaic->ll_image_extent[LL][1] );
    MessageHandler( NULL, "    LR:  %.12f %.12f ",
        mosaic->ll_image_extent[LR][0], mosaic->ll_image_extent[LR][1] );

    MessageHandler( NULL, "\noutput image corners (X-Y projection units):" );
    MessageHandler( NULL, "    UL:  %.12f %.12f ",
        mosaic->proj_image_extent[UL][0], mosaic->proj_image_extent[UL][1] );
    MessageHandler( NULL, "    UR:  %.12f %.12f ",
        mosaic->proj_image_extent[UR][0], mosaic->proj_image_extent[UR][1] );
    MessageHandler( NULL, "    LL:  %.12f %.12f ",
        mosaic->proj_image_extent[LL][0], mosaic->proj_image_extent[LL][1] );
    MessageHandler( NULL, "    LR:  %.12f %.12f ",
        mosaic->proj_image_extent[LR][0], mosaic->proj_image_extent[LR][1] );

    MessageHandler( NULL,
    "\n     band               type lines smpls pixsiz      min    max   fill");

    for ( i = j = 0; i < mosaic->nbands; i++ )
    {
	if ( mosaic->bandinfo[i].selected )
        {
	    switch ( mosaic->bandinfo[i].input_datatype )
	    {
		case DFNT_INT8:
		    strcpy( datatype, "INT8" );
		    break;
		case DFNT_UINT8:
		    strcpy( datatype, "UINT8" );
		    break;
		case DFNT_INT16:
		    strcpy( datatype, "INT16" );
		    break;
		case DFNT_UINT16:
		    strcpy( datatype, "UINT16" );
		    break;
		case DFNT_INT32:
		    strcpy( datatype, "INT32" );
		    break;
		case DFNT_UINT32:
		    strcpy( datatype, "UINT32" );
		    break;
		case DFNT_FLOAT32:
		    strcpy( datatype, "FLT32" );
		    break;
		default:
		    strcpy( datatype, "BAD" );
		    break;
	    }

	    MessageHandler( NULL,
                "%3i) %-16s %6s %5i %5i %6.4f %6.0f %6.0f %6.0f",
                j + 1,
                mosaic->bandinfo[i].name,
                datatype,
                mosaic->bandinfo[i].nlines,
                mosaic->bandinfo[i].nsamples,
                mosaic->bandinfo[i].pixel_size,
                mosaic->bandinfo[i].min_value,
                mosaic->bandinfo[i].max_value,
                mosaic->bandinfo[i].background_fill );
            j++;
        }
    }

    MessageHandler( NULL, "" );
    return;
}

