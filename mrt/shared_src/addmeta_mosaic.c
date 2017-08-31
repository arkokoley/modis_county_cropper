/************************************************************************

FILE: addmdata_mosaic.c

PURPOSE:  Attach metadata from input hdf file to output hdf file for
          mosaicking.

HISTORY:
Version    Date     Programmer      Code     Reason
-------    ----     ----------      ----     ------
1.0        07/02    Gail Schmidt             Original Development 

HARDWARE AND/OR SOFTWARE LIMITATIONS:
    None

PROJECT: 	Modis Reprojection Tool

NOTES:   This program is an adaptation of metadmp.c, developed by Doug
Ilg (Doug.Ilg@fsfc.nasa.gov).


*************************************************************************/

#include "shared_mosaic.h"
#include "mfhdf.h"
#include "mrt_error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/************** LOCAL PROTOTYPES *****************************************/ 
intn TransferMetadataMosaic( int32 old_fid, int32 new_fid, int row, int col );
intn TransferAttrMosaic( int32 fid_old, int32 fid_new, int row, int col,
    char *attr );
intn TransferAttributesMosaic( MosaicDescriptor *mosaic, int32 old_fid,
    int32 new_fid );
intn CreateAttributesMosaic( MosaicDescriptor *mosaic, int32 new_fid );
intn write_bounding_coords( int32 in_sd_id, int32 out_sd_id,
    MosaicDescriptor *mosaic );
intn write_bounds_attr( int32 in_sd_id, int32 out_sd_id, char *attr,
    MosaicDescriptor *mosaic );
/*************************************************************************/


/*************************************************************************

MODULE: AppendMetadataMosaic

PURPOSE: 
    This module allows EOSDIS user-defined core, archive, and structural
    metadata from the original (input) HDF files to be appended to the new
    (output) HDF file.  Specifically, the metadata from the old files is
    first extracted and then added to the new file as a string of characters
    assigned to a file-level attribute tag.  The user-defined metadata fields
    are assigned names of "OldCoreMetadata<r,c>", "OldArchiveMetadata<r,c>",
    and "OldStructMetadata<r,c>". The <#> will represent the row, column
    location of the specific image in the output mosaic. These metadata
    fields can be extracted using the SDfindattr and SDattrinfo commands
    from the HDF library.
 

RETURN VALUE:
Type = int
Value    	Description
-----		-----------
  0		Returns MRT_NO_ERROR if the metadata appending operation was
                successful.  Error codes found in mrt_error.c and mrt_error.h.
-14             Returns ERROR_OPEN_INPUTIMAGE if the old HDF file can't be
                opened.
-15             Returns ERROR_READ_INPUTIMAGE if the old HDF file can't be
                read.
-16		Returns ERROR_OPEN_OUTPUTIMAGE if the new HDF file can't
   		be opened.
-17		Returns ERROR_WRITE_OUTPUTIMAGE if the new HDF file can't
   		be written to.

HISTORY:
Version    Date     Programmer      Code     Reason
-------    ----     ----------      ----     ------
           07/02    Gail Schmidt             Original Development
           03/03    Gail Schmidt             Limit the appended metadata files
                                             to one

NOTES:
    1. Only the first file will be appended as metadata. The large
       global mosaics produce a large number of input metadata to be attached
       to the output metadata, which ultimately overflows the metadata buffer.

**************************************************************************/

intn AppendMetadataMosaic
(
    int numh_tiles,             /* I: number of horiz tiles in the mosaic */
    int numv_tiles,             /* I: number of vert tiles in the mosaic */
    int **tile_array,           /* I: 2D array of size [numv_tiles][numh_tiles]
                                      specifying which input file represents
                                      that tile location */
    MosaicDescriptor infiles[], /* I: file descriptor array for the input
                                      files */
    MosaicDescriptor *mosaic	/* I: mosaic info */
)

{
    int h, v;            /* horizontal and vertical tile looping vars */
    int curfile;         /* current file */
    intn trans_status;   /* used to capture return val frm subfunct */
    int32 old_sd_id;     /* holds the id# for the old HDF file */
    int32 new_sd_id;     /* holds the id# for the new HDF file */ 
    char *new_hdf_file;  /* the HDF file we want to append metadata to */

    /* Open the new HDF file for writing */
    new_hdf_file = strdup( mosaic->filename );
    new_sd_id = SDstart( new_hdf_file, DFACC_WRITE );
    if ( new_sd_id == -1 )
    {
	fprintf( stdout, "Error: unable to open ouput HDF-EOS file %s \n",
            new_hdf_file );
        fflush( stdout );
	return( ERROR_OPEN_OUTPUTIMAGE );
    }

    /* open the first HDF file for the metadata */
    old_sd_id = SDstart( infiles[0].filename, DFACC_READ );
    if ( old_sd_id == -1 )
    {
        fprintf( stdout, "Error: unable to open input file %s: \n",
            infiles[0].filename );
        fflush( stdout );
        return( ERROR_OPEN_INPUTIMAGE );
    }

    /* output the bounding rectangular coordinates using the first HDF file */
    write_bounding_coords( old_sd_id, new_sd_id, mosaic );

    /* close the old HDF file */
    SDend( old_sd_id);

    /* add metadata and attributes since processing HDF-EOS to HDF-EOS */
    /* Loop through the input HDF-EOS files */
    for ( v = 0; v < numv_tiles; v++ )
    {
        for ( h = 0; h < numh_tiles; h++ )
        {
            /* Check to see if the current tile has an input file. If so,
               then process metadata for that tile. */
            if ( tile_array[v][h] != -9 )
            {
                /* Determine which input file is associated with this h,v tile
                   location */
                curfile = tile_array[v][h];

                /* Open the old HDF file for reading */
                old_sd_id = SDstart( infiles[curfile].filename, DFACC_READ );
                if ( old_sd_id == -1 )
                {
                    /* first close the output file */
                    SDend( new_sd_id );

                    fprintf( stdout, "Error: unable to open input file %s \n",
                        infiles[curfile].filename );
                    fflush( stdout );
                    return( ERROR_OPEN_INPUTIMAGE );
                }

                /* transfer SDS attributes from old HDF file to new HDF file */
                trans_status = TransferAttributesMosaic( mosaic, old_sd_id,
                    new_sd_id );
                if ( trans_status == -1 )
                {
                    fprintf( stdout, "Warning: unable to transfer attributes "
                        "from %s to %s\n", infiles[curfile].filename,
                        new_hdf_file );
                    fflush( stdout );
                }

                /* transfer metadata from old HDF file to new HDF file. only
                   output information from the first tile, since large
                   mosaics will overwrite the metadata buffers. */
                if ( curfile < 1 )
                {
                    trans_status = TransferMetadataMosaic( old_sd_id, new_sd_id,
                        v, h );

                    /* if the transfer was unsuccessful, return an error */
                    if ( trans_status == -1 )
                    {
                        /* first close the input and output HDF files */
                        SDend( old_sd_id );
                        SDend( new_sd_id );
 
                        fprintf( stdout, "Warning: unable to append metadata "
                            "from %s to %s\n", infiles[curfile].filename,
                            new_hdf_file );
                        fflush( stdout );
                        return( trans_status );
                    }
                }

                /* close the old HDF file */
                SDend( old_sd_id);
            }
        }
    }

    /* close the new HDF file */
    SDend( new_sd_id );
    free( new_hdf_file );

    return( MOSAIC_SUCCESS );
}

/*************************************************************************

Name: TransferAttributesMosaic

Author: Gail Schmidt 07/2002

Transfer HDF SDS attributes from input to output, pretty much intact.
Global attributes (HDF-EOS version, metadata) are handled elsewhere.

Note: should check various function return values.

*************************************************************************/

intn TransferAttributesMosaic
(
    MosaicDescriptor *mosaic,
    int32 old_fid,
    int32 new_fid
)

{
    int    j = 0;
    size_t i;
    int32 old_sds, new_sds;
    int32 nattr, attr_index;
    int32 data_type, n_values;
    char attr_name[1024], sds_name[1024], *buffer;
    int32 rank;
    int32 dims[10];
    int16 dtm_code;

    /* loop through the SDS's */
    for ( i = 0; i < mosaic->nbands; i++ )
    {
	/* if SDS was reprojected */
	if ( mosaic->bandinfo[i].selected )
        {
	    /* open it */
            old_sds = SDselect( old_fid, mosaic->bandinfo[i].fieldnum );
            new_sds = SDselect( new_fid, j++ );

	    /* get various SDS info */
	    SDgetinfo( old_sds, sds_name, &rank, dims, &data_type, &nattr );

	    /* for each SDS attribute */
            for ( attr_index = 0; attr_index < nattr; attr_index++ )
            {
		/* get SDS attribute info */
                SDattrinfo( old_sds, attr_index, attr_name, &data_type,
                    &n_values );

                /* if this is the datum, then don't read it. instead use
                   the output datum in the mosaic descriptor */
                if ( strcmp( attr_name, "HorizontalDatumName" ) != 0 )
                {   /* not the datum attribute */
                    /* allocate memory */
		    switch (data_type)
		    {
		        case 6:
		        case 26:
		        case 27:
		            buffer = calloc(n_values, 8);
		            break;
		        case 5:
		        case 24:
		        case 25:
		            buffer = calloc(n_values, 4);
		            break;
		        case 22:
		        case 23:
		            buffer = calloc(n_values, 2);
		            break;
		        default:
		            buffer = calloc(n_values, 1);
		            break;
		    }

		    /* get SDS attribute values */
                    SDreadattr( old_sds, attr_index, buffer );

		    /* write SDS attribute to output file */
                    SDsetattr( new_sds, attr_name, data_type, n_values,
                        buffer );

		    /* free memory */
	            free( buffer );
                }
            }

            /* add the datum value to the attributes for this band */
            dtm_code = mosaic->datum_code;
            SDsetattr( new_sds, "HorizontalDatumName", DFNT_INT16, 1,
                (void *) &dtm_code );

	    /* close current SDS */
            SDendaccess( old_sds );
            SDendaccess( new_sds );
        }
    }

    return 0;
}

/*************************************************************************

Name: CreateAttributesMosaic

Author: Gail Schmidt 07/2002

Create HDF SDS attributes from data in the input parameter file and raw
binary file.

Note: should check various function return values.

*************************************************************************/

intn CreateAttributesMosaic
(
    MosaicDescriptor *mosaic,
    int32 new_fid
)

{
    int    j = 0;
    size_t i;
    int32 new_sds;
    int16 dtm_code;
    char *band_name = NULL;
    int8 int8_valid_range[2], int8_fill_value;
    uint8 uint8_valid_range[2], uint8_fill_value;
    int16 int16_valid_range[2], int16_fill_value;
    uint16 uint16_valid_range[2], uint16_fill_value;
    int32 int32_valid_range[2], int32_fill_value;
    uint32 uint32_valid_range[2], uint32_fill_value;
    float32 float32_valid_range[2], float32_fill_value;

    /* loop through the SDS's */
    for ( i = 0; i < mosaic->nbands; i++ )
    {
	/* if SDS was reprojected */
	if ( mosaic->bandinfo[i].selected )
        {
	    /* open the new SDS */
            new_sds = SDselect( new_fid, j++ );

            /* add the band name to the attributes for this band */
            band_name = strdup( mosaic->bandinfo[i].name );
            if ( band_name == NULL )
                return 1;
            SDsetattr( new_sds, "long_name", DFNT_CHAR, strlen( band_name ),
                (void *) band_name );
            free( band_name );

            switch ( mosaic->bandinfo[i].output_datatype )
            {
                case ( DFNT_INT8 ):
                    /* add the min value and max value to the attributes for
                       this band */
                    int8_valid_range[0] = (int8) mosaic->bandinfo[i].min_value;
                    int8_valid_range[1] = (int8) mosaic->bandinfo[i].max_value;
                    SDsetattr( new_sds, "valid_range", DFNT_INT8, 2,
                        int8_valid_range );

                    /* add the fill value to the attributes for this band */
                    int8_fill_value =
                        (int8) mosaic->bandinfo[i].background_fill;
                    SDsetattr( new_sds, "_FillValue", DFNT_INT8, 1,
                        &int8_fill_value );
                    break;

                case ( DFNT_UINT8 ):
                    /* add the min value and max value to the attributes for
                       this band */
                    uint8_valid_range[0] =
                        (uint8) mosaic->bandinfo[i].min_value;
                    uint8_valid_range[1] =
                        (uint8) mosaic->bandinfo[i].max_value;
                    SDsetattr( new_sds, "valid_range", DFNT_UINT8, 2,
                        uint8_valid_range );

                    /* add the fill value to the attributes for this band */
                    uint8_fill_value =
                        (uint8) mosaic->bandinfo[i].background_fill;
                    SDsetattr( new_sds, "_FillValue", DFNT_UINT8, 1,
                        &uint8_fill_value );
                    break;

                case ( DFNT_INT16 ):
                    /* add the min value and max value to the attributes for
                       this band */
                    int16_valid_range[0] =
                        (int16) mosaic->bandinfo[i].min_value;
                    int16_valid_range[1] =
                        (int16) mosaic->bandinfo[i].max_value;
                    SDsetattr( new_sds, "valid_range", DFNT_INT16, 2,
                        int16_valid_range );

                    /* add the fill value to the attributes for this band */
                    int16_fill_value =
                        (int16) mosaic->bandinfo[i].background_fill;
                    SDsetattr( new_sds, "_FillValue", DFNT_INT16, 1,
                        &int16_fill_value );
                    break;

                case ( DFNT_UINT16 ):
                    /* add the min value and max value to the attributes for
                       this band */
                    uint16_valid_range[0] =
                        (uint16) mosaic->bandinfo[i].min_value;
                    uint16_valid_range[1] =
                        (uint16) mosaic->bandinfo[i].max_value;
                    SDsetattr( new_sds, "valid_range", DFNT_UINT16, 2,
                        uint16_valid_range );

                    /* add the fill value to the attributes for this band */
                    uint16_fill_value =
                        (uint16) mosaic->bandinfo[i].background_fill;
                    SDsetattr( new_sds, "_FillValue", DFNT_UINT16, 1,
                        &uint16_fill_value );
                    break;

                case ( DFNT_INT32 ):
                    /* add the min value and max value to the attributes for
                       this band */
                    int32_valid_range[0] =
                        (int32) mosaic->bandinfo[i].min_value;
                    int32_valid_range[1] =
                        (int32) mosaic->bandinfo[i].max_value;
                    SDsetattr( new_sds, "valid_range", DFNT_INT32, 2,
                        int32_valid_range );

                    /* add the fill value to the attributes for this band */
                    int32_fill_value =
                        (int32) mosaic->bandinfo[i].background_fill;
                    SDsetattr( new_sds, "_FillValue", DFNT_INT32, 1,
                        &int32_fill_value );
                    break;

                case ( DFNT_UINT32 ):
                    /* add the min value and max value to the attributes for
                       this band */
                    uint32_valid_range[0] =
                        (uint32) mosaic->bandinfo[i].min_value;
                    uint32_valid_range[1] =
                        (uint32) mosaic->bandinfo[i].max_value;
                    SDsetattr( new_sds, "valid_range", DFNT_UINT32, 2,
                        uint32_valid_range );

                    /* add the fill value to the attributes for this band */
                    uint32_fill_value =
                        (uint32) mosaic->bandinfo[i].background_fill;
                    SDsetattr( new_sds, "_FillValue", DFNT_UINT32, 1,
                        &uint32_fill_value );
                    break;

                case ( DFNT_FLOAT32 ):
                    /* add the min value and max value to the attributes for
                       this band */
                    float32_valid_range[0] =
                        (float32) mosaic->bandinfo[i].min_value;
                    float32_valid_range[1] =
                        (float32) mosaic->bandinfo[i].max_value;
                    SDsetattr( new_sds, "valid_range", DFNT_FLOAT32,
                        2, float32_valid_range );

                    /* add the fill value to the attributes for this band */
                    float32_fill_value =
                        (float32) mosaic->bandinfo[i].background_fill;
                    SDsetattr( new_sds, "_FillValue", DFNT_FLOAT32, 1,
                        &float32_fill_value );
                    break;
            }

            /* add the datum value to the attributes for this band */
            dtm_code = mosaic->datum_code;
            SDsetattr( new_sds, "HorizontalDatumName", DFNT_INT16, 1,
                (void *) &dtm_code ); 

	    /* close current SDS */
            SDendaccess( new_sds );
        }
    }

    return 0;
}

/*************************************************************************

MODULE: TransferMetadataMosaic

PURPOSE: 
    This module sends requests to TransferAttrMosaic to append user-defined
    core, archive, and structural metadata to the new HDF file from the old
    HDF files. An <r,c> tag will be placed in the groupname of the old
    metadata to represent the row, column placement of that file in the
    mosaic.
 

RETURN VALUE:
Type = intn
Value    	Description
-----		-----------
MRT_NO_ERROR	Returns MRT_NO_ERROR if the metadata appending operation was
                successful.  Error codes found in mrt_error.c and mrt_error.h.
-15             Returns ERROR_READ_INPUTIMAGE if the old HDF file can't be
                read.
-17		Returns ERROR_WRITE_OUTPUTIMAGE if the new HDF file can't
   		be written to.


HISTORY:
Version    Date     Programmer      Code     Reason
-------    ----     ----------      ----     ------
           07/02    Gail Schmidt             Original Development

NOTES:

**************************************************************************/

intn TransferMetadataMosaic
(
    int32 old_fid,	/* I: the SDS file id # for the old HDF file */ 
    int32 new_fid,      /* I: the SDS file id # for the new HDF file */ 
    int row,            /* I: row placement of the old file in the mosaic */
    int col             /* I: column placement of the old file in the mosaic */
)

{
    intn i;                /* used to loop through attrs */
    intn j;
    intn status;           /* receives result of processing
                              from transferattr function */     
    char attrname[256];    /* holds the file_name string */ 
    char *root = NULL;     /* will point to attr type */

    /* set up a loop to check for and transfer the three
       types of metadata -- structural, core, and archive */
    for ( i = 0; i < 3; i++ )
    {
        /* set up root to hold correct string */ 
        switch( i )
        {
            case 0:
                root = "StructMetadata";
                break;
            case 1:
                root = "CoreMetadata";
                break;
            case 2:
                root = "ArchiveMetadata";
                break;
        }  

        /* First, try root name alone */
        status = TransferAttrMosaic( old_fid, new_fid, row, col, root );
        if ( status != MRT_NO_ERROR )
        {
            /* Now, try concatenating sequence numbers */
            for ( j = 0; j <= 9; j++ )
            {
                sprintf( attrname, "%s.%d", root, j );
	        status = TransferAttrMosaic( old_fid, new_fid, row, col,
                    attrname );
                if ( status == MRT_NO_ERROR )
                    break;
            }
        }
    }

    /* return the value of the function to main function */
    return ( status );
}


/*************************************************************************

MODULE: TransferAttrMosaic

PURPOSE: 
    This module reads the user-defined core, archive, and structural metadata
    from the original (input) HDF file and writes it as new user-defined
    metadata in the new HDF file.  The new file-level attribute namefields in
    the output HDF file are of the form "Old+<original field name>+<r,c>"
    and can be extracted from the output HDF file using the SDfindattr and
    SDattrinfo commands from the HDf library.

RETURN VALUE:
Type = int
Value    	Description
-----		-----------
MRT_NO_ERROR	Returns MRT_NO_ERROR if the metadata appending operation was
                successful.  Error codes found in mrt_error.c and mrt_error.h.
-4         	Returns ERROR_MEMORY if strdup mem allocation fails.
-15             Returns ERROR_READ_INPUTIMAGE if the old HDF file can't be
                read from.
-17		Returns ERROR_WRITE_OUTPUTIMAGE if the new HDF file can't
   		be written to.


HISTORY:
Version    Date     Programmer      Code     Reason
-------    ----     ----------      ----     ------
           07/02    Gail Schmidt             Original Development

NOTES:

**************************************************************************/
intn TransferAttrMosaic
(
    int32 fid_old,	/* I: the SDS id # for the old HDF file */ 
    int32 fid_new,	/* I: the SDS id # for the new HDF file */ 
    int row,            /* I: row placement of the old file in the mosaic */
    int col,            /* I: column placement of the old file in the mosaic */
    char *attr		/* I: the filename handle of the attribute to move */ 
)

{
    intn status = MRT_NO_ERROR;       /* this is the var that holds return val */
    int32 my_attr_index;          /* holds return val from SDfindattr */ 
    int32 data_type;	          /* holds attribute's data type */ 
    int32 n_values;               /* stores # of vals of the attribute */
    char *file_data = NULL,       /* char ptr used to allocate temp space
                                     during transfer of attribute info */
         attr_name[MAX_NC_NAME],  /* holds attribute's name */
         new_attr_name[MAX_NC_NAME + 3],  /* holds new attr name */
         tmpstr[SMALL_STRING];    /* temporary string */

    /* look for attribute in the old HDF file */
    my_attr_index = SDfindattr( fid_old, attr );

    /* only proceed if the attribute was found */
    if ( my_attr_index == -1 )
    {
        return( ERROR_READ_INPUTIMAGE );
    }
 
    /* get size of old HDF file attribute  */
    status = SDattrinfo( fid_old, my_attr_index, attr_name, &data_type,
        &n_values );

    /* only proceed if successful read of attribute info */
    if ( status == -1 )
    {
        return( ERROR_READ_INPUTIMAGE );
    } 

    /* attempt to allocate memory for old HDF file attribute contents (add
       one character for the end of string character) */
    file_data = ( char * ) calloc( n_values+1, sizeof( char ) );
    if ( file_data == NULL )
    {
	fprintf( stdout, "Error: Unable to allocate %d bytes for %s\n",
	    (int) n_values, attr );
        fflush( stdout );
	return ( ERROR_MEMORY );
    }

    /* read attribute from the old HDF file */
    status = SDreadattr( fid_old, my_attr_index, file_data );
    if ( status == -1 || !strcmp( file_data, "" ) )
    {
        /* first free the allocated memory */
        free( file_data );
 
	return( ERROR_READ_INPUTIMAGE );
    }

    /* modify the attribute name from old HDF file prior to appending
       metadata to new HDF file; put prefix "Old" in front of old name
       and place postfix _r#_c# after the old name */
    strcpy( new_attr_name, "Old" );
    strcat( new_attr_name, attr_name );
    sprintf( tmpstr, "_r%d_c%d", row, col );
    strcat( new_attr_name, tmpstr );
    new_attr_name[ strlen( new_attr_name ) + 1 ] = '\0';

    /* attempt to write old metadata to output HDF file */
    status = SDsetattr( fid_new, new_attr_name, DFNT_CHAR8,
        strlen(file_data), (VOIDP)file_data );
 
    /* free dynamically allocated memory */
    free( file_data );

    /* return the value of status */
    return ( status );
}


/*************************************************************************

MODULE: write_bounding_coords

PURPOSE:
    This module sends requests to write_bounds_attr to write the archive,
    core, and structural metadata to the HDF file.  The metadata is read
    from the first input file and the bounding rectangular coordinates
    are searched for.  Once these coords are found, then the bounding
    rectangular coords are output to the mosaic hdf file.

RETURN VALUE:
Type = intn
Value           Description
-----           -----------
MRT_NO_ERROR        Returns MRT_NO_ERROR if the metadata read was successful and
                the bounding rectangular coordinates were found.  Error
                codes found in mrt_error.c and mrt_error.h.
-15             Returns ERROR_READ_INPUTIMAGE if the HDF file can't be
                read or the bounding rectangular coordinates were not
                found.


HISTORY:
Version    Date     Programmer      Code     Reason
-------    ----     ----------      ----     ------
           07/02    Gail Schmidt             Original development

NOTES:

**************************************************************************/

intn write_bounding_coords
(
    int32 in_sd_id,           /* I: the SDS file id # for the old HDF file */
    int32 out_sd_id,          /* I: the SDS id # for the output HDF file */
    MosaicDescriptor *mosaic  /* I: file descriptor (bounding coords are
                                    present in this structure) */
)

{
    intn j;                /* looping variable */
    intn status;           /* return status */
    char attrname[256];    /* holds the file_name string */

    /* First, ArchiveMetadata alone */
    status = write_bounds_attr( in_sd_id, out_sd_id, "ArchiveMetadata",
        mosaic);
    if ( status == MRT_NO_ERROR )
        return ( status );

    /* Try concatenating sequence numbers */
    for ( j = 0; j <= 9; j++ )
    {
        sprintf( attrname, "%s.%d", "ArchiveMetadata", j );
        status = write_bounds_attr( in_sd_id, out_sd_id, attrname, mosaic );
        if ( status == MRT_NO_ERROR )
            return ( status );
    }

    /* Now try CoreMetadata alone */
    status = write_bounds_attr( in_sd_id, out_sd_id, "CoreMetadata", mosaic );
    if ( status == MRT_NO_ERROR )
        return ( status );

    /* Try concatenating sequence numbers */
    for ( j = 0; j <= 9; j++ )
    {
        sprintf( attrname, "%s.%d", "CoreMetadata", j );
        status = write_bounds_attr( in_sd_id, out_sd_id, attrname, mosaic );
        if ( status == MRT_NO_ERROR )
            return ( status );
    }

    /* Last, try StructMetadata alone */
    status = write_bounds_attr( in_sd_id, out_sd_id, "StructMetadata", mosaic );
    if ( status == MRT_NO_ERROR )
        return ( status );

    /* Try concatenating sequence numbers */
    for ( j = 0; j <= 9; j++ )
    {
        sprintf( attrname, "%s.%d", "StructMetadata", j );
        status = write_bounds_attr( in_sd_id, out_sd_id, attrname, mosaic );
        if ( status == MRT_NO_ERROR )
            return ( status );
    }

    /* return the value of the function to main function */
    return ( status );

}


/*************************************************************************

MODULE: write_bounds_attr

PURPOSE:
    This module will write the bounding rectangular coordinates for the
    entire mosaic to the output mosaic .hdf file.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
MRT_NO_ERROR        Returns MRT_NO_ERROR if any of the bounding rectangular coords
                were found in the metadata.  Error codes found in mrt_error.c
                and mrt_error.h.
-4              Returns ERROR_MEMORY if strdup mem allocation fails.
-15             Returns ERROR_READ_INPUTIMAGE if the HDF file can't be
                read from or if the bounding rectangular coords were
                not found in the metadata.


HISTORY:
Version    Date     Programmer      Code     Reason
-------    ----     ----------      ----     ------
           07/02    Gail Schmidt             Original development

NOTES:

**************************************************************************/
intn write_bounds_attr
(
    int32 in_sd_id,          /* I: the SDS id # for the input HDF file */
    int32 out_sd_id,         /* I: the SDS id # for the output HDF file */
    char *attr,              /* I: attribute name to read */
    MosaicDescriptor *mosaic /* I: file descriptor (bounding coords are
                                   present in this structure) */
)

{
    int i;                  /* looping variable */
    intn status;            /* this is the var that holds return val */
    int32 my_attr_index;    /* holds return val from SDfindattr */
    int32 data_type;        /* holds attribute's data type */
    int32 n_values;         /* stores # of vals of the attribute */
    int num_chars;          /* number of characters read in the line */
    char *file_data = NULL; /* char ptr used to allocate temp space during
                               transfer of attribute info */
    char *file_data_ptr = NULL;
                            /* pointer to file_data for scanning */
    char tmpstr[SMALL_STRING];    /* temporary string for bounding coord */
    char attr_name[MAX_NC_NAME];  /* holds attributes name */
    char token_buffer[256]; /* holds the current token */
    int found_north_bound;  /* north bounding rectangle coord been found? */
    int found_south_bound;  /* south bounding rectangle coord been found? */
    int found_east_bound;   /* east bounding rectangle coord been found? */
    int found_west_bound;   /* west bounding rectangle coord been found? */
    int digit_count;        /* counter for the digits in the bounding coord */

    /* look for attribute in the input HDF file */
    my_attr_index = SDfindattr( in_sd_id, attr );

    /* only proceed if the attribute was found */
    if ( my_attr_index == -1 )
    {
        return( ERROR_READ_INPUTIMAGE );
    }

    /* get size of HDF file attribute */
    status = SDattrinfo( in_sd_id, my_attr_index, attr_name, &data_type,
        &n_values );
    if ( status == -1 )
    {
        return ( ERROR_READ_INPUTIMAGE );
    }

    /* attempt to allocate memory for HDF file attribute contents (add one
       character for the end of string character) */
    file_data = ( char * ) calloc ( n_values+1, sizeof(char) );
    if ( file_data == NULL )
    {
        fprintf( stdout, "Error: Unable to allocate %d bytes for %s\n",
            (int) n_values, attr );
        fflush( stdout );
        return ( ERROR_MEMORY );
    }

    /* read attribute from the HDF file */
    status = SDreadattr( in_sd_id, my_attr_index, file_data );
    if ( status == -1 || !strcmp( file_data, "" ) )
    {
        /* first free the allocated memory */
        free ( file_data );
        return ( ERROR_READ_INPUTIMAGE );
    }

    /* update the bounding coordinates values to represent the entire mosaic
       and not just the first tile in the mosaic */
    found_north_bound = FALSE;
    found_south_bound = FALSE;
    found_east_bound = FALSE;
    found_west_bound = FALSE;
    file_data_ptr = file_data;
    status = ERROR_READ_INPUTIMAGE;
    while ( sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars ) != EOF )
    {
        /* if this token is END, then we are done with the metadata */
        if ( !strcmp ( token_buffer, "END" ) )
            break;

        /* if all the bounding coords have been found, don't waste time
           with the rest of the metadata */
        if ( found_north_bound && found_south_bound && found_east_bound &&
            found_west_bound )
        {
            break;
        }

        /* increment the file_data_ptr pointer to point to the next token */
        file_data_ptr += num_chars;

        /* look for the NORTHBOUNDINGCOORDINATE token */
        if ( !found_north_bound &&
             !strcmp ( token_buffer, "NORTHBOUNDINGCOORDINATE" ) )
        {
            /* read the next three tokens, this should be the
               NUM_VAL = ... line */
            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            /* read the next three tokens, this should be the
               VALUE = ... line */
            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            /* this token is the value of the bounding coord */
            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );

            /* get a string for the bounding coord value */
            sprintf( tmpstr, "%.6f", mosaic->north_bound );

            /* write the new bounding coord. if new coord value is not
               "long" enough, then fill with blank spaces. make sure the
               we only replace digits or the decimal point. */
            digit_count = 0;
            for ( i = 0; i < num_chars; i++ )
            {
                if ( isdigit( (int) file_data_ptr[i] ) ||
                     file_data_ptr[i] == '.' || file_data_ptr[i] == '-' )
                {
                    /* don't write an end of string character from the
                       new bounding string */
                    if ( tmpstr[digit_count] != '\0' )
                    {
                        file_data_ptr[i] = tmpstr[digit_count];
                        digit_count++;
                    }
                    else
                    {
                        /* we're at the end of the new string so break out */
                        break;
                    }
                }
            }

            /* increment the pointer and move on */
            file_data_ptr += num_chars;
            found_north_bound = TRUE;
            status = MRT_NO_ERROR;
            continue;  /* don't waste time looking for the other
                          bounding rectangles with this token */
        }

        /* look for the SOUTHBOUNDINGCOORDINATE token */
        if ( !found_south_bound &&
             !strcmp ( token_buffer, "SOUTHBOUNDINGCOORDINATE" ) )
        {
            /* read the next three tokens, this should be the
               NUM_VAL = ... line */
            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            /* read the next three tokens, this should be the
               VALUE = ... line */
            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            /* this token is the value of the bounding coord */
            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );

            /* get a string for the bounding coord value */
            sprintf( tmpstr, "%.6f", mosaic->south_bound );

            /* write the new bounding coord. if new coord value is not
               "long" enough, then fill with blank spaces. make sure the
               we only replace digits or the decimal point. */
            digit_count = 0;
            for ( i = 0; i < num_chars; i++ )
            {
                if ( isdigit( (int) file_data_ptr[i] ) ||
                     file_data_ptr[i] == '.' || file_data_ptr[i] == '-' )
                {
                    /* don't write an end of string character from the
                       new bounding string */
                    if ( tmpstr[digit_count] != '\0' )
                    {
                        file_data_ptr[i] = tmpstr[digit_count];
                        digit_count++;
                    }
                    else
                    {
                        /* we're at the end of the new string so break out */
                        break;
                    }
                }
            }

            /* increment the pointer and move on */
            file_data_ptr += num_chars;
            found_south_bound = TRUE;
            status = MRT_NO_ERROR;
            continue;  /* don't waste time looking for the other
                          bounding rectangles with this token */
        }

        /* look for the EASTBOUNDINGCOORDINATE token */
        if ( !found_east_bound &&
             !strcmp ( token_buffer, "EASTBOUNDINGCOORDINATE" ) )
        {
            /* read the next three tokens, this should be the
               NUM_VAL = ... line */
            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            /* read the next three tokens, this should be the
               VALUE = ... line */
            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            /* this token is the value of the bounding coord */
            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );

            /* get a string for the bounding coord value */
            sprintf( tmpstr, "%.6f", mosaic->east_bound );

            /* write the new bounding coord. if new coord value is not
               "long" enough, then fill with blank spaces. make sure the
               we only replace digits or the decimal point. */
            digit_count = 0;
            for ( i = 0; i < num_chars; i++ )
            {
                if ( isdigit( (int) file_data_ptr[i] ) ||
                     file_data_ptr[i] == '.' || file_data_ptr[i] == '-' )
                {
                    /* don't write an end of string character from the
                       new bounding string */
                    if ( tmpstr[digit_count] != '\0' )
                    {
                        file_data_ptr[i] = tmpstr[digit_count];
                        digit_count++;
                    }
                    else
                    {
                        /* we're at the end of the new string so break out */
                        break;
                    }
                }
            }

            /* increment the pointer and move on */
            file_data_ptr += num_chars;
            found_east_bound = TRUE;
            status = MRT_NO_ERROR;
            continue;  /* don't waste time looking for the other
                          bounding rectangles with this token */
        }

        /* look for the WESTBOUNDINGCOORDINATE token */
        if ( !found_west_bound &&
             !strcmp ( token_buffer, "WESTBOUNDINGCOORDINATE" ) )
        {
            /* read the next three tokens, this should be the
               NUM_VAL = ... line */
            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            /* read the next three tokens, this should be the
               VALUE = ... line */
            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            /* this token is the value of the bounding coord */
            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );

            /* get a string for the bounding coord value */
            sprintf( tmpstr, "%.6f", mosaic->west_bound );

            /* write the new bounding coord. if new coord value is not
               "long" enough, then fill with blank spaces. make sure the
               we only replace digits or the decimal point. */
            digit_count = 0;
            for ( i = 0; i < num_chars; i++ )
            {
                if ( isdigit( (int) file_data_ptr[i] ) ||
                     file_data_ptr[i] == '.' || file_data_ptr[i] == '-' )
                {
                    /* don't write an end of string character from the
                       new bounding string */
                    if ( tmpstr[digit_count] != '\0' )
                    {
                        file_data_ptr[i] = tmpstr[digit_count];
                        digit_count++;
                    }
                    else
                    {
                        /* we're at the end of the new string so break out */
                        break;
                    }
                }
            }

            /* increment the pointer and move on */
            file_data_ptr += num_chars;
            found_west_bound = TRUE;
            status = MRT_NO_ERROR;
            continue;  /* don't waste time looking for the other
                          bounding rectangles with this token */
        }
    }

    /* create the attribute (with the updated bounding coordinates) in the
       output HDF file, if this attribute contains the bounding coords */
    if ( found_north_bound && found_south_bound && found_east_bound &&
         found_west_bound )
    {
        status = SDsetattr( out_sd_id, attr_name, data_type, n_values,
            file_data );
        if ( status == -1 )
        {
            /* first free the allocated memory */
            free ( file_data );
            return ( ERROR_WRITE_OUTPUTIMAGE );
        }
    }

    /* free dynamically allocated memory */
    free ( file_data );

    /* if any of the bounding rectangular coords were found then MRT_NO_ERROR
       is returned, OW ERROR_READ_INPUTIMAGE is returned */
    return ( status );
}

