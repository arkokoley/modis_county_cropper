#include "shared_mosaic.h"

/******************************************************************************

MODULE:  InitializeModisDescriptor

PURPOSE:  Initialize a modis descriptor with default values

RETURN VALUE:
Type = none
Value           Description
-----           -----------

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  John Weiss             Original Development
         06/00  John Weiss             Initializations for codes and other 
                                       projection info
         10/00  Rob Burrell            Mods to handle image extents
         01/01  John Rishea            Standardized formatting
         11/01  Gail Schmidt           Initialize the bounding rectangle
                                       coordinates
         01/02  Gail Schmidt           Initialize the switches to false
         01/07  Gail Schmidt           Initialize the input sphere code

NOTES:

******************************************************************************/
void InitializeModisDescriptor
(
    ModisDescriptor *P	/* I/O:  the descriptor to initialize */
)

{
    int i, j;			/* loop indices */
    int iparam;

    P->parameter_filename = NULL;
    P->input_filename = NULL;
    P->output_filename = NULL;
    P->input_filetype = BAD_FILE_TYPE;
    P->input_file_endian = MRT_UNKNOWN_ENDIAN;
    P->output_filetype = BAD_FILE_TYPE;
    P->nbands = 0;
    P->bandinfo = NULL;
    P->input_projection_type = BAD_PROJECTION_TYPE;
    P->output_projection_type = BAD_PROJECTION_TYPE;
    P->resampling_type = BAD_RESAMPLING_TYPE;
    P->spatial_subset_type = BAD_SPATIAL_SUBSET_TYPE;
    P->is_subset = TRUE;
    P->input_datum_code = E_NODATUM;
    P->output_datum_code = E_NODATUM;
    P->input_sphere_code = -1;
    P->input_zone_code = 0;
    P->output_zone_code = 0;
    P->in_projection_info = NULL;
    P->out_projection_info = NULL;
    P->output_file_info = NULL;
    P->output_units = -1;		/* valid values are >= 0 */
    P->nfiles_out = 0;

    for ( i = 0; i < 15; i++ )
    {
	P->input_projection_parameters[i] = 0.0;
	P->output_projection_parameters[i] = 0.0;
    }

    for ( i = 0; i < 4; i++ )
    {
	for ( j = 0; j < 2; j++ )
	{
            P->input_image_extent[i][j] = 0.0;
            P->orig_coord_corners[i][j] = 0.0;
            P->ll_spac_sub_gring_corners[i][j] = 0.0;
            P->proj_spac_sub_gring_corners[i][j] = 0.0;
            P->input_corner_points[i][j] = 0;
            P->ll_extents_corners[i][j] = 0.0;
            P->proj_extents_corners[i][j] = 0.0;
	}
    }

    P->north_bound = 0.0;
    P->south_bound = 0.0;
    P->east_bound = 0.0;
    P->west_bound = 0.0;
    P->use_bound = FALSE;

    P->aswitch = FALSE;
    P->iswitch = FALSE;
    P->jswitch = FALSE;
    P->lswitch = FALSE;
    P->oswitch = FALSE;
    P->rswitch = FALSE;
    P->sswitch = FALSE;
    P->tswitch = FALSE;
    P->uswitch = FALSE;
    P->xswitch = FALSE;

    for ( iparam = 0; iparam < NSTRINGS; iparam++ )
        P->ParamsPresent[iparam] = 0;

    P->tmpspectralsubset = NULL;
}


/******************************************************************************

MODULE:  InitializeMosaicDescriptor

PURPOSE:  Initialize a mosaic descriptor with default values

RETURN VALUE:
Type = none
Value           Description
-----           -----------

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         03/02  Gail Schmidt           Original Development

NOTES:

******************************************************************************/
void InitializeMosaicDescriptor
(
    MosaicDescriptor *P    /* I/O:  the descriptor to initialize */
)

{
    int i, j;                   /* loop indices */

    P->filename = NULL;
    P->horiz = -9;
    P->vert = -9;
    P->filetype = BAD_FILE_TYPE;
    P->input_file_endian = MRT_UNKNOWN_ENDIAN;
    P->nbands = 0;
    P->bandinfo = NULL;
    P->coord_origin = UL;
    P->projection_type = BAD_PROJECTION_TYPE;
    P->datum_code = E_NODATUM;
    P->zone_code = 0;
    P->projection_info = NULL;

    for ( i = 0; i < 15; i++ )
    {
        P->projection_parameters[i] = 0.0;
    }

    for ( i = 0; i < 4; i++ )
    {
        for ( j = 0; j < 2; j++ )
        {
            P->ll_image_extent[i][j] = 0.0;
            P->proj_image_extent[i][j] = 0.0;
        }
    }

    P->north_bound = 0.0;
    P->south_bound = 0.0;
    P->east_bound = 0.0;
    P->west_bound = 0.0;
}

