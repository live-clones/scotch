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
/**   NAME       : mesh.h                                  **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                These lines are the data declarations   **/
/**                for the mesh structure.                 **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 21 mar 2003     **/
/**                                 to     24 oct 2003     **/
/**                # Version 2.0  : from : 28 feb 2004     **/
/**                                 to     13 may 2004     **/
/**                                                        **/
/************************************************************/

#define MESH_H

/*
**  The defines.
*/

#define Mesh                        SCOTCH_Mesh

#define meshInit                    SCOTCH_meshInit
#define meshExit                    SCOTCH_meshExit
#define meshLoad                    SCOTCH_meshLoad
#define meshSave                    SCOTCH_meshSave
#define meshBuildMesh               SCOTCH_meshBuild
#define meshBase                    SCOTCH_meshBase
#define meshData                    SCOTCH_meshData
#define meshCheck                   SCOTCH_meshCheck

/*
**  The function prototypes.
*/

#ifndef MESH
#define static
#endif

#undef static
