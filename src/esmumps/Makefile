## Copyright 2004,2007,2008 ENSEIRB, INRIA & CNRS
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

bindir		= ../../bin
includedir	= ../../include
libdir		= ../../lib

##
##  General inference rules.
##

include ../Makefile.inc

%$(OBJ)	:	%.c
				$(CC) $(CFLAGS) -I$(includedir) -c $(<) -o $(@)

%$(EXE)	:	%.c
		 		$(CC) $(CFLAGS) -I$(includedir) $(<) -o $(@) -L$(libdir) $(LDFLAGS) -L. -lesmumps -lfax -lsymbol -ldof -lorder -lgraph -lscotch -lorder -lcommon

##
##  Project rules.
##

.PHONY				:	all

all				:	libesmumps$(LIB)			\
					main_esmumps$(EXE)

install				:	all
					-$(CP) esmumps.h $(includedir)
					-$(CP) lib*$(LIB) $(libdir)
					-$(CP) main_esmumps$(EXE) $(bindir)

clean				:
					-$(RM) *~ *$(OBJ) lib*$(LIB) main_esmumps$(EXE)

realclean			:	clean

##
##  Todo list.
##

esmumps$(OBJ)			:	esmumps.c				\
					../libscotch/common.h			\
					$(includedir)/graph.h			\
					$(includedir)/symbol.h			\
					$(includedir)/order.h			\
					$(includedir)/fax.h			\
					esmumps.h

esmumps_f$(OBJ)			:	esmumps_f.c				\
					../libscotch/common.h			\
					esmumps.h

esmumps_strats$(OBJ)		:	esmumps_strats.c			\
					../libscotch/common.h			\
					esmumps.h

libesmumps$(LIB)		:	esmumps$(OBJ)				\
					esmumps_f$(OBJ)				\
					esmumps_strats$(OBJ)
					$(AR) $(ARFLAGS) $(@) $(^)
					-$(RANLIB) $(@)

main_esmumps$(EXE)		:	main_esmumps.c				\
					esmumps.h				\
					libesmumps$(LIB)			\
					../libscotch/common.h			\
					$(libdir)/libcommon$(LIB)		\
					$(includedir)/graph.h			\
					$(libdir)/libgraph$(LIB)		\
					$(includedir)/dof.h			\
					$(libdir)/libdof$(LIB)			\
					$(includedir)/symbol.h			\
					$(libdir)/libsymbol$(LIB)		\
					$(includedir)/order.h			\
					$(libdir)/liborder$(LIB)		\
					$(libdir)/libscotch$(LIB)
