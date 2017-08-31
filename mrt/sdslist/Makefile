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
EXE = sdslist

#-----------------------------
# Targets for each executable:
#-----------------------------
all: $(EXE)

sdslist: sdslist.o $(INC)
	$(CC) -o sdslist sdslist.o $(LDFLAGS)

install:
	$(MV) sdslist ../bin

clean:
	$(RM) *.o *~ sdslist

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

