/*******************************************************************************
NAME:           LAMBERT AZIMUTHAL EQUAL-AREA (Ellipsoid)
 
PURPOSE:	Transforms input longitude and latitude to Easting and
		Northing for the Lambert Azimuthal Equal-Area projection,
		based on an ellipsoid.  The longitude and latitude must be
		in radians.  The Easting and Northing values will be
		returned in meters.

PROGRAMMER              DATE            
----------              ----           
G. Schmidt, SAIC/EROS   December 2005

ALGORITHM REFERENCES:

1.  "Map Projections Used by the U.S. Geological Survey", John P. Snyder,
    Geological Survey Bulletin 1532, Second Edition Reprinted, 1984.
    
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

double my_square (double x)
{
    return (x * x);
};

/* Initialize the Lambert Azimuthal Equal Area projection for ellipsoids
  ----------------------------------------------------------------------*/
long lamazellipsforint
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

/* Lambert Azimuthal Equal Area ellipsoidal forward equations -- mapping
   lat,long to x,y
  -----------------------------------------------------------------------*/
long lamazellipsfor
(
    double lon,			/* I: Longitude (radians) */
    double lat,			/* I: Latitude (radians) */
    double *x,			/* O: X projection coordinate (meters) */
    double *y			/* O: Y projection coordinate (meters) */
)
{
    double delta_lon;		/* Delta longitude
                                   (longitude - center long) */
    double sin_delta_lon;	/* Sine of the delta longitude */
    double cos_delta_lon;	/* Cosine of the delta longitude */
    double sin_lat;		/* Sine of the given latitude */
    double cos_lat;		/* Cosine of the given latitude	*/
    double q;                   /* q variable */
    double beta;                /* beta variable (not B) */
    double sin_beta;            /* sine of beta variable */
    double cos_beta;            /* cosine of beta variable */
    double m;                   /* m variable */
    double B;                   /* B variable (not beta) */

    /* Forward equations
      -----------------*/
    gctp_sincos(lat, &sin_lat, &cos_lat);
/*    delta_lon = adjust_lon(lon - lon_center); */
    delta_lon = lon - lon_center;
    gctp_sincos(delta_lon, &sin_delta_lon, &cos_delta_lon);

    m = cos_lat / sqrt(1.0 - e2*my_square(sin_lat));
    q = (1.0-e2) * ((sin_lat / (1.0-e2*my_square(sin_lat))) -
        (1.0/(2.0*e)) * log ((1.0-e*sin_lat) / (1.0+e*sin_lat)));
    beta = asin(q/qp);
    gctp_sincos (beta, &sin_beta, &cos_beta);
    B = Rq * sqrt(2.0 / (1.0 + sin_beta1*sin_beta +
        cos_beta1*cos_beta*cos_delta_lon));

    /* Calculate the x and y values adding in the false easting and northing
      ---------------------------------------------------------------------*/ 
    *x = B*D*cos_beta*sin_delta_lon + false_easting;
    *y = (B/D) * (cos_beta1*sin_beta - sin_beta1*cos_beta*cos_delta_lon) +
         false_northing;

    return(GCTP_OK);
}
