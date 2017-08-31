/************************************************************************

FILE: update_tile_meta.c

PURPOSE:  Take the user-specified tile values from the command line
          and update CoreMetadata for the HDF file containing the specified
          horizontal and vertical tile values.

HISTORY:
Version    Date     Programmer      Code     Reason
-------    ----     ----------      ----     ------
           03/05    Gail Schmidt             Original development

HARDWARE AND/OR SOFTWARE LIMITATIONS:
    None

*************************************************************************/

/*************************************************************************

MODULE: update_tile_meta

PURPOSE: 
    This module appends the tile values from the user input values to the
    HDF-EOS file.

RETURN VALUE:
Type = int
Value    	Description
-----		-----------
  0		Returns MRT_NO_ERROR if the metadata appending operation was
                successful.  Error codes found in mrt_error.c and mrt_error.h.
-13             Returns ERROR_GENERAL for general usage error.
-16		Returns ERROR_OPEN_IMAGE_WRITE if the HDF file can't
   		be opened for writing.
-17		Returns ERROR_WRITE_IMAGE if the HDF file can't
   		be written to.

HISTORY:
Version    Date     Programmer      Code     Reason
-------    ----     ----------      ----     ------
           03/05    Gail Schmidt             Original development

NOTES:

**************************************************************************/

#if defined(__CYGWIN__) || defined(WIN32)
#include <getopt.h>		/* getopt  prototype */
#else
#include <unistd.h>		/* getopt  prototype */
#endif
#include "update_tile_meta.h"

int main (int argc, char *argv[])
{
    char *hdfname = NULL;    /* HDF filename of file to modify */
    int32 sd_id;             /* holds the id# for the HDF file */ 
    int c;                   /* return value from getopt */
    int intval;              /* integer value for tile numbers */
    char *htile = NULL;      /* horizontal tile value (00-35)*/
    char *vtile = NULL;      /* vertical tile value (00-17) */
    char *core_meta = NULL;  /* string containing the core metadata */

    /* Check usage */
    if (argc != 7)
    {
        Usage ();
        return (ERROR_GENERAL);
    }

    opterr = 0;		/* do not print error messages to stdout */
    while ( ( c = getopt( argc, argv, "f:h:v:" ) ) != -1 )
    {
        switch ( c )
        {
            case 'f':    /* HDF filename */
                hdfname = strdup( optarg );
                break;

            case 'h':    /* horizontal tile number */
                htile = strdup( optarg );
                intval = atoi (htile);
                if (intval < 0 || intval > MAX_HORIZ_TILES)
                {
                    fprintf( stdout, "Horizontal tile number must be between "
                        "00 and %d.  Input of %d is invalid.\n",
                        MAX_HORIZ_TILES, intval);
                    Usage( );
                    return (ERROR_GENERAL);
                }
                break;

            case 'v':    /* vertical tile number */
                vtile = strdup( optarg );
                intval = atoi (vtile);
                if (intval < 0 || intval > MAX_VERT_TILES)
                {
                    fprintf( stdout, "Vertical tile number must be between "
                        "00 and %d.  Input of %d is invalid.\n",
                        MAX_VERT_TILES, intval);
                    Usage( );
                    return (ERROR_GENERAL);
                }
                break;

            case '?':		/* error */
                fprintf( stdout, "Unknown option (%s)", argv[optind - 1] );
                Usage( );
                return (ERROR_GENERAL);
        }
    }

    fprintf (stdout, "Modifying CoreMetadata for %s ...\n", hdfname);
    fprintf (stdout, "  htile is %s\n", htile);
    fprintf (stdout, "  vtile is %s\n", vtile);

    /* Open the HDF file for reading */
    sd_id = SDstart (hdfname, DFACC_READ);
    if (sd_id == -1)
    {
        fprintf (stdout, "Error: Unable to open file %s for reading.\n",
            hdfname);
        return (ERROR_OPEN_IMAGE_READ);
    }

    /* Read the CoreMetadata */
    core_meta = ReadMeta (sd_id);
    if (core_meta == NULL)
    {
        /* CoreMetadata doesn't exist, so create it */
        core_meta = CreateMeta (htile, vtile);
        if (core_meta == NULL)
        {
            fprintf (stdout, "Error: Unable to create CoreMetadata from %s.\n",
                hdfname);
            return (ERROR_GENERAL);
        }
    }
    else
    {
        /* Modify the horizontal and vertical tile values in the core metadata
           to use the user-specified htile and vtile values */
        if (UpdateTiles (core_meta, htile, vtile) != MRT_NO_ERROR)
        {
            fprintf (stdout, "Error: Unable to update the horizontal and "
                "vertical tile values in the CoreMetadata for %s.\n", hdfname);
            return (ERROR_GENERAL);
        }
    }

    /* Close the HDF file */
    SDend (sd_id);

    /* Open the HDF file for writing */
    sd_id = SDstart (hdfname, DFACC_WRITE);
    if (sd_id == -1)
    {
        fprintf (stdout, "Error: Unable to open file %s for writing.\n",
            hdfname);
        return (ERROR_OPEN_IMAGE_WRITE);
    }

    /* Append (or overwrite) the CoreMetadata HDF attribute */
    if (AppendMeta (sd_id, core_meta) != MRT_NO_ERROR)
    {
        fprintf (stdout, "Error: Unable to append HDF metadata to file %s.\n",
            hdfname);
        return (ERROR_WRITE_IMAGE);
    }

    /* Close the HDF file */
    SDend (sd_id);

    /* Clean up the pointers */
    free (hdfname);
    free (htile);
    free (vtile);
    free (core_meta);

    /* Return status */
    fprintf (stdout, "Appending metadata complete!\n");
    return (MRT_NO_ERROR);
}


/*************************************************************************

MODULE: ReadMeta

PURPOSE:
    This module sends requests to ReadCoreMetadata to read the core metadata
    from the HDF file.  The CoreMetadata string is returned.

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
           03/05    Gail Schmidt             Original Development

NOTES:

**************************************************************************/

char *ReadMeta
(
    int32 sd_id                /* I: SDS id # for the HDF file */
)

{
    intn j;                    /* looping variable */
    char attrname[256];        /* holds the file_name string */
    char *core_data = NULL;    /* core metadata string */

    /* Read the CoreMetadata */
    core_data = ReadCoreMetadata (sd_id, "CoreMetadata" );
    if (core_data == NULL)
    {
        /* Try concatenating sequence numbers */
        for (j = 0; j <= 9; j++)
        {
            sprintf (attrname, "%s.%d", "CoreMetadata", j);
            core_data = ReadCoreMetadata (sd_id, attrname);
            if (core_data != NULL)
                break;
        }
    }

    return (core_data);
}


/*************************************************************************

MODULE: ReadCoreMetadata

PURPOSE: 
    This module reads the core metadata information from the HDF file.

RETURN VALUE:
Type = int
Value    	Description
-----		-----------
!= NULL		Core metadata string is returned
NULL            Error occurred


HISTORY:
Version    Date     Programmer      Code     Reason
-------    ----     ----------      ----     ------
           03/05    Gail Schmidt             Original Development

NOTES:

**************************************************************************/
char *ReadCoreMetadata
(
    int32 sd_id,                /* I: SDS id # for the HDF file */ 
    char *attr                  /* I: Name of the attribute to read */
)
{
    intn status;                 /* this is the var that holds return val */
    int32 my_attr_index;         /* holds return val from SDfindattr */ 
    int32 data_type;             /* holds attribute's data type */
    int32 n_values;              /* stores # of vals of the attribute */
    char *core_data = NULL;      /* string containing the core metadata */
    char attr_name[MAX_NC_NAME]; /* holds attribute's name */

    /* look for attribute in the HDF file */
    my_attr_index = SDfindattr (sd_id, attr);

    /* only proceed if the attribute was found */
    if (my_attr_index == -1)
    {
        return (NULL);
    }

    /* get size of HDF file attribute */
    status = SDattrinfo (sd_id, my_attr_index, attr_name, &data_type,
        &n_values);
    if (status == -1)
    {
        return (NULL);
    }

    /* attempt to allocate memory for HDF file attribute contents (add one
       character for the end of string character) */
    core_data = (char *) calloc (n_values+1, sizeof (char));
    if (core_data == NULL)
    {
        fprintf (stdout, "Error: Unable to allocate %d bytes for %s\n",
            (int) n_values, attr);
        return (NULL);
    }

    /* read attribute from the HDF file */
    status = SDreadattr (sd_id, my_attr_index, core_data);
    if (status == -1 || !strcmp (core_data, ""))
    {
        /* first free the allocated memory */
        free (core_data);

        return (NULL);
    }

    /* Return the core metadata */
    return (core_data);
}


/*************************************************************************

MODULE: UpdateTiles

PURPOSE:
    This module updates the tile coordinate values in the CoreMetadata
    string.

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
           03/05    Gail Schmidt             Original Development

NOTES:

**************************************************************************/
intn UpdateTiles
(
    char *core_data,            /* I/O: CoreMetadata string */
    char *htile,                /* I: Horizontal tile value */
    char *vtile                 /* I: Vertical tile value */
)

{
    intn status;                 /* this is the var that holds return val */
    int num_chars;               /* number of characters read in the line */
    char *core_data_ptr = NULL;  /* pointer to core_data for scanning */
    char token_buffer[256];      /* holds the current token */
    int found_horiz;             /* horizontal tile coord been found? */
    int found_vert;              /* vertical tile coord been found? */
    char class[256];             /* class string */
    char value[256];             /* value string */

    /* walk through the core_data string one token at a time looking for the
       tile coords */
    found_horiz = FALSE;
    found_vert = FALSE;
    core_data_ptr = core_data;
    status = ERROR_READ_IMAGE;
    while (sscanf (core_data_ptr, "%s%n", token_buffer, &num_chars) != EOF)
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

        /* increment the core_data_ptr pointer to point to the next token */
        core_data_ptr += num_chars;

        /* look for the ADDITIONALATTRIBUTENAME token */
        if (!strcmp (token_buffer, "ADDITIONALATTRIBUTENAME"))
        {
            /* read the next three tokens, this should be the
               CLASS = ... line */
            sscanf (core_data_ptr, "%s%n", token_buffer, &num_chars);
            core_data_ptr += num_chars;

            sscanf (core_data_ptr, "%s%n", token_buffer, &num_chars);
            core_data_ptr += num_chars;

            sscanf (core_data_ptr, "%s%n", token_buffer, &num_chars);
            core_data_ptr += num_chars;

            /* store the class name */
            strcpy (class, token_buffer);

            /* read the next three tokens, this should be the
               NUM_VAL = ... line */
            sscanf (core_data_ptr, "%s%n", token_buffer, &num_chars);
            core_data_ptr += num_chars;

            sscanf (core_data_ptr, "%s%n", token_buffer, &num_chars);
            core_data_ptr += num_chars;

            sscanf (core_data_ptr, "%s%n", token_buffer, &num_chars);
            core_data_ptr += num_chars;

            /* read the next three tokens, this should be the
               VALUE = ... line */
            sscanf (core_data_ptr, "%s%n", token_buffer, &num_chars);
            core_data_ptr += num_chars;

            sscanf (core_data_ptr, "%s%n", token_buffer, &num_chars);
            core_data_ptr += num_chars;

            sscanf (core_data_ptr, "%s%n", token_buffer, &num_chars);
            core_data_ptr += num_chars;

            /* store the value name */
            strcpy (value, token_buffer );

            /* if this is HORIZONTALTILENUMBER or VERTICALTILENUMBER, then
               look for the PARAMETERVALUE token */
            if (!strcmp (token_buffer, "\"HORIZONTALTILENUMBER\"") ||
                !strcmp (token_buffer, "\"VERTICALTILENUMBER\""))
            {
                while (sscanf (core_data_ptr, "%s%n", token_buffer,
                       &num_chars) != EOF)
                {
                    /* if this token is END, then we are done with the
                       metadata */
                    if (!strcmp (token_buffer, "END"))
                        break;

                    /* increment the core_data_ptr pointer to point to the
                       next token */
                    core_data_ptr += num_chars;

                    /* look for the PARAMETERVALUE token */
                    if (!strcmp (token_buffer, "PARAMETERVALUE"))
                    {
                        /* read the next three tokens, this should be the
                           NUM_VAL = ... line */
                        sscanf (core_data_ptr, "%s%n", token_buffer,
                            &num_chars);
                        core_data_ptr += num_chars;

                        sscanf (core_data_ptr, "%s%n", token_buffer,
                            &num_chars);
                        core_data_ptr += num_chars;

                        sscanf (core_data_ptr, "%s%n", token_buffer,
                            &num_chars);
                        core_data_ptr += num_chars;

                        /* read the next three tokens, this should be the
                           CLASS = ... line */
                        sscanf (core_data_ptr, "%s%n", token_buffer,
                            &num_chars);
                        core_data_ptr += num_chars;

                        sscanf (core_data_ptr, "%s%n", token_buffer,
                            &num_chars);
                        core_data_ptr += num_chars;

                        sscanf (core_data_ptr, "%s%n", token_buffer,
                            &num_chars);
                        core_data_ptr += num_chars;

                        /* verify that the class number matches the class
                           number read earlier */
                        if (strcmp (token_buffer, class))
                            continue;

                        /* read the next three tokens, this should be the
                           VALUE = ... line */
                        sscanf (core_data_ptr, "%s%n", token_buffer,
                            &num_chars);
                        core_data_ptr += num_chars;

                        sscanf (core_data_ptr, "%s%n", token_buffer,
                            &num_chars);
                        core_data_ptr += num_chars;

                        /* this is our tile value so modify it, skip over the
                           blank space and first quote */
                        if (!strcmp (value, "\"HORIZONTALTILENUMBER\""))
                        {
                            /* set the value of the horizontal tile number */
                            strncpy (&core_data_ptr[2], htile, 2);
                            found_horiz = TRUE;
                            status = MRT_NO_ERROR;
                            break;
                        }
                        else if (!strcmp (value, "\"VERTICALTILENUMBER\""))
                        {
                            /* set the value of the vertical tile number */
                            strncpy (&core_data_ptr[2], vtile, 2);
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

    return (status);
}


/*************************************************************************

MODULE: CreateMeta

PURPOSE: 
    This module builds the necessary horizontal and vertical tile sections
    for the CoreMetadata attribute.

RETURN VALUE:
Type = int
Value    	Description
-----		-----------
!= NULL		Core metadata string is returned
NULL            Error occurred

HISTORY:
Version    Date     Programmer      Code     Reason
-------    ----     ----------      ----     ------
           03/05    Gail Schmidt             Original Development

NOTES:

**************************************************************************/
char *CreateMeta
(
    char *htile,                /* I: Horizontal tile value */
    char *vtile                 /* I: Vertical tile value */
)
{
    char *file_data = NULL;       /* char ptr used to store the metadata */
    
    /* Allocate space for the file_data ptr */
    file_data = (char *) calloc (4000, sizeof (char));
    if (file_data == NULL)
    {
        fprintf (stdout, "Error allocating space for the core metadata "
            "string.\n");
        return NULL;
    }

    /* Create the necessary tile metadata information */
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

    /* Return status */
    return (file_data);
}


/*************************************************************************

MODULE: AppendMeta

PURPOSE: 
    This module appends (or overwrites) the necessary tile coordinates to
    the HDF file.  These are written as the CoreMetadata.0 attribute.

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
           03/05    Gail Schmidt             Original Development

NOTES:

**************************************************************************/
intn AppendMeta
(
    int32 sd_id,                /* I: SDS id # for the HDF file */ 
    char *core_data             /* I: CoreMetadata string */
)
{
    char attr_name[MAX_NC_NAME];   /* holds attribute's name */

    /* The attribute name will be CoreMetadata.0 */
    strcpy (attr_name, "CoreMetadata.0");

    /* Attempt to write file metadata to HDF file */
    if (SDsetattr (sd_id, attr_name, DFNT_CHAR8, strlen (core_data),
       (void *) core_data) == FAIL)
    {
        fprintf (stdout, "Error writing the tile values to the "
            "CoreMetadata.0 in the metadata\n");
        return (ERROR_WRITE_IMAGE);
    }

    /* Return status */
    return (MRT_NO_ERROR);
}


/*************************************************************************

MODULE: Usage

PURPOSE: Prints the usage information for this executable.

RETURN VALUE:
Type = none

HISTORY:
Version    Date     Programmer      Code     Reason
-------    ----     ----------      ----     ------
           03/05    Gail Schmidt             Original Development

NOTES:

**************************************************************************/
void Usage ()

{
    fprintf (stdout, "Usage: update_tile_meta -f <filename> -h <htile> "
        "-v <vtile>\n");
    fprintf (stdout, "Where filename is the name of the HDF file to be "
        "modified.\n"
        "      htile is the 2-digit horizontal tile number (00-35)\n"
        "      vtile is the 2-digit vertical tile number (00-17)\n\n");
}
