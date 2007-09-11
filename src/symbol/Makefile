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
		 		$(CC) $(CFLAGS) $(<) -o $(@) $(LDFLAGS) -L. -lsymbol -ldof -lcommon

##
##  Project rules.
##

include ../Makefile.inc

.PHONY				:	all

all				:	libdof$(LIB)				\
					libsymbol$(LIB)				\
					symboltops$(EXE)

install				:	all
					-$(CP) dof.h symbol.h lib*$(LIB) symboltops$(EXE) ../../bin

clean				:
					-$(RM) *~ *$(OBJ) lib*$(LIB) symboltops$(EXE)

realclean			:	clean

##
##  Todo list.
##

dof$(OBJ)			:	dof.c					\
					dof.h					\
					../../bin/common.h			\
					../../bin/graph.h

dof_io$(OBJ)			:	dof_io.c				\
					dof.h					\
					../../bin/common.h			\
					../../bin/graph.h

libdof$(LIB)			:	dof$(OBJ)				\
					dof_io$(OBJ)
					$(AR) $(ARFLAGS) $(@) $(^)
					-$(RANLIB) $(@)

symbol$(OBJ)			:	symbol.c				\
					symbol.h				\
					../../bin/common.h

symbol_base$(OBJ)		:	symbol_base.c				\
					symbol.h				\
					../../bin/common.h

symbol_check$(OBJ)		:	symbol_check.c				\
					symbol.h				\
					../../bin/common.h

symbol_cost$(OBJ)		:	symbol_cost.c				\
					dof.h					\
					symbol.h				\
					symbol_cost.h				\
					../../bin/common.h

symbol_draw$(OBJ)		:	symbol_draw.c				\
					symbol.h				\
					symbol_draw.h				\
					../../bin/common.h

symbol_io$(OBJ)			:	symbol_io.c				\
					symbol.h				\
					../../bin/common.h

symbol_keep$(OBJ)		:	symbol_keep.c				\
					symbol.h				\
					../../bin/common.h

symbol_levf$(OBJ)		:	symbol_levf.c				\
					symbol.h				\
					../../bin/common.h

symbol_nonzeros$(OBJ)		:	symbol_nonzeros.c			\
					symbol.h				\
					../../bin/common.h


symbol_tree$(OBJ)		:	symbol_tree.c				\
					symbol.h				\
					../../bin/common.h

libsymbol$(LIB)			:	symbol$(OBJ)				\
					symbol_base$(OBJ)			\
					symbol_check$(OBJ)			\
					symbol_cost$(OBJ)			\
					symbol_draw$(OBJ)			\
					symbol_io$(OBJ)				\
					symbol_keep$(OBJ)			\
					symbol_levf$(OBJ)			\
					symbol_nonzeros$(OBJ)			\
					symbol_tree$(OBJ)
					$(AR) $(ARFLAGS) $(@) $(^)
					-$(RANLIB) $(@)

symboltops$(EXE)		:	symboltops.c				\
					symbol.h				\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					libsymbol$(LIB)
