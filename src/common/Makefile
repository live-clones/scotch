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

all				:	libcommon$(LIB)

install				:	all
					-$(CP) common.h libcommon$(LIB) ../../bin

clean				:
					-$(RM) *~ *$(OBJ) lib*$(LIB)

realclean			:	clean

##
##  Todo list.
##

common$(OBJ)			:	common.c				\
					common.h

common_error$(OBJ)		:	common_error.c				\
					common.h

common_integer$(OBJ)		:	common_integer.c			\
					common.h				\
					common_integer.h

common_memory$(OBJ)		:	common_memory.c				\
					common.h

libcommon$(LIB)			:	common$(OBJ)				\
					common_error$(OBJ)			\
					common_integer$(OBJ)			\
					common_memory$(OBJ)
					$(AR) $(ARFLAGS) $(@) $(^)
					-$(RANLIB) $(@)
