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
