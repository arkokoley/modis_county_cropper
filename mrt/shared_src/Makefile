#-----------------------------------------------------------------------
# Makefile for MODIS Reprojection Tool (MRT)
#
# Note: Makefile.lnx can be used to compile on the MacOS X architecture.
#-----------------------------------------------------------------------

ARCHIVE = libmrt.a
CC = gcc
CFLAGS = -O3 -Wall -W -Wno-switch
CP = cp
RM = rm -f
AR = ar rcsv

INCS = -I../include -I../gctp -I../geolib

#--------------------------
# Define the include files:
#--------------------------
INC = mrt_error.h loc_prot.h loc_prot_mosaic.h shared_mosaic.h shared_resample.h

#-----------------------------------------
# Define the source code and object files:
#-----------------------------------------
SRC	= \
	abortex.c  fileoc.c  init_md.c  proc_arg.c  space2us.c     \
	addmeta.c  gctp_call.c  initfile.c  projutil.c  struplow.c \
	addmeta_mosaic.c  hdf_init.c  logh.c  rb_io.c  tif_init.c  \
	mrt_error.c  hdf_init_mosaic.c  modproj.c  rb_oc.c  tif_io.c   \
	filebuf.c  hdf_io.c  msgh.c  rdhdfhdr.c  tif_oc.c          \
	filedesc.c  hdf_oc.c  print_md.c  read_hdr.c  writ_hdr.c   \
	fileio.c  hdf_oc_mosaic.c  print_proj.c  read_prm.c usage.c

OBJ = $(SRC:.c=.o)

#-------------------------------
# Rules for building the archive
#-------------------------------
$(ARCHIVE): $(OBJ) $(INC)
	$(AR) $(ARCHIVE) $(OBJ)

all: $(ARCHIVE)

install:
#	$(MV) $(ARCHIVE) ../lib

clean:
	$(RM) *~ $(OBJ) $(ARCHIVE)

copy-makefile:
	@if [ ! -f Makefile.orig ]; then \
	  $(CP) Makefile Makefile.orig; fi
	$(CP) Makefile.$(CPMAKEFILEEXT) Makefile


#-------------------------------------
# Rules for compiling the object files
#-------------------------------------

$(OBJ):

.c.o:
	$(CC) $(CFLAGS) $(INCS) -c $<

