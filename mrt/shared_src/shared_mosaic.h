/******************************************************************************

FILE:  shared_mosaic.h

PURPOSE:  Type definitions and const values for mosaicking

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  Gail Schmidt

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
    __DJGPP__ is included for DOS

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/

#ifndef _SHARED_MOSAIC_H_
#define _SHARED_MOSAIC_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shared_resample.h"

#define MOSAIC_NAME    "MODIS Mosaic Tool"
#define MOSAIC_VERSION "v4.1 March 2009"

/* number of ISIN tiles (36 horizontal and 18 vertical tiles) */
#define MAX_HORIZ_TILES 35
#define MAX_VERT_TILES 17
#define NUM_HORIZ_TILES 36
#define NUM_VERT_TILES 18
#define NUM_ISIN_TILES 648

/* ERROR/SUCCESS defines */
#define MOSAIC_ERROR 0
#define MOSAIC_SUCCESS 1

/* maximum number of bands in any one image is less than 100 */
#define MAXBANDS 100 

/* MosaicDescriptor struct */
typedef struct
{
    /* image filename */
    char *filename;

    /* array of grid (Vgroup) names */
    char gridname[MAXBANDS][LARGE_STRING];

    /* ISIN tile information */
    int horiz;
    int vert;

    /* file types (raw binary, HDF-EOS, GeoTiff) */
    FileType filetype;

    /* if raw binary, what endianness did the header say the data was in */
    MrtEndianness input_file_endian;

    /* number of bands in image */
    size_t nbands;

    /* array[nbands] of info about each band */
    BandType *bandinfo;

    /* coordinate system origin: UL, UR, LL, LR */
    CornerType coord_origin;

    /* lat/long image corners (UL, UR, LL, LR lat/long)
       Note: only used for reading raw binary files. */
    double ll_image_extent[4][2];

    /* projection corner coordinates (UL, UR, LL, LR x/y) */
    double proj_image_extent[4][2];

    /* input bounding rectangle in degrees (input space) */
    double north_bound;
    double south_bound;
    double east_bound;
    double west_bound;

    /* projection types (ISIN, GEO, UTM, etc.) */
    ProjectionType projection_type;

    /* array of 15 projection parameters */
    double projection_parameters[15];

    /* datum codes (NAD27, NAD83, WGS66, WGS72, WGS84, NODATUM) */
    int datum_code;
    int input_sphere_code;  /* required for UTM projections since the
                               projection parameters can't store the
                               sphere/ellipse information */
    /* UTM zone codes */
    int zone_code;

    /* projection info structures for Geolib */
    ProjInfo *projection_info;
}
MosaicDescriptor;

/* Mosaic Prototypes */
#include "loc_prot_mosaic.h"

#endif /* _SHARED_MOSAIC_H_ */
