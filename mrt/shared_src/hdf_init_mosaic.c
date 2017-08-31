
/******************************************************************************

FILE:  hdf_init_mosaic.c

PURPOSE:  Initialize an HDF output file for mosaicking

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  John Weiss             Original Development

HARDWARE AND/OR SOFTWARE LIMITATIONS: 
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/

#include "shared_mosaic.h"

/******************************************************************************

MODULE:  CreateHdfEosGridMosaic

PURPOSE:  Create a grid in an HDF output file for mosaicking

RETURN VALUE:
Type = int
Value           Description
-----           -----------
-1              Failure
0               Success

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         07/02  Gail Schmidt           Original Development
         11/02  Gail Schmidt           HDF-EOS documentation specifies that
                                       Geographic corner points should be
                                       output as DMS. The MRT processes these
                                       corner points as decimal degrees, so
                                       they must be converted to DMS before
                                       outputting them to HDF-EOS.

NOTES:

******************************************************************************/

int CreateHdfEosGridMosaic
(
    MosaicDescriptor *mosaic,   /* I: session info */
    int curr_band,              /* I: current output band to be processed */
    FileDescriptor *output 	/* O: output file descriptor */
)

{
    double temp;                    /* temp corner value for deg to dms */
    HdfEosFD *hdfptr = NULL;	    /* HDF file descriptor */
    float64 upleft[2], lowright[2]; /* upperleft and lwright corner coords */
    double proj_coef[15];           /* 15 projection parameters */
    int32 xdimsize, ydimsize;	    /* pixel dimensions */
    int status;			    /* error status */
    int i;                          /* looping variable */
    char gridname[LARGE_STRING],    /* strings for names */
         tempname[LARGE_STRING],    /* LARGE_STRING defined in shared_resample.h */
         *tmpptr = NULL,            /* temporary basename ptr */
         *ext = NULL;               /* pointer to the file extension */
    ProjInfo *outproj = NULL;	    /* projection info structure */

    hdfptr = ( HdfEosFD * ) output->fileptr;

    /* copy info from the mosaic descriptor */
    strcpy( output->filename, mosaic->filename );
    output->fileopentype = FILE_WRITE_MODE;
    output->filetype = mosaic->filetype;
    output->datatype = mosaic->bandinfo[curr_band].input_datatype;
    output->datasize = DFKNTsize( output->datatype );

    output->nrows = mosaic->bandinfo[curr_band].nlines;
    output->ncols = mosaic->bandinfo[curr_band].nsamples;
    output->pixel_size = mosaic->bandinfo[curr_band].pixel_size;
    output->output_pixel_size = mosaic->bandinfo[curr_band].pixel_size;
    output->background_fill = mosaic->bandinfo[curr_band].background_fill;

    /* set the projection coordinate corners for the output file to
       be those of the mosaic file */
    for ( i = 0; i < 4; i++ )
    {
        output->coord_corners[i][0] = mosaic->proj_image_extent[i][0];
        output->coord_corners[i][1] = mosaic->proj_image_extent[i][1];
    }

    /* reallocate based on window */
    if ( output->rowbuffer )
        free ( output->rowbuffer );

    /* new buffer, things may have changed */
    output->rowbuffer = ( void * ) calloc ( output->ncols,
        output->datasize );
    if ( !output->rowbuffer )
        ErrorHandler( TRUE, "CreateHdfEosGridMosaic", ERROR_MEMORY,
            "Error allocating memory for the output Row Buffer" );

    xdimsize = output->ncols;
    ydimsize = output->nrows;

    /* set real corners.  geographic corners will be output in degrees and
       all other projection corners will be output in meters. */
    upleft[0] = output->coord_corners[UL][0];
    upleft[1] = output->coord_corners[UL][1];
    lowright[0] = output->coord_corners[LR][0];
    lowright[1] = output->coord_corners[LR][1];

    /* if this data is being output as Geographic, then the upper left
       and lower right corners should be output in DMS (according to the
       HDF-EOS documentation). the MRT software processes the Geographic
       corners in decimal degrees. convert the corner points to DMS for
       output to the HDF-EOS file. */
    if ( mosaic->projection_type == PROJ_GEO )
    {
        c_degdms( &upleft[0], &temp, "DEG", "LON" );
        upleft[0] = temp;
        c_degdms( &upleft[1], &temp, "DEG", "LAT" );
        upleft[1] = temp;
        c_degdms( &lowright[0], &temp, "DEG", "LON" );
        lowright[0] = temp;
        c_degdms( &lowright[1], &temp, "DEG", "LAT" );
        lowright[1] = temp;
    }

    /* if file type is HDF-EOS, then use the grid name from the original
       HDF-EOS file */
    if ( mosaic->filetype == HDFEOS )
    {
        strcpy( gridname, mosaic->gridname[curr_band] );
    }

    /* otherwise create a new grid name */
    else
    {
        /* give new grid same name as file, plus pixel size */
        strcpy( tempname, output->filename );
        ext = strrchr( tempname, '.' );
        *ext = '\0';		/* strip .hdf extension */

        /* find the end of the path since some pathnames may have a
           '.' in them. check Unix/Linux then Windows. */
        tmpptr = strrchr( tempname, '/' );
        if ( tmpptr == NULL )
        {   /* check Windows */
            tmpptr = strrchr( tempname, '\\' );
            if ( tmpptr == NULL )
            {
                /* no pathname specified so just use the filename */
                tmpptr = tempname;
            }
            else
            {
                /* increment the pointer by one so that the '\' is not part
                   of the gridname */
                tmpptr++;
            }
        }
        else
        {
            /* increment the pointer by one so that the '/' is not part of
               the gridname */
            tmpptr++;
        }
        sprintf( gridname, "%s_%.0f", tmpptr, output->pixel_size );
    }

    /* store current grid name */
    if ( hdfptr->currgrid )
	free( hdfptr->currgrid );
    hdfptr->currgrid = strdup( gridname );
    if ( hdfptr->currgrid == NULL )
    {
	sprintf( tempname, "Unable to allocate strdup memory for currgrid" );
	ErrorHandler( FALSE, "CreateHdfEosGridMosaic", ERROR_MEMORY, tempname );
	DestroyFileDescriptor( output );
	return ERROR_MEMORY;
    }

    /* if already attached to a grid, detach from it */
    if ( hdfptr->gid >= 0 )
	GDdetach( hdfptr->gid );

    /* create the new grid */
    hdfptr->gid = GDcreate( hdfptr->fid, gridname, xdimsize, ydimsize,
        upleft, lowright );

    /* check if grid was successfully created */
    if ( hdfptr->gid == -1 )
    {
	sprintf( tempname, "Unable to create grid %s", gridname );
	ErrorHandler( FALSE, "CreateHdfEosGridMosaic", ERROR_OPEN_OUTPUTIMAGE,
            tempname );
	DestroyFileDescriptor( output );
	return -1;
    }

    /* allocate a structure */
    outproj = ( ProjInfo * ) calloc( 1, sizeof( ProjInfo ) );
    if ( !outproj )
    {
        ErrorHandler( FALSE, "CreateHdfEosGridMosaic", ERROR_MEMORY,
            "ProjInfo" );
	DestroyFileDescriptor( output );
	return -1;
    }

    /* write projection parameters */
    /* note that proj type must be corresponding GCTP value */
    /* proj params also should match GCTP values */
    outproj->proj_code = mosaic->projection_info->proj_code;
    outproj->datum_code = mosaic->datum_code;

    /* hack to output ISIN projection code as 99 instead of 31 */
    if ( outproj->proj_code == ISINUS )
	outproj->proj_code = GCTP_ISINUS;

    /* set up the 15 projection parameters, using the original semi-major
       and semi-minor if this is ISIN. NOTE: do not use the outproj->proj_coef
       since these values are in DMS. */
    for ( i = 0; i < NUM_PROJECTION_PARAMS; i++ )
    {
        proj_coef[i] = mosaic->projection_parameters[i];
    }

    /* according to HDF-EOS documentation (e.g. GDdefproj example),
       angular projection parameters in HDF-EOS structural metadata
       are in DMS. the MRT software used the parameters in decimal
       degrees. therefore they need to be converted from decimal degrees
       to DMS before outputting to the HDF-EOS file. */
    if ( !Deg2DMS( mosaic->projection_type, proj_coef ) )
    {
        ErrorHandler( TRUE, "CreateHdfEosGridMosaic", ERROR_GENERAL,
            "Error converting projection parameters from degrees to DMS");
    }

    /* write projection parameters to grid */
    status = GDdefproj( hdfptr->gid, outproj->proj_code, outproj->zone_code,
	outproj->datum_code, proj_coef );

    /* define coordinate system origin */
    status = GDdeforigin( hdfptr->gid, mosaic->coord_origin );

    DestroyProjectionInfo( outproj );

    /* apparently must detach/reattach grid before writing data */
    status = GDdetach( hdfptr->gid );
    hdfptr->gid = GDattach( hdfptr->fid, gridname );

    return ( status == -1 );	/* nonzero return means error */
}

/******************************************************************************

MODULE:  CreateHdfEosFieldMosaic

PURPOSE:  Create a new field in an output HDF file for mosaicking

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status          See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         07/02  Gail Schmidt           Original Development

NOTES:

******************************************************************************/
int CreateHdfEosFieldMosaic
(
    MosaicDescriptor *mosaic,   /* I: session info */
    int curr_band,              /* I: current output band to be processed */
    FileDescriptor *output 	/* O: output file descriptor */
)
{
    int32 numbertype;
    int status;

    char fieldname[LARGE_STRING],    /* strings for names */
         tempname[LARGE_STRING],     /* LARGE_STRING defined in shared_resample.h */
         *tmpptr = NULL,            /* temporary basename ptr */
         *ext = NULL;
    HdfEosFD *hdfptr = ( HdfEosFD * ) output->fileptr;

    /* make a field name */
    /* HDF-EOS input file? give it same name as input field */
    if ( mosaic->filetype == HDFEOS )
    {
	strcpy( fieldname, mosaic->bandinfo[curr_band].name );
    }

    /* otherwise give it same name as file, plus pixel size, plus band # */
    else
    {
	strcpy( tempname, output->filename );
	ext = strrchr( tempname, '.' );
	*ext = '\0';		/* strip .hdf extension */
        /* find the end of the path since some pathnames may have a
           '.' in them. check Unix/Linux then Windows. */
        tmpptr = strrchr( tempname, '/' );
        if ( tmpptr == NULL )
        {   /* check Windows */
            tmpptr = strrchr( tempname, '\\' );
            if ( tmpptr == NULL )
            {
                /* no pathname specified so just use the filename */
                tmpptr = tempname;
            }
            else
            {
                /* increment the pointer by one so that the '\' is not part
                   of the gridname */
                tmpptr++;
            }
        }
        else
        {
            /* increment the pointer by one so that the '/' is not part of
               the gridname */
            tmpptr++;
        }
        sprintf( fieldname, "%s_%.0f_band%i", tmpptr, output->pixel_size,
            output->bandnum );
    }

    /* set data type */
    switch ( output->datatype )
    {
	case DFNT_INT8:
	case DFNT_UINT8:
	case DFNT_INT16:
	case DFNT_UINT16:
	case DFNT_INT32:
	case DFNT_UINT32:
	case DFNT_FLOAT32:
	    numbertype = output->datatype;
	    break;
	default:
	    numbertype = BAD_DATA_TYPE;
	    ErrorHandler( FALSE, "CreateHdfEosFieldMosaic", ERROR_DATA_TYPE,
		"Bad output data type" );
	    DestroyFileDescriptor( output );
	    return ERROR_DATA_TYPE;
    }

    /* define a field/band/entry in the grid */
    status = GDdeffield( hdfptr->gid, fieldname, "YDim,XDim", numbertype,
                         HDFE_NOMERGE );

    /* check if field was successfully created */
    if ( status == -1 )
    {
	sprintf( tempname, "Unable to create field %s in grid %s", fieldname,
	    hdfptr->currgrid );
	ErrorHandler( FALSE, "CreateHdfEosFieldMosaic", ERROR_OPEN_OUTPUTIMAGE,
            tempname );
	DestroyFileDescriptor( output );
	return ERROR_OPEN_OUTPUTIMAGE;
    }

    /* store current field name */
    hdfptr->currfield = strdup( fieldname );
    if ( hdfptr->currfield == NULL )
    {
	sprintf( tempname, "Unable to allocate memory for hdfptr->currfield" );
	ErrorHandler( FALSE, "CreateHdfEosFieldMosaic", ERROR_MEMORY,
            tempname );
	DestroyFileDescriptor( output );
	return ERROR_MEMORY;
    }

    /* apparently must detach/reattach grid before writing data */
    GDdetach( hdfptr->gid );
    hdfptr->gid = GDattach( hdfptr->fid, hdfptr->currgrid );

    return status;
}
