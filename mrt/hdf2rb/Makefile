#-----------------------------------------------------------------------
# Makefile for MODIS Reprojection Tool (MRT)
#
# Note: Makefile.lnx can be used to compile on the MacOS X architecture.
#-----------------------------------------------------------------------

CC = gcc
CFLAGS = -O3 -Wall -W -Wno-switch
LDFLAGS = $(MRTLIB) $(HDFLIB) $(GEOLIB) $(TIFFLIB) -lm -s
MV = mv
CP = cp
RM = rm -f

INCS = -I../include -I../gctp -I../geolib -I../shared_src

#--------------------------
# Define the include files:
#--------------------------
INC = hdf2rb.h

#-----------------------------------------
# Define the source code and object files:
#-----------------------------------------
SRC	= \
	convert_image.c no_res.c

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
EXE = hdf2rb

#-----------------------------
# Targets for each executable:
#-----------------------------
all: $(EXE)

hdf2rb: hdf2rb.o $(OBJ) $(INC)
	$(CC) -o hdf2rb hdf2rb.o $(OBJ) $(LDFLAGS)

install:
	$(MV) hdf2rb ../bin

clean:
	$(RM) *.o *~ hdf2rb

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

