## Copyright INRIA 2004
##
## This file is part of the Scotch distribution.
##
## The Scotch distribution is libre/free software; you can
## redistribute it and/or modify it under the terms of the
## GNU Lesser General Public License as published by the
## Free Software Foundation; either version 2.1 of the
## License, or (at your option) any later version.
##
## The Scotch distribution is distributed in the hope that
## it will be useful, but WITHOUT ANY WARRANTY; without even
## the implied warranty of MERCHANTABILITY or FITNESS FOR A
## PARTICULAR PURPOSE. See the GNU Lesser General Public
## License for more details.
##
## You should have received a copy of the GNU Lesser General
## Public License along with the Scotch distribution; if not,
## write to the Free Software Foundation, Inc.,
## 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
##
## $Id$
##

##
##  General inference rules.
##

%$(OBJ)	:	%.c
				$(CC) $(CFLAGS) -c $(<) -o $(@)

%$(EXE)	:	%.c
		 		$(CC) $(CFLAGS) -I../libscotch -DSCOTCH_VERSION="\"4.0\"" $(<) -o $(@) $(LDFLAGS) -lscotch -lscotcherrcom -lcommon

%$(EXE)	:	%.$(OBJ)
		 		$(CC) $(CFLAGS) -DSCOTCH_VERSION='"4.0"' $(<) -o $(@) $(LDFLAGS) -lscotch -lscotcherrcom -lcommon

##
##  Project rules.
##

include ../Makefile.inc

.PHONY				:	all

all				:	acpl$(EXE)				\
					amk_ccc$(EXE)				\
					amk_fft2$(EXE)				\
					amk_grf$(EXE)				\
					amk_hy$(EXE)				\
					amk_m2$(EXE)				\
					amk_p2$(EXE)				\
					atst$(EXE)				\
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

install				:	all
					-$(CP) acpl$(EXE) amk_ccc$(EXE) amk_fft2$(EXE) amk_grf$(EXE) amk_hy$(EXE) amk_m2$(EXE) amk_p2$(EXE) atst$(EXE) gcv$(EXE) gmap$(EXE) gmk_hy$(EXE) gmk_m2$(EXE) gmk_m3$(EXE) gmk_msh$(EXE) gmk_ub2$(EXE) gmtst$(EXE) gord$(EXE) gotst$(EXE) gout$(EXE) gtst$(EXE) mcv$(EXE) mmk_m2$(EXE) mmk_m3$(EXE) mord$(EXE) mtst$(EXE) ../../bin

clean				:
					-$(RM) *~ *$(OBJ) acpl$(EXE) amk_ccc$(EXE) amk_fft2$(EXE) amk_grf$(EXE) amk_hy$(EXE) amk_m2$(EXE) amk_p2$(EXE) atst$(EXE) gcv$(EXE) gmap$(EXE) gmk_hy$(EXE) gmk_m2$(EXE) gmk_m3$(EXE) gmk_msh$(EXE) gmk_ub2$(EXE) gmtst$(EXE) gord$(EXE) gotst$(EXE) gout$(EXE) gtst$(EXE) mcv$(EXE) mmk_m2$(EXE) mmk_m3$(EXE) mord$(EXE) mtst$(EXE)

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
					$(CC) $(CFLAGS) -I../libscotch -DSCOTCH_VERSION=4.0 gout_c.c gout_o.c -o $(@) $(LDFLAGS)  -lscotch -lscotcherrcom -lcommon

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
