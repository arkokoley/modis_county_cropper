/******************************************************************************

FILE:  filebuf.c

PURPOSE:  Handle input file read buffering

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  Rob Burrell            Original Development
         04/02  Gail Schmidt           Changed data pointers from floats to
                                       doubles

HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:
  Basically just a pushdown and replace the least recently used 
  image row.  Future development might include write buffers as well.

******************************************************************************/
#include "shared_resample.h"

/******************************************************************************

MODULE:  CreateFileBuffers

PURPOSE:  Initialize an LRU queue

RETURN VALUE:
Type = int
Value           Description
-----           -----------
TRUE            Success
FALSE           Failure

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00   Rob Burrell            Original Development
         01/01   John Rishea            Standardized formatting

NOTES:
  See shared_resample.h for memory allocation limits for the
  buffering scheme.  
  
******************************************************************************/
int CreateFileBuffers
(
    FileDescriptor *file	/* I/O:  file for which buffers are created */
)

{
    size_t i;			/* init loop index */
    QueueType *curr = NULL,	/* queue pointers */
              tempqueue;
    size_t rowsize;		/* number of bytes/row */
    size_t numrows;		/* number of rows we can allocate */

    /* how many bytes do we need */
    rowsize = file->ncols * sizeof( double );

    /* how many rows can we make from the max memory allowed */
    numrows = MAX_BUFFER_SIZE / rowsize;

    /* if we've got more memory than we need, reduce the total.
     * This will mean the entire image can live in memory */
    if ( numrows > file->nrows )
	numrows = file->nrows;

    /* the queue must have at least two entries to complete the
     * circle */
    if ( numrows < 2 )
	numrows = 2;

    /* save the total in the file descriptor */
    file->queuetop.numbuffers = numrows;

    /* a list of flags for easy checking */
    file->queuetop.in_cache =
	( QueueType ** ) calloc( file->nrows, sizeof( QueueType * ) );
    if ( !file->queuetop.in_cache )
	ErrorHandler( TRUE, "CreateFileBuffers", ERROR_MEMORY, "Queue Cache" );

    /* just a place holder to simplify the loop */
    curr = &tempqueue;
    curr->prev = NULL;

    /* allocate the doubly linked list of buffers */
    for ( i = 0; i < numrows; i++ )
    {
	/* create an element */
	curr->next = ( QueueType * ) calloc( 1, sizeof( QueueType ) );
	if ( !curr->next )
	    ErrorHandler( TRUE, "CreateFileBuffers", ERROR_MEMORY, "Node" );

	/* link it to the previous and the next */
	curr->next->prev = curr;
	curr = curr->next;

	/* allocate the buffer itself */
	curr->data = ( double * ) calloc( file->ncols, sizeof( double ) );

	if ( !curr->data )
	    ErrorHandler( TRUE, "CreateFileBuffers", ERROR_MEMORY,
            "Row Buffer" );

	/* mark this row as unused */
	curr->row = (size_t)-1;
    }

    /* set the descriptor to hold the queue */
    file->queuetop.first = tempqueue.next;
    file->queuetop.last = curr;

    return ( TRUE );
}

/******************************************************************************

MODULE:  ClobberFileBuffers

PURPOSE:  Sets the buffers to empty

RETURN VALUE:
Type = none

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00   Rob Burrell            Original Development
         01/01   John Rishea            Standardized formatting 
NOTES:
  Since we may be reading from different bands having
  the same size, we can simply "clobber" the buffers
  to prevent future reads from getting "old" data 
  
******************************************************************************/
void ClobberFileBuffers
(
    FileDescriptor *file	/* I:  file for which buffers are clobbered */
)

{
    size_t i;			/* loop index for flags */
    QueueType *curr = NULL;	/* linked list walker */

    /* set all flags to empty */
    for ( i = 0; i < file->nrows; i++ )
	file->queuetop.in_cache[i] = NULL;

    /* run through and set all buffers to empty */
    curr = file->queuetop.first;
    while ( curr )
    {
	curr->row = (size_t)-1;
	curr = curr->next;
    }
}

/******************************************************************************

MODULE:  DestroyFileBuffers

PURPOSE:  Free all memory used by the queue

RETURN VALUE:
Type = int
Value           Description
-----           -----------
TRUE            Success

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00   Rob Burrell            Original Development
         01/01   John Rishea            Standardized formatting 

NOTES:
  
******************************************************************************/
int DestroyFileBuffers
(
    FileDescriptor *file	/* I:  file for which buffers are destroyed */
)

{
    QueueType *curr = NULL,	/* linked list walkers */
              *next = NULL;

    /* don't assume the flag list is available */
    if ( file->queuetop.in_cache )
	free( file->queuetop.in_cache );

    /* mark it gone */
    file->queuetop.in_cache = NULL;

    /* don't assume the buffers are available */
    if ( file->queuetop.first )
    {
	curr = file->queuetop.first;
	while ( curr )
	{
	    next = curr->next;
	    free( curr->data );
	    free( curr );
	    curr = next;
	}
    }
    /* mark it gone */
    file->queuetop.first = NULL;
    return ( TRUE );
}

/******************************************************************************

MODULE:  ReadBufferValue

PURPOSE:  Get a pixel value from a read buffer

RETURN VALUE:
Type = double
Value           Description
-----           -----------
pixel           value of the pixel at some row/col

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00   Rob Burrell            Original Development
         01/01   John Rishea            Standardized formatting

NOTES:
  Check the queue to see if the row we need is available.
  
  If it is the first row, just return a value from that
  row.  If the row is in the queue, find it, move it to 
  the top, and return a value.  If the row is not in the 
  queue, throw away the row at the bottom (least recently 
  used), read a new row, move it to the top, and return
  a pixel.

******************************************************************************/
double ReadBufferValue
(
    size_t col,			/* I:  column to find */
    size_t row,			/* I:  row to find */
    FileDescriptor *file	/* I:  file to read from */
)

{
    size_t i;			/* loop index for setting an empty row */
    QueueType *curr = NULL,	/* linked list walkers */
              *topnext = NULL;

    /* if we're outside the image, return no value */
    if ( (col >= file->ncols) || (row >= file->nrows) )
	return ( file->background_fill );

    /* grab the first row in the queue */
    topnext = file->queuetop.first;

    /* if this is the row we need, return a value */
    if ( topnext->row == row )
	return ( topnext->data[col] );

    /* check the flag list to see if this row is in memory */
    if ( file->queuetop.in_cache[row] )
    {
	/* grab the element */
	curr = file->queuetop.in_cache[row];

	/* take it out of the loop */
	curr->prev->next = curr->next;

	/* if there's another following, we're not at the bottom,
	 * close the link */
	if ( curr->next )
	    curr->next->prev = curr->prev;

	else
	{
	    /* we're at the bottom, save the new bottom by pointing
	     * the list header at the new bottom */
	    file->queuetop.last = curr->prev;
	    curr->prev->next = NULL;
	}

	/* move it to the top of the list */
	curr->next = topnext;
	curr->next->prev = curr;
	file->queuetop.first = curr;

	return ( curr->data[col] );
    }

    /* oops, need to read a row, get the last row in the queue */
    curr = file->queuetop.last;

    /* If this row has been used before, set the flag to empty 
     * for the row we're going to toss, this may only save a few
     * clocks in the beginning. */
    if ( curr->row != (size_t)-1 ) 
	file->queuetop.in_cache[curr->row] = NULL;

    /* read a new row into the buffer, if the read failed, lets
     * just set it to empty rather than complaining */
    if ( !ReadRow( file, row, curr->data ) )
    {
	for ( i = 0; i < file->ncols; i++ )
	    curr->data[i] = file->background_fill;
    }

    /* the prev to the bottom is now the botton (bottom->next = NULL) */
    curr->prev->next = NULL;

    /* the last queuetop gets pushed down */
    curr->next = topnext;

    /* the new bottom is set */
    file->queuetop.last = curr->prev;

    /* old queuetop points back to the new queuetop */
    curr->next->prev = curr;

    /* set the new queuetop */
    file->queuetop.first = curr;

    /* set the flags to show this row available */
    curr->row = row;
    file->queuetop.in_cache[row] = curr;

    return ( curr->data[col] );
}
