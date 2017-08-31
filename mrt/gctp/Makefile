#-----------------------------------------------------------------------
# Makefile for GCTP library.
#
# Note: Makefile.lnx can be used to compile on the MacOS X architecture.
#-----------------------------------------------------------------------

ARCHIVE = libgctp.a
INCS    = cproj.h proj.h
CC      = gcc
CFLAGS  = -O3 -Wall -W -Wno-switch
AR      = ar rcsv
CP      = cp
RM      = rm -f
MV      = mv

OBJ     = $(SRC:.c=.o)

#-----------------------------------------------------------------------------
# Rules for compiling the object files.  If you have multiple groups of object
# files, add one dependency of the form "$(OBJS): $(INC)" for each group of
# object files.
#-----------------------------------------------------------------------------
.c.o:
	$(CC) $(CFLAGS) -c $<

SRC =   alberfor.c alberinv.c alconfor.c alconinv.c azimfor.c aziminv.c \
        eqconfor.c eqconinv.c equifor.c equiinv.c gnomfor.c gnominv.c \
        goodfor.c goodinv.c gvnspfor.c gvnspinv.c hamfor.c haminv.c \
        imolwfor.c imolwinv.c lamazfor.c lamazinv.c \
	lamccfor.c lamccinv.c merfor.c merinv.c millfor.c \
	millinv.c molwfor.c molwinv.c obleqfor.c obleqinv.c omerfor.c \
	omerinv.c orthfor.c orthinv.c polyfor.c polyinv.c psfor.c psinv.c \
	robfor.c robinv.c sinfor.c sininv.c somfor.c sominv.c sterfor.c \
	sterinv.c stplnfor.c stplninv.c tmfor.c tminv.c utmfor.c utminv.c \
        vandgfor.c vandginv.c wivfor.c wivinv.c wviifor.c wviiinv.c \
        isinfor.c isininv.c for_init.c inv_init.c cproj.c report.c \
        paksz.c sphdz.c untfz.c gctp.c

all: $(ARCHIVE) spload nad27sp.lut nad83sp.lut

$(ARCHIVE):     $(OBJ) $(INCS)
	$(AR) $(ARCHIVE) $(OBJ)

# only build if the source file is present to allow for the check-in process
spload:
	if [ -f spload.c ]; then \
            $(CC) $(CFLAGS) spload.c -o spload; \
        fi

# only run if the spload is present to allow for the check-in process
nad27sp.lut: spload
	if [ -f spload ]; then \
            ./spload; fi

# only run if the spload is present to allow for the check-in process
nad83sp.lut: spload
	@if [ -f spload ]; then \
            ./spload; fi

# need to copy the nad27 and nad83 binary tables to the MRT_DATA_DIR
install:
	$(CP) nad27sp.lut ../data/nad27sp
	$(CP) nad83sp.lut ../data/nad83sp

clean:
	$(RM) $(OBJ) *~ spload nad27sp.lut nad83sp.lut $(ARCHIVE)

copy-makefile:
	@if [ ! -f Makefile.orig ]; then \
	  $(CP) Makefile Makefile.orig; fi
	$(CP) Makefile.$(CPMAKEFILEEXT) Makefile


