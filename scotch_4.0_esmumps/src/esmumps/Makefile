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
		 		$(CC) $(CFLAGS) $(<) -o $(@) $(LDFLAGS) -L. -lesmumps -lfax -lsymbol -ldof -lorder -lgraph -lscotch -lscotcherrcom -lcommon

##
##  Project rules.
##

include ../Makefile.inc

.PHONY				:	all

all				:	libesmumps$(LIB)			\
					main_esmumps$(EXE)

install				:	all
					-$(CP) esmumps.h lib*$(LIB) main_esmumps$(EXE) ../../bin

clean				:
					-$(RM) *~ *$(OBJ) lib*$(LIB) main_esmumps$(EXE)

realclean			:	clean

##
##  Todo list.
##

esmumps$(OBJ)			:	esmumps.c				\
					../../bin/common.h			\
					../../bin/graph.h			\
					../../bin/symbol.h			\
					../../bin/order.h			\
					../../bin/fax.h				\
					esmumps.h

esmumps_f$(OBJ)			:	esmumps_f.c				\
					../../bin/common.h			\
					esmumps.h

esmumps_strats$(OBJ)		:	esmumps_strats.c			\
					../../bin/common.h			\
					esmumps.h

libesmumps$(LIB)		:	esmumps$(OBJ)				\
					esmumps_f$(OBJ)				\
					esmumps_strats$(OBJ)
					$(AR) $(ARFLAGS) $(@) $(^)
					-$(RANLIB) $(@)

main_esmumps$(EXE)		:	main_esmumps.c				\
					esmumps.h				\
					libesmumps$(LIB)			\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/graph.h			\
					../../bin/libgraph$(LIB)		\
					../../bin/dof.h				\
					../../bin/libdof$(LIB)			\
					../../bin/symbol.h			\
					../../bin/libsymbol$(LIB)		\
					../../bin/order.h			\
					../../bin/liborder$(LIB)		\
					../../bin/libscotch$(LIB)
