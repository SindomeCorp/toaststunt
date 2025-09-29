/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_HOME_UBUNTU_TOASTSTUNT_SRC_PARSER_HH_INCLUDED
# define YY_YY_HOME_UBUNTU_TOASTSTUNT_SRC_PARSER_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    tINTEGER = 258,
    tOBJECT = 259,
    tFLOAT = 260,
    tSTRING = 261,
    tID = 262,
    tERROR = 263,
    tIF = 264,
    tELSE = 265,
    tELSEIF = 266,
    tENDIF = 267,
    tFOR = 268,
    tIN = 269,
    tENDFOR = 270,
    tRETURN = 271,
    tFORK = 272,
    tENDFORK = 273,
    tWHILE = 274,
    tENDWHILE = 275,
    tTRY = 276,
    tENDTRY = 277,
    tEXCEPT = 278,
    tFINALLY = 279,
    tANY = 280,
    tBREAK = 281,
    tCONTINUE = 282,
    tTO = 283,
    tARROW = 284,
    tMAP = 285,
    tOR = 286,
    tAND = 287,
    tEQ = 288,
    tNE = 289,
    tLE = 290,
    tGE = 291,
    tBITOR = 292,
    tBITAND = 293,
    tBITXOR = 294,
    tBITSHL = 295,
    tBITSHR = 296,
    tUNARYMINUS = 297
  };
#endif
/* Tokens.  */
#define tINTEGER 258
#define tOBJECT 259
#define tFLOAT 260
#define tSTRING 261
#define tID 262
#define tERROR 263
#define tIF 264
#define tELSE 265
#define tELSEIF 266
#define tENDIF 267
#define tFOR 268
#define tIN 269
#define tENDFOR 270
#define tRETURN 271
#define tFORK 272
#define tENDFORK 273
#define tWHILE 274
#define tENDWHILE 275
#define tTRY 276
#define tENDTRY 277
#define tEXCEPT 278
#define tFINALLY 279
#define tANY 280
#define tBREAK 281
#define tCONTINUE 282
#define tTO 283
#define tARROW 284
#define tMAP 285
#define tOR 286
#define tAND 287
#define tEQ 288
#define tNE 289
#define tLE 290
#define tGE 291
#define tBITOR 292
#define tBITAND 293
#define tBITXOR 294
#define tBITSHL 295
#define tBITSHR 296
#define tUNARYMINUS 297

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 69 "src/parser.y"

  Stmt         *stmt;
  Expr         *expr;
  Num           integer;
  Objid         object;
  double       real;
  char         *string;
  enum error    error;
  Arg_List     *args;
  Map_List     *map;
  Cond_Arm     *arm;
  Except_Arm   *except;
  Scatter      *scatter;

#line 156 "/home/ubuntu/toaststunt/src/parser.hh"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_HOME_UBUNTU_TOASTSTUNT_SRC_PARSER_HH_INCLUDED  */
