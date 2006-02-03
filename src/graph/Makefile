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

all				:	libgraph$(LIB)

install				:	all
					-$(CP) graph.h mesh.h libgraph$(LIB) ../../bin

clean				:
					-$(RM) *~ *$(OBJ) lib*$(LIB)

realclean			:	clean

##
##  Todo list.
##

graph_build$(OBJ)		:	graph_build.c				\
					../../bin/common.h			\
					../../bin/scotch.h			\
					graph.h

graph_graph$(OBJ)		:	graph_graph.c				\
					../../bin/common.h			\
					../../bin/scotch.h			\
					graph.h

graph_grid$(OBJ)		:	graph_grid.c				\
					../../bin/common.h			\
					../../bin/scotch.h			\
					graph.h					\
					graph_grid.h

libgraph$(LIB)			:	graph_build$(OBJ)			\
					graph_graph$(OBJ)			\
					graph_grid$(OBJ)
					$(AR) $(ARFLAGS) $(@) $(^)
					-$(RANLIB) $(@)
