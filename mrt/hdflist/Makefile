#-----------------------------------------------------------------------
# Makefile for MODIS Reprojection Tool (MRT)
#
# Note: Makefile.lnx can be used to compile on the MacOS X architecture.
#-----------------------------------------------------------------------

CC = gcc
CFLAGS = -O3 -Wall -W -Wno-switch
LDFLAGS = $(HDFLIB) $(GEOLIB) -lm -s
MV = mv
CP = cp
RM = rm -f

INCS = -I../include -I../gctp -I../geolib

#--------------------------
# Define the include files:
#--------------------------
INC =

#-----------------------------------------
# Define the source code and object files:
#-----------------------------------------
SRC	= \
OBJ = $(SRC:.c=.o)

#-----------------------------
# Define the object libraries:
#-----------------------------
GEOLIB = ../gctp/libgctp.a
HDFLIB = ../lib/libhdfeos.a ../lib/libmfhdf.a ../lib/libdf.a ../lib/libjpeg.a ../lib/libz.a ../lib/libsz.a

#-----------------------
# Define the executable:
#-----------------------
EXE = hdflist

#-----------------------------
# Targets for each executable:
#-----------------------------
all: $(EXE)

hdflist: hdflist.o $(INC)
	$(CC) -o hdflist hdflist.o $(LDFLAGS)

install:
	$(MV) hdflist ../bin

clean:
	$(RM) *.o *~ hdflist

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

