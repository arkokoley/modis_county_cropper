#ifndef _APPEND_META_H_
#define _APPEND_META_H_

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
#include <mfhdf.h>

/* Local Prototypes */
intn AppendMeta
(
    int32 sd_id,                /* I: SDS id # for the HDF file */
    char *nboundcoord,          /* I: North bounding coord */
    char *sboundcoord,          /* I: South bounding coord */
    char *eboundcoord,          /* I: East bounding coord */
    char *wboundcoord,          /* I: West bounding coord */
    char *htile,                /* I: Horizontal tile value */
    char *vtile                 /* I: Vertical tile value */
);

intn ReadMeta
(
    int32 sd_id,                /* I: SDS id # for the HDF file */
    char *nboundcoord,          /* O: North bounding coord */
    char *sboundcoord,          /* O: South bounding coord */
    char *eboundcoord,          /* O: East bounding coord */
    char *wboundcoord,          /* O: West bounding coord */
    int *all_coords_present,    /* O: Were all bounding coordinates present? */
    char *htile,                /* O: Horizontal tile value */
    char *vtile                 /* O: Vertical tile value */
);

intn ReadBoundCoords
(
    int32 sd_id,                /* I: SDS id # for the HDF file */
    char *attr,                 /* I: Name of the attribute to read */
    char *nboundcoord,          /* O: North bounding coord */
    char *sboundcoord,          /* O: South bounding coord */
    char *eboundcoord,          /* O: East bounding coord */
    char *wboundcoord,          /* O: West bounding coord */
    int *all_coords_present     /* O: Were all bounding coordinates present? */
);

intn ReadTile
(
    int32 sd_id,                /* I: SDS id # for the HDF file */
    char *attr,                 /* I: Name of the attribute to read */
    char *htile,                /* O: Horizontal tile value */
    char *vtile                 /* O: Vertical tile value */
);

void Usage (void);

#endif
