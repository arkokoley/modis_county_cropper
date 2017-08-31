/************************************************************************

FILE: append_meta.c

PURPOSE:  Read the bounding coordinates and tile values from one HDF-EOS
          file and append to the second HDF-EOS file.

HISTORY:
Version    Date     Programmer      Code     Reason
-------    ----     ----------      ----     ------
           04/04    Gail Schmidt             Original development

HARDWARE AND/OR SOFTWARE LIMITATIONS:
    None

NOTES:   This program is an adaptation of metadmp.c, developed by Doug
Ilg (Doug.Ilg@gsfc.nasa.gov).

*************************************************************************/

/*************************************************************************

MODULE: append_meta

PURPOSE: 
    This module appends the bounding coordinates and tile values from the
    input HDF-EOS file to the output HDF-EOS file.

RETURN VALUE:
Type = int
Value    	Description
-----		-----------
  0		Returns MRT_NO_ERROR if the metadata appending operation was
                successful.  Error codes found in mrt_error.c and mrt_error.h.
-13             Returns ERROR_GENERAL for general usage error.
-14		Returns ERROR_OPEN_IMAGE_READ if the HDF file can't
   		be opened for reading.
-15		Returns ERROR_READ_IMAGE if the HDF file can't
   		be read from.
-16		Returns ERROR_OPEN_IMAGE_WRITE if the HDF file can't
   		be opened for writing.
-17		Returns ERROR_WRITE_IMAGE if the HDF file can't
   		be written to.

HISTORY:
Version    Date     Programmer      Code     Reason
-------    ----     ----------      ----     ------
           04/04    Gail Schmidt             Original development using
                                             Doug Ilg's metadmp.c program.

NOTES:

**************************************************************************/

#include "append_meta.h"

int main (int argc, char *argv[])
{
    char in_hdfname[256];        /* input HDF filename of file to copy */
    char out_hdfname[256];       /* input HDF filename of file to append */
    int32 sd_id;                 /* holds the id# for the HDF file */ 
    int all_coords_present;      /* were all bounding coordinates present? */
    char nboundcoord[256];       /* north bounding coord */
    char sboundcoord[256];       /* south bounding coord */
    char eboundcoord[256];       /* east bounding coord */
    char wboundcoord[256];       /* west bounding coord */
    char htile[256];             /* horizontal tile value */
    char vtile[256];             /* vertical tile value */

    /* Check usage */
    if (argc != 3)
    {
        Usage ();
        return (ERROR_GENERAL);
    }

    /* Get the first argument, which is the input HDF filename for copying */
    strcpy (in_hdfname, argv[1]);

    /* Get the second argument, which is the HDF filename for appending */
    strcpy (out_hdfname, argv[2]);

    fprintf (stdout, "Appending meta to %s from %s ...\n", out_hdfname,
        in_hdfname);

    /* Open the HDF file for reading */
    sd_id = SDstart (in_hdfname, DFACC_READ);
    if (sd_id == -1)
    {
        fprintf (stdout, "Error: Unable to open file %s for reading.\n",
            in_hdfname);
        fflush (stdout);
        return (ERROR_OPEN_IMAGE_READ);
    }

    /* Read the current metadata parameters needed to append the additional
       HDF metadata */
    all_coords_present = FALSE;
    strcpy (htile, "99");
    strcpy (vtile, "99");
    if (ReadMeta (sd_id, nboundcoord, sboundcoord, eboundcoord,
        wboundcoord, &all_coords_present, htile, vtile) != MRT_NO_ERROR ||
        all_coords_present == FALSE)
    {
        fprintf (stdout, "Error: Unable to read necessary metadata from "
            "file %s.\n", in_hdfname);
        fflush (stdout);
        return (ERROR_READ_IMAGE);
    }

    /* Close the HDF file */
    SDend (sd_id);

    /* Open the HDF file for writing */
    sd_id = SDstart (out_hdfname, DFACC_WRITE);
    if (sd_id == -1)
    {
        fprintf (stdout, "Error: Unable to open file %s for writing.\n",
            out_hdfname);
        fflush (stdout);
        return (ERROR_OPEN_IMAGE_WRITE);
    }

    /* Append the necessary HDF metadata */
    if (AppendMeta (sd_id, nboundcoord, sboundcoord, eboundcoord, wboundcoord,
        htile, vtile) != MRT_NO_ERROR)
    {
        fprintf (stdout, "Error: Unable to append HDF metadata to "
            "file %s.\n", out_hdfname);
        fflush (stdout);
        return (ERROR_WRITE_IMAGE);
    }

    /* Close the HDF file */
    SDend (sd_id);

    /* Return status */
    fprintf (stdout, "Appending metadata complete!\n");
    return (MRT_NO_ERROR);
}


/*************************************************************************

MODULE: AppendMeta

PURPOSE: 
    This module appends the necessary bounding coordinates metadata
    information and writes it to the HDF file as the ArchiveMetadata.0
    attribute. The tile coordinates are written as the CoreMetadata.0
    attribute.

RETURN VALUE:
Type = int
Value    	Description
-----		-----------
MRT_NO_ERROR	Returns MRT_NO_ERROR if the metadata appending operation was
                successful.  Error codes found in mrt_error.c and mrt_error.h.
-16		Returns ERROR_OPEN_IMAGE_WRITE if the HDF file can't
   		be opened for writing.
-17		Returns ERROR_WRITE_IMAGE if the HDF file can't
   		be written to.

HISTORY:
Version    Date     Programmer      Code     Reason
-------    ----     ----------      ----     ------
           04/04    Gail Schmidt             Original Development

NOTES:

**************************************************************************/
intn AppendMeta
(
    int32 sd_id,                /* I: SDS id # for the HDF file */ 
    char *nboundcoord,          /* I: North bounding coord */
    char *sboundcoord,          /* I: South bounding coord */
    char *eboundcoord,          /* I: East bounding coord */
    char *wboundcoord,          /* I: West bounding coord */
    char *htile,                /* I: Horizontal tile value */
    char *vtile                 /* I: Vertical tile value */
)

{
    char file_data[10000],         /* char ptr used to store the metadata */
         attr_name[MAX_NC_NAME];   /* holds attribute's name */

    /** The attribute name will be ArchiveMetadata.0 **/
    strcpy (attr_name, "ArchiveMetadata.0");

    /* Create the necessary metadata information */
    sprintf (file_data,
        "GROUP                  = ARCHIVEDMETADATA\n"
        "  GROUPTYPE            = MASTERGROUP\n\n"
        "  GROUP                  = BOUNDINGRECTANGLE\n\n"
        "    OBJECT                 = NORTHBOUNDINGCOORDINATE\n"
        "      NUM_VAL              = 1\n"
        "      VALUE                = %s\n"
        "    END_OBJECT             = NORTHBOUNDINGCOORDINATE\n\n"
        "    OBJECT                 = SOUTHBOUNDINGCOORDINATE\n"
        "      NUM_VAL              = 1\n"
        "      VALUE                = %s\n"
        "    END_OBJECT             = SOUTHBOUNDINGCOORDINATE\n\n"
        "    OBJECT                 = EASTBOUNDINGCOORDINATE\n"
        "      NUM_VAL              = 1\n"
        "      VALUE                = %s\n"
        "    END_OBJECT             = EASTBOUNDINGCOORDINATE\n\n"
        "    OBJECT                 = WESTBOUNDINGCOORDINATE\n"
        "      NUM_VAL              = 1\n"
        "      VALUE                = %s\n"
        "    END_OBJECT             = WESTBOUNDINGCOORDINATE\n\n"
        "  END_GROUP              = BOUNDINGRECTANGLE\n\n"
        "END_GROUP              = ARCHIVEDMETADATA\n\n"
        "END\n",
        nboundcoord, sboundcoord, eboundcoord, wboundcoord);

    /* Attempt to write file metadata to HDF file */
    if (SDsetattr (sd_id, attr_name, DFNT_CHAR8, strlen (file_data),
       (void *) file_data) == FAIL)
    {
        fprintf (stdout, "Error writing the bounding coordinates to the "
            "ArchiveMetadata.0 in the metadata\n");
        return (ERROR_WRITE_IMAGE);
    }

    /** The attribute name will be CoreMetadata.0 **/
    strcpy (attr_name, "CoreMetadata.0");

    /* Create the necessary tile metadata information */
    if (!strcmp (htile, "99") && !strcmp (vtile, "99"))
    {  /* htile and vtile are valid */
        sprintf (file_data,
            "GROUP                  = INVENTORYMETADATA\n"
            "  GROUPTYPE            = MASTERGROUP\n\n"
            "  GROUP                  = ADDITIONALATTRIBUTES\n\n"
            "    OBJECT                 = ADDITIONALATTRIBUTESCONTAINER\n"
            "      CLASS                = \"7\"\n\n"
            "      OBJECT                 = ADDITIONALATTRIBUTENAME\n"
            "        CLASS                = \"7\"\n"
            "        NUM_VAL              = 1\n"
            "        VALUE                = \"HORIZONTALTILENUMBER\"\n"
            "      END_OBJECT             = ADDITIONALATTRIBUTENAME\n\n"
            "      GROUP                  = INFORMATIONCONTENT\n"
            "        CLASS                = \"7\"\n\n"
            "        OBJECT                 = PARAMETERVALUE\n"
            "          NUM_VAL              = 1\n"
            "          CLASS                = \"7\"\n"
            "          VALUE                = \"%s\"\n"
            "        END_OBJECT             = PARAMETERVALUE\n\n"
            "      END_GROUP              = INFORMATIONCONTENT\n\n"
            "    END_OBJECT             = ADDITIONALATTRIBUTESCONTAINER\n\n"
            "    OBJECT                 = ADDITIONALATTRIBUTESCONTAINER\n"
            "      CLASS                = \"8\"\n\n"
            "      OBJECT                 = ADDITIONALATTRIBUTENAME\n"
            "        CLASS                = \"8\"\n"
            "        NUM_VAL              = 1\n"
            "        VALUE                = \"VERTICALTILENUMBER\"\n"
            "      END_OBJECT             = ADDITIONALATTRIBUTENAME\n\n"
            "      GROUP                  = INFORMATIONCONTENT\n"
            "        CLASS                = \"8\"\n\n"
            "        OBJECT                 = PARAMETERVALUE\n"
            "          NUM_VAL              = 1\n"
            "          CLASS                = \"8\"\n"
            "          VALUE                = \"%s\"\n"
            "        END_OBJECT             = PARAMETERVALUE\n\n"
            "      END_GROUP              = INFORMATIONCONTENT\n\n"
            "    END_OBJECT             = ADDITIONALATTRIBUTESCONTAINER\n\n"
            "  END_GROUP              = ADDITIONALATTRIBUTES\n\n"
            "END_GROUP              = INVENTORYMETADATA\n\n"
            "END\n",
            htile, vtile);
    }

    /* Attempt to write file metadata to HDF file */
    if (SDsetattr (sd_id, attr_name, DFNT_CHAR8, strlen (file_data),
       (void *) file_data) == FAIL)
    {
        fprintf (stdout, "Error writing the tile values to the "
            "CoreMetadata.0 in the metadata\n");
        return (ERROR_WRITE_IMAGE);
    }

    /* Return status */
    return (MRT_NO_ERROR);
}


/*************************************************************************

MODULE: ReadMeta

PURPOSE:
    This module sends requests to read_metadata to read the archive, core,
    and structural metadata from the HDF file.  The metadata is read and
    the bounding rectangular coordinates and tile values are searched for.
    Once these coords are found, then the routine returns.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
MRT_NO_ERROR        Returns MRT_NO_ERROR if the metadata appending operation was
                successful.  Error codes found in mrt_error.c and mrt_error.h.
-4              Returns ERROR_MEMORY if mem allocation fails.
-15             Returns ERROR_READ_IMAGE if the HDF file can't
                be read from.


HISTORY:
Version    Date     Programmer      Code     Reason
-------    ----     ----------      ----     ------
           04/04    Gail Schmidt             Original Development

NOTES:

**************************************************************************/

intn ReadMeta
(
    int32 sd_id,                /* I: SDS id # for the HDF file */
    char *nboundcoord,          /* O: North bounding coord */
    char *sboundcoord,          /* O: South bounding coord */
    char *eboundcoord,          /* O: East bounding coord */
    char *wboundcoord,          /* O: West bounding coord */
    int *all_coords_present,    /* O: Were all bounding coordinates present? */
    char *htile,                /* O: Horizontal tile value */
    char *vtile                 /* O: Vertical tile value */
)

{
    intn j;                  /* looping variable */
    intn status;             /* return status */
    char attrname[256];      /* holds the file_name string */

    /* Read the bounding coordinates, trying Archive, Core, and Struct
       metadata locations */
    /* First try ArchiveMetadata alone */
    status = ReadBoundCoords (sd_id, "ArchiveMetadata", nboundcoord,
        sboundcoord, eboundcoord, wboundcoord, all_coords_present);
    if (status != MRT_NO_ERROR)
    {
        /* Try concatenating sequence numbers */
        for (j = 0; j <= 9; j++)
        {
            sprintf (attrname, "%s.%d", "ArchiveMetadata", j);
            status = ReadBoundCoords (sd_id, attrname, nboundcoord,
                sboundcoord, eboundcoord, wboundcoord, all_coords_present);
            if (status == MRT_NO_ERROR)
                break;
        }
    }

    if (status != MRT_NO_ERROR)
    {
        /* Now try CoreMetadata alone */
        status = ReadBoundCoords (sd_id, "CoreMetadata", nboundcoord,
            sboundcoord, eboundcoord, wboundcoord, all_coords_present);

        if (status != MRT_NO_ERROR)
        {
            /* Try concatenating sequence numbers */
            for (j = 0; j <= 9; j++)
            {
                sprintf (attrname, "%s.%d", "CoreMetadata", j);
                status = ReadBoundCoords (sd_id, attrname, nboundcoord,
                    sboundcoord, eboundcoord, wboundcoord, all_coords_present);
                if (status == MRT_NO_ERROR)
                    break;
            }
        }
    }

    if (status != MRT_NO_ERROR)
    {
        /* Last, try StructMetadata alone */
        status = ReadBoundCoords (sd_id, "StructMetadata", nboundcoord,
            sboundcoord, eboundcoord, wboundcoord, all_coords_present);

        if (status != MRT_NO_ERROR)
        {
            /* Try concatenating sequence numbers */
            for (j = 0; j <= 9; j++)
            {
                sprintf (attrname, "%s.%d", "StructMetadata", j);
                status = ReadBoundCoords (sd_id, attrname, nboundcoord,
                    sboundcoord, eboundcoord, wboundcoord, all_coords_present);
                if (status == MRT_NO_ERROR)
                    break;
            }
        }
    }

    /* Read the tile values from the Core metadata locations */
    if (status == MRT_NO_ERROR)
    {
        status = ReadTile (sd_id, "CoreMetadata", htile, vtile);
        if (status != MRT_NO_ERROR)
        {
            /* Try concatenating sequence numbers */
            for (j = 0; j <= 9; j++)
            {
                sprintf (attrname, "%s.%d", "CoreMetadata", j);
                status = ReadTile (sd_id, attrname, htile, vtile);
                if (status == MRT_NO_ERROR)
                    break;
            }
        }

        /* If the tile values are not available, its ok */
        status = MRT_NO_ERROR;
    }

    /* return the value of the function to main function */
    return (status);
}

/*************************************************************************

MODULE: ReadBoundCoords

PURPOSE: 
    This module reads the bounding coordinates metadata information from
    the HDF file.

RETURN VALUE:
Type = int
Value    	Description
-----		-----------
MRT_NO_ERROR	Returns MRT_NO_ERROR if the metadata appending operation was
                successful.  Error codes found in mrt_error.c and mrt_error.h.
-4              Returns ERROR_MEMORY if mem allocation fails.
-15		Returns ERROR_READ_IMAGE if the HDF file can't
   		be read from.


HISTORY:
Version    Date     Programmer      Code     Reason
-------    ----     ----------      ----     ------
           04/04    Gail Schmidt             Original Development

NOTES:

**************************************************************************/
intn ReadBoundCoords
(
    int32 sd_id,                /* I: SDS id # for the HDF file */ 
    char *attr,                 /* I: Name of the attribute to read */
    char *nboundcoord,          /* O: North bounding coord */
    char *sboundcoord,          /* O: South bounding coord */
    char *eboundcoord,          /* O: East bounding coord */
    char *wboundcoord,          /* O: West bounding coord */
    int *all_coords_present     /* O: Were all bounding coordinates present? */
)

{
    intn status;                 /* this is the var that holds return val */
    int32 my_attr_index;         /* holds return val from SDfindattr */ 
    int32 data_type;             /* holds attribute's data type */
    int32 n_values;              /* stores # of vals of the attribute */
    int num_chars;               /* number of characters read in the line */
    char *file_data = NULL;      /* char ptr used to allocate temp space during
                                    transfer of attribute info */
    char *file_data_ptr = NULL;
                                 /* pointer to file_data for scanning */
    char attr_name[MAX_NC_NAME]; /* holds attribute's name */
    char token_buffer[256]; /* holds the current token */
    int found_north_bound;  /* north bounding rectangle coord been found? */
    int found_south_bound;  /* south bounding rectangle coord been found? */
    int found_east_bound;   /* east bounding rectangle coord been found? */
    int found_west_bound;   /* west bounding rectangle coord been found? */

    /* look for attribute in the HDF file */
    my_attr_index = SDfindattr (sd_id, attr);

    /* only proceed if the attribute was found */
    if (my_attr_index == -1)
    {
        return (ERROR_READ_IMAGE);
    }

    /* get size of HDF file attribute */
    status = SDattrinfo (sd_id, my_attr_index, attr_name, &data_type,
        &n_values);
    if (status == -1)
    {
        return (ERROR_READ_IMAGE);
    }

    /* attempt to allocate memory for HDF file attribute contents (add one
       character for the end of string character) */
    file_data = (char *) calloc (n_values+1, sizeof (char));
    if (file_data == NULL)
    {
        fprintf (stdout, "Error: Unable to allocate %d bytes for %s\n",
            (int) n_values, attr);
        fflush (stdout);
        return (ERROR_MEMORY);
    }

    /* read attribute from the HDF file */
    status = SDreadattr (sd_id, my_attr_index, file_data);
    if (status == -1 || !strcmp (file_data, ""))
    {
        /* first free the allocated memory */
        free (file_data);

        return (ERROR_READ_IMAGE);
    }

    /* walk through the file_data string one token at a time looking for the
       bounding rectangular coords */
    found_north_bound = FALSE;
    found_south_bound = FALSE;
    found_east_bound = FALSE;
    found_west_bound = FALSE;
    file_data_ptr = file_data;
    status = ERROR_READ_IMAGE;
    while (sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars) != EOF)
    {
        /* if this token is END, then we are done with the metadata */
        if (!strcmp (token_buffer, "END"))
            break;

        /* if all the bounding coords have been found, don't waste time
           with the rest of the metadata */
        if (found_north_bound && found_south_bound && found_east_bound &&
            found_west_bound)
        {
            break;
        }

        /* increment the file_data_ptr pointer to point to the next token */
        file_data_ptr += num_chars;

        /* look for the NORTHBOUNDINGCOORDINATE token */
        if (!found_north_bound &&
            !strcmp (token_buffer, "NORTHBOUNDINGCOORDINATE"))
        {
            /* read the next three tokens, this should be the
               NUM_VAL = ... line */
            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            /* read the next three tokens, this should be the
               VALUE = ... line */
            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            /* find the value of the bounding coordinate */
            strcpy (nboundcoord, token_buffer);
            found_north_bound = TRUE;
            status = MRT_NO_ERROR;
            continue;  /* don't waste time looking for the other
                          bounding rectangles with this token */
        }

        /* look for the SOUTHBOUNDINGCOORDINATE token */
        if (!found_south_bound &&
            !strcmp (token_buffer, "SOUTHBOUNDINGCOORDINATE"))
        {
            /* read the next three tokens, this should be the
               NUM_VAL = ... line */
            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            /* read the next three tokens, this should be the
               VALUE = ... line */
            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            /* find the value of the bounding coordinate */
            strcpy (sboundcoord, token_buffer);
            found_south_bound = TRUE;
            status = MRT_NO_ERROR;
            continue;  /* don't waste time looking for the other
                          bounding rectangles with this token */
        }

        /* look for the EASTBOUNDINGCOORDINATE token */
        if (!found_east_bound &&
            !strcmp (token_buffer, "EASTBOUNDINGCOORDINATE"))
        {
            /* read the next three tokens, this should be the
               NUM_VAL = ... line */
            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;
            /* read the next three tokens, this should be the
               VALUE = ... line */
            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            /* find the value of the bounding coordinate */
            strcpy (eboundcoord, token_buffer);
            found_east_bound = TRUE;
            status = MRT_NO_ERROR;
            continue;  /* don't waste time looking for the other
                          bounding rectangles with this token */
        }

        /* look for the WESTBOUNDINGCOORDINATE token */
        if (!found_west_bound &&
            !strcmp (token_buffer, "WESTBOUNDINGCOORDINATE"))
        {
            /* read the next three tokens, this should be the
               NUM_VAL = ... line */
            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            /* read the next three tokens, this should be the
               VALUE = ... line */
            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            /* find the value of the bounding coordinate */
            strcpy (wboundcoord, token_buffer);
            found_west_bound = TRUE;
            status = MRT_NO_ERROR;
            continue;  /* don't waste time looking for the other
                          bounding rectangles with this token */
        }
    }

    /* did we read all the bounding coordinates? */
    if (found_north_bound && found_south_bound && found_east_bound &&
        found_west_bound)
    {
        *all_coords_present = TRUE;
    }
    else
    {
        *all_coords_present = FALSE;
    }

    /* free dynamically allocated memory */
    free (file_data);

    /* if any of the bounding rectangular coords were found then MRT_NO_ERROR
       is returned, OW ERROR_READ_IMAGE is returned */
    return (status);
}


/*************************************************************************

MODULE: ReadTile

PURPOSE:
    This module reads the tile coordinates metadata information from
    the HDF file.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
MRT_NO_ERROR        Returns MRT_NO_ERROR if the metadata appending operation was
                successful.  Error codes found in mrt_error.c and mrt_error.h.
-4              Returns ERROR_MEMORY if mem allocation fails.
-15             Returns ERROR_READ_IMAGE if the HDF file can't
                be read from.


HISTORY:
Version    Date     Programmer      Code     Reason
-------    ----     ----------      ----     ------
           04/04    Gail Schmidt             Original Development

NOTES:

**************************************************************************/
intn ReadTile
(
    int32 sd_id,                /* I: SDS id # for the HDF file */
    char *attr,                 /* I: Name of the attribute to read */
    char *htile,                /* O: Horizontal tile value */
    char *vtile                 /* O: Vertical tile value */
)

{
    intn status;                 /* this is the var that holds return val */
    int32 my_attr_index;         /* holds return val from SDfindattr */
    int32 data_type;             /* holds attribute's data type */
    int32 n_values;              /* stores # of vals of the attribute */
    int num_chars;               /* number of characters read in the line */
    char error_str[256];         /* string for error messages */
    char *file_data = NULL;      /* char ptr used to allocate temp space during
                                    transfer of attribute info */
    char *file_data_ptr = NULL;
                                 /* pointer to file_data for scanning */
    char attr_name[MAX_NC_NAME]; /* holds attribute's name */
    char token_buffer[256];      /* holds the current token */
    int found_horiz;             /* horizontal tile coord been found? */
    int found_vert;              /* vertical tile coord been found? */
    char tmp_str[256];           /* temporary holder of the VALUE string */
    char class[256];             /* class string */
    char value[256];             /* value string */

    /* look for attribute in the HDF file */
    my_attr_index = SDfindattr (sd_id, attr);

    /* only proceed if the attribute was found */
    if (my_attr_index == -1)
    {
        return (ERROR_READ_IMAGE);
    }

    /* get size of HDF file attribute */
    status = SDattrinfo (sd_id, my_attr_index, attr_name, &data_type,
        &n_values);
    if (status == -1)
    {
        return (ERROR_READ_IMAGE);
    }

    /* attempt to allocate memory for HDF file attribute contents (add one
       character for the end of string character) */
    file_data = (char *) calloc (n_values+1, sizeof (char));
    if (file_data == NULL)
    {
        fprintf (stdout, "Error: Unable to allocate %d bytes for %s\n",
            (int) n_values, attr);
        fflush (stdout);
        return (ERROR_MEMORY);
    }

    /* read attribute from the HDF file */
    status = SDreadattr (sd_id, my_attr_index, file_data);
    if (status == -1 || !strcmp (file_data, ""))
    {
        /* first free the allocated memory */
        free (file_data);

        return (ERROR_READ_IMAGE);
    }

    /* walk through the file_data string one token at a time looking for the
       tile coords */
    found_horiz = FALSE;
    found_vert = FALSE;
    file_data_ptr = file_data;
    status = ERROR_READ_IMAGE;
    while (sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars) != EOF)
    {
        /* if this token is END, then we are done with the metadata */
        if (!strcmp (token_buffer, "END"))
            break;

        /* if the horizontal and vertical tiles have been found, don't waste
           time with the rest of the metadata */
        if (found_horiz && found_vert)
        {
            break;
        }

        /* increment the file_data_ptr pointer to point to the next token */
        file_data_ptr += num_chars;

        /* look for the ADDITIONALATTRIBUTENAME token */
        if (!strcmp (token_buffer, "ADDITIONALATTRIBUTENAME"))
        {
            /* read the next three tokens, this should be the
               CLASS = ... line */
            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            /* store the class name */
            strcpy (class, token_buffer);

            /* read the next three tokens, this should be the
               NUM_VAL = ... line */
            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            /* read the next three tokens, this should be the
               VALUE = ... line */
            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            sscanf (file_data_ptr, "%s%n", token_buffer, &num_chars);
            file_data_ptr += num_chars;

            /* store the value name */
            strcpy (value, token_buffer );

            /* if this is HORIZONTALTILENUMBER or VERTICALTILENUMBER, then
               look for the PARAMETERVALUE token */
            if (!strcmp (token_buffer, "\"HORIZONTALTILENUMBER\"") ||
                !strcmp (token_buffer, "\"VERTICALTILENUMBER\""))
            {
                while (sscanf (file_data_ptr, "%s%n", token_buffer,
                       &num_chars) != EOF)
                {
                    /* if this token is END, then we are done with the
                       metadata */
                    if (!strcmp (token_buffer, "END"))
                        break;

                    /* increment the file_data_ptr pointer to point to the
                       next token */
                    file_data_ptr += num_chars;

                    /* look for the PARAMETERVALUE token */
                    if (!strcmp (token_buffer, "PARAMETERVALUE"))
                    {
                        /* read the next three tokens, this should be the
                           NUM_VAL = ... line */
                        sscanf (file_data_ptr, "%s%n", token_buffer,
                            &num_chars);
                        file_data_ptr += num_chars;

                        sscanf (file_data_ptr, "%s%n", token_buffer,
                            &num_chars);
                        file_data_ptr += num_chars;

                        sscanf (file_data_ptr, "%s%n", token_buffer,
                            &num_chars);
                        file_data_ptr += num_chars;

                        /* read the next three tokens, this should be the
                           CLASS = ... line */
                        sscanf (file_data_ptr, "%s%n", token_buffer,
                            &num_chars);
                        file_data_ptr += num_chars;

                        sscanf (file_data_ptr, "%s%n", token_buffer,
                            &num_chars);
                        file_data_ptr += num_chars;

                        sscanf (file_data_ptr, "%s%n", token_buffer,
                            &num_chars);
                        file_data_ptr += num_chars;

                        /* verify that the class number matches the class
                           number read earlier */
                        if (strcmp (token_buffer, class))
                            continue;

                        /* read the next three tokens, this should be the
                           VALUE = ... line */
                        sscanf (file_data_ptr, "%s%n", token_buffer,
                            &num_chars);
                        file_data_ptr += num_chars;

                        sscanf (file_data_ptr, "%s%n", token_buffer,
                            &num_chars);
                        file_data_ptr += num_chars;

                        sscanf (file_data_ptr, "%s%n", token_buffer,
                            &num_chars);
                        file_data_ptr += num_chars;

                        /* remove the beginning and ending "s (i.e. "23") */
                        /* NOTE: num_chars includes a char for endline */
                        strncpy (tmp_str, &token_buffer[1], num_chars-2);
                        tmp_str[num_chars-3] = '\0';

                        if (!strcmp (value, "\"HORIZONTALTILENUMBER\""))
                        {
                            /* set the value of the horizontal tile number */
                            strcpy (htile, tmp_str);
                            found_horiz = TRUE;
                            status = MRT_NO_ERROR;
                            break;
                        }
                        else if (!strcmp (value, "\"VERTICALTILENUMBER\""))
                        {
                            /* set the value of the vertical tile number */
                            strcpy (vtile, tmp_str);
                            found_vert = TRUE;
                            status = MRT_NO_ERROR;
                            break;
                        }
                    }
                }

            }  /* end tile read */

            if (found_horiz && found_vert)
                break;
        }
    }

    /* verify that they are within the bounds of the 10-degree tiles */
    if (found_horiz && found_vert)
    {
        if (atoi (htile) > MAX_HORIZ_TILES)
        {
            status = ERROR_READ_IMAGE;
            sprintf (error_str, "Horizontal tile number (%s) in the embedded "
                "metadata is larger than the maximum number of 10-degree "
                "ISIN tiles (%d).\n", htile, MAX_HORIZ_TILES);
            fprintf (stdout, "%s", error_str);
        }

        if (atoi (vtile) > MAX_VERT_TILES)
        {
            status = ERROR_READ_IMAGE;
            sprintf (error_str, "Vertical tile number (%s) in the embedded "
                "metadata is larger than the maximum number of 10-degree "
                "ISIN tiles (%d).\n", vtile, MAX_VERT_TILES);
            fprintf (stdout, "%s", error_str);
        }
    }

    /* free dynamically allocated memory */
    free (file_data);

    return (status);
}


/*************************************************************************

MODULE: Usage

PURPOSE: Prints the usage information for this executable.

RETURN VALUE:
Type = none

HISTORY:
Version    Date     Programmer      Code     Reason
-------    ----     ----------      ----     ------
           07/03    Gail Schmidt             Original Development

NOTES:

**************************************************************************/
void Usage ()

{
    fprintf (stdout, "Usage: append_meta hdf_srcname hdf_destname\n");
    fprintf (stdout, "Where hdf_srcname is the name of the source file "
        "containing the bounding coordinate metadata.\n"
        "And, hdf_destname is the name of the HDF file to which to copy "
        "the bounding coordinate metadata.\n\n");
}
