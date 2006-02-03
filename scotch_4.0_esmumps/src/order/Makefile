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
		 		$(CC) $(CFLAGS) $(<) -o $(@) $(LDFLAGS)

##
##  Project rules.
##

include ../Makefile.inc

.PHONY				:	all

all				:	liborder$(LIB)

install				:	all
					-$(CP) order.h liborder$(LIB) ../../bin

clean				:
					-$(RM) *~ *$(OBJ) lib*$(LIB)

realclean			:	clean

##
##  Todo list.
##

order$(OBJ)			:	order.c					\
					order.h					\
					../../bin/common.h			\
					../../bin/graph.h

order_base$(OBJ)		:	order_base.c				\
					order.h					\
					../../bin/common.h

order_check$(OBJ)		:	order_check.c				\
					order.h					\
					../../bin/common.h

order_grid$(OBJ)		:	order_grid.c				\
					order_grid.h				\
					order.h					\
					../../bin/common.h

order_io$(OBJ)			:	order_io.c				\
					order.h					\
					../../bin/common.h

order_scotch$(OBJ)		:	order_scotch.c				\
					order.h					\
					../../bin/common.h			\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)

order_scotch_graph$(OBJ)	:	order_scotch_graph.c			\
					order.h					\
					../../bin/common.h			\
					../../bin/graph.h			\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)

order_scotch_mesh$(OBJ)		:	order_scotch_mesh.c			\
					order.h					\
					../../bin/common.h			\
					../../bin/mesh.h			\
					../../bin/scotch.h			\
					../../bin/libscotch$(LIB)

liborder$(LIB)			:	order$(OBJ)				\
					order_base$(OBJ)			\
					order_check$(OBJ)			\
					order_grid$(OBJ)			\
					order_io$(OBJ)				\
					order_scotch$(OBJ)			\
					order_scotch_graph$(OBJ)		\
					order_scotch_mesh$(OBJ)
					$(AR) $(ARFLAGS) $(@) $(^)
					-$(RANLIB) $(@)
