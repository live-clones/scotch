/* A Bison parser, made by GNU Bison 2.0.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     METHODNAME = 258,
     PARAMNAME = 259,
     VALCASE = 260,
     VALDOUBLE = 261,
     VALINT = 262,
     VALSTRING = 263,
     VALSTRAT = 264,
     VALPARAM = 265,
     VALTEST = 266
   };
#endif
#define METHODNAME 258
#define PARAMNAME 259
#define VALCASE 260
#define VALDOUBLE 261
#define VALINT 262
#define VALSTRING 263
#define VALSTRAT 264
#define VALPARAM 265
#define VALTEST 266




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 94 "parser_yy.y"
typedef union YYSTYPE {
  char                      CASEVAL;              /* Case value          */
  StratTest *               TEST;                 /* Test type           */
  StratTestType             TESTOP;               /* Relational type     */
  double                    DOUBLE;               /* Double-precision    */
  INT                       INTEGER;              /* Integer             */
  char                      STRING[PARSERSTRINGLEN]; /* Character string */
  struct {
    const StratTab *        tabl;                 /* Current tables    */
    Strat *                 strat;                /* Current method    */
    StratParamTab *         param;                /* Current parameter */
  } SAVE;                                         /* Parameter type    */
  Strat *                   STRAT;                /* Strategy tree     */
} YYSTYPE;
/* Line 1318 of yacc.c.  */
#line 74 "y.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



