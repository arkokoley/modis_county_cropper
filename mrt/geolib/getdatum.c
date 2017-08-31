/*******************************************************************************

NAME			      c_getdatum

PURPOSE	     Retrieve datum information from datum and spheroid tables and fill
             the DATUMDEF variable

PROGRAM HISTORY
PROGRAMMER		DATE		REASON
----------		----		------
J. Willems		02/98    	Initial development for datum conversion
D. Lloyd		05/99    	Free malloc'd memory
T. Ratliff		10/99		Redesigned the routine to fix an
					  error and be more efficient
D. Hames		12/99		Previous redesign discontinued the file
					check to spheroid.txt after a datum code
					was assigned. Copy and modify c_getdatum
					version 09/01/99. Add additional checks
					to while statements to fix infinite 
					looping and fatal error if the datum is
					not found. SRF#3127.
PROJECT     LAS

ALGORITHM 
	Check for valid datum codes
        If the datum number shows a datum is present
  	  Read datum table until datum value have been retrieved 
	    assign datum values from the table
        If there is only a spheroid represented assign the number
            and assign everything else to zero
	Read spheroid table until spheroid number has been retrieved
	  assign spheroid values with values from the table
	return

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "shared_resample.h"

#include "tae.h"
#include "worgen.h"
#include "datum.h"
#include "geompak.h"

#define MINDTM 99

int c_getdatum
(
    struct DATUMDEF *dtm_info   /* Datum code */
)
{
FILE *file_ptr;         	/* Pointer to current table */
long dtm_num = -1;		/* Datum code */
long found = FALSE;		/* Flag set if line contains correct code */
long dtmflag = FALSE;		/* Flag set if datum code represents datum */
long i = 0;			/* increment for loop */
long numb = -1;			/* Number retrieved form table */

char *temp;			/* temporary storage of line in table */
char *table_ptr;		/* Location of the LASTABLES */
char line[BUFSIZ];		/* Line scanned in from datum table */
char *dtmary[11] = {0};		/* Array of dtm_num fields */
char *spherary[4] = {0};	/* Array of spher fields */
char dtmfile[BUFSIZ];          	/* Pointer to datum table */
char spherfile[BUFSIZ];		/* Pointer to spheroid table */
char errmsg[256];

double flat = 0.0;              /* Flattening of the spheroid */

dtm_num = dtm_info->datumnum;

/* Check for a valid datum code
  ----------------------------*/
if(dtm_num < 0)
   {
   return(E_GEO_FAIL);
   }
else if(dtm_num > MINDTM)
   dtmflag = TRUE;

/* Get the directory of the tables
  ------------------------------*/
table_ptr = getenv("MRT_DATA_DIR");
if( table_ptr == NULL ) {
 table_ptr = getenv("MRTDATADIR");
 if (table_ptr == NULL) {
   fprintf (stderr,
    "Error: MRT_DATA_DIR or MRTDATADIR environment variable is not set\n");
   return(E_GEO_FAIL);
 } 
}

/* Add the file names
  ------------------*/
sprintf(dtmfile,"%s/datum.txt",table_ptr);
sprintf(spherfile,"%s/spheroid.txt",table_ptr); 

/* Open the datum file.  Retrieve the necessary data
  -------------------------------------------------*/
if ((file_ptr = fopen(dtmfile, "r")) == NULL)
     {
     sprintf( errmsg, "Cannot open %s", dtmfile);
     ErrorHandler( FALSE, "getdatum", ERROR_OPEN_DATUMFILE, errmsg );
     return(E_GEO_FAIL);
     }

/* If datum is present, retrieve information from datum table
  ----------------------------------------------------------*/
if (dtmflag)
   {
   while ((fgets(line,BUFSIZ,file_ptr) != NULL) && (found != TRUE))
         {
         sscanf(line, "%ld", &numb);
         if (numb == dtm_num)
            {
            for (i = 0; i < 11; i++)
                {
                temp = strtok(i ? NULL: line, ":");
                dtmary[i] = malloc(strlen(temp) + 1);
                strcpy(dtmary[i], temp);
                }
            strcpy(dtm_info->datumname, dtmary[1]);
            strcpy(dtm_info->area, dtmary[2]);
            strcpy(dtm_info->category, dtmary[3]);
            dtm_info->spherenum = atol(dtmary[4]);
            dtm_info->xshift = atol(dtmary[5]);
            dtm_info->yshift = atol(dtmary[6]);
            dtm_info->zshift = atol(dtmary[7]);
            dtm_info->xdelta = atol(dtmary[8]);
            dtm_info->ydelta = atol(dtmary[9]);
            dtm_info->zdelta = atol(dtmary[10]);
            found = TRUE;
            }
         }
   if (found == FALSE)
      {
      fprintf (stderr, "Error: Datum number %ld not found in %s\n", dtm_num,
          dtmfile);
      return (E_GEO_FAIL);
      }
   }

/* Since there is no datum, assign sphere numb and set everything else to zero
  ---------------------------------------------------------------------------*/
else
   {
   dtm_info->spherenum = dtm_num;
   dtm_info->datumname[0] = '\0';
   dtm_info->area[0] = '\0';
   dtm_info->category[0] = '\0';
   dtm_info->xshift = 0;
   dtm_info->yshift = 0;
   dtm_info->zshift = 0;
   dtm_info->xdelta = 0;
   dtm_info->ydelta = 0;
   dtm_info->zdelta = 0;
   }

fclose(file_ptr);

/* Open the spheroid file.  Retrieve the necessary data
  ----------------------------------------------------*/
if ((file_ptr = fopen(spherfile, "r")) == NULL)
   {
   fprintf (stderr, "Error: Cannot open %s\n", spherfile);
   return(E_GEO_FAIL);
   }

found = FALSE;
while ((fgets(line,BUFSIZ,file_ptr) != NULL) && (found != TRUE))
   {
   sscanf(line, "%ld", &numb);
   if (numb == dtm_info->spherenum)
      {
      for (i = 0; i < 4; i++)
          {
          temp = strtok(i ? NULL: line, ":");
          spherary[i] = malloc(strlen(temp) + 1);
          strcpy(spherary[i], temp);
          }
      strcpy(dtm_info->spherename, spherary[1]);
      dtm_info->smajor = atof(spherary[2]);
      dtm_info->recip_flat = atof(spherary[3]);

      /* If the recip_flat is zero it is a sphere, otherwise compute minor
       ------------------------------------------------------------------*/
      if (dtm_info->recip_flat != 0)
         {
         flat = (1/dtm_info->recip_flat);
         dtm_info->sminor = (dtm_info->smajor * (1 - flat));
         }
      else
         dtm_info->sminor = atof(spherary[2]);
      found = TRUE;
      }
   }

if (found == FALSE)
   {
   fprintf (stderr, "Error: Sphere number %ld not found in %s\n",
       numb, spherfile);
   return (E_GEO_FAIL);
   }

fclose(file_ptr);

/* Free malloc'd memory
-----------------------*/
for ( i = 0; i < 11; i++ )
    if ( dtmary[i] != NULL )
	free( dtmary[i] );
for ( i = 0; i < 4; i++ )
    if ( spherary[i] != NULL )
	free( spherary[i] );

return(E_GEO_SUCC);
}
