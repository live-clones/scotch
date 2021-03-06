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
/**   NAME       : arch.h                                  **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declarations   **/
/**                for the generic target architecture     **/
/**                functions.                              **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 01 dec 1992     **/
/**                                 to   : 24 mar 1993     **/
/**                # Version 1.2  : from : 04 feb 1994     **/
/**                                 to   : 11 feb 1994     **/
/**                # Version 1.3  : from : 20 apr 1994     **/
/**                                 to   : 20 apr 1994     **/
/**                # Version 2.0  : from : 06 jun 1994     **/
/**                                 to   : 12 nov 1994     **/
/**                # Version 2.1  : from : 07 apr 1995     **/
/**                                 to   : 30 jun 1995     **/
/**                # Version 3.0  : from : 01 jul 1995     **/
/**                                 to     08 sep 1995     **/
/**                # Version 3.1  : from : 02 may 1996     **/
/**                                 to     20 jul 1996     **/
/**                # Version 3.2  : from : 07 sep 1996     **/
/**                                 to     13 may 1998     **/
/**                # Version 3.3  : from : 01 oct 1998     **/
/**                                 to     07 oct 1998     **/
/**                # Version 3.4  : from : 08 nov 2001     **/
/**                                 to     08 nov 2001     **/
/**                # Version 4.0  : from : 01 jan 2002     **/
/**                                 to     07 dec 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

typedef INT Anum;                                 /*+ Generic integer +*/

/** The domain number type. **/

typedef Anum ArchDomNum;                          /*+ Domain number +*/

#define ARCHDOMNOTTERM              ((ArchDomNum) ~0) /*+ Not-terminal number +*/

/** The architecture class type. **/

typedef struct ArchClass_ {
  char *                    archname;             /*+ Architecture name                 +*/
  int                    (* archLoad)  ();        /*+ Architecture loading function     +*/
  int                    (* archSave)  ();        /*+ Architecture saving function      +*/
  int                    (* archFree)  ();        /*+ Architecture freeing function     +*/
  ArchDomNum             (* domNum)    ();        /*+ Domain labeling function          +*/
  int                    (* domTerm)   ();        /*+ Terminal domain building function +*/
  Anum                   (* domSize)   ();        /*+ Domain size function              +*/
  Anum                   (* domWght)   ();        /*+ Domain weight function            +*/
  Anum                   (* domDist)   ();        /*+ Distance computation function     +*/
  int                    (* domFrst)   ();        /*+ Compute biggest domain            +*/
  int                    (* domLoad)   ();        /*+ Domain loading routine            +*/
  int                    (* domSave)   ();        /*+ Domain saving routine             +*/
  int                    (* domBipart) ();        /*+ Domain bipartitioning routine     +*/
  int                       domsizeof;            /*+ Size in bytes of domain data      +*/
} ArchClass;

/** The architecture type. **/

typedef double ArchDummy[3];                      /*+ Size of the dummy space that can hold any architecture +*/

typedef struct Arch_ {
  const ArchClass *         class;                /*+ Pointer to architecture class        +*/
  union {                                         /*+ Architecture data                    +*/
    ArchDummy               dummy;                /*+ Dummy data for external size setting +*/
#if 0
    ArchDeco                deco;                 /*+ Decomposition-descripted architecture      +*/
    ArchMesh2               mesh2;                /*+ 2D-mesh architecture                       +*/
    ArchMesh3               mesh3;                /*+ 3D-mesh architecture                       +*/
    ArchTorus2              torus2;               /*+ 2D-torus architecture                      +*/
    ArchTorus3              torus3;               /*+ 3D-torus architecture                      +*/
    ArchHcub                hcub;                 /*+ Hypercube architecture                     +*/
    ArchCmplt               cmplt;                /*+ Complete graph architecture                +*/
    ArchLeaf                leaf;                 /*+ Tree-leaf architecture                     +*/
    ArchVcmplt              vcmplt;               /*+ Variable-sized complete graph architecture +*/
    ArchVhcub               vhcub;                /*+ Variable-sized hypercube architecture      +*/
#endif
  } data;
} Arch;

/** The domain structure type. **/

typedef Anum ArchDomDummy[6];                     /*+ Size of the dummy space that can hold any domain +*/

typedef struct ArchDom_ {
  union {                                         /*+ The domain data                       +*/
    ArchDomDummy            dummy;                /*+ Dummy data for external space setting +*/
#if 0
    ArchDomDeco             deco;                 /*+ Decomposition-descripted domain type      +*/
    ArchDomMesh2            mesh2;                /*+ 2D-mesh domain type                       +*/
    ArchDomMesh3            mesh3;                /*+ 3D-mesh domain type                       +*/
    ArchDomTorus2           torus2;               /*+ 2D-torus domain type                      +*/
    ArchDomTorus3           torus3;               /*+ 3D-torus domain type                      +*/
    ArchDomHcub             hcub;                 /*+ Hypercube domain type                     +*/
    ArchDomCmplt            cmplt;                /*+ Complete graph domain type                +*/
    ArchDomLeaf             leaf;                 /*+ Tree-leaf domain type                     +*/
    ArchDomVcmplt           vcmplt;               /*+ Variable-sized complete graph domain type +*/
    ArchDomVhcub            vhcub;                /*+ Variable-sized hypercube domain type      +*/
#endif
  } data;
} ArchDom;

/*
**  The function prototypes.
*/

#ifndef ARCH
#define static
#endif

int                         archInit            (Arch * restrict const);
int                         archExit            (Arch * restrict const);
int                         archFree            (Arch * restrict const);
int                         archLoad            (Arch * restrict const, FILE * const);
int                         archSave            (const Arch * const, FILE * const);
char *                      archName            (const Arch * const);
const ArchClass *           archClass           (const char * const);

ArchDomNum                  archDomNum          (const Arch * const, const ArchDom * const);
int                         archDomTerm         (const Arch * const, ArchDom * const, const ArchDomNum);
Anum                        archDomSize         (const Arch * const, const ArchDom * const);
Anum                        archDomWght         (const Arch * const, const ArchDom * const);
Anum                        archDomDist         (const Arch * const, const ArchDom * const, const ArchDom * const);
int                         archDomFrst         (const Arch * const, ArchDom * const);
int                         archDomLoad         (const Arch * const, ArchDom * const, FILE * const);
int                         archDomSave         (const Arch * const, const ArchDom * const, FILE * const);
int                         archDomBipart       (const Arch * const, const ArchDom * const, ArchDom * const, ArchDom * const);

#undef static

/*
**  The macro definitions.
*/

#define archDomSizeof(a)            ((a)->class->domsizeof)
#define archName(a)                 (((a)->class == NULL) ? "" : (a)->class->archname)

#if ((! defined SCOTCH_DEBUG_ARCH2) || (defined ARCH))
#define archDomNum2(arch,dom)       (((ArchDomNum (*) (const void * const, const void * const)) (arch)->class->domNum) ((const void * const) &(arch)->data, (const void * const) &(dom)->data))
#define archDomTerm2(arch,dom,num)  (((int (*) (const void * const, void * const, const ArchDomNum)) (arch)->class->domTerm) ((void *) &(arch)->data, (void *) &(dom)->data, (num)))
#define archDomSize2(arch,dom)      (((Anum (*) (const void * const, const void * const)) (arch)->class->domSize) ((void *) &(arch)->data, (void *) &(dom)->data))
#define archDomWght2(arch,dom)      (((Anum (*) (const void * const, const void * const)) (arch)->class->domWght) ((void *) &(arch)->data, (void *) &(dom)->data))
#define archDomDist2(arch,dom0,dom1) (((Anum (*) (const void * const, const void * const, const void * const)) (arch)->class->domDist) ((const void *) &(arch)->data, (const void *) &(dom0)->data, (const void *) &(dom1)->data))
#define archDomFrst2(arch,dom)      (((int (*) (const void * const, void * const)) (arch)->class->domFrst) ((const void * const) &(arch)->data, (void * const) &(dom)->data))
#define archDomBipart2(arch,dom,dom0,dom1) (((int (*) (const void * const, const void * const, void * const, void * const)) (arch)->class->domBipart) ((const void * const) &(arch)->data, (const void * const) &(dom)->data, (void * const) &(dom0)->data, (void * const) &(dom1)->data))
#endif
#ifndef SCOTCH_DEBUG_ARCH2
#define archDomNum                  archDomNum2
#define archDomTerm                 archDomTerm2
#define archDomSize                 archDomSize2
#define archDomWght                 archDomWght2
#define archDomDist                 archDomDist2
#define archDomFrst                 archDomFrst2
#define archDomBipart               archDomBipart2
#endif /* SCOTCH_DEBUG_ARCH2 */
