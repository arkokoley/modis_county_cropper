#-----------------------------------------------------------------------
# Makefile for MODIS Reprojection Tool (MRT)
#
# Note: Makefile.lnx can be used to compile on the MacOS X architecture.
#-----------------------------------------------------------------------

CC = gcc
CFLAGS = -O3 -Wall -W -Wno-switch
LDFLAGS = $(HDFLIB) -s
MV = mv
CP = cp
RM = rm -f

INCS = -I../include

#--------------------------
# Define the include files:
#--------------------------
INC = update_tile_meta.h

#-----------------------------------------
# Define the source code and object files:
#-----------------------------------------
SRC	= \
OBJ = $(SRC:.c=.o)

#-----------------------------
# Define the object libraries:
#-----------------------------
HDFLIB = ../lib/libmfhdf.a ../lib/libdf.a ../lib/libjpeg.a ../lib/libz.a ../lib/libsz.a

#-----------------------
# Define the executable:
#-----------------------
EXE = update_tile_meta

#-----------------------------
# Targets for each executable:
#-----------------------------
all: $(EXE)

update_tile_meta: update_tile_meta.o $(INC)
	$(CC) -o update_tile_meta update_tile_meta.o $(LDFLAGS)

install:
	$(MV) update_tile_meta ../bin

clean:
	$(RM) *.o *~ update_tile_meta

copy-makefile:
	@if [ ! -f Makefile.orig ]; then \
	  $(CP) Makefile Makefile.orig; fi
	$(CP) Makefile.$(CPMAKEFILEEXT) Makefile

#--------------------------------------
# Rules for compiling the object files:
#--------------------------------------

"$(OBJ)": $(INC)

.c.o:
	$(CC) $(CFLAGS) $(INCS) -c $<

