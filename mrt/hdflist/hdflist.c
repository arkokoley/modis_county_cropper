
/******************************************************************************

FILE:  hdflist.c

PURPOSE:  Opens input HDF-EOS data file and dumps info to stdout.

HISTORY:
Version  Date   Programmer       Code  Reason
-------  -----  ---------------  ----  -------------------------------------
         05/00  John M. Weiss, Ph.D.   Original Development
         08/00  John M. Weiss, Ph.D    Fix to handle multiple grids properly.
         01/01  John Rishea            Standardized formatting 
         01/01  John Rishea            Removed testing code
         
HARDWARE AND/OR SOFTWARE LIMITATIONS:  
  None

PROJECT:    MODIS Reprojection Tool

NOTES:

******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "hdf.h"
#include "HdfEosDef.h"
#ifdef WIN32
#define strdup _strdup
#endif

int main
(
    int argc,
    char *argv[]
)

{
    int i, j;
    int32 fid, gid;
    int32 ngrids, nentries, nattrs, ndims, nfields;
    int32 rank, numbertype, sizedim, status, bufsiz;
    int32 projcode, zonecode, spherecode;
    int32 xdimsize, ydimsize, origininfo;
    int16 fillvalue;
    int32 dims[10], ranks[100], numbertypes[100];
    float64 upleft[2], lowright[2], projparm[16];
    char strbuf[4096];
    char *attrname = NULL, *dimname = NULL;
    char *fieldlist = NULL, *fieldname = NULL;
    char *gridlist = NULL, *gridname = NULL, *gridend = NULL;

    /* check usage */
    if ( argc != 2 )
    {
	fprintf( stdout, "Usage: %s file.hdf\n", argv[0] );
        fflush( stdout );
	return -1;
    }

    /* test GDinqgrid() */
    ngrids = GDinqgrid( argv[1], strbuf, &bufsiz );
    gridlist = strdup( strbuf );
    printf( "%s: %i grids (%s)\n", argv[1], (int) ngrids, gridlist );

    /* open file */
    fid = GDopen( argv[1], DFACC_READ );

    /* loop through the grids in gridlist using strtok() */
    for ( i = 0; i < ngrids; i++ )
    {
	/* open next grid */
	/* unfortunately, can't use strtok for parsing both grids and fields */
	gridname = gridlist;
	gridend = strchr( gridlist, ',' );
	if ( gridend != NULL )
	{
	    gridlist = gridend + 1;
	    *gridend = '\0';
	}

	/* attach to grid gridname */
	gid = GDattach( fid, gridname );

	/* get number of entries (fields?) */
	nentries = GDnentries( gid, HDFE_NENTDFLD, &bufsiz );
	printf( "\nGrid %i: %s (%i entries)\n", i + 1, gridname,
            (int) nentries );

	/* get attribute info */
	/* NOTE: not HDF-EOS readable, must use HDF SDreadattr() */
	strbuf[0] = '\0';
	nattrs = GDinqattrs( gid, strbuf, &bufsiz );
	printf( "%i attributes:", (int) nattrs );
	for ( attrname = strtok( strbuf, "," );
	    attrname != NULL; attrname = strtok( NULL, "," ) )
	    printf( " %s", attrname );
	printf( "\n" );

	/* try specifically to read background fill value */
	/* NOTE: not HDF-EOS readable, must use HDF SDgetfillvalue() */
	status = GDreadattr( gid, "_FillValue", &fillvalue );
	if ( status != -1 )
	    printf( "_FillValue = %i\n", fillvalue );
	/* else printf( "could not read _FillValue attribute\n" ); */

	/* get dimension info */
	strbuf[0] = '\0';
	ndims = GDinqdims( gid, strbuf, dims );
	printf( "GDinqdims: %i dimensions (%s):", (int) ndims, strbuf );
	for ( j = 0, dimname = strtok( strbuf, "," );
	    dimname != NULL; j++, dimname = strtok( NULL, "," ) )
	{
	    sizedim = GDdiminfo( gid, dimname );
	    printf( " %s (%i) %i", dimname, (int) sizedim, (int) dims[j] );
	}
	printf( "\n" );

	/* get grid info */
	status = GDgridinfo( gid, &xdimsize, &ydimsize, upleft, lowright );
	printf( "gridinfo: xdimsize = %i, ydimsize = %i\n", (int) xdimsize,
            (int) ydimsize );
	printf( "          UL = (%f, %f)\n", upleft[0], upleft[1] );
	printf( "          LR = (%f, %f)\n", lowright[0], lowright[1] );

	/* get grid origin info (this seems to work) */
	status = GDorigininfo( gid, &origininfo );
	if ( status == -1 )
	    printf( "no origin info\n" );
	else
	    printf( "origin code = %i\n", (int) origininfo );

	/* get list of fields (entries?) */
	strbuf[0] = '\0';
	nfields = GDinqfields( gid, strbuf, ranks, numbertypes );
	fieldlist = strdup( strbuf );
	printf( "%i fields (%s):\n", (int) nfields, fieldlist );
	printf( "ranks:" );
	for ( j = 0; j < nfields; j++ )
	    printf( " %i", (int) ranks[j] );
	printf( "\n" );
	printf( "numbertypes:" );
	for ( j = 0; j < nfields; j++ )
	    printf( " %i", (int) numbertypes[j] );
	printf( "\n" );

	/* loop through field names using strtok() */
	for ( fieldname = strtok( fieldlist, "," );
	    fieldname != NULL; fieldname = strtok( NULL, "," ) )
	{
	    /* get field info */
	    status = GDfieldinfo( gid, fieldname, &rank, dims, &numbertype,
                strbuf );
	    printf( "%s: rank %i, numbertype %i, dimlist %s:", fieldname,
                (int) rank, (int) numbertype, strbuf );
	    for ( j = 0; j < rank; j++ )
		printf( " %i", (int) dims[j] );
	    printf( "\n" );
	}

	/* get projection parameters */
	GDprojinfo( gid, &projcode, &zonecode, &spherecode, projparm );
	printf( "projcode %i, zonecode %i, spherecode %i\n", (int) projcode,
            (int) zonecode, (int) spherecode );
	printf( "projparm:" );
	for ( j = 0; j < 13; j++ )
	    printf( " %f", projparm[j] );
	printf( " %f %f\n", 0.0, 0.0 );	/* last two proj params are zero */

	/* detach from grid */
	GDdetach( gid );
    }

    /* close file and quit */
    GDclose( fid );
    return 0;
}
