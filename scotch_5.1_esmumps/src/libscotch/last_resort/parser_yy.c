/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

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
/* Tokens.  */
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

/* Copyright 2004,2007,2008 ENSEIRB, INRIA & CNRS
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
/**                # Version 5.1  : from : 30 oct 2007     **/
/**                                 to     22 oct 2008     **/
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

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 96 "parser_yy.y"
{
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
}
/* Line 187 of yacc.c.  */
#line 228 "y.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 241 "y.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
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
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  13
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   82

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  31
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  35
/* YYNRULES -- Number of rules.  */
#define YYNRULES  61
/* YYNRULES -- Number of states.  */
#define YYNSTATES  88

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   266

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    24,     2,     2,     2,    30,    23,     2,
      17,    18,    29,    27,    21,    28,     2,    13,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    16,    15,
      25,    22,    26,    14,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    19,    12,    20,     2,     2,     2,     2,
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
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     9,    11,    13,    14,    17,    19,
      20,    21,    30,    32,    35,    36,    40,    42,    43,    47,
      48,    49,    55,    56,    60,    62,    63,    68,    70,    72,
      74,    76,    77,    80,    82,    84,    88,    90,    94,    96,
      99,   103,   105,   109,   111,   113,   115,   119,   121,   123,
     125,   129,   131,   133,   137,   139,   141,   145,   147,   149,
     151,   153
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      32,     0,    -1,    33,    -1,    33,    12,    34,    -1,    34,
      -1,    35,    -1,    -1,    35,    36,    -1,    36,    -1,    -1,
      -1,    37,    13,    51,    38,    14,    33,    39,    15,    -1,
      40,    -1,    16,    33,    -1,    -1,    17,    33,    18,    -1,
      41,    -1,    -1,     3,    42,    43,    -1,    -1,    -1,    44,
      19,    46,    45,    20,    -1,    -1,    46,    21,    47,    -1,
      47,    -1,    -1,     4,    48,    22,    49,    -1,     5,    -1,
       6,    -1,     7,    -1,     8,    -1,    -1,    50,    33,    -1,
       1,    -1,    52,    -1,    52,    12,    53,    -1,    53,    -1,
      53,    23,    54,    -1,    54,    -1,    24,    54,    -1,    17,
      52,    18,    -1,    55,    -1,    57,    56,    57,    -1,    25,
      -1,    22,    -1,    26,    -1,    57,    58,    59,    -1,    59,
      -1,    27,    -1,    28,    -1,    59,    60,    61,    -1,    61,
      -1,    29,    -1,    61,    62,    63,    -1,    63,    -1,    30,
      -1,    17,    57,    18,    -1,    64,    -1,    65,    -1,     6,
      -1,     7,    -1,     4,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   136,   136,   142,   160,   163,   165,   180,   198,   202,
     206,   202,   230,   233,   238,   243,   247,   251,   250,   312,
     316,   312,   320,   323,   324,   328,   327,   366,   399,   412,
     425,   440,   440,   462,   470,   473,   491,   494,   512,   515,
     531,   535,   538,   557,   561,   565,   571,   587,   590,   594,
     600,   616,   619,   625,   641,   644,   650,   654,   655,   658,
     673,   690
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "METHODNAME", "PARAMNAME", "VALCASE",
  "VALDOUBLE", "VALINT", "VALSTRING", "VALSTRAT", "VALPARAM", "VALTEST",
  "'|'", "'/'", "'?'", "';'", "':'", "'('", "')'", "'{'", "'}'", "','",
  "'='", "'&'", "'!'", "'<'", "'>'", "'+'", "'-'", "'*'", "'%'", "$accept",
  "STRAT", "STRATSELECT", "STRATEMPTY", "STRATCONCAT", "STRATTEST", "@1",
  "@2", "STRATTESTELSE", "STRATGROUP", "STRATMETHOD", "@3", "METHODPARAM",
  "@4", "@5", "PARAMLIST", "PARAMPARAM", "@6", "PARAMVAL", "@7", "TEST",
  "TESTOR", "TESTAND", "TESTNOT", "TESTREL", "TESTRELOP", "TESTEXPR1",
  "TESTEXPR1OP", "TESTEXPR2", "TESTEXPR2OP", "TESTEXPR3", "TESTEXPR3OP",
  "TESTEXPR4", "TESTVAL", "TESTVAR", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   124,    47,    63,    59,    58,    40,    41,   123,
     125,    44,    61,    38,    33,    60,    62,    43,    45,    42,
      37
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    31,    32,    33,    33,    34,    34,    35,    35,    37,
      38,    36,    36,    39,    39,    40,    40,    42,    41,    44,
      45,    43,    43,    46,    46,    48,    47,    49,    49,    49,
      49,    50,    49,    49,    51,    52,    52,    53,    53,    54,
      54,    54,    55,    56,    56,    56,    57,    57,    58,    58,
      59,    59,    60,    61,    61,    62,    63,    63,    63,    64,
      64,    65
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     3,     1,     1,     0,     2,     1,     0,
       0,     8,     1,     2,     0,     3,     1,     0,     3,     0,
       0,     5,     0,     3,     1,     0,     4,     1,     1,     1,
       1,     0,     2,     1,     1,     3,     1,     3,     1,     2,
       3,     1,     3,     1,     1,     1,     3,     1,     1,     1,
       3,     1,     1,     3,     1,     1,     3,     1,     1,     1,
       1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       6,    17,     6,     0,     2,     4,     5,     8,     0,    12,
      16,    22,     0,     1,     6,     7,     0,    18,     0,    15,
       3,    61,    59,    60,     0,     0,    10,    34,    36,    38,
      41,     0,    47,    51,    54,    57,    58,     0,     0,     0,
      39,     0,     0,     0,    44,    43,    45,    48,    49,     0,
       0,    52,     0,    55,     0,    25,    20,    24,    40,    56,
       6,    35,    37,     0,    42,    46,    50,    53,     0,     0,
       0,    14,     0,     0,    23,    21,     6,     0,    33,    27,
      28,    29,    30,    26,     6,    13,    11,    32
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     3,     4,     5,     6,     7,     8,    41,    77,     9,
      10,    11,    17,    18,    70,    56,    57,    68,    83,    84,
      26,    27,    28,    29,    30,    49,    31,    50,    32,    52,
      33,    54,    34,    35,    36
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -24
static const yytype_int8 yypact[] =
{
      12,   -24,    12,     3,    15,   -24,    12,   -24,    11,   -24,
     -24,     9,    31,   -24,    12,   -24,    13,   -24,    16,   -24,
     -24,   -24,   -24,   -24,    13,    13,   -24,    24,    29,   -24,
     -24,    34,    10,     2,   -24,   -24,   -24,    49,    45,    20,
     -24,    40,    13,    13,   -24,   -24,   -24,   -24,   -24,    27,
      27,   -24,    27,   -24,    27,   -24,    43,   -24,   -24,   -24,
      12,    29,   -24,    27,   -17,    10,     2,   -24,    33,    49,
      46,     0,    23,     1,   -24,   -24,    12,    50,   -24,   -24,
     -24,   -24,   -24,   -24,    12,    15,   -24,    15
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -24,   -24,    -2,    53,   -24,    62,   -24,   -24,   -24,   -24,
     -24,   -24,   -24,   -24,   -24,   -24,     4,   -24,   -24,   -24,
     -24,    47,    28,   -20,   -24,   -24,   -23,   -24,    19,   -24,
      25,   -24,    18,   -24,   -24
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -32
static const yytype_int8 yytable[] =
{
      12,    39,    78,    13,   -31,    40,    79,    80,    81,    82,
      47,    48,    14,   -31,   -31,     1,    76,    21,   -31,    22,
      23,   -31,   -31,    62,    16,    -9,    64,    14,   -19,     2,
      24,    21,    53,    22,    23,    37,    42,    25,    59,    51,
      72,    59,    44,    14,    63,    45,    46,    47,    48,    19,
      47,    48,    43,    55,    60,    73,    44,    42,    71,    45,
      46,    47,    48,    58,    69,    86,    75,    20,    15,    65,
      61,    38,    67,    74,    85,     0,     0,    66,     0,     0,
       0,     0,    87
};

static const yytype_int8 yycheck[] =
{
       2,    24,     1,     0,     3,    25,     5,     6,     7,     8,
      27,    28,    12,    12,    13,     3,    16,     4,    17,     6,
       7,    20,    21,    43,    13,    13,    49,    12,    19,    17,
      17,     4,    30,     6,     7,    19,    12,    24,    18,    29,
      63,    18,    22,    12,    17,    25,    26,    27,    28,    18,
      27,    28,    23,     4,    14,    22,    22,    12,    60,    25,
      26,    27,    28,    18,    21,    15,    20,    14,     6,    50,
      42,    24,    54,    69,    76,    -1,    -1,    52,    -1,    -1,
      -1,    -1,    84
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,    17,    32,    33,    34,    35,    36,    37,    40,
      41,    42,    33,     0,    12,    36,    13,    43,    44,    18,
      34,     4,     6,     7,    17,    24,    51,    52,    53,    54,
      55,    57,    59,    61,    63,    64,    65,    19,    52,    57,
      54,    38,    12,    23,    22,    25,    26,    27,    28,    56,
      58,    29,    60,    30,    62,     4,    46,    47,    18,    18,
      14,    53,    54,    17,    57,    59,    61,    63,    48,    21,
      45,    33,    57,    22,    47,    20,    16,    39,     1,     5,
       6,     7,     8,    49,    50,    33,    15,    33
};

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
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
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
    while (YYID (0))
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
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

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
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
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
  YYUSE (yyvaluep);

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
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
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
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

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

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
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

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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
#line 137 "parser_yy.y"
    {
                parserstratcurr = ((yyvsp[(1) - (1)].STRAT));           /* Save pointer to root of tree */
              }
    break;

  case 3:
#line 143 "parser_yy.y"
    {
                Strat *           strat;

                if ((strat = (Strat *) memAlloc (sizeof (Strat))) == NULL) {
                  errorPrint ("stratParserParse: out of memory (2)");
                  stratExit  ((yyvsp[(1) - (3)].STRAT));
                  stratExit  ((yyvsp[(3) - (3)].STRAT));
                  YYABORT;
                }

                strat->tabl                 = parserstrattab;
                strat->type                 = STRATNODESELECT;
                strat->data.select.strat[0] = ((yyvsp[(1) - (3)].STRAT));
                strat->data.select.strat[1] = ((yyvsp[(3) - (3)].STRAT));

                ((yyval.STRAT)) = strat;
              }
    break;

  case 6:
#line 165 "parser_yy.y"
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

  case 7:
#line 181 "parser_yy.y"
    {
                Strat *           strat;

                if ((strat = (Strat *) memAlloc (sizeof (Strat))) == NULL) {
                  errorPrint ("stratParserParse: out of memory (4)");
                  stratExit  ((yyvsp[(1) - (2)].STRAT));
                  stratExit  ((yyvsp[(2) - (2)].STRAT));
                  YYABORT;
                }

                strat->tabl                 = parserstrattab;
                strat->type                 = STRATNODECONCAT;
                strat->data.concat.strat[0] = ((yyvsp[(1) - (2)].STRAT));
                strat->data.concat.strat[1] = ((yyvsp[(2) - (2)].STRAT));

                ((yyval.STRAT)) = strat;
              }
    break;

  case 9:
#line 202 "parser_yy.y"
    {
                stratParserSelect (VALTEST);      /* Parse parameter tokens */
              }
    break;

  case 10:
#line 206 "parser_yy.y"
    {
                stratParserSelect (VALSTRAT);     /* Parse strategy tokens */
              }
    break;

  case 11:
#line 210 "parser_yy.y"
    {
                Strat *           strat;

                if ((strat = (Strat *) memAlloc (sizeof (Strat))) == NULL) {
                  errorPrint  ("stratParserParse: out of memory (1)");
                  stratExit ((yyvsp[(6) - (8)].STRAT));
                  if (((yyvsp[(7) - (8)].STRAT)) != NULL)
                    stratExit ((yyvsp[(7) - (8)].STRAT));
                  stratTestExit ((yyvsp[(3) - (8)].TEST));
                  YYABORT;
                }

                strat->tabl               = parserstrattab;
                strat->type               = STRATNODECOND;
                strat->data.cond.test     = ((yyvsp[(3) - (8)].TEST));
                strat->data.cond.strat[0] = ((yyvsp[(6) - (8)].STRAT));
                strat->data.cond.strat[1] = ((yyvsp[(7) - (8)].STRAT));

                ((yyval.STRAT)) = strat;
              }
    break;

  case 13:
#line 234 "parser_yy.y"
    {
                ((yyval.STRAT)) = ((yyvsp[(2) - (2)].STRAT));
              }
    break;

  case 14:
#line 238 "parser_yy.y"
    {
                ((yyval.STRAT)) = NULL;
              }
    break;

  case 15:
#line 244 "parser_yy.y"
    {
                ((yyval.STRAT)) = ((yyvsp[(2) - (3)].STRAT));
              }
    break;

  case 17:
#line 251 "parser_yy.y"
    {
                Strat *           strat;
                int               meth;
                int               methlen;
                StratMethodTab *  methtab;
                int               i, j;

                meth    =
                methlen = 0;                      /* No method recognized yet   */
                methtab = parserstrattab->methtab; /* Point to the method table */
                for (i = 0; methtab[i].name != NULL; i ++) {
                  if ((strncasecmp (((yyvsp[(1) - (1)].STRING)),         /* Find longest matching code name */
                       methtab[i].name,
                       j = strlen (methtab[i].name)) == 0) &&
                      (j > methlen)) {
                    meth    = methtab[i].meth;
                    methlen = j;
                  }
                }
                if (methlen == 0) {               /* If method name not known */
                  errorPrint ("stratParserParse: invalid method name (\"%s\", before \"%s\")",
                              ((yyvsp[(1) - (1)].STRING)), stratParserRemain ());
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
#line 291 "parser_yy.y"
    {
                StratParamTab *   paratab;
                int               i;

                paratab = parserstrattab->paratab; /* Point to the parameter table */
                for (i = 0; paratab[i].name != NULL; i ++) {
                  if ((paratab[i].meth == parserstratcurr->data.method.meth) && /* If a strategy parameter found for this method */
                      (paratab[i].type == STRATPARAMSTRAT)) {
                    if (*((Strat **) ((byte *) &parserstratcurr->data.method.data + /* And this parameter has not been set */
                        (paratab[i].dataofft - paratab[i].database))) == NULL)
                      errorPrintW ("stratParserParse: strategy parameter \"%s\" of method \"%s\" not set, before \"%s\"",
                                   paratab[i].name, parserstrattab->methtab[parserstratcurr->data.method.meth].name, stratParserRemain ());
                  }
                }

                ((yyval.STRAT)) = parserstratcurr;           /* Return current structure */
                parserstratcurr = NULL;           /* No current structure     */
              }
    break;

  case 19:
#line 312 "parser_yy.y"
    {
                stratParserSelect (VALPARAM);     /* Parse parameter tokens */
              }
    break;

  case 20:
#line 316 "parser_yy.y"
    {
                stratParserSelect (VALSTRAT);     /* Parse strategy tokens */
              }
    break;

  case 25:
#line 328 "parser_yy.y"
    {
                int               para;
                int               paralen;
                StratParamTab *   paratab;
                int               i, j;

                para    =
                paralen = 0;                      /* No parameter recognized yet   */
                paratab = parserstrattab->paratab; /* Point to the parameter table */
                for (i = 0; paratab[i].name != NULL; i ++) {
                  if ((paratab[i].meth == parserstratcurr->data.method.meth) &&
                      (strncasecmp (((yyvsp[(1) - (1)].STRING)),         /* Find longest matching parameter name */
                                    paratab[i].name,
                                    j = strlen (paratab[i].name)) == 0) &&
                      (j > paralen)) {
                    para    = i;
                    paralen = j;
                  }
                }
                if (paralen == 0) {
                  errorPrint ("stratParserParse: invalid method parameter name (\"%s\", before \"%s\")",
                              ((yyvsp[(1) - (1)].STRING)), stratParserRemain ());
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
#line 360 "parser_yy.y"
    {
                stratParserSelect (VALPARAM);     /* Go-on reading parameters        */
                parserstrattab = ((yyvsp[(2) - (4)].SAVE)).tabl; /* Restore current strategy tables */
              }
    break;

  case 27:
#line 367 "parser_yy.y"
    {
                char              c;              /* Character read             */
                char *            p;              /* Pointer to selector string */
                int               i;              /* Index in selector string   */

                c = ((yyvsp[(1) - (1)].CASEVAL));                         /* First, use char as is */
                for (p = (char *) parserparamcurr->datasltr, i = 0;
                     (*p != '\0') && (*p != c);
                     p ++, i ++) ;
                if (*p == '\0') {                 /* Char was not found         */
                  c = tolower (c);                /* Convert char to lower case */
                  for (p = (char *) parserparamcurr->datasltr, i = 0;
                       (*p != '\0') && (*p != c);
                       p ++, i ++) ;
                  if (*p == '\0') {
                    errorPrint ("stratParserParse: invalid method parameter switch (\"%s=%c\", before \"%s\")",
                                parserparamcurr->name, ((yyvsp[(1) - (1)].CASEVAL)), stratParserRemain ());
                    YYABORT;
                  }
                }

#ifdef SCOTCH_DEBUG_PARSER2
                if ((parserparamcurr->dataofft - parserparamcurr->database + sizeof (int)) > sizeof (StratNodeMethodData)) {
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
#line 400 "parser_yy.y"
    {
#ifdef SCOTCH_DEBUG_PARSER2
                if ((parserparamcurr->dataofft - parserparamcurr->database + sizeof (double)) > sizeof (StratNodeMethodData)) {
                  errorPrint ("stratParserParse: internal error (2)");
                  YYABORT;
                }
#endif /* SCOTCH_DEBUG_PARSER2 */

                *((double *) ((byte *) &parserstratcurr->data.method.data +
                              (parserparamcurr->dataofft -
                               parserparamcurr->database))) = ((yyvsp[(1) - (1)].DOUBLE));
              }
    break;

  case 29:
#line 413 "parser_yy.y"
    {
#ifdef SCOTCH_DEBUG_PARSER2
                if ((parserparamcurr->dataofft - parserparamcurr->database + sizeof (INT)) > sizeof (StratNodeMethodData)) {
                  errorPrint ("stratParserParse: internal error (3)");
                  YYABORT;
                }
#endif /* SCOTCH_DEBUG_PARSER2 */

                *((INT *) ((byte *) &parserstratcurr->data.method.data +
                           (parserparamcurr->dataofft -
                            parserparamcurr->database))) = (INT) ((yyvsp[(1) - (1)].INTEGER));
              }
    break;

  case 30:
#line 426 "parser_yy.y"
    {
#ifdef SCOTCH_DEBUG_PARSER2
                if ((parserparamcurr->dataofft - parserparamcurr->database + strlen ((yyvsp[(1) - (1)].STRING)) + 1) > sizeof (StratNodeMethodData)) {
                  errorPrint ("stratParserParse: internal error (4)");
                  YYABORT;
                }
#endif /* SCOTCH_DEBUG_PARSER2 */

                strcpy ((char *) ((byte *) &parserstratcurr->data.method.data +
                                  (parserparamcurr->dataofft -
                                   parserparamcurr->database)),
                        ((yyvsp[(1) - (1)].STRING)));
              }
    break;

  case 31:
#line 440 "parser_yy.y"
    {
                ((yyval.SAVE)).strat = parserstratcurr;
                ((yyval.SAVE)).param = parserparamcurr;
                parserstratcurr  = NULL;
                parserparamcurr  = NULL;
              }
    break;

  case 32:
#line 447 "parser_yy.y"
    {
                parserstratcurr = ((yyvsp[(1) - (2)].SAVE)).strat; /* Restore current method    */
                parserparamcurr = ((yyvsp[(1) - (2)].SAVE)).param; /* Restore current parameter */

#ifdef SCOTCH_DEBUG_PARSER2
                if ((parserparamcurr->dataofft - parserparamcurr->database + sizeof (Strat *)) > sizeof (StratNodeMethodData)) {
                  errorPrint ("stratParserParse: internal error (5)");
                  YYABORT;
                }
#endif /* SCOTCH_DEBUG_PARSER2 */

                *((Strat **) ((byte *) &parserstratcurr->data.method.data +
                              (parserparamcurr->dataofft -
                               parserparamcurr->database))) = ((yyvsp[(2) - (2)].STRAT));
              }
    break;

  case 33:
#line 463 "parser_yy.y"
    {
                errorPrint ("stratParserParse: invalid value for parameter \"%s\" of method \"%s\" (before \"%s\")",
                            parserparamcurr->name, parserstratcurr->tabl->methtab[parserstratcurr->data.method.meth].name, stratParserRemain ());
                YYABORT;
              }
    break;

  case 35:
#line 474 "parser_yy.y"
    {
                StratTest *       test;

                if ((test = (StratTest *) memAlloc (sizeof (StratTest))) == NULL) {
                  errorPrint    ("stratParserParse: out of memory (6)");
                  stratTestExit ((yyvsp[(1) - (3)].TEST));
                  stratTestExit ((yyvsp[(3) - (3)].TEST));
                  YYABORT;
                }

                test->typetest     = STRATTESTOR;
                test->typenode     = STRATPARAMLOG;
                test->data.test[0] = ((yyvsp[(1) - (3)].TEST));
                test->data.test[1] = ((yyvsp[(3) - (3)].TEST));

                ((yyval.TEST)) = test;
              }
    break;

  case 37:
#line 495 "parser_yy.y"
    {
                StratTest *       test;

                if ((test = (StratTest *) memAlloc (sizeof (StratTest))) == NULL) {
                  errorPrint    ("stratParserParse: out of memory (7)");
                  stratTestExit ((yyvsp[(1) - (3)].TEST));
                  stratTestExit ((yyvsp[(3) - (3)].TEST));
                  YYABORT;
                }

                test->typetest     = STRATTESTAND;
                test->typenode     = STRATPARAMLOG;
                test->data.test[0] = ((yyvsp[(1) - (3)].TEST));
                test->data.test[1] = ((yyvsp[(3) - (3)].TEST));

                ((yyval.TEST)) = test;
              }
    break;

  case 39:
#line 516 "parser_yy.y"
    {
                StratTest *       test;

                if ((test = (StratTest *) memAlloc (sizeof (StratTest))) == NULL) {
                  errorPrint    ("stratParserParse: out of memory (8)");
                  stratTestExit ((yyvsp[(2) - (2)].TEST));
                  YYABORT;
                }

                test->typetest     = STRATTESTNOT;
                test->typenode     = STRATPARAMLOG;
                test->data.test[0] = ((yyvsp[(2) - (2)].TEST));

                ((yyval.TEST)) = test;
              }
    break;

  case 40:
#line 532 "parser_yy.y"
    {
                ((yyval.TEST)) = ((yyvsp[(2) - (3)].TEST));
              }
    break;

  case 42:
#line 539 "parser_yy.y"
    {
                StratTest *       test;

                if ((test = (StratTest *) memAlloc (sizeof (StratTest))) == NULL) {
                  errorPrint    ("stratParserParse: out of memory (9)");
                  stratTestExit ((yyvsp[(1) - (3)].TEST));
                  stratTestExit ((yyvsp[(3) - (3)].TEST));
                  YYABORT;
                }
                test->typetest     = ((yyvsp[(2) - (3)].TESTOP));
                test->typenode     = STRATPARAMLOG;
                test->data.test[0] = ((yyvsp[(1) - (3)].TEST));
                test->data.test[1] = ((yyvsp[(3) - (3)].TEST));

                ((yyval.TEST)) = test;
              }
    break;

  case 43:
#line 558 "parser_yy.y"
    {
                ((yyval.TESTOP)) = STRATTESTLT;
              }
    break;

  case 44:
#line 562 "parser_yy.y"
    {
                ((yyval.TESTOP)) = STRATTESTEQ;
              }
    break;

  case 45:
#line 566 "parser_yy.y"
    {
                ((yyval.TESTOP)) = STRATTESTGT;
              }
    break;

  case 46:
#line 572 "parser_yy.y"
    {
                StratTest *       test;

                if ((test = (StratTest *) memAlloc (sizeof (StratTest))) == NULL) {
                  errorPrint    ("stratParserParse: out of memory (10)");
                  stratTestExit ((yyvsp[(1) - (3)].TEST));
                  stratTestExit ((yyvsp[(3) - (3)].TEST));
                  YYABORT;
                }
                test->typetest     = ((yyvsp[(2) - (3)].TESTOP));
                test->data.test[0] = ((yyvsp[(1) - (3)].TEST));
                test->data.test[1] = ((yyvsp[(3) - (3)].TEST));

                ((yyval.TEST)) = test;
              }
    break;

  case 48:
#line 591 "parser_yy.y"
    {
                ((yyval.TESTOP)) = STRATTESTADD;
              }
    break;

  case 49:
#line 595 "parser_yy.y"
    {
                ((yyval.TESTOP)) = STRATTESTSUB;
              }
    break;

  case 50:
#line 601 "parser_yy.y"
    {
                StratTest *       test;

                if ((test = (StratTest *) memAlloc (sizeof (StratTest))) == NULL) {
                  stratTestExit ((yyvsp[(1) - (3)].TEST));
                  stratTestExit ((yyvsp[(3) - (3)].TEST));
                  errorPrint    ("stratParserParse: out of memory (11)");
                  YYABORT;
                }
                test->typetest     = ((yyvsp[(2) - (3)].TESTOP));
                test->data.test[0] = ((yyvsp[(1) - (3)].TEST));
                test->data.test[1] = ((yyvsp[(3) - (3)].TEST));

                ((yyval.TEST)) = test;
              }
    break;

  case 52:
#line 620 "parser_yy.y"
    {
                ((yyval.TESTOP)) = STRATTESTMUL;
              }
    break;

  case 53:
#line 626 "parser_yy.y"
    {
                StratTest *       test;

                if ((test = (StratTest *) memAlloc (sizeof (StratTest))) == NULL) {
                  errorPrint    ("stratParserParse: out of memory (12)");
                  stratTestExit ((yyvsp[(1) - (3)].TEST));
                  stratTestExit ((yyvsp[(3) - (3)].TEST));
                  YYABORT;
                }
                test->typetest     = ((yyvsp[(2) - (3)].TESTOP));
                test->data.test[0] = ((yyvsp[(1) - (3)].TEST));
                test->data.test[1] = ((yyvsp[(3) - (3)].TEST));

                ((yyval.TEST)) = test;
              }
    break;

  case 55:
#line 645 "parser_yy.y"
    {
                ((yyval.TESTOP)) = STRATTESTMOD;
              }
    break;

  case 56:
#line 651 "parser_yy.y"
    {
                ((yyval.TEST)) = ((yyvsp[(2) - (3)].TEST));
              }
    break;

  case 59:
#line 659 "parser_yy.y"
    {
                StratTest *       test;

                if ((test = (StratTest *) memAlloc (sizeof (StratTest))) == NULL) {
                  errorPrint ("stratParserParse: out of memory (13)");
                  YYABORT;
                }

                test->typetest        = STRATTESTVAL;
                test->typenode        = STRATPARAMDOUBLE;
                test->data.val.valdbl = ((yyvsp[(1) - (1)].DOUBLE));

                ((yyval.TEST)) = test;
              }
    break;

  case 60:
#line 674 "parser_yy.y"
    {
                StratTest *       test;

                if ((test = (StratTest *) memAlloc (sizeof (StratTest))) == NULL) {
                  errorPrint ("stratParserParse: out of memory (14)");
                  YYABORT;
                }

                test->typetest        = STRATTESTVAL;
                test->typenode        = STRATPARAMINT;
                test->data.val.valint = ((yyvsp[(1) - (1)].INTEGER));

                ((yyval.TEST)) = test;
              }
    break;

  case 61:
#line 691 "parser_yy.y"
    {
                StratTest *       test;
                StratParamTab *   condtab;
                int               para;
                int               paralen;
                int               i, j;

                para    =
                paralen = 0;                      /* No parameter recognized yet */
                condtab = parserstrattab->condtab; /* Point to parameter table   */
                for (i = 0; condtab[i].name != NULL; i ++) {
                  if ((strncasecmp (((yyvsp[(1) - (1)].STRING)),         /* Find longest matching parameter name */
                                    condtab[i].name,
                                    j = strlen (condtab[i].name)) == 0) &&
                      (j > paralen)) {
                    para    = i;
                    paralen = j;
                  }
                }
                if (paralen == 0) {
                  errorPrint ("stratParserParse: invalid graph parameter name (\"%s\", before \"%s\")",
                              ((yyvsp[(1) - (1)].STRING)), stratParserRemain ());
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


/* Line 1267 of yacc.c.  */
#line 2166 "y.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
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
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
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

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
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


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
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
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 731 "parser_yy.y"


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

