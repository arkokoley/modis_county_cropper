#include "mosaic.h"

/******************************************************************************

MODULE:  CopyMosaicDescriptor

PURPOSE:  Copies a mosaic descriptor given a source desriptor

RETURN VALUE:
Type = none
Value           Description
-----           -----------

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/02  Gail Schmidt           Original Development

NOTES:

******************************************************************************/
void CopyMosaicDescriptor
(
    MosaicDescriptor *in,    /* I:  the source descriptor to be copied */
    char output_filename[],  /* I:  name of the output file */
    MosaicDescriptor *out    /* O:  the descriptor to be copied to */
)

{
    int i, j;                   /* loop indices */
    size_t k;                   /* loop indixes */

    out->filename = strdup( output_filename );
    out->horiz = in->horiz;
    out->vert = in->vert;
    out->filetype = in->filetype;
    out->nbands = in->nbands;

    out->bandinfo = ( BandType * ) calloc( in->nbands, sizeof( BandType ) );
    if ( out->bandinfo == NULL )
    {
        ErrorHandler( TRUE, "CopyMosaicDescriptor", ERROR_MEMORY,
            "Cannot allocate memory for band selection!" );
    }

    for (k = 0; k < in->nbands; k++ )
    {
        out->bandinfo[k].name = strdup( in->bandinfo[k].name );
        out->bandinfo[k].nlines = in->bandinfo[k].nlines;
        out->bandinfo[k].nsamples = in->bandinfo[k].nsamples;
        out->bandinfo[k].input_datatype = in->bandinfo[k].input_datatype;
        out->bandinfo[k].output_datatype = in->bandinfo[k].output_datatype;
        out->bandinfo[k].pixel_size = in->bandinfo[k].pixel_size;
        out->bandinfo[k].output_pixel_size = in->bandinfo[k].output_pixel_size;
        out->bandinfo[k].fieldnum = in->bandinfo[k].fieldnum;
        out->bandinfo[k].rank = in->bandinfo[k].rank;
        out->bandinfo[k].pos[0] = in->bandinfo[k].pos[0];
        out->bandinfo[k].pos[1] = in->bandinfo[k].pos[1];
        out->bandinfo[k].pos[2] = in->bandinfo[k].pos[2];
        out->bandinfo[k].pos[3] = in->bandinfo[k].pos[3];
        out->bandinfo[k].min_value = in->bandinfo[k].min_value;
        out->bandinfo[k].max_value = in->bandinfo[k].max_value;
        out->bandinfo[k].background_fill = in->bandinfo[k].background_fill;
        out->bandinfo[k].scale_factor = in->bandinfo[k].scale_factor;
        out->bandinfo[k].offset = in->bandinfo[k].offset;
        out->bandinfo[k].selected = in->bandinfo[k].selected;
        strcpy( out->gridname[k], in->gridname[k] );
    }

    out->projection_type = in->projection_type;
    out->datum_code = in->datum_code;
    out->zone_code = in->zone_code;

    out->projection_info = GetInputProjectionMosaic( in );
    for ( i = 0; i < 15; i++ )
    {
        out->projection_parameters[i] = in->projection_parameters[i];
    }

    for ( i = 0; i < 4; i++ )
    {
        for ( j = 0; j < 2; j++ )
        {
            out->ll_image_extent[i][j] = in->ll_image_extent[i][j];
            out->proj_image_extent[i][j] = in->proj_image_extent[i][j];
        }
    }

    out->north_bound = in->north_bound;
    out->south_bound = in->south_bound;
    out->east_bound = in->east_bound;
    out->west_bound = in->west_bound;

    return;
}

