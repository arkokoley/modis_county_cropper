/******************************************************************************

FILE:  hdf_oc.c

PURPOSE:  Open and close HDF-EOS files

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  John Weiss             Original Development
         06/01  John Weiss             Add 3-D/4-D data support.

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/

#include "shared_resample.h"

/******************************************************************************

MODULE:  OpenHdfEosFile

PURPOSE:  Open an HDF-EOS file for reading or writing

RETURN VALUE:
Type = HdfEosFD

Value           	Description
-----           	-----------
HdfEosFD*		An opened HDF file
NULL			Failure

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  John Weiss             Original Development
         01/01  John Rishea            Standardized formatting

NOTES:

******************************************************************************/
HdfEosFD *OpenHdfEosFile
(
    char *input_filename,      /* I:  input filename for reading */
    char *output_filename,     /* I:  output filename for writing */
    FileOpenType mode,	       /* I:  reading or writing */
    int *status		       /* I/O:  error code status */
)
 
{
    char str[LARGE_STRING],         /* LARGE_STRING defined in shared_resample.h */
         gridlist[HUGE_STRING];	    /* HUGE_STRING defined in shared_resample.h */
    int fid, ngrids;
    int32 bufsiz;
    HdfEosFD *hdfptr = NULL;

    /* open input HDF-EOS file */
    if ( mode == FILE_READ_MODE )
    {
	/* open file for reading */
	fid = GDopen( input_filename, DFACC_READ );

	/* check if file was successfully opened */
	if ( fid == -1 )
	{
	    sprintf( str, "Unable to open %s for reading\n", input_filename );
	    ErrorHandler( TRUE, "OpenHdfEosFile", ERROR_OPEN_INPUTIMAGE, str );
	    *status = ERROR_OPEN_INPUTIMAGE;
	    return NULL;
	}

	/* create HDF-EOS file pointer */
	hdfptr = ( HdfEosFD * ) calloc( 1, sizeof( HdfEosFD ) );
	if ( !hdfptr )
	{
	    sprintf( str, "Unable to create HdfEosFD for %s\n",
                input_filename );
	    ErrorHandler( TRUE, "OpenHdfEosFile", ERROR_OPEN_INPUTIMAGE, str );
	    *status = ERROR_OPEN_INPUTIMAGE;
	    return NULL;
	}

	/* multi-grid bug fix by JMW 06/06/01 */
	/* see if there are any grids in the file */
	ngrids = GDinqgrid( input_filename, gridlist, &bufsiz );
	if ( ngrids < 1 || strlen( gridlist ) < 1 )
	{
	    sprintf( str, "Unable to find any gridnames in %s\n",
                input_filename );
	    ErrorHandler( TRUE, "OpenHdfEosFile", ERROR_OPEN_INPUTIMAGE, str );
	    *status = ERROR_OPEN_INPUTIMAGE;
	    return NULL;
	}

	/* fill in fields in hdfptr */
	hdfptr->fid = fid;
	hdfptr->gid = -1;
	hdfptr->ngrids = ngrids;
	hdfptr->gridlist = strdup( gridlist );
	if ( hdfptr->gridlist == NULL )
	{
	    sprintf( str, "Unable to allocate memory for gridlist for %s\n",
		input_filename );
	    ErrorHandler( TRUE, "OpenHdfEosFile", ERROR_MEMORY, str );
	    *status = ERROR_MEMORY;
	    return NULL;
	}

	hdfptr->currgrid = NULL;
	hdfptr->fieldlist = NULL;
	hdfptr->currfield = NULL;
    }

    /* open output HDF-EOS file */
    else			/* if ( mode == FILE_WRITE_MODE ) */
    {
	/* open HDF-EOS file for output */
	fid = GDopen( output_filename, DFACC_CREATE );

	/* check if file was successfully opened */
	if ( fid == -1 )
	{
	    sprintf( str, "Unable to open %s for writing", output_filename );
	    ErrorHandler( TRUE, "OpenHdfEosFile", ERROR_OPEN_OUTPUTIMAGE, str );
	    *status = ERROR_OPEN_OUTPUTIMAGE;
	    return NULL;
	}

	/* create HDF-EOS file pointer */
	hdfptr = ( HdfEosFD * ) calloc( 1, sizeof( HdfEosFD ) );
	if ( !hdfptr )
	{
	    sprintf( str, "Unable to create HdfEosFD for %s", output_filename );
	    ErrorHandler( TRUE, "OpenHdfEosFile", ERROR_OPEN_OUTPUTIMAGE, str );
	    *status = ERROR_OPEN_OUTPUTIMAGE;
	    return NULL;
	}

	/* fill in fields in hdfptr */
	hdfptr->fid = fid;
	hdfptr->gid = -1;
	hdfptr->ngrids = 0;
	hdfptr->gridlist = NULL;
	hdfptr->currgrid = NULL;
	hdfptr->fieldlist = NULL;
	hdfptr->currfield = NULL;
    }

    /* finish up */
    return hdfptr;
}

/******************************************************************************

MODULE:  CloseHdfEos

PURPOSE:  Close an HDF-EOS file

RETURN VALUE:
Type = void

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         02/03  Gail Schmidt           Original Development

NOTES:

******************************************************************************/
void CloseHdfEos
(
    HdfEosFD *hdfptr         /* I: hdf file pointer to cleanup and close */
)
 
{
    /* detach from the grid */
    if ( hdfptr->gid >= 0 )
    {
        GDdetach( hdfptr->gid );
        hdfptr->gid = -1;
    }

    /* close the file id */
    if ( hdfptr->fid >= 0 )
    {
        GDclose( hdfptr->fid );
        hdfptr->fid = -1;
    }

    /* free the fields in hdfptr */
    if ( hdfptr->gridlist != NULL )
        free( hdfptr->gridlist );
    if ( hdfptr->currgrid != NULL )
        free( hdfptr->currgrid );
    if ( hdfptr->fieldlist != NULL )
        free( hdfptr->fieldlist );
    if ( hdfptr->currfield != NULL )
        free( hdfptr->currfield );

    /* free hdfptr itself */
    free( hdfptr );
}

/******************************************************************************

MODULE:  MakeHdfEosFD

PURPOSE:  Create an HDF-EOS file descriptor

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
FileDescriptor *MakeHdfEosFD
(
    ModisDescriptor *modis,	/* I:  session info */
    HdfEosFD *hdfptr,		/* I:  HDF file descriptor */
    FileOpenType mode,		/* I:  read or write */
    int bandnum,		/* I:  band number to read or write */
    int *status			/* I/O:  error status code */
)

{
    int i;
    FileDescriptor *file = NULL;
    char filename[LARGE_STRING],	/* LARGE_STRING defined in resample.h */
         str[LARGE_STRING];

    /* input HDF-EOS file */
    if ( mode == FILE_READ_MODE )
    {
	/* create file descriptor */
	strcpy( filename, modis->input_filename );
	file = CreateFileDescriptor( modis, bandnum, mode, filename );
	if ( !file )
	{
	    sprintf( str, "Unable to create FileDescriptor for %s\n", filename );
	    ErrorHandler( TRUE, "OpenHdfEosFile", ERROR_OPEN_INPUTIMAGE, str );
	    *status = ERROR_OPEN_INPUTIMAGE;
	    return NULL;
	}

	/* store rank and dimension field position info */
	hdfptr->rank = modis->bandinfo[bandnum].rank;
	for ( i = 0; i < 4; i++ )
	    hdfptr->pos[i] = modis->bandinfo[bandnum].pos[i];
    }

    /* output HDF-EOS file */
    else			/* if ( mode == FILE_WRITE_MODE ) */
    {
	/* create file descriptor */
	strcpy( filename, modis->output_filename );
	file = CreateFileDescriptor( modis, bandnum, mode, filename );
	if ( !file )
	{
	    sprintf( str, "Unable to create FileDescriptor for %s\n", filename );
	    ErrorHandler( TRUE, "OpenHdfEosFile", ERROR_OPEN_OUTPUTIMAGE, str );
	    *status = ERROR_OPEN_OUTPUTIMAGE;
	    return NULL;
	}
    }

    /* finish up */
    file->fileptr = hdfptr;
    return ( file );
}

/******************************************************************************

MODULE:  CloseHdfEosFile

PURPOSE:  Close an open HDF FD

RETURN VALUE:
Type = int
Value           Description
-----           -----------
TRUE			Success
FALSE			Failure

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  John Weiss             Original Development
         01/01  John Rishea            Standardized formatting 

NOTES:

******************************************************************************/
int CloseHdfEosFile
(
    FileDescriptor *filedescriptor	/* I:  file to close */
)

{
    HdfEosFD *hdfptr = ( HdfEosFD * ) filedescriptor->fileptr;

    switch ( filedescriptor->fileopentype )
    {
	case FILE_READ_MODE:
	    GDdetach( hdfptr->gid );
            hdfptr->gid = -1;
	    GDclose( hdfptr->fid );
            hdfptr->fid = -1;
	    DestroyFileDescriptor( filedescriptor );
	    break;

	case FILE_WRITE_MODE:
	    GDdetach( hdfptr->gid );
            hdfptr->gid = -1;
	    GDclose( hdfptr->fid );
            hdfptr->fid = -1;
	    DestroyFileDescriptor( filedescriptor );
	    break;

	default:
	    ErrorHandler( TRUE, "CloseHdfEosFile", ERROR_GENERAL,
                "Bad Open Type" );
	    return ( FALSE );
    }

    return ( TRUE );
}

/******************************************************************************

MODULE:  GetHdfEosField

PURPOSE:  Get next HDF-EOS field by number

RETURN VALUE:
Type = int
Value           Description
-----           -----------
status			See mrt_error.h for a complete list of codes

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  John Weiss             Original Development
         01/01  John Rishea            Standardized formatting
         01/01  John Rishea            Removed testing code
         05/02  Gail Schmidt           Keep the original grid/Vgroup name
                                       if input file is HDF-EOS

NOTES:

******************************************************************************/
int GetHdfEosField
(
    ModisDescriptor *modis,     /* I:   session info */
    HdfEosFD *hdfptr,		/* I/O: file to get field number */
    int fieldnum		/* I:   field to find */
)

{
    int k, m, n;
    int bandnum;
    int dim3 = 0, dim4 = 0, dim3d = 0, dim4d = 0;
    int32 rank, numbertype;
    int32 dims[10];
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
    GDinqfields( hdfptr->gid, strbuf, NULL, NULL );
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
	    GDinqfields( hdfptr->gid, strbuf, NULL, NULL );
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
    strcpy( modis->input_gridname, gridname );
    hdfptr->currfield = strdup( fieldname );
    hdfptr->dim3 = dim3;
    hdfptr->dim4 = dim4;

    return MRT_NO_ERROR;
}

