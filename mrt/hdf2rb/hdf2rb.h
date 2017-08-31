#ifndef _HDF2RB_H_
#define _HDF2RB_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shared_resample.h"
#include "loc_prot.h"

/* Local Prototypes */
int ConvertImageHDF2RB
(
    ModisDescriptor *modis      /* I:  session info */
);

int NoResampleHDF2RB
(
    ModisDescriptor *modis,     /* I: session info */
    FileDescriptor *input,      /* I: input file info */
    FileDescriptor *output      /* I: output file info */
);

#endif /* _HDF2RB_H_ */
