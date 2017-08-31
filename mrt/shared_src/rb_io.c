
/******************************************************************************

FILE:  rb_io.c

PURPOSE:  Raw binary read/write routines

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  Rob Burrell            Original Development
         05/07  Gail Schmidt           Removed the MRT_BYTE_ORDER and
                                       replaced with BYTE_ORDER
                                       Also removed swab definition for Linux
                                       since swab is defined for Linux now

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/
#if defined(__linux__)
#  ifndef _XOPEN_SOURCE
#    define _XOPEN_SOURCE
#    include <unistd.h>
#    undef _XOPEN_SOURCE
#  else
#    include <unistd.h>
#  endif
#else
#  ifndef WIN32
#     include <unistd.h>
#  endif
#endif

#include <sys/types.h>
#include "shared_mosaic.h"


/******************************************************************************

MODULE:  ReadRowMultiFile

PURPOSE:  Read a row of data from a multi-file

RETURN VALUE:
Type = int
Value           Description
-----           -----------
TRUE            Success
FALSE           Failure

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  Rob Burrell            Original Development
         06/00  John Weiss             byte swapping for endian issues
         01/01  John Rishea            Standardized formatting
         01/01  John Rishea            Added check for memory allocation 
NOTES:

******************************************************************************/

int ReadRowMultiFile
(
    FileDescriptor *file,	/* I/O:  file to read from */
    int row			/* I:  row number to read */
)

{
    int status = TRUE;		/* error status */
    int offset;			/* offset into file */
    size_t i;			/* index for byte swapping */
    int tmp;			/* temp value for byte swapping */
    unsigned char *val = NULL;	/* single byte for byte swapping */
    static unsigned char *bufswp = NULL;	/* pointer for swab */
    /* true if machine is big endian */
    MrtEndianness machineEndianness = MRT_UNKNOWN_ENDIAN;
    int doByteSwap = 0;         /* true if byte swapping is to occur */

    /* seek to correct spot in file for read */
    offset = file->datasize * row * file->ncols;
    fseek( ( FILE * ) file->fileptr, offset, SEEK_SET );

    /* unfortunately, byte order is an issue for multifile I/O.
     * check the endianness of the raw binary file against the
     * endianness of the machine.  If it is the same, then no
     * byte swapping is necessary, otherwise if they are different
     * then byte swapping is necessary.
     */
    if(file->file_endianness != MRT_BIG_ENDIAN &&
       file->file_endianness != MRT_LITTLE_ENDIAN)
    {
       ErrorHandler( TRUE, "ReadRowMultiFile", ERROR_READ_INPUTIMAGE,
                     "Cannot determine byte order, exiting." );
    }

    machineEndianness = GetMachineEndianness();

    if( file->file_endianness == MRT_BIG_ENDIAN &&
        machineEndianness != MRT_BIG_ENDIAN )
       doByteSwap = 1;
    if( file->file_endianness == MRT_LITTLE_ENDIAN &&
        machineEndianness != MRT_LITTLE_ENDIAN )
       doByteSwap = 1;

    if( !doByteSwap )
    {
       /* read big endian data, no need to byte swap */
       if ( fread( file->rowbuffer, file->datasize, file->ncols,
                   ( FILE * ) file->fileptr ) != file->ncols )
       {
          /* oops, read wrong number of data items */
          ErrorHandler( TRUE, "ReadRowMultiFile", ERROR_READ_INPUTIMAGE,
                        "Read wrong number of data items" );
          return FALSE;
       }
    } 
    else
    {
       /* have to byte swap int/float big endian data on little endian boxes */
       switch ( file->datasize )
       {
       /* byte data? no need to byte swap */
       case 1:
          /* read row */
          if ( fread( file->rowbuffer, file->datasize, file->ncols,
                      ( FILE * ) file->fileptr ) != file->ncols )
          {
             /* oops, read wrong number of data items */
             ErrorHandler( TRUE, "ReadRowMultiFile", ERROR_READ_INPUTIMAGE,
             "Read wrong number of data items" );
             return FALSE;
          }
          break;

       /* 2-byte values: assume that swab is implemented efficiently */
       case 2:
          /* allocate memory for swap buffer */
          if ( bufswp == NULL )
             bufswp = calloc( file->ncols, file->datasize );

          /* if swap buffer memory can't be allocated, exit w/ error */
          if ( bufswp == NULL )
          {
             /* log error */
             ErrorHandler( TRUE, "ReadRowMultiFile", ERROR_MEMORY,
                           "Unable to allocate memory for byte swapping" );
             return FALSE;
          } 

          /* read row */
          if ( fread( bufswp, file->datasize, file->ncols,
                      ( FILE * ) file->fileptr ) != file->ncols )
          {
             /* free allocated memory */
             free ( bufswp );

             /* oops, read wrong number of data items */
             ErrorHandler( TRUE, "ReadRowMultiFile", ERROR_READ_INPUTIMAGE,
                           "Read wrong number of data items" );
             return FALSE;
          }

	  /* swap bytes: swab(src, dest, n) */
	  swab( bufswp, file->rowbuffer, file->datasize * file->ncols );

          /* free allocated memory */
          free ( bufswp );
          bufswp = NULL;
	
       	  break;

       /* 4-byte values: we better do this by hand (or use htonl() ?) */
       case 4:
          /* read row */
          if ( fread( file->rowbuffer, file->datasize, file->ncols,
                      ( FILE * ) file->fileptr ) != file->ncols )
          {
          /* oops, read wrong number of data items */
          ErrorHandler( TRUE, "ReadRowMultiFile", ERROR_READ_INPUTIMAGE,
                        "Read wrong number of data items" );
          return FALSE;
          }

          /* swap bytes */
          val = file->rowbuffer;
          for ( i = 0; i < file->ncols; i++, val += 4 )
          {
             tmp = val[0];
             val[0] = val[3];
             val[3] = tmp;
             tmp = val[1];
             val[1] = val[2];
             val[2] = tmp;
          }
       break;
       }
    }

    return ( status );
}

/******************************************************************************

MODULE:  WriteRowMultiFile

PURPOSE:  Write a row of data to a multi-file file

RETURN VALUE:
Type = int
Value           Description
-----           -----------
TRUE            Success
FALSE           Failure

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  Rob Burrell            Original Development
         06/00  John Weiss             byte swapping for endian issues
         01/01  John Rishea            Standardized formatting
         01/01  John Rishea            Added check for memory allocation 
         07/07  Maverick Merritt       Modified for new endianness behavior

NOTES:

******************************************************************************/

int WriteRowMultiFile
(
    FileDescriptor *file	/* I:  file to write data */
)
{
    int status = TRUE;		/* error status */

    /* byte order is an issue for multifile I/O.  The MRT was
     * modified to output data in the endianness of the machine that
     * the applications were run from.  The endianness is noted in the
     * raw binary header file.  If the endianness tag is not found
     * in the raw binary header file, then the applications will exit
     * with an error.
     */

    /* write data, no need to byte swap */
    if ( fwrite( file->rowbuffer, file->datasize, file->ncols,
	    ( FILE * ) file->fileptr ) != file->ncols )
    {
	/* oops, wrote wrong number of data items */
	ErrorHandler( TRUE, "WriteRowMultiFile", ERROR_WRITE_OUTPUTIMAGE,
	    "Wrote wrong number of data items" );
	return FALSE;
    }

    return ( status );
}

/*************************************************************************

MODULE: read_tile_number_rb

PURPOSE:
    This module parses the input filenames for h##v##, which are the
    horizontal and vertical tile numbers.

RETURN VALUE:
Type = intn
Value           Description
-----           -----------
MRT_NO_ERROR        Returns MRT_NO_ERROR if the metadata read was successful and
                the tile numbers were found.  Error codes found in mrt_error.c
                and mrt_error.h.
-29             Returns ERROR_INPUTNAME_FIELD if the h##v## cannot be found
                in the input raw binary filename.


HISTORY:
Version    Date     Programmer      Code     Reason
-------    ----     ----------      ----     ------
           06/02    Gail Schmidt             Original development

NOTES:

**************************************************************************/

int read_tile_number_rb
(
    char filename[],          /* I: the filename of the input file */
    int *horiz,               /* O: horizontal tile number */
    int *vert                 /* O: vertical tile number */
)

{
    int name_length;            /* length of the base filename */
    int curr_pos;               /* current position in the filename */
    char tmpstr[HUGE_STRING];   /* temporary copy of the filename */
    char *tmpptr;               /* pointer to the '/' */
    char tile_pos[3];           /* horizontal and vertical tile position */
    char errmsg[SMALL_STRING];  /* error message string */


    /* Strip off the directory name */
    tmpptr = strrchr( filename, '/' );
    if ( tmpptr != NULL )
        strcpy( tmpstr, tmpptr );
    else
        strcpy( tmpstr, filename );

    /* Strip off the file extension */
    tmpptr = strrchr( tmpstr, '.' );
    if ( tmpptr != NULL )
        *tmpptr = '\0';

    /* Upper case the string to make searching easier */
    strupr( tmpstr );

    /* Look for h##v##.  Stop looping at 5 characters short of the end of
       the filename, since h##v## has 6 characters. */
    name_length = strlen( tmpstr );
    for ( curr_pos = 0; curr_pos < name_length - 5; curr_pos++ )
    {
        /* Is this the h##v##? */
        if ( tmpstr[curr_pos] == 'H' && tmpstr[curr_pos+3] == 'V' )
        {
            /* Check the ## for the h position to make sure it is a valid
               integer */
            strncpy( tile_pos, &tmpstr[curr_pos+1], 2 );
            tile_pos[2] = '\0';
            *horiz = atoi( tile_pos );

            /* If not a valid ## then continue to the next position in the
               filename. NOTE: tile_pos = "00" is a valid tile. */
            if ( *horiz == 0 && tile_pos[0] != '0' && tile_pos[1] != '0' )
                continue;

            /* Check the ## for the v position to make sure it is a valid
               integer */
            strncpy( tile_pos, &tmpstr[curr_pos+4], 2 );
            tile_pos[2] = '\0';
            *vert = atoi( tile_pos );

            /* If not a valid ## then continue to the next position in the
               filename.  OW, we have a valid horizontal and vertical tile
               position so exit the loop.  NOTE: tile_pos = "00" is a valid
               tile. */
            if ( *vert == 0 && tile_pos[0] != '0' && tile_pos[1] != '0' )
                continue;

            /* Verify that the horizontal and vertical tiles are within the
               bounds of the 10-degree tiles */
            if ( *horiz > MAX_HORIZ_TILES )
            {
                sprintf( errmsg, "Horizontal tile number (%d) in the filename "
                    "of %s is larger than the maximum number of 10-degree "
                    "ISIN tiles (%d).\n", *horiz, filename, MAX_HORIZ_TILES );
                ErrorHandler( TRUE, "read_tile_number_rb", ERROR_GENERAL,
                    errmsg );
            }

            if ( *vert > MAX_VERT_TILES )
            {
                sprintf( errmsg, "Vertical tile number (%d) in the filename "
                    "of %s is larger than the maximum number of 10-degree "
                    "ISIN tiles (%d).\n", *vert, filename, MAX_VERT_TILES );
                ErrorHandler( TRUE, "read_tile_number_rb", ERROR_GENERAL,
                    errmsg );
            }

            /* Successful read of the tiles so return */
            return ( MRT_NO_ERROR );
        }
    }

    /* The filename does not contain a valid h##v##, so output an error
       message. */
    sprintf( errmsg,
        "Error: h##v## not found in the raw binary filename: %s\n", filename );
    ErrorHandler( TRUE, "read_tile_number_rb", ERROR_INPUTNAME_FIELD,
        errmsg );

    return ( ERROR_INPUTNAME_FIELD );
}

MrtEndianness GetMachineEndianness(void)
{
   long one = 1;
   if( !(*((char *)(&one))) )
      return MRT_BIG_ENDIAN;
   return MRT_LITTLE_ENDIAN;
}

