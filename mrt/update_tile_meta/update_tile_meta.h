#ifndef _UPDATE_TILE_META_H_
#define _UPDATE_TILE_META_H_

#define MRT_NO_ERROR             0
#define ERROR_MEMORY            -4
#define ERROR_GENERAL           -13
#define ERROR_OPEN_IMAGE_READ   -14
#define ERROR_READ_IMAGE        -15
#define ERROR_OPEN_IMAGE_WRITE  -16
#define ERROR_WRITE_IMAGE       -17

#define MAX_HORIZ_TILES 35
#define MAX_VERT_TILES 17

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mfhdf.h"

#ifdef WIN32
#define strdup _strdup
#endif

/* Local Prototypes */
char *ReadMeta
(
    int32 sd_id                 /* I: SDS id # for the HDF file */
);

char *ReadCoreMetadata
(
    int32 sd_id,                /* I: SDS id # for the HDF file */ 
    char *attr                  /* I: Name of the attribute to read */
);

intn UpdateTiles
(
    char *core_data,            /* I/O: CoreMetadata string */
    char *htile,                /* I: Horizontal tile value */
    char *vtile                 /* I: Vertical tile value */
);

char *CreateMeta
(
    char *htile,                /* I: Horizontal tile value */
    char *vtile                 /* I: Vertical tile value */
);

intn AppendMeta
(
    int32 sd_id,                /* I: SDS id # for the HDF file */
    char *core_data             /* I: CoreMetadata string */
);

void Usage (void);

#endif
