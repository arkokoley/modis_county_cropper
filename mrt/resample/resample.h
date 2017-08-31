#ifndef _RESAMPLE_H_
#define _RESAMPLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shared_resample.h"

/* Local Prototypes */
int calc_isin_shift
(
    int lisin,                  /* I: input projection line coord */
    int sisin,                  /* I: input projection sample coord */
    ProjInfo *inproj,           /* I: input projection data for geolib */
    double upleft_x,            /* I: upper left input projection coord */
    double upleft_y,            /* I: upper left input projection coord */
    double input_pixel_size,    /* I: pixel size for input image */
    int output_datum_code,      /* I: output datum code */
    double *delta_s             /* O: shift for the ISIN shift calculation */
);

int Hdf2Hdr
(
    char *filename
);

int ResampleImage
(
    ModisDescriptor *modis      /* I:  session info */
);

double GetBIInterpValue
(
    double x,                /* I: input sample */
    double y,                /* I: input line */
    double background,       /* I: background fill */
    FileDescriptor *input,   /* I: input file data */
    int is_isin,             /* I: is this the ISIN projection */
    double *delta_s_start,   /* I: array of starting shifts */
    double *delta_s_slope    /* I: array of shifts slopes for shifts */
);

double *CreateWeightTable
(
    void
);

double GetCCInterpValue
(
    double x,                /* I: input sample */
    double y,                /* I: input line */
    double background,       /* I: background fill */
    FileDescriptor *input,   /* I: input file data */
    int is_isin,             /* I: is this the ISIN projection */
    double *delta_s_start,   /* I: array of starting shifts */
    double *delta_s_slope    /* I: array of shifts slopes for shifts */
);

int BIResample
(
    ModisDescriptor *modis,     /* I:  session info */
    FileDescriptor *input,      /* I:  input file info */
    FileDescriptor *output,     /* I:  output file info */
    int last_band               /* I: is this the last band to be processed */
);

int CCResample
(
    ModisDescriptor *modis,     /* I: session info */
    FileDescriptor *input,      /* I: input file info */
    FileDescriptor *output,     /* I: output file info */
    int last_band               /* I: is this the last band to be processed */
);

int NNResample
(
    ModisDescriptor *modis,     /* I:  session info */
    FileDescriptor *input,      /* I:  input file info */
    FileDescriptor *output,     /* I:  output file info */
    int last_band               /* I: is this the last band to be processed */
);

int NoResample
(
    ModisDescriptor *modis,     /* I:  session info */
    FileDescriptor *input,      /* I:  input file info */
    FileDescriptor *output      /* I:  output file info */
);

#endif /* _RESAMPLE_H_ */

