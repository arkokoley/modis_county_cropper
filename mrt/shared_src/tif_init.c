/******************************************************************************

FILE:  tif_init.c

PURPOSE:  Initialize TIFF output file

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  Rob Burrell            Original Development
         05/02  Gail Schmidt           Modified to handle signed vs. unsigned

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/
#include "worgen.h"
#include "geotiffio.h"
#include "xtiffio.h"
#include "hntdefs.h"		/* DFNT_... number types */
#include "shared_resample.h"

void SetGeoTIFFDatum
( 
    GeoTIFFFD *geotiff, 	/* I:  GeoTIFF descriptor */
    ProjInfo *outproj, 		/* I:  projection info */
    long sphere_code,           /* I:  sphere code to be used (only
                                       used for UTM) */
    char *citation		/* I/O:  add datum citation */
);

/******************************************************************************

MODULE:  SetTIFFTags

PURPOSE:  Set GeoTIFF tags and initialize TIFF output info

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status	        See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  Rob Burrell            Original Development
         01/01  John Rishea            Standardized formatting
         02/01  Rob Burrell            Output Units	
         06/01  Rob Burrell            Added TM projection, fixed some
                                       ASCII types that should've been SHORT
                                       Mods to Geographic
         10/01  Gail Schmidt           Made modifications to follow the
                                       Landsat-7 DFCB for GeoTiff products
         03/02  Gail Schmidt           Made mods as suggested by Ron Eastman
                                       with IDRISI. Geographic output cannot
                                       be user-defined. Only UL corner should
                                       be specified in the tiepoints.
         03/02  Gail Schmidt           Removed the Arc-Second definition for
                                       Geographic since output units are
                                       degrees
         12/02  Gail Schmidt           Added Albers Equal Area and Mercator
         06/04  Gail Schmidt           Modified the output corner point to
                                       be the center of the pixel rather than
                                       the extent of the pixel
         12/04  Gail Schmidt           Originally used PixelIsArea tag for
                                       the center of the pixel instead of
                                       PixelIsPoint. This has been changed.

NOTES:

******************************************************************************/
int SetTIFFTags
(
    FileDescriptor *output, 	/* I:  file info */
    ModisDescriptor *modis	/* I:  session info */
)

{
    GeoTIFFFD *geotiff;			/* geotiff file descriptor */

    double tiepoints[6];		/* corner tie points for projection */
    double pixelscale[3];		/* same as pixel size */
    char software[256];			/* string for software citation tag */
    char citation[256];			/* string for geo citation tag */
    ProjInfo *outproj = NULL;		/* projection data */

    int UTMWGS84_ZoneCodes[2][60] = { /* zone code for UTM WGS84 projections */
        {PCS_WGS84_UTM_zone_1N,
         PCS_WGS84_UTM_zone_2N,
         PCS_WGS84_UTM_zone_3N,
         PCS_WGS84_UTM_zone_4N,
         PCS_WGS84_UTM_zone_5N,
         PCS_WGS84_UTM_zone_6N,
         PCS_WGS84_UTM_zone_7N,
         PCS_WGS84_UTM_zone_8N,
         PCS_WGS84_UTM_zone_9N,
         PCS_WGS84_UTM_zone_10N,
         PCS_WGS84_UTM_zone_11N,
         PCS_WGS84_UTM_zone_12N,
         PCS_WGS84_UTM_zone_13N,
         PCS_WGS84_UTM_zone_14N,
         PCS_WGS84_UTM_zone_15N,
         PCS_WGS84_UTM_zone_16N,
         PCS_WGS84_UTM_zone_17N,
         PCS_WGS84_UTM_zone_18N,
         PCS_WGS84_UTM_zone_19N,
         PCS_WGS84_UTM_zone_20N,
         PCS_WGS84_UTM_zone_21N,
         PCS_WGS84_UTM_zone_22N,
         PCS_WGS84_UTM_zone_23N,
         PCS_WGS84_UTM_zone_24N,
         PCS_WGS84_UTM_zone_25N,
         PCS_WGS84_UTM_zone_26N,
         PCS_WGS84_UTM_zone_27N,
         PCS_WGS84_UTM_zone_28N,
         PCS_WGS84_UTM_zone_29N,
         PCS_WGS84_UTM_zone_30N,
         PCS_WGS84_UTM_zone_31N,
         PCS_WGS84_UTM_zone_32N,
         PCS_WGS84_UTM_zone_33N,
         PCS_WGS84_UTM_zone_34N,
         PCS_WGS84_UTM_zone_35N,
         PCS_WGS84_UTM_zone_36N,
         PCS_WGS84_UTM_zone_37N,
         PCS_WGS84_UTM_zone_38N,
         PCS_WGS84_UTM_zone_39N,
         PCS_WGS84_UTM_zone_40N,
         PCS_WGS84_UTM_zone_41N,
         PCS_WGS84_UTM_zone_42N,
         PCS_WGS84_UTM_zone_43N,
         PCS_WGS84_UTM_zone_44N,
         PCS_WGS84_UTM_zone_45N,
         PCS_WGS84_UTM_zone_46N,
         PCS_WGS84_UTM_zone_47N,
         PCS_WGS84_UTM_zone_48N,
         PCS_WGS84_UTM_zone_49N,
         PCS_WGS84_UTM_zone_50N,
         PCS_WGS84_UTM_zone_51N,
         PCS_WGS84_UTM_zone_52N,
         PCS_WGS84_UTM_zone_53N,
         PCS_WGS84_UTM_zone_54N,
         PCS_WGS84_UTM_zone_55N,
         PCS_WGS84_UTM_zone_56N,
         PCS_WGS84_UTM_zone_57N,
         PCS_WGS84_UTM_zone_58N,
         PCS_WGS84_UTM_zone_59N,
         PCS_WGS84_UTM_zone_60N},
        {PCS_WGS84_UTM_zone_1S,
         PCS_WGS84_UTM_zone_2S,
         PCS_WGS84_UTM_zone_3S,
         PCS_WGS84_UTM_zone_4S,
         PCS_WGS84_UTM_zone_5S,
         PCS_WGS84_UTM_zone_6S,
         PCS_WGS84_UTM_zone_7S,
         PCS_WGS84_UTM_zone_8S,
         PCS_WGS84_UTM_zone_9S,
         PCS_WGS84_UTM_zone_10S,
         PCS_WGS84_UTM_zone_11S,
         PCS_WGS84_UTM_zone_12S,
         PCS_WGS84_UTM_zone_13S,
         PCS_WGS84_UTM_zone_14S,
         PCS_WGS84_UTM_zone_15S,
         PCS_WGS84_UTM_zone_16S,
         PCS_WGS84_UTM_zone_17S,
         PCS_WGS84_UTM_zone_18S,
         PCS_WGS84_UTM_zone_19S,
         PCS_WGS84_UTM_zone_20S,
         PCS_WGS84_UTM_zone_21S,
         PCS_WGS84_UTM_zone_22S,
         PCS_WGS84_UTM_zone_23S,
         PCS_WGS84_UTM_zone_24S,
         PCS_WGS84_UTM_zone_25S,
         PCS_WGS84_UTM_zone_26S,
         PCS_WGS84_UTM_zone_27S,
         PCS_WGS84_UTM_zone_28S,
         PCS_WGS84_UTM_zone_29S,
         PCS_WGS84_UTM_zone_30S,
         PCS_WGS84_UTM_zone_31S,
         PCS_WGS84_UTM_zone_32S,
         PCS_WGS84_UTM_zone_33S,
         PCS_WGS84_UTM_zone_34S,
         PCS_WGS84_UTM_zone_35S,
         PCS_WGS84_UTM_zone_36S,
         PCS_WGS84_UTM_zone_37S,
         PCS_WGS84_UTM_zone_38S,
         PCS_WGS84_UTM_zone_39S,
         PCS_WGS84_UTM_zone_40S,
         PCS_WGS84_UTM_zone_41S,
         PCS_WGS84_UTM_zone_42S,
         PCS_WGS84_UTM_zone_43S,
         PCS_WGS84_UTM_zone_44S,
         PCS_WGS84_UTM_zone_45S,
         PCS_WGS84_UTM_zone_46S,
         PCS_WGS84_UTM_zone_47S,
         PCS_WGS84_UTM_zone_48S,
         PCS_WGS84_UTM_zone_49S,
         PCS_WGS84_UTM_zone_50S,
         PCS_WGS84_UTM_zone_51S,
         PCS_WGS84_UTM_zone_52S,
         PCS_WGS84_UTM_zone_53S,
         PCS_WGS84_UTM_zone_54S,
         PCS_WGS84_UTM_zone_55S,
         PCS_WGS84_UTM_zone_56S,
         PCS_WGS84_UTM_zone_57S,
         PCS_WGS84_UTM_zone_58S,
         PCS_WGS84_UTM_zone_59S,
         PCS_WGS84_UTM_zone_60S}
    };     

    int UTMWGS72_ZoneCodes[2][60] = { /* zone code for UTM WGS72 projections */
        {PCS_WGS72_UTM_zone_1N,
         PCS_WGS72_UTM_zone_2N,
         PCS_WGS72_UTM_zone_3N,
         PCS_WGS72_UTM_zone_4N,
         PCS_WGS72_UTM_zone_5N,
         PCS_WGS72_UTM_zone_6N,
         PCS_WGS72_UTM_zone_7N,
         PCS_WGS72_UTM_zone_8N,
         PCS_WGS72_UTM_zone_9N,
         PCS_WGS72_UTM_zone_10N,
         PCS_WGS72_UTM_zone_11N,
         PCS_WGS72_UTM_zone_12N,
         PCS_WGS72_UTM_zone_13N,
         PCS_WGS72_UTM_zone_14N,
         PCS_WGS72_UTM_zone_15N,
         PCS_WGS72_UTM_zone_16N,
         PCS_WGS72_UTM_zone_17N,
         PCS_WGS72_UTM_zone_18N,
         PCS_WGS72_UTM_zone_19N,
         PCS_WGS72_UTM_zone_20N,
         PCS_WGS72_UTM_zone_21N,
         PCS_WGS72_UTM_zone_22N,
         PCS_WGS72_UTM_zone_23N,
         PCS_WGS72_UTM_zone_24N,
         PCS_WGS72_UTM_zone_25N,
         PCS_WGS72_UTM_zone_26N,
         PCS_WGS72_UTM_zone_27N,
         PCS_WGS72_UTM_zone_28N,
         PCS_WGS72_UTM_zone_29N,
         PCS_WGS72_UTM_zone_30N,
         PCS_WGS72_UTM_zone_31N,
         PCS_WGS72_UTM_zone_32N,
         PCS_WGS72_UTM_zone_33N,
         PCS_WGS72_UTM_zone_34N,
         PCS_WGS72_UTM_zone_35N,
         PCS_WGS72_UTM_zone_36N,
         PCS_WGS72_UTM_zone_37N,
         PCS_WGS72_UTM_zone_38N,
         PCS_WGS72_UTM_zone_39N,
         PCS_WGS72_UTM_zone_40N,
         PCS_WGS72_UTM_zone_41N,
         PCS_WGS72_UTM_zone_42N,
         PCS_WGS72_UTM_zone_43N,
         PCS_WGS72_UTM_zone_44N,
         PCS_WGS72_UTM_zone_45N,
         PCS_WGS72_UTM_zone_46N,
         PCS_WGS72_UTM_zone_47N,
         PCS_WGS72_UTM_zone_48N,
         PCS_WGS72_UTM_zone_49N,
         PCS_WGS72_UTM_zone_50N,
         PCS_WGS72_UTM_zone_51N,
         PCS_WGS72_UTM_zone_52N,
         PCS_WGS72_UTM_zone_53N,
         PCS_WGS72_UTM_zone_54N,
         PCS_WGS72_UTM_zone_55N,
         PCS_WGS72_UTM_zone_56N,
         PCS_WGS72_UTM_zone_57N,
         PCS_WGS72_UTM_zone_58N,
         PCS_WGS72_UTM_zone_59N,
         PCS_WGS72_UTM_zone_60N},
        {PCS_WGS72_UTM_zone_1S,
         PCS_WGS72_UTM_zone_2S,
         PCS_WGS72_UTM_zone_3S,
         PCS_WGS72_UTM_zone_4S,
         PCS_WGS72_UTM_zone_5S,
         PCS_WGS72_UTM_zone_6S,
         PCS_WGS72_UTM_zone_7S,
         PCS_WGS72_UTM_zone_8S,
         PCS_WGS72_UTM_zone_9S,
         PCS_WGS72_UTM_zone_10S,
         PCS_WGS72_UTM_zone_11S,
         PCS_WGS72_UTM_zone_12S,
         PCS_WGS72_UTM_zone_13S,
         PCS_WGS72_UTM_zone_14S,
         PCS_WGS72_UTM_zone_15S,
         PCS_WGS72_UTM_zone_16S,
         PCS_WGS72_UTM_zone_17S,
         PCS_WGS72_UTM_zone_18S,
         PCS_WGS72_UTM_zone_19S,
         PCS_WGS72_UTM_zone_20S,
         PCS_WGS72_UTM_zone_21S,
         PCS_WGS72_UTM_zone_22S,
         PCS_WGS72_UTM_zone_23S,
         PCS_WGS72_UTM_zone_24S,
         PCS_WGS72_UTM_zone_25S,
         PCS_WGS72_UTM_zone_26S,
         PCS_WGS72_UTM_zone_27S,
         PCS_WGS72_UTM_zone_28S,
         PCS_WGS72_UTM_zone_29S,
         PCS_WGS72_UTM_zone_30S,
         PCS_WGS72_UTM_zone_31S,
         PCS_WGS72_UTM_zone_32S,
         PCS_WGS72_UTM_zone_33S,
         PCS_WGS72_UTM_zone_34S,
         PCS_WGS72_UTM_zone_35S,
         PCS_WGS72_UTM_zone_36S,
         PCS_WGS72_UTM_zone_37S,
         PCS_WGS72_UTM_zone_38S,
         PCS_WGS72_UTM_zone_39S,
         PCS_WGS72_UTM_zone_40S,
         PCS_WGS72_UTM_zone_41S,
         PCS_WGS72_UTM_zone_42S,
         PCS_WGS72_UTM_zone_43S,
         PCS_WGS72_UTM_zone_44S,
         PCS_WGS72_UTM_zone_45S,
         PCS_WGS72_UTM_zone_46S,
         PCS_WGS72_UTM_zone_47S,
         PCS_WGS72_UTM_zone_48S,
         PCS_WGS72_UTM_zone_49S,
         PCS_WGS72_UTM_zone_50S,
         PCS_WGS72_UTM_zone_51S,
         PCS_WGS72_UTM_zone_52S,
         PCS_WGS72_UTM_zone_53S,
         PCS_WGS72_UTM_zone_54S,
         PCS_WGS72_UTM_zone_55S,
         PCS_WGS72_UTM_zone_56S,
         PCS_WGS72_UTM_zone_57S,
         PCS_WGS72_UTM_zone_58S,
         PCS_WGS72_UTM_zone_59S,
         PCS_WGS72_UTM_zone_60S}
    };     

    int UTMNAD27_ZoneCodes[] =      /* zone code for UTM NAD27 projections */
        {0,
         0,
         PCS_NAD27_UTM_zone_3N,
         PCS_NAD27_UTM_zone_4N,
         PCS_NAD27_UTM_zone_5N,
         PCS_NAD27_UTM_zone_6N,
         PCS_NAD27_UTM_zone_7N,
         PCS_NAD27_UTM_zone_8N,
         PCS_NAD27_UTM_zone_9N,
         PCS_NAD27_UTM_zone_10N,
         PCS_NAD27_UTM_zone_11N,
         PCS_NAD27_UTM_zone_12N,
         PCS_NAD27_UTM_zone_13N,
         PCS_NAD27_UTM_zone_14N,
         PCS_NAD27_UTM_zone_15N,
         PCS_NAD27_UTM_zone_16N,
         PCS_NAD27_UTM_zone_17N,
         PCS_NAD27_UTM_zone_18N,
         PCS_NAD27_UTM_zone_19N,
         PCS_NAD27_UTM_zone_20N,
         PCS_NAD27_UTM_zone_21N,
         PCS_NAD27_UTM_zone_22N};

    int UTMNAD83_ZoneCodes[] =      /* zone code for UTM NAD83 projections */
        {0,
         0,
         PCS_NAD83_UTM_zone_3N,
         PCS_NAD83_UTM_zone_4N,
         PCS_NAD83_UTM_zone_5N,
         PCS_NAD83_UTM_zone_6N,
         PCS_NAD83_UTM_zone_7N,
         PCS_NAD83_UTM_zone_8N,
         PCS_NAD83_UTM_zone_9N,
         PCS_NAD83_UTM_zone_10N,
         PCS_NAD83_UTM_zone_11N,
         PCS_NAD83_UTM_zone_12N,
         PCS_NAD83_UTM_zone_13N,
         PCS_NAD83_UTM_zone_14N,
         PCS_NAD83_UTM_zone_15N,
         PCS_NAD83_UTM_zone_16N,
         PCS_NAD83_UTM_zone_17N,
         PCS_NAD83_UTM_zone_18N,
         PCS_NAD83_UTM_zone_19N,
         PCS_NAD83_UTM_zone_20N,
         PCS_NAD83_UTM_zone_21N,
         PCS_NAD83_UTM_zone_22N,
         PCS_NAD83_UTM_zone_23N};

    char NorS;                          /* north or south UTM zone */
    int Set;                            /* north or south UTM zone codes */
    int Zone;                           /* abs zone number */
    char GTCitationText[28];            /* UTM GTCitationText */
    int LinearValue = Linear_Meter;     /* Default value */

    /* get the projection and file pointers */
    strcpy( citation, "" );
    outproj = GetOutputProjection( modis );
    geotiff = ( GeoTIFFFD * ) output->fileptr;

    TIFFSetField( geotiff->tif, TIFFTAG_IMAGEWIDTH, output->ncols );
    TIFFSetField( geotiff->tif, TIFFTAG_IMAGELENGTH, output->nrows );
    TIFFSetField( geotiff->tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE );
    TIFFSetField( geotiff->tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK );
    TIFFSetField( geotiff->tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG );
    TIFFSetField( geotiff->tif, TIFFTAG_SAMPLESPERPIXEL, 1 );
    TIFFSetField( geotiff->tif, TIFFTAG_ROWSPERSTRIP, 1L );

    sprintf( software, "%s  %s", RESAMPLER_NAME, RESAMPLER_VERSION );
    TIFFSetField( geotiff->tif, TIFFTAG_SOFTWARE, software );

    switch ( output->datatype )
    {
        case DFNT_INT8:
          TIFFSetField( geotiff->tif, TIFFTAG_BITSPERSAMPLE, 8 );
          TIFFSetField( geotiff->tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_INT );
          break;
        case DFNT_UINT8:
          TIFFSetField( geotiff->tif, TIFFTAG_BITSPERSAMPLE, 8 );
          TIFFSetField( geotiff->tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT );
          break;
        case DFNT_INT16:
          TIFFSetField( geotiff->tif, TIFFTAG_BITSPERSAMPLE, 16 );
          TIFFSetField( geotiff->tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_INT );
          break;
        case DFNT_UINT16:
          TIFFSetField( geotiff->tif, TIFFTAG_BITSPERSAMPLE, 16 );
          TIFFSetField( geotiff->tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT );
          break;
        case DFNT_INT32:
          TIFFSetField( geotiff->tif, TIFFTAG_BITSPERSAMPLE, 32 );
          TIFFSetField( geotiff->tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_INT );
          break;
        case DFNT_UINT32:
          TIFFSetField( geotiff->tif, TIFFTAG_BITSPERSAMPLE, 32 );
          TIFFSetField( geotiff->tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT );
          break;
        case DFNT_FLOAT32:
          TIFFSetField( geotiff->tif, TIFFTAG_BITSPERSAMPLE, 32 );
          TIFFSetField( geotiff->tif, TIFFTAG_SAMPLEFORMAT,
              SAMPLEFORMAT_IEEEFP );
          break;
	case BAD_DATA_TYPE:
          ErrorHandler( TRUE, "SetTIFFTags", ERROR_GENERAL, "Bad Data Type" );
          break;
    }

    /* UL corner
       NOTE: according to the Geotiff documentation, only one tiepoint
       (the UL corner) is specified. */
    /* Since we are using RasterPixelIsPoint for the RasterTypeGeoKey, the
       UL corner point needs to be the center of the pixel */
    tiepoints[0] = 0.0;
    tiepoints[1] = 0.0;
    tiepoints[2] = 0.0;
    tiepoints[3] = output->coord_corners[UL][0] +
        0.5 * output->output_pixel_size;
    tiepoints[4] = output->coord_corners[UL][1] -
        0.5 * output->output_pixel_size;
    tiepoints[5] = 0.0;

    TIFFSetField( geotiff->tif, TIFFTAG_GEOTIEPOINTS, 6, tiepoints );

    if ( outproj->proj_code == GEO )
    {
        /* output pixel size in degrees */
        pixelscale[1] = pixelscale[0] = output->output_pixel_size;
        pixelscale[2] = 0.0;
    }
    else
    {
        /* output pixel size in meters */
        pixelscale[1] = pixelscale[0] = output->output_pixel_size;
        pixelscale[2] = 0.0;
    }

    TIFFSetField( geotiff->tif, TIFFTAG_GEOPIXELSCALE, 3, pixelscale );

    if( outproj->units != METER )
    {
            LinearValue = Linear_Foot;
    }

    switch ( outproj->proj_code )
    {
        case EQRECT:
                GTIFKeySet( geotiff->gtif, ProjCoordTransGeoKey, TYPE_SHORT, 1,
                        CT_Equirectangular );
                GTIFKeySet( geotiff->gtif, GTModelTypeGeoKey, TYPE_SHORT, 1,
                        ModelTypeProjected );
                GTIFKeySet( geotiff->gtif, GTRasterTypeGeoKey, TYPE_SHORT, 1,
                        RasterPixelIsPoint );
                strcpy( citation, "EQRECT     " );
                SetGeoTIFFDatum( geotiff, outproj, -1, citation );
                GTIFKeySet(geotiff->gtif, GTCitationGeoKey, TYPE_ASCII, 0,
                        citation);
                GTIFKeySet(geotiff->gtif, GeogLinearUnitsGeoKey, TYPE_SHORT,
                        1, LinearValue);
                GTIFKeySet(geotiff->gtif, GeogAngularUnitsGeoKey, TYPE_SHORT,
                        1, Angular_Degree);
                GTIFKeySet(geotiff->gtif, ProjectedCSTypeGeoKey, TYPE_SHORT,
                        1, KvUserDefined);
                GTIFKeySet(geotiff->gtif, ProjLinearUnitsGeoKey, TYPE_SHORT,
                        1, LinearValue);
                GTIFKeySet(geotiff->gtif, ProjNatOriginLongGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[4]);
                GTIFKeySet(geotiff->gtif, ProjNatOriginLatGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[5]);
                GTIFKeySet( geotiff->gtif, ProjFalseEastingGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[6] );
                GTIFKeySet( geotiff->gtif, ProjFalseNorthingGeoKey, TYPE_DOUBLE,                        1, modis->output_projection_parameters[7] );
                break;

	case GEO:
		GTIFKeySet( geotiff->gtif, GTModelTypeGeoKey, TYPE_SHORT, 1,
                        ModelTypeGeographic );
                GTIFKeySet( geotiff->gtif, GTRasterTypeGeoKey, TYPE_SHORT, 1,
	                RasterPixelIsPoint );
		GTIFKeySet( geotiff->gtif, GeogAngularUnitsGeoKey,
                        TYPE_SHORT, 1, Angular_Degree );
 		strcpy( citation, "Geographic (Longitude, Latitude) " );
		SetGeoTIFFDatum( geotiff, outproj, -1, citation );
                GTIFKeySet( geotiff->gtif, GTCitationGeoKey, TYPE_ASCII, 1,
                        citation );
		break;

	case ISINUS:
		GTIFKeySet( geotiff->gtif, GTModelTypeGeoKey, TYPE_SHORT, 1,
			ModelTypeProjected );
                GTIFKeySet( geotiff->gtif, GTRasterTypeGeoKey, TYPE_SHORT, 1,
	                RasterPixelIsPoint );
		GTIFKeySet( geotiff->gtif, ProjCoordTransGeoKey, TYPE_SHORT, 1,
			KvUserDefined );			
		GTIFKeySet( geotiff->gtif, ProjectionGeoKey, TYPE_SHORT, 1,
			KvUserDefined );
		GTIFKeySet( geotiff->gtif, ProjectedCSTypeGeoKey, TYPE_SHORT, 1,
			KvUserDefined );

		strcpy( citation, "Integerized Sinusoidal " );
		SetGeoTIFFDatum( geotiff, outproj, -1, citation );
		GTIFKeySet( geotiff->gtif, PCSCitationGeoKey, TYPE_ASCII, 1,
			citation );
						
		GTIFKeySet( geotiff->gtif, GeogLinearUnitsGeoKey,
                        TYPE_SHORT, 1, LinearValue );
                GTIFKeySet( geotiff->gtif, GeogAngularUnitsGeoKey,
                        TYPE_SHORT, 1, Angular_Degree );
                GTIFKeySet( geotiff->gtif, ProjCenterLongGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[4] );

	        /* Set the false easting and false northing
	           ---------------------------------------- */
	        GTIFKeySet( geotiff->gtif, ProjFalseEastingGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[6]);
                GTIFKeySet( geotiff->gtif, ProjFalseNorthingGeoKey,
                        TYPE_DOUBLE, 1, modis->output_projection_parameters[7]);
		break;

	case LAMAZ:
		GTIFKeySet( geotiff->gtif, ProjCoordTransGeoKey, TYPE_SHORT, 1,
			CT_LambertAzimEqualArea );
		GTIFKeySet( geotiff->gtif, GTModelTypeGeoKey, TYPE_SHORT, 1,
			ModelTypeProjected );
                GTIFKeySet( geotiff->gtif, GTRasterTypeGeoKey, TYPE_SHORT, 1,
	                RasterPixelIsPoint );
                strcpy( citation, "LAEA       " );
                SetGeoTIFFDatum( geotiff, outproj, -1, citation );
                GTIFKeySet(geotiff->gtif, GTCitationGeoKey, TYPE_ASCII, 0,
                        citation);
                GTIFKeySet(geotiff->gtif, GeogLinearUnitsGeoKey, TYPE_SHORT,
                        1, LinearValue);
                GTIFKeySet(geotiff->gtif, GeogAngularUnitsGeoKey, TYPE_SHORT,
                        1, Angular_Degree);
                GTIFKeySet(geotiff->gtif, ProjectedCSTypeGeoKey, TYPE_SHORT,
                        1, KvUserDefined);
                GTIFKeySet(geotiff->gtif, ProjLinearUnitsGeoKey, TYPE_SHORT,
                        1, LinearValue);
                GTIFKeySet( geotiff->gtif, ProjCenterLongGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[4] );
                GTIFKeySet( geotiff->gtif, ProjCenterLatGeoKey, TYPE_DOUBLE, 1,
                        modis->output_projection_parameters[5] );
                GTIFKeySet( geotiff->gtif, ProjFalseEastingGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[6] );
                GTIFKeySet( geotiff->gtif, ProjFalseNorthingGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[7] );
		break;

        case TM:
                GTIFKeySet(geotiff->gtif, ProjCoordTransGeoKey, TYPE_SHORT, 1,
                        CT_TransverseMercator);
                GTIFKeySet(geotiff->gtif, GTModelTypeGeoKey, TYPE_SHORT, 1,
                        ModelTypeProjected);
                GTIFKeySet(geotiff->gtif, GTRasterTypeGeoKey, TYPE_SHORT, 1,
                        RasterPixelIsPoint);
                strcpy( citation, "TM         " );
                SetGeoTIFFDatum( geotiff, outproj, -1, citation );
                GTIFKeySet(geotiff->gtif, GTCitationGeoKey, TYPE_ASCII, 0,
                        citation);
                GTIFKeySet(geotiff->gtif, GeogLinearUnitsGeoKey, TYPE_SHORT,
                        1, LinearValue);
                GTIFKeySet(geotiff->gtif, GeogAngularUnitsGeoKey, TYPE_SHORT,
                        1, Angular_Degree);
                GTIFKeySet(geotiff->gtif, ProjectedCSTypeGeoKey, TYPE_SHORT,
                        1, KvUserDefined);
                GTIFKeySet(geotiff->gtif, ProjectionGeoKey, TYPE_SHORT, 1,
                        KvUserDefined);
                GTIFKeySet(geotiff->gtif, ProjLinearUnitsGeoKey, TYPE_SHORT,
                        1, LinearValue);

                GTIFKeySet(geotiff->gtif, ProjCenterLongGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[4]);
                GTIFKeySet(geotiff->gtif, ProjNatOriginLatGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[5]);
                GTIFKeySet(geotiff->gtif, ProjFalseEastingGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[6]);
                GTIFKeySet(geotiff->gtif, ProjFalseNorthingGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[7]);
                GTIFKeySet(geotiff->gtif, ProjScaleAtNatOriginGeoKey,
                        TYPE_DOUBLE, 1, modis->output_projection_parameters[2]);
                break;

	case UTM:
                if (outproj->zone_code < 0)     /* South */
                {
                        NorS = 'S';
                        Set = 1;
                        Zone = abs (outproj->zone_code);
                }
                else
                {
                        NorS = 'N';
                        Set = 0;
                        Zone = outproj->zone_code;
                }

                if (outproj->datum_code == E_WGS84) /* WGS84 */
                {
                        (void) sprintf( GTCitationText,
                                "UTM Zone %d %c with WGS84", Zone, NorS);
                        Zone -= 1; /* zero base */

                        GTIFKeySet (geotiff->gtif, GTModelTypeGeoKey,
                                TYPE_SHORT, 1, ModelTypeProjected);
                        GTIFKeySet (geotiff->gtif, GTRasterTypeGeoKey,
                                TYPE_SHORT, 1, RasterPixelIsPoint);
                        GTIFKeySet (geotiff->gtif, GTCitationGeoKey,
                                TYPE_ASCII, 0, GTCitationText);
                        GTIFKeySet (geotiff->gtif, GeogLinearUnitsGeoKey,
                                TYPE_SHORT, 1, LinearValue);
                        GTIFKeySet (geotiff->gtif, GeogAngularUnitsGeoKey,
                                TYPE_SHORT, 1, Angular_Degree);
                        GTIFKeySet (geotiff->gtif, ProjectedCSTypeGeoKey,
                                TYPE_SHORT, 1, UTMWGS84_ZoneCodes[Set][Zone]);
                }
                else if (outproj->datum_code == E_WGS72) /* WGS72 */
                {
                        (void) sprintf( GTCitationText,
                                "UTM Zone %d %c with WGS72", Zone, NorS);
                        Zone -= 1; /* zero base */

                        GTIFKeySet (geotiff->gtif, GTModelTypeGeoKey,
                                TYPE_SHORT, 1, ModelTypeProjected);
                        GTIFKeySet (geotiff->gtif, GTRasterTypeGeoKey,
                                TYPE_SHORT, 1, RasterPixelIsPoint);
                        GTIFKeySet (geotiff->gtif, GTCitationGeoKey,
                                TYPE_ASCII, 0, GTCitationText);
                        GTIFKeySet (geotiff->gtif, GeogLinearUnitsGeoKey,
                                TYPE_SHORT, 1, LinearValue);
                        GTIFKeySet (geotiff->gtif, GeogAngularUnitsGeoKey,
                                TYPE_SHORT, 1, Angular_Degree);
                        GTIFKeySet (geotiff->gtif, ProjectedCSTypeGeoKey,
                                TYPE_SHORT, 1, UTMWGS72_ZoneCodes[Set][Zone]);
                }
                else if (outproj->datum_code == E_NAD27 &&
                        (Zone >= 3 && Zone <= 22) &&
                         NorS == 'N') /* NAD27 (only valid are 3N to 22N) */
                {
                        (void) sprintf( GTCitationText,
                                "UTM Zone %d %c with NAD27", Zone, NorS);
                        Zone -= 1; /* zero base */

                        GTIFKeySet (geotiff->gtif, GTModelTypeGeoKey,
                                TYPE_SHORT, 1, ModelTypeProjected);
                        GTIFKeySet (geotiff->gtif, GTRasterTypeGeoKey,
                                TYPE_SHORT, 1, RasterPixelIsPoint);
                        GTIFKeySet (geotiff->gtif, GTCitationGeoKey,
                                TYPE_ASCII, 0, GTCitationText);
                        GTIFKeySet (geotiff->gtif, GeogLinearUnitsGeoKey,
                                TYPE_SHORT, 1, LinearValue);
                        GTIFKeySet (geotiff->gtif, GeogAngularUnitsGeoKey,
                                TYPE_SHORT, 1, Angular_Degree);
                        GTIFKeySet (geotiff->gtif, ProjectedCSTypeGeoKey,
                                TYPE_SHORT, 1, UTMNAD27_ZoneCodes[Zone]);
                }
                else if (outproj->datum_code == E_NAD83 &&
                        (Zone >= 3 && Zone <= 23) &&
                         NorS == 'N') /* NAD83 (only valid are 3N to 23N) */
                {
                        (void) sprintf( GTCitationText,
                                "UTM Zone %d %c with NAD83", Zone, NorS);
                        Zone -= 1; /* zero base */

                        GTIFKeySet (geotiff->gtif, GTModelTypeGeoKey,
                                TYPE_SHORT, 1, ModelTypeProjected);
                        GTIFKeySet (geotiff->gtif, GTRasterTypeGeoKey,
                                TYPE_SHORT, 1, RasterPixelIsPoint);
                        GTIFKeySet (geotiff->gtif, GTCitationGeoKey,
                                TYPE_ASCII, 0, GTCitationText);
                        GTIFKeySet (geotiff->gtif, GeogLinearUnitsGeoKey,
                                TYPE_SHORT, 1, LinearValue);
                        GTIFKeySet (geotiff->gtif, GeogAngularUnitsGeoKey,
                                TYPE_SHORT, 1, Angular_Degree);
                        GTIFKeySet (geotiff->gtif, ProjectedCSTypeGeoKey,
                                TYPE_SHORT, 1, UTMNAD83_ZoneCodes[Zone]);
                }
                else  /* Datum/Zone combination is not currently supported */
                {
                        /* Set up the datum GeoKeys and determine the
                           datum being used.  If both the input and output
                           projections are UTM, then pass the input sphere
                           code to be used as the output sphere code. */
                        if (modis->input_projection_type == PROJ_UTM &&
                            modis->output_projection_type == PROJ_UTM)
                        {
                            SetGeoTIFFDatum( geotiff, outproj, 
                                modis->input_sphere_code, citation );
                        }
                        else
                        {
                            SetGeoTIFFDatum( geotiff, outproj, -1, citation );
                        }

                        /* Fill in the rest of the GeoKeys */
                        (void) sprintf( GTCitationText,
                                "UTM Zone %d %c with %s", Zone, NorS, citation);
                        GTIFKeySet (geotiff->gtif, GTModelTypeGeoKey,
                                TYPE_SHORT, 1, ModelTypeProjected);
                        GTIFKeySet (geotiff->gtif, GTRasterTypeGeoKey,
                                TYPE_SHORT, 1, RasterPixelIsPoint);
                        GTIFKeySet (geotiff->gtif, GTCitationGeoKey,
                                TYPE_ASCII, 0, GTCitationText);
                        GTIFKeySet (geotiff->gtif, GeogLinearUnitsGeoKey,
                                TYPE_SHORT, 1, LinearValue);
                        GTIFKeySet (geotiff->gtif, GeogAngularUnitsGeoKey,
                                TYPE_SHORT, 1, Angular_Degree);
                }

		break;
		
        case HAMMER:
		GTIFKeySet(geotiff->gtif, ProjCoordTransGeoKey, TYPE_SHORT, 1,
			32);	/* CT_Hammer */		
                GTIFKeySet(geotiff->gtif, GTModelTypeGeoKey, TYPE_SHORT, 1,
			ModelTypeProjected );
                GTIFKeySet(geotiff->gtif, GTRasterTypeGeoKey, TYPE_SHORT, 1,
	                RasterPixelIsPoint);
                strcpy( citation, "HAMMER     " );
                SetGeoTIFFDatum( geotiff, outproj, -1, citation );
                GTIFKeySet(geotiff->gtif, GTCitationGeoKey, TYPE_ASCII, 0,
                        citation);
                GTIFKeySet(geotiff->gtif, GeogLinearUnitsGeoKey, TYPE_SHORT, 1,
                        LinearValue);
                GTIFKeySet(geotiff->gtif, GeogAngularUnitsGeoKey, TYPE_SHORT,
                        1, Angular_Degree);
                GTIFKeySet(geotiff->gtif, ProjectedCSTypeGeoKey, TYPE_SHORT, 1,
                        KvUserDefined);
                GTIFKeySet(geotiff->gtif, ProjNatOriginLongGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[4]);
                GTIFKeySet(geotiff->gtif, ProjLinearUnitsGeoKey, TYPE_SHORT, 1,
                        LinearValue);
		GTIFKeySet(geotiff->gtif, ProjFalseEastingGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[6]);
		GTIFKeySet(geotiff->gtif, ProjFalseNorthingGeoKey,
                        TYPE_DOUBLE, 1, modis->output_projection_parameters[7]);
		break;
		
         case GOOD:
                GTIFKeySet(geotiff->gtif, ProjCoordTransGeoKey, TYPE_SHORT, 1,
                        29);    /* CT_InterrGoodesHomol */
                GTIFKeySet(geotiff->gtif, GTModelTypeGeoKey, TYPE_SHORT, 1,
                        ModelTypeProjected );
                GTIFKeySet(geotiff->gtif, GTRasterTypeGeoKey, TYPE_SHORT, 1,
                        RasterPixelIsPoint);
                strcpy( citation, "IGH        " );
                SetGeoTIFFDatum( geotiff, outproj, -1, citation );
                GTIFKeySet(geotiff->gtif, GTCitationGeoKey, TYPE_ASCII, 0,
                        citation);
                GTIFKeySet(geotiff->gtif, GeogLinearUnitsGeoKey, TYPE_SHORT, 1,
                        LinearValue);
                GTIFKeySet(geotiff->gtif, GeogAngularUnitsGeoKey, TYPE_SHORT,
                        1, Angular_Degree);
                GTIFKeySet(geotiff->gtif, ProjectedCSTypeGeoKey, TYPE_SHORT, 1,
                        KvUserDefined);
		break;
	
        case ALBERS:
                GTIFKeySet(geotiff->gtif, ProjCoordTransGeoKey, TYPE_SHORT, 1,
                        CT_AlbersEqualArea);
                GTIFKeySet(geotiff->gtif, GTModelTypeGeoKey, TYPE_SHORT, 1,
                        ModelTypeProjected);
                GTIFKeySet(geotiff->gtif, GTRasterTypeGeoKey, TYPE_SHORT, 1,
                        RasterPixelIsPoint);
                strcpy( citation, "AEA        " );
                SetGeoTIFFDatum( geotiff, outproj, -1, citation );
                GTIFKeySet(geotiff->gtif, GTCitationGeoKey, TYPE_ASCII, 0,
                        citation);
                GTIFKeySet(geotiff->gtif, GeogLinearUnitsGeoKey, TYPE_SHORT, 1,
                        LinearValue);
                GTIFKeySet(geotiff->gtif, GeogAngularUnitsGeoKey, TYPE_SHORT,
                        1, Angular_Degree);
                GTIFKeySet(geotiff->gtif, ProjectedCSTypeGeoKey, TYPE_SHORT, 1,
                        KvUserDefined);
                GTIFKeySet(geotiff->gtif, ProjectionGeoKey, TYPE_SHORT, 1,
                        KvUserDefined);
                GTIFKeySet(geotiff->gtif, ProjLinearUnitsGeoKey, TYPE_SHORT, 1,
                        LinearValue);
                GTIFKeySet(geotiff->gtif, ProjStdParallel1GeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[2]);
                GTIFKeySet(geotiff->gtif, ProjStdParallel2GeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[3]);
                GTIFKeySet(geotiff->gtif, ProjNatOriginLongGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[4]);
                GTIFKeySet(geotiff->gtif, ProjNatOriginLatGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[5]);
                GTIFKeySet(geotiff->gtif, ProjFalseEastingGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[6]);
                GTIFKeySet(geotiff->gtif, ProjFalseNorthingGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[7]);
                GTIFKeySet(geotiff->gtif, ProjFalseOriginLongGeoKey,
                        TYPE_DOUBLE, 1, (double)0.0);
                GTIFKeySet(geotiff->gtif, ProjFalseOriginLatGeoKey,
                        TYPE_DOUBLE, 1, (double)0.0);
                break;

	case LAMCC:
                GTIFKeySet(geotiff->gtif, ProjCoordTransGeoKey, TYPE_SHORT, 1,
                        CT_LambertConfConic_2SP);
                GTIFKeySet(geotiff->gtif, GTModelTypeGeoKey, TYPE_SHORT, 1,
                        ModelTypeProjected);
                GTIFKeySet(geotiff->gtif, GTRasterTypeGeoKey, TYPE_SHORT, 1,
                        RasterPixelIsPoint);
                strcpy( citation, "LCC        " );
                SetGeoTIFFDatum( geotiff, outproj, -1, citation );
                GTIFKeySet(geotiff->gtif, GTCitationGeoKey, TYPE_ASCII, 0,
                        citation);
                GTIFKeySet(geotiff->gtif, GeogLinearUnitsGeoKey, TYPE_SHORT, 1,
                        LinearValue);
                GTIFKeySet(geotiff->gtif, GeogAngularUnitsGeoKey, TYPE_SHORT,
                        1, Angular_Degree);
                GTIFKeySet(geotiff->gtif, ProjectedCSTypeGeoKey, TYPE_SHORT, 1,
                        KvUserDefined);
                GTIFKeySet(geotiff->gtif, ProjectionGeoKey, TYPE_SHORT, 1,
                        KvUserDefined);
                GTIFKeySet(geotiff->gtif, ProjLinearUnitsGeoKey, TYPE_SHORT, 1,
                        LinearValue);
                GTIFKeySet(geotiff->gtif, ProjStdParallel1GeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[2]);
                GTIFKeySet(geotiff->gtif, ProjStdParallel2GeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[3]);
                GTIFKeySet(geotiff->gtif, ProjNatOriginLongGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[4]);
                GTIFKeySet(geotiff->gtif, ProjNatOriginLatGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[5]);
                GTIFKeySet(geotiff->gtif, ProjFalseEastingGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[6]);
                GTIFKeySet(geotiff->gtif, ProjFalseNorthingGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[7]);
                GTIFKeySet(geotiff->gtif, ProjFalseOriginLongGeoKey,
                        TYPE_DOUBLE, 1, (double)0.0);
                GTIFKeySet(geotiff->gtif, ProjFalseOriginLatGeoKey,
                        TYPE_DOUBLE, 1, (double)0.0);
		break;
		
        case MOLL:
                GTIFKeySet(geotiff->gtif, ProjCoordTransGeoKey, TYPE_SHORT, 1,
                        30);    /* CT_Mollweide */
                GTIFKeySet(geotiff->gtif, GTModelTypeGeoKey, TYPE_SHORT, 1,
                        ModelTypeProjected );
                GTIFKeySet(geotiff->gtif, GTRasterTypeGeoKey, TYPE_SHORT, 1,
                        RasterPixelIsPoint);
                strcpy( citation, "MOLLWEIDE  " );
                SetGeoTIFFDatum( geotiff, outproj, -1, citation );
                GTIFKeySet(geotiff->gtif, GTCitationGeoKey, TYPE_ASCII, 0,
                        citation);
                GTIFKeySet(geotiff->gtif, GeogLinearUnitsGeoKey, TYPE_SHORT, 1,
                        LinearValue);
                GTIFKeySet(geotiff->gtif, GeogAngularUnitsGeoKey, TYPE_SHORT,
                        1, Angular_Degree);
                GTIFKeySet(geotiff->gtif, ProjectedCSTypeGeoKey, TYPE_SHORT, 1,
                        KvUserDefined);
                GTIFKeySet(geotiff->gtif, ProjNatOriginLongGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[4]);
                GTIFKeySet(geotiff->gtif, ProjLinearUnitsGeoKey, TYPE_SHORT, 1,
                        LinearValue);
                GTIFKeySet(geotiff->gtif, ProjFalseEastingGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[6]);
                GTIFKeySet(geotiff->gtif, ProjFalseNorthingGeoKey,
                        TYPE_DOUBLE, 1, modis->output_projection_parameters[7]);
		break;
		
        case MERCAT:
                GTIFKeySet(geotiff->gtif, ProjCoordTransGeoKey, TYPE_SHORT, 1,
                        CT_Mercator);
                GTIFKeySet(geotiff->gtif, GTModelTypeGeoKey, TYPE_SHORT, 1,
                        ModelTypeProjected);
                GTIFKeySet(geotiff->gtif, GTRasterTypeGeoKey, TYPE_SHORT, 1,
                        RasterPixelIsPoint);
                strcpy( citation, "MERCATOR   " );
                SetGeoTIFFDatum( geotiff, outproj, -1, citation );
                GTIFKeySet(geotiff->gtif, GTCitationGeoKey, TYPE_ASCII, 0,
                        citation);
                GTIFKeySet(geotiff->gtif, GeogLinearUnitsGeoKey, TYPE_SHORT,
                        1, LinearValue);
                GTIFKeySet(geotiff->gtif, GeogAngularUnitsGeoKey, TYPE_SHORT,
                        1, Angular_Degree);
                GTIFKeySet(geotiff->gtif, ProjectedCSTypeGeoKey, TYPE_SHORT,
                        1, KvUserDefined);
                GTIFKeySet(geotiff->gtif, ProjectionGeoKey, TYPE_SHORT, 1,
                        KvUserDefined);
                GTIFKeySet(geotiff->gtif, ProjLinearUnitsGeoKey, TYPE_SHORT, 1,
                        LinearValue);
                GTIFKeySet(geotiff->gtif, ProjNatOriginLongGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[4]);
                GTIFKeySet(geotiff->gtif, ProjNatOriginLatGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[5]);
                GTIFKeySet(geotiff->gtif, ProjFalseEastingGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[6]);
                GTIFKeySet(geotiff->gtif, ProjFalseNorthingGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[7]);
                break;

        case PS:
                GTIFKeySet(geotiff->gtif, ProjCoordTransGeoKey, TYPE_SHORT, 1,
                        CT_PolarStereographic);
                GTIFKeySet(geotiff->gtif, GTModelTypeGeoKey, TYPE_SHORT, 1,
                        ModelTypeProjected);
                GTIFKeySet(geotiff->gtif, GTRasterTypeGeoKey, TYPE_SHORT, 1,
                        RasterPixelIsPoint);
                strcpy( citation, "PS         " );
                SetGeoTIFFDatum( geotiff, outproj, -1, citation );
                GTIFKeySet(geotiff->gtif, GTCitationGeoKey, TYPE_ASCII, 0,
                        citation);
                GTIFKeySet(geotiff->gtif, GeogLinearUnitsGeoKey, TYPE_SHORT,
                        1, LinearValue);
                GTIFKeySet(geotiff->gtif, GeogAngularUnitsGeoKey, TYPE_SHORT,
                        1, Angular_Degree);
                GTIFKeySet(geotiff->gtif, ProjectedCSTypeGeoKey, TYPE_SHORT,
                        1, KvUserDefined);
                GTIFKeySet(geotiff->gtif, ProjectionGeoKey, TYPE_SHORT, 1,
                        KvUserDefined);
                GTIFKeySet(geotiff->gtif, ProjLinearUnitsGeoKey, TYPE_SHORT, 1,
                        LinearValue);
                GTIFKeySet(geotiff->gtif, ProjStraightVertPoleLongGeoKey,
                        TYPE_DOUBLE, 1, modis->output_projection_parameters[4]);
                GTIFKeySet(geotiff->gtif, ProjNatOriginLatGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[5]);
                GTIFKeySet(geotiff->gtif, ProjFalseEastingGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[6]);
                GTIFKeySet(geotiff->gtif, ProjFalseNorthingGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[7]);
		break;
		
        case SNSOID:
                GTIFKeySet(geotiff->gtif, ProjCoordTransGeoKey, TYPE_SHORT, 1,
                        CT_Sinusoidal);
                GTIFKeySet(geotiff->gtif, GTModelTypeGeoKey, TYPE_SHORT, 1,
                        ModelTypeProjected );
                GTIFKeySet(geotiff->gtif, GTRasterTypeGeoKey, TYPE_SHORT, 1,
                        RasterPixelIsPoint);
                strcpy( citation, "SINUSOIDAL " );
                SetGeoTIFFDatum( geotiff, outproj, -1, citation );
                GTIFKeySet(geotiff->gtif, GTCitationGeoKey, TYPE_ASCII, 0,
                        citation);
                GTIFKeySet(geotiff->gtif, GeogLinearUnitsGeoKey, TYPE_SHORT, 1,
                        LinearValue);
                GTIFKeySet(geotiff->gtif, GeogAngularUnitsGeoKey, TYPE_SHORT,
                        1, Angular_Degree);
                GTIFKeySet(geotiff->gtif, ProjectedCSTypeGeoKey, TYPE_SHORT, 1,
                        KvUserDefined);
                GTIFKeySet(geotiff->gtif, ProjNatOriginLongGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[4]);
                GTIFKeySet(geotiff->gtif, ProjLinearUnitsGeoKey, TYPE_SHORT, 1,
                        LinearValue);
                GTIFKeySet(geotiff->gtif, ProjFalseEastingGeoKey, TYPE_DOUBLE,
                        1, modis->output_projection_parameters[6]);
                GTIFKeySet(geotiff->gtif, ProjFalseNorthingGeoKey,
                        TYPE_DOUBLE, 1, modis->output_projection_parameters[7]);
		break;

	default:
		ErrorHandler( TRUE, "SetTIFFTags", ERROR_GENERAL,
			 "Bad Projection Type" );
		break;

    }

    DestroyProjectionInfo( outproj );

    return ( MRT_NO_ERROR );
}

/******************************************************************************

MODULE:  SetGeoTIFFDatum

PURPOSE:  Set GeoTIFF tags for the datum used

RETURN VALUE:
Type = none

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         10/00  Rob Burrell            Original Development
         01/01  John Rishea            Standardized formatting
         06/01  Rob Burrell            Removed support for datums that were
                                       not being used by the MRT
         01/07  Gail Schmidt           If the datum is not specified for
                                       UTM projection, then use the sphere
                                       code

NOTES:

******************************************************************************/
void SetGeoTIFFDatum
( 
    GeoTIFFFD *geotiff, 		/* I:  GeoTIFF descriptor */
    ProjInfo *outproj, 			/* I:  projection info */
    long sphere_code,                   /* I:  sphere code to be used (only
                                               used for UTM) */
    char *citation			/* I/O:  add datum citation */
)

{
    char tmpstr[256];
    long datum_code;
    long datum_valid;

    /* Use the specified datum code unless one was not specified.  If the
       datum code was not specified, then try to determine it with the
       semi-major and semi-minor axis. */
    datum_code = outproj->datum_code;
    if ( outproj->datum_code == E_NODATUM )
    {
        /* If the projection is UTM then use the sphere code (if it's defined),
           otherwise use the projection parameters to determine the
           ellipsoid/spheroid */
        if (outproj->proj_code == PROJ_UTM && sphere_code != -1) {
            datum_code = sphere_code;
        } else {
           if ( c_get_spheroid( outproj->proj_coef[0], outproj->proj_coef[1],
                &datum_code, &datum_valid ) != E_GEO_SUCC || !datum_valid ) {
                datum_code = E_NODATUM;
           }
        }

        /* Process as ellipsoids */
        switch( datum_code )
        {
            case 0:  /* Clarke 1866 */
    		strcat( citation, "Clarke 1866" );
		GTIFKeySet( geotiff->gtif, GeogEllipsoidGeoKey, TYPE_SHORT,
                            1, Ellipse_Clarke_1866 );
                GTIFKeySet( geotiff->gtif, GeogSemiMajorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[0] );
                GTIFKeySet( geotiff->gtif, GeogSemiMinorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[1] );
                break;

            case 1:  /* Clarke 1880 */
                strcat( citation, "Clarke 1880" );
                GTIFKeySet( geotiff->gtif, GeogEllipsoidGeoKey, TYPE_SHORT,
                            1, Ellipse_Clarke_1880 );
                GTIFKeySet( geotiff->gtif, GeogSemiMajorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[0] );
                GTIFKeySet( geotiff->gtif, GeogSemiMinorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[1] );
                break;

            case 3:  /* International 1967 */
                strcat( citation, "International 1967" );
                GTIFKeySet( geotiff->gtif, GeogEllipsoidGeoKey, TYPE_SHORT,
                            1, Ellipse_International_1967 );
                GTIFKeySet( geotiff->gtif, GeogSemiMajorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[0] );
                GTIFKeySet( geotiff->gtif, GeogSemiMinorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[1] );
                break;

            case 6:  /* Everest */
                strcat( citation, "Everest" );
                GTIFKeySet( geotiff->gtif, GeogEllipsoidGeoKey, TYPE_SHORT,
                            1, Ellipse_Everest_1830_Modified );
                GTIFKeySet( geotiff->gtif, GeogSemiMajorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[0] );
                GTIFKeySet( geotiff->gtif, GeogSemiMinorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[1] );
                break;

            case 8:  /* GRS 80 */
                strcat( citation, "GRS 1980" );
                GTIFKeySet( geotiff->gtif, GeogEllipsoidGeoKey, TYPE_SHORT,
                            1, Ellipse_GRS_1980 );
                GTIFKeySet( geotiff->gtif, GeogSemiMajorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[0] );
                GTIFKeySet( geotiff->gtif, GeogSemiMinorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[1] );
                break;

            case 9:  /* Airy */
                strcat( citation, "Airy" );
                GTIFKeySet( geotiff->gtif, GeogEllipsoidGeoKey, TYPE_SHORT,
                            1, Ellipse_Airy_1830 );
                GTIFKeySet( geotiff->gtif, GeogSemiMajorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[0] );
                GTIFKeySet( geotiff->gtif, GeogSemiMinorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[1] );
                break;

            case 11: /* Modified Airy */
                strcat( citation, "Modified Airy" );
                GTIFKeySet( geotiff->gtif, GeogEllipsoidGeoKey, TYPE_SHORT,
                            1, Ellipse_Airy_Modified_1849 );
                GTIFKeySet( geotiff->gtif, GeogSemiMajorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[0] );
                GTIFKeySet( geotiff->gtif, GeogSemiMinorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[1] );
                break;

            case 12: /* WGS 84 */
                strcat( citation, "WGS 1984" );
                GTIFKeySet( geotiff->gtif, GeogEllipsoidGeoKey, TYPE_SHORT,
                            1, Ellipse_WGS_84 );
                GTIFKeySet( geotiff->gtif, GeogSemiMajorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[0] );
                GTIFKeySet( geotiff->gtif, GeogSemiMinorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[1] );
                break;

            case 14: /* Australian National */
                strcat( citation, "Australian National" );
                GTIFKeySet( geotiff->gtif, GeogEllipsoidGeoKey, TYPE_SHORT,
                            1, Ellipse_Australian_National_Spheroid );
                GTIFKeySet( geotiff->gtif, GeogSemiMajorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[0] );
                GTIFKeySet( geotiff->gtif, GeogSemiMinorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[1] );
                break;

            case 15: /* Krassovsky */
                strcat( citation, "Krassovsky" );
                GTIFKeySet( geotiff->gtif, GeogEllipsoidGeoKey, TYPE_SHORT,
                            1, Ellipse_Krassowsky_1940 );
                GTIFKeySet( geotiff->gtif, GeogSemiMajorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[0] );
                GTIFKeySet( geotiff->gtif, GeogSemiMinorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[1] );
                break;

            case 19: /* Sphere 19 */
                strcat( citation, "Sphere 19" );
                GTIFKeySet( geotiff->gtif, GeogEllipsoidGeoKey, TYPE_SHORT,
                            1, Ellipse_Sphere );
                GTIFKeySet( geotiff->gtif, GeogSemiMajorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[0] );
                /* Since this is a sphere, use the radius for the semi-minor
                   value as well. */
                GTIFKeySet( geotiff->gtif, GeogSemiMinorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[0] );
                break;

            case 20: /* Bessel 1841 */
                strcat( citation, "Bessel 1841" );
                GTIFKeySet( geotiff->gtif, GeogEllipsoidGeoKey, TYPE_SHORT,
                            1, Ellipse_Bessel_1841 );
                GTIFKeySet( geotiff->gtif, GeogSemiMajorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[0] );
                GTIFKeySet( geotiff->gtif, GeogSemiMinorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[1] );
                break;

            case 27: /* Helmert */
                strcat( citation, "Helmert" );
                GTIFKeySet( geotiff->gtif, GeogEllipsoidGeoKey, TYPE_SHORT,
                            1, Ellipse_Helmert_1906 );
                GTIFKeySet( geotiff->gtif, GeogSemiMajorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[0] );
                GTIFKeySet( geotiff->gtif, GeogSemiMinorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[1] );
                break;

            case 28: /* Indonesian 1974 */
                strcat( citation, "Indonesian 1974" );
                GTIFKeySet( geotiff->gtif, GeogEllipsoidGeoKey, TYPE_SHORT,
                            1, Ellipse_Indonesian_National_Spheroid );
                GTIFKeySet( geotiff->gtif, GeogSemiMajorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[0] );
                GTIFKeySet( geotiff->gtif, GeogSemiMinorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[1] );
                break;

	    case 317: /* WGS 1984 Datum */
		strcat( citation, "WGS 1984");
                GTIFKeySet( geotiff->gtif, GeogEllipsoidGeoKey, TYPE_SHORT,
                            1, Ellipse_WGS_84 );
                GTIFKeySet( geotiff->gtif, GeogSemiMajorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[0] );
                GTIFKeySet( geotiff->gtif, GeogSemiMinorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[1] );
		break;

           default:
                /* Unsupported Datum/Ellipsoid */
                sprintf( tmpstr, "Unspecified Datum. Semi-major axis: %f, "
                    "Semi-minor axis: %f", outproj->proj_coef[0],
                    outproj->proj_coef[1] );
                strcat( citation, tmpstr );
                GTIFKeySet( geotiff->gtif, GeogGeodeticDatumGeoKey,
                    TYPE_SHORT, 1, KvUserDefined );
                GTIFKeySet( geotiff->gtif, GeographicTypeGeoKey,
                    TYPE_SHORT, 1, KvUserDefined );
                GTIFKeySet( geotiff->gtif, GeogSemiMajorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[0] );
                if ( outproj->proj_coef[1] != 0.0 )
                    GTIFKeySet( geotiff->gtif, GeogSemiMinorAxisGeoKey,
                        TYPE_DOUBLE, 1, outproj->proj_coef[1] );
                else
                    /* If this is a sphere, use the radius for the semi-minor
                       value as well. */
                    GTIFKeySet( geotiff->gtif, GeogSemiMinorAxisGeoKey,
                        TYPE_DOUBLE, 1, outproj->proj_coef[0] );
		break;
        }
    }
    else
    {
        /* Process as datums */
        switch( datum_code )
        {
	    case E_NAD27:
		strcat( citation, "North American Datum 1927" );
		GTIFKeySet( geotiff->gtif, GeogGeodeticDatumGeoKey, TYPE_SHORT,
                            1, Datum_North_American_Datum_1927);
                GTIFKeySet( geotiff->gtif, GeographicTypeGeoKey, TYPE_SHORT, 1,
                            GCS_NAD27 );
		break;

	    case E_NAD83:
		strcat( citation, "North American Datum 1983");
		GTIFKeySet( geotiff->gtif, GeogGeodeticDatumGeoKey, TYPE_SHORT,
                            1, Datum_North_American_Datum_1983 );
                GTIFKeySet( geotiff->gtif, GeographicTypeGeoKey, TYPE_SHORT, 1,
                            GCS_NAD83 );
		break;

            case E_WGS66:
                /* WGS66 is not supported by GeoTiff in the epsg_datum.inc
                   file */
                strcat( citation, "WGS 1966");
                GTIFKeySet( geotiff->gtif, GeogGeodeticDatumGeoKey,
                    TYPE_SHORT, 1, KvUserDefined );
                GTIFKeySet( geotiff->gtif, GeographicTypeGeoKey,
                    TYPE_SHORT, 1, KvUserDefined );
                GTIFKeySet( geotiff->gtif, GeogSemiMajorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[0] );
                GTIFKeySet( geotiff->gtif, GeogSemiMinorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[1] );
/*              GTIFKeySet( geotiff->gtif, GeogGeodeticDatumGeoKey, TYPE_SHORT,
                            1, Datum_WGS66 );
                GTIFKeySet( geotiff->gtif, GeographicTypeGeoKey, TYPE_SHORT, 1,
                            GCS_WGS_66 );
*/
                break;

            case E_WGS72:
                strcat( citation, "WGS 1972");
                GTIFKeySet( geotiff->gtif, GeogGeodeticDatumGeoKey, TYPE_SHORT,
                            1, Datum_WGS72 );
                GTIFKeySet( geotiff->gtif, GeographicTypeGeoKey, TYPE_SHORT, 1,
                            GCS_WGS_72 );
                break;

	    case E_WGS84:
		strcat( citation, "WGS 1984");
		GTIFKeySet( geotiff->gtif, GeogGeodeticDatumGeoKey, TYPE_SHORT,
                            1, Datum_WGS84 );
                GTIFKeySet( geotiff->gtif, GeographicTypeGeoKey, TYPE_SHORT, 1,
                            GCS_WGS_84 );
		break;

           default:
                /* Unsupported Datum/Ellipsoid */
                sprintf( tmpstr, "Unspecified Datum. Semi-major axis: %f, "
                    "Semi-minor axis: %f", outproj->proj_coef[0],
                    outproj->proj_coef[1] );
                strcat( citation, tmpstr );
                GTIFKeySet( geotiff->gtif, GeogGeodeticDatumGeoKey,
                    TYPE_SHORT, 1, KvUserDefined );
                GTIFKeySet( geotiff->gtif, GeographicTypeGeoKey,
                    TYPE_SHORT, 1, KvUserDefined );
                GTIFKeySet( geotiff->gtif, GeogSemiMajorAxisGeoKey,
                    TYPE_DOUBLE, 1, outproj->proj_coef[0] );
                if ( outproj->proj_coef[1] != 0.0 )
                    GTIFKeySet( geotiff->gtif, GeogSemiMinorAxisGeoKey,
                        TYPE_DOUBLE, 1, outproj->proj_coef[1] );
                else
                    /* If this is a sphere, use the radius for the semi-minor
                       value as well. */
                    GTIFKeySet( geotiff->gtif, GeogSemiMinorAxisGeoKey,
                        TYPE_DOUBLE, 1, outproj->proj_coef[0] );
		break;
        }
    }
}
