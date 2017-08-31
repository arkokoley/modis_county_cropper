/************************************************************************

FILE: addmeta.c

PURPOSE:  Attach metadata from input hdf file to output hdf file.

HISTORY:
Version    Date     Programmer      Code     Reason
-------    ----     ----------      ----     ------
1.0        UK       Doug Ilg                 Original Development 
           02/01    John Rishea              Modified original file
                                             to use with ModisTool
           05/01    John Weiss               Handle HDF attributes

HARDWARE AND/OR SOFTWARE LIMITATIONS:
    None

PROJECT: 	Modis Reprojection Tool

NOTES:   This program is an adaptation of metadmp.c, developed by Doug
Ilg (Doug.Ilg@gsfc.nasa.gov).

*************************************************************************/

#include "shared_mosaic.h"
#include "mrt_error.h"
#include "mfhdf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/************** LOCAL PROTOTYPES *****************************************/ 
intn TransferMetadata( int32 old_fid, int32 new_fid );
intn TransferAttributes( ModisDescriptor *modis, int32 old_fid, int32 new_fid );
intn CreateAttributes( ModisDescriptor *modis, int32 new_fid );
intn TransferAttr( int32 fid_old, int32 fid_new, char *attr );


/*************************************************************************

MODULE: AppendMetadata

PURPOSE: 
    This module allows EOSDIS user-defined core, archive, and structural
    metadata from the original (input) HDF file to be appended to the new
    (output) HDF file.  Specifically, the metadata from the old file is
    first extracted and then added to the new file as a string of characters
    assigned to a file-level attribute tag.  The user-defined metadata fields
    are assigned names of "OldCoreMetadata", "OldArchiveMetadata", and
    "OldStructMetadata." These metadata fields can be extracted using the
    SDfindattr and SDattrinfo commands from the HDF library.
 

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
           02/01    John Rishea              Original Development based on
                                             Doug Ilg's metadmp.c program.
           02/01    John Weiss               Adapt for use in MRT resampler.
           05/01    John Weiss               Handle HDF attributes

NOTES:

**************************************************************************/

intn AppendMetadata
(
    ModisDescriptor *modis	/* I:  session info */
)

{
    intn status;       /* used to capture return val frm SD ops */
    intn trans_status; /* used to capture return val frm subfunct */
    int32 old_sd_id;   /* holds the id# for the old HDF file */
    int32 new_sd_id;   /* holds the id# for the new HDF file */ 
    char *old_hdf_file = modis->input_filename;
                       /* the HDF file we want to get metadata from */
    char *new_hdf_file = modis->output_filename;
                       /* the HDF file we want to append metadata to */

    /* Open the new HDF file for writing */
    new_sd_id = SDstart( new_hdf_file, DFACC_WRITE );
    if ( new_sd_id == -1 )
    {
	fprintf( stdout, "Error: unable to open file %s \n", new_hdf_file );
        fflush( stdout );
	return( ERROR_OPEN_OUTPUTIMAGE );
    }

    /* add metadata and attributes if processing HDF-EOS to HDF-EOS */
    if ( modis->input_filetype == HDFEOS && modis->output_filetype == HDFEOS )
    {
        /* Open the old HDF file for reading */
        old_sd_id = SDstart( old_hdf_file, DFACC_READ );
        if ( old_sd_id == -1 )
        {
            /* first close the output file */
            status = SDend( new_sd_id );
 
            fprintf( stdout, "Error: unable to open file %s \n", old_hdf_file );
            fflush( stdout );
            return( ERROR_OPEN_INPUTIMAGE );
        }

        /* transfer attributes from old HDF file to new HDF file */
        trans_status = TransferAttributes( modis, old_sd_id, new_sd_id );

        if ( trans_status == -1 )
        {
            fprintf( stdout, "Warning: unable to transfer attributes from\n" );
            fprintf( stdout, "%s to %s\n", old_hdf_file, new_hdf_file );
            fflush( stdout );
        }

        /* transfer metadata from old HDF file to new HDF file */
        trans_status = TransferMetadata( old_sd_id, new_sd_id );

        /* if the transfer was unsuccessful, return an error */
        if ( trans_status == -1 )
        {
            /* first close the input and output HDF files */
            status = SDend( old_sd_id );
            status = SDend( new_sd_id );
 
            fprintf( stdout, "Warning: unable to append metadata from\n" );
            fprintf( stdout, "%s to %s\n", old_hdf_file, new_hdf_file );
            fflush( stdout );
    	    return( trans_status );
        }

        /* close the old HDF file */
        status = SDend( old_sd_id);
    }

    /* otherwise just add the attributes if processing raw binary to HDF-EOS */
    else if ( modis->input_filetype == RAW_BINARY &&
              modis->output_filetype == HDFEOS )
    {
        /* transfer attributes from old HDF file to new HDF file */
        trans_status = CreateAttributes( modis, new_sd_id );

        if ( trans_status == -1 )
        {
            fprintf( stdout, "Warning: unable to create attributes for\n" );
            fprintf( stdout, "%s\n", new_hdf_file );
            fflush( stdout );
        }
    }

    /* close the new HDF file */
    status = SDend( new_sd_id );

    /* return the status value */
    return( status );
}

/*************************************************************************

intn TransferAttributes( ModisDescriptor *modis, int32 old_fid, int32 new_fid );

Author: JMW 05/01/01
        GLS 01/26/02  Must handle outputting the datum value

Transfer HDF SDS attributes from input to output, pretty much intact.
Global attributes (HDF-EOS version, metadata) are handled elsewhere.

Note: should check various function return values.

*************************************************************************/

intn TransferAttributes( ModisDescriptor *modis, int32 old_fid, int32 new_fid )
{
    size_t i;
    int    j = 0;
    int32 old_sds, new_sds;
    int32 nattr, attr_index;
    int32 data_type, n_values;
    char attr_name[1024], sds_name[1024], *buffer;
    int32 rank;
    int32 dims[10];
    int16 dtm_code;

    /* loop through the SDS's */
    for ( i = 0; i < modis->nbands; i++ )
    {
	/* if SDS was reprojected */
	if ( modis->bandinfo[i].selected )
        {
	    /* open it */
            old_sds = SDselect( old_fid, modis->bandinfo[i].fieldnum );
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
                   the output datum in the modis descriptor */
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
	            free(buffer);
                }
            }

            /* add the datum value to the attributes for this band */
            dtm_code = modis->output_datum_code;
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

intn CreateAttributes( ModisDescriptor *modis, int32 new_fid );

Author: GLS 02/05/02

Create HDF SDS attributes from data in the input parameter file and raw
binary file.

Note: should check various function return values.

*************************************************************************/

intn CreateAttributes( ModisDescriptor *modis, int32 new_fid )
{
    size_t i;
    int    j = 0;
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
    for ( i = 0; i < modis->nbands; i++ )
    {
	/* if SDS was reprojected */
	if ( modis->bandinfo[i].selected )
        {
	    /* open the new SDS */
            new_sds = SDselect( new_fid, j++ );

            /* add the band name to the attributes for this band */
            band_name = strdup( modis->bandinfo[i].name );
            if ( band_name == NULL )
                return 1;
            SDsetattr( new_sds, "long_name", DFNT_CHAR, strlen( band_name ),
                (void *) band_name );
            free( band_name );

            switch ( modis->bandinfo[i].output_datatype )
            {
                case ( DFNT_INT8 ):
                    /* add the min value and max value to the attributes for
                       this band */
                    int8_valid_range[0] = (int8) modis->bandinfo[i].min_value;
                    int8_valid_range[1] = (int8) modis->bandinfo[i].max_value;
                    SDsetattr( new_sds, "valid_range", DFNT_INT8, 2,
                        int8_valid_range );

                    /* add the fill value to the attributes for this band */
                    int8_fill_value = (int8) modis->bandinfo[i].background_fill;
                    SDsetattr( new_sds, "_FillValue", DFNT_INT8, 1,
                        &int8_fill_value );
                    break;

                case ( DFNT_UINT8 ):
                    /* add the min value and max value to the attributes for
                       this band */
                    uint8_valid_range[0] = (uint8) modis->bandinfo[i].min_value;
                    uint8_valid_range[1] = (uint8) modis->bandinfo[i].max_value;
                    SDsetattr( new_sds, "valid_range", DFNT_UINT8, 2,
                        uint8_valid_range );

                    /* add the fill value to the attributes for this band */
                    uint8_fill_value =
                        (uint8) modis->bandinfo[i].background_fill;
                    SDsetattr( new_sds, "_FillValue", DFNT_UINT8, 1,
                        &uint8_fill_value );
                    break;

                case ( DFNT_INT16 ):
                    /* add the min value and max value to the attributes for
                       this band */
                    int16_valid_range[0] = (int16) modis->bandinfo[i].min_value;
                    int16_valid_range[1] = (int16) modis->bandinfo[i].max_value;
                    SDsetattr( new_sds, "valid_range", DFNT_INT16, 2,
                        int16_valid_range );

                    /* add the fill value to the attributes for this band */
                    int16_fill_value =
                        (int16) modis->bandinfo[i].background_fill;
                    SDsetattr( new_sds, "_FillValue", DFNT_INT16, 1,
                        &int16_fill_value );
                    break;

                case ( DFNT_UINT16 ):
                    /* add the min value and max value to the attributes for
                       this band */
                    uint16_valid_range[0] =
                        (uint16) modis->bandinfo[i].min_value;
                    uint16_valid_range[1] =
                        (uint16) modis->bandinfo[i].max_value;
                    SDsetattr( new_sds, "valid_range", DFNT_UINT16, 2,
                        uint16_valid_range );

                    /* add the fill value to the attributes for this band */
                    uint16_fill_value =
                        (uint16) modis->bandinfo[i].background_fill;
                    SDsetattr( new_sds, "_FillValue", DFNT_UINT16, 1,
                        &uint16_fill_value );
                    break;

                case ( DFNT_INT32 ):
                    /* add the min value and max value to the attributes for
                       this band */
                    int32_valid_range[0] =
                        (int32) modis->bandinfo[i].min_value;
                    int32_valid_range[1] =
                        (int32) modis->bandinfo[i].max_value;
                    SDsetattr( new_sds, "valid_range", DFNT_INT32, 2,
                        int32_valid_range );

                    /* add the fill value to the attributes for this band */
                    int32_fill_value =
                        (int32) modis->bandinfo[i].background_fill;
                    SDsetattr( new_sds, "_FillValue", DFNT_INT32, 1,
                        &int32_fill_value );
                    break;

                case ( DFNT_UINT32 ):
                    /* add the min value and max value to the attributes for
                       this band */
                    uint32_valid_range[0] =
                        (uint32) modis->bandinfo[i].min_value;
                    uint32_valid_range[1] =
                        (uint32) modis->bandinfo[i].max_value;
                    SDsetattr( new_sds, "valid_range", DFNT_UINT32, 2,
                        uint32_valid_range );

                    /* add the fill value to the attributes for this band */
                    uint32_fill_value =
                        (uint32) modis->bandinfo[i].background_fill;
                    SDsetattr( new_sds, "_FillValue", DFNT_UINT32, 1,
                        &uint32_fill_value );
                    break;

                case ( DFNT_FLOAT32 ):
                    /* add the min value and max value to the attributes for
                       this band */
                    float32_valid_range[0] =
                        (float32) modis->bandinfo[i].min_value;
                    float32_valid_range[1] =
                        (float32) modis->bandinfo[i].max_value;
                    SDsetattr( new_sds, "valid_range", DFNT_FLOAT32,
                        2, float32_valid_range );

                    /* add the fill value to the attributes for this band */
                    float32_fill_value =
                        (float32) modis->bandinfo[i].background_fill;
                    SDsetattr( new_sds, "_FillValue", DFNT_FLOAT32,
                        1, &float32_fill_value );
                    break;
            }

            /* add the datum value to the attributes for this band */
            dtm_code = modis->output_datum_code;
            SDsetattr( new_sds, "HorizontalDatumName", DFNT_INT16, 1,
                (void *) &dtm_code ); 

	    /* close current SDS */
            SDendaccess( new_sds );
        }
    }

    return 0;
}

/*************************************************************************

MODULE: TransferMetadata

PURPOSE: 
    This module sends requests to TransferAttr to append user-defined core,
    archive, and structural metadata to the new HDF file from the old HDF
    file.
 

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
           02/01    John Rishea              Original Development based on
                                             Doug Ilg's metadmp.c program.

NOTES:

**************************************************************************/

intn TransferMetadata
(
    int32 old_fid,	/* the SDS file id # for the old HDF file */ 
    int32 new_fid	/* the SDS file id # for the new HDF file */ 
)

{
    intn i;                /* used to loop through attrs */
    intn j;
    intn status;           /* receives result of processing
                              from transferattr function */     
    char attrname[256];    /* holds the file_name string */ 
    char *root = NULL;     /* will point to attr type */

    /* set up a loop to check for and transfer the three
       types of metadata -- core, archive, and structural */
    /* JMW 02/28/01 - reorder to structural, core, archive */
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
        status = TransferAttr( old_fid, new_fid, root );
        if ( status != MRT_NO_ERROR )
        {
            /* Now, try concatenating sequence numbers */
            for ( j = 0; j <= 9; j++ )
            {
                sprintf( attrname, "%s.%d", root, j );
	        status = TransferAttr( old_fid, new_fid, attrname );
                if ( status == MRT_NO_ERROR )
                    break;
            }
        }
    }  /* end outer for loop */

    /* return the value of the function to main function */
    return ( status );

}  /* end TransferMetadata function */


/*************************************************************************

MODULE: TransferAttr

PURPOSE: 
    This module reads the user-defined core, archive, and structural metadata
    from the original (input) HDF file and writes it as new user-defined
    metadata in the new HDF file.  The new file-level attribute namefields in
    the output HDF file are of the form "Old+<original field name>" and can 
    be extracted from the output HDF file using the SDfindattr and SDattrinfo
    commands from the HDf library.
 

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
           02/01    John Rishea              Original Development based on
                                             Doug Ilg's metadmp.c program.

NOTES:

**************************************************************************/
intn TransferAttr
(
    int32 fid_old,	/* the SDS id # for the old HDF file */ 
    int32 fid_new,	/* the SDS id # for the new HDF file */ 
    char *attr		/* the filename handle of the attribute to move */ 
)

{
    intn status = MRT_NO_ERROR; /* this is the var that holds return val */
    int32 my_attr_index;    /* holds return val from SDfindattr */ 
    int32 data_type;	    /* holds attribute's data type */ 
    int32 n_values;         /* stores # of vals of the attribute */
    char *file_data = NULL,         /* char ptr used to allocate temp space
                                     * during transfer of attribute info */
        attr_name[MAX_NC_NAME],            /* holds attribute's name */
        new_attr_name[MAX_NC_NAME + 3];    /* holds new attr name */

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
    file_data = ( char * ) calloc( n_values+1, sizeof(char) );

    if ( file_data == NULL )
    {
	fprintf( stdout, "Error: Unable to allocate %d bytes for %s\n",
	    (int) n_values, attr );
        fflush( stdout );
	return ( ERROR_MEMORY );
    }

    /* read attribute from the old HDF file */
    status = SDreadattr( fid_old, my_attr_index, file_data );
    if(status == -1 || !strcmp( file_data, "" ) )
    {
        /* first free the allocated memory */
        free( file_data );
 
	return( ERROR_READ_INPUTIMAGE );
    }

    /* modify the attribute name from old HDF file prior to appending
       metadata to new HDF file; put prefix "Old" in front of old name */
    strcpy( new_attr_name, "Old" );
    strcat( new_attr_name, attr_name );
    new_attr_name[ strlen(new_attr_name) + 1 ] = '\0';

    /* attempt to write old metadata to output HDF file */
    status = SDsetattr( fid_new, new_attr_name, DFNT_CHAR8,
        strlen(file_data), (VOIDP)file_data );
 
    /* free dynamically allocated memory */
    free( file_data );

    /* return the value of status to TransferMetadata */
    return ( status );
}

/*************************************************************************

MODULE: read_bounding_coords

PURPOSE:
    This module sends requests to read_metadata to read the archive, core,
    and structural metadata from the HDF file.  The metadata is read and
    the bounding rectangular coordinates are searched for.  Once these
    coords are found, then the routine returns.

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
           11/01    Gail Schmidt             Original development

NOTES:

**************************************************************************/

intn read_bounding_coords
(
    int32 sd_id,              /* I: the SDS file id # for the old HDF file */
    ModisDescriptor *modis,   /* I/O: file descriptor (bounding coords are
                                      updated in this structure) */
    int *all_coords_present   /* O: were all bounding coordinates present? */
)

{
    intn j;                /* looping variable */
    intn status;           /* return status */
    char attrname[256];    /* holds the file_name string */

    /* First, ArchiveMetadata alone */
    status = read_metadata( sd_id, "ArchiveMetadata", modis,
        all_coords_present );
    if ( status == MRT_NO_ERROR )
        return ( status );

    /* Try concatenating sequence numbers */
    for ( j = 0; j <= 9; j++ )
    {
        sprintf( attrname, "%s.%d", "ArchiveMetadata", j );
        status = read_metadata( sd_id, attrname, modis, all_coords_present );
        if ( status == MRT_NO_ERROR )
            return ( status );
    }

    /* Now try CoreMetadata alone */
    status = read_metadata( sd_id, "CoreMetadata", modis,
        all_coords_present );
    if ( status == MRT_NO_ERROR )
        return ( status );

    /* Try concatenating sequence numbers */
    for ( j = 0; j <= 9; j++ )
    {
        sprintf( attrname, "%s.%d", "CoreMetadata", j );
        status = read_metadata( sd_id, attrname, modis, all_coords_present );
        if ( status == MRT_NO_ERROR )
            return ( status );
    }

    /* Last, try StructMetadata alone */
    status = read_metadata( sd_id, "StructMetadata", modis,
        all_coords_present );
    if ( status == MRT_NO_ERROR )
        return ( status );

    /* Try concatenating sequence numbers */
    for ( j = 0; j <= 9; j++ )
    {
        sprintf( attrname, "%s.%d", "StructMetadata", j );
        status = read_metadata( sd_id, attrname, modis, all_coords_present );
        if ( status == MRT_NO_ERROR )
            return ( status );
    }

    /* return the value of the function to main function */
    return ( status );

}

/*************************************************************************

MODULE: read_metadata

PURPOSE:
    This module reads the user-defined metadata from the HDF file and
    determines the bounding rectangular coordinates.

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
           11/01    Gail Schmidt             Original development

NOTES:

**************************************************************************/
intn read_metadata
(
    int32 sd_id,             /* I: the SDS id # for the HDF file */
    char *attr,              /* I: the name handle of the attribute to read */
    ModisDescriptor *modis,  /* I/O: file descriptor (bounding coords are
                                     updated in this structure) */
    int *all_coords_present  /* O: were all bounding coordinates present? */
)

{
    intn status;            /* this is the var that holds return val */
    int32 my_attr_index;    /* holds return val from SDfindattr */
    int32 data_type;        /* holds attribute's data type */
    int32 n_values;         /* stores # of vals of the attribute */
    int num_chars;          /* number of characters read in the line */
    char *file_data = NULL; /* char ptr used to allocate temp space during
                               transfer of attribute info */
    char *file_data_ptr = NULL;
                            /* pointer to file_data for scanning */
    char attr_name[MAX_NC_NAME];  /* holds attributes name */
    char token_buffer[256]; /* holds the current token */
    int found_north_bound;  /* north bounding rectangle coord been found? */
    int found_south_bound;  /* south bounding rectangle coord been found? */
    int found_east_bound;   /* east bounding rectangle coord been found? */
    int found_west_bound;   /* west bounding rectangle coord been found? */

    /* look for attribute in the HDF file */
    my_attr_index = SDfindattr( sd_id, attr );

    /* only proceed if the attribute was found */
    if ( my_attr_index == -1 )
    {
        return( ERROR_READ_INPUTIMAGE );
    }

    /* get size of HDF file attribute */
    status = SDattrinfo( sd_id, my_attr_index, attr_name, &data_type,
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
    status = SDreadattr( sd_id, my_attr_index, file_data );
    if ( status == -1 || !strcmp( file_data, "" ) )
    {
        /* first free the allocated memory */
        free ( file_data );

        return ( ERROR_READ_INPUTIMAGE );
    }

    /* walk through the file_data string one token at a time looking for the
       bounding rectangular coords */
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

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            /* find the value of the bounding coordinate */
            modis->north_bound = atof (token_buffer);
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

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            /* find the value of the bounding coordinate */
            modis->south_bound = atof (token_buffer);
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

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            /* find the value of the bounding coordinate */
            modis->east_bound = atof (token_buffer);
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

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            /* find the value of the bounding coordinate */
            modis->west_bound = atof (token_buffer);
            found_west_bound = TRUE;
            status = MRT_NO_ERROR;
            continue;  /* don't waste time looking for the other
                          bounding rectangles with this token */
        }
    }

    /* did we read all the bounding coordinates? */
    if ( found_north_bound && found_south_bound && found_east_bound &&
         found_west_bound )
    {
        *all_coords_present = TRUE;
    }
    else
    {
        *all_coords_present = FALSE;
    }

    /* free dynamically allocated memory */
    free ( file_data );

    /* if any of the bounding rectangular coords were found then MRT_NO_ERROR
       is returned, OW ERROR_READ_INPUTIMAGE is returned */
    return ( status );
}


/*************************************************************************

MODULE: read_tile_number_hdf

PURPOSE:
    This module reads the core metadata, searching for the horizontal and
    vertical tile numbers.

RETURN VALUE:
Type = intn
Value           Description
-----           -----------
MRT_NO_ERROR        Returns MRT_NO_ERROR if the metadata read was successful and
                the tile numbers were found.  Error codes found in mrt_error.c
                and mrt_error.h.
-15             Returns ERROR_READ_INPUTIMAGE if the HDF file can't be
                read or the tile numbers were not found.


HISTORY:
Version    Date     Programmer      Code     Reason
-------    ----     ----------      ----     ------
           06/02    Gail Schmidt             Original development

NOTES:

**************************************************************************/

int read_tile_number_hdf
(
    char filename[],          /* I: the filename of the input file */
    int *horiz,               /* O: horizontal tile number */
    int *vert                 /* O: vertical tile number */
)

{
    intn j;                /* looping variable */
    int32 my_attr_index;   /* holds return val from SDfindattr */
    int32 data_type;       /* holds attribute's data type */
    int32 n_values;        /* stores # of vals of the attribute */
    int32 sd_id = -1;      /* SDS id number */
    char attr_name[MAX_NC_NAME];  /* holds attributes name */
    intn status;           /* return status */
    char error_str[256];   /* string for error messages */
    char attrname[256];    /* holds the file_name string */
    char *file_data = NULL; /* char ptr used to allocate temp space during
                               transfer of attribute info */
    char *file_data_ptr = NULL;
                            /* pointer to file_data for scanning */
    int found_horiz;        /* horizontal tile number been found? */
    int found_vert;         /* vertical tile number been found? */
    int num_chars;          /* number of characters read in the line */
    char token_buffer[256]; /* holds the current token */
    char tmp_str[256];      /* temporary holder of the VALUE string */
    char class[256];        /* class string */
    char value[256];        /* value string */

    /* open as HDF file */
    sd_id = SDstart( filename, DFACC_RDONLY );
    if ( sd_id < 0 )
    {
        sprintf( error_str, "Unable to open %s for reading as SDS", filename );
        ErrorHandler( TRUE, "read_tile_number_hdf", ERROR_READ_INPUTHEADER,
            error_str );
    }

    /* look for the CoreMetadata in the HDF file */
    my_attr_index = SDfindattr( sd_id, "CoreMetadata" );

    /* only proceed if the attribute was found */
    if ( my_attr_index == -1 )
    {
        /* if not found then try concatenating sequence numbers */
        for ( j = 0; j <= 9; j++ )
        {
            sprintf( attrname, "%s.%d", "CoreMetadata", j );
            my_attr_index = SDfindattr( sd_id, attrname );
            if ( my_attr_index != -1 )
                break;
        }

        if ( my_attr_index == -1 )
        {
            sprintf( error_str, "Unable to locate CoreMetadata for reading" );
            ErrorHandler( TRUE, "read_tile_number_hdf", ERROR_READ_INPUTHEADER,
                error_str );
        }
    }

    /* get size of HDF file attribute */
    status = SDattrinfo( sd_id, my_attr_index, attr_name, &data_type,
        &n_values );
    if ( status == -1 )
    {
        sprintf( error_str, "Unable to get the size of the HDF attributes" );
        ErrorHandler( TRUE, "read_tile_number_hdf", ERROR_READ_INPUTHEADER,
            error_str );
    }

    /* attempt to allocate memory for HDF file attribute contents (add one
       character for the end of string character) */
    file_data = ( char * ) calloc ( n_values+1, sizeof(char) );
    if ( file_data == NULL )
    {
        sprintf( error_str, "Error: Unable to allocate %d bytes for %s\n",
            (int) n_values, attr_name );
        ErrorHandler( TRUE, "read_tile_number_hdf", ERROR_READ_INPUTHEADER,
            error_str );
        return ( ERROR_MEMORY );
    }

    /* read attribute from the HDF file */
    status = SDreadattr( sd_id, my_attr_index, file_data );
    if ( status == -1 || !strcmp( file_data, "" ) )
    {
        /* first free the allocated memory */
        free ( file_data );

        sprintf( error_str, "Unable to read the HDF attributes" );
        ErrorHandler( TRUE, "read_tile_number_hdf", ERROR_READ_INPUTHEADER,
            error_str );
    }

    /* walk through the file_data string one token at a time looking for the
       horizontal and vertical tile numbers */
    found_horiz = FALSE;
    found_vert = FALSE;
    file_data_ptr = file_data;
    status = ERROR_READ_INPUTIMAGE;
    while ( sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars ) != EOF )
    {
        /* if this token is END, then we are done with the metadata */
        if ( !strcmp ( token_buffer, "END" ) )
            break;

        /* if the horizontal and vertical tiles have been found, don't waste
           time with the rest of the metadata */
        if ( found_horiz && found_vert )
        {
            break;
        }

        /* increment the file_data_ptr pointer to point to the next token */
        file_data_ptr += num_chars;

        /* look for the ADDITIONALATTRIBUTENAME token */
        if ( !strcmp ( token_buffer, "ADDITIONALATTRIBUTENAME" ) )
        {
            /* read the next three tokens, this should be the
               CLASS = ... line */
            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            /* store the class name */
            strcpy( class, token_buffer );

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

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars );
            file_data_ptr += num_chars;

            /* store the value name */
            strcpy( value, token_buffer );

            /* if this is HORIZONTALTILENUMBER or VERTICALTILENUMBER, then
               look for the PARAMETERVALUE token */
            if ( !strcmp ( token_buffer, "\"HORIZONTALTILENUMBER\"" ) ||
                 !strcmp ( token_buffer, "\"VERTICALTILENUMBER\"" ) )
            {
                while ( sscanf (file_data_ptr, "%s%n", token_buffer,
                        &num_chars ) != EOF )
                {
                    /* if this token is END, then we are done with the
                       metadata */
                    if ( !strcmp ( token_buffer, "END" ) )
                        break;

                    /* increment the file_data_ptr pointer to point to the
                       next token */
                    file_data_ptr += num_chars;

                    /* look for the PARAMETERVALUE token */
                    if ( !strcmp ( token_buffer, "PARAMETERVALUE" ) )
                    {
                        /* read the next three tokens, this should be the
                           NUM_VAL = ... line */
                        sscanf (file_data_ptr, "%s%n", token_buffer,
                            &num_chars );
                        file_data_ptr += num_chars;

                        sscanf (file_data_ptr, "%s%n", token_buffer,
                            &num_chars );
                        file_data_ptr += num_chars;

                        sscanf (file_data_ptr, "%s%n", token_buffer,
                            &num_chars );
                        file_data_ptr += num_chars;

                        /* read the next three tokens, this should be the
                           CLASS = ... line */
                        sscanf (file_data_ptr, "%s%n", token_buffer,
                            &num_chars );
                        file_data_ptr += num_chars;

                        sscanf (file_data_ptr, "%s%n", token_buffer,
                            &num_chars );
                        file_data_ptr += num_chars;

                        sscanf (file_data_ptr, "%s%n", token_buffer,
                            &num_chars );
                        file_data_ptr += num_chars;

                        /* verify that the class number matches the class
                           number read earlier */
                        if ( strcmp( token_buffer, class ) )
                            continue;

                        /* read the next three tokens, this should be the
                           VALUE = ... line */
                        sscanf (file_data_ptr, "%s%n", token_buffer,
                            &num_chars );
                        file_data_ptr += num_chars;

                        sscanf (file_data_ptr, "%s%n", token_buffer,
                            &num_chars );
                        file_data_ptr += num_chars;

                        sscanf (file_data_ptr, "%s%n", token_buffer,
                            &num_chars );
                        file_data_ptr += num_chars;

                        /* remove the beginning and ending "s (i.e. "23") */
                        /* NOTE: num_chars includes a char for endline */
                        strncpy( tmp_str, &token_buffer[1], num_chars-2 );
                        tmp_str[num_chars-3] = '\0';

                        if ( !strcmp (value, "\"HORIZONTALTILENUMBER\"" ) )
                        {
                            /* set the value of the horizontal tile number */
                            *horiz = atoi( tmp_str );
                            found_horiz = TRUE;
                            status = MRT_NO_ERROR;
                            break;
                        }
                        else if ( !strcmp (value, "\"VERTICALTILENUMBER\"" ) )
                        {
                            /* set the value of the vertical tile number */
                            *vert = atoi( tmp_str );
                            found_vert = TRUE;
                            status = MRT_NO_ERROR;
                            break;
                        }
                    }
                }

            }  /* end tile read */

            if ( found_horiz && found_vert )
                break;
        }
    }

    /* did we read all the tile numbers? */
    if ( !found_horiz || !found_vert )
    {
        *horiz = -9;
        *vert = -9;
        sprintf( error_str, "Horizontal and vertical tile numbers were not"
            "available in the embedded metadata of %s\n", filename );
        ErrorHandler( TRUE, "read_tile_number_hdf", ERROR_GENERAL,
            error_str );

    }

    /* verify that they are within the bounds of the 10-degree tiles */
    else
    {
        if ( *horiz > MAX_HORIZ_TILES )
        {
            sprintf( error_str, "Horizontal tile number (%d) in the embedded "
                "metadata of %s is larger than the maximum number of 10-degree "
                "ISIN tiles (%d).\n", *horiz, filename, MAX_HORIZ_TILES );
            ErrorHandler( TRUE, "read_tile_number_hdf", ERROR_GENERAL,
                error_str );
        }

        if ( *vert > MAX_VERT_TILES )
        {
            sprintf( error_str, "Vertical tile number (%d) in the embedded "
                "metadata of %s is larger than the maximum number of 10-degree "
                "ISIN tiles (%d).\n", *vert, filename, MAX_VERT_TILES );
            ErrorHandler( TRUE, "read_tile_number_hdf", ERROR_GENERAL,
                error_str );
        }
    }

    /* close the file */
    SDend( sd_id );

    /* free dynamically allocated memory */
    free ( file_data );

    return ( status );
}
