#-----------------------------------------------------------------------
# Makefile for MODIS Reprojection Tool (MRT)
#
# Note: Makefile.lnx can be used to compile on the MacOS X architecture.
#-----------------------------------------------------------------------

CC = gcc
CFLAGS = -O3 -Wall -W -Wno-switch
LDFLAGS = $(HDFLIB) -s
MV = mv
RM = rm -f
CP = cp

INCS = -I../include

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
HDFLIB = ../lib/libmfhdf.a ../lib/libdf.a ../lib/libjpeg.a ../lib/libz.a ../lib/libsz.a

#-----------------------
# Define the executable:
#-----------------------
EXE = dumpmeta

#-----------------------------
# Targets for each executable:
#-----------------------------
all: $(EXE)

dumpmeta: dumpmeta.o $(INC)
	$(CC) -o dumpmeta dumpmeta.o $(LDFLAGS)

install:
	$(MV) dumpmeta ../bin

clean:
	$(RM) *.o *~ dumpmeta

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

