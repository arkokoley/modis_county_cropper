#ifndef _MOSAIC_H_
#define _MOSAIC_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shared_mosaic.h"

/* Local Prototypes */
void CopyMosaicDescriptor
(
    MosaicDescriptor *in,    /* I:  the source descriptor to be copied */
    char output_filename[],  /* I:  name of the output file */
    MosaicDescriptor *out    /* O:  the descriptor to be copied to */
);

int OutputHdrMosaic
(
    MosaicDescriptor *mosaic,
    char *output_filename
);

#endif /* _MOSAIC_H_ */
