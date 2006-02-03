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
/**   NAME       : parser.h                                **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the declarations for    **/
/**                the strategy lexical and syntactic      **/
/**                analyzer.                               **/
/**                                                        **/
/**   DATES      : # Version 3.1  : from : 07 nov 1995     **/
/**                                 to     02 may 1996     **/
/**                # Version 3.2  : from : 07 oct 1996     **/
/**                                 to     19 oct 1996     **/
/**                # Version 3.3  : from : 01 oct 1998     **/
/**                                 to     01 oct 1998     **/
/**                # Version 4.0  : from : 20 dec 2001     **/
/**                                 to     11 jun 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

#define PARSERSTRINGLEN             256           /*+ Length of parser strings +*/

/*
**  The type definitions.
*/

/*+ Strategy node types. +*/

typedef enum StratNodeType_ {
  STRATNODECONCAT,                                /*+ Concatenation node       +*/
  STRATNODECOND,                                  /*+ Condition node           +*/
  STRATNODEEMPTY,                                 /*+ Empty strategy           +*/
  STRATNODEMETHOD,                                /*+ Method                   +*/
  STRATNODESELECT,                                /*+ Selection node           +*/
  STRATNODENBR                                    /*+ Number of strategy nodes +*/
} StratNodeType;

/*+ Method and graph parameter types. +*/

typedef enum StratParamType_ {
  STRATPARAMCASE = 0,                             /*+ Character; TRICK: FIRST +*/
  STRATPARAMDOUBLE,                               /*+ Double floating-point   +*/
  STRATPARAMINT,                                  /*+ Integer                 +*/
  STRATPARAMLOG,                                  /*+ Logical value           +*/
  STRATPARAMSTRAT,                                /*+ Strategy                +*/
  STRATPARAMSTRING                                /*+ String of characters    +*/
} StratParamType;

/*+ Test types, ordered by ascending priority,
    for proper writing of parentheses. Initial
    value should be zero for proper indexing.  +*/

typedef enum StratTestType_ {
  STRATTESTOR = 0,                                /*+ Or operator             +*/
  STRATTESTAND,                                   /*+ And operator            +*/
  STRATTESTNOT,                                   /*+ Not operator            +*/
  STRATTESTEQ,                                    /*+ Equal-to operator       +*/
  STRATTESTGT,                                    /*+ Greater-than operator   +*/
  STRATTESTLT,                                    /*+ Less-than operator      +*/
  STRATTESTADD,                                   /*+ Addition operator       +*/
  STRATTESTSUB,                                   /*+ Subtraction operator    +*/
  STRATTESTMUL,                                   /*+ Multiplication operator +*/
  STRATTESTMOD,                                   /*+ Modulus operator        +*/
  STRATTESTVAL,                                   /*+ Constant value          +*/
  STRATTESTVAR,                                   /*+ Variable                +*/
  STRATTESTNBR                                    /*+ Number of test nodes    +*/
} StratTestType;

/*+ Method characteristics. +*/

typedef struct StratMethodTab_ {
  unsigned int              meth;                 /*+ Method number in method table    +*/
  char *                    name;                 /*+ Method name                      +*/
  int                    (* func) ();             /*+ Pointer to bipartitioning method +*/
  void *                    data;                 /*+ Pointer to default parameters    +*/
} StratMethodTab;

/*+ Method parameter characteristics. +*/

typedef struct StratParamTab_ {
  unsigned int              meth;                 /*+ Method number in method table    +*/
  StratParamType            type;                 /*+ Parameter type                   +*/
  char *                    name;                 /*+ Parameter name                   +*/
  byte *                    database;             /*+ Pointer to data base in method   +*/
  byte *                    dataofft;             /*+ Pointer to data offset in method +*/
  void *                    datasltr;             /*+ Pointer to data selector         +*/
} StratParamTab;

/*+ Strategy characteristics. +*/

typedef struct StratTab_ {
  StratMethodTab *          methtab;              /*+ Pointer to method table    +*/
  StratParamTab *           paratab;              /*+ Pointer to parameter table +*/
  StratParamTab *           condtab;              /*+ Pointer to condition table +*/
} StratTab;

/*+ Concatenation strategy node. +*/

typedef struct StratNodeConcat_ {                 /*+ Concatenation node                        +*/
  struct Strat_ *           strat[2];             /*+ Pointers to the two strategies to combine +*/
} StratNodeConcat;

/*+ Condition and test strategy nodes. +*/

typedef union StratTestVal_ {                     /*+ Constant value +*/
  double                    valdbl;               /*+ Double value   +*/
  INT                       valint;               /*+ Integer value  +*/
  int                       vallog;               /*+ Logical value  +*/
} StratTestVal;

typedef struct StratTestVar_ {                    /*+ Condition variable                     +*/
  const StratTab *          datatab;              /*+ Pointer to data parameter table        +*/
  unsigned int              datadisp;             /*+ Displacement with respect to beginning +*/
} StratTestVar;

typedef struct StratTest_ {                       /*+ Test node +*/
  StratTestType             typetest;             /*+ Test type +*/
  StratParamType            typenode;             /*+ Node type +*/
  union {
    struct StratTest_ *     test[2];              /*+ Logical/relational branches +*/
    StratTestVal            val;                  /*+ Value                       +*/
    StratTestVar            var;                  /*+ Variable                    +*/
  } data;
} StratTest;

typedef struct StratNodeCond_ {                   /*+ Test node            +*/
  StratTest *               test;                 /*+ Test condition       +*/
  struct Strat_ *           strat[2];             /*+ Then/else strategies +*/
} StratNodeCond;

/*+ Data structure of the empty strategy operator node. +*/

typedef struct StratNodeEmpty_ {                  /*+ Empty node +*/
  byte                      dummy;                /*+ Dummy data +*/
} StratNodeEmpty;

/*+ Data structure of the empty strategy operator node. +*/

typedef double StratNodeMethodData[5];            /*+ Reserved padded space for method data */

typedef struct StratNodeMethod_ {                 /*+ Method node           +*/
  unsigned int              meth;                 /*+ Index in method table +*/
  StratNodeMethodData       data;                 /*+ Method data           +*/
} StratNodeMethod;

/*+ Data structure of the selection strategy operator node. +*/

typedef struct StratNodeSelect_ {                 /*+ Selection node                         +*/
  struct Strat_ *           strat[2];             /*+ Pointers to the two strategies to test +*/
} StratNodeSelect;

/*+ The strategy node data structure. +*/

typedef struct Strat_ {
  const StratTab *          tabl;                 /*+ Pointer to parsing strategy table +*/
  StratNodeType             type;                 /*+ Method type                       +*/
  union {                                         /*+ Method data                       +*/
    double                  padding;              /*+ Padding for double alignment      +*/
    StratNodeConcat         concat;               /*+ Concatenation node data           +*/
    StratNodeCond           cond;                 /*+ Condition node data               +*/
    StratNodeEmpty          empty;                /*+ Empty node data                   +*/
    StratNodeMethod         method;               /*+ Method node data                  +*/
    StratNodeSelect         select;               /*+ Selection node data               +*/
  } data;
} Strat;

/*
**  The external declarations.
*/

extern Strat                stratdummy;           /*+ Dummy empty strategy node +*/

/*
**  The function prototypes.
*/

#ifndef PARSER
#define static
#endif

Strat *                     stratInit           (const StratTab * const , const char * const);
int                         stratExit           (Strat * const);
int                         stratSave           (const Strat * const, FILE * const);

int                         stratTestEval       (const StratTest * const, StratTest * const, const void * const);
static int                  stratTestEvalCast   (StratTest * const, StratTest * const);
int                         stratTestExit       (StratTest * const);
int                         stratTestSave       (const StratTest * const, FILE * const);

#undef static
