#ifdef unix
#include "cproj.h"
/*  Fortran bridge routine for the UNIX */

void gctp_
(
    double *incoor,
    long *insys,
    long *inzone,
    double *inparm,
    long *inunit,
    long *inspheroid,
    long *ipr,        /* printout flag for error messages. 0=yes, 1=no*/
    char *efile,
    long *jpr,        /* printout flag for projection parameters 0=yes, 1=no*/
    char *pfile,
    double *outcoor,
    long *outsys,
    long *outzone,
    double *outparm,
    long *outunit,
    long *outspheroid,
    int fn27,
    int fn83,
    long *iflg
)

{
gctp(incoor,insys,inzone,inparm,inunit,inspheroid,ipr,efile,jpr,pfile,outcoor,
     outsys,outzone,outparm,outunit,outspheroid,(char *)fn27,(char *)fn83,iflg);

return;
}
#endif
