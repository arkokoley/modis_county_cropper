
/******************************************************************************

FILE:  hdf_init.c

PURPOSE:  Initialize an HDF output file

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  John Weiss             Original Development

HARDWARE AND/OR SOFTWARE LIMITATIONS: 
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/

#include "shared_resample.h"

/******************************************************************************

MODULE:  CreateHdfEosGrid

PURPOSE:  Create a grid in an HDF output file

RETURN VALUE:
Type = int
Value           Description
-----           -----------
-1              Failure
0               Success

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  John Weiss             Original Development
         01/01  John Rishea            Standardized formatting
         01/01  John Rishea            Removed debugging code
         02/01  Rob Burrell            Added packed DMS to output if GEO
         07/01  Rob Burrell            Move the corner coord to the LR corner of
                                       LR pixel
         11/01  Gail Schmidt           The LR pixel is already the LR of the
                                       LR pixel
         01/02  Gail Schmidt           Removed the pathname from the output
                                       filename before creating the grid name
         01/02  Gail Schmidt           Output Geographic corners and
                                       projection coords in DEG instead of
                                       DMS, since the MRT expects DEG when
                                       reading the data back in
         05/02  Gail Schmidt           Keep the original grid/Vgroup name
                                       if input file is HDF-EOS
         11/02  Gail Schmidt           HDF-EOS documentation specifies that
                                       Geographic corner points should be
                                       output as DMS. The MRT processes these
                                       corner points as decimal degrees, so
                                       they must be converted to DMS before
                                       outputting them to HDF-EOS.
         10/04  Gail Schmidt           HDF-EOS documentation specifies that
                (from Terry Haran)     angular projection parameters should
                                       be output as DMS. The MRT processes
                                       these parameters as decimal degrees, so
                                       they must be converted to DMS before
                                       outputting them to HDF-EOS.


NOTES:

******************************************************************************/

int CreateHdfEosGrid
(
    FileDescriptor *input,	/* input file descriptor */
    FileDescriptor *output,	/* output file descriptor */
    ModisDescriptor *modis	/* session info */
)

{
    double temp;                    /* temp value for deg to dms */
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

    if ( modis->resampling_type == NO_RESAMPLE )
    {   /* format conversion */
	CopyInputParametersToOutput( modis );

        /* get the projection coordinate corners for the input file */
        GetInputImageCorners( modis, input );

        /* set the projection coordinate corners for the output file to
           be the same as the input file */
        /* GetOutputImageCorners( modis, output, input ); */
        for ( i = 0; i < 4; i++ )
        {
            output->coord_corners[i][0] =
                modis->proj_spac_sub_gring_corners[i][0];
            output->coord_corners[i][1] =
                modis->proj_spac_sub_gring_corners[i][1];
        }

        /* calculate the number of rows/cols for output */
        /* GetOutputExtents( output ); */
        output->nrows = (int) ((((fabs)
           (output->coord_corners[UL][1] - output->coord_corners[LL][1])) /
            output->output_pixel_size) + 0.5);
        output->ncols = (int) ((((fabs)
           (output->coord_corners[UL][0] - output->coord_corners[UR][0])) /
            output->output_pixel_size) + 0.5);

        /* reallocate based on window */
        if ( output->rowbuffer )
            free ( output->rowbuffer );

        /* new buffer, things may have changed */
        output->rowbuffer = ( void * ) calloc ( output->ncols,
            output->datasize );
        if ( !output->rowbuffer )
            ErrorHandler( TRUE, "NoResample", ERROR_MEMORY,
                "Error allocating memory for the output Row Buffer" );
    }
    else
    {
        /* convert between projection units and lat/lon */
        GetInputImageCorners( modis, input );
        GetOutputImageCorners( modis, output, input );
        GetOutputExtents( output );
    }

    xdimsize = output->ncols;
    ydimsize = output->nrows;

    /* set real corners.  geographic corners will be output in dms and
       all other projection corners will be output in meters. */
    upleft[0] = output->coord_corners[UL][0];
    upleft[1] = output->coord_corners[UL][1];
    lowright[0] = output->coord_corners[LR][0];
    lowright[1] = output->coord_corners[LR][1];

    /* if this data is being reprojected to Geographic, then the upper left
       and lower right corners should be output in DMS (according to the
       HDF-EOS documentation). the MRT software processes the Geographic
       corners in decimal degrees. convert the corner points to DMS for
       output to the HDF-EOS file. */
    if ( modis->output_projection_type == PROJ_GEO )
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

    /* if input and output data types are HDF-EOS, then use the grid name
       from the original HDF-EOS file */
    if ( modis->input_filetype == HDFEOS )
    {
        strcpy( gridname, modis->input_gridname );
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
	ErrorHandler( TRUE, "CreateHdfEosGrid", ERROR_MEMORY, tempname );
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
	ErrorHandler( TRUE, "CreateHdfEosGrid", ERROR_OPEN_OUTPUTIMAGE,
            tempname );
	DestroyFileDescriptor( output );
	return -1;
    }

    /* write projection parameters */
    /* note that proj type must be corresponding GCTP value */
    /* proj params also should match GCTP values */
    outproj = GetOutputProjection( modis );

    /* JMW 04/19/01 - hack to output ISIN projection code as 99 instead of 31 */
    if ( outproj->proj_code == ISINUS )
	outproj->proj_code = GCTP_ISINUS;

    /* set up the 15 projection parameters, using the original semi-major
       and semi-minor if this is ISIN. NOTE: do not use the outproj->proj_coef
       since these values are in DMS. */
    for ( i = 0; i < NUM_PROJECTION_PARAMS; i++ )
    {
        proj_coef[i] = modis->output_projection_parameters[i];
    }

    /* according to HDF-EOS documentation (e.g. GDdefproj example),
       angular projection parameters in HDF-EOS structural metadata
       are in DMS. the MRT software used the parameters in decimal
       degrees. therefore they need to be converted from decimal degrees
       to DMS before outputting to the HDF-EOS file. */
    if ( !Deg2DMS( modis->output_projection_type, proj_coef ) )
    {
        ErrorHandler( TRUE, "CreateHdfEosGrid", ERROR_GENERAL,
            "Error converting projection parameters from degrees to DMS");
    }

    /* if both the input and output projections are UTM and the output
       datum code is not defined, then the input sphere code was used for
       the output sphere */
    if (modis->input_projection_type == PROJ_UTM &&
        modis->output_projection_type == PROJ_UTM &&
        modis->output_datum_code == E_NODATUM)
    {
        outproj->datum_code = modis->input_sphere_code;
    }

    /* write projection parameters to grid */
    status = GDdefproj( hdfptr->gid, outproj->proj_code, outproj->zone_code,
	outproj->datum_code, proj_coef );

    /* define coordinate system origin */
    status = GDdeforigin( hdfptr->gid, modis->coord_origin );

    DestroyProjectionInfo( outproj );

    /* apparently must detach/reattach grid before writing data */
    status = GDdetach( hdfptr->gid );
    hdfptr->gid = GDattach( hdfptr->fid, gridname );

    return ( status == -1 );	/* nonzero return means error */
}

/******************************************************************************

MODULE:  CreateHdfEosField

PURPOSE:  Create a new field in an output HDF file

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status          See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  John Weiss             Original Development
         01/01  John Rishea            Standardized formatting

NOTES:

******************************************************************************/
int CreateHdfEosField
(
    FileDescriptor *input,	/* input file descriptor */
    FileDescriptor *output,	/* output file descriptor */
    ModisDescriptor *modis	/* session info */
)
{
    int32 numbertype;
    int status;

    char fieldname[LARGE_STRING],   /* strings for names */
         tempname[LARGE_STRING],    /* LARGE_STRING defined in shared_resample.h */
         *tmpptr = NULL,            /* temporary basename ptr */
         *ext = NULL;               /* pointer to the file extension */
    HdfEosFD *hdfptr = ( HdfEosFD * ) output->fileptr;

    /* make a field name */
    /* HDF-EOS input file? give it same name as input field */
    if ( input->filetype == HDFEOS )
    {
	strcpy( fieldname, modis->bandinfo[input->bandnum].name );
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
	    ErrorHandler( TRUE, "CreateHdfEosField", ERROR_DATA_TYPE,
		"Bad output data type" );
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
	ErrorHandler( TRUE, "CreateHdfEosField", ERROR_OPEN_OUTPUTIMAGE,
            tempname );
	DestroyFileDescriptor( output );
	return ERROR_OPEN_OUTPUTIMAGE;
    }

    /* store current field name */
    hdfptr->currfield = strdup( fieldname );
    if ( hdfptr->currfield == NULL )
    {
	sprintf( tempname, "Unable to allocate memory for hdfptr->currfield" );
	ErrorHandler( TRUE, "CreateHdfEosField", ERROR_MEMORY, tempname );
	DestroyFileDescriptor( output );
	return ERROR_MEMORY;
    }

    /* apparently must detach/reattach grid before writing data */
    GDdetach( hdfptr->gid );
    hdfptr->gid = GDattach( hdfptr->fid, hdfptr->currgrid );

    return status;
}
