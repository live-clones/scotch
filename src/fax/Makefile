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
		 		$(CC) $(CFLAGS) $(<) -o $(@) $(LDFLAGS) -L. -lfax -lsymbol -ldof -lorder -lgraph -lscotch -lscotcherrcom -lcommon

##
##  Project rules.
##

include ../Makefile.inc

.PHONY				:	all

all				:	libfax$(LIB)				\
					main_fax_graph$(EXE)			\
					main_fax_mesh$(EXE)


install				:	all
					-$(CP) fax.h lib*$(LIB) main_fax_graph$(EXE) main_fax_mesh$(EXE) ../../bin

clean				:
					-$(RM) *~ *$(OBJ) lib*$(LIB) main_fax_graph$(EXE) main_fax_mesh$(EXE)

realclean			:	clean

##
##  Todo list.
##

symbol_compact$(OBJ)		:	symbol_compact.c			\
					../../bin/common.h			\
					../../bin/graph.h			\
					../../bin/symbol.h			\
					../../bin/order.h			\
					fax.h

symbol_costi$(OBJ)		:	symbol_costi.c				\
					../../bin/symbol.h			\
					symbol_costi.h				\
					../../bin/common.h			\
					../../bin/dof.h

symbol_fax$(OBJ)		:	symbol_fax.c				\
					../../bin/common.h			\
					../../bin/graph.h			\
					../../bin/symbol.h			\
					../../bin/order.h			\
					fax.h

symbol_fax_graph$(OBJ)		:	symbol_fax_graph.c			\
					symbol_fax.c				\
					../../bin/common.h			\
					../../bin/graph.h			\
					../../bin/symbol.h			\
					../../bin/order.h			\
					fax.h

symbol_fax_mesh$(OBJ)		:	symbol_fax_mesh.c			\
					../../bin/common.h			\
					../../bin/mesh.h			\
					../../bin/symbol.h			\
					../../bin/order.h			\
					fax.h					\
					symbol_fax_mesh.h

libfax$(LIB)			:	symbol_compact$(OBJ)			\
					symbol_costi$(OBJ)			\
					symbol_fax$(OBJ)			\
					symbol_fax_graph$(OBJ)			\
					symbol_fax_mesh$(OBJ)
					$(AR) $(ARFLAGS) $(@) $(^)
					-$(RANLIB) $(@)

main_fax_graph$(EXE)		:	main_fax_graph.c			\
					fax.h					\
					libfax$(LIB)				\
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

main_fax_mesh$(EXE)		:	main_fax_mesh.c				\
					fax.h					\
					libfax$(LIB)				\
					../../bin/common.h			\
					../../bin/libcommon$(LIB)		\
					../../bin/mesh.h			\
					../../bin/dof.h				\
					../../bin/libdof$(LIB)			\
					../../bin/symbol.h			\
					../../bin/libsymbol$(LIB)		\
					../../bin/order.h			\
					../../bin/liborder$(LIB)		\
					../../bin/libscotch$(LIB)
