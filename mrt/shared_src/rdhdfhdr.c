/******************************************************************************

FILE:  rdhdfhdr.c

PURPOSE:  read an HDF-EOS header

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  John Weiss             Original Development
         01/01  John Rishea            Standardized formatting
         01/01  John Rishea            Moved local prototype to loc_prot.h
         06/01  John Weiss             Add 3-D/4-D data support.
         11/02  Gail Schmidt           ISIN_ERANGE is changed to GCTP_ERANGE to
                                       also support SIN and other projs
         11/02  Gail Schmidt           HDF-EOS documentation specifies that
                                       Geographic corner points are stored in
                                       DMS. The MRT now reads Geographic
                                       corner points as DMS, then converts
                                       them to decimal degrees for processing.
         02/03  Gail Schmidt           Added support for GCTP_IN_BREAK returned
                                       from Interrupted Goode's
         10/03  Gail Schmidt           If the 3D/4D names are too long, then
                                       they need to be truncated
         10/04  Gail Schmidt           HDF-EOS documentation specifies that
                (from Terry Haran)     angular projection parameters are
                                       stored in DMS. The MRT expects that
                                       input projection parameters are in
                                       decimal degrees, so the HDF projection
                                       parameters are converted to DMS after
                                       reading them.
         01/07  Gail Schmidt           Modified the call to GCTP to send in
                                       the sphere code which will be used for
                                       UTM only
         11/07  Gail Schmidt           Only log messages dumped out from
	                               ReadHDFHeader* routines, since these
				       also interface with the GUI.  Messages
				       dumped to stdout have an adverse affect
				       on the GUI when reading the HDF header
				       info.

HARDWARE AND/OR SOFTWARE LIMITATIONS:
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/
#include <math.h>               /* fabs */
#if !defined(__CYGWIN__) && !defined(__APPLE__) && !defined(WIN32)
#include <values.h>
#endif
#include "mrt_dtype.h"
#include "shared_mosaic.h"
#include "worgen.h"
#include "cproj.h"

void getdatumvalue ( int32 sds_id, int *datumvalue );

int GetSDSValues( int32 sd_id, int band, int32 numbertype,
    double *scalevalue, double *offsetvalue, double *minvalue,
    double *maxvalue, double *fillvalue, int *datumvalue );

/******************************************************************************

MODULE:  ReadHDFHeader

PURPOSE:  Opens input data file and fills in ModisDescriptor with nlines,
  nsamples, nbands, etc.

RETURN VALUE:  Type = int

Value           Description
-----           -----------
status          See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  John Weiss             Original Development
         05/00  John Weiss             GetInputGeoCorner to find lat/lon corners
         06/00  John Weiss             Fixed pixel_size, multiple grids
         06/00  John Weiss             Separate data type for each field
         07/00  John Weiss             Error handling return values added
         01/01  John Rishea            Standardized formatting
         01/01  John Rishea            Added checks for memory allocation
         01/01  John Rishea            Removed test code
         04/01  John Weiss             Background fill defaults to valid_min-1.
         04/01  Rob Burrell            Added TM projection
         07/01  Rob Burrell            Fixed pixel size and input corner coord
         11/01  Gail Schmidt           Check for ISIN_ERANGE to be returned
                                       from any GetInputGeoCorner
                                       Read and return the bounding rectangular
                                       coordinates
         11/01  Gail Schmidt           Removed the +1 from the pixel size
                                       calculation.
         11/01  Gail Schmidt           Leave the UL and LR corners as the UL
                                       and LR extents of the pixels.
         01/02  Gail Schmidt           Read the datum value as an HDF attribute
                                       if it is available (supports the fact
                                       that the datum will be written to
                                       HDF-EOS files output from the MRT).
         11/02  Gail Schmidt           Added support for Albers Equal Area.
4.0      10/07  Gail Schmidt           Modified to appropriately handle
                                       bounding tiles

NOTES:

******************************************************************************/
int ReadHDFHeader
(
    ModisDescriptor *modis      /* I/O:  session info */
)

{
    int i, j, k, kk, m, n;
    size_t nbi;
    int32 fid, gid;
    int32 ngrids, nfields;
    int bandnum = 0, total_bands = 0, nslices = 0;
    int dim3d = 0, dim4d = 0;
    int sdsfield = 0;
    int32 tmprank[256], tmpnumtype[256];
    int32 rank, numbertype, status, bufsiz;
    int32 projcode, zonecode, spherecode, origincode;
    int32 xdimsize, ydimsize;
    int32 dims[10];
    int32 sd_id = -1;
    int all_bounding_coords_present = FALSE;
    int modis_tile = FALSE;
    int datumvalue;

    float64 upleft[2], lowright[2], projparm[16];
    double pixel_size_x, pixel_size_y;
    double scalevalue, offsetvalue, minvalue, maxvalue, fillvalue;
    char strbuf[HUGE_STRING], error_str[SMALL_STRING];
    char strbuf2[HUGE_STRING];
    char logmsg[HUGE_STRING];
    char *gridlist = NULL, *gridname = NULL, *gridend = NULL;
    char *gridlist_ptr = NULL;
    char *fieldlist = NULL, *fieldname = NULL, *fieldend = NULL;
    char *fieldlist_ptr = NULL;
    char *dimname = NULL, *name3d = NULL, *name4d = NULL;
    int pos[4];

        /*******************************************************************/

    /* open HDF-EOS file for reading */
    fid = GDopen( modis->input_filename, DFACC_READ );
    if ( fid < 0 )
    {
        sprintf( error_str, "Unable to open %s", modis->input_filename );
        ErrorHandler( TRUE, "ReadHDFHeader", ERROR_OPEN_INPUTHEADER,
            error_str );
        /* return ERROR_OPEN_INPUTHEADER; */
    }

    /* also open as HDF file */
    sd_id = SDstart( modis->input_filename, DFACC_RDONLY );
    if ( sd_id < 0 )
    {
        sprintf( error_str, "unable to open %s for reading as SDS",
            modis->input_filename );
        ErrorHandler( FALSE, "ReadHDFHeader", ERROR_READ_INPUTHEADER,
            error_str );
        /* return ERROR_READ_INPUTHEADER; */
    }

        /*******************************************************************/

    /* get list of grids from HDF-EOS file */
    ngrids = GDinqgrid( modis->input_filename, strbuf, &bufsiz );

    /* store list of grids */
    gridlist_ptr = strdup( strbuf );
    if ( gridlist_ptr == NULL )
    {
        sprintf( error_str, "Unable to allocate memory for gridlist." );
        ErrorHandler( TRUE, "ReadHDFHeader", ERROR_MEMORY, error_str );
        /* return ERROR_MEMORY; */
    }
    gridlist = gridlist_ptr;

    /* check if there's any data in it. if no data in the grid then most
       likely this is swath or point data. the MRT only supports gridded
       HDF-EOS data.  use MRTSwath for swath products. */
    if ( ngrids < 1 || strlen( gridlist ) < 1 )
    {
        sprintf( error_str, "No grid data found in %s. The MRT only supports "
            "gridded HDF-EOS data, not swath or point HDF-EOS data. Use "
            "MRTSwath for processing MODIS swath products.",
            modis->input_filename );
        free( gridlist_ptr );
        ErrorHandler( TRUE, "ReadHDFHeader", ERROR_READ_INPUTHEADER,
            error_str );
        /* return ERROR_READ_INPUTHEADER; */
    }

    /*******************************************************************/

    /*  prepare to allocate memory for band info and subsetting */

    /* add up bands in each grid */
    for ( i = 0; i < ngrids; i++ )
    {
        /* get next grid name from comma-separated gridlist */
        /* gridname = i == 0 ? strtok( gridlist, "," ) : strtok( NULL, "," ); */
        gridname = gridlist;
        gridend = strchr( gridlist, ',' );
        if ( gridend != NULL )
        {
            gridlist = gridend + 1;
            *gridend = '\0';
        }

        /* attach to next grid */
        gid = GDattach( fid, gridname );
        if ( gid < 0 )
        {
            sprintf( error_str, "Unable to attach to grid %s", gridname );
            free( gridlist_ptr );
            ErrorHandler( TRUE, "ReadHDFHeader", ERROR_OPEN_INPUTHEADER,
                error_str );
            /* return ERROR_OPEN_INPUTHEADER; */
        }

        /* get list of fields in grid */
        nfields = GDinqfields( gid, strbuf, tmprank, tmpnumtype );
        fieldlist_ptr = strdup( strbuf );
        if ( fieldlist_ptr == NULL )
        {
            free( gridlist_ptr );
            ErrorHandler( TRUE, "ReadHDFHeader", ERROR_MEMORY,
                "Unable to allocate strdup memory for fieldlist." );
            /* return ERROR_MEMORY; */
        }
        fieldlist = fieldlist_ptr;

        /* loop through fields in grid */
        for ( j = 0; j < nfields; j++ )
        {
            /* get name of next field */
            fieldname = fieldlist;
            fieldend = strchr( fieldlist, ',' );
            if ( fieldend != NULL )
            {
                fieldlist = fieldend + 1;
                *fieldend = '\0';
            }

            /* get field info */
            status = GDfieldinfo( gid, fieldname, &rank, dims, &numbertype,
                strbuf );

            /* count number of slices in 3-D and 4-D data sets */
            nslices = 1;
            if ( rank > 2 )     /* save some processing */
            {
                for ( k = 0, dimname = strtok( strbuf, "," );
                    k < rank && dimname != NULL; k++,
                    dimname = strtok( NULL, "," ) )
                {
                    if ( strcmp( dimname, "XDim" ) && strcmp( dimname, "YDim" ))
                        nslices *= dims[k];
                }
            }

            /* count total number of bands */
            total_bands += nslices;
        }

        /* close grid */
        GDdetach( gid );

        /* free fieldlist */
        free( fieldlist_ptr );
        fieldlist_ptr = fieldlist = NULL;
    }

    /* free gridlist */
    free( gridlist_ptr );
    gridlist_ptr = gridlist = NULL;

    /* allocate memory for band info and subsetting */
    modis->nbands = total_bands;
    modis->bandinfo = calloc( modis->nbands, sizeof( BandType ) );
    if ( modis->bandinfo == NULL )
    {
        ErrorHandler( TRUE, "ReadHDFHeader", ERROR_MEMORY,
            "Cannot allocate memory for band selection!" );
        /* return ERROR_MEMORY; */
    }

    /*******************************************************************/

    /* by default, select all bands */
    for ( nbi = 0; nbi < modis->nbands; nbi++ )
        modis->bandinfo[nbi].selected = 1;

    /*******************************************************************/

    /* get list of grids from HDF file (again) */
    ngrids = GDinqgrid( modis->input_filename, strbuf, &bufsiz );
    gridlist_ptr = strdup( strbuf );
    if ( gridlist_ptr == NULL )
    {
        sprintf( error_str, "Unable to allocate memory for gridlist." );
        ErrorHandler( TRUE, "ReadHDFHeader", ERROR_MEMORY, error_str );
        /* return ERROR_MEMORY; */
    }
    gridlist = gridlist_ptr;

    /* loop through grids */
    for ( i = 0; i < ngrids; i++ )
    {
        /* get next grid name from comma-separated gridlist */
        gridname = gridlist;
        gridend = strchr( gridlist, ',' );
        if ( gridend != NULL )
        {
            gridlist = gridend + 1;
            *gridend = '\0';
        }

        /* attach to next grid */
        gid = GDattach( fid, gridname );
        if ( gid < 0 )
        {
            sprintf( error_str, "Unable to attach to grid %s", gridname );
            free( gridlist_ptr );
            ErrorHandler( TRUE, "ReadHDFHeader", ERROR_OPEN_INPUTHEADER,
                error_str );
            /* return ERROR_OPEN_INPUTHEADER; */
        }

        /*******************************************************************/

        /* determine input projection type: PROJECTION_TYPE = ... */

        /* get projection parameters */
        status = GDprojinfo( gid, &projcode, &zonecode, &spherecode, projparm );
        if ( status != 0 )
        {
            free( gridlist_ptr );
            ErrorHandler( TRUE, "ReadHDFHeader", ERROR_READ_INPUTHEADER,
                "No input projection info" );
            /* return ERROR_READ_INPUTHEADER; */
        }

        /* store projection info, if the datum code has been specified then
           ignore the spherecode. store the sphere code in case the input
           and/or output projection is UTM. */
        if ( modis->input_datum_code == E_NODATUM )
            modis->input_datum_code = spherecode;
        modis->input_sphere_code = spherecode;
        modis->input_zone_code = zonecode;
        switch ( projcode )
        {
            case ISINUS:
            case GCTP_ISINUS:
                modis->input_projection_type = PROJ_ISIN;
                break;
            case GCTP_ALBERS:
                modis->input_projection_type = PROJ_AEA;
                break;
            case GCTP_EQRECT:
                modis->input_projection_type = PROJ_ER;
                break;
            case GCTP_GEO:
                modis->input_projection_type = PROJ_GEO;
                break;
            case GCTP_HAMMER:
                modis->input_projection_type = PROJ_HAM;
                break;
            case GCTP_GOOD:
                modis->input_projection_type = PROJ_IGH;
                break;
            case GCTP_LAMAZ:
                modis->input_projection_type = PROJ_LA;
                break;
            case GCTP_LAMCC:
                modis->input_projection_type = PROJ_LCC;
                break;
            case GCTP_MERCAT:
                modis->input_projection_type = PROJ_MERC;
                break;
            case GCTP_MOLL:
                modis->input_projection_type = PROJ_MOL;
                break;
            case GCTP_PS:
                modis->input_projection_type = PROJ_PS;
                break;
            case GCTP_SNSOID:
                modis->input_projection_type = PROJ_SIN;
                break;
            case GCTP_TM:
                modis->input_projection_type = PROJ_TM;
                break;
            case GCTP_UTM:
                modis->input_projection_type = PROJ_UTM;
                break;

            default:
                modis->input_projection_type = BAD_PROJECTION_TYPE;
                free( gridlist_ptr );
                ErrorHandler( TRUE, "ReadHDFHeader", ERROR_PROJECTION_TYPE,
                    "Bad Input Projection Type" );
                /* return ERROR_PROJECTION_TYPE; */
        }

        /*******************************************************************/

        /* store the input projection parameters */
        for ( j = 0; j < 13; j++ )
            modis->input_projection_parameters[j] = projparm[j];
        modis->input_projection_parameters[13] = 0.0;
        modis->input_projection_parameters[14] = 0.0;

        /* according to HDF-EOS documentation (e.g. GDdefproj example),
           angular projection parameters in HDF-EOS structural metadata
           are in DMS. the MRT software needs the parameters in decimal
           degrees. convert from DMS to decimal degrees. */
        if ( !DMS2Deg( modis->input_projection_type,
                       modis->input_projection_parameters ) )
        {
            free( gridlist_ptr );
            ErrorHandler( TRUE, "ReadHDFHeader", ERROR_GENERAL,
                "Error converting projection parameters from DMS to degrees");
        }

        /*******************************************************************/

        /* get grid info */
        status = GDgridinfo( gid, &xdimsize, &ydimsize, upleft, lowright );

        /* if this data is in the Geographic projection, then the upper left
           and lower right corners should be in DMS (according to the
           HDF-EOS documentation). the MRT software needs the corners in
           decimal degrees for processing Geographic data. convert from DMS
           to decimal degrees. */
        if ( modis->input_projection_type == PROJ_GEO )
        {
            dmsdeg( upleft[0], &upleft[0] );
            dmsdeg( upleft[1], &upleft[1] );
            dmsdeg( lowright[0], &lowright[0] );
            dmsdeg( lowright[1], &lowright[1] );
        }

        /*******************************************************************/

        /* get coordinate system origin */
        status = GDorigininfo( gid, &origincode );
        if ( status == 0 )
            modis->coord_origin = ( CornerType )origincode;

        /*******************************************************************/

        /* read the bounding rectangle coordinates */
        if ( read_bounding_coords (sd_id, modis, &all_bounding_coords_present)
             != 0 )
        {
            /* not able to read the bounding rectangular coords, but continue
               because only the bounding ISIN tiles need these */
            all_bounding_coords_present = FALSE;
        }

        /*******************************************************************/

        /* compute actual pixel size */
        pixel_size_y = ( upleft[1] - lowright[1] ) / ydimsize;
        pixel_size_x = ( lowright[0] - upleft[0] ) / xdimsize;

        if ( fabs( pixel_size_x - pixel_size_y ) > 0.5 )
        {
            sprintf( error_str, "Warning: non-square pixel sizes!\n"
                                "   pixel_size_x = %f\n"
                                "   pixel_size_y = %f\n",
                                pixel_size_x, pixel_size_y );
            MessageHandler( "ReadHDFHeader", error_str );
        }

        /*******************************************************************/

        /* process each band in grid, storing nlines/nsamples/pixel_size */

        /* get list of fields in grid (again) */
        nfields = GDinqfields( gid, strbuf, tmprank, tmpnumtype );
        fieldlist_ptr = strdup( strbuf );
        if ( fieldlist_ptr == NULL )
        {
            free( gridlist_ptr );
            ErrorHandler( TRUE, "ReadHDFHeader", ERROR_MEMORY,
                "Unable to allocate strdup memory for fieldlist." );
            return ERROR_MEMORY;
        }
        fieldlist = fieldlist_ptr;

        /* loop through fields in grid */
        for ( j = 0; j < nfields; j++ )
        {
            /* get name of next field */
            fieldname = fieldlist;
            fieldend = strchr( fieldlist, ',' );
            if ( fieldend != NULL )
            {
                fieldlist = fieldend + 1;
                *fieldend = '\0';
            }

            /* get field info */
            status = GDfieldinfo( gid, fieldname, &rank, dims, &numbertype,
                strbuf );

            /* determine input and output data type for each field */
            switch ( numbertype )
            {
                case DFNT_INT8:
                case DFNT_UINT8:
                case DFNT_INT16:
                case DFNT_UINT16:
                case DFNT_INT32:
                case DFNT_UINT32:
                case DFNT_FLOAT32:
                    break;

                default:
                    numbertype = BAD_DATA_TYPE;
                    sprintf( error_str, "Bad value (%i)", ( int ) numbertype );
                    free( gridlist_ptr );
                    free( fieldlist_ptr );
                    ErrorHandler( TRUE, "ReadHDFHeader", ERROR_DATA_TYPE,
                        error_str );
                    /* return ERROR_DATA_TYPE; */
            }

            /* read valid range, background fill, and datum values as
               HDF attributes */
            scalevalue = offsetvalue = minvalue = maxvalue = fillvalue = 0.0;
            GetSDSValues( sd_id, sdsfield, numbertype, &scalevalue,
                &offsetvalue, &minvalue, &maxvalue, &fillvalue, &datumvalue );
            if ( modis->input_projection_type == PROJ_ISIN && datumvalue == 1 )
                modis->input_datum_code = E_NODATUM;            
            else
                modis->input_datum_code = datumvalue;            

            /* count number of slices in 3-D and 4-D data sets */
            for ( kk = 0; kk < 4; kk++ )
                pos[kk] = -1;
            switch ( rank )
            {
                case 1:         /* 1-D case - user should NOT select this! */
                case 2:         /* 2-D case - just store fieldname */
                    modis->bandinfo[bandnum].name = strdup( fieldname );
                    if ( modis->bandinfo[bandnum].name == NULL )
                    {
                        sprintf( error_str, "band name memory allocation" );
                        free( gridlist_ptr );
                        free( fieldlist_ptr );
                        ErrorHandler( TRUE, "ReadHDFHeader", ERROR_MEMORY,
                            error_str );
                        /* return ERROR_MEMORY; */
                    }

                    /* store nlines, nsamples, pixel size, datatype for each
                       band */
                    modis->bandinfo[bandnum].nlines = ydimsize;
                    modis->bandinfo[bandnum].nsamples = xdimsize;
                    if ( rank == 1 )    /* 1-D case */
                        modis->bandinfo[bandnum].nsamples = 1;
                    modis->bandinfo[bandnum].pixel_size = pixel_size_y;
                    modis->bandinfo[bandnum].input_datatype =
                        modis->bandinfo[bandnum].output_datatype = numbertype;
                    modis->bandinfo[bandnum].scale_factor = scalevalue;
                    modis->bandinfo[bandnum].offset = offsetvalue;
                    modis->bandinfo[bandnum].min_value = minvalue;
                    modis->bandinfo[bandnum].max_value = maxvalue;
                    modis->bandinfo[bandnum].background_fill = fillvalue;

                    modis->bandinfo[bandnum].fieldnum = sdsfield;
                    modis->bandinfo[bandnum].rank = rank;
                    for ( k = 0, dimname = strtok( strbuf, "," );
                        k < rank && dimname != NULL;
                        k++, dimname = strtok( NULL, "," ) )
                    {
                        if ( !strcmp( dimname, "YDim" ) )
                            pos[0] = k;
                        else
                            pos[1] = k;
                    }
                    for ( kk = 0; kk < 4; kk++ )
                        modis->bandinfo[bandnum].pos[kk] = pos[kk];

                    bandnum++;
                    break;

                case 3:         /* 3-D case */
                    /* loop through dimensions and get the positions before
                       they are used later */
                    for ( k = 0, dimname = strtok( strbuf, "," );
                        k < rank && dimname != NULL;
                        k++, dimname = strtok( NULL, "," ) )
                    {
                        if ( !strcmp( dimname, "YDim" ) )
                            pos[0] = k;
                        else if ( !strcmp( dimname, "XDim" ) )
                            pos[1] = k;
                        else
                            pos[2] = k;

                        if ( strcmp( dimname, "XDim" ) &&
                             strcmp( dimname, "YDim" ) )
                        {
                            name3d = strdup( dimname );
                            dim3d = dims[k];
                        }
                    }

                    /* use the 3d name to store band names according to our
                       3-D slice naming conventions */
                    for ( m = 1; m <= dim3d; m++ )
                    {
                        sprintf( strbuf2, "%s.%s_%.2i", fieldname, name3d, m );

                        /* HDF_EOS library sets field name length limit
                         * of 57 in GDapi.c GDdeffield() function.  If the
                         * output file format is HDF, then we need to limit
                         * the size of the SDS names.
                         */
                        if ( modis->output_filetype == HDFEOS )
                        {
                            /* if it's too long try a shortened form */
                            if ( strlen( strbuf2 ) > 57 )
                                sprintf( strbuf2, "%s.Dim3_%.2i", fieldname,
                                    m );

                            /* if it's still too long truncate the name */
                            if ( strlen( strbuf2 ) > 57 )
                                sprintf( strbuf2, "%.52s.3_%.2i", fieldname,
                                    m );
                        }

                        modis->bandinfo[bandnum].name = strdup( strbuf2 );
                        if ( modis->bandinfo[bandnum].name == NULL )
                        {
                            sprintf( error_str, "band name memory allocation" );
                            free( gridlist_ptr );
                            free( fieldlist_ptr );
                            ErrorHandler( TRUE, "ReadHDFHeader", ERROR_MEMORY,
                                error_str );
                            /* return ERROR_MEMORY; */
                        }

                        /* store nlines, nsamples, pixel size, datatype for
                           each band */
                        modis->bandinfo[bandnum].nlines = ydimsize;
                        modis->bandinfo[bandnum].nsamples = xdimsize;
                        modis->bandinfo[bandnum].pixel_size = pixel_size_y;
                        modis->bandinfo[bandnum].input_datatype =
                            modis->bandinfo[bandnum].output_datatype =
                            numbertype;
                        modis->bandinfo[bandnum].scale_factor = scalevalue;
                        modis->bandinfo[bandnum].offset = offsetvalue;
                        modis->bandinfo[bandnum].min_value = minvalue;
                        modis->bandinfo[bandnum].max_value = maxvalue;
                        modis->bandinfo[bandnum].background_fill = fillvalue;

                        modis->bandinfo[bandnum].fieldnum = sdsfield;
                        modis->bandinfo[bandnum].rank = rank;
                        for ( kk = 0; kk < 4; kk++ )
                            modis->bandinfo[bandnum].pos[kk] = pos[kk];
                        bandnum++;
                    }

                    break;

                case 4:         /* 4-D case */
                    /* loop through dimensions to get names and sizes of
                       dimensions */
                    dim3d = -1;
                    for ( k = 0, dimname = strtok( strbuf, "," );
                        k < rank && dimname != NULL;
                        k++, dimname = strtok( NULL, "," ) )
                    {
                        if ( !strcmp( dimname, "YDim" ) )
                            pos[0] = k;
                        else if ( !strcmp( dimname, "XDim" ) )
                            pos[1] = k;
                        else if ( pos[2] == -1 )
                            pos[2] = k;
                        else
                            pos[3] = k;

                        if ( strcmp( dimname, "XDim" ) &&
                             strcmp( dimname, "YDim" ) )
                        {
                            if ( dim3d < 0 )
                            {
                                name3d = strdup( dimname );
                                dim3d = dims[k];
                            }
                            else
                            {
                                name4d = strdup( dimname );
                                dim4d = dims[k];
                            }
                        }
                    }

                    /* store band names according to our 4-D slice naming
                       conventions */
                    for ( n = 1; n <= dim4d; n++ )
                        for ( m = 1; m <= dim3d; m++ )
                        {
                            sprintf( strbuf2, "%s.%s_%.2i.%s_%.2i", fieldname,
                                name3d, m, name4d, n );

                            /* HDF_EOS library sets field name length limit
                             * of 57 in GDapi.c GDdeffield() function.  If the
                             * output file format is HDF, then we need to limit
                             * the size of the SDS names.
                             */
                            if ( modis->output_filetype == HDFEOS )
                            {
                                /* if it's too long, try a shortened form */
                                if ( strlen( strbuf2 ) > 57 )
                                    sprintf( strbuf2, "%s.Dim3_%.2i.Dim4_%.2i",
                                        fieldname, m, n );

                                /* if it's still too long, truncate the field
                                   name */
                                if ( strlen( strbuf2 ) > 57 )
                                    sprintf( strbuf2, "%.47s.3_%.2i.4_%.2i",
                                        fieldname, m, n );
                            }

                            modis->bandinfo[bandnum].name = strdup( strbuf2 );
                            if ( modis->bandinfo[bandnum].name == NULL )
                            {
                                sprintf( error_str,
                                    "bandinfo.name memory allocation" );
                                free( gridlist_ptr );
                                free( fieldlist_ptr );
                                ErrorHandler( TRUE, "ReadHDFHeader",
                                    ERROR_MEMORY, error_str );
                                /* return ERROR_MEMORY; */
                            }

                            /* store nlines, nsamples, pixel size, datatype
                               for each band */
                            modis->bandinfo[bandnum].nlines = ydimsize;
                            modis->bandinfo[bandnum].nsamples = xdimsize;
                            modis->bandinfo[bandnum].pixel_size = pixel_size_y;
                            modis->bandinfo[bandnum].input_datatype =
                                modis->bandinfo[bandnum].output_datatype =
                                numbertype;
                            modis->bandinfo[bandnum].scale_factor = scalevalue;
                            modis->bandinfo[bandnum].offset = offsetvalue;
                            modis->bandinfo[bandnum].min_value = minvalue;
                            modis->bandinfo[bandnum].max_value = maxvalue;
                            modis->bandinfo[bandnum].background_fill =
                                fillvalue;

                            modis->bandinfo[bandnum].fieldnum = sdsfield;
                            modis->bandinfo[bandnum].rank = rank;
                            for ( kk = 0; kk < 4; kk++ )
                                modis->bandinfo[bandnum].pos[kk] = pos[kk];

                            bandnum++;
                        }
                    break;

                default:
                    sprintf( error_str, "MRT can only handle 2-D, 3-D, and "
                        "4-D data (field %s, rank %i)", fieldname, (int) rank );
                    free( gridlist_ptr );
                    free( fieldlist_ptr );
                    ErrorHandler( TRUE, "ReadHDFHeader",
                        ERROR_READ_INPUTHEADER, error_str );
                    /* return ERROR_READ_INPUTHEADER; */
            }

            sdsfield++;
        }

        /* free fieldlist */
        free( fieldlist_ptr );
        fieldlist_ptr = fieldlist = NULL;

        /*******************************************************************/

        /*
         * determine spatial extents:
         *
         * UL_CORNER = ( ULlat ULlon )
         * UR_CORNER = ( URlat URlon )
         * LL_CORNER = ( LLlat LLlon )
         * LR_CORNER = ( LRlat LRlon )
         * NOTE: The input datum is required for these calculations.
         */

        /* Convert corner points from projection coordinates to lat/long.
           If GCTP_ERANGE is returned from GetInputGeoCorner, then use the
           bounding rectangle coordinates for all corner points. */
        modis_tile = FALSE;
        /* UL corner */
        status = GetInputGeoCorner( modis, upleft[0], upleft[1],
             &modis->input_image_extent[UL][0],
             &modis->input_image_extent[UL][1] );
        if ( status == GCTP_ERANGE || status == GCTP_IN_BREAK )
        {
            /* If any corners fall outside the bounds, then use the bounding
               rectangle coordinates for all corner points. If dealing with
               SIN or ISIN then just bound the longitude at -180 and use the
               latitude from GCTP. */
            modis->use_bound = TRUE;
            if (modis->input_projection_type == PROJ_SIN ||
                modis->input_projection_type == PROJ_ISIN)
            {
                sprintf ( logmsg, "ReadHDFHeader : UL corner falls outside "
                    "the bounds of the input projection. The rectangle will "
                    "be bounded at -180 or 180 degrees longitude." );
                LogHandler (logmsg);

                /* If there was an issue inverse mapping this coordinate
                   then the coordinate likely wrapped around to the other
                   side of the Earth.  We want to bound at 180 and use the
                   opposite sign of the wrap-around longitude value. */
                modis_tile = TRUE;
                if (modis->input_image_extent[UL][1] > 0)
                    modis->input_image_extent[UL][1] = -BOUND_LONG;
                else
                    modis->input_image_extent[UL][1] = BOUND_LONG;
            }
            else
            {
                sprintf ( logmsg, "ReadHDFHeader : UL corner falls outside "
                    "the bounds of the input projection. The bounding "
                    "rectangle coordinates from the metadata will be used." );
                LogHandler (logmsg);
            }
        }

        /* UR corner */
        status = GetInputGeoCorner( modis, lowright[0], upleft[1],
             &modis->input_image_extent[UR][0],
             &modis->input_image_extent[UR][1] );
        if ( status == GCTP_ERANGE || status == GCTP_IN_BREAK )
        {
            /* If any corners fall outside the bounds, then use the bounding
               rectangle coordinates for all corner points. If dealing with
               SIN or ISIN then just bound the longitude at 180 and use the
               latitude from GCTP. */
            modis->use_bound = TRUE;
            if (modis->input_projection_type == PROJ_SIN ||
                modis->input_projection_type == PROJ_ISIN)
            {
                sprintf ( logmsg, "ReadHDFHeader : UR corner falls outside "
                    "the bounds of the input projection. The rectangle will "
                    "be bounded at -180 or 180 degrees longitude." );
                LogHandler (logmsg);

                /* If there was an issue inverse mapping this coordinate
                   then the coordinate likely wrapped around to the other
                   side of the Earth.  We want to bound at 180 and use the
                   opposite sign of the wrap-around longitude value. */
                modis_tile = TRUE;
                if (modis->input_image_extent[UR][1] > 0)
                    modis->input_image_extent[UR][1] = -BOUND_LONG;
                else
                    modis->input_image_extent[UR][1] = BOUND_LONG;
            }
            else
            {
                sprintf ( logmsg, "ReadHDFHeader : UR corner falls outside "
                    "the bounds of the input projection. The bounding "
                    "rectangle coordinates from the metadata will be used." );
                LogHandler (logmsg);
            }
        }

        /* LL corner */
        status = GetInputGeoCorner( modis, upleft[0], lowright[1],
             &modis->input_image_extent[LL][0],
             &modis->input_image_extent[LL][1] );
        if ( status == GCTP_ERANGE || status == GCTP_IN_BREAK )
        {
            /* If any corners fall outside the bounds, then use the bounding
               rectangle coordinates for all corner points. If dealing with
               SIN or ISIN then just bound the longitude at -180 and use the
               latitude from GCTP. */
            modis->use_bound = TRUE;
            if (modis->input_projection_type == PROJ_SIN ||
                modis->input_projection_type == PROJ_ISIN)
            {
                sprintf( logmsg, "ReadHDFHeader : LL corner falls outside "
                    "the bounds of the input projection. The rectangle will "
                    "be bounded at -180 or 180 degrees longitude." );
                LogHandler (logmsg);

                /* If there was an issue inverse mapping this coordinate
                   then the coordinate likely wrapped around to the other
                   side of the Earth.  We want to bound at 180 and use the
                   opposite sign of the wrap-around longitude value. */
                modis_tile = TRUE;
                if (modis->input_image_extent[LL][1] > 0)
                    modis->input_image_extent[LL][1] = -BOUND_LONG;
                else
                    modis->input_image_extent[LL][1] = BOUND_LONG;
            }
            else
            {
                sprintf( logmsg, "ReadHDFHeader : LL corner falls outside "
                    "the bounds of the input projection. The bounding "
                    "rectangle coordinates from the metadata will be used." );
                LogHandler (logmsg);
            }
        }

        /* LR corner */
        status = GetInputGeoCorner( modis, lowright[0], lowright[1],
             &modis->input_image_extent[LR][0],
             &modis->input_image_extent[LR][1] );
        if ( status == GCTP_ERANGE || status == GCTP_IN_BREAK )
        {
            /* If any corners fall outside the bounds, then use the bounding
               rectangle coordinates for all corner points. If dealing with
               SIN or ISIN then just bound the longitude at 180 and use the
               latitude from GCTP. */
            modis->use_bound = TRUE;
            if (modis->input_projection_type == PROJ_SIN ||
                modis->input_projection_type == PROJ_ISIN)
            {
                sprintf( logmsg, "ReadHDFHeader : LR corner falls outside "
                    "the bounds of the input projection. The rectangle "
                    "will be bounded at -180 or 180 degrees longitude." );
                LogHandler (logmsg);

                /* If there was an issue inverse mapping this coordinate
                   then the coordinate likely wrapped around to the other
                   side of the Earth.  We want to bound at 180 and use the
                   opposite sign of the wrap-around longitude value. */
                modis_tile = TRUE;
                if (modis->input_image_extent[LR][1] > 0)
                    modis->input_image_extent[LR][1] = -BOUND_LONG;
                else
                    modis->input_image_extent[LR][1] = BOUND_LONG;
            }
            else
            {
                sprintf( logmsg, "ReadHDFHeader : LR corner falls outside "
                    "the bounds of the input projection. The bounding "
                    "rectangle coordinates from the metadata will be used." );
                LogHandler (logmsg);
            }
        }

        /* Set the corner points to the bounding rectangular corner values
           if any of the original corner points were outside the bounds
           of the input projection.  If this is a MODIS tile, then we've
           already handled the bounding. */
        if ( modis->use_bound )
        {
            if ( !modis_tile)
            {
                if ( !all_bounding_coords_present )
                {
                    free( gridlist_ptr );
                    ErrorHandler( TRUE, "ReadHDFHeader", ERROR_GENERAL,
                        "The bounding rectangular coordinates were not "
                        "available in the Archive Metadata. This product "
                        "spans outside the bounds of the input projection "
                        "and the bounding coordinates are required to "
                        "process this product.\n" );
                }

                /* Set the lat/longs of the image extent based on the bounding
                   rectangular coordinates */
                modis->input_image_extent[UL][0] = modis->north_bound;
                modis->input_image_extent[UL][1] = modis->west_bound;
                modis->input_image_extent[UR][0] = modis->north_bound;
                modis->input_image_extent[UR][1] = modis->east_bound;
                modis->input_image_extent[LL][0] = modis->south_bound;
                modis->input_image_extent[LL][1] = modis->west_bound;
                modis->input_image_extent[LR][0] = modis->south_bound;
                modis->input_image_extent[LR][1] = modis->east_bound;
            }

            /* Save the original corner coordinates of this file */
            modis->orig_coord_corners[UL][0] = upleft[0];
            modis->orig_coord_corners[UL][1] = upleft[1];
            modis->orig_coord_corners[UR][0] = lowright[0];
            modis->orig_coord_corners[UR][1] = upleft[1];
            modis->orig_coord_corners[LL][0] = upleft[0];
            modis->orig_coord_corners[LL][1] = lowright[1];
            modis->orig_coord_corners[LR][0] = lowright[0];
            modis->orig_coord_corners[LR][1] = lowright[1];
        }

        /* close grid */
        GDdetach( gid );
    }  /* for (i = 0; i < ngrids; i++) */

        /*******************************************************************/

    /* finish up */

    /* free gridlist */
    free( gridlist_ptr );
    gridlist_ptr = gridlist = NULL;

    /* close the HDF file */
    status = SDend( sd_id );

    /* close the HDF-EOS file */
    GDclose( fid );

    return MRT_NO_ERROR;
}

/******************************************************************************

MODULE:  ReadHDFHeaderHDF2RB

PURPOSE:  Opens input data file and fills in ModisDescriptor with nlines,
  nsamples, nbands, etc. for the HDF to raw binary conversion.

RETURN VALUE:  Type = int

Value           Description
-----           -----------
status          See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         11/05  Gail Schmidt           Original Development

NOTES:

******************************************************************************/
int ReadHDFHeaderHDF2RB
(
    ModisDescriptor *modis      /* I/O:  session info */
)

{
    int i, j, k, kk, m, n;
    size_t nbi;
    int32 fid, gid;
    int32 ngrids, nfields;
    int bandnum = 0, total_bands = 0, nslices = 0;
    int dim3d = 0, dim4d = 0;
    int sdsfield = 0;
    int32 tmprank[256], tmpnumtype[256];
    int32 rank, numbertype, status, bufsiz;
    int32 projcode, zonecode, spherecode, origincode;
    int32 xdimsize, ydimsize;
    int32 dims[10];
    int32 sd_id = -1;
    int all_bounding_coords_present = FALSE;
    int datumvalue;

    float64 upleft[2], lowright[2], projparm[16];
    double pixel_size_x, pixel_size_y;
    double scalevalue, offsetvalue, minvalue, maxvalue, fillvalue;
    char strbuf[HUGE_STRING], error_str[SMALL_STRING];
    char strbuf2[HUGE_STRING];
    char *gridlist = NULL, *gridname = NULL, *gridend = NULL;
    char *gridlist_ptr = NULL;
    char *fieldlist = NULL, *fieldname = NULL, *fieldend = NULL;
    char *fieldlist_ptr = NULL;
    char *dimname = NULL, *name3d = NULL, *name4d = NULL;
    int pos[4];

        /*******************************************************************/

    /* open HDF-EOS file for reading */
    fid = GDopen( modis->input_filename, DFACC_READ );
    if ( fid < 0 )
    {
        sprintf( error_str, "Unable to open %s", modis->input_filename );
        ErrorHandler( TRUE, "ReadHDFHeaderHDF2RB", ERROR_OPEN_INPUTHEADER,
            error_str );
        /* return ERROR_OPEN_INPUTHEADER; */
    }

    /* also open as HDF file */
    sd_id = SDstart( modis->input_filename, DFACC_RDONLY );
    if ( sd_id < 0 )
    {
        sprintf( error_str, "unable to open %s for reading as SDS",
            modis->input_filename );
        ErrorHandler( FALSE, "ReadHDFHeaderHDF2RB", ERROR_READ_INPUTHEADER,
            error_str );
        /* return ERROR_READ_INPUTHEADER; */
    }

    /*******************************************************************/

    /* get list of grids from HDF-EOS file */
    ngrids = GDinqgrid( modis->input_filename, strbuf, &bufsiz );

    /* store list of grids */
    gridlist_ptr = strdup( strbuf );
    if ( gridlist_ptr == NULL )
    {
        sprintf( error_str, "Unable to allocate memory for gridlist." );
        ErrorHandler( TRUE, "ReadHDFHeaderHDF2RB", ERROR_MEMORY, error_str );
        /* return ERROR_MEMORY; */
    }
    gridlist = gridlist_ptr;

    /* check if there's any data in it. if no data in the grid then most
       likely this is swath or point data. the MRT only supports gridded
       HDF-EOS data.  use MRTSwath for swath products. */
    if ( ngrids < 1 || strlen( gridlist ) < 1 )
    {
        sprintf( error_str, "No grid data found in %s. The MRT only supports "
            "gridded HDF-EOS data, not swath or point HDF-EOS data. Use "
            "MRTSwath for processing MODIS swath products.",
            modis->input_filename );
        free( gridlist_ptr );
        ErrorHandler( TRUE, "ReadHDFHeaderHDF2RB", ERROR_READ_INPUTHEADER,
            error_str );
        /* return ERROR_READ_INPUTHEADER; */
    }

    /*******************************************************************/

    /* prepare to allocate memory for band info and subsetting */

    /* add up bands in each grid */
    for ( i = 0; i < ngrids; i++ )
    {
        /* get next grid name from comma-separated gridlist */
        /* gridname = i == 0 ? strtok( gridlist, "," ) : strtok( NULL, "," ); */
        gridname = gridlist;
        gridend = strchr( gridlist, ',' );
        if ( gridend != NULL )
        {
            gridlist = gridend + 1;
            *gridend = '\0';
        }

        /* attach to next grid */
        gid = GDattach( fid, gridname );
        if ( gid < 0 )
        {
            sprintf( error_str, "Unable to attach to grid %s", gridname );
            free( gridlist );
            ErrorHandler( TRUE, "ReadHDFHeaderHDF2RB", ERROR_OPEN_INPUTHEADER,
                error_str );
            /* return ERROR_OPEN_INPUTHEADER; */
        }

        /* get list of fields in grid */
        nfields = GDinqfields( gid, strbuf, tmprank, tmpnumtype );
        fieldlist_ptr = strdup( strbuf );
        if ( fieldlist_ptr == NULL )
        {
            free( gridlist_ptr );
            ErrorHandler( TRUE, "ReadHDFHeaderHDF2RB", ERROR_MEMORY,
                "Unable to allocate strdup memory for fieldlist." );
            /* return ERROR_MEMORY; */
        }
        fieldlist = fieldlist_ptr;

        /* loop through fields in grid */
        for ( j = 0; j < nfields; j++ )
        {
            /* get name of next field */
            fieldname = fieldlist;
            fieldend = strchr( fieldlist, ',' );
            if ( fieldend != NULL )
            {
                fieldlist = fieldend + 1;
                *fieldend = '\0';
            }

            /* get field info */
            status = GDfieldinfo( gid, fieldname, &rank, dims, &numbertype,
                strbuf );

            /* count number of slices in 3-D and 4-D data sets */
            nslices = 1;
            if ( rank > 2 )     /* save some processing */
            {
                for ( k = 0, dimname = strtok( strbuf, "," );
                    k < rank && dimname != NULL; k++,
                    dimname = strtok( NULL, "," ) )
                {
                    if ( strcmp( dimname, "XDim" ) && strcmp( dimname, "YDim" ))
                        nslices *= dims[k];
                }
            }

            /* count total number of bands */
            total_bands += nslices;
        }

        /* close grid */
        GDdetach( gid );

        /* free fieldlist */
        free( fieldlist_ptr );
        fieldlist_ptr = fieldlist = NULL;
    }

    /* free gridlist */
    free( gridlist_ptr );
    gridlist_ptr = gridlist = NULL;

    /* allocate memory for band info and subsetting */
    modis->nbands = total_bands;
    modis->bandinfo = calloc( modis->nbands, sizeof( BandType ) );
    if ( modis->bandinfo == NULL )
    {
        ErrorHandler( TRUE, "ReadHDFHeaderHDF2RB", ERROR_MEMORY,
            "Cannot allocate memory for band selection!" );
        /* return ERROR_MEMORY; */
    }

        /*******************************************************************/

    /* by default, select all bands */
    for ( nbi = 0; nbi < modis->nbands; nbi++ )
        modis->bandinfo[nbi].selected = 1;

        /*******************************************************************/

    /* get list of grids from HDF file (again) */
    ngrids = GDinqgrid( modis->input_filename, strbuf, &bufsiz );
    gridlist_ptr = strdup( strbuf );
    if ( gridlist_ptr == NULL )
    {
        sprintf( error_str, "Unable to allocate memory for gridlist." );
        ErrorHandler( TRUE, "ReadHDFHeaderHDF2RB", ERROR_MEMORY, error_str );
        return ERROR_MEMORY;
    }
    gridlist = gridlist_ptr;

    /* loop through grids */
    for ( i = 0; i < ngrids; i++ )
    {
        /* get next grid name from comma-separated gridlist */
        gridname = gridlist;
        gridend = strchr( gridlist, ',' );
        if ( gridend != NULL )
        {
            gridlist = gridend + 1;
            *gridend = '\0';
        }

        /* attach to next grid */
        gid = GDattach( fid, gridname );
        if ( gid < 0 )
        {
            sprintf( error_str, "Unable to attach to grid %s", gridname );
            free( gridlist_ptr );
            ErrorHandler( TRUE, "ReadHDFHeaderHDF2RB", ERROR_OPEN_INPUTHEADER,
                error_str );
            /* return ERROR_OPEN_INPUTHEADER; */
        }

        /*******************************************************************/

        /* determine input projection type: PROJECTION_TYPE = ... */

        /* get projection parameters */
        status = GDprojinfo( gid, &projcode, &zonecode, &spherecode, projparm );
        if ( status != 0 )
        {
            free( gridlist_ptr );
            ErrorHandler( TRUE, "ReadHDFHeaderHDF2RB", ERROR_READ_INPUTHEADER,
                "No input projection info" );
            /* return ERROR_READ_INPUTHEADER; */
        }

        /* store projection info, if the datum code has been specified then
           ignore the spherecode */
        if ( modis->input_datum_code == E_NODATUM )
            modis->input_datum_code = spherecode;
        modis->input_sphere_code = spherecode;
        modis->input_zone_code = zonecode;
        switch ( projcode )
        {
            case ISINUS:
            case GCTP_ISINUS:
                modis->input_projection_type = PROJ_ISIN;
                break;
            case GCTP_ALBERS:
                modis->input_projection_type = PROJ_AEA;
                break;
            case GCTP_EQRECT:
                modis->input_projection_type = PROJ_ER;
                break;
            case GCTP_GEO:
                modis->input_projection_type = PROJ_GEO;
                break;
            case GCTP_HAMMER:
                modis->input_projection_type = PROJ_HAM;
                break;
            case GCTP_GOOD:
                modis->input_projection_type = PROJ_IGH;
                break;
            case GCTP_LAMAZ:
                modis->input_projection_type = PROJ_LA;
                break;
            case GCTP_LAMCC:
                modis->input_projection_type = PROJ_LCC;
                break;
            case GCTP_MERCAT:
                modis->input_projection_type = PROJ_MERC;
                break;
            case GCTP_MOLL:
                modis->input_projection_type = PROJ_MOL;
                break;
            case GCTP_PS:
                modis->input_projection_type = PROJ_PS;
                break;
            case GCTP_SNSOID:
                modis->input_projection_type = PROJ_SIN;
                break;
            case GCTP_TM:
                modis->input_projection_type = PROJ_TM;
                break;
            case GCTP_UTM:
                modis->input_projection_type = PROJ_UTM;
                break;

            default:
                modis->input_projection_type = BAD_PROJECTION_TYPE;
                free( gridlist_ptr );
                ErrorHandler( TRUE, "ReadHDFHeaderHDF2RB",
                    ERROR_PROJECTION_TYPE, "Bad Input Projection Type" );
                /* return ERROR_PROJECTION_TYPE; */
        }

        /*******************************************************************/

        /* store the input projection parameters */
        for ( j = 0; j < 13; j++ )
            modis->input_projection_parameters[j] = projparm[j];
        modis->input_projection_parameters[13] = 0.0;
        modis->input_projection_parameters[14] = 0.0;

        /* according to HDF-EOS documentation (e.g. GDdefproj example),
           angular projection parameters in HDF-EOS structural metadata
           are in DMS. the MRT software needs the parameters in decimal
           degrees. convert from DMS to decimal degrees. */
        if ( !DMS2Deg( modis->input_projection_type,
                       modis->input_projection_parameters ) )
        {
            free( gridlist_ptr );
            ErrorHandler( TRUE, "ReadHDFHeaderHDF2RB", ERROR_GENERAL,
                "Error converting projection parameters from DMS to degrees");
        }

        /*******************************************************************/

        /* get grid info */
        status = GDgridinfo( gid, &xdimsize, &ydimsize, upleft, lowright );

        /* if this data is in the Geographic projection, then the upper left
           and lower right corners should be in DMS (according to the
           HDF-EOS documentation). the MRT software needs the corners in
           decimal degrees for processing Geographic data. convert from DMS
           to decimal degrees. */
        if ( modis->input_projection_type == PROJ_GEO )
        {
            dmsdeg( upleft[0], &upleft[0] );
            dmsdeg( upleft[1], &upleft[1] );
            dmsdeg( lowright[0], &lowright[0] );
            dmsdeg( lowright[1], &lowright[1] );
        }

        /*******************************************************************/

        /* get coordinate system origin */
        status = GDorigininfo( gid, &origincode );
        if ( status == 0 )
            modis->coord_origin = ( CornerType )origincode;

        /*******************************************************************/

        /* read the bounding rectangle coordinates */
        if ( read_bounding_coords (sd_id, modis, &all_bounding_coords_present)
             != 0 )
        {
            /* not able to read the bounding rectangular coords, but continue
               because only the bounding ISIN tiles need these */
            all_bounding_coords_present = FALSE;
        }

        /*******************************************************************/

        /* compute actual pixel size */
        pixel_size_y = ( upleft[1] - lowright[1] ) / ydimsize;
        pixel_size_x = ( lowright[0] - upleft[0] ) / xdimsize;

        if ( fabs( pixel_size_x - pixel_size_y ) > 0.5 )
        {
            sprintf( error_str, "Warning: non-square pixel sizes!\n"
                                "   pixel_size_x = %f\n"
                                "   pixel_size_y = %f\n",
                                pixel_size_x, pixel_size_y );
            MessageHandler( "ReadHDFHeaderHDF2RB", error_str );
        }

        /*******************************************************************/

        /* process each band in grid, storing nlines/nsamples/pixel_size */

        /* get list of fields in grid (again) */
        nfields = GDinqfields( gid, strbuf, tmprank, tmpnumtype );
        fieldlist_ptr = strdup( strbuf );
        if ( fieldlist_ptr == NULL )
        {
            free( gridlist_ptr );
            ErrorHandler( TRUE, "ReadHDFHeaderHDF2RB", ERROR_MEMORY,
                "Unable to allocate strdup memory for fieldlist." );
            /* return ERROR_MEMORY; */
        }
        fieldlist = fieldlist_ptr;

        /* loop through fields in grid */
        for ( j = 0; j < nfields; j++ )
        {
            /* get name of next field */
            fieldname = fieldlist;
            fieldend = strchr( fieldlist, ',' );
            if ( fieldend != NULL )
            {
                fieldlist = fieldend + 1;
                *fieldend = '\0';
            }

            /* get field info */
            status = GDfieldinfo( gid, fieldname, &rank, dims, &numbertype,
                strbuf );

            /* determine input and output data type for each field */
            switch ( numbertype )
            {
                case DFNT_INT8:
                case DFNT_UINT8:
                case DFNT_INT16:
                case DFNT_UINT16:
                case DFNT_INT32:
                case DFNT_UINT32:
                case DFNT_FLOAT32:
                    break;

                default:
                    numbertype = BAD_DATA_TYPE;
                    sprintf( error_str, "Bad value (%i)", ( int ) numbertype );
                    free( gridlist_ptr );
                    free( fieldlist_ptr );
                    ErrorHandler( TRUE, "ReadHDFHeaderHDF2RB", ERROR_DATA_TYPE,
                        error_str );
                    /* return ERROR_DATA_TYPE; */
            }

            /* read valid range, background fill, and datum values as
               HDF attributes */
            scalevalue = offsetvalue = minvalue = maxvalue = fillvalue = 0.0;
            GetSDSValues( sd_id, sdsfield, numbertype, &scalevalue,
                &offsetvalue, &minvalue, &maxvalue, &fillvalue, &datumvalue );
            if ( modis->input_projection_type == PROJ_ISIN && datumvalue == 1 )
                modis->input_datum_code = E_NODATUM;            
            else
                modis->input_datum_code = datumvalue;            

            /* count number of slices in 3-D and 4-D data sets */
            for ( kk = 0; kk < 4; kk++ )
                pos[kk] = -1;
            switch ( rank )
            {
                case 1:         /* 1-D case - user should NOT select this! */
                case 2:         /* 2-D case - just store fieldname */
                    modis->bandinfo[bandnum].name = strdup( fieldname );
                    if ( modis->bandinfo[bandnum].name == NULL )
                    {
                        sprintf( error_str, "band name memory allocation" );
                        free( gridlist_ptr );
                        free( fieldlist_ptr );
                        ErrorHandler( TRUE, "ReadHDFHeaderHDF2RB", ERROR_MEMORY,
                            error_str );
                        /* return ERROR_MEMORY; */
                    }

                    /* store nlines, nsamples, pixel size, datatype for each
                       band */
                    modis->bandinfo[bandnum].nlines = ydimsize;
                    modis->bandinfo[bandnum].nsamples = xdimsize;
                    if ( rank == 1 )    /* 1-D case */
                        modis->bandinfo[bandnum].nsamples = 1;
                    modis->bandinfo[bandnum].pixel_size = pixel_size_y;
                    modis->bandinfo[bandnum].input_datatype =
                        modis->bandinfo[bandnum].output_datatype = numbertype;
                    modis->bandinfo[bandnum].scale_factor = scalevalue;
                    modis->bandinfo[bandnum].offset = offsetvalue;
                    modis->bandinfo[bandnum].min_value = minvalue;
                    modis->bandinfo[bandnum].max_value = maxvalue;
                    modis->bandinfo[bandnum].background_fill = fillvalue;

                    modis->bandinfo[bandnum].fieldnum = sdsfield;
                    modis->bandinfo[bandnum].rank = rank;
                    for ( k = 0, dimname = strtok( strbuf, "," );
                        k < rank && dimname != NULL;
                        k++, dimname = strtok( NULL, "," ) )
                    {
                        if ( !strcmp( dimname, "YDim" ) )
                            pos[0] = k;
                        else
                            pos[1] = k;
                    }
                    for ( kk = 0; kk < 4; kk++ )
                        modis->bandinfo[bandnum].pos[kk] = pos[kk];

                    bandnum++;
                    break;

                case 3:         /* 3-D case */
                    /* loop through dimensions and get the positions before
                       they are used later */
                    for ( k = 0, dimname = strtok( strbuf, "," );
                        k < rank && dimname != NULL;
                        k++, dimname = strtok( NULL, "," ) )
                    {
                        if ( !strcmp( dimname, "YDim" ) )
                            pos[0] = k;
                        else if ( !strcmp( dimname, "XDim" ) )
                            pos[1] = k;
                        else
                            pos[2] = k;

                        if ( strcmp( dimname, "XDim" ) &&
                             strcmp( dimname, "YDim" ) )
                        {
                            name3d = strdup( dimname );
                            dim3d = dims[k];
                        }
                    }

                    /* use the 3d name to store band names according to our
                       3-D slice naming conventions */
                    for ( m = 1; m <= dim3d; m++ )
                    {
                        sprintf( strbuf2, "%s.%s_%.2i", fieldname, name3d, m );
                        modis->bandinfo[bandnum].name = strdup( strbuf2 );
                        if ( modis->bandinfo[bandnum].name == NULL )
                        {
                            sprintf( error_str, "band name memory allocation" );
                            free( gridlist_ptr );
                            free( fieldlist_ptr );
                            ErrorHandler( TRUE, "ReadHDFHeaderHDF2RB",
                                ERROR_MEMORY, error_str );
                            /* return ERROR_MEMORY; */
                        }

                        /* store nlines, nsamples, pixel size, datatype for
                           each band */
                        modis->bandinfo[bandnum].nlines = ydimsize;
                        modis->bandinfo[bandnum].nsamples = xdimsize;
                        modis->bandinfo[bandnum].pixel_size = pixel_size_y;
                        modis->bandinfo[bandnum].input_datatype =
                            modis->bandinfo[bandnum].output_datatype =
                            numbertype;
                        modis->bandinfo[bandnum].scale_factor = scalevalue;
                        modis->bandinfo[bandnum].offset = offsetvalue;
                        modis->bandinfo[bandnum].min_value = minvalue;
                        modis->bandinfo[bandnum].max_value = maxvalue;
                        modis->bandinfo[bandnum].background_fill = fillvalue;

                        modis->bandinfo[bandnum].fieldnum = sdsfield;
                        modis->bandinfo[bandnum].rank = rank;
                        for ( kk = 0; kk < 4; kk++ )
                            modis->bandinfo[bandnum].pos[kk] = pos[kk];
                        bandnum++;
                    }

                    break;

                case 4:         /* 4-D case */
                    /* loop through dimensions to get names and sizes of
                       dimensions */
                    dim3d = -1;
                    for ( k = 0, dimname = strtok( strbuf, "," );
                        k < rank && dimname != NULL;
                        k++, dimname = strtok( NULL, "," ) )
                    {
                        if ( !strcmp( dimname, "YDim" ) )
                            pos[0] = k;
                        else if ( !strcmp( dimname, "XDim" ) )
                            pos[1] = k;
                        else if ( pos[2] == -1 )
                            pos[2] = k;
                        else
                            pos[3] = k;

                        if ( strcmp( dimname, "XDim" ) &&
                             strcmp( dimname, "YDim" ) )
                        {
                            if ( dim3d < 0 )
                            {
                                name3d = strdup( dimname );
                                dim3d = dims[k];
                            }
                            else
                            {
                                name4d = strdup( dimname );
                                dim4d = dims[k];
                            }
                        }
                    }

                    /* store band names according to our 4-D slice naming
                       conventions */
                    for ( n = 1; n <= dim4d; n++ )
                        for ( m = 1; m <= dim3d; m++ )
                        {
                            sprintf( strbuf2, "%s.%s_%.2i.%s_%.2i", fieldname,
                                name3d, m, name4d, n );
                            modis->bandinfo[bandnum].name = strdup( strbuf2 );
                            if ( modis->bandinfo[bandnum].name == NULL )
                            {
                                sprintf( error_str,
                                    "bandinfo.name memory allocation" );
                                free( gridlist_ptr );
                                free( fieldlist_ptr );
                                ErrorHandler( TRUE, "ReadHDFHeaderHDF2RB",
                                    ERROR_MEMORY, error_str );
                                /* return ERROR_MEMORY; */
                            }

                            /* store nlines, nsamples, pixel size, datatype
                               for each band */
                            modis->bandinfo[bandnum].nlines = ydimsize;
                            modis->bandinfo[bandnum].nsamples = xdimsize;
                            modis->bandinfo[bandnum].pixel_size = pixel_size_y;
                            modis->bandinfo[bandnum].input_datatype =
                                modis->bandinfo[bandnum].output_datatype =
                                numbertype;
                            modis->bandinfo[bandnum].scale_factor = scalevalue;
                            modis->bandinfo[bandnum].offset = offsetvalue;
                            modis->bandinfo[bandnum].min_value = minvalue;
                            modis->bandinfo[bandnum].max_value = maxvalue;
                            modis->bandinfo[bandnum].background_fill =
                                fillvalue;

                            modis->bandinfo[bandnum].fieldnum = sdsfield;
                            modis->bandinfo[bandnum].rank = rank;
                            for ( kk = 0; kk < 4; kk++ )
                                modis->bandinfo[bandnum].pos[kk] = pos[kk];

                            bandnum++;
                        }
                    break;

                default:
                    sprintf( error_str, "MRT can only handle 2-D, 3-D, and "
                        "4-D data (field %s, rank %i)", fieldname, (int) rank );
                    free( gridlist_ptr );
                    free( fieldlist_ptr );
                    ErrorHandler( TRUE, "ReadHDFHeaderHDF2RB",
                        ERROR_READ_INPUTHEADER, error_str );
                    /* return ERROR_READ_INPUTHEADER; */
            }

            sdsfield++;
        }

        /* free fieldlist */
        free( fieldlist_ptr );
        fieldlist_ptr = fieldlist = NULL;

        /*******************************************************************/

        /* Save the original corner coordinates of this file */
        modis->orig_coord_corners[UL][0] = upleft[0];
        modis->orig_coord_corners[UL][1] = upleft[1];
        modis->orig_coord_corners[UR][0] = lowright[0];
        modis->orig_coord_corners[UR][1] = upleft[1];
        modis->orig_coord_corners[LL][0] = upleft[0];
        modis->orig_coord_corners[LL][1] = lowright[1];
        modis->orig_coord_corners[LR][0] = lowright[0];
        modis->orig_coord_corners[LR][1] = lowright[1];

        /* Set the lat/long corner coords to an invalid value */
        modis->input_image_extent[UL][0] = -999.0;
        modis->input_image_extent[UL][1] = -999.0;
        modis->input_image_extent[UR][0] = -999.0;
        modis->input_image_extent[UR][1] = -999.0;
        modis->input_image_extent[LL][0] = -999.0;
        modis->input_image_extent[LL][1] = -999.0;
        modis->input_image_extent[LR][0] = -999.0;
        modis->input_image_extent[LR][1] = -999.0;

        /* close grid */
        GDdetach( gid );
    }

    /*******************************************************************/

    /* finish up */

    /* close the HDF file */
    status = SDend( sd_id );

    /* close the HDF-EOS file */
    GDclose( fid );

    /* free gridlist */
    free( gridlist_ptr );
    gridlist_ptr = gridlist = NULL;

    return MRT_NO_ERROR;
}

/******************************************************************************

MODULE:  ReadHDFHeaderMosaic

PURPOSE:  Opens input data file and fills in MosaicDescriptor with nlines,
  nsamples, nbands, etc.

RETURN VALUE:  Type = int

Value           Description
-----           -----------
status          See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         03/02  Gail Schmidt           Original Development

NOTES:

******************************************************************************/
int ReadHDFHeaderMosaic
(
    MosaicDescriptor *mosaic     /* I/O:  session info */
)

{
    int i, j, k, kk, m, n;
    int32 fid, gid;
    int32 ngrids, nfields;
    int bandnum = 0, total_bands = 0, nslices = 0;
    int dim3d = 0, dim4d = 0;
    int sdsfield = 0;
    int32 tmprank[256], tmpnumtype[256];
    int32 rank, numbertype, status, bufsiz;
    int32 projcode, zonecode, spherecode, origincode;
    int32 xdimsize, ydimsize;
    int32 dims[10];
    int32 sd_id = -1;
    int all_bounding_coords_present = FALSE;
    int use_bound = FALSE;
    int modis_tile = FALSE;
    int datumvalue;

    float64 upleft[2], lowright[2], projparm[16];
    double pixel_size_x, pixel_size_y;
    double scalevalue, offsetvalue, minvalue, maxvalue, fillvalue;
    char strbuf[HUGE_STRING], error_str[SMALL_STRING];
    char strbuf2[HUGE_STRING];
    char logmsg[HUGE_STRING];
    char *gridlist = NULL, *gridname = NULL, *gridend = NULL;
    char *gridlist_ptr = NULL;
    char *fieldlist = NULL, *fieldname = NULL, *fieldend = NULL;
    char *fieldlist_ptr = NULL;
    char *dimname = NULL, *name3d = NULL, *name4d = NULL;
    int pos[4];

    ModisDescriptor tmpmodis;

        /*******************************************************************/
    /* open HDF-EOS file for reading */
    fid = GDopen( mosaic->filename, DFACC_READ );
    if ( fid < 0 )
    {
        sprintf( error_str, "Unable to open %s for reading.",
            mosaic->filename );
        ErrorHandler( TRUE, "ReadHDFHeaderMosaic", ERROR_OPEN_INPUTHEADER,
            error_str );
        /* return ERROR_OPEN_INPUTHEADER; */
    }

    /* also open as HDF file */
    sd_id = SDstart( mosaic->filename, DFACC_RDONLY );
    if ( sd_id < 0 )
    {
        sprintf( error_str, "Unable to open %s for reading as SDS",
            mosaic->filename );
        ErrorHandler( FALSE, "ReadHDFHeaderMosaic", ERROR_READ_INPUTHEADER,
            error_str );
        /* return ERROR_READ_INPUTHEADER; */
    }

        /*******************************************************************/

    /* get list of grids from HDF-EOS file */
    ngrids = GDinqgrid( mosaic->filename, strbuf, &bufsiz );

    /* store list of grids */
    gridlist_ptr = strdup( strbuf );
    if ( gridlist_ptr == NULL )
    {
        sprintf( error_str, "Unable to allocate memory for gridlist." );
        ErrorHandler( TRUE, "ReadHDFHeaderMosaic", ERROR_MEMORY, error_str );
        /* return ERROR_MEMORY; */
    }
    gridlist = gridlist_ptr;

    /* check if there's any data in it. if no data in the grid then most
       likely this is swath or point data. the MRT currently only supports
       gridded HDF-EOS data. */
    if ( ngrids < 1 || strlen( gridlist ) < 1 )
    {
        sprintf( error_str, "No grid data found in %s. The MRT only supports "
            "gridded HDF-EOS data, not swath or point HDF-EOS data.",
            mosaic->filename );
        free( gridlist_ptr );
        ErrorHandler( TRUE, "ReadHDFHeaderMosaic", ERROR_READ_INPUTHEADER,
            error_str );
        /* return ERROR_READ_INPUTHEADER; */
    }

        /*******************************************************************/

    /*  prepare to allocate memory for band info and subsetting */

    /* add up bands in each grid */
    for ( i = 0; i < ngrids; i++ )
    {
        /* get next grid name from comma-separated gridlist */
        /* gridname = i == 0 ? strtok( gridlist, "," ) : strtok( NULL, "," ); */
        gridname = gridlist;
        gridend = strchr( gridlist, ',' );
        if ( gridend != NULL )
        {
            gridlist = gridend + 1;
            *gridend = '\0';
        }

        /* attach to next grid */
        gid = GDattach( fid, gridname );
        if ( gid < 0 )
        {
            sprintf( error_str, "Unable to attach to grid %s", gridname );
            free( gridlist_ptr );
            ErrorHandler( TRUE, "ReadHDFHeaderMosaic", ERROR_OPEN_INPUTHEADER,
                error_str );
            /* return ERROR_OPEN_INPUTHEADER; */
        }

        /* get list of fields in grid */
        nfields = GDinqfields( gid, strbuf, tmprank, tmpnumtype );
        fieldlist_ptr = strdup( strbuf );
        if ( fieldlist_ptr == NULL )
        {
            free( gridlist_ptr );
            ErrorHandler( TRUE, "ReadHDFHeaderMosaic", ERROR_MEMORY,
                "Unable to allocate strdup memory for fieldlist." );
            return ERROR_MEMORY;
        }
        fieldlist = fieldlist_ptr;

        /* loop through fields in grid */
        for ( j = 0; j < nfields; j++ )
        {
            /* get name of next field */
            fieldname = fieldlist;
            fieldend = strchr( fieldlist, ',' );
            if ( fieldend != NULL )
            {
                fieldlist = fieldend + 1;
                *fieldend = '\0';
            }

            /* get field info */
            status = GDfieldinfo( gid, fieldname, &rank, dims, &numbertype,
                strbuf );

            /* count number of slices in 3-D and 4-D data sets */
            nslices = 1;
            if ( rank > 2 )     /* save some processing */
            {
                for ( k = 0, dimname = strtok( strbuf, "," );
                    k < rank && dimname != NULL; k++,
                    dimname = strtok( NULL, "," ) )
                {
                    if ( strcmp( dimname, "XDim" ) && strcmp( dimname, "YDim" ))                        nslices *= dims[k];
                }
            }

            /* count total number of bands */
            total_bands += nslices;
        }

        /* close grid */
        GDdetach( gid );

        /* free fieldlist */
        free( fieldlist_ptr );
        fieldlist_ptr = fieldlist = NULL;
    }

    /* free the gridlist */
    free( gridlist_ptr );
    gridlist_ptr = gridlist = NULL;

    /* allocate memory for band info and subsetting */
    mosaic->nbands = total_bands;
    mosaic->bandinfo = calloc( mosaic->nbands, sizeof( BandType ) );
    if ( mosaic->bandinfo == NULL )
    {
        ErrorHandler( TRUE, "ReadHDFHeaderMosaic", ERROR_MEMORY,
            "Cannot allocate memory for band selection!" );
        return ERROR_MEMORY;
    }

    /*******************************************************************/

    /* get list of grids from HDF file (again) */
    ngrids = GDinqgrid( mosaic->filename, strbuf, &bufsiz );
    gridlist_ptr = strdup( strbuf );
    if ( gridlist_ptr == NULL )
    {
        sprintf( error_str, "Unable to allocate memory for gridlist." );
        ErrorHandler( TRUE, "ReadHDFHeaderMosaic", ERROR_MEMORY, error_str );
        return ERROR_MEMORY;
    }
    gridlist = gridlist_ptr;

    /* initialize the temporary MODIS Descriptor */
    InitializeModisDescriptor( &tmpmodis );

    /* loop through grids */
    for ( i = 0; i < ngrids; i++ )
    {
        /* get next grid name from comma-separated gridlist */
        /* gridname = i == 0 ? strtok( gridlist, "," ) : strtok( NULL, "," ); */
        gridname = gridlist;
        gridend = strchr( gridlist, ',' );
        if ( gridend != NULL )
        {
            gridlist = gridend + 1;
            *gridend = '\0';
        }

        /* attach to next grid */
        gid = GDattach( fid, gridname );
        if ( gid < 0 )
        {
            sprintf( error_str, "Unable to attach to grid %s", gridname );
            free( gridlist_ptr );
            ErrorHandler( TRUE, "ReadHDFHeaderMosaic", ERROR_OPEN_INPUTHEADER,
                error_str );
            /* return ERROR_OPEN_INPUTHEADER; */
        }

        /*******************************************************************/

        /* determine input projection type: PROJECTION_TYPE = ... */

        /* get projection parameters */
        status = GDprojinfo( gid, &projcode, &zonecode, &spherecode, projparm );
        if ( status != 0 )
        {
            free( gridlist_ptr );
            ErrorHandler( TRUE, "ReadHDFHeaderMosaic", ERROR_READ_INPUTHEADER,
                "No input projection info" );
            /* return ERROR_READ_INPUTHEADER; */
        }

        /* store projection info, if the datum code has been specified then
           ignore the spherecode */
        if ( mosaic->datum_code == E_NODATUM )
            mosaic->datum_code = spherecode;
        mosaic->input_sphere_code = spherecode;
        mosaic->zone_code = zonecode;
        switch ( projcode )
        {
            case ISINUS:
            case GCTP_ISINUS:
                mosaic->projection_type = PROJ_ISIN;
                break;
            case GCTP_ALBERS:
                mosaic->projection_type = PROJ_AEA;
                break;
            case GCTP_EQRECT:
                mosaic->projection_type = PROJ_ER;
                break;
            case GCTP_GEO:
                mosaic->projection_type = PROJ_GEO;
                break;
            case GCTP_HAMMER:
                mosaic->projection_type = PROJ_HAM;
                break;
            case GCTP_GOOD:
                mosaic->projection_type = PROJ_IGH;
                break;
            case GCTP_LAMAZ:
                mosaic->projection_type = PROJ_LA;
                break;
            case GCTP_LAMCC:
                mosaic->projection_type = PROJ_LCC;
                break;
            case GCTP_MERCAT:
                mosaic->projection_type = PROJ_MERC;
                break;
            case GCTP_MOLL:
                mosaic->projection_type = PROJ_MOL;
                break;
            case GCTP_PS:
                mosaic->projection_type = PROJ_PS;
                break;
            case GCTP_SNSOID:
                mosaic->projection_type = PROJ_SIN;
                break;
            case GCTP_TM:
                mosaic->projection_type = PROJ_TM;
                break;
            case GCTP_UTM:
                mosaic->projection_type = PROJ_UTM;
                break;

            default:
                mosaic->projection_type = BAD_PROJECTION_TYPE;
                free( gridlist_ptr );
                ErrorHandler( TRUE, "ReadHDFHeaderMosaic",
                    ERROR_PROJECTION_TYPE, "Bad Projection Type" );
                return ERROR_PROJECTION_TYPE;
        }

        /*******************************************************************/

        /* store the input projection parameters */
        for ( j = 0; j < 13; j++ )
            mosaic->projection_parameters[j] = projparm[j];
        mosaic->projection_parameters[13] = 0.0;
        mosaic->projection_parameters[14] = 0.0;

        /* according to HDF-EOS documentation (e.g. GDdefproj example),
           angular projection parameters in HDF-EOS structural metadata
           are in DMS. the MRT software needs the parameters in decimal
           degrees. convert from DMS to decimal degrees. */
        if ( !DMS2Deg( mosaic->projection_type,
                       mosaic->projection_parameters ) )
        {
            free( gridlist_ptr );
            ErrorHandler( TRUE, "ReadHDFHeaderMosaic", ERROR_GENERAL,
                "Error converting projection parameters from DMS to degrees");
        }

        /*******************************************************************/

        /* get grid info */
        status = GDgridinfo( gid, &xdimsize, &ydimsize, upleft, lowright );

        /* if this data is in the Geographic projection, then the upper left
           and lower right corners should be in DMS (according to the
           HDF-EOS documentation). the MRT software needs the corners in
           decimal degrees for processing Geographic data. convert from DMS
           to decimal degrees. */
        if ( mosaic->projection_type == PROJ_GEO )
        {
            dmsdeg( upleft[0], &upleft[0] );
            dmsdeg( upleft[1], &upleft[1] );
            dmsdeg( lowright[0], &lowright[0] );
            dmsdeg( lowright[1], &lowright[1] );
        }

        /* store the corner points */
        mosaic->proj_image_extent[UL][0] = upleft[0];
        mosaic->proj_image_extent[UL][1] = upleft[1];
        mosaic->proj_image_extent[UR][0] = lowright[0];
        mosaic->proj_image_extent[UR][1] = upleft[1];
        mosaic->proj_image_extent[LL][0] = upleft[0];
        mosaic->proj_image_extent[LL][1] = lowright[1];
        mosaic->proj_image_extent[LR][0] = lowright[0];
        mosaic->proj_image_extent[LR][1] = lowright[1];

        /*******************************************************************/

        /* get coordinate system origin */
        status = GDorigininfo( gid, &origincode );
        if ( status == 0 )
            mosaic->coord_origin = ( CornerType )origincode;

        /*******************************************************************/

        /* read the bounding rectangle coordinates */
        if ( read_bounding_coords (sd_id, &tmpmodis,
             &all_bounding_coords_present) != 0 )
        {
            /* not able to read the bounding rectangular coords, but continue
               because only the bounding ISIN tiles need these */
            all_bounding_coords_present = FALSE;
        }
        mosaic->north_bound = tmpmodis.north_bound;
        mosaic->south_bound = tmpmodis.south_bound;
        mosaic->east_bound = tmpmodis.east_bound;
        mosaic->west_bound = tmpmodis.west_bound;

        /*******************************************************************/

        /* compute actual pixel size */
        pixel_size_y = ( upleft[1] - lowright[1] ) / ydimsize;
        pixel_size_x = ( lowright[0] - upleft[0] ) / xdimsize;

        if ( fabs( pixel_size_x - pixel_size_y ) > 0.5 )
        {
            sprintf( error_str, "Warning: non-square pixel sizes!\n"
                                "   pixel_size_x = %f\n"
                                "   pixel_size_y = %f\n",
                                pixel_size_x, pixel_size_y );
            MessageHandler( "ReadHDFHeaderMosaic", error_str );
        }

        /*******************************************************************/

        /* process each band in grid, storing nlines/nsamples/pixel_size */

        /* get list of fields in grid (again) */
        nfields = GDinqfields( gid, strbuf, tmprank, tmpnumtype );
        fieldlist_ptr = strdup( strbuf );
        if ( fieldlist_ptr == NULL )
        {
            free( gridlist_ptr );
            ErrorHandler( TRUE, "ReadHDFHeaderMosaic", ERROR_MEMORY,
                "Unable to allocate strdup memory for fieldlist." );
            /* return ERROR_MEMORY; */
        }
        fieldlist = fieldlist_ptr;

        /* loop through fields in grid */
        for ( j = 0; j < nfields; j++ )
        {
            /* get name of next field */
            fieldname = fieldlist;
            fieldend = strchr( fieldlist, ',' );
            if ( fieldend != NULL )
            {
                fieldlist = fieldend + 1;
                *fieldend = '\0';
            }

            /* get field info */
            status = GDfieldinfo( gid, fieldname, &rank, dims, &numbertype,
                strbuf );

            /* determine input and output data type for each field */
            switch ( numbertype )
            {
                case DFNT_INT8:
                case DFNT_UINT8:
                case DFNT_INT16:
                case DFNT_UINT16:
                case DFNT_INT32:
                case DFNT_UINT32:
                case DFNT_FLOAT32:
                    break;

                default:
                    numbertype = BAD_DATA_TYPE;
                    sprintf( error_str, "Bad value (%i)", ( int ) numbertype );
                    free( gridlist_ptr );
                    free( fieldlist_ptr );
                    ErrorHandler( TRUE, "ReadHDFHeaderMosaic", ERROR_DATA_TYPE,
                        error_str );
                    /* return ERROR_DATA_TYPE; */
            }

            /* read valid range, background fill, and datum values as
               HDF attributes */
            scalevalue = offsetvalue = minvalue = maxvalue = fillvalue = 0.0;
            GetSDSValues( sd_id, sdsfield, numbertype, &scalevalue,
                &offsetvalue, &minvalue, &maxvalue, &fillvalue, &datumvalue );
            if ( mosaic->projection_type == PROJ_ISIN && datumvalue == 1 )
                mosaic->datum_code = E_NODATUM;
            else
                mosaic->datum_code = datumvalue;

            /* count number of slices in 3-D and 4-D data sets */
            for ( kk = 0; kk < 4; kk++ )
                pos[kk] = -1;
            switch ( rank )
            {
                case 1:         /* 1-D case - user should NOT select this! */
                case 2:         /* 2-D case - just store fieldname */
                    /* keep track of the grid name */
                    strcpy( mosaic->gridname[bandnum], gridname );

                    mosaic->bandinfo[bandnum].name = strdup( fieldname );
                    if ( mosaic->bandinfo[bandnum].name == NULL )
                    {
                        sprintf( error_str, "band name memory allocation" );
                        free( gridlist_ptr );
                        free( fieldlist_ptr );
                        ErrorHandler( TRUE, "ReadHDFHeaderMosaic",
                            ERROR_MEMORY, error_str );
                        /* return ERROR_MEMORY; */
                    }

                    /* store nlines, nsamples, pixel size, datatype for each
                       band */
                    mosaic->bandinfo[bandnum].nlines = ydimsize;
                    mosaic->bandinfo[bandnum].nsamples = xdimsize;
                    if ( rank == 1 )    /* 1-D case */
                        mosaic->bandinfo[bandnum].nsamples = 1;
                    mosaic->bandinfo[bandnum].pixel_size = pixel_size_y;
                    mosaic->bandinfo[bandnum].input_datatype =
                        mosaic->bandinfo[bandnum].output_datatype = numbertype;
                    mosaic->bandinfo[bandnum].scale_factor = scalevalue;
                    mosaic->bandinfo[bandnum].offset = offsetvalue;
                    mosaic->bandinfo[bandnum].min_value = minvalue;
                    mosaic->bandinfo[bandnum].max_value = maxvalue;
                    mosaic->bandinfo[bandnum].background_fill = fillvalue;

                    mosaic->bandinfo[bandnum].fieldnum = sdsfield;
                    mosaic->bandinfo[bandnum].rank = rank;
                    for ( k = 0, dimname = strtok( strbuf, "," );
                        k < rank && dimname != NULL;
                        k++, dimname = strtok( NULL, "," ) )
                    {
                        if ( !strcmp( dimname, "YDim" ) )
                            pos[0] = k;
                        else
                            pos[1] = k;
                    }
                    for ( kk = 0; kk < 4; kk++ )
                        mosaic->bandinfo[bandnum].pos[kk] = pos[kk];

                    bandnum++;
                    break;

                case 3:         /* 3-D case */
                    /* loop through dimensions and get the positions before
                       they are used later */
                    for ( k = 0, dimname = strtok( strbuf, "," );
                        k < rank && dimname != NULL;
                        k++, dimname = strtok( NULL, "," ) )
                    {
                        if ( !strcmp( dimname, "YDim" ) )
                            pos[0] = k;
                        else if ( !strcmp( dimname, "XDim" ) )
                            pos[1] = k;
                        else
                            pos[2] = k;

                        if ( strcmp( dimname, "XDim" ) &&
                             strcmp( dimname, "YDim" ) )
                        {
                            name3d = strdup( dimname );
                            dim3d = dims[k];
                        }
                    }

                    /* use the 3d name to store band names according to our
                       3-D slice naming conventions */
                    for ( m = 1; m <= dim3d; m++ )
                    {
                        /* keep track of the grid name */
                        strcpy( mosaic->gridname[bandnum], gridname );

                        sprintf( strbuf2, "%s.%s_%.2i", fieldname, name3d, m );

                        /* HDF_EOS library sets field name length limit
                         * of 57 in GDapi.c GDdeffield() function.  If the
                         * output file format is HDF, then we need to limit
                         * the size of the SDS names.
                         */
                        if ( mosaic->filetype == HDFEOS )
                        {
                            /* if it's too long try a shortened form */
                            if ( strlen( strbuf2 ) > 57 )
                                sprintf( strbuf2, "%s.Dim3_%.2i", fieldname,
                                    m );

                            /* if it's still too long truncate the name */
                            if ( strlen( strbuf2 ) > 57 )
                                sprintf( strbuf2, "%.52s.3_%.2i", fieldname,
                                    m );
                        }

                        mosaic->bandinfo[bandnum].name = strdup( strbuf2 );
                        if ( mosaic->bandinfo[bandnum].name == NULL )
                        {
                            sprintf( error_str, "band name memory allocation" );
                            free( gridlist_ptr );
                            free( fieldlist_ptr );
                            ErrorHandler( TRUE, "ReadHDFHeaderMosaic",
                                ERROR_MEMORY, error_str );
                            /* return ERROR_MEMORY; */
                        }

                        /* store nlines, nsamples, pixel size, datatype for
                           each band */
                        mosaic->bandinfo[bandnum].nlines = ydimsize;
                        mosaic->bandinfo[bandnum].nsamples = xdimsize;
                        mosaic->bandinfo[bandnum].pixel_size = pixel_size_y;
                        mosaic->bandinfo[bandnum].input_datatype =
                            mosaic->bandinfo[bandnum].output_datatype =
                            numbertype;
                        mosaic->bandinfo[bandnum].scale_factor = scalevalue;
                        mosaic->bandinfo[bandnum].offset = offsetvalue;
                        mosaic->bandinfo[bandnum].min_value = minvalue;
                        mosaic->bandinfo[bandnum].max_value = maxvalue;
                        mosaic->bandinfo[bandnum].background_fill = fillvalue;

                        mosaic->bandinfo[bandnum].fieldnum = sdsfield;
                        mosaic->bandinfo[bandnum].rank = rank;
                        for ( kk = 0; kk < 4; kk++ )
                            mosaic->bandinfo[bandnum].pos[kk] = pos[kk];
                        bandnum++;
                    }

                    break;

                case 4:         /* 4-D case */
                    /* loop through dimensions to get names and sizes of
                       dimensions */
                    dim3d = -1;
                    for ( k = 0, dimname = strtok( strbuf, "," );
                        k < rank && dimname != NULL;
                        k++, dimname = strtok( NULL, "," ) )
                    {
                        if ( !strcmp( dimname, "YDim" ) )
                            pos[0] = k;
                        else if ( !strcmp( dimname, "XDim" ) )
                            pos[1] = k;
                        else if ( pos[2] == -1 )
                            pos[2] = k;
                        else
                            pos[3] = k;

                        if ( strcmp( dimname, "XDim" ) &&
                             strcmp( dimname, "YDim" ) )
                        {
                            if ( dim3d < 0 )
                            {
                                name3d = strdup( dimname );
                                dim3d = dims[k];
                            }
                            else
                            {
                                name4d = strdup( dimname );
                                dim4d = dims[k];
                            }
                        }
                    }

                    /* store band names according to our 4-D slice naming
                       conventions */
                    for ( n = 1; n <= dim4d; n++ )
                        for ( m = 1; m <= dim3d; m++ )
                        {
                            /* keep track of the grid name */
                            strcpy( mosaic->gridname[bandnum], gridname );

                            sprintf( strbuf2, "%s.%s_%.2i.%s_%.2i", fieldname,
                                name3d, m, name4d, n );

                            /* HDF_EOS library sets field name length limit
                             * of 57 in GDapi.c GDdeffield() function.  If the
                             * output file format is HDF, then we need to limit
                             * the size of the SDS names.
                             */
                            if ( mosaic->filetype == HDFEOS )
                            {
                                /* if it's too long try a shortened form */
                                if ( strlen( strbuf2 ) > 57 )
                                    sprintf( strbuf2, "%s.Dim3_%.2i.Dim4_%.2i",
                                        fieldname, m, n );

                                /* if it's still too long, truncate the field
                                   name */
                                if ( strlen( strbuf2 ) > 57 )
                                    sprintf( strbuf2, "%.47s.3_%.2i.4_%.2i",
                                        fieldname, m, n );
                            }

                            mosaic->bandinfo[bandnum].name = strdup( strbuf2 );
                            if ( mosaic->bandinfo[bandnum].name == NULL )
                            {
                                sprintf( error_str,
                                    "bandinfo.name memory allocation" );
                                free( gridlist_ptr );
                                free( fieldlist_ptr );
                                ErrorHandler( TRUE, "ReadHDFHeaderMosaic",
                                    ERROR_MEMORY, error_str );
                                /* return ERROR_MEMORY; */
                            }

                            /* store nlines, nsamples, pixel size, datatype
                               for each band */
                            mosaic->bandinfo[bandnum].nlines = ydimsize;
                            mosaic->bandinfo[bandnum].nsamples = xdimsize;
                            mosaic->bandinfo[bandnum].pixel_size = pixel_size_y;                            mosaic->bandinfo[bandnum].input_datatype =
                                mosaic->bandinfo[bandnum].output_datatype =
                                numbertype;
                            mosaic->bandinfo[bandnum].scale_factor = scalevalue;                            mosaic->bandinfo[bandnum].offset = offsetvalue;
                            mosaic->bandinfo[bandnum].min_value = minvalue;
                            mosaic->bandinfo[bandnum].max_value = maxvalue;
                            mosaic->bandinfo[bandnum].background_fill =
                                fillvalue;

                            mosaic->bandinfo[bandnum].fieldnum = sdsfield;
                            mosaic->bandinfo[bandnum].rank = rank;
                            for ( kk = 0; kk < 4; kk++ )
                                mosaic->bandinfo[bandnum].pos[kk] = pos[kk];

                            bandnum++;
                        }
                    break;

                default:
                    sprintf( error_str, "MRT can only handle 2-D, 3-D, and "
                        "4-D data (field %s, rank %i)", fieldname, (int) rank );
                    free( gridlist_ptr );
                    free( fieldlist_ptr );
                    ErrorHandler( TRUE, "ReadHDFHeaderMosaic",
                        ERROR_READ_INPUTHEADER, error_str );
                    /* return ERROR_READ_INPUTHEADER; */
            }

            sdsfield++;
        }

        /*******************************************************************/

        /* close grid */
        GDdetach( gid );

        /* free fieldlist */
        free( fieldlist_ptr );
        fieldlist_ptr = fieldlist = NULL;
    }

    /* free the gridlist */
    free( gridlist_ptr );
    gridlist_ptr = gridlist = NULL;

    /*******************************************************************/

    /* convert the input projection coords to lat/long for the four corner
       points */
    use_bound = FALSE;
    modis_tile = FALSE;
    for ( i = 0; i < 4; i++ )
    {
        /* Convert corner points from projection coordinates to lat/long.
           If GCTP_ERANGE is returned from GetInputGeoCorner, then use the
           bounding rectangle coordinates for all corner points. */
        status = GetInputGeoCornerMosaic( mosaic,
             mosaic->proj_image_extent[i][0], mosaic->proj_image_extent[i][1],
             &mosaic->ll_image_extent[i][0], &mosaic->ll_image_extent[i][1] );
        if ( status == GCTP_ERANGE || status == GCTP_IN_BREAK )
        {
            /* If any corners fall outside the bounds, then use the bounding
               rectangle coordinates for all corner points. If dealing with
               SIN or ISIN then just bound the longitude at -180 and use the
               latitude from GCTP. */
            use_bound = TRUE;
            if (mosaic->projection_type == PROJ_SIN ||
                mosaic->projection_type == PROJ_ISIN)
            {
                sprintf( logmsg, "ReadHDFHeaderMosaic : "
                    "Corner point falls outside the bounds of "
                    "the input projection.  The rectangle will be bounded "
                    "at -180 or 180 degrees latitude." );
                LogHandler( logmsg );

                /* If there was an issue inverse mapping this coordinate
                   then the coordinate likely wrapped around to the other
                   side of the Earth.  We want to bound at 180 and use the
                   opposite sign of the wrap-around longitude value. */
                modis_tile = TRUE;
                if (mosaic->ll_image_extent[i][1] > 0)
                    mosaic->ll_image_extent[i][1] = -BOUND_LONG;
                else
                    mosaic->ll_image_extent[i][1] = BOUND_LONG;
            }
            else
            {
                sprintf( logmsg, "ReadHDFHeaderMosaic : "
                    "At least one corner point falls outside the bounds of "
                    "the input projection.  The bounding rectangular "
                    "coordinates from the metadata will be used for the "
                    "lat/long in the header." );
                LogHandler( logmsg );
                break;  /* just break out and use bounding coords */
            }
        }
    }

    /* do we need to use the bounding rectangular coords?  bounding values
       have already been determined for the Modis tiles, so only worry about
       non-Modis tiles */
    if ( use_bound && !modis_tile )
    {
        if ( !all_bounding_coords_present )
        {
            ErrorHandler( TRUE, "ReadHDFHeaderMosaic", ERROR_GENERAL,
                "The bounding rectangular coordinates were not "
                "available in the Archive Metadata. This product "
                "spans outside the bounds of the input projection "
                "and the bounding coordinates are required to "
                "process this product.\n" );
        }

        /* Set the lat/longs of the image based on the bounding
           rectangular coordinates */
        mosaic->ll_image_extent[UL][0] = mosaic->north_bound;
        mosaic->ll_image_extent[UL][1] = mosaic->west_bound;
        mosaic->ll_image_extent[UR][0] = mosaic->north_bound;
        mosaic->ll_image_extent[UR][1] = mosaic->east_bound;
        mosaic->ll_image_extent[LL][0] = mosaic->south_bound;
        mosaic->ll_image_extent[LL][1] = mosaic->west_bound;
        mosaic->ll_image_extent[LR][0] = mosaic->south_bound;
        mosaic->ll_image_extent[LR][1] = mosaic->east_bound;
    }

        /*******************************************************************/

    /* finish up */

    /* close the HDF file */
    status = SDend( sd_id );

    /* close the HDF-EOS file */
    GDclose( fid );

    return MRT_NO_ERROR;
}

/******************************************************************************

MODULE:  GetSDSValues

PURPOSE:  Fills in ModisDescriptor with valid range, background fill, and
          datum (if an MRT output product) values.  These are read from
          HDF-EOS file as HDF attributes.

RETURN VALUE:  int      (error code)

Value           Description
-----           -----------

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         04/01  John Weiss             Modularize code and set proper
                                       background fill defaults.
         01/02  Gail Schmidt           Read the datum values to support
                                       products output from the MRT.

NOTES:

******************************************************************************/

int GetSDSValues
(
    int32 sd_id,
    int band,
    int32 numbertype,
    double *scalevalue,
    double *offsetvalue,
    double *minvalue,
    double *maxvalue,
    double *fillvalue,
    int *datumvalue
)

{
    int32 sds_id = -1;
    int32 range_status, fill_status, cal_status;
    double cal, cal_error, offset, offset_error;
    int32 num_type;
    char error_str[SMALL_STRING];       /* SMALL_STRING defined in shared_resample.h */

    /* specific variable types to replace the void * types */
    unsigned char ucmin_value, ucmax_value, ucbackground_fill;
    char cmin_value, cmax_value, cbackground_fill;
    unsigned short usmin_value, usmax_value, usbackground_fill;
    short smin_value, smax_value, sbackground_fill;
    unsigned long ulmin_value, ulmax_value, ulbackground_fill;
    long lmin_value, lmax_value, lbackground_fill;
    float fmin_value, fmax_value, fbackground_fill;

    /* open the HDF SDS */
    sds_id = SDselect( sd_id, band );
    if ( sds_id < 0 )
    {
        sprintf( error_str, "unable to select SDS band %i", band );
        ErrorHandler( FALSE, "ReadHDFHeader", ERROR_READ_INPUTHEADER,
            error_str );
        return ERROR_READ_INPUTHEADER;
    }

    /* store scale factor and offset */
    cal_status = SDgetcal( sds_id, &cal, &cal_error, &offset, &offset_error,
        &num_type );
    if ( cal_status != -1 )
    {
        *scalevalue = cal;
        *offsetvalue = offset;
    }

    /* check for the datum value (actually it is most likely that only
       HDF-EOS products output by the MRT will have the datum value
       available). the datumvalue is set to E_NODATUM if it is not in the
       HDF-EOS file */
    getdatumvalue( sds_id, datumvalue );

    /* store valid range and background fill */
    switch ( numbertype )
    {
        case DFNT_INT8:
            /* get range and background fill value */
            range_status = SDgetrange( sds_id, ( void * ) &cmax_value,
                ( void * ) &cmin_value );
            fill_status = SDgetfillvalue( sds_id, (void *) &cbackground_fill );

            /* check for missing valid range values */
            if ( range_status != -1 )
            {
                *minvalue = cmin_value;
                *maxvalue = cmax_value;
            }
            else
            {
                *minvalue = *maxvalue = 0.0;
            }

            /* check for missing background fill value */
            /* use value outside valid range if background fill isn't present */
            if ( fill_status != -1 )
            {
                *fillvalue = cbackground_fill;
            }
            else
            {
                if ( range_status == -1 )   /* no valid range info? use min */
                    *fillvalue = ( float ) MRT_INT8_MIN;
                else if ( *minvalue > ( float ) MRT_INT8_MIN )
                    *fillvalue = *minvalue - 1.0;
                else if ( *maxvalue < ( float ) MRT_INT8_MAX )
                    *fillvalue = *maxvalue + 1.0;
                else
                    *fillvalue = ( float ) MRT_INT8_MIN;
            }

            break;

        case DFNT_UINT8:
            /* get range and background fill value */
            range_status = SDgetrange( sds_id, ( void * ) &ucmax_value,
                ( void * ) &ucmin_value );
            fill_status = SDgetfillvalue( sds_id, (void *) &ucbackground_fill );

            /* check for missing valid range values */
            if ( range_status != -1 )
            {
                *minvalue = ucmin_value;
                *maxvalue = ucmax_value;
            }
            else
            {
                *minvalue = *maxvalue = 0.0;
            }

            /* check for missing background fill value */
            /* use value outside valid range if background fill isn't present */
            if ( fill_status != -1 )
            {
                *fillvalue = ucbackground_fill;
            }
            else
            {
                if ( range_status == -1 )   /* no valid range info? use min */
                    *fillvalue = 0.0;
                else if ( *minvalue > 0.0 )
                    *fillvalue = *minvalue - 1.0;
                else if ( *maxvalue < ( float ) MRT_UINT8_MAX )
                    *fillvalue = *maxvalue + 1.0;
                else
                    *fillvalue = 0.0;
            }

            break;

        case DFNT_INT16:
            /* get range and background fill value */
            range_status = SDgetrange( sds_id, ( void * ) &smax_value,
                ( void * ) &smin_value );
            fill_status = SDgetfillvalue( sds_id, (void *) &sbackground_fill );

            /* check for missing valid range values */
            if ( range_status != -1 )
            {
                *minvalue = smin_value;
                *maxvalue = smax_value;
            }
            else
            {
                *minvalue = *maxvalue = 0.0;
            }

            /* check for missing background fill value */
            /* use value outside valid range if background fill isn't present */
            if ( fill_status != -1 )
            {
                *fillvalue = sbackground_fill;
            }
            else
            {
                if ( range_status == -1 )   /* no valid range info? use min */
                    *fillvalue = ( float ) MRT_INT16_MIN;
                else if ( *minvalue > ( float ) MRT_INT16_MIN )
                    *fillvalue = *minvalue - 1.0;
                else if ( *maxvalue < ( float ) MRT_INT16_MAX )
                    *fillvalue = *maxvalue + 1.0;
                else
                    *fillvalue = ( float ) MRT_INT16_MIN;
            }

            break;

        case DFNT_UINT16:
            /* get range and background fill value */
            range_status = SDgetrange( sds_id, ( void * ) &usmax_value,
                ( void * ) &usmin_value );
            fill_status = SDgetfillvalue( sds_id, (void *) &usbackground_fill );

            /* check for missing valid range values */
            if ( range_status != -1 )
            {
                *minvalue = usmin_value;
                *maxvalue = usmax_value;
            }
            else
            {
                *minvalue = *maxvalue = 0.0;
            }

            /* check for missing background fill value */
            /* use value outside valid range if background fill isn't present */
            if ( fill_status != -1 )
            {
                *fillvalue = usbackground_fill;
            }
            else
            {
                if ( range_status == -1 )   /* no valid range info? use min */
                    *fillvalue = 0.0;
                else if ( *minvalue > 0.0 )
                    *fillvalue = *minvalue - 1.0;
                else if ( *maxvalue < ( float ) MRT_UINT16_MAX )
                    *fillvalue = *maxvalue + 1.0;
                else
                    *fillvalue = 0.0;
            }

            break;

        case DFNT_INT32:
            /* get range and background fill value */
            range_status = SDgetrange( sds_id, ( void * ) &lmax_value,
                ( void * ) &lmin_value );
            fill_status = SDgetfillvalue( sds_id, (void *) &lbackground_fill );

            /* check for missing valid range values */
            if ( range_status != -1 )
            {
                *minvalue = lmin_value;
                *maxvalue = lmax_value;
            }
            else
            {
                *minvalue = *maxvalue = 0.0;
            }

            /* check for missing background fill value */
            /* use value outside valid range if background fill isn't present */
            if ( fill_status != -1 )
            {
                *fillvalue = lbackground_fill;
            }
            else
            {
                if ( range_status == -1 )   /* no valid range info? use min */
                    *fillvalue = ( float ) MRT_INT32_MIN;
                else if ( *minvalue > ( float ) MRT_INT32_MIN )
                    *fillvalue = *minvalue - 1.0;
                else if ( *maxvalue < ( float ) MRT_INT32_MAX )
                    *fillvalue = *maxvalue + 1.0;
                else
                    *fillvalue = ( float ) MRT_INT32_MIN;
            }

            break;

        case DFNT_UINT32:
            /* get range and background fill value */
            range_status = SDgetrange( sds_id, ( void * ) &ulmax_value,
                ( void * ) &ulmin_value );
            fill_status = SDgetfillvalue( sds_id, (void *) &ulbackground_fill );

            /* check for missing valid range values */
            if ( range_status != -1 )
            {
                *minvalue = ulmin_value;
                *maxvalue = ulmax_value;
            }
            else
            {
                *minvalue = *maxvalue = 0.0;
            }

            /* check for missing background fill value */
            /* use value outside valid range if background fill isn't present */
            if ( fill_status != -1 )
            {
                *fillvalue = ulbackground_fill;
            }
            else
            {
                if ( range_status == -1 )   /* no valid range info? use min */
                    *fillvalue = 0.0;
                else if ( *minvalue > 0.0 )
                    *fillvalue = *minvalue - 1.0;
                else if ( *maxvalue < ( float ) MRT_UINT32_MAX )
                    *fillvalue = *maxvalue + 1.0;
                else
                    *fillvalue = 0.0;
            }

            break;

        case DFNT_FLOAT32:
            /* get range and background fill value */
            range_status = SDgetrange( sds_id, ( void * ) &fmax_value,
                ( void * ) &fmin_value );
            fill_status = SDgetfillvalue( sds_id, (void *) &fbackground_fill );

            /* check for missing valid range values */
            if ( range_status != -1 )
            {
                *minvalue = fmin_value;
                *maxvalue = fmax_value;
            }
            else
            {
                *minvalue = *maxvalue = 0.0;
            }

            /* check for missing background fill value */
            /* use value outside valid range if background fill isn't present */
            if ( fill_status != -1 )
            {
                *fillvalue = fbackground_fill;
            }
            else
            {
                if ( range_status == -1 )   /* no valid range info? use min */
                    *fillvalue = -MRT_FLOAT4_MAX;
                else if ( *minvalue > -MRT_FLOAT4_MAX + 1.0 )
                    *fillvalue = *minvalue - 1.0;
                else if ( *maxvalue < MRT_FLOAT4_MAX - 1.0 )
                    *fillvalue = *maxvalue + 1.0;
                else
                    *fillvalue = -MRT_FLOAT4_MAX;
            }

            break;
    }

    /* terminate access to the SDS */
    SDendaccess( sds_id );

    return 0;
}

/******************************************************************************

MODULE:  getdatumvalue

PURPOSE:  Reads the datum value (if available) from the HDF-EOS file as
          an HDF attribute.

RETURN VALUE:  void

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         01/02  Gail Schmidt           Original Development

NOTES:

******************************************************************************/

void getdatumvalue
(
    int32 sds_id,     /* I: sds_id to read */
    int *datumvalue   /* O: datum value (= E_NODATUM if not available) */
)

{
    char attr_name[1024], sds_name[1024];
    int32 data_type, n_values;
    int32 nattr, attr_index;
    int32 rank;
    int32 dims[10];
    int16 data_buffer;

    /* initialize the datum value to E_NODATUM */
    *datumvalue = E_NODATUM;

    /* get various SDS info */
    SDgetinfo( sds_id, sds_name, &rank, dims, &data_type, &nattr );

    /* loop through the SDS attributes looking for the datum attribute */
    for ( attr_index = 0; attr_index < nattr; attr_index++ )
    {
        /* get SDS attribute info */
        SDattrinfo( sds_id, attr_index, attr_name, &data_type, &n_values );

        /* if this is the datum, then read it */
        if ( !strcmp( attr_name, "HorizontalDatumName" ) )
        {
            /* get datum value */
            SDreadattr( sds_id, attr_index, &data_buffer );
            *datumvalue = data_buffer;
        }
    }
}
