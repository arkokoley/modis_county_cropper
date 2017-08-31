#-----------------------------------------------------------------------
# Makefile for the GeoLib library.
#
# Note: Makefile.lnx can be used to compile on the MacOS X architecture.
#-----------------------------------------------------------------------

#--------------------------------------------------------
# Define the archive, compiler options, and include files 
#--------------------------------------------------------

ARCHIVE = libgeolib.a

CC = gcc
AR = ar rcsv
CP = cp
RM = rm -f

INC = -I../include -I../gctp -I../shared_src

CFLAGS = -O3 -Wall -W -Wno-switch

INCS    = \
	loclprot.h \
	../include/datum.h \
	../include/desc.h \
	../include/geompak.h \
	../include/las.h \
	../include/ltable.h \
        ../gctp/cproj.h \
	../gctp/proj.h \
	../include/ptio.h \
	../include/tae.h \
	../include/typlim.h \
	../include/worgen.h \
	../shared_src/shared_resample.h

#------------------------------------------
# Define the source files and their objects
#------------------------------------------
SRC	= \
        checkdms.c crd2geo.c c_ckptid.c c_ckunit.c c_decdeg.c c_degdms.c \
        c_dtmnum.c c_dtmstr.c c_eval.c c_invert.c c_mapedg.c c_prostr.c \
        c_trans.c fcorner.c geo2crd.c getdatum.c getspher.c ndcon.c \
        pckdatum.c proj_err.c proj_prn.c proj_rpt.c pspcszon.c \
        psupport.c spcszone.c spheroid.c transfrm.c

OBJ     = $(SRC:.c=.o)

#------------------------------------------------------------------------
# Do not use $(OBJS) when building the archive.  When using the CVS build
# scripts, only those supports that have changed will be transferred and
# compiled on all systems.  Thus, $(OBJS) will give an error for those
# object files that do not exist while, *.o will not.
#------------------------------------------------------------------------
all: $(ARCHIVE)

$(ARCHIVE):     $(OBJ) $(INCS)
	$(AR) $(ARCHIVE) $(OBJ)

install:
#	mv $(ARCHIVE) ../lib

clean:
	$(RM) *~ $(OBJ) $(ARCHIVE)

copy-makefile:
	@if [ ! -f Makefile.orig ]; then \
	  $(CP) Makefile Makefile.orig; fi
	$(CP) Makefile.$(CPMAKEFILEEXT) Makefile

#-------------------------------------
# Rules for compiling the object files
#-------------------------------------
"$(OBJ)": $(SRC)

.c.o:
	$(CC) $(CFLAGS) $(INC) -c $<

