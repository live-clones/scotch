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

.PHONY				:	all default long required

default				:	all

required			:	Makefile.inc

Makefile.inc			:
					@echo "#####################################################################"
			        	@echo "BEFORE COMPILING Scotch, YOU SHOULD HAVE AN APPROPRIATE Makefile.inc"
					@echo "FILE IN THIS DIRECTORY. PLEASE LOOK INTO DIRECTORY ./Make.inc FOR AN"
	        			@echo "EXISTING Makefile.inc FILE THAT FITS YOUR NEED, OR USE THEM AS MODELS"
		        		@echo "IN CASE YOU NEED TO BUILD A NEW ONE FOR YOUR PARTICULAR PLATFORM."
			        	@echo "#####################################################################"
					@exit 1

include Makefile.inc

all				:	required
					(cd common ;    $(MAKE) all && $(MAKE) install)
					(cd libscotch ; $(MAKE) all && $(MAKE) install)
					(cd scotch ;    $(MAKE) all && $(MAKE) install)

long				:	required
					(cd common ;    $(MAKE) -DINT=long all && $(MAKE) install)
					(cd libscotch ; $(MAKE) -DINT=long all && $(MAKE) install)
					(cd scotch ;    $(MAKE) -DINT=long all && $(MAKE) install)

clean				:	required
					(cd common ;    $(MAKE) clean)
					(cd libscotch ; $(MAKE) clean)
					(cd scotch ;    $(MAKE) clean)

realclean			:	required
					(cd common ;    $(MAKE) realclean)
					(cd libscotch ; $(MAKE) realclean)
					(cd scotch ;    $(MAKE) realclean)
					-$(RM) ../bin/*
