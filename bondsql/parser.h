/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

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
/* Line 1489 of yacc.c.  */
#line 191 "bondsql/parser.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE sqllval;

