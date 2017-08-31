#ifndef CPROJ_H
#define CPROJ_H

#include <math.h>
#include "proj.h"

#define PI 	3.141592653589793238
#define HALF_PI (PI*0.5)
#define TWO_PI 	(PI*2.0)
#define EPSLN	1.0e-10
#define EPSLN2	1.0e-12
#define R2D     57.2957795131
/*
#define D2R     0.0174532925199
*/
#define D2R     1.745329251994328e-2
#define S2R	4.848136811095359e-6

#define RADIAN  0
#define FEET    1
#define METER   2
#define SECOND  3
#define DEGREE  4
#define DMS     5

#define GCTP_OK	0
#define GCTP_ERROR  -1
#define GCTP_IN_BREAK -2
#define GCTP_ERANGE -9

/* Misc macros
  -----------*/
#define SQUARE(x)       x * x   /* x**2 */
#define CUBE(x)     x * x * x   /* x**3 */
#define QUAD(x) x * x * x * x   /* x**4 */

#define GMAX(A, B)      ((A) > (B) ? (A) : (B)) /* assign maximum of a and b */
#define GMIN(A, B)      ((A) < (B) ? (A) : (B)) /* assign minimum of a and b */

#define IMOD(A, B)      (A) - (((A) / (B)) * (B)) /* Integer mod function */

#endif
