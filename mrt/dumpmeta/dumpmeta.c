/************************************************************************

FILE: dumpmeta.c

PURPOSE:  Dump the metadata that is associated with the HDF file.

HISTORY:
Version    Date     Programmer      Code     Reason
-------    ----     ----------      ----     ------
1.0        UK       Zhangshi Yin             Original Development
           07/02    Gail Schmidt             Modified to work with the MRT

HARDWARE AND/OR SOFTWARE LIMITATIONS:
    None

PROJECT:        Modis Reprojection Tool

NOTES:   This program is an adaptation of hmr.c, developed by Zhangshi Yin.
The original hmr.c software was pulled from the HDF web site.

*************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#ifndef _MIPS_SZLONG
#define _MIPS_SZLONG 32
#endif
#include <mfhdf.h>

/* Read metadata from hdfName and write temp file. */
void readMetadata(char *hdfName, FILE *metadataFile, char *attr);

/* Read metadata from hdfFile and write temp file. */
intn readAttr(int32 hdfFile, FILE *metadataFile, char *attr);

int main(int argc, char *argv[])
{
  char *hdfName, *metadataName;
  FILE *metadataFile;

  /* Check for arguments. */
  if (argc !=3) 
  {
    printf("\n");
    printf("Usage: dumpmeta <input HDF file> <output metadata text file>\n");
    printf("\n");
    return EXIT_FAILURE;
  }

  /* Get HDF file name. */
  hdfName=argv[1];
  
  /* Get meta file name. */
  metadataName=argv[2];
  metadataFile=fopen(metadataName, "w");

  /* Read metadata from hdf file. Write to temporary inventory and archive
     files. */
  readMetadata(hdfName, metadataFile, "StructMetadata");
  readMetadata(hdfName, metadataFile, "structmetadata");
  readMetadata(hdfName, metadataFile, "ProductMetadata");
  readMetadata(hdfName, metadataFile, "productmetadata");
  readMetadata(hdfName, metadataFile, "CoreMetadata");
  readMetadata(hdfName, metadataFile, "coremetadata");
  readMetadata(hdfName, metadataFile, "ArchiveMetadata");
  readMetadata(hdfName, metadataFile, "archivemetadata");
  readMetadata(hdfName, metadataFile, "InventoryMetadata");
  readMetadata(hdfName, metadataFile, "inventorymetadata");
  readMetadata(hdfName, metadataFile, "Collection");
  readMetadata(hdfName, metadataFile, "collection");
  readMetadata(hdfName, metadataFile, "CollectionMetadata");
  readMetadata(hdfName, metadataFile, "collectionmetadata");

  fclose(metadataFile);
  return EXIT_SUCCESS;
}


/* Read metadata from hdfName and write temp file. */
void readMetadata(char *hdfName, FILE *metadataFile, char *attr)
{
  intn i;
  int32 hdfFile;
  char attrname[100];

  hdfFile = SDstart(hdfName, DFACC_READ);
  if (hdfFile == -1) 
  {
    fprintf(stdout, "Unable to open HDF file %s \n", hdfName);
    fflush(stdout);
    exit(EXIT_FAILURE);
  }

  /* First, try attr name alone */
  readAttr(hdfFile, metadataFile, attr);

  /* Now, try concatenating sequence numbers */
  for (i = 0; i <= 9; i++) 
  {
    sprintf(attrname, "%s.%d", attr, i);
    readAttr(hdfFile, metadataFile, attrname);
  }

  SDend(hdfFile);
}


/* Read metadata from hdfFile and write temp file. */
intn readAttr(int32 hdfFile, FILE *metadataFile, char *attr)
{
  intn status = 0;
  int32 attr_index, data_type, n_values;
  char *metadata, attr_name[MAX_NC_NAME];

  /* Look for attribute */
  attr_index = SDfindattr(hdfFile, attr);
  if (attr_index == -1) 
  {
/*    fprintf(stdout, "Unable to find attribute %s.\n", attr);
    fflush(stdout); 
*/
    return (1);
  }
 
  /* Get size of attribute  */
  status = SDattrinfo(hdfFile, attr_index, attr_name, &data_type, &n_values);
  if (status == -1) 
  {
    fprintf(stdout, "SDattrinfo failed for existing attribute %s.\n", attr);
    fflush(stdout);
    return (1);
  }
 
  /* Allocate memory for attribute contents (add one character for the end
     of string character) */
  metadata = (char *) malloc(n_values+1);
  if (metadata == NULL) 
  {
    fprintf(stdout, "Unable to allocate %d bytes for %s.\n", (int) n_values,
        attr);
    fflush(stdout);
    return (1);
  }
 
  /* Read attribute */
  status = SDreadattr(hdfFile, attr_index, metadata);
  if (status == -1 || !strcmp (metadata, "")) 
  {
    fprintf(stdout, "Unable to read attribute %s.\n", attr);
    fflush(stdout);
    free(metadata);
    return (1);
  }

  fprintf(metadataFile, "%s", metadata);

  /* Clean up and return success */
  free(metadata);
  return (0);
}

