/* Copyright 2004,2007 ENSEIRB, INRIA & CNRS
**
** This file is part of the Scotch software package for static mapping,
** graph partitioning and sparse matrix ordering.
**
** This software is governed by the CeCILL-C license under French law
** and abiding by the rules of distribution of free software. You can
** use, modify and/or redistribute the software under the terms of the
** CeCILL-C license as circulated by CEA, CNRS and INRIA at the following
** URL: "http://www.cecill.info".
** 
** As a counterpart to the access to the source code and rights to copy,
** modify and redistribute granted by the license, users are provided
** only with a limited warranty and the software's author, the holder of
** the economic rights, and the successive licensors have only limited
** liability.
** 
** In this respect, the user's attention is drawn to the risks associated
** with loading, using, modifying and/or developing or reproducing the
** software by the user in light of its specific status of free software,
** that may mean that it is complicated to manipulate, and that also
** therefore means that it is reserved for developers and experienced
** professionals having in-depth computer knowledge. Users are therefore
** encouraged to load and test the software's suitability as regards
** their requirements in conditions enabling the security of their
** systems and/or data to be ensured and, more generally, to use and
** operate it in the same conditions as regards security.
** 
** The fact that you are presently reading this means that you have had
** knowledge of the CeCILL-C license and that you accept its terms.
*/
/************************************************************/
/**                                                        **/
/**   NAME       : arch.c                                  **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module handles the generic target  **/
/**                architecture functions.                 **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 01 dec 1992     **/
/**                                 to   : 24 mar 1993     **/
/**                # Version 1.2  : from : 04 feb 1994     **/
/**                                 to   : 11 feb 1994     **/
/**                # Version 1.3  : from : 20 apr 1994     **/
/**                                 to   : 20 apr 1994     **/
/**                # Version 2.0  : from : 06 jun 1994     **/
/**                                 to   : 23 dec 1994     **/
/**                # Version 2.1  : from : 07 apr 1995     **/
/**                                 to   : 29 jun 1995     **/
/**                # Version 3.0  : from : 01 jul 1995     **/
/**                                 to     16 aug 1995     **/
/**                # Version 3.1  : from : 02 may 1996     **/
/**                                 to     17 jul 1996     **/
/**                # Version 3.2  : from : 07 sep 1996     **/
/**                                 to     28 sep 1998     **/
/**                # Version 3.3  : from : 01 oct 1998     **/
/**                                 to     01 oct 1998     **/
/**                # Version 3.4  : from : 08 nov 2001     **/
/**                                 to     08 nov 2001     **/
/**                # Version 4.0  : from : 04 nov 2003     **/
/**                                 to     09 jan 2004     **/
/**                # Version 5.1  : from : 11 dec 2007     **/
/**                                 to     11 dec 2007     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define ARCH

#include "module.h"
#include "common.h"
#include "arch.h"
#include "arch_cmplt.h"
#include "arch_cmpltw.h"
#include "arch_deco.h"
#include "arch_hcub.h"
#include "arch_mesh.h"
#include "arch_tleaf.h"
#include "arch_torus.h"
#include "arch_vcmplt.h"
#include "arch_vhcub.h"

/*
**  The static definitions.
*/

static const ArchClass      archClassTab[] = { { "cmplt",
                                                 archCmpltArchLoad,
                                                 archCmpltArchSave,
                                                 NULL,
                                                 archCmpltDomNum,
                                                 archCmpltDomTerm,
                                                 archCmpltDomSize,
                                                 archCmpltDomSize,
                                                 archCmpltDomDist,
                                                 archCmpltDomFrst,
                                                 archCmpltDomLoad,
                                                 archCmpltDomSave,
                                                 archCmpltDomBipart,
                                                 sizeof (ArchCmpltDom) },
                                               { "cmpltw",
                                                 archCmpltwArchLoad,
                                                 archCmpltwArchSave,
                                                 archCmpltwArchFree,
                                                 archCmpltwDomNum,
                                                 archCmpltwDomTerm,
                                                 archCmpltwDomSize,
                                                 archCmpltwDomWght,
                                                 archCmpltwDomDist,
                                                 archCmpltwDomFrst,
                                                 archCmpltwDomLoad,
                                                 archCmpltwDomSave,
                                                 archCmpltwDomBipart,
                                                 sizeof (ArchCmpltwDom) },
                                               { "deco",
                                                 archDecoArchLoad,
                                                 archDecoArchSave,
                                                 archDecoArchFree,
                                                 archDecoDomNum,
                                                 archDecoDomTerm,
                                                 archDecoDomSize,
                                                 archDecoDomWght,
                                                 archDecoDomDist,
                                                 archDecoDomFrst,
                                                 archDecoDomLoad,
                                                 archDecoDomSave,
                                                 archDecoDomBipart,
                                                 sizeof (ArchDecoDom) },
                                               { "hcub",
                                                 archHcubArchLoad,
                                                 archHcubArchSave,
                                                 NULL,
                                                 archHcubDomNum,
                                                 archHcubDomTerm,
                                                 archHcubDomSize,
                                                 archHcubDomSize,
                                                 archHcubDomDist,
                                                 archHcubDomFrst,
                                                 archHcubDomLoad,
                                                 archHcubDomSave,
                                                 archHcubDomBipart,
                                                 sizeof (ArchHcubDom) },
                                               { "leaf",
                                                 archTleafArchLoad,
                                                 archTleafArchSave,
                                                 NULL,
                                                 archTleafDomNum,
                                                 archTleafDomTerm,
                                                 archTleafDomSize,
                                                 archTleafDomSize,
                                                 archTleafDomDist,
                                                 archTleafDomFrst,
                                                 archTleafDomLoad,
                                                 archTleafDomSave,
                                                 archTleafDomBipart,
                                                 sizeof (ArchTleafDom) },
                                               { "mesh2D",
                                                 archMesh2ArchLoad,
                                                 archMesh2ArchSave,
                                                 NULL,
                                                 archMesh2DomNum,
                                                 archMesh2DomTerm,
                                                 archMesh2DomSize,
                                                 archMesh2DomSize,
                                                 archMesh2DomDist,
                                                 archMesh2DomFrst,
                                                 archMesh2DomLoad,
                                                 archMesh2DomSave,
                                                 archMesh2DomBipart,
                                                 sizeof (ArchMesh2Dom) },
#ifdef SCOTCH_DEBUG_ARCH3
                                               { "mesh2O",
                                                 archMesh2ArchLoad,
                                                 archMesh2ArchSave,
                                                 NULL,
                                                 archMesh2DomNum,
                                                 archMesh2DomTerm,
                                                 archMesh2DomSize,
                                                 archMesh2DomSize,
                                                 archMesh2DomDist,
                                                 archMesh2DomFrst,
                                                 archMesh2DomLoad,
                                                 archMesh2DomSave,
                                                 archMesh2DomBipartO,
                                                 sizeof (ArchMesh2Dom) },
                                               { "mesh2U",
                                                 archMesh2ArchLoad,
                                                 archMesh2ArchSave,
                                                 NULL,
                                                 archMesh2DomNum,
                                                 archMesh2DomTerm,
                                                 archMesh2DomSize,
                                                 archMesh2DomSize,
                                                 archMesh2DomDist,
                                                 archMesh2DomFrst,
                                                 archMesh2DomLoad,
                                                 archMesh2DomSave,
                                                 archMesh2DomBipartU,
                                                 sizeof (ArchMesh2Dom) },
#endif /* SCOTCH_DEBUG_ARCH3 */
                                               { "mesh3D",
                                                 archMesh3ArchLoad,
                                                 archMesh3ArchSave,
                                                 NULL,
                                                 archMesh3DomNum,
                                                 archMesh3DomTerm,
                                                 archMesh3DomSize,
                                                 archMesh3DomSize,
                                                 archMesh3DomDist,
                                                 archMesh3DomFrst,
                                                 archMesh3DomLoad,
                                                 archMesh3DomSave,
                                                 archMesh3DomBipart,
                                                 sizeof (ArchMesh3Dom) },
                                               { "torus2D",
                                                 archTorus2ArchLoad,
                                                 archTorus2ArchSave,
                                                 NULL,
                                                 archTorus2DomNum,
                                                 archTorus2DomTerm,
                                                 archTorus2DomSize,
                                                 archTorus2DomSize,
                                                 archTorus2DomDist,
                                                 archTorus2DomFrst,
                                                 archTorus2DomLoad,
                                                 archTorus2DomSave,
                                                 archTorus2DomBipart,
                                                 sizeof (ArchTorus2Dom) },
                                               { "torus3D",
                                                 archTorus3ArchLoad,
                                                 archTorus3ArchSave,
                                                 NULL,
                                                 archTorus3DomNum,
                                                 archTorus3DomTerm,
                                                 archTorus3DomSize,
                                                 archTorus3DomSize,
                                                 archTorus3DomDist,
                                                 archTorus3DomFrst,
                                                 archTorus3DomLoad,
                                                 archTorus3DomSave,
                                                 archTorus3DomBipart,
                                                 sizeof (ArchTorus3Dom) },
                                               { "varcmplt",
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 archVcmpltDomNum,
                                                 archVcmpltDomTerm,
                                                 archVcmpltDomSize,
                                                 archVcmpltDomSize,
                                                 archVcmpltDomDist,
                                                 archVcmpltDomFrst,
                                                 archVcmpltDomLoad,
                                                 archVcmpltDomSave,
                                                 archVcmpltDomBipart,
                                                 sizeof (ArchVcmpltDom) },
                                               { "varhcub",
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 archVhcubDomNum,
                                                 archVhcubDomTerm,
                                                 archVhcubDomSize,
                                                 archVhcubDomSize,
                                                 archVhcubDomDist,
                                                 archVhcubDomFrst,
                                                 archVhcubDomLoad,
                                                 archVhcubDomSave,
                                                 archVhcubDomBipart,
                                                 sizeof (ArchVhcubDom) },
                                               { NULL,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 0     } };

/**************************************/
/*                                    */
/* These are the entry points for the */
/* generic architecture routines.     */
/*                                    */
/**************************************/

/* This routine initializes an architecture structure.
** It zeroes the architecture body so that architecture
** specific routines can check if their pointers have
** been initialized or not.
** It returns:
** - !NULL  : pointer to the target architecture.
** - NULL   : on error.
*/

int
archInit (
Arch * restrict const       archptr)
{
  memset (archptr, 0, sizeof (Arch));             /* Initialize architecture body (arch->class = NULL) */

  return (0);
}

/* This routine deletes an architecture.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
archExit (
Arch * restrict const       archptr)
{
  return (archFree (archptr));                    /* Free architecture data */
}

/* This routine frees the architecture data.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
archFree (
Arch * restrict const       archptr)
{
  int                 o;

  o = 0;                                          /* Assume everything will be all right */
  if ((archptr->class           != NULL) &&
      (archptr->class->archFree != NULL))         /* If there is a specific freeing routing                */
    o = archptr->class->archFree (&archptr->data); /* Call it                                              */
  memset (archptr, 0, sizeof (Arch));             /* Initialize the architecture body (arch->class = NULL) */

  return (o);
}

/* This routine loads an architecture.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
archLoad (
Arch * restrict const       archptr,
FILE * const                stream)
{
  const ArchClass * restrict  class;              /* Pointer to architecture class */
  char                        name[256];          /* Architecture name string      */

  if (fscanf (stream, "%255s", name) != 1) {      /* Read architecture name */
    errorPrint ("archLoad: cannot load architecture type");
    return     (1);
  }
  name[255] = '\0';                               /* Set end of string */

  if ((class = archClass (name)) == NULL) {       /* Get class from its name */
    errorPrint ("archLoad: invalid architecture type");
    return     (1);
  }

  if (class->archLoad != NULL) {                  /* If class has loading function */
    if (class->archLoad (&archptr->data, stream) != 0) { /* Load class data        */
      errorPrint ("archLoad: cannot load architecture data");
      memset     (archptr, 0, sizeof (Arch));     /* Initialize architecture body */
      return     (1);
    }
  }
  archptr->class = class;                         /* Set architecture class */

  return (0);
}

/* This routine saves an architecture.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
archSave (
const Arch * restrict const archptr,
FILE * restrict const       stream)
{
  int                 o;

  if (archptr->class == NULL)                     /* If no architecture type defined */
    return (0);                                   /* Nothing to do                   */

  o = (fprintf (stream, "%s\n",                   /* Write architecture class */
                archptr->class->archname) == EOF);
  if (archptr->class->archSave != NULL)           /* If class has saving function      */
    o |= archptr->class->archSave (&archptr->data, stream); /* Write architecture data */
  o |= (fprintf (stream, "\n") == EOF);
  if (o != 0)
    errorPrint ("archSave: bad output");

  return (o);
}

/* This routine returns the pointer to
** the class of a given architecture
** name.
** It returns:
** - !NULL  : class pointer.
** - NULL   : on error.
*/

const ArchClass *
archClass (
const char * const          name)
{
  const ArchClass * restrict  class;              /* Pointer to architecture class */

  for (class = archClassTab;                      /* For all classes             */
       (class->archname != NULL) &&               /* Search if class names match */
       (strcasecmp (name, class->archname) != 0);
       class ++) ;

  return ((class->archname != NULL) ? class : NULL);
}

/**************************************/
/*                                    */
/* These are the entry points for the */
/* generic domain routines. They are  */
/* used only in debugging mode, to    */
/* provide breakpoints for routines   */
/* which are else implemented as      */
/* macros for the sake of efficiency. */
/*                                    */
/**************************************/

/* This function returns the smallest number
** of terminal domain included within the
** given domain.
*/

#ifdef SCOTCH_DEBUG_ARCH2

ArchDomNum
archDomNum (
const Arch * const          archptr,
const ArchDom * const       domptr)
{
  return (archDomNum2 (archptr, domptr));         /* Call proper routine */
}

#endif /* SCOTCH_DEBUG_ARCH2 */

/* This function computes the terminal domain
** associated with the given terminal number.
** It returns:
** - 0  : if label is valid and domain has been updated.
** - 1  : if label is invalid.
** - 2  : on error.
*/

#ifdef SCOTCH_DEBUG_ARCH2

int
archDomTerm (
const Arch * const          archptr,
ArchDom * restrict const    domptr,
const ArchDomNum            domnum)
{
  return (archDomTerm2 (archptr, domptr, domnum)); /* Call proper routine */
}

#endif /* SCOTCH_DEBUG_ARCH2 */

/* This function returns the number
** of elements in the given domain.
** It returns:
** - >0  : size of the domain.
** - 0   : on error.
*/

#ifdef SCOTCH_DEBUG_ARCH2

Anum
archDomSize (
const Arch * const          archptr,
const ArchDom * const       domptr)
{
  return (archDomSize2 (archptr, domptr));        /* Call proper routine */
}

#endif /* SCOTCH_DEBUG_ARCH2 */

/* This function returns the weight
** of the given domain.
** It returns:
** - >0  : weight of the domain.
** - 0   : on error.
*/

#ifdef SCOTCH_DEBUG_ARCH2

Anum
archDomWght (
const Arch * const          archptr,
const ArchDom * const       domptr)
{
  return (archDomWght2 (archptr, domptr));        /* Call proper routine */
}

#endif /* SCOTCH_DEBUG_ARCH2 */

/* This function gives the average
** distance between two domains.
** It returns:
** - !-1  : distance between subdomains.
** - -1   : on error.
*/

#ifdef SCOTCH_DEBUG_ARCH2

Anum
archDomDist (
const Arch * const          archptr,
const ArchDom * const       dom0ptr,
const ArchDom * const       dom1ptr)
{
  return (archDomDist2 (archptr, dom0ptr, dom1ptr)); /* Call proper routine */
}

#endif /* SCOTCH_DEBUG_ARCH2 */

/* This function sets the biggest
** available domain for the given
** architecture.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

#ifdef SCOTCH_DEBUG_ARCH2

int
archDomFrst (
const Arch * const          archptr,
ArchDom * const             domptr)
{
  return (archDomFrst2 (archptr, domptr));        /* Call proper routine */
}

#endif /* SCOTCH_DEBUG_ARCH2 */

/* This routine reads domain information
** from the given stream.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
archDomLoad (
const Arch * const          archptr,
ArchDom * const             domptr,
FILE * const                stream)
{
  return (archptr->class->domLoad (&archptr->data, /* Call proper routine */
                                   &domptr->data,
                                   stream));
}

/* This routine saves domain information
** to the given stream.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
archDomSave (
const Arch * const          archptr,
const ArchDom * const       domptr,
FILE * const                stream)
{
  return (archptr->class->domSave (&archptr->data, /* Call proper routine */
                                   &domptr->data,
                                   stream));
}

/* This function tries to split a domain into
** two subdomains. The two subdomains are created
** so that subdomain 0 has same T_domNum as
** original domain.
** It returns:
** - 0  : if bipartitioning succeeded.
** - 1  : if bipartitioning could not be performed.
** - 2  : on error.
*/

#ifdef SCOTCH_DEBUG_ARCH2

int
archDomBipart (
const Arch * const          archptr,
const ArchDom * const       domptr,
ArchDom * const             dom0ptr,
ArchDom * const             dom1ptr)
{
  int                 o;

  o = archDomBipart2 (archptr, domptr, dom0ptr, dom1ptr); /* Call proper routine */

  if ((o == 0) &&                                 /* Check domain number coherence for fixed-sized architectures */
      (strncmp (archName (archptr), "var", 3) != 0) &&
      (archDomNum (archptr, dom0ptr) != archDomNum (archptr, domptr))) {
    errorPrint ("archDomBipart: domain number mismatch");
    return     (2);
  }

  return (o);
}

#endif /* SCOTCH_DEBUG_ARCH2 */
