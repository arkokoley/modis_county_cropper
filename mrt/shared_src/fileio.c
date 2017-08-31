
/******************************************************************************

FILE:  fileio.c

PURPOSE:  Handles reading/writing and data translation

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  Rob Burrell            Original Development
         06/00  John Weiss             Clip 8-bit values to 0-255 in WriteRow
         06/00  John Weiss             Handle signed vs. unsigned ints
         06/00  John Weiss             byte-swapping for endian issues
         06/00  John Weiss             HDF-EOS output
         06/00  John Weiss             Remove multifile/HDF-EOS I/O routines
         06/00  John Weiss             Clip all int values on output
         01/01  John Rishea            Standardized formatting
         01/01  John Rishea            Moved local prototypes to loc_prot.h
         04/02  Gail Schmidt           Changed data pointers from floats to
                                       doubles

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/
#include "mrt_dtype.h"
#include "shared_resample.h"
#include <float.h>

/******************************************************************************

MODULE:  ReadRow

PURPOSE:  Reads a row of data from an input file

RETURN VALUE:
Type = int
Value           Description
-----           -----------
TRUE            Successful read
FALSE           Unable to read

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  Rob Burrell            Original Development
         01/01  John Rishea            Standardized formatting

NOTES:

******************************************************************************/
int ReadRow
(
    FileDescriptor *file,	/* I:  input file descriptor */
    int row,			/* I:  row number to read */
    double *buffer		/* I/O:  buffer to hold data */
)

{
    int status = FALSE;                 /* error status */
    size_t i;                           /* loop index for conversion */
    MRT_INT8_PTR cptr = NULL;           /* cast pointer for char data */
    MRT_UINT8_PTR ucptr = NULL;         /* cast ptr for unsigned char data */
    MRT_INT16_PTR sptr = NULL;          /* cast pointer for short data */
    MRT_UINT16_PTR usptr = NULL;        /* cast ptr for unsigned short data */
    MRT_INT32_PTR lptr = NULL;          /* cast pointer for int (4-byte) data */
    MRT_UINT32_PTR ulptr = NULL;        /* ptr for unsigned int (4-byte) data */
    MRT_FLOAT4_PTR fptr = NULL;         /* ptr for 4-byte float data */

    /* read in a row of data from which type of file */
    switch ( file->filetype )
    {
	case RAW_BINARY:
	    status = ReadRowMultiFile( file, row );
	    break;

	case HDFEOS:
	    status = ReadRowHdfEos( file, row );
	    break;
    }

    /* was the read successful ? */
    if ( status == FALSE )
	return FALSE;

    /* convert everything to double */
    switch ( file->datatype )
    {
	case DFNT_INT8:
	    cptr = ( MRT_INT8_PTR ) file->rowbuffer;
	    for ( i = 0; i < file->ncols; i++ )
		buffer[i] = ( double ) cptr[i];
	    break;

	case DFNT_UINT8:
	    ucptr = ( MRT_UINT8_PTR ) file->rowbuffer;
	    for ( i = 0; i < file->ncols; i++ )
		buffer[i] = ( double ) ucptr[i];
	    break;

	case DFNT_INT16:
	    sptr = ( MRT_INT16_PTR ) file->rowbuffer;
	    for ( i = 0; i < file->ncols; i++ )
		buffer[i] = ( double ) sptr[i];
	    break;

	case DFNT_UINT16:
	    usptr = ( MRT_UINT16_PTR ) file->rowbuffer;
	    for ( i = 0; i < file->ncols; i++ )
		buffer[i] = ( double ) usptr[i];
	    break;

	case DFNT_INT32:
	    lptr = ( MRT_INT32_PTR ) file->rowbuffer;
	    for ( i = 0; i < file->ncols; i++ )
		buffer[i] = ( double ) lptr[i];
	    break;

	case DFNT_UINT32:
	    ulptr = ( MRT_UINT32_PTR ) file->rowbuffer;
	    for ( i = 0; i < file->ncols; i++ )
		buffer[i] = ( double ) ulptr[i];
	    break;

	    /* no conversion required */
	case DFNT_FLOAT32:
            fptr = ( MRT_FLOAT4_PTR ) file->rowbuffer;
            for ( i = 0; i < file->ncols; i++ )
                buffer[i] = ( double ) fptr[i];
            break;

	default:
	    return ( FALSE );
    }

    return status;
}

/******************************************************************************

MODULE:  WriteRow

PURPOSE:  Writes a row of data to an output file

RETURN VALUE:
Type = int
Value           Description
-----           -----------
TRUE            Successful write
FALSE           Unable to write

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00   Rob Burrell            Original Development
         01/01   John Rishea            Standardized formatting 

NOTES:

******************************************************************************/
int WriteRow
(
    FileDescriptor *file,	/* I:  output file descriptor */
    int row,			/* I:  row number to write */
    double *buffer		/* I:  buffer of data to write */
)

{
    int status = FALSE;			/* error status */
    size_t i;                           /* loop index for conversion */
    MRT_INT8_PTR  cptr = NULL;          /* cast pointer for char data */
    MRT_UINT8_PTR ucptr = NULL;         /* cast ptr for unsigned char data */
    MRT_INT16_PTR sptr = NULL;          /* cast pointer for short data */
    MRT_UINT16_PTR usptr = NULL;        /* cast ptr for unsigned short data */
    MRT_INT32_PTR lptr = NULL;          /* cast pointer for int (4-byte) data */
    MRT_UINT32_PTR ulptr = NULL;        /* ptr for unsigned int (4-byte) data */
    MRT_FLOAT4_PTR fptr = NULL;         /* ptr for 4-byte float data */
    double bufferd;

    /* convert from double to desired data type */
    switch ( file->datatype )
    {
	case DFNT_INT8:
	    cptr = ( MRT_INT8_PTR ) file->rowbuffer;
	    for ( i = 0; i < file->ncols; i++ )
	    {
                bufferd = buffer[i] < 0.0 ? buffer[i] - (double)0.5 :
                                            buffer[i] + (double)0.5;
		if ( bufferd < MRT_INT8_MIN )
		    cptr[i] = MRT_INT8_MIN;
		else if ( bufferd > MRT_INT8_MAX )
		    cptr[i] = MRT_INT8_MAX;
		else
		    cptr[i] = ( MRT_INT8 ) bufferd;
	    }
	    break;

	case DFNT_UINT8:
	    ucptr = ( MRT_UINT8_PTR ) file->rowbuffer;
	    for ( i = 0; i < file->ncols; i++ )
	    {
                bufferd = buffer[i] < 0.0 ? buffer[i] - (double)0.5 :
                                            buffer[i] + (double)0.5;
		if ( bufferd < 0.0 )
		    ucptr[i] = 0;
		else if ( bufferd > MRT_UINT8_MAX )
		    ucptr[i] = MRT_UINT8_MAX;
		else
		    ucptr[i] = ( MRT_UINT8 ) bufferd;
	    }
	    break;

	case DFNT_INT16:
	    sptr = ( MRT_INT16_PTR ) file->rowbuffer;
	    for ( i = 0; i < file->ncols; i++ )
	    {
                bufferd = buffer[i] < 0.0 ? buffer[i] - (double)0.5 :
                                            buffer[i] + (double)0.5;
		if ( bufferd < MRT_INT16_MIN )
		    sptr[i] = MRT_INT16_MIN;
		else if ( bufferd > MRT_INT16_MAX )
		    sptr[i] = MRT_INT16_MAX;
		else
		    sptr[i] = ( MRT_INT16 ) bufferd;
	    }
	    break;

	case DFNT_UINT16:
	    usptr = ( MRT_UINT16_PTR ) file->rowbuffer;
	    for ( i = 0; i < file->ncols; i++ )
	    {
                bufferd = buffer[i] < 0.0 ? buffer[i] - (double)0.5 :
                                            buffer[i] + (double)0.5;
		if ( bufferd < 0.0 )
		    usptr[i] = 0;
		else if ( bufferd > MRT_UINT16_MAX )
		    usptr[i] = MRT_UINT16_MAX;
		else
		    usptr[i] = ( MRT_UINT16 ) bufferd;
	    }
	    break;

	case DFNT_INT32:
	    lptr = ( MRT_INT32_PTR ) file->rowbuffer;
	    for ( i = 0; i < file->ncols; i++ )
	    {
                bufferd = buffer[i] < 0.0 ? buffer[i] - (double)0.5 :
                                            buffer[i] + (double)0.5;
		if ( bufferd < MRT_INT32_MIN )
		    lptr[i] = MRT_INT32_MIN;
		else if ( bufferd > MRT_INT32_MAX )
		    lptr[i] = MRT_INT32_MAX;
		else
		    lptr[i] = ( MRT_INT32 ) bufferd;
	    }
	    break;

	case DFNT_UINT32:
	    ulptr = ( MRT_UINT32_PTR ) file->rowbuffer;
	    for ( i = 0; i < file->ncols; i++ )
	    {
                bufferd = buffer[i] < 0.0 ? buffer[i] - (double)0.5 :
                                            buffer[i] + (double)0.5;
		if ( bufferd < 0 )
		    ulptr[i] = 0;
		else if ( bufferd > MRT_INT32_MAX )
		    ulptr[i] = MRT_INT32_MAX;
		else
		    ulptr[i] = ( MRT_INT32 ) bufferd;
	    }
	    break;

	    /* no conversion required */
	case DFNT_FLOAT32:
            fptr = ( MRT_FLOAT4_PTR ) file->rowbuffer;
            for ( i = 0; i < file->ncols; i++ )
            {
               if( buffer[i] < 0.0 )
                  fptr[i] = 0.0;
               else if( buffer[i] > MRT_FLOAT4_MAX )
                  fptr[i] = MRT_FLOAT4_MAX;
               else 
                  fptr[i] = ( MRT_FLOAT4 ) buffer[i];
            }
	    break;

	default:
	    return ( FALSE );
    }

    /* write row to file */
    switch ( file->filetype )
    {
	case RAW_BINARY:
	    status = WriteRowMultiFile( file );
	    break;

	case HDFEOS:
	    status = WriteRowHdfEos( file, row );
	    break;

	case GEOTIFF:
	    status = WriteRowGeoTIFF( file, row );
	    break;
    }

    return ( status );
}
