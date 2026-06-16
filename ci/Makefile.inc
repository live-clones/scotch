EXE		=
LIB		= .a
OBJ		= .o

MAKE		= make
AR		= ar
ARFLAGS		= -ruv
CAT		= cat
CCS		= gcc
CCP		= mpicc
CCD		= mpicc
FCS		= gfortran
CFLAGS		= -Wall -fdiagnostics-show-option -Drestrict=__restrict -DSCOTCH_GITLAB_SEPARATOR -DCOMMON_FILE_COMPRESS_GZ -DCOMMON_FILE_COMPRESS_BZ2 -DSCOTCH_RENAME ${COVERAGE}
LDFLAGS	= -lz -lbz2 -llzma -lm -lrt -pthread ${COVERAGE}
CP		= cp
FLEX		= flex
LN		= ln
MKDIR		= mkdir
MV		= mv
RANLIB		= ranlib
BISON		= bison
