#-----------------------------------------------------------------------
# Makefile for MODIS Reprojection Tool (MRT)
#
# Note: Makefile.lnx can be used to compile on the MacOS X architecture.
#-----------------------------------------------------------------------

CC = gcc
CFLAGS = -O3 -Wall -W -Wno-switch
LDFLAGS = $(MRTLIB) $(HDFLIB) $(GEOLIB) $(TIFFLIB) -lm -s
CP = cp
MV = mv
RM = rm -f
INCS = -I../include -I../gctp -I../geolib -I../shared_src

#--------------------------
# Define the include files:
#--------------------------
INC = resample.h

#-----------------------------------------
# Define the source code and object files:
#-----------------------------------------
SRC	= \
	bi_res.c  calc_isin_shift.c  cc_res.c  hdf2hdr.c  nn_res.c \
	no_res.c  resample_image.c

OBJ = $(SRC:.c=.o)

#-----------------------------
# Define the object libraries:
#-----------------------------
GEOLIB = ../geolib/libgeolib.a ../gctp/libgctp.a
HDFLIB = ../lib/libhdfeos.a ../lib/libmfhdf.a ../lib/libdf.a ../lib/libjpeg.a ../lib/libz.a ../lib/libsz.a
TIFFLIB = ../lib/libgeotiff.a ../lib/libtiff.a
MRTLIB = ../shared_src/libmrt.a

#-----------------------
# Define the executable:
#-----------------------
EXE = resample

#-----------------------------
# Targets for each executable:
#-----------------------------
all: $(EXE)

resample: resample.o $(OBJ) $(INC)
	$(CC) -o resample resample.o $(OBJ) $(LDFLAGS)

install:
	$(MV) resample ../bin

clean:
	$(RM) *.o *~ resample

copy-makefile:
	@if [ ! -f Makefile.orig ]; then \
	  $(CP) Makefile Makefile.orig; fi
	$(CP) Makefile.$(CPMAKEFILEEXT) Makefile

#-------------------------------------
# Rules for compiling the object files
#-------------------------------------

"$(OBJ)": $(INC)

.c.o:
	$(CC) $(CFLAGS) $(INCS) -c $<

