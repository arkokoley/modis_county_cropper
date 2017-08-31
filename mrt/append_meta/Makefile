#-----------------------------------------------------------------------
# Makefile for MODIS Reprojection Tool (MRT)
#
# Note: Makefile.lnx can be used to compile on the MacOS X architecture.
#-----------------------------------------------------------------------

CC = gcc
CFLAGS = -O3 -Wall -W -Wno-switch
LDFLAGS = $(HDFLIB) -s
CP = cp
MV = mv
RM = rm -f

INCS = -I../include

#--------------------------
# Define the include files:
#--------------------------
INC = append_meta.h

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
EXE = append_meta

#-----------------------------
# Targets for each executable:
#-----------------------------
all: $(EXE)

append_meta: append_meta.o $(INC)
	$(CC) -o append_meta append_meta.o $(LDFLAGS)

install:
	$(MV) append_meta ../bin

clean:
	$(RM) *.o *~ append_meta

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

