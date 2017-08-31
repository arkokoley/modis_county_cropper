/******************************************************************************

FILE:  hdf_io.c

PURPOSE:  Read and write rows from HDF-EOS files

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

MODULE:  ReadRowHdfEos

PURPOSE:  Read a row from an HDF-EOS file

RETURN VALUE:
Type = int
Value           Description
-----           -----------
TRUE            Success
FALSE           Failure

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  John Weiss             Original Development
         01/01  John Rishea            Standardized formatting

NOTES:

******************************************************************************/
int ReadRowHdfEos
(
    FileDescriptor *file,	/* I/O:  file to read */
    int row			/* I:  row number to read */
)

{
    int32 status;		/* error status */
    int32 start[4], edge[4];	/* limits of read */
    HdfEosFD *hdfptr = ( HdfEosFD * ) file->fileptr;	/* cast to HDF FD */

    /* set up GDreadfield() call */
    switch ( hdfptr->rank )
    {
	case 4:
	    start[hdfptr->pos[3]] = hdfptr->dim4; /* 4Dim: start[3] = cube#; */
	    edge[hdfptr->pos[3]] = 1;		  /* 4Dim: edge[3] = 1; */
	case 3:
	    start[hdfptr->pos[2]] = hdfptr->dim3; /* 3Dim: start[2] = slice#; */
	    edge[hdfptr->pos[2]] = 1;		  /* 3Dim: edge[2] = 1; */
	case 2:
	    start[hdfptr->pos[1]] = 0;		  /* XDim: start[1] = 0; */
	    edge[hdfptr->pos[1]] = file->ncols;	  /* XDim: edge[1] = ncols; */
	    start[hdfptr->pos[0]] = row;	  /* YDim: start[0] = row#; */
	    edge[hdfptr->pos[0]] = 1;		  /* YDim: edge[0] = 1; */
	    break;
    }

    /* read row of data */
    /* according to Robert Wolfe, HDF library handles byte order */
    status = GDreadfield( hdfptr->gid, hdfptr->currfield, start, NULL,
                          edge, file->rowbuffer );
    if ( status == -1 )
        return ( FALSE );
    else
        return ( TRUE );
}

/******************************************************************************

MODULE:  WriteRowHdfEos

PURPOSE:  Write a row to an HDF-EOS file

RETURN VALUE:
Type = int
Value           Description
-----           -----------
TRUE            Success
FALSE           Failure

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         06/00  John Weiss             Original Development
         01/01  John Rishea            Standardized formatting
         01/01  John Rishea            Removed testing code

NOTES:

******************************************************************************/
int WriteRowHdfEos
(
    FileDescriptor *file,	/* I: file to write */
    int row			/* I: row number to write */
)

{
    int32 status;		/* error status */
    int32 start[2], edge[2];	/* limits of write */
    HdfEosFD *hdfptr = ( HdfEosFD * ) file->fileptr;	/* cast to HDF FD */

    /* set up GDwritefield() call */
    start[0] = row;
    start[1] = 0;
    edge[0] = 1;
    edge[1] = file->ncols;

    /* write row of data */
    /* according to Robert Wolfe, HDF library handles byte order */
    status = GDwritefield( hdfptr->gid, hdfptr->currfield, start, NULL,
                           edge, file->rowbuffer );
    if ( status == -1 )
        return ( FALSE );
    else
        return ( TRUE );
}
