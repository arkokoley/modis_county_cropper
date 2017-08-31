
/******************************************************************************

NAME:				C_FILE_CORNER

PURPOSE: Uses a QR factorization of a polynomial to find the file projection
	 corners of an image.  This information is found from the corners of
	 the valid image.

PROGRAM HISTORY:
  Version       Date	Author		Request
  -------       ----	------		-------
   1.0		3/95	S. Nelson	Original Development
   1.1		6/95	S. Nelson	Stole from INPEDDR to develop a more
					general support.
   1.2		7/95	S. Nelson	Added output parameter out_corners

******************************************************************************/

/* include files, global parameters, and definitions for FDHR2DDR    */

#include <math.h>
#include "las.h"
#include "lasmath.h"
#include "geompak.h"

void c_file_corner( double *in_corners,	/* input image data corners:
					   0:ul y, 1:ul x
					   2:ur y, 3:ur x
					   4:ll y, 5:ll x
					   6:lr y, 7:lr x */
		    double *ls_corners,	/* line/sample corners as above */
		    long *window,	/* image window */
		    double *out_corners	/* output image data corners as above */
     )
{

/* file names 
  ----------*/
    long one = 1;		/* one */
    long i;			/* loop control variable */
    double A[12];		/* matrix of line/sample of qr function */
    double projy[4];		/* vector of line search space */
    double projx[4];		/* vector of sample serach space */
    double v[4];
    double sample;
    double line;

/* Get assign line/sample values to the matrix
  -------------------------------------------*/
    for ( i = 0; i < 4; i++ )
    {
	A[i] = 1.0;
	A[i + 4] = ls_corners[i * 2 + 1];
	A[i + 8] = ls_corners[i * 2];
    }

/* assign x/y values
   ----------------*/

    projx[0] = in_corners[UPLEFT_X];
    projx[1] = in_corners[UPRIGHT_X];
    projx[2] = in_corners[LOLEFT_X];
    projx[3] = in_corners[LORIGHT_X];
    projy[0] = in_corners[UPLEFT_Y];
    projy[1] = in_corners[UPRIGHT_Y];
    projy[2] = in_corners[LOLEFT_Y];
    projy[3] = in_corners[LORIGHT_Y];

/* find the polynomial values by QR factorization
   --------------------------------------------*/
    c_qrdecomp( A, 4, 3, v, 0 );
    c_qrsolve( A, 4, 3, v, projy, 0 );
    c_qrsolve( A, 4, 3, v, projx, 0 );

/* find the ul corner projection x/y value
  ---------------------------------------*/
    line = window[SL];
    sample = window[SS];
    out_corners[UPLEFT_Y] = c_eval( &one, projy, &sample, &line );
    out_corners[UPLEFT_X] = c_eval( &one, projx, &sample, &line );

/* find the ur corner projection x/y value
  ---------------------------------------*/
    line = window[SL];
    sample = window[NS] + window[SS] - 1;
    out_corners[UPRIGHT_Y] = c_eval( &one, projy, &sample, &line );
    out_corners[UPRIGHT_X] = c_eval( &one, projx, &sample, &line );

/* find the lr corner projection x/y value
  ---------------------------------------*/
    line = window[NL] + window[SL] - 1;
    sample = window[NS] + window[SS] - 1;
    out_corners[LORIGHT_Y] = c_eval( &one, projy, &sample, &line );
    out_corners[LORIGHT_X] = c_eval( &one, projx, &sample, &line );

/* find the ll corner projection x/y value
  ---------------------------------------*/
    line = window[NL] + window[SL] - 1;
    sample = window[SS];
    out_corners[LOLEFT_Y] = c_eval( &one, projy, &sample, &line );
    out_corners[LOLEFT_X] = c_eval( &one, projx, &sample, &line );

}
