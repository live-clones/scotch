/* Copyright INRIA 2004
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
*/
/************************************************************/
/**                                                        **/
/**   NAME       : library_graph_io_chac_f.c               **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module is the Fortran API for the  **/
/**                graph i/o routines of the libSCOTCH     **/
/**                library.                                **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 23 nov 2005     **/
/**                                 to     23 nov 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define LIBRARY

#include "module.h"
#include "common.h"
#include "scotch.h"

/**************************************/
/*                                    */
/* These routines are the Fortran API */
/* for the mapping routines.          */
/*                                    */
/**************************************/

/* String lengths are passed at the very
** end of the argument list.
*/

FORTRAN (                                             \
SCOTCHFGRAPHGEOMLOADCHAC, scotchfgraphgeomloadchac, ( \
SCOTCH_Graph * const        grafptr,                  \
SCOTCH_Geom * const         geomptr,                  \
const int * const           filegrfptr,               \
const int * const           filegeoptr,               \
const char * const          dataptr, /* No use */     \
int * const                 revaptr,                  \
const int                   datanbr),                 \
(grafptr, geomptr, filegrfptr, filegeoptr, dataptr, revaptr, datanbr))
{
  FILE *              filegrfstream;              /* Streams to build from handles */
  FILE *              filegeostream;
  int                 filegrfnum;                 /* Duplicated handle */
  int                 filegeonum;
  int                 o;

  if ((filegrfnum = dup (*filegrfptr)) < 0) {     /* If cannot duplicate file descriptor */
    errorPrint ("SCOTCHFGRAPHGEOMLOADCHAC: cannot duplicate handle (1)");
    *revaptr = 1;                                 /* Indicate error */
    return;
  }
  if ((filegeonum = dup (*filegeoptr)) < 0) {     /* If cannot duplicate file descriptor */
    errorPrint ("SCOTCHFGRAPHGEOMLOADCHAC: cannot duplicate handle (2)");
    close      (filegrfnum);
    *revaptr = 1;                                 /* Indicate error */
    return;
  }
  if ((filegrfstream = fdopen (filegrfnum, "r+")) == NULL) { /* Build stream from handle */
    errorPrint ("SCOTCHFGRAPHGEOMLOADCHAC: cannot open input stream (1)");
    close      (filegrfnum);
    close      (filegeonum);
    *revaptr = 1;
    return;
  }
  if ((filegeostream = fdopen (filegeonum, "r+")) == NULL) { /* Build stream from handle */
    errorPrint ("SCOTCHFGRAPHGEOMLOADCHAC: cannot open input stream (2)");
    fclose     (filegrfstream);
    close      (filegeonum);
    *revaptr = 1;
    return;
  }

  o = SCOTCH_graphGeomLoadChac (grafptr, geomptr, filegrfstream, filegeostream, NULL);

  fclose (filegrfstream);                         /* This closes file descriptors too */
  fclose (filegeostream);

  *revaptr = o;
}

/* String lengths are passed at the very
** end of the argument list.
*/

FORTRAN (                                             \
SCOTCHFGRAPHGEOMSAVECHAC, scotchfgraphgeomsavechac, ( \
const SCOTCH_Graph * const  grafptr,                  \
const SCOTCH_Geom * const   geomptr,                  \
const int * const           filegrfptr,               \
const int * const           filegeoptr,               \
const char * const          dataptr, /* No use */     \
int * const                 revaptr,                  \
const int                   datanbr),                 \
(grafptr, geomptr, filegrfptr, filegeoptr, dataptr, revaptr, datanbr))
{
  FILE *              filegrfstream;              /* Streams to build from handles */
  FILE *              filegeostream;
  int                 filegrfnum;                 /* Duplicated handle */
  int                 filegeonum;
  int                 o;

  if ((filegrfnum = dup (*filegrfptr)) < 0) {     /* If cannot duplicate file descriptor */
    errorPrint ("SCOTCHFGRAPHGEOMSAVECHAC: cannot duplicate handle (1)");
    *revaptr = 1;                                 /* Indicate error */
    return;
  }
  if ((filegeonum = dup (*filegeoptr)) < 0) {     /* If cannot duplicate file descriptor */
    errorPrint ("SCOTCHFGRAPHGEOMSAVECHAC: cannot duplicate handle (2)");
    close      (filegrfnum);
    *revaptr = 1;                                 /* Indicate error */
    return;
  }
  if ((filegrfstream = fdopen (filegrfnum, "w+")) == NULL) { /* Build stream from handle */
    errorPrint ("SCOTCHFGRAPHGEOMSAVECHAC: cannot open output stream (1)");
    close      (filegrfnum);
    close      (filegeonum);
    *revaptr = 1;
    return;
  }
  if ((filegeostream = fdopen (filegeonum, "w+")) == NULL) { /* Build stream from handle */
    errorPrint ("SCOTCHFGRAPHGEOMSAVECHAC: cannot open output stream (2)");
    fclose     (filegrfstream);
    close      (filegeonum);
    *revaptr = 1;
    return;
  }

  o = SCOTCH_graphGeomSaveChac (grafptr, geomptr, filegrfstream, filegeostream, NULL);

  fclose (filegrfstream);                         /* This closes file descriptors too */
  fclose (filegeostream);

  *revaptr = o;
}
