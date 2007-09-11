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

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



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




/* Copy the first part of user declarations.  */
#line 1 "parser_yy.y"

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
/**   NAME       : parser_yy.y                             **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module is the syntactic parser     **/
/**                which processes strategy strings.       **/
/**                                                        **/
/**   DATES      : # Version 3.1  : from : 07 nov 1995     **/
/**                                 to     13 jun 1996     **/
/**                # Version 3.2  : from : 24 sep 1996     **/
/**                                 to     27 feb 1997     **/
/**                # Version 3.3  : from : 01 oct 1998     **/
/**                                 to     01 oct 1998     **/
/**                # Version 4.0  : from : 20 dec 2001     **/
/**                                 to     11 jun 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define PARSER_YY

#include "module.h"
#include "common.h"

#undef INTEGER                                    /* In case someone defined them */
#undef DOUBLE

#include "parser.h"
#include "parser_ll.h"
#include "parser_yy.h"

/* Change some function names. */

#define yylex                       stratParserLex /* Lexical analyzer           */
#define yyparse                     stratParserParse2 /* Parser function name    */
#define yyerror                     stratParserError /* Error processing routine */

/* #define SCOTCH_DEBUG_PARSER3 */
#ifdef SCOTCH_DEBUG_PARSER3
extern int                  yydebug;
#define YYDEBUG                     1
#endif /* SCOTCH_DEBUG_PARSER3 */

/*
**  The static and global definitions.
**  See also at the end of this file.
*/

static const StratTab *     parserstrattab;       /* Pointer to parsing tables          */
static Strat *              parserstratcurr = NULL; /* Pointer to current strategy node */
static StratParamTab *      parserparamcurr = NULL; /* Pointer to current parameter     */

extern unsigned int         parsermethtokentab[]; /* Pre-definition for stupid compilers */



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

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
/* Line 190 of yacc.c.  */
#line 206 "y.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 213 of yacc.c.  */
#line 218 "y.tab.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

# ifndef YYFREE
#  define YYFREE free
# endif
# ifndef YYMALLOC
#  define YYMALLOC malloc
# endif

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   else
#    define YYSTACK_ALLOC alloca
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE))			\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  13
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   82

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  31
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  35
/* YYNRULES -- Number of rules. */
#define YYNRULES  61
/* YYNRULES -- Number of states. */
#define YYNSTATES  88

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   266

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    24,     2,     2,     2,    30,    23,     2,
      17,    18,    29,    27,    21,    28,     2,    12,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    15,    14,
      25,    22,    26,    13,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    19,    16,    20,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     5,     6,     7,    16,    18,    21,    22,
      26,    28,    30,    31,    34,    36,    40,    42,    43,    47,
      48,    49,    55,    56,    60,    62,    63,    68,    70,    72,
      74,    76,    77,    80,    82,    84,    88,    90,    94,    96,
      99,   103,   105,   109,   111,   113,   115,   119,   121,   123,
     125,   129,   131,   133,   137,   139,   141,   145,   147,   149,
     151,   153
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      32,     0,    -1,    33,    -1,    -1,    -1,    34,    12,    51,
      35,    13,    37,    36,    14,    -1,    37,    -1,    15,    37,
      -1,    -1,    37,    16,    38,    -1,    38,    -1,    39,    -1,
      -1,    39,    40,    -1,    40,    -1,    17,    33,    18,    -1,
      41,    -1,    -1,     3,    42,    43,    -1,    -1,    -1,    44,
      19,    46,    45,    20,    -1,    -1,    46,    21,    47,    -1,
      47,    -1,    -1,     4,    48,    22,    49,    -1,     5,    -1,
       6,    -1,     7,    -1,     8,    -1,    -1,    50,    33,    -1,
       1,    -1,    52,    -1,    52,    16,    53,    -1,    53,    -1,
      53,    23,    54,    -1,    54,    -1,    24,    54,    -1,    17,
      52,    18,    -1,    55,    -1,    57,    56,    57,    -1,    25,
      -1,    22,    -1,    26,    -1,    57,    58,    59,    -1,    59,
      -1,    27,    -1,    28,    -1,    59,    60,    61,    -1,    61,
      -1,    29,    -1,    61,    62,    63,    -1,    63,    -1,    30,
      -1,    17,    57,    18,    -1,    64,    -1,    65,    -1,     6,
      -1,     7,    -1,     4,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   134,   134,   141,   145,   141,   169,   172,   177,   182,
     200,   203,   205,   220,   238,   241,   245,   249,   248,   295,
     299,   295,   303,   306,   307,   311,   310,   348,   381,   394,
     407,   422,   422,   444,   452,   455,   473,   476,   494,   497,
     513,   517,   520,   539,   543,   547,   553,   569,   572,   576,
     582,   598,   601,   607,   623,   626,   632,   636,   637,   640,
     655,   672
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "METHODNAME", "PARAMNAME", "VALCASE",
  "VALDOUBLE", "VALINT", "VALSTRING", "VALSTRAT", "VALPARAM", "VALTEST",
  "'/'", "'?'", "';'", "':'", "'|'", "'('", "')'", "'{'", "'}'", "','",
  "'='", "'&'", "'!'", "'<'", "'>'", "'+'", "'-'", "'*'", "'%'", "$accept",
  "STRAT", "STRATTEST", "@1", "@2", "STRATTESTELSE", "STRATSELECT",
  "STRATEMPTY", "STRATCONCAT", "STRATGROUP", "STRATMETHOD", "@3",
  "METHODPARAM", "@4", "@5", "PARAMLIST", "PARAMPARAM", "@6", "PARAMVAL",
  "@7", "TEST", "TESTOR", "TESTAND", "TESTNOT", "TESTREL", "TESTRELOP",
  "TESTEXPR1", "TESTEXPR1OP", "TESTEXPR2", "TESTEXPR2OP", "TESTEXPR3",
  "TESTEXPR3OP", "TESTEXPR4", "TESTVAL", "TESTVAR", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,    47,    63,    59,    58,   124,    40,    41,   123,
     125,    44,    61,    38,    33,    60,    62,    43,    45,    42,
      37
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    31,    32,    34,    35,    33,    33,    36,    36,    37,
      37,    38,    38,    39,    39,    40,    40,    42,    41,    44,
      45,    43,    43,    46,    46,    48,    47,    49,    49,    49,
      49,    50,    49,    49,    51,    52,    52,    53,    53,    54,
      54,    54,    55,    56,    56,    56,    57,    57,    58,    58,
      59,    59,    60,    61,    61,    62,    63,    63,    63,    64,
      64,    65
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     0,     0,     8,     1,     2,     0,     3,
       1,     1,     0,     2,     1,     3,     1,     0,     3,     0,
       0,     5,     0,     3,     1,     0,     4,     1,     1,     1,
       1,     0,     2,     1,     1,     3,     1,     3,     1,     2,
       3,     1,     3,     1,     1,     1,     3,     1,     1,     1,
       3,     1,     1,     3,     1,     1,     3,     1,     1,     1,
       1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
      12,    17,    12,     0,     2,     0,     6,    10,    11,    14,
      16,    22,     0,     1,     0,    12,    13,    18,     0,    15,
      61,    59,    60,     0,     0,     4,    34,    36,    38,    41,
       0,    47,    51,    54,    57,    58,     9,     0,     0,     0,
      39,     0,     0,     0,    44,    43,    45,    48,    49,     0,
       0,    52,     0,    55,     0,    25,    20,    24,    40,    56,
      12,    35,    37,     0,    42,    46,    50,    53,     0,     0,
       0,     8,     0,     0,    23,    21,    12,     0,    33,    27,
      28,    29,    30,    26,    12,     7,     5,    32
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,     3,     4,     5,    41,    77,     6,     7,     8,     9,
      10,    11,    17,    18,    70,    56,    57,    68,    83,    84,
      25,    26,    27,    28,    29,    49,    30,    50,    31,    52,
      32,    54,    33,    34,    35
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -58
static const yysigned_char yypact[] =
{
      17,   -58,    17,    23,   -58,    14,    15,   -58,    13,   -58,
     -58,    20,    24,   -58,     8,    13,   -58,   -58,    32,   -58,
     -58,   -58,   -58,     8,     8,   -58,    43,    37,   -58,   -58,
      30,    33,    31,   -58,   -58,   -58,   -58,    59,    27,    22,
     -58,    51,     8,     8,   -58,   -58,   -58,   -58,   -58,    29,
      29,   -58,    29,   -58,    29,   -58,    44,   -58,   -58,   -58,
      13,    37,   -58,    29,   -17,    33,    31,   -58,    45,    59,
      46,    38,    10,     1,   -58,   -58,    13,    54,   -58,   -58,
     -58,   -58,   -58,   -58,    17,    15,   -58,   -58
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -58,   -58,    -2,   -58,   -58,   -58,   -57,    55,   -58,    61,
     -58,   -58,   -58,   -58,   -58,   -58,     2,   -58,   -58,   -58,
     -58,    49,    34,   -19,   -58,   -58,   -22,   -58,    25,   -58,
      21,   -58,    26,   -58,   -58
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -32
static const yysigned_char yytable[] =
{
      12,    39,    78,    71,   -31,    40,    79,    80,    81,    82,
      47,    48,    20,   -31,    21,    22,     1,   -31,   -31,    85,
       1,   -31,   -31,    13,    62,    23,    14,    64,    59,    -3,
       2,    15,    24,    20,     2,    21,    22,    47,    48,   -19,
      59,    72,    19,    42,    44,    58,    63,    45,    46,    47,
      48,    37,    44,    76,    15,    45,    46,    47,    48,    42,
      43,    53,    51,    55,    60,    69,    75,    73,    86,    16,
      36,    74,    38,    66,     0,    65,    61,     0,     0,     0,
      67,     0,    87
};

static const yysigned_char yycheck[] =
{
       2,    23,     1,    60,     3,    24,     5,     6,     7,     8,
      27,    28,     4,    12,     6,     7,     3,    16,    17,    76,
       3,    20,    21,     0,    43,    17,    12,    49,    18,    12,
      17,    16,    24,     4,    17,     6,     7,    27,    28,    19,
      18,    63,    18,    16,    22,    18,    17,    25,    26,    27,
      28,    19,    22,    15,    16,    25,    26,    27,    28,    16,
      23,    30,    29,     4,    13,    21,    20,    22,    14,     8,
      15,    69,    23,    52,    -1,    50,    42,    -1,    -1,    -1,
      54,    -1,    84
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     3,    17,    32,    33,    34,    37,    38,    39,    40,
      41,    42,    33,     0,    12,    16,    40,    43,    44,    18,
       4,     6,     7,    17,    24,    51,    52,    53,    54,    55,
      57,    59,    61,    63,    64,    65,    38,    19,    52,    57,
      54,    35,    16,    23,    22,    25,    26,    27,    28,    56,
      58,    29,    60,    30,    62,     4,    46,    47,    18,    18,
      13,    53,    54,    17,    57,    59,    61,    63,    48,    21,
      45,    37,    57,    22,    47,    20,    15,    36,     1,     5,
       6,     7,     8,    49,    50,    37,    14,    33
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (N)								\
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (0)
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
              (Loc).first_line, (Loc).first_column,	\
              (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Type, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);


# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  register short int *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;


  yyvsp[0] = yylval;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short int *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short int *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a look-ahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to look-ahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 135 "parser_yy.y"
    {
                parserstratcurr = ((yyvsp[0].STRAT));           /* Save pointer to root of tree */
              }
    break;

  case 3:
#line 141 "parser_yy.y"
    {
                stratParserSelect (VALTEST);      /* Parse parameter tokens */
              }
    break;

  case 4:
#line 145 "parser_yy.y"
    {
                stratParserSelect (VALSTRAT);     /* Parse strategy tokens */
              }
    break;

  case 5:
#line 149 "parser_yy.y"
    {
                Strat *           strat;

                if ((strat = (Strat *) memAlloc (sizeof (Strat))) == NULL) {
                  errorPrint  ("stratParserParse: out of memory (1)");
                  stratExit ((yyvsp[-2].STRAT));
                  if (((yyvsp[-1].STRAT)) != NULL)
                    stratExit ((yyvsp[-1].STRAT));
                  stratTestExit ((yyvsp[-5].TEST));
                  YYABORT;
                }

                strat->tabl               = parserstrattab;
                strat->type               = STRATNODECOND;
                strat->data.cond.test     = ((yyvsp[-5].TEST));
                strat->data.cond.strat[0] = ((yyvsp[-2].STRAT));
                strat->data.cond.strat[1] = ((yyvsp[-1].STRAT));

                ((yyval.STRAT)) = strat;
              }
    break;

  case 7:
#line 173 "parser_yy.y"
    {
                ((yyval.STRAT)) = ((yyvsp[0].STRAT));
              }
    break;

  case 8:
#line 177 "parser_yy.y"
    {
                ((yyval.STRAT)) = NULL;
              }
    break;

  case 9:
#line 183 "parser_yy.y"
    {
                Strat *           strat;

                if ((strat = (Strat *) memAlloc (sizeof (Strat))) == NULL) {
                  errorPrint ("stratParserParse: out of memory (2)");
                  stratExit  ((yyvsp[-2].STRAT));
                  stratExit  ((yyvsp[0].STRAT));
                  YYABORT;
                }

                strat->tabl                 = parserstrattab;
                strat->type                 = STRATNODESELECT;
                strat->data.select.strat[0] = ((yyvsp[-2].STRAT));
                strat->data.select.strat[1] = ((yyvsp[0].STRAT));

                ((yyval.STRAT)) = strat;
              }
    break;

  case 12:
#line 205 "parser_yy.y"
    {
                Strat *           strat;

                if ((strat = (Strat *) memAlloc (sizeof (Strat))) == NULL) {
                  errorPrint ("stratParserParse: out of memory (3)");
                  YYABORT;
                }

                strat->tabl = parserstrattab;
                strat->type = STRATNODEEMPTY;

                ((yyval.STRAT)) = strat;
              }
    break;

  case 13:
#line 221 "parser_yy.y"
    {
                Strat *           strat;

                if ((strat = (Strat *) memAlloc (sizeof (Strat))) == NULL) {
                  errorPrint ("stratParserParse: out of memory (4)");
                  stratExit  ((yyvsp[-1].STRAT));
                  stratExit  ((yyvsp[0].STRAT));
                  YYABORT;
                }

                strat->tabl                 = parserstrattab;
                strat->type                 = STRATNODECONCAT;
                strat->data.concat.strat[0] = ((yyvsp[-1].STRAT));
                strat->data.concat.strat[1] = ((yyvsp[0].STRAT));

                ((yyval.STRAT)) = strat;
              }
    break;

  case 15:
#line 242 "parser_yy.y"
    {
                ((yyval.STRAT)) = ((yyvsp[-1].STRAT));
              }
    break;

  case 17:
#line 249 "parser_yy.y"
    {
                Strat *           strat;
                unsigned int      meth;
                unsigned int      methlen;
                StratMethodTab *  methtab;
                unsigned int      i, j;

                methlen = 0;                      /* No method recognized yet   */
                methtab = parserstrattab->methtab; /* Point to the method table */
                for (i = 0; methtab[i].name != NULL; i ++) {
                  if ((strncasecmp (((yyvsp[0].STRING)),         /* Find longest matching code name */
                       methtab[i].name,
                       j = strlen (methtab[i].name)) == 0) &&
                      (j > methlen)) {
                    meth    = methtab[i].meth;
                    methlen = j;
                  }
                }
                if (methlen == 0) {               /* If method name not known */
                  errorPrint ("stratParserParse: invalid method name (\"%s\", before \"%s\")",
                              ((yyvsp[0].STRING)), stratParserRemain ());
                  YYABORT;
                }
                if ((strat = (Strat *) memAlloc (sizeof (Strat))) == NULL) {
                  errorPrint ("stratParserParse: out of memory (5)");
                  YYABORT;
                }

                strat->tabl             = parserstrattab;
                strat->type             = STRATNODEMETHOD;
                strat->data.method.meth = meth;   /* Set method type         */
                if (methtab[meth].data != NULL)   /* If default values exist */
                  memcpy (&strat->data.method.data, /* Set values to default */
                          methtab[meth].data,
                          sizeof (StratNodeMethodData));

                parserstratcurr = strat;          /* Structure available for parameter processing */
              }
    break;

  case 18:
#line 288 "parser_yy.y"
    {
                ((yyval.STRAT)) = parserstratcurr;           /* Return current structure */
                parserstratcurr = NULL;           /* No current structure     */
              }
    break;

  case 19:
#line 295 "parser_yy.y"
    {
                stratParserSelect (VALPARAM);     /* Parse parameter tokens */
              }
    break;

  case 20:
#line 299 "parser_yy.y"
    {
                stratParserSelect (VALSTRAT);     /* Parse strategy tokens */
              }
    break;

  case 25:
#line 311 "parser_yy.y"
    {
                unsigned int      para;
                unsigned int      paralen;
                StratParamTab *   paratab;
                unsigned int      i, j;

                paralen = 0;                      /* No parameter recognized yet   */
                paratab = parserstrattab->paratab; /* Point to the parameter table */
                for (i = 0; paratab[i].name != NULL; i ++) {
                  if ((paratab[i].meth == parserstratcurr->data.method.meth) &&
                      (strncasecmp (((yyvsp[0].STRING)),         /* Find longest matching parameter name */
                                    paratab[i].name,
                                    j = strlen (paratab[i].name)) == 0) &&
                      (j > paralen)) {
                    para    = i;
                    paralen = j;
                  }
                }
                if (paralen == 0) {
                  errorPrint ("stratParserParse: invalid method parameter name (\"%s\", before \"%s\")",
                              ((yyvsp[0].STRING)), stratParserRemain ());
                  YYABORT;
                }

                ((yyval.SAVE)).tabl = parserstrattab; /* Save current strategy tables */
                parserparamcurr = &paratab[para]; /* Save current parameter value */
                stratParserSelect (parsermethtokentab[parserparamcurr->type]);
                if (parserparamcurr->type == STRATPARAMSTRAT) /* If parameter is a strategy           */
                  parserstrattab = (StratTab *) parserparamcurr->datasltr; /* Use new strategy tables */
              }
    break;

  case 26:
#line 342 "parser_yy.y"
    {
                stratParserSelect (VALPARAM);     /* Go-on reading parameters        */
                parserstrattab = ((yyvsp[-2].SAVE)).tabl; /* Restore current strategy tables */
              }
    break;

  case 27:
#line 349 "parser_yy.y"
    {
                char              c;              /* Character read             */
                char *            p;              /* Pointer to selector string */
                unsigned int      i;              /* Index in selector string   */

                c = ((yyvsp[0].CASEVAL));                         /* First, use char as is */
                for (p = (char *) parserparamcurr->datasltr, i = 0;
                     (*p != '\0') && (*p != c);
                     p ++, i ++) ;
                if (*p == '\0') {                 /* Char was not found         */
                  c = tolower ((yyvsp[0].CASEVAL));               /* Convert char to lower case */
                  for (p = (char *) parserparamcurr->datasltr, i = 0;
                       (*p != '\0') && (*p != c);
                       p ++, i ++) ;
                  if (*p == '\0') {
                    errorPrint ("stratParserParse: invalid method parameter switch (\"%s=%c\", before \"%s\")",
                                parserparamcurr->name, ((yyvsp[0].CASEVAL)), stratParserRemain ());
                    YYABORT;
                  }
                }

#ifdef SCOTCH_DEBUG_PARSER2
                if ((parserparamcurr->dataofft -  parserparamcurr->database + sizeof (int)) > sizeof (StratNodeMethodData)) {
                  errorPrint ("stratParserParse: internal error (1)");
                  YYABORT;
                }
#endif /* SCOTCH_DEBUG_PARSER2 */

                *((int *) ((byte *) &parserstratcurr->data.method.data +
                           (parserparamcurr->dataofft -
                            parserparamcurr->database))) = i;
              }
    break;

  case 28:
#line 382 "parser_yy.y"
    {
#ifdef SCOTCH_DEBUG_PARSER2
                if ((parserparamcurr->dataofft -  parserparamcurr->database + sizeof (double)) > sizeof (StratNodeMethodData)) {
                  errorPrint ("stratParserParse: internal error (2)");
                  YYABORT;
                }
#endif /* SCOTCH_DEBUG_PARSER2 */

                *((double *) ((byte *) &parserstratcurr->data .method.data +
                              (parserparamcurr->dataofft -
                               parserparamcurr->database))) = ((yyvsp[0].DOUBLE));
              }
    break;

  case 29:
#line 395 "parser_yy.y"
    {
#ifdef SCOTCH_DEBUG_PARSER2
                if ((parserparamcurr->dataofft -  parserparamcurr->database + sizeof (INT)) > sizeof (StratNodeMethodData)) {
                  errorPrint ("stratParserParse: internal error (3)");
                  YYABORT;
                }
#endif /* SCOTCH_DEBUG_PARSER2 */

                *((INT *) ((byte *) &parserstratcurr->data.method.data +
                           (parserparamcurr->dataofft -
                            parserparamcurr->database))) = (INT) ((yyvsp[0].INTEGER));
              }
    break;

  case 30:
#line 408 "parser_yy.y"
    {
#ifdef SCOTCH_DEBUG_PARSER2
                if ((parserparamcurr->dataofft -  parserparamcurr->database + strlen ((yyvsp[0].STRING)) + 1) > sizeof (StratNodeMethodData)) {
                  errorPrint ("stratParserParse: internal error (4)");
                  YYABORT;
                }
#endif /* SCOTCH_DEBUG_PARSER2 */

                strcpy ((char *) ((byte *) &parserstratcurr->data.method.data +
                                  (parserparamcurr->dataofft -
                                   parserparamcurr->database)),
                        ((yyvsp[0].STRING)));
              }
    break;

  case 31:
#line 422 "parser_yy.y"
    {
                ((yyval.SAVE)).strat = parserstratcurr;
                ((yyval.SAVE)).param = parserparamcurr;
                parserstratcurr  = NULL;
                parserparamcurr  = NULL;
              }
    break;

  case 32:
#line 429 "parser_yy.y"
    {
                parserstratcurr = ((yyvsp[-1].SAVE)).strat; /* Restore current method    */
                parserparamcurr = ((yyvsp[-1].SAVE)).param; /* Restore current parameter */

#ifdef SCOTCH_DEBUG_PARSER2
                if ((parserparamcurr->dataofft -  parserparamcurr->database + sizeof (Strat *)) > sizeof (StratNodeMethodData)) {
                  errorPrint ("stratParserParse: internal error (5)");
                  YYABORT;
                }
#endif /* SCOTCH_DEBUG_PARSER2 */

                *((Strat **) ((byte *) &parserstratcurr->data.method.data +
                              (parserparamcurr->dataofft -
                               parserparamcurr->database))) = ((yyvsp[0].STRAT));
              }
    break;

  case 33:
#line 445 "parser_yy.y"
    {
                errorPrint ("stratParserParse: invalid value for parameter \"%s\" of method \"%s\" (before \"%s\")",
                            parserparamcurr->name, parserstratcurr->tabl->methtab[parserstratcurr->data.method.meth].name, stratParserRemain ());
                YYABORT;
              }
    break;

  case 35:
#line 456 "parser_yy.y"
    {
                StratTest *       test;

                if ((test = (StratTest *) memAlloc (sizeof (StratTest))) == NULL) {
                  errorPrint    ("stratParserParse: out of memory (6)");
                  stratTestExit ((yyvsp[-2].TEST));
                  stratTestExit ((yyvsp[0].TEST));
                  YYABORT;
                }

                test->typetest     = STRATTESTOR;
                test->typenode     = STRATPARAMLOG;
                test->data.test[0] = ((yyvsp[-2].TEST));
                test->data.test[1] = ((yyvsp[0].TEST));

                ((yyval.TEST)) = test;
              }
    break;

  case 37:
#line 477 "parser_yy.y"
    {
                StratTest *       test;

                if ((test = (StratTest *) memAlloc (sizeof (StratTest))) == NULL) {
                  errorPrint    ("stratParserParse: out of memory (7)");
                  stratTestExit ((yyvsp[-2].TEST));
                  stratTestExit ((yyvsp[0].TEST));
                  YYABORT;
                }

                test->typetest     = STRATTESTAND;
                test->typenode     = STRATPARAMLOG;
                test->data.test[0] = ((yyvsp[-2].TEST));
                test->data.test[1] = ((yyvsp[0].TEST));

                ((yyval.TEST)) = test;
              }
    break;

  case 39:
#line 498 "parser_yy.y"
    {
                StratTest *       test;

                if ((test = (StratTest *) memAlloc (sizeof (StratTest))) == NULL) {
                  errorPrint    ("stratParserParse: out of memory (8)");
                  stratTestExit ((yyvsp[0].TEST));
                  YYABORT;
                }

                test->typetest     = STRATTESTNOT;
                test->typenode     = STRATPARAMLOG;
                test->data.test[0] = ((yyvsp[0].TEST));

                ((yyval.TEST)) = test;
              }
    break;

  case 40:
#line 514 "parser_yy.y"
    {
                ((yyval.TEST)) = ((yyvsp[-1].TEST));
              }
    break;

  case 42:
#line 521 "parser_yy.y"
    {
                StratTest *       test;

                if ((test = (StratTest *) memAlloc (sizeof (StratTest))) == NULL) {
                  errorPrint    ("stratParserParse: out of memory (9)");
                  stratTestExit ((yyvsp[-2].TEST));
                  stratTestExit ((yyvsp[0].TEST));
                  YYABORT;
                }
                test->typetest     = ((yyvsp[-1].TESTOP));
                test->typenode     = STRATPARAMLOG;
                test->data.test[0] = ((yyvsp[-2].TEST));
                test->data.test[1] = ((yyvsp[0].TEST));

                ((yyval.TEST)) = test;
              }
    break;

  case 43:
#line 540 "parser_yy.y"
    {
                ((yyval.TESTOP)) = STRATTESTLT;
              }
    break;

  case 44:
#line 544 "parser_yy.y"
    {
                ((yyval.TESTOP)) = STRATTESTEQ;
              }
    break;

  case 45:
#line 548 "parser_yy.y"
    {
                ((yyval.TESTOP)) = STRATTESTGT;
              }
    break;

  case 46:
#line 554 "parser_yy.y"
    {
                StratTest *       test;

                if ((test = (StratTest *) memAlloc (sizeof (StratTest))) == NULL) {
                  errorPrint    ("stratParserParse: out of memory (10)");
                  stratTestExit ((yyvsp[-2].TEST));
                  stratTestExit ((yyvsp[0].TEST));
                  YYABORT;
                }
                test->typetest     = ((yyvsp[-1].TESTOP));
                test->data.test[0] = ((yyvsp[-2].TEST));
                test->data.test[1] = ((yyvsp[0].TEST));

                ((yyval.TEST)) = test;
              }
    break;

  case 48:
#line 573 "parser_yy.y"
    {
                ((yyval.TESTOP)) = STRATTESTADD;
              }
    break;

  case 49:
#line 577 "parser_yy.y"
    {
                ((yyval.TESTOP)) = STRATTESTSUB;
              }
    break;

  case 50:
#line 583 "parser_yy.y"
    {
                StratTest *       test;

                if ((test = (StratTest *) memAlloc (sizeof (StratTest))) == NULL) {
                  stratTestExit ((yyvsp[-2].TEST));
                  stratTestExit ((yyvsp[0].TEST));
                  errorPrint    ("stratParserParse: out of memory (11)");
                  YYABORT;
                }
                test->typetest     = ((yyvsp[-1].TESTOP));
                test->data.test[0] = ((yyvsp[-2].TEST));
                test->data.test[1] = ((yyvsp[0].TEST));

                ((yyval.TEST)) = test;
              }
    break;

  case 52:
#line 602 "parser_yy.y"
    {
                ((yyval.TESTOP)) = STRATTESTMUL;
              }
    break;

  case 53:
#line 608 "parser_yy.y"
    {
                StratTest *       test;

                if ((test = (StratTest *) memAlloc (sizeof (StratTest))) == NULL) {
                  errorPrint    ("stratParserParse: out of memory (12)");
                  stratTestExit ((yyvsp[-2].TEST));
                  stratTestExit ((yyvsp[0].TEST));
                  YYABORT;
                }
                test->typetest     = ((yyvsp[-1].TESTOP));
                test->data.test[0] = ((yyvsp[-2].TEST));
                test->data.test[1] = ((yyvsp[0].TEST));

                ((yyval.TEST)) = test;
              }
    break;

  case 55:
#line 627 "parser_yy.y"
    {
                ((yyval.TESTOP)) = STRATTESTMOD;
              }
    break;

  case 56:
#line 633 "parser_yy.y"
    {
                ((yyval.TEST)) = ((yyvsp[-1].TEST));
              }
    break;

  case 59:
#line 641 "parser_yy.y"
    {
                StratTest *       test;

                if ((test = (StratTest *) memAlloc (sizeof (StratTest))) == NULL) {
                  errorPrint ("stratParserParse: out of memory (13)");
                  YYABORT;
                }

                test->typetest        = STRATTESTVAL;
                test->typenode        = STRATPARAMDOUBLE;
                test->data.val.valdbl = ((yyvsp[0].DOUBLE));

                ((yyval.TEST)) = test;
              }
    break;

  case 60:
#line 656 "parser_yy.y"
    {
                StratTest *       test;

                if ((test = (StratTest *) memAlloc (sizeof (StratTest))) == NULL) {
                  errorPrint ("stratParserParse: out of memory (14)");
                  YYABORT;
                }

                test->typetest        = STRATTESTVAL;
                test->typenode        = STRATPARAMINT;
                test->data.val.valint = ((yyvsp[0].INTEGER));

                ((yyval.TEST)) = test;
              }
    break;

  case 61:
#line 673 "parser_yy.y"
    {
                StratTest *       test;
                StratParamTab *   condtab;
                unsigned int      para;
                unsigned int      paralen;
                unsigned int      i, j;

                paralen = 0;                      /* No parameter recognized yet */
                condtab = parserstrattab->condtab; /* Point to parameter table   */
                for (i = 0; condtab[i].name != NULL; i ++) {
                  if ((strncasecmp (((yyvsp[0].STRING)),         /* Find longest matching parameter name */
                                    condtab[i].name,
                                    j = strlen (condtab[i].name)) == 0) &&
                      (j > paralen)) {
                    para    = i;
                    paralen = j;
                  }
                }
                if (paralen == 0) {
                  errorPrint ("stratParserParse: invalid graph parameter name (\"%s\", before \"%s\")",
                              ((yyvsp[0].STRING)), stratParserRemain ());
                  YYABORT;
                }

                if ((test = (StratTest *) memAlloc (sizeof (StratTest))) == NULL) {
                  errorPrint ("stratParserParse: out of memory (15)");
                  YYABORT;
                }

                test->typetest          = STRATTESTVAR;
                test->typenode          = condtab[para].type;
                test->data.var.datatab  = parserstrattab;
                test->data.var.datadisp = condtab[para].dataofft -
                                          condtab[para].database;

                ((yyval.TEST)) = test;
              }
    break;


    }

/* Line 1037 of yacc.c.  */
#line 1841 "y.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* If at end of input, pop the error token,
	     then the rest of the stack, then return failure.  */
	  if (yychar == YYEOF)
	     for (;;)
	       {

		 YYPOPSTACK;
		 if (yyssp == yyss)
		   YYABORT;
		 yydestruct ("Error: popping",
                             yystos[*yyssp], yyvsp);
	       }
        }
      else
	{
	  yydestruct ("Error: discarding", yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

#ifdef __GNUC__
  /* Pacify GCC when the user code never invokes YYERROR and the label
     yyerrorlab therefore never appears in user code.  */
  if (0)
     goto yyerrorlab;
#endif

yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping", yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token. */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yydestruct ("Error: discarding lookahead",
              yytoken, &yylval);
  yychar = YYEMPTY;
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 712 "parser_yy.y"


/*
**  The static and global definitions (bis).
**  These are put at the end of the file because
**  the token values that they use are not yet
**  defined in the first section of the file.
*/

unsigned int                parsermethtokentab[] = { /* Table for parameter/token type conversion */
                              VALCASE,
                              VALDOUBLE,
                              VALINT,
                              -1,                 /* No logical parameters */
                              VALSTRAT,
                              VALSTRING,
                              -1                  /* One more value to detect array overflow */
                            };

/************************************/
/*                                  */
/* These routines drive the parser. */
/*                                  */
/************************************/

/* This routine is the entry point for
** the strategy parser.
** It returns:
** - !NULL  : pointer to the strategy.
** - NULL   : on error.
*/

Strat *
stratParserParse (
const StratTab * const      strattab,             /*+ Pointer to parsing tables +*/
const char * const          string)               /*+ Strategy string to parse  +*/
{
  yyclearin;                                      /* Reset the parser state */

#ifdef SCOTCH_DEBUG_PARSER3
  yydebug = 1;                                    /* Set debugging if needed */
#endif /* SCOTCH_DEBUG_PARSER3 */

  stratParserInit (string);                       /* Initialize the lexical parser           */
  parserstrattab  = strattab;                     /* Point to the parsing tables             */
  parserstratcurr = NULL;                         /* Clear up the temporary strategy pointer */

  if (stratParserParse2 () != 0) {                /* Parse the strategy string */
    if (parserstratcurr != NULL)
      stratExit (parserstratcurr);
    return (NULL);
  }

  return (parserstratcurr);                       /* Return strategy pointer */
}

/* This routine displays the parser error message.
** It returns:
** - 1  : in all cases.
*/

static
int
stratParserError (
const char * const          errstr)
{
  errorPrint ("stratParserParse: invalid strategy string (before \"%s\")", stratParserRemain ());
  return     (1);
}

