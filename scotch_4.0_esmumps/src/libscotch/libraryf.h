** Copyright INRIA 2004
**
** This file is part of the Scotch distribution.
**
** The Scotch distribution is libre/free software; you can
** redistribute it and/or modify it under the terms of the
** GNU Lesser General Public License as published by the
** Free Software Foundation; either version 2.1 of the
** License, or (at your option) any later version.
**
** The Scotch distribution is distributed in the hope that
** it will be useful, but WITHOUT ANY WARRANTY; without even
** the implied warranty of MERCHANTABILITY or FITNESS FOR A
** PARTICULAR PURPOSE. See the GNU Lesser General Public
** License for more details.
**
** You should have received a copy of the GNU Lesser General
** Public License along with the Scotch distribution; if not,
** write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
**
** $Id$
**
************************************************************
**                                                        **
**   NAME       : libraryf.h                              **
**                                                        **
**   AUTHOR     : Francois PELLEGRINI                     **
**                                                        **
**   FUNCTION   : FORTRAN declaration file for the        **
**                LibScotch static mapping and sparse     **
**                matrix block ordering sequential        **
**                library.                                **
**                                                        **
**   DATES      : # Version 3.4  : from : 04 feb 2000     **
**                                 to     22 oct 2001     **
**                # Version 4.0  : from : 16 jan 2004     **
**                                 to     16 jan 2004     **
**                                                        **
************************************************************

** Size definitions for the SCOTCH opaque
** structures. These structures must be
** allocated as arrays of DOUBLEPRECISION
** values for proper padding. The dummy
** sizes are computed at compile-time by
** program "dummysizes".

        INTEGER SCOTCH_ARCHDIM
        INTEGER SCOTCH_GEOMDIM
        INTEGER SCOTCH_GRAPHDIM
        INTEGER SCOTCH_MAPDIM
        INTEGER SCOTCH_MESHDIM
        INTEGER SCOTCH_ORDERDIM
        INTEGER SCOTCH_STRATDIM
        PARAMETER (SCOTCH_ARCHDIM  = DUMMYSIZEARCH)
        PARAMETER (SCOTCH_GEOMDIM  = DUMMYSIZEGEOM)
        PARAMETER (SCOTCH_GRAPHDIM = DUMMYSIZEGRAPH)
        PARAMETER (SCOTCH_MAPDIM   = DUMMYSIZEMAP)
        PARAMETER (SCOTCH_MESHDIM  = DUMMYSIZEMESH)
        PARAMETER (SCOTCH_ORDERDIM = DUMMYSIZEORDER)
        PARAMETER (SCOTCH_STRATDIM = DUMMYSIZESTRAT)
