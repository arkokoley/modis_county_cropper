
/******************************************************************************

FILE:  tif_io.c

PURPOSE:  TIFF data writing routines

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  Rob Burrell            Original Development

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/
#include "geotiffio.h"
#include "xtiffio.h"

#include "cproj.h"
#include "datum.h"

#include "shared_resample.h"

/******************************************************************************

MODULE:  WriteRowGeoTIFF

PURPOSE:  Write a row of data to a TIFF image

RETURN VALUE:
Type = int
Value           Description
-----           -----------
TRUE			Success
FALSE			Failure

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  Rob Burrell            Original Development
         01/01  John Rishea            Standardized formatting

NOTES:

******************************************************************************/

int WriteRowGeoTIFF
(
    FileDescriptor *file,	/* I:  file to write */
    int row			/* I:  row number to write */
)

{
    return ( TIFFWriteScanline
	( ( ( GeoTIFFFD * ) file->fileptr )->tif, file->rowbuffer, row, 0 ) );
}
