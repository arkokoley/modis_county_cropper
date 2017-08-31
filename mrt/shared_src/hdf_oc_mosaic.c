/******************************************************************************

FILE:  hdf_oc_mosaic.c

PURPOSE:  Open and close HDF-EOS files for mosaicking

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         07/02  Gail Schmidt           Original Development

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/

#include "shared_mosaic.h"

/******************************************************************************

MODULE:  MakeHdfEosFDMosaic

PURPOSE:  Create an HDF-EOS file descriptor for mosaicking

RETURN VALUE:
Type = FileDescriptor
Value           Description
-----           -----------
FileDescriptor	New FD for HDF-EOS
NULL			failure

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  John Weiss             Original Development
         01/01  John Rishea            Standardized formatting

NOTES:

******************************************************************************/
FileDescriptor *MakeHdfEosFDMosaic
(
    MosaicDescriptor *mosaic,	/* I:  session info */
    HdfEosFD *hdfptr,		/* I:  HDF file descriptor */
    FileOpenType mode,		/* I:  read or write */
    int in_bandnum,             /* I:  number of the input band */
    int out_bandnum,            /* I:  number of the output band */
    int *status			/* I/O:  error status code */
)

{
    int i;
    FileDescriptor *file = NULL;
    char filename[LARGE_STRING],	/* LARGE_STRING defined in shared_resample.h */
         str[LARGE_STRING];

    /* input HDF-EOS file */
    if ( mode == FILE_READ_MODE )
    {
	/* create file descriptor */
	strcpy( filename, mosaic->filename );
	file = CreateFileDescriptorMosaic( mosaic, in_bandnum, out_bandnum,
            mode, filename );
	if ( !file )
	{
	    sprintf( str, "Unable to create Mosaic FileDescriptor for %s\n",
                filename );
	    ErrorHandler( TRUE, "MakeHdfEosFileMosaic", ERROR_OPEN_INPUTIMAGE,
                str );
	    *status = ERROR_OPEN_INPUTIMAGE;
	    return NULL;
	}

	/* store rank and dimension field position info */
	hdfptr->rank = mosaic->bandinfo[in_bandnum].rank;
	for ( i = 0; i < 4; i++ )
	    hdfptr->pos[i] = mosaic->bandinfo[in_bandnum].pos[i];
    }

    /* output HDF-EOS file */
    else			/* if ( mode == FILE_WRITE_MODE ) */
    {
	/* create file descriptor */
	strcpy( filename, mosaic->filename );
	file = CreateFileDescriptorMosaic( mosaic, in_bandnum, out_bandnum,
            mode, filename );
	if ( !file )
	{
	    sprintf( str, "Unable to create Mosaic FileDescriptor for %s\n",
                filename );
	    ErrorHandler( TRUE, "MakeHdfEosFileMosaic", ERROR_OPEN_OUTPUTIMAGE,
                str );
	    *status = ERROR_OPEN_OUTPUTIMAGE;
	    return NULL;
	}
    }

    /* finish up */
    file->fileptr = hdfptr;
    return ( file );
}

/******************************************************************************

MODULE:  GetHdfEosFieldMosaic

PURPOSE:  Get next HDF-EOS field by number for mosaicking

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status		See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         07/02  Gail Schmidt           Original Development

NOTES:

******************************************************************************/
int GetHdfEosFieldMosaic
(
    HdfEosFD *hdfptr,		/* I/O: file to get field number */
    int fieldnum		/* I:   field to find */
)

{
    int k, m, n;
    int bandnum;
    int dim3 = 0, dim4 = 0, dim3d = 0, dim4d = 0;
    int32 tmprank[256], tmpnumtype[256];
    int32 rank, numbertype;
    int32 dims[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    char *fieldlist = NULL, *fieldname = NULL, *fieldend = NULL;
    char *gridlist = NULL, *gridname = NULL, *gridend = NULL;
    char strbuf[HUGE_STRING];
    char errstr[SMALL_STRING];
    char *dimname = NULL;

    /* copy grid list */
    gridlist = strdup( hdfptr->gridlist );
    if ( gridlist == NULL )
    {
	ErrorHandler( TRUE, "GeHdfEosField", ERROR_MEMORY,
            "Bad gridlist strdup" );
	return ( ERROR_MEMORY );
    }

    /* get first grid name */
    gridname = gridlist;
    gridend = strchr( gridlist, ',' );
    if ( gridend != NULL )
    {
	gridlist = gridend + 1;
	*gridend = '\0';
    }

    /* if necessary, detach from old grid */
    if ( hdfptr->gid >= 0 )
	GDdetach( hdfptr->gid );

    /* attach to first grid */
    hdfptr->gid = GDattach( hdfptr->fid, gridname );
    if ( hdfptr->gid < 0 )
    {
	sprintf( errstr, "Unable to attach to grid %s", gridname );
	ErrorHandler( TRUE, "GetHdfEosField", ERROR_READ_INPUTIMAGE, errstr );
	return ( ERROR_READ_INPUTIMAGE );
    }

    /* get list of fields in first grid */
    GDinqfields( hdfptr->gid, strbuf, tmprank, tmpnumtype );
    fieldlist = strdup( strbuf );
    if ( fieldlist == NULL )
    {
        ErrorHandler( TRUE, "GetHdfEosField", ERROR_MEMORY,
	    "Unable to allocate strdup memory for fieldlist." );
        return ERROR_MEMORY;
    }

    /* loop through field names to get desired field */
    for ( bandnum = 0; bandnum <= fieldnum; )
    {
	/* if run out of fields, must open a new grid */
	if ( fieldlist == NULL )
	{
	    /* make sure we have a grid */
	    if ( gridlist == NULL )
	    {
		ErrorHandler( TRUE, "GetHdfEosField", ERROR_READ_INPUTIMAGE,
		    "No grids to attach to!" );
		return ( ERROR_READ_INPUTIMAGE );
	    }

	    /* close current grid and open next grid */
	    gridname = gridlist;
	    gridend = strchr( gridlist, ',' );
	    if ( gridend != NULL )
	    {
		gridlist = gridend + 1;
		*gridend = '\0';
	    }
	    else
		gridlist = NULL;

	    /* detach from old grid */
	    GDdetach( hdfptr->gid );

	    /* attach to new grid */
	    hdfptr->gid = GDattach( hdfptr->fid, gridname );
	    if ( hdfptr->gid < 0 )
	    {
		sprintf( errstr, "Unable to attach to grid %s", gridname );
		ErrorHandler( TRUE, "GetHdfEosField", ERROR_READ_INPUTIMAGE,
		    errstr );
		return ( ERROR_READ_INPUTIMAGE );
	    }

	    /* get list of fields in new grid */
	    GDinqfields( hdfptr->gid, strbuf, tmprank, tmpnumtype );
	    fieldlist = strdup( strbuf );
	    if ( fieldlist == NULL )
	    {
	        ErrorHandler( TRUE, "GetHdfEosField", ERROR_MEMORY,
		    "Unable to allocate strdup memory for fieldlist." );
	        return ERROR_MEMORY;
	    }
	}

	/* get next field name from field list */
	fieldname = fieldlist;
	fieldend = strchr( fieldlist, ',' );
	if ( fieldend != NULL )
	{
	    fieldlist = fieldend + 1;
	    *fieldend = '\0';
	}
	else
	    fieldlist = NULL;

	/* get field info */
	GDfieldinfo( hdfptr->gid, fieldname, &rank, dims, &numbertype,
            strbuf );

	/* count number of slices in 3-D and 4-D data sets */
	switch ( rank )
	{
	    case 1:		/* 1-D case - should not happen! */
	    case 2:		/* 2-D case - just store fieldname */
		bandnum++;
		break;

	    case 3:		/* 3-D case */
		/* loop through dimensions */
		for ( k = 0, dimname = strtok( strbuf, "," );
		    k < rank && dimname != NULL;
                    k++, dimname = strtok( NULL, "," ) )
		{
		    if ( strcmp( dimname, "XDim" ) &&
                         strcmp( dimname, "YDim" ) )
		    {
			/* store band names according to our 3-D slice naming
                           conventions */
			for ( m = 0; m < dims[k]; m++ )
			    if ( bandnum <= fieldnum )
			    {
				dim3 = m;
				bandnum++;
			    }
			    else
			    {
				break;
			    }
		    }
		}
		break;

	    case 4:		/* 4-D case */
		/* loop through dimensions to get names and sizes of
                   dimensions */
		dim3d = -1;
		for ( k = 0, dimname = strtok( strbuf, "," );
		      k < rank && dimname != NULL;
                      k++, dimname = strtok( NULL, "," ) )
		{
		    if ( strcmp( dimname, "XDim" ) &&
                         strcmp( dimname, "YDim" ) )
		    {
			if ( dim3d < 0 )
			    dim3d = dims[k];
			else
			    dim4d = dims[k];
		    }
		}

		/* store band names according to our 4-D slice naming
                   conventions */
		for ( n = 0; n < dim4d; n++ )
		    for ( m = 0; m < dim3d; m++ )
			if ( bandnum <= fieldnum )
			{
			    dim3 = m;
			    dim4 = n;
			    bandnum++;
			}
			else
			{
			    goto done;
			}
		done:
		break;
	}
    }

    if ( hdfptr->currgrid )
	free( hdfptr->currgrid );
    if ( hdfptr->currfield )
	free( hdfptr->currfield );

    hdfptr->currgrid = strdup( gridname );
    hdfptr->currfield = strdup( fieldname );
    hdfptr->dim3 = dim3;
    hdfptr->dim4 = dim4;

    return MRT_NO_ERROR;
}
