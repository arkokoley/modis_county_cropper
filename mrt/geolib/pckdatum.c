/*******************************************************************************
NAME			      PACKDATUM

PURPOSE	     Places spheroid and datum values into projection parameter array
             and datum array

PROGRAM HISTORY
PROGRAMMER		DATE		REASON
----------		----		------
J. Willems		1998		Initial development for datum conversion
					

PROJECT     LAS

ALGORITHM 
	Check for valid datum codes
	If datums are not both spheroids or datums, error out
	If inproj or outproj are GEO, UTM, or State Plane and the first value
	is set in the parm array, 
	  set projflag to FALSE so the semi-major axis is not stuffed there
	If we don't need to assign proj. info and no datum conv
          return
	Set the datumflag depending on what type of conversion needs to happen.
	  Set the temporary datum if NADCON will be doing part of the conv.
	Fill the datum structure 
	If indatum and outdatum represent datums and are not equal, (dtm conv.)
          If inproj and outproj are not zero,
 	    compare the values to those retrieved from the table 
	    warn if they are not equal
	If parmflag is TRUE
	  Fill inproj and outproj with values from the table
	return

ALGORITHM REFERENCES
Based on spheroid.c written by Dan Steinwand, and by algorithms written with
Shar Nelson.

1.  Snyder, John P., "Map Projections--A Working Manual", U.S. Geological
    Survey Professional Paper 1395 (Supersedes USGS Bulletin 1532), United
    State Government Printing Office, Washington D.C., 1987. 

2.  "Software Documentation for GCTP General Cartographic Transformation
    Package", U.S. Geological Survey National Mapping Division, May 1982.

3.  Clarie, Charles N, "State Plane Coordinates by Automatic Data Processing",
    U.S. Department of Commerce, Environmental Science Services Admin.,
    Coast and Geodetic Survey, Publication 62-4, 1973.
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shared_resample.h"

#include "tae.h"
#include "worgen.h"
#include "cproj.h"
#include "proj.h"
#include "datum.h"
#include "geompak.h"

#define AXIS_TOL	0.01	/* Tolerance for the semi-major and minor axis*/
#define ECSQ_TOL	0.0001	/* Tolerance for the eccentricity squared */

int packdatum
(
    long inproj,			/* I: GCTP projection code */
    long outproj,			/* I: GCTP projection code */
    struct DATUMDEF *indtmparm,	/* O: Input datum structure */
    struct DATUMDEF *outdtmparm,	/* O: Output datum structure */
    double inparm[15],	       /* I: Array of 15 input projection parameters */
    double outparm[15],	       /* I: Array of 15 output projection param. */
    long *datumflag            /* O: Flag for conversion, listed in Transform */
)
{
long inparmflg = TRUE;		/* Flag set if inparm should not be set */
long outparmflg = TRUE;		/* Flag set if outparm should not be set */
char error_string[100];   /*  string to hold info for error messages */


/* Check for a valid datum code
  ----------------------------*/
if((indtmparm->datumnum < 0) || (outdtmparm->datumnum < 0) ||
   (indtmparm->datumnum > MAX_DATUM) || (outdtmparm->datumnum > MAX_DATUM))
   {
		ErrorHandler( FALSE, "packdatum", ERROR_PROJECTION,
					  "Datum code value out of valid range" );
   return(E_GEO_FAIL);
   }

/* If one datum represents a spheroid and the other a datum, 
   the conversion cannot happen.  
  ---------------------------------------------------------*/ 
if (((indtmparm->datumnum > MINDTM) && (outdtmparm->datumnum < MINDTM)) || 
    ((indtmparm->datumnum < MINDTM) && (outdtmparm->datumnum > MINDTM)))
    {
	ErrorHandler( FALSE, "packdatum", ERROR_PROJECTION,
	"The Datums in this transformation do not appear to be compatible" );
     
     /* get datum info for error message */
     if (c_getdatum(indtmparm) != E_GEO_SUCC)
        {
		ErrorHandler( FALSE, "packdatum", ERROR_PROJECTION, 
		"Could not retrieve input datum information" );
        return(E_GEO_FAIL);
        }
     if (c_getdatum(outdtmparm) != E_GEO_SUCC)
        {
		ErrorHandler( FALSE, "packdatum", ERROR_PROJECTION,
		"Could not retrieve output datum information" );
        return(E_GEO_FAIL);
        }

     /* provide input infomation to user */
     if(indtmparm->datumnum >  MINDTM) /* is input value a datum */
        {
        sprintf(error_string,"%s %s:%s:%s ", "  Input System: ",
             indtmparm->datumname,indtmparm->area,indtmparm->category);
             ErrorHandler( FALSE, "packdatum", ERROR_PROJECTION, error_string );
		
        sprintf(error_string,"%s %ld", "          Datum Code:  ",
             indtmparm->datumnum);
             ErrorHandler( FALSE, "packdatum", ERROR_PROJECTION, error_string );
        }
     else                               /* input is a sphere */
        {
        if (indtmparm->spherenum == 19 || indtmparm->recip_flat==0)
        {
           sprintf(error_string,"%s %s %s %ld %s", "  Input System: ",
                   "Sphere","of Radius", (long)indtmparm->smajor,"meters"); 
        }
        else
        {
            sprintf(error_string,"%s %s %s %ld %s %s %ld %s", 
                    "  Input System: ",indtmparm->spherename, "Major Axis:",
                    (long)indtmparm->smajor,"meters","Minor Axis:",
                    (long)indtmparm->sminor,"meters");
        }
		ErrorHandler( FALSE, "packdatum", ERROR_PROJECTION, error_string );
        sprintf(error_string,"%s %ld", "          Spheroid Code: ",
                indtmparm->spherenum);
		ErrorHandler( FALSE, "packdatum", ERROR_PROJECTION, error_string );
        }

     if(outdtmparm->datumnum > MINDTM) /* is output value a datum */
        {
        sprintf(error_string,"%s %s:%s:%s", "  Output System: ",
                outdtmparm->datumname,outdtmparm->area,outdtmparm->category);
		ErrorHandler( FALSE, "packdatum", ERROR_PROJECTION, error_string );
        sprintf(error_string,"%s %ld", "          Datum Code: ",
                outdtmparm->datumnum);
		ErrorHandler( FALSE, "packdatum", ERROR_PROJECTION, error_string );
        }
     else                              /* output is a sphere */
        {
        if (outdtmparm->spherenum == 19 || outdtmparm->recip_flat==0)
            {
            sprintf(error_string,"%s %s %s %ld %s", "  Output System: ",
                    "Sphere","of Radius", (long)outdtmparm->smajor,"meters"); 
            }
        else
            {
            sprintf(error_string,"%s %s %s %ld %s %s %ld %s", 
                    "  Output System: ",outdtmparm->spherename, "Major Axis:",
                    (long)outdtmparm->smajor,"meters","Minor Axis:",
                    (long)outdtmparm->sminor,"meters");
            }
		ErrorHandler( FALSE, "packdatum", ERROR_PROJECTION, error_string );
        sprintf(error_string,"%s %ld", "          Spheroid Code: ",
                outdtmparm->spherenum);
		ErrorHandler( FALSE, "packdatum", ERROR_PROJECTION, error_string );
        }
     return(E_GEO_FAIL);
    }

/* If GEO,UTM,or SPCS, do not fill the projection parameters.  GCTP does 
   not have semi-major and minor defined as first two parameters for these 
   projections.
  -----------------------------------------------------------------------*/
if((inproj == GEO) || (inproj == SPCS) || (inproj == UTM))
   inparmflg = FALSE;

if((outproj == GEO) || (outproj == SPCS) || (outproj == UTM)) 
   outparmflg = FALSE;

/* Set flag if there will not be a datum conversion.
 ---------------------------------------------------*/
if ((indtmparm->datumnum < MINDTM) || 
   (indtmparm->datumnum == outdtmparm->datumnum))
   {
   *datumflag = NO_TRANS;
   }

/* If indtmparm is NAD27, then flag is set based on whether outdtmparm is
   NAD83 or not.
 --------------------------------------------------------------------*/
else if((indtmparm->datumnum >= MINNAD27) && (indtmparm->datumnum <= MAXNAD27))
   {
   if ((outdtmparm->datumnum >= MINNAD27) && (outdtmparm->datumnum <= MAXNAD27))
      {
      if (indtmparm->datumnum != outdtmparm->datumnum)
         {
			ErrorHandler( FALSE, "packdatum", ERROR_PROJECTION, 
			"Cannot convert from NAD27 to NAD27" );
         return(E_GEO_FAIL);
         }
      }
   else if((outdtmparm->datumnum<MINNAD83) || (outdtmparm->datumnum>MAXNAD83))
      {
      *datumflag = NAD27_TO_GEN;
      }
   else
      {
      *datumflag = NAD27_TO_83;
      }
   }

/* Check if outdtmparm is NAD27, and set indtmparm if needed.
 -------------------------------------------------------*/
else if((outdtmparm->datumnum >= MINNAD27)&&(outdtmparm->datumnum <= MAXNAD27))
   {
   if (indtmparm->datumnum < MINNAD83 || indtmparm->datumnum > MAXNAD83)
      {
      *datumflag = GEN_TO_27;
      }
   else
      {
      *datumflag = NAD83_TO_27;
      }
   }
else
   {
   *datumflag = MOLODENSKY;
   }

/* If both projection parameters don't need to be defined and no 
   datum conversion will happen, exit
  -------------------------------------------------------------*/
if ((inparmflg == FALSE)&&(outparmflg == FALSE)&&(indtmparm->datumnum < MINDTM))
   {
   *datumflag = NO_TRANS;
   }

/* Fill the Datum Structures for LAS datum conversion
 ---------------------------------------------------*/
if (c_getdatum(indtmparm) != E_GEO_SUCC)
   {
		ErrorHandler( FALSE, "packdatum", ERROR_PROJECTION, 
		"Could not retrieve input datum information" );
   return(E_GEO_FAIL);
   }
if (c_getdatum(outdtmparm) != E_GEO_SUCC)
   {
		ErrorHandler( FALSE, "packdatum", ERROR_PROJECTION, 
		"Could not retrieve output datum information" );
   return(E_GEO_FAIL);
   }

/* Check the projection parameter values against the datum code
   as long as projection is not UTM, GEO, or STATE PLANE
 -------------------------------------------------------------*/
if (inparmflg == TRUE)
  if (proj_check(indtmparm, inparm, &inparmflg) != E_GEO_SUCC)
     return(E_GEO_FAIL);
if (outparmflg == TRUE)
  if (proj_check(outdtmparm, outparm, &outparmflg) != E_GEO_SUCC)
     return(E_GEO_FAIL);

/* Fill projection parameters
  --------------------------*/
if (inparmflg)
   {
   /* If this is the MODIS sphere, then don't reset the input projection
      parameters. */
   if ((inproj != ISINUS && inproj != SNSOID) ||
       fabs(inparm[0] - 6371007.181) > AXIS_TOL)
      {
      inparm[0] = indtmparm->smajor;
      inparm[1] = indtmparm->sminor;
      }
   }
if (outparmflg)
   {
   outparm[0] = outdtmparm->smajor;
   outparm[1] = outdtmparm->sminor;
   }

return(E_GEO_SUCC);
}

/******************************************************************************

NAME:                           proj_check

PURPOSE:  Check the projection parameter values representing the semi-major
          and semi-minor axis against those associated with the datum code
          value.

HISTORY:  When datum conversions were not possible, the values in the proj.
          parameter were used to pass to GCTP to do the projection conversion.
          Once the datum conversion was implemented, it was decided to use
          the values associated with the datum code to insure accuracy.  This
          made the users concerned that the values in the proj. could differ
          from the ones used.  Therefore, we need to check the values, and if
          they are not the same, we need to fatal error out.

          Gail Schmidt   11/02   If the radius specified in the projection
                                 parameters is the radius for the Modis
                                 sphere, then an override of the defualt
                                 sphere 19 radius is ok.


PARAMETERS:     DTMPARM
		PROJPARM

*****************************************************************************/
int proj_check
(
    struct DATUMDEF *dtmparm,	/* I: Datum structure */
    double projparm[15],	/* I: Array of 15 projection param. */
    long *parmflg		/* O: Flag set if proj param. should be set */
)
{
double e_square;		/* Eccentricty squared for the spheroid */
double flat;			/* Flattening for the spheroid */


/* Check to see if the semi-major is defined, if not we don't need to worry
   about the minor either
  ------------------------------------------------------------------------*/
if(fabs(projparm[0]) > 0.000001)
  {
  /* Check to see if the semi-major axis matches.  If not, fatal error out
     If it does, check the minor axis.
   ----------------------------------------------------------------------*/
  if (fabs(projparm[0] - dtmparm->smajor) > AXIS_TOL)
     {
        /* If the radius specified in the projection parameters is the
           radius for the Modis sphere, then an override of the defualt
           sphere 19 radius is ok. */
        if (fabs(projparm[0] - 6371007.181) > AXIS_TOL)
        {
           ErrorHandler( FALSE, "proj_check", ERROR_PROJECTION, 
             "Major axis in projection array does not match datum code" );
           return (E_GEO_FAIL);
        }
     }

  /* See if the semi-minor axis is defined
   --------------------------------------*/
  else 
     {
     if (fabs(projparm[1]) > 0.000001) 
        {
        if (projparm[1] < 1) 
          {
          /* If the value is less than 1, it represents the eccentricity 
            squared. Calculate it for the datum and compare.
           -----------------------------------------------------------*/
          flat = (1 / dtmparm->recip_flat);
          e_square = ((2 * flat) - SQUARE(flat));
          if (fabs(projparm[1] - e_square) > ECSQ_TOL)
            {
			ErrorHandler( FALSE, "proj_check", ERROR_PROJECTION, 
			"Flattening in projection array does not match datum code" );
                return (E_GEO_FAIL);
            }
          /* Since the eccentricty squared is defined and it matches, set
             the parm flag to FALSE so the value isn't overwritten with the
             semi-minor
           ---------------------------------------------------------------*/
          *parmflg = FALSE;
          }

        /* If the value is greater than 1, it represents the semi-minor,
           compare it against the datum value
         --------------------------------------------------------------*/
        else if((projparm[1] >1)
                &&(fabs(projparm[1]-dtmparm->sminor) > AXIS_TOL))
           {
			ErrorHandler( FALSE, "proj_check", ERROR_PROJECTION, 
			"Minor axis in projection array does not match datum code" );
                return (E_GEO_FAIL);
           }
        }
     /* If the semi-minor was zero, there is a spheroid.  Check the major of
        the parameters against the minor since they should be the same.
      ---------------------------------------------------------------------*/
     else if (fabs(projparm[0] - dtmparm->sminor) > AXIS_TOL)
        {
			ErrorHandler( FALSE, "proj_check", ERROR_PROJECTION, 
			"Minor axis in projection array does not match datum code" );
/*                return (E_GEO_FAIL); */
        }
     }
  }
return(E_GEO_SUCC);
}
