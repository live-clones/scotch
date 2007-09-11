## Copyright 2004,2007 ENSEIRB, INRIA & CNRS
##
## This file is part of the Scotch software package for static mapping,
## graph partitioning and sparse matrix ordering.
##
## This software is governed by the CeCILL-C license under French law
## and abiding by the rules of distribution of free software. You can
## use, modify and/or redistribute the software under the terms of the
## CeCILL-C license as circulated by CEA, CNRS and INRIA at the following
## URL: "http://www.cecill.info".
## 
## As a counterpart to the access to the source code and rights to copy,
## modify and redistribute granted by the license, users are provided
## only with a limited warranty and the software's author, the holder of
## the economic rights, and the successive licensors have only limited
## liability.
## 
## In this respect, the user's attention is drawn to the risks associated
## with loading, using, modifying and/or developing or reproducing the
## software by the user in light of its specific status of free software,
## that may mean that it is complicated to manipulate, and that also
## therefore means that it is reserved for developers and experienced
## professionals having in-depth computer knowledge. Users are therefore
## encouraged to load and test the software's suitability as regards
## their requirements in conditions enabling the security of their
## systems and/or data to be ensured and, more generally, to use and
## operate it in the same conditions as regards security.
## 
## The fact that you are presently reading this means that you have had
## knowledge of the CeCILL-C license and that you accept its terms.
##

##
##  General inference rules.
##

%$(OBJ)	:	%.c
				$(CC) $(CFLAGS) -c $(<) -o $(@)

%$(EXE)	:	%.c
		 		$(CC) $(CFLAGS) -I../libscotch -DSCOTCH_VERSION=\"$(VERSION)\" $(<) -o $(@) $(LDFLAGS) -l$(SCOTCHLIB) -lscotcherrcom -lcommon

%$(EXE)	:	%.$(OBJ)
		 		$(CC) $(CFLAGS) -DSCOTCH_VERSION=\"$(VERSION)\" $(<) -o $(@) $(LDFLAGS) -lscotch -lscotcherrcom -lcommon

##
##  Project rules.
##

include ../Makefile.inc

.PHONY				:	ptscotch	scotch	ptinstall	install	clean	realclean

scotch				:	clean
					$(MAKE)	SCOTCHLIB=scotch		\
					acpl$(EXE)				\
					amk_ccc$(EXE)				\
					amk_fft2$(EXE)				\
					amk_grf$(EXE)				\
					amk_hy$(EXE)				\
					amk_m2$(EXE)				\
					amk_p2$(EXE)				\
					atst$(EXE)				\
					gbase$(EXE)				\
					gcv$(EXE)				\
					gmap$(EXE)				\
					gmk_hy$(EXE)				\
					gmk_m2$(EXE)				\
					gmk_m3$(EXE)				\
					gmk_msh$(EXE)				\
					gmk_ub2$(EXE)				\
					gmtst$(EXE)				\
					gord$(EXE)				\
					gotst$(EXE)				\
					gout$(EXE)				\
					gtst$(EXE)				\
					mcv$(EXE)				\
					mmk_m2$(EXE)				\
					mmk_m3$(EXE)				\
					mord$(EXE)				\
					mtst$(EXE)

ptscotch			:	clean
					$(MAKE) SCOTCHLIB=ptscotch CC=$(CCP)	\
					dgord$(EXE)				\
					dgscat$(EXE)				\
					dgtst$(EXE)

install				:
					-$(CP) acpl$(EXE) amk_ccc$(EXE) amk_fft2$(EXE) amk_grf$(EXE) amk_hy$(EXE) amk_m2$(EXE) amk_p2$(EXE) atst$(EXE) gbase$(EXE) gcv$(EXE) gmap$(EXE) gmk_hy$(EXE) gmk_m2$(EXE) gmk_m3$(EXE) gmk_msh$(EXE) gmk_ub2$(EXE) gmtst$(EXE) gord$(EXE) gotst$(EXE) gout$(EXE) *gtst$(EXE) mcv$(EXE) mmk_m2$(EXE) mmk_m3$(EXE) mord$(EXE) mtst$(EXE) ../../bin

ptinstall			:
					-$(CP) dgord$(EXE) dgscat$(EXE) dgtst$(EXE) ../../bin

clean				:
					-$(RM) *~ *$(OBJ) acpl$(EXE) amk_ccc$(EXE) amk_fft2$(EXE) amk_grf$(EXE) amk_hy$(EXE) amk_m2$(EXE) amk_p2$(EXE) atst$(EXE) dgscat$(EXE) gbase$(EXE) gcv$(EXE) gmap$(EXE) gmk_hy$(EXE) gmk_m2$(EXE) gmk_m3$(EXE) gmk_msh$(EXE) gmk_ub2$(EXE) gmtst$(EXE) *gord$(EXE) gotst$(EXE) gout$(EXE) *gtst$(EXE) mcv$(EXE) mmk_m2$(EXE) mmk_m3$(EXE) mord$(EXE) mtst$(EXE)

realclean			:	clean

##
##  Todo list.
##

acpl$(EXE)			:	acpl.c					\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					acpl.h

amk_ccc$(EXE)			:	amk_ccc.c				\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					amk_ccc.h

amk_fft2$(EXE)			:	amk_fft2.c				\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					amk_fft2.h

amk_grf$(EXE)			:	amk_grf.c				\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					amk_grf.h

amk_hy$(EXE)			:	amk_hy.c				\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					amk_hy.h

amk_m2$(EXE)			:	amk_m2.c				\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					amk_m2.h

amk_p2$(EXE)			:	amk_p2.c				\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					amk_p2.h

atst$(EXE)			:	atst.c					\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					atst.h

dgord$(EXE)			:	dgord.c					\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/ptscotch.h			\
					../../bin/libptscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					dgord.h

dgscat$(EXE)			:	dgscat.c				\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/ptscotch.h			\
					../../bin/libptscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					dgscat.h

dgtst$(EXE)			:	dgtst.c					\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/ptscotch.h			\
					../../bin/libptscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					dgtst.h

gbase$(EXE)			:	gbase.c					\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					gbase.h

gcv$(EXE)			:	gcv.c					\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					gcv.h

gmap$(EXE)			:	gmap.c					\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					gmap.h

gmk_hy$(EXE)			:	gmk_hy.c				\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					gmk_hy.h

gmk_m2$(EXE)			:	gmk_m2.c				\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					gmk_m2.h

gmk_m3$(EXE)			:	gmk_m3.c				\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					gmk_m3.h

gmk_msh$(EXE)			:	gmk_msh.c				\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					gmk_msh.h

gmk_ub2$(EXE)			:	gmk_ub2.c				\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					gmk_ub2.h

gmtst$(EXE)			:	gmtst.c					\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					gmtst.h

gord$(EXE)			:	gord.c					\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					gord.h

gotst$(EXE)			:	gotst.c					\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					gotst.h

gout$(EXE)			:	gout_c.c				\
					gout_o.c				\
					../../bin/libcommon$(LIB)		\
					../../bin/libscotch$(LIB)
					$(CC) $(CFLAGS) -I../libscotch -DSCOTCH_VERSION=5.0 gout_c.c gout_o.c -o $(@) $(LDFLAGS)  -lscotch -lscotcherrcom -lcommon

gtst$(EXE)			:	gtst.c					\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					gtst.h

mcv$(EXE)			:	mcv.c					\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					mcv.h

mmk_m2$(EXE)			:	mmk_m2.c				\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					mmk_m2.h

mmk_m3$(EXE)			:	mmk_m3.c				\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					mmk_m3.h

mord$(EXE)			:	mord.c					\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					mord.h

mtst$(EXE)			:	mtst.c					\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)		\
					../../bin/libscotcherrcom$(LIB)		\
					mtst.h
