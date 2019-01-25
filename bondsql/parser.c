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

/* Substitute the variable and function names.  */
#define yyparse sqlparse
#define yylex   sqllex
#define yyerror sqlerror
#define yylval  sqllval
#define yychar  sqlchar
#define yydebug sqldebug
#define yynerrs sqlnerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     L_SELECT = 258,
     L_FROM = 259,
     L_WHERE = 260,
     L_AS = 261,
     L_ON = 262,
     L_ORDER = 263,
     L_BY = 264,
     L_ORDER_ASC = 265,
     L_ORDER_DSC = 266,
     L_DISTINCT = 267,
     L_BETWEEN = 268,
     L_IN = 269,
     L_GROUP = 270,
     L_INSERT = 271,
     L_INTO = 272,
     L_VALUES = 273,
     L_UPDATE = 274,
     L_SET = 275,
     L_DOT = 276,
     L_COMMA = 277,
     L_NULL = 278,
     L_LBRACKET = 279,
     L_RBRACKET = 280,
     L_IDENT = 281,
     L_EQ = 282,
     L_IS = 283,
     L_LIKE = 284,
     L_ILIKE = 285,
     L_GT = 286,
     L_LT = 287,
     L_GEQ = 288,
     L_LEQ = 289,
     L_DIFF = 290,
     L_REGEXP = 291,
     L_REGEXP_CI = 292,
     L_NOTREGEXP = 293,
     L_NOTREGEXP_CI = 294,
     L_SIMILAR = 295,
     L_NOT = 296,
     L_AND = 297,
     L_OR = 298,
     L_MINUS = 299,
     L_PLUS = 300,
     L_TIMES = 301,
     L_DIV = 302,
     L_STRING = 303,
     L_TEXTUAL = 304,
     L_DELETE = 305,
     L_JOIN = 306,
     L_INNER = 307,
     L_LEFT = 308,
     L_RIGHT = 309,
     L_FULL = 310,
     L_OUTER = 311,
     L_LSBRACKET = 312,
     L_RSBRACKET = 313,
     L_PNAME = 314,
     L_PTYPE = 315,
     L_PISPARAM = 316,
     L_PDESCR = 317,
     L_PNULLOK = 318,
     L_UNSPECVAL = 319
   };
#endif
/* Tokens.  */
#define L_SELECT 258
#define L_FROM 259
#define L_WHERE 260
#define L_AS 261
#define L_ON 262
#define L_ORDER 263
#define L_BY 264
#define L_ORDER_ASC 265
#define L_ORDER_DSC 266
#define L_DISTINCT 267
#define L_BETWEEN 268
#define L_IN 269
#define L_GROUP 270
#define L_INSERT 271
#define L_INTO 272
#define L_VALUES 273
#define L_UPDATE 274
#define L_SET 275
#define L_DOT 276
#define L_COMMA 277
#define L_NULL 278
#define L_LBRACKET 279
#define L_RBRACKET 280
#define L_IDENT 281
#define L_EQ 282
#define L_IS 283
#define L_LIKE 284
#define L_ILIKE 285
#define L_GT 286
#define L_LT 287
#define L_GEQ 288
#define L_LEQ 289
#define L_DIFF 290
#define L_REGEXP 291
#define L_REGEXP_CI 292
#define L_NOTREGEXP 293
#define L_NOTREGEXP_CI 294
#define L_SIMILAR 295
#define L_NOT 296
#define L_AND 297
#define L_OR 298
#define L_MINUS 299
#define L_PLUS 300
#define L_TIMES 301
#define L_DIV 302
#define L_STRING 303
#define L_TEXTUAL 304
#define L_DELETE 305
#define L_JOIN 306
#define L_INNER 307
#define L_LEFT 308
#define L_RIGHT 309
#define L_FULL 310
#define L_OUTER 311
#define L_LSBRACKET 312
#define L_RSBRACKET 313
#define L_PNAME 314
#define L_PTYPE 315
#define L_PISPARAM 316
#define L_PDESCR 317
#define L_PNULLOK 318
#define L_UNSPECVAL 319




/* Copy the first part of user declarations.  */
#line 1 "bondsql/parser.y"

#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include <string.h>

#include "sql_parser.h"
#include "sql_tree.h"
#include "bcmem.h"

int sqlerror (char *);
int sqllex ();

extern sql_statement *sql_result;


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
#line 17 "bondsql/parser.y"
{
void            *v;
char            *str;
int              i;
GList           *list;
sql_field       *f;
sql_field_item  *fi;
sql_table       *t;
sql_condition   *c;
sql_where       *w;
param_spec      *ps;
sql_order_field *of;
}
/* Line 187 of yacc.c.  */
#line 262 "bondsql/parser.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 275 "bondsql/parser.c"

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
#define YYFINAL  17
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   242

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  65
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  33
/* YYNRULES -- Number of rules.  */
#define YYNRULES  111
/* YYNRULES -- Number of states.  */
#define YYNSTATES  199

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   319

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    11,    20,    27,    36,
      42,    47,    49,    53,    57,    58,    60,    61,    64,    65,
      69,    70,    72,    76,    78,    81,    84,    88,    89,    91,
      95,    98,   102,   105,   109,   112,   116,   119,   121,   123,
     127,   131,   137,   139,   141,   143,   147,   152,   156,   158,
     162,   165,   167,   171,   174,   176,   180,   184,   186,   188,
     190,   192,   194,   196,   198,   200,   202,   204,   208,   210,
     214,   219,   223,   225,   229,   233,   236,   241,   243,   246,
     250,   254,   258,   262,   264,   266,   268,   270,   272,   274,
     276,   278,   280,   282,   284,   286,   288,   290,   292,   294,
     298,   304,   309,   316,   321,   325,   327,   330,   334,   338,
     342,   346
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      66,     0,    -1,    67,    -1,    68,    -1,    69,    -1,    70,
      -1,     3,    73,    79,     4,    81,    74,    75,    78,    -1,
       3,    73,    79,    74,    75,    78,    -1,    16,    17,    85,
      72,    18,    24,    79,    25,    -1,    19,    85,    20,    71,
      74,    -1,    50,     4,    85,    74,    -1,    92,    -1,    92,
      22,    71,    -1,    24,    79,    25,    -1,    -1,    12,    -1,
      -1,     5,    91,    -1,    -1,     8,     9,    76,    -1,    -1,
      77,    -1,    77,    22,    76,    -1,    87,    -1,    87,    10,
      -1,    87,    11,    -1,    15,     9,    79,    -1,    -1,    90,
      -1,    90,    22,    79,    -1,    53,    51,    -1,    53,    56,
      51,    -1,    54,    51,    -1,    54,    56,    51,    -1,    55,
      51,    -1,    55,    56,    51,    -1,    52,    51,    -1,    51,
      -1,    84,    -1,    81,    22,    84,    -1,    81,    80,    84,
      -1,    81,    80,    84,     7,    91,    -1,    26,    -1,    82,
      -1,    67,    -1,    24,    83,    25,    -1,    26,    24,    79,
      25,    -1,    26,    24,    25,    -1,    83,    -1,    83,     6,
      26,    -1,    83,    26,    -1,    82,    -1,    82,     6,    26,
      -1,    82,    26,    -1,    26,    -1,    26,    21,    86,    -1,
      26,    21,    46,    -1,    86,    -1,    46,    -1,    23,    -1,
      48,    -1,    64,    -1,    44,    -1,    45,    -1,    46,    -1,
      47,    -1,    87,    -1,    89,    88,    89,    -1,    67,    -1,
      24,    89,    25,    -1,    26,    24,    79,    25,    -1,    26,
      24,    25,    -1,    89,    -1,    89,     6,    26,    -1,    89,
       6,    49,    -1,    89,    95,    -1,    89,    95,     6,    26,
      -1,    94,    -1,    41,    91,    -1,    91,    43,    91,    -1,
      91,    42,    91,    -1,    24,    91,    25,    -1,    90,    27,
      90,    -1,    27,    -1,    28,    -1,    14,    -1,    29,    -1,
      30,    -1,    40,    -1,    31,    -1,    32,    -1,    33,    -1,
      34,    -1,    35,    -1,    36,    -1,    37,    -1,    38,    -1,
      39,    -1,    41,    -1,    90,    93,    90,    -1,    90,    13,
      90,    42,    90,    -1,    90,    41,    93,    90,    -1,    90,
      41,    13,    90,    42,    90,    -1,    90,    28,    41,    90,
      -1,    57,    96,    58,    -1,    97,    -1,    97,    96,    -1,
      59,    27,    49,    -1,    62,    27,    49,    -1,    60,    27,
      49,    -1,    61,    27,    49,    -1,    63,    27,    49,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    64,    64,    65,    66,    67,    70,    72,    76,    80,
      84,    88,    89,    92,    93,    96,    97,   100,   101,   104,
     105,   108,   109,   112,   113,   114,   117,   118,   121,   122,
     125,   126,   127,   128,   129,   130,   131,   132,   135,   136,
     137,   138,   141,   144,   145,   146,   147,   148,   151,   152,
     153,   155,   156,   157,   160,   161,   162,   166,   167,   168,
     169,   170,   174,   175,   176,   177,   180,   181,   182,   183,
     184,   185,   188,   189,   190,   191,   192,   195,   196,   197,
     198,   199,   202,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   223,
     224,   225,   226,   227,   230,   233,   234,   238,   239,   240,
     241,   242
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "L_SELECT", "L_FROM", "L_WHERE", "L_AS",
  "L_ON", "L_ORDER", "L_BY", "L_ORDER_ASC", "L_ORDER_DSC", "L_DISTINCT",
  "L_BETWEEN", "L_IN", "L_GROUP", "L_INSERT", "L_INTO", "L_VALUES",
  "L_UPDATE", "L_SET", "L_DOT", "L_COMMA", "L_NULL", "L_LBRACKET",
  "L_RBRACKET", "L_IDENT", "L_EQ", "L_IS", "L_LIKE", "L_ILIKE", "L_GT",
  "L_LT", "L_GEQ", "L_LEQ", "L_DIFF", "L_REGEXP", "L_REGEXP_CI",
  "L_NOTREGEXP", "L_NOTREGEXP_CI", "L_SIMILAR", "L_NOT", "L_AND", "L_OR",
  "L_MINUS", "L_PLUS", "L_TIMES", "L_DIV", "L_STRING", "L_TEXTUAL",
  "L_DELETE", "L_JOIN", "L_INNER", "L_LEFT", "L_RIGHT", "L_FULL",
  "L_OUTER", "L_LSBRACKET", "L_RSBRACKET", "L_PNAME", "L_PTYPE",
  "L_PISPARAM", "L_PDESCR", "L_PNULLOK", "L_UNSPECVAL", "$accept",
  "statement", "select_statement", "insert_statement", "update_statement",
  "delete_statement", "set_list", "opt_fields_list", "opt_distinct",
  "opt_where", "opt_orderby", "order_fields_list", "order_field",
  "opt_groupby", "fields_list", "join_type", "targets_list",
  "simple_table", "table", "target", "simple_target", "dotted_name",
  "field_name", "field_op", "field_raw", "field", "where_list", "set_item",
  "condition_operator", "where_item", "param_spec", "param_spec_list",
  "param_spec_item", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    65,    66,    66,    66,    66,    67,    67,    68,    69,
      70,    71,    71,    72,    72,    73,    73,    74,    74,    75,
      75,    76,    76,    77,    77,    77,    78,    78,    79,    79,
      80,    80,    80,    80,    80,    80,    80,    80,    81,    81,
      81,    81,    82,    83,    83,    83,    83,    83,    84,    84,
      84,    85,    85,    85,    86,    86,    86,    87,    87,    87,
      87,    87,    88,    88,    88,    88,    89,    89,    89,    89,
      89,    89,    90,    90,    90,    90,    90,    91,    91,    91,
      91,    91,    92,    93,    93,    93,    93,    93,    93,    93,
      93,    93,    93,    93,    93,    93,    93,    93,    93,    94,
      94,    94,    94,    94,    95,    96,    96,    97,    97,    97,
      97,    97
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     8,     6,     8,     5,
       4,     1,     3,     3,     0,     1,     0,     2,     0,     3,
       0,     1,     3,     1,     2,     2,     3,     0,     1,     3,
       2,     3,     2,     3,     2,     3,     2,     1,     1,     3,
       3,     5,     1,     1,     1,     3,     4,     3,     1,     3,
       2,     1,     3,     2,     1,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     1,     3,
       4,     3,     1,     3,     3,     2,     4,     1,     2,     3,
       3,     3,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       5,     4,     6,     4,     3,     1,     2,     3,     3,     3,
       3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    16,     0,     0,     0,     0,     2,     3,     4,     5,
      15,     0,     0,    42,    51,     0,     0,     1,    59,     0,
      54,    58,    60,    61,    68,    18,    57,    66,    72,    28,
      14,     0,    53,     0,    18,     0,     0,     0,     0,     0,
      20,     0,    62,    63,    64,    65,     0,     0,    75,     0,
       0,     0,    52,    18,     0,    11,    10,    69,    54,    56,
      55,    71,     0,     0,    42,    44,    18,    43,    48,    38,
       0,     0,     0,    17,    77,     0,    27,    73,    74,     0,
       0,     0,     0,     0,     0,   105,    67,     0,    29,     0,
       0,     9,     0,     0,    70,     0,     0,     0,    37,     0,
       0,     0,     0,    20,     0,     0,    50,    72,     0,    78,
       0,    85,    83,    84,    86,    87,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    88,    98,     0,     0,     0,
       0,     0,     7,     0,     0,     0,     0,     0,   104,   106,
      76,    13,     0,    82,    12,    45,    47,     0,    39,    36,
      30,     0,    32,     0,    34,     0,    27,    40,    49,    81,
       0,     0,     0,    84,    98,     0,    99,    80,    79,    19,
      21,    23,     0,   107,   109,   110,   108,   111,     0,    46,
      31,    33,    35,     6,     0,     0,   103,     0,   101,     0,
      24,    25,    26,     8,    41,   100,     0,    22,   102
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     5,    24,     7,     8,     9,    53,    51,    11,    40,
      76,   169,   170,   132,    25,   104,    66,    67,    68,    69,
      15,    26,    27,    47,    28,    29,    73,    55,   127,    74,
      48,    84,    85
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -121
static const yytype_int16 yypact[] =
{
       5,    -3,     6,    21,    37,    53,  -121,  -121,  -121,  -121,
    -121,    55,    21,  -121,     7,    86,    21,  -121,  -121,    55,
     106,  -121,  -121,  -121,  -121,    13,  -121,  -121,    29,    89,
      93,    87,  -121,    55,   110,    79,   -10,     4,    22,     8,
     113,   -23,  -121,  -121,  -121,  -121,    96,    55,   125,    55,
      55,   118,  -121,   110,   120,   138,  -121,  -121,   143,  -121,
    -121,  -121,   136,    22,   142,  -121,    88,  -121,    14,  -121,
       8,     8,   140,   103,  -121,   153,   150,  -121,  -121,   157,
     162,   163,   164,   165,   135,    96,   141,   168,  -121,   170,
     187,  -121,    55,    55,  -121,   188,    19,    22,  -121,   161,
      15,    31,    58,   113,    22,   189,  -121,    45,    91,  -121,
      55,  -121,  -121,   173,  -121,  -121,  -121,  -121,  -121,  -121,
    -121,  -121,  -121,  -121,  -121,  -121,   169,    55,     8,     8,
      74,   207,  -121,   171,   172,   174,   175,   176,  -121,  -121,
    -121,  -121,    55,  -121,  -121,  -121,  -121,   192,  -121,  -121,
    -121,   167,  -121,   177,  -121,   178,   150,   212,  -121,  -121,
     180,    55,    55,  -121,  -121,    55,  -121,  -121,   184,  -121,
     205,   139,    55,  -121,  -121,  -121,  -121,  -121,   206,  -121,
    -121,  -121,  -121,  -121,     8,    55,  -121,   190,  -121,    74,
    -121,  -121,  -121,  -121,   103,  -121,    55,  -121,  -121
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -121,  -121,     1,  -121,  -121,  -121,   137,  -121,  -121,    46,
     130,    47,  -121,    78,   -35,  -121,  -121,   132,   179,   -85,
      72,   199,  -120,  -121,    38,   -33,   -66,  -121,   111,  -121,
    -121,   154,  -121
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      54,     6,    62,    77,   108,   109,    72,     1,     1,    10,
     171,     1,   148,    31,    88,    89,    58,    38,    39,   157,
     105,     2,     1,    12,     3,     1,    78,    18,    19,    61,
      20,    18,    70,    32,    20,    41,    59,    72,    72,    65,
     106,    16,    18,    19,   146,    20,    63,    13,    64,    71,
      21,    41,    22,    17,    21,     4,    22,    35,     1,   143,
      54,   147,   167,   168,    65,    21,   150,    22,    23,   171,
      57,   151,    23,    42,    43,    44,    45,   160,    18,    19,
      56,    20,   152,    23,    30,    86,    46,   153,    34,    42,
      43,    44,    45,    39,   166,    72,    72,    18,    65,    91,
      58,    21,    46,    22,    57,    65,    33,   178,   107,   154,
      97,    49,   103,    52,   155,    39,   159,    50,   194,    23,
      21,    75,    22,    42,    43,    44,    45,    36,   186,   187,
      37,    87,   188,   128,   129,    14,    90,   192,    23,    98,
      99,   100,   101,   102,    14,   128,   129,    92,    14,   190,
     191,    72,   195,   110,   111,    79,    80,    81,    82,    83,
      93,    94,   130,   198,    36,   131,    96,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   162,   111,   133,    42,    43,    44,    45,   134,
     135,   136,   137,   138,   140,   141,   112,   163,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     164,   142,   149,   145,   161,   158,   172,   179,   180,   184,
     173,   174,   185,   175,   176,   177,   128,   189,   181,   182,
     144,   193,   196,   156,   183,    60,   197,   165,     0,   139,
       0,     0,    95
};

static const yytype_int16 yycheck[] =
{
      33,     0,    37,    26,    70,    71,    39,     3,     3,    12,
     130,     3,    97,     6,    49,    50,    26,     4,     5,   104,
       6,    16,     3,    17,    19,     3,    49,    23,    24,    25,
      26,    23,    24,    26,    26,     6,    46,    70,    71,    38,
      26,     4,    23,    24,    25,    26,    24,    26,    26,    41,
      46,     6,    48,     0,    46,    50,    48,    19,     3,    92,
      93,    96,   128,   129,    63,    46,    51,    48,    64,   189,
      25,    56,    64,    44,    45,    46,    47,   110,    23,    24,
      34,    26,    51,    64,    12,    47,    57,    56,    16,    44,
      45,    46,    47,     5,   127,   128,   129,    23,    97,    53,
      26,    46,    57,    48,    25,   104,    20,   142,    70,    51,
      22,    22,    66,    26,    56,     5,    25,    24,   184,    64,
      46,     8,    48,    44,    45,    46,    47,    21,   161,   162,
      24,     6,   165,    42,    43,     3,    18,   172,    64,    51,
      52,    53,    54,    55,    12,    42,    43,    27,    16,    10,
      11,   184,   185,    13,    14,    59,    60,    61,    62,    63,
      22,    25,     9,   196,    21,    15,    24,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    13,    14,    27,    44,    45,    46,    47,    27,
      27,    27,    27,    58,    26,    25,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    24,    51,    25,    41,    26,     9,    25,    51,     7,
      49,    49,    42,    49,    49,    49,    42,    22,    51,    51,
      93,    25,    42,   103,   156,    36,   189,   126,    -1,    85,
      -1,    -1,    63
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,    16,    19,    50,    66,    67,    68,    69,    70,
      12,    73,    17,    26,    82,    85,     4,     0,    23,    24,
      26,    46,    48,    64,    67,    79,    86,    87,    89,    90,
      85,     6,    26,    20,    85,    89,    21,    24,     4,     5,
      74,     6,    44,    45,    46,    47,    57,    88,    95,    22,
      24,    72,    26,    71,    90,    92,    74,    25,    26,    46,
      86,    25,    79,    24,    26,    67,    81,    82,    83,    84,
      24,    41,    90,    91,    94,     8,    75,    26,    49,    59,
      60,    61,    62,    63,    96,    97,    89,     6,    79,    79,
      18,    74,    27,    22,    25,    83,    24,    22,    51,    52,
      53,    54,    55,    74,    80,     6,    26,    89,    91,    91,
      13,    14,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    93,    42,    43,
       9,    15,    78,    27,    27,    27,    27,    27,    58,    96,
      26,    25,    24,    90,    71,    25,    25,    79,    84,    51,
      51,    56,    51,    56,    51,    56,    75,    84,    26,    25,
      90,    41,    13,    28,    41,    93,    90,    91,    91,    76,
      77,    87,     9,    49,    49,    49,    49,    49,    79,    25,
      51,    51,    51,    78,     7,    42,    90,    90,    90,    22,
      10,    11,    79,    25,    91,    90,    42,    76,    90
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
#line 64 "bondsql/parser.y"
    {sql_result = sql_statement_build (SQL_select, (yyvsp[(1) - (1)].v));;}
    break;

  case 3:
#line 65 "bondsql/parser.y"
    {sql_result = sql_statement_build (SQL_insert, (yyvsp[(1) - (1)].v));;}
    break;

  case 4:
#line 66 "bondsql/parser.y"
    {sql_result = sql_statement_build (SQL_update, (yyvsp[(1) - (1)].v));;}
    break;

  case 5:
#line 67 "bondsql/parser.y"
    {sql_result = sql_statement_build (SQL_delete, (yyvsp[(1) - (1)].v));;}
    break;

  case 6:
#line 71 "bondsql/parser.y"
    {(yyval.v) = sql_select_statement_build ((yyvsp[(2) - (8)].i), (yyvsp[(3) - (8)].list), (yyvsp[(5) - (8)].list), (yyvsp[(6) - (8)].w), (yyvsp[(7) - (8)].list), (yyvsp[(8) - (8)].list));;}
    break;

  case 7:
#line 73 "bondsql/parser.y"
    {(yyval.v) = sql_select_statement_build ((yyvsp[(2) - (6)].i), (yyvsp[(3) - (6)].list), NULL, (yyvsp[(4) - (6)].w), (yyvsp[(5) - (6)].list), (yyvsp[(6) - (6)].list));;}
    break;

  case 8:
#line 77 "bondsql/parser.y"
    {(yyval.v) = sql_insert_statement_build ((yyvsp[(3) - (8)].t), (yyvsp[(4) - (8)].list), (yyvsp[(7) - (8)].list));;}
    break;

  case 9:
#line 81 "bondsql/parser.y"
    {(yyval.v) = sql_update_statement_build ((yyvsp[(2) - (5)].t), (yyvsp[(4) - (5)].list), (yyvsp[(5) - (5)].w));;}
    break;

  case 10:
#line 85 "bondsql/parser.y"
    {(yyval.v) = sql_delete_statement_build ((yyvsp[(3) - (4)].t), (yyvsp[(4) - (4)].w));;}
    break;

  case 11:
#line 88 "bondsql/parser.y"
    {(yyval.list) = g_list_append (NULL, (yyvsp[(1) - (1)].c));;}
    break;

  case 12:
#line 89 "bondsql/parser.y"
    {(yyval.list) = g_list_prepend ((yyvsp[(3) - (3)].list), (yyvsp[(1) - (3)].c));;}
    break;

  case 13:
#line 92 "bondsql/parser.y"
    {(yyval.list) = (yyvsp[(2) - (3)].list);;}
    break;

  case 14:
#line 93 "bondsql/parser.y"
    {(yyval.list) = NULL;;}
    break;

  case 15:
#line 96 "bondsql/parser.y"
    {(yyval.i) = 1;;}
    break;

  case 16:
#line 97 "bondsql/parser.y"
    {(yyval.i) = 0;;}
    break;

  case 17:
#line 100 "bondsql/parser.y"
    {(yyval.w) = (yyvsp[(2) - (2)].w);;}
    break;

  case 18:
#line 101 "bondsql/parser.y"
    {(yyval.w) = NULL;;}
    break;

  case 19:
#line 104 "bondsql/parser.y"
    {(yyval.list) = (yyvsp[(3) - (3)].list);;}
    break;

  case 20:
#line 105 "bondsql/parser.y"
    {(yyval.list) = NULL;;}
    break;

  case 21:
#line 108 "bondsql/parser.y"
    {(yyval.list) = g_list_append (NULL, (yyvsp[(1) - (1)].of));;}
    break;

  case 22:
#line 109 "bondsql/parser.y"
    {(yyval.list) = g_list_prepend ((yyvsp[(3) - (3)].list), (yyvsp[(1) - (3)].of));;}
    break;

  case 23:
#line 112 "bondsql/parser.y"
    {(yyval.of) = sql_order_field_build ((yyvsp[(1) - (1)].list), SQL_asc);;}
    break;

  case 24:
#line 113 "bondsql/parser.y"
    {(yyval.of) = sql_order_field_build ((yyvsp[(1) - (2)].list), SQL_asc);;}
    break;

  case 25:
#line 114 "bondsql/parser.y"
    {(yyval.of) = sql_order_field_build ((yyvsp[(1) - (2)].list), SQL_desc);;}
    break;

  case 26:
#line 117 "bondsql/parser.y"
    {(yyval.list) = (yyvsp[(3) - (3)].list);;}
    break;

  case 27:
#line 118 "bondsql/parser.y"
    {(yyval.list) = NULL;;}
    break;

  case 28:
#line 121 "bondsql/parser.y"
    {(yyval.list) = g_list_append (NULL, (yyvsp[(1) - (1)].f));;}
    break;

  case 29:
#line 122 "bondsql/parser.y"
    {(yyval.list) = g_list_prepend ((yyvsp[(3) - (3)].list), (yyvsp[(1) - (3)].f));;}
    break;

  case 30:
#line 125 "bondsql/parser.y"
    {(yyval.i) = SQL_left_join;;}
    break;

  case 31:
#line 126 "bondsql/parser.y"
    {(yyval.i) = SQL_left_join;;}
    break;

  case 32:
#line 127 "bondsql/parser.y"
    {(yyval.i) = SQL_right_join;;}
    break;

  case 33:
#line 128 "bondsql/parser.y"
    {(yyval.i) = SQL_right_join;;}
    break;

  case 34:
#line 129 "bondsql/parser.y"
    {(yyval.i) = SQL_full_join;;}
    break;

  case 35:
#line 130 "bondsql/parser.y"
    {(yyval.i) = SQL_full_join;;}
    break;

  case 36:
#line 131 "bondsql/parser.y"
    {(yyval.i) = SQL_inner_join;;}
    break;

  case 37:
#line 132 "bondsql/parser.y"
    {(yyval.i) = SQL_inner_join;;}
    break;

  case 38:
#line 135 "bondsql/parser.y"
    {(yyval.list) = g_list_append (NULL, (yyvsp[(1) - (1)].t));;}
    break;

  case 39:
#line 136 "bondsql/parser.y"
    {(yyval.list) = g_list_append ((yyvsp[(1) - (3)].list), (yyvsp[(3) - (3)].t));;}
    break;

  case 40:
#line 137 "bondsql/parser.y"
    {(yyval.list) = g_list_append ((yyvsp[(1) - (3)].list), sql_table_set_join ((yyvsp[(3) - (3)].t), (yyvsp[(2) - (3)].i), NULL));;}
    break;

  case 41:
#line 138 "bondsql/parser.y"
    {(yyval.list) = g_list_append ((yyvsp[(1) - (5)].list), sql_table_set_join ((yyvsp[(3) - (5)].t), (yyvsp[(2) - (5)].i), (yyvsp[(5) - (5)].w)));;}
    break;

  case 42:
#line 141 "bondsql/parser.y"
    {(yyval.t) = sql_table_build ((yyvsp[(1) - (1)].str)); mem_free ((yyvsp[(1) - (1)].str));;}
    break;

  case 43:
#line 144 "bondsql/parser.y"
    {(yyval.t) = (yyvsp[(1) - (1)].t);;}
    break;

  case 44:
#line 145 "bondsql/parser.y"
    {(yyval.t) = sql_table_build_select ((yyvsp[(1) - (1)].v));;}
    break;

  case 45:
#line 146 "bondsql/parser.y"
    {(yyval.t) = (yyvsp[(2) - (3)].t);;}
    break;

  case 46:
#line 147 "bondsql/parser.y"
    {(yyval.t) = sql_table_build_function ((yyvsp[(1) - (4)].str), (yyvsp[(3) - (4)].list)); ;}
    break;

  case 47:
#line 148 "bondsql/parser.y"
    {(yyval.t) = sql_table_build_function ((yyvsp[(1) - (3)].str), NULL); ;}
    break;

  case 48:
#line 151 "bondsql/parser.y"
    {(yyval.t) = (yyvsp[(1) - (1)].t);;}
    break;

  case 49:
#line 152 "bondsql/parser.y"
    {(yyval.t) = sql_table_set_as ((yyvsp[(1) - (3)].t), (yyvsp[(3) - (3)].str));;}
    break;

  case 50:
#line 153 "bondsql/parser.y"
    {(yyval.t) = sql_table_set_as ((yyvsp[(1) - (2)].t), (yyvsp[(2) - (2)].str));;}
    break;

  case 51:
#line 155 "bondsql/parser.y"
    {(yyval.t) = (yyvsp[(1) - (1)].t);;}
    break;

  case 52:
#line 156 "bondsql/parser.y"
    {(yyval.t) = sql_table_set_as ((yyvsp[(1) - (3)].t), (yyvsp[(3) - (3)].str));;}
    break;

  case 53:
#line 157 "bondsql/parser.y"
    {(yyval.t) = sql_table_set_as ((yyvsp[(1) - (2)].t), (yyvsp[(2) - (2)].str));;}
    break;

  case 54:
#line 160 "bondsql/parser.y"
    {(yyval.list) = g_list_append (NULL, mem_strdup ((yyvsp[(1) - (1)].str))); mem_free ((yyvsp[(1) - (1)].str));;}
    break;

  case 55:
#line 161 "bondsql/parser.y"
    {(yyval.list) = g_list_prepend ((yyvsp[(3) - (3)].list), mem_strdup ((yyvsp[(1) - (3)].str))); mem_free ((yyvsp[(1) - (3)].str));;}
    break;

  case 56:
#line 162 "bondsql/parser.y"
    {(yyval.list) = g_list_append (NULL, mem_strdup ((yyvsp[(1) - (3)].str))); mem_free ((yyvsp[(1) - (3)].str));
					 (yyval.list) = g_list_append ((yyval.list), mem_strdup ("*"));;}
    break;

  case 57:
#line 166 "bondsql/parser.y"
    {(yyval.list) = (yyvsp[(1) - (1)].list);;}
    break;

  case 58:
#line 167 "bondsql/parser.y"
    {(yyval.list) = g_list_append (NULL, mem_strdup ("*"));;}
    break;

  case 59:
#line 168 "bondsql/parser.y"
    {(yyval.list) = g_list_append (NULL, mem_strdup ("null"));;}
    break;

  case 60:
#line 169 "bondsql/parser.y"
    {(yyval.list) = g_list_append (NULL, (yyvsp[(1) - (1)].str));;}
    break;

  case 61:
#line 170 "bondsql/parser.y"
    {(yyval.list) = g_list_append (NULL, mem_strdup (""));;}
    break;

  case 62:
#line 174 "bondsql/parser.y"
    {(yyval.i) = SQL_minus;;}
    break;

  case 63:
#line 175 "bondsql/parser.y"
    {(yyval.i) = SQL_plus;;}
    break;

  case 64:
#line 176 "bondsql/parser.y"
    {(yyval.i) = SQL_times;;}
    break;

  case 65:
#line 177 "bondsql/parser.y"
    {(yyval.i) = SQL_div;;}
    break;

  case 66:
#line 180 "bondsql/parser.y"
    {(yyval.fi) = sql_field_item_build ((yyvsp[(1) - (1)].list));;}
    break;

  case 67:
#line 181 "bondsql/parser.y"
    {(yyval.fi) = sql_field_item_build_equation ((yyvsp[(1) - (3)].fi), (yyvsp[(3) - (3)].fi), (yyvsp[(2) - (3)].i));;}
    break;

  case 68:
#line 182 "bondsql/parser.y"
    {(yyval.fi) = sql_field_item_build_select ((yyvsp[(1) - (1)].v));;}
    break;

  case 69:
#line 183 "bondsql/parser.y"
    {(yyval.fi) = (yyvsp[(2) - (3)].fi);;}
    break;

  case 70:
#line 184 "bondsql/parser.y"
    {(yyval.fi) = sql_field_build_function((yyvsp[(1) - (4)].str), (yyvsp[(3) - (4)].list)); ;}
    break;

  case 71:
#line 185 "bondsql/parser.y"
    {(yyval.fi) = sql_field_build_function((yyvsp[(1) - (3)].str), NULL); ;}
    break;

  case 72:
#line 188 "bondsql/parser.y"
    {(yyval.f) = sql_field_build ((yyvsp[(1) - (1)].fi));;}
    break;

  case 73:
#line 189 "bondsql/parser.y"
    {(yyval.f) = sql_field_set_as (sql_field_build ((yyvsp[(1) - (3)].fi)), (yyvsp[(3) - (3)].str));;}
    break;

  case 74:
#line 190 "bondsql/parser.y"
    {(yyval.f) = sql_field_set_as (sql_field_build ((yyvsp[(1) - (3)].fi)), (yyvsp[(3) - (3)].str));;}
    break;

  case 75:
#line 191 "bondsql/parser.y"
    {(yyval.f) = sql_field_set_param_spec (sql_field_build ((yyvsp[(1) - (2)].fi)), (yyvsp[(2) - (2)].list));;}
    break;

  case 76:
#line 192 "bondsql/parser.y"
    {(yyval.f) = sql_field_set_as (sql_field_set_param_spec (sql_field_build ((yyvsp[(1) - (4)].fi)), (yyvsp[(2) - (4)].list)), (yyvsp[(4) - (4)].str));;}
    break;

  case 77:
#line 195 "bondsql/parser.y"
    {(yyval.w) = sql_where_build_single ((yyvsp[(1) - (1)].c));;}
    break;

  case 78:
#line 196 "bondsql/parser.y"
    {(yyval.w) = sql_where_build_negated ((yyvsp[(2) - (2)].w));;}
    break;

  case 79:
#line 197 "bondsql/parser.y"
    {(yyval.w) = sql_where_build_pair ((yyvsp[(1) - (3)].w), (yyvsp[(3) - (3)].w), SQL_or);;}
    break;

  case 80:
#line 198 "bondsql/parser.y"
    {(yyval.w) = sql_where_build_pair ((yyvsp[(1) - (3)].w), (yyvsp[(3) - (3)].w), SQL_and);;}
    break;

  case 81:
#line 199 "bondsql/parser.y"
    {(yyval.w) = (yyvsp[(2) - (3)].w);;}
    break;

  case 82:
#line 202 "bondsql/parser.y"
    {(yyval.c) = sql_build_condition ((yyvsp[(1) - (3)].f), (yyvsp[(3) - (3)].f), SQL_eq);;}
    break;

  case 83:
#line 205 "bondsql/parser.y"
    {(yyval.i) = SQL_eq;;}
    break;

  case 84:
#line 206 "bondsql/parser.y"
    {(yyval.i) = SQL_is;;}
    break;

  case 85:
#line 207 "bondsql/parser.y"
    {(yyval.i) = SQL_in;;}
    break;

  case 86:
#line 208 "bondsql/parser.y"
    {(yyval.i) = SQL_like;;}
    break;

  case 87:
#line 209 "bondsql/parser.y"
    {(yyval.i) = SQL_ilike;;}
    break;

  case 88:
#line 210 "bondsql/parser.y"
    {(yyval.i) = SQL_similar;;}
    break;

  case 89:
#line 211 "bondsql/parser.y"
    {(yyval.i) = SQL_gt;;}
    break;

  case 90:
#line 212 "bondsql/parser.y"
    {(yyval.i) = SQL_lt;;}
    break;

  case 91:
#line 213 "bondsql/parser.y"
    {(yyval.i) = SQL_geq;;}
    break;

  case 92:
#line 214 "bondsql/parser.y"
    {(yyval.i) = SQL_leq;;}
    break;

  case 93:
#line 215 "bondsql/parser.y"
    {(yyval.i) = SQL_diff;;}
    break;

  case 94:
#line 216 "bondsql/parser.y"
    {(yyval.i) = SQL_regexp;;}
    break;

  case 95:
#line 217 "bondsql/parser.y"
    {(yyval.i) = SQL_regexp_ci;;}
    break;

  case 96:
#line 218 "bondsql/parser.y"
    {(yyval.i) = SQL_not_regexp;;}
    break;

  case 97:
#line 219 "bondsql/parser.y"
    {(yyval.i) = SQL_not_regexp_ci;;}
    break;

  case 98:
#line 220 "bondsql/parser.y"
    {(yyval.i) = SQL_not;;}
    break;

  case 99:
#line 223 "bondsql/parser.y"
    {(yyval.c) = sql_build_condition ((yyvsp[(1) - (3)].f), (yyvsp[(3) - (3)].f), (yyvsp[(2) - (3)].i));;}
    break;

  case 100:
#line 224 "bondsql/parser.y"
    {(yyval.c) = sql_build_condition_between ((yyvsp[(1) - (5)].f), (yyvsp[(3) - (5)].f), (yyvsp[(5) - (5)].f));;}
    break;

  case 101:
#line 225 "bondsql/parser.y"
    {(yyval.c) = sql_condition_negate (sql_build_condition ((yyvsp[(1) - (4)].f), (yyvsp[(4) - (4)].f), (yyvsp[(3) - (4)].i)));;}
    break;

  case 102:
#line 226 "bondsql/parser.y"
    {(yyval.c) = sql_condition_negate (sql_build_condition_between ((yyvsp[(1) - (6)].f), (yyvsp[(4) - (6)].f), (yyvsp[(6) - (6)].f)));;}
    break;

  case 103:
#line 227 "bondsql/parser.y"
    {(yyval.c) = sql_condition_negate (sql_build_condition ((yyvsp[(1) - (4)].f), (yyvsp[(4) - (4)].f), SQL_is));;}
    break;

  case 104:
#line 230 "bondsql/parser.y"
    {(yyval.list) = (yyvsp[(2) - (3)].list);;}
    break;

  case 105:
#line 233 "bondsql/parser.y"
    {(yyval.list) = g_list_append (NULL, (yyvsp[(1) - (1)].ps));;}
    break;

  case 106:
#line 234 "bondsql/parser.y"
    {(yyval.list) = g_list_prepend ((yyvsp[(2) - (2)].list), (yyvsp[(1) - (2)].ps));;}
    break;

  case 107:
#line 238 "bondsql/parser.y"
    {(yyval.ps) = param_spec_build (PARAM_name, (yyvsp[(3) - (3)].str));;}
    break;

  case 108:
#line 239 "bondsql/parser.y"
    {(yyval.ps) = param_spec_build (PARAM_descr, (yyvsp[(3) - (3)].str));;}
    break;

  case 109:
#line 240 "bondsql/parser.y"
    {(yyval.ps) = param_spec_build (PARAM_type, (yyvsp[(3) - (3)].str));;}
    break;

  case 110:
#line 241 "bondsql/parser.y"
    {(yyval.ps) = param_spec_build (PARAM_isparam, (yyvsp[(3) - (3)].str));;}
    break;

  case 111:
#line 242 "bondsql/parser.y"
    {(yyval.ps) = param_spec_build (PARAM_nullok, (yyvsp[(3) - (3)].str));;}
    break;


/* Line 1267 of yacc.c.  */
#line 2223 "bondsql/parser.c"
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


#line 244 "bondsql/parser.y"


