/*******************************************************************************
NAME:           LAMBERT AZIMUTHAL EQUAL-AREA (Ellipsoid)
 
PURPOSE:	Transforms input Easting and Northing to longitude and
                latitude for the Lambert Azimuthal Equal-Area projection,
		based on an ellipsoid.  The Easting and Northing values must
		be in meters.  The longitude and latitude will be returned
		in radians.  

PROGRAMMER              DATE            
----------              ----           
G. Schmidt, SAIC/EROS   December 2005

ALGORITHM REFERENCES:

1.  "Map Projections Used by the U.S. Geological Survey", John P. Snyder,
    Geological Survey Bulletin 1532, Second Edition Reprinted, 1984.
    
NOTES:
The inverse equations specify that they are not valid for the lat_origin
(center_lat) of +-90 degrees.
*******************************************************************************/
#include <stdio.h>
#include "cproj.h"

/* Variables common to all subroutines in this code file
  -----------------------------------------------------*/
static double semi_major;	/* semi-major axis */
static double semi_minor;	/* semi-minor axis */
static double lon_center;	/* center longitude (projection center) */
static double lat_origin;	/* center latitude */
static double sin_lat_origin;	/* sine of the center latitude */
static double cos_lat_origin;	/* cosine of the center latitude */
static double false_easting;	/* x offset in meters */
static double false_northing;	/* y offset in meters */
static double e,e2;		/* eccentricity constants (e and e-squared) */
static double qp,q1;            /* q sub p and q sub 1 variables */
static double beta1;            /* beta sub 1 variable */
static double sin_beta1;        /* sine of beta sub 1 variable */
static double cos_beta1;        /* cosine of beta sub 1 variable */
static double Rq;               /* R sub q variable */
static double m1;               /* m sub 1 variable */
static double D;                /* D variable */

/* Initialize the Lambert Azimuthal Equal Area projection for ellipsoids
  ----------------------------------------------------------------------*/
long lamazellipsinvint
(
    double r_major,		/* I: Semi-major axis */
    double r_minor,		/* I: Semi-minor axis */
    double center_long,		/* I: Center longitude (radians) */
    double center_lat,		/* I: Center latitude (radians) */
    double false_east,		/* I: x offset in meters */
    double false_north		/* I: y offset in meters */
)
{
    double sin90;               /* sine of 90 degrees */

    /* Place parameters in static storage for common use
      -------------------------------------------------*/
    semi_major = r_major;
    semi_minor = r_minor;
    lon_center = center_long;
    lat_origin = center_lat;
    false_easting = false_east;
    false_northing = false_north;
    gctp_sincos (lat_origin, &sin_lat_origin, &cos_lat_origin);

    e2 = (my_square(semi_major) - my_square(semi_minor)) /
        my_square(semi_major);
    e = sqrt(e2);
    sin90 = sin(90.0 * D2R);
    qp = (1.0-e2) * ((sin90 / (1.0-e2*my_square(sin90))) - (1.0/(2.0*e)) *
         log ((1.0-e*sin90) / (1.0+e*sin90)));
    q1 = (1.0-e2) * ((sin_lat_origin / (1.0-e2*my_square(sin_lat_origin))) -
        (1.0/(2.0*e)) * log ((1.0-e*sin_lat_origin) / (1.0+e*sin_lat_origin)));
    beta1 = asin(q1/qp);
    gctp_sincos (beta1, &sin_beta1, &cos_beta1);
    Rq = semi_major * sqrt(qp*0.5);
    m1 = cos_lat_origin / sqrt(1.0 - e2*my_square(sin_lat_origin));
    D = semi_major * m1 / (Rq*cos_beta1);

    /* Report parameters to the user
      -----------------------------*/
    ptitle("LAMBERT AZIMUTHAL EQUAL-AREA"); 
    radius2(semi_major, semi_minor);
    cenlon(lon_center);
    cenlat(lat_origin);
    offsetp(false_easting,false_northing);
    return(GCTP_OK);
}

/* Lambert Azimuthal Equal Area ellipsoidal inverse equations -- mapping
   x,y to lat,long
  -----------------------------------------------------------------------*/
long lamazellipsinv
(
    double x,			/* I: X projection coordinate (meters) */
    double y,			/* I: Y projection coordinate (meters) */
    double *lon,		/* O: Longitude (radians) */
    double *lat 		/* O: Latitude (radians) */
)
{
    #define MAX_ITER_LAT 10
    int num_iter;               /* number of iterations for lat calculation */
    double sin_lat;		/* Sine of the given latitude */
    double cos_lat;		/* Cosine of the given latitude	*/
    double sin_ce;		/* Sine of the ce variable */
    double cos_ce;		/* Cosine of the ce variable */
    double q;                   /* q variable */
    double rowvar;              /* row variable */
    double ce;                  /* c sub e variable */
    double tempval;             /* temporary variable */
    double currlat;             /* current latitude value */
    double prevlat;             /* previous latitude value */

    /* Inverse equations
      -----------------*/
    x -= false_easting;
    y -= false_northing;
    rowvar = sqrt(my_square(x/D) + my_square(D*y));
    ce = 2.0 * asin(rowvar / (2.0*Rq));
    gctp_sincos(ce, &sin_ce, &cos_ce);
    q = qp * (cos_ce*sin_beta1 + (D*y*sin_ce*cos_beta1/rowvar));

    /* Calculate output longitude
      --------------------------*/
    *lon = lon_center + atan(x*sin_ce / (D*rowvar*cos_beta1*cos_ce -
        (my_square(D)*y*sin_beta1*sin_ce)));

    /* Special case on q in which the latitude iteration does not converge.
       If this case is true, then the latitude is 90 degrees with the sign
       being that of q.
      --------------------------------------------------------------------*/
    tempval = 1.0 - (((1.0 - e2) / (2.0*e)) * log((1.0 - e) / (1.0 + e)));
    if (fabs (fabs(q) - fabs(tempval)) < EPSLN2)
    {
        *lat = sign(q) * 90.0 * D2R;
        return(GCTP_OK);
    }

    /* Calculate output latitude. This is an iterative process. Start with
       a latitude of arcsin(q/2) then interate until the change in latitude
       is negligible.
      --------------------------------------------------------------------*/
    currlat = asin(q*0.5);
    prevlat = currlat;
    num_iter = 0;
    while ((fabs (currlat - prevlat) > EPSLN2) && (num_iter < MAX_ITER_LAT))
    {  /* If the difference isn't negligible then keep going. Stop at
          MAX_ITER_LAT iterations. */
       prevlat = currlat;
       gctp_sincos(prevlat, &sin_lat, &cos_lat);
       currlat = prevlat + (my_square(1.0 - e2*my_square(sin_lat)) /
          (2.0*cos_lat)) * ((q / (1.0 - e2)) - (sin_lat /
          (1.0 - e2*my_square(sin_lat))) + (1.0 / (2.0 * e)) *
          log((1.0 - e*sin_lat) / (1.0 + e*sin_lat)));
       num_iter++;
    }
    *lat = currlat;

    return(GCTP_OK);
}
