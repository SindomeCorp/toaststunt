/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
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
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "src/parser.y"

/******************************************************************************
  Copyright (c) 1992, 1995, 1996 Xerox Corporation.  All rights reserved.
  Portions of this code were written by Stephen White, aka ghond.
  Use and copying of this software and preparation of derivative works based
  upon this software are permitted.  Any distribution of this software or
  derivative works must comply with all applicable United States export
  control laws.  This software is made available AS IS, and Xerox Corporation
  makes no warranty about the software, its performance or its conformity to
  any specification.  Any person obtaining a copy of this software is requested
  to send their name and post office or electronic mail address to:
    Pavel Curtis
    Xerox PARC
    3333 Coyote Hill Rd.
    Palo Alto, CA 94304
    Pavel@Xerox.Com
 *****************************************************************************/

/*************************************************************************/
/* NOTE: If you add an #include here, make sure you properly update the  */
/*       parser.o dependency line in the Makefile.                       */
/*************************************************************************/

#include <ctype.h>
#include "my-math.h"
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "code_gen.h"
#include "config.h"
#include "functions.h"
#include "keywords.h"
#include "list.h"
#include "log.h"
#include "numbers.h"
#include "opcode.h"
#include "parser.h"
#include "program.h"
#include "storage.h"
#include "streams.h"
#include "structures.h"
#include "sym_table.h"
#include "utils.h"
#include "version.h"
#include "waif.h"

static Stmt            *prog_start;
static int              dollars_ok;
static DB_Version       language_version;

static void     error(const char *, const char *);
static void     warning(const char *, const char *);
static int      find_id(char *name);
static void     yyerror(const char *s);
static int      yylex(void);
static Scatter *scatter_from_arglist(Arg_List *);
static Scatter *add_scatter_item(Scatter *, Scatter *);
static void     vet_scatter(Scatter *);
static void     push_loop_name(const char *);
static void     pop_loop_name(void);
static void     suspend_loop_scope(void);
static void     resume_loop_scope(void);

enum loop_exit_kind { LOOP_BREAK, LOOP_CONTINUE };

static void     check_loop_name(const char *, enum loop_exit_kind);

#line 139 "/home/seven/toaststunt/build-asan/parser.cc"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.hh"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_tINTEGER = 3,                   /* tINTEGER  */
  YYSYMBOL_tOBJECT = 4,                    /* tOBJECT  */
  YYSYMBOL_tFLOAT = 5,                     /* tFLOAT  */
  YYSYMBOL_tSTRING = 6,                    /* tSTRING  */
  YYSYMBOL_tID = 7,                        /* tID  */
  YYSYMBOL_tERROR = 8,                     /* tERROR  */
  YYSYMBOL_tIF = 9,                        /* tIF  */
  YYSYMBOL_tELSE = 10,                     /* tELSE  */
  YYSYMBOL_tELSEIF = 11,                   /* tELSEIF  */
  YYSYMBOL_tENDIF = 12,                    /* tENDIF  */
  YYSYMBOL_tFOR = 13,                      /* tFOR  */
  YYSYMBOL_tIN = 14,                       /* tIN  */
  YYSYMBOL_tENDFOR = 15,                   /* tENDFOR  */
  YYSYMBOL_tRETURN = 16,                   /* tRETURN  */
  YYSYMBOL_tFORK = 17,                     /* tFORK  */
  YYSYMBOL_tENDFORK = 18,                  /* tENDFORK  */
  YYSYMBOL_tWHILE = 19,                    /* tWHILE  */
  YYSYMBOL_tENDWHILE = 20,                 /* tENDWHILE  */
  YYSYMBOL_tTRY = 21,                      /* tTRY  */
  YYSYMBOL_tENDTRY = 22,                   /* tENDTRY  */
  YYSYMBOL_tEXCEPT = 23,                   /* tEXCEPT  */
  YYSYMBOL_tFINALLY = 24,                  /* tFINALLY  */
  YYSYMBOL_tANY = 25,                      /* tANY  */
  YYSYMBOL_tBREAK = 26,                    /* tBREAK  */
  YYSYMBOL_tCONTINUE = 27,                 /* tCONTINUE  */
  YYSYMBOL_tTO = 28,                       /* tTO  */
  YYSYMBOL_tARROW = 29,                    /* tARROW  */
  YYSYMBOL_tMAP = 30,                      /* tMAP  */
  YYSYMBOL_31_ = 31,                       /* '='  */
  YYSYMBOL_32_ = 32,                       /* '?'  */
  YYSYMBOL_33_ = 33,                       /* '|'  */
  YYSYMBOL_tOR = 34,                       /* tOR  */
  YYSYMBOL_tAND = 35,                      /* tAND  */
  YYSYMBOL_tEQ = 36,                       /* tEQ  */
  YYSYMBOL_tNE = 37,                       /* tNE  */
  YYSYMBOL_38_ = 38,                       /* '<'  */
  YYSYMBOL_tLE = 39,                       /* tLE  */
  YYSYMBOL_40_ = 40,                       /* '>'  */
  YYSYMBOL_tGE = 41,                       /* tGE  */
  YYSYMBOL_tBITOR = 42,                    /* tBITOR  */
  YYSYMBOL_tBITAND = 43,                   /* tBITAND  */
  YYSYMBOL_tBITXOR = 44,                   /* tBITXOR  */
  YYSYMBOL_tBITSHL = 45,                   /* tBITSHL  */
  YYSYMBOL_tBITSHR = 46,                   /* tBITSHR  */
  YYSYMBOL_47_ = 47,                       /* '+'  */
  YYSYMBOL_48_ = 48,                       /* '-'  */
  YYSYMBOL_49_ = 49,                       /* '*'  */
  YYSYMBOL_50_ = 50,                       /* '/'  */
  YYSYMBOL_51_ = 51,                       /* '%'  */
  YYSYMBOL_52_ = 52,                       /* '^'  */
  YYSYMBOL_53_ = 53,                       /* '!'  */
  YYSYMBOL_54_ = 54,                       /* '~'  */
  YYSYMBOL_tUNARYMINUS = 55,               /* tUNARYMINUS  */
  YYSYMBOL_56_ = 56,                       /* '.'  */
  YYSYMBOL_57_ = 57,                       /* ':'  */
  YYSYMBOL_58_ = 58,                       /* '['  */
  YYSYMBOL_59_ = 59,                       /* '$'  */
  YYSYMBOL_60_ = 60,                       /* '('  */
  YYSYMBOL_61_ = 61,                       /* ')'  */
  YYSYMBOL_62_ = 62,                       /* ','  */
  YYSYMBOL_63_ = 63,                       /* ']'  */
  YYSYMBOL_64_ = 64,                       /* ';'  */
  YYSYMBOL_65_ = 65,                       /* '{'  */
  YYSYMBOL_66_ = 66,                       /* '}'  */
  YYSYMBOL_67_ = 67,                       /* '`'  */
  YYSYMBOL_68_ = 68,                       /* '\''  */
  YYSYMBOL_69_ = 69,                       /* '@'  */
  YYSYMBOL_YYACCEPT = 70,                  /* $accept  */
  YYSYMBOL_program = 71,                   /* program  */
  YYSYMBOL_statements = 72,                /* statements  */
  YYSYMBOL_statement = 73,                 /* statement  */
  YYSYMBOL_74_1 = 74,                      /* $@1  */
  YYSYMBOL_75_2 = 75,                      /* $@2  */
  YYSYMBOL_76_3 = 76,                      /* $@3  */
  YYSYMBOL_77_4 = 77,                      /* $@4  */
  YYSYMBOL_78_5 = 78,                      /* $@5  */
  YYSYMBOL_79_6 = 79,                      /* $@6  */
  YYSYMBOL_80_7 = 80,                      /* $@7  */
  YYSYMBOL_elseifs = 81,                   /* elseifs  */
  YYSYMBOL_elsepart = 82,                  /* elsepart  */
  YYSYMBOL_excepts = 83,                   /* excepts  */
  YYSYMBOL_84_8 = 84,                      /* $@8  */
  YYSYMBOL_except = 85,                    /* except  */
  YYSYMBOL_opt_id = 86,                    /* opt_id  */
  YYSYMBOL_expr = 87,                      /* expr  */
  YYSYMBOL_dollars_up = 88,                /* dollars_up  */
  YYSYMBOL_codes = 89,                     /* codes  */
  YYSYMBOL_default = 90,                   /* default  */
  YYSYMBOL_maplist = 91,                   /* maplist  */
  YYSYMBOL_arglist = 92,                   /* arglist  */
  YYSYMBOL_ne_arglist = 93,                /* ne_arglist  */
  YYSYMBOL_scatter = 94,                   /* scatter  */
  YYSYMBOL_scatter_item = 95               /* scatter_item  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1898

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  70
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  26
/* YYNRULES -- Number of rules.  */
#define YYNRULES  108
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  251

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   297


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    53,     2,     2,    59,    51,     2,    68,
      60,    61,    49,    47,    62,    48,    56,    50,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    57,    64,
      38,    31,    40,    32,    69,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    58,     2,    63,    52,     2,    67,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    65,    33,    66,    54,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30,    34,    35,    36,    37,
      39,    41,    42,    43,    44,    45,    46,    55
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   117,   117,   123,   124,   139,   148,   147,   161,   160,
     176,   175,   189,   188,   201,   200,   213,   212,   225,   224,
     236,   241,   247,   253,   259,   265,   270,   275,   277,   283,
     293,   294,   312,   313,   318,   321,   320,   347,   353,   354,
     359,   364,   369,   374,   379,   384,   389,   398,   405,   417,
     421,   428,   437,   441,   446,   454,   460,   466,   488,   496,
     517,   521,   525,   529,   533,   537,   541,   545,   549,   553,
     557,   561,   565,   569,   573,   577,   581,   585,   589,   593,
     597,   618,   623,   628,   630,   635,   640,   646,   653,   664,
     668,   670,   676,   677,   682,   684,   702,   703,   708,   710,
     712,   726,   743,   752,   756,   761,   766,   771,   775
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "tINTEGER", "tOBJECT",
  "tFLOAT", "tSTRING", "tID", "tERROR", "tIF", "tELSE", "tELSEIF",
  "tENDIF", "tFOR", "tIN", "tENDFOR", "tRETURN", "tFORK", "tENDFORK",
  "tWHILE", "tENDWHILE", "tTRY", "tENDTRY", "tEXCEPT", "tFINALLY", "tANY",
  "tBREAK", "tCONTINUE", "tTO", "tARROW", "tMAP", "'='", "'?'", "'|'",
  "tOR", "tAND", "tEQ", "tNE", "'<'", "tLE", "'>'", "tGE", "tBITOR",
  "tBITAND", "tBITXOR", "tBITSHL", "tBITSHR", "'+'", "'-'", "'*'", "'/'",
  "'%'", "'^'", "'!'", "'~'", "tUNARYMINUS", "'.'", "':'", "'['", "'$'",
  "'('", "')'", "','", "']'", "';'", "'{'", "'}'", "'`'", "'\\''", "'@'",
  "$accept", "program", "statements", "statement", "$@1", "$@2", "$@3",
  "$@4", "$@5", "$@6", "$@7", "elseifs", "elsepart", "excepts", "$@8",
  "except", "opt_id", "expr", "dollars_up", "codes", "default", "maplist",
  "arglist", "ne_arglist", "scatter", "scatter_item", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-58)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -58,    11,   844,   -58,   -58,   -58,   -58,   -58,   -40,   -58,
     -31,    32,   879,    20,    25,   -58,    10,    14,   944,   -58,
     944,   944,   909,    34,   944,   -58,   211,   944,   -58,   981,
     314,   944,   -11,   -58,  1020,   -10,   944,    21,   944,   399,
      28,   -58,    31,   -58,   -50,   -50,   -50,   -58,  1605,   -38,
      37,  1176,    84,   944,  1801,    33,    36,   -57,   -58,  1644,
     944,   944,   944,   944,   944,   944,   944,   944,   944,   944,
     944,   944,   944,   944,   944,   944,   944,   944,   944,   944,
     944,   944,    19,    27,   -58,   -58,    39,    40,  1215,   -27,
      96,   -58,   944,  1254,   944,  1293,   115,   -58,    24,   -58,
     -58,   944,   944,   -58,   314,   -58,    92,  1801,   -58,   229,
       8,    93,   247,   299,  1801,  1684,    69,    69,   299,   299,
     299,   299,   299,   299,   382,   382,   382,    91,    91,   -14,
     -14,    98,    98,    98,    98,   -58,   123,   944,    73,   944,
     944,   -58,   332,   -58,   944,   944,   121,  1332,   -58,  1371,
     -58,   -58,   -58,    86,   464,   -58,   -58,  1801,  1723,    83,
     944,   944,  1801,   -58,   -58,   144,   -58,   944,   -58,   128,
      40,   944,   -58,  1410,   314,  1449,  1059,   844,  1762,  1488,
     100,   -58,   -58,   -58,   -58,   247,   -58,   115,   944,   -58,
    1801,  1801,   -58,  1801,   944,    90,  1840,   -58,   101,   104,
     944,   -58,    38,   944,   -58,   944,   -58,   529,   -58,   559,
     107,   -58,  1801,  1801,   -58,   -58,   314,  1098,   -58,   105,
     158,  1137,   -58,  1527,   624,   -58,   654,   -58,   -58,   110,
     -58,   844,   944,   -58,   -58,   719,   -58,   -58,   -58,   844,
     -58,  1566,   -58,   -58,   -58,   -58,   749,   814,   844,   -58,
     -58
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       3,     0,     2,     1,    40,    43,    41,    42,    45,    44,
       0,     0,     0,     0,     0,     3,     0,     0,     0,    55,
       0,     0,     0,    56,     0,    27,    96,     0,     4,     0,
      96,     0,     0,    26,     0,     0,     0,     0,     0,     0,
       0,    21,     0,    23,    80,    81,    82,    86,     0,     0,
      46,     0,     0,     0,    98,     0,    97,     0,   106,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    89,    20,     0,    97,     0,     0,
       0,    25,     0,     0,     0,     0,    38,     3,     0,    22,
      24,     0,     0,    85,    96,    83,   107,    99,    84,     0,
       0,     0,     0,    77,    57,     0,    67,    66,    71,    72,
      73,    74,    75,    76,    68,    69,    70,    78,    79,    60,
      61,    62,    63,    64,    65,    47,     0,     0,     0,     0,
       0,    59,     0,     3,     0,     0,     0,     0,    16,     0,
      12,    39,    34,     0,     0,    28,    35,    94,     0,     0,
       0,     0,   100,   102,   104,     0,   103,     0,    90,    92,
      91,     0,    48,     0,    96,     0,     0,    30,     0,     0,
       0,    18,     3,    14,     3,     0,    29,    38,     0,    51,
     108,   101,   105,    58,     0,     0,    87,    49,     0,     0,
       0,    53,    32,     0,     6,     0,     3,     0,     3,     0,
       0,    36,    95,    93,    88,    50,    96,     0,     3,     0,
       0,     0,     3,     0,     0,    17,     0,    13,     3,     0,
      54,    33,     0,     5,    10,     0,     8,    19,    15,    37,
      52,     0,     3,     7,     3,     3,     0,     0,    31,    11,
       9
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -58,   -58,   -15,   -58,   -58,   -58,   -58,   -58,   -58,   -58,
     -58,   -58,   -58,   -58,   -58,   -13,   -58,    -8,   -58,   -12,
     -58,   -58,   -29,   -24,   -58,   -20
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     1,     2,    28,   222,   244,   242,   184,   208,   182,
     206,   202,   220,    98,   187,   152,   153,    29,   140,   169,
     195,    49,    55,    87,    57,    58
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      39,    86,    56,    89,    34,   110,    82,    83,    84,   111,
      44,     3,    45,    46,    48,   164,    51,    40,    54,    59,
      30,    42,    54,    88,   102,   103,   135,    35,    93,    31,
      95,   144,    37,   145,   138,    78,    79,    80,    81,    32,
      52,    50,    82,    83,    84,   107,   155,   156,   218,   219,
      92,    90,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,    41,   159,   136,   165,    43,   137,
      36,    94,   154,    60,   147,    38,   149,   139,   170,   163,
     166,   106,    99,   157,   158,   100,    54,   104,   109,   108,
     141,   162,   142,   146,    54,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,   151,   160,   167,    82,    83,    84,   177,   173,
     172,   175,   176,   174,   162,   180,   178,   179,    76,    77,
      78,    79,    80,    81,   189,   198,   185,    82,    83,    84,
      81,   192,   190,   191,    82,    83,    84,   194,   214,   193,
     205,   170,   215,   196,   216,   232,    54,   207,   228,   209,
     233,   240,     0,   210,   211,     0,     0,    54,     0,     0,
     212,     0,     0,     0,     0,     0,   213,   229,     0,     0,
       0,   224,   217,   226,     0,   221,     0,   223,     0,     0,
       0,     0,     0,   231,     0,     0,     0,   235,    54,     0,
       0,     0,     0,   239,     4,     5,     6,     7,     8,     9,
       0,     0,     0,     0,   241,     0,     0,   246,     0,   247,
     248,     0,     4,     5,     6,     7,     8,     9,     0,     0,
       0,     0,     0,    52,     0,     0,     0,     0,     0,     0,
       4,     5,     6,     7,     8,     9,     0,     0,     0,    18,
       0,    52,     0,    19,    20,    21,     0,     0,     0,    22,
      23,    24,   168,     0,     0,     0,    26,    18,    27,     0,
      53,    19,    20,    21,     0,     0,     0,    22,    23,    24,
       0,     0,     0,     0,    26,    18,    27,     0,   161,    19,
      20,    21,     0,     0,     0,    22,    23,    24,     0,     0,
       0,     0,    26,     0,    27,     0,    53,     4,     5,     6,
       7,     8,     9,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     4,     5,     6,     7,     8,
       9,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,     0,     0,     0,    82,    83,    84,     0,     0,
       0,     0,    18,     0,     0,     0,    19,    20,    21,     0,
       0,     0,    22,    23,    24,     0,     0,     0,     0,    26,
      18,    27,     0,    53,    19,    20,    21,     0,     0,     0,
      22,    23,    24,     0,     0,     0,     0,    26,     0,    27,
       0,   161,     4,     5,     6,     7,     8,     9,    10,     0,
       0,     0,    11,     0,     0,    12,    13,     0,    14,     0,
      15,     0,    96,    97,     0,    16,    17,    74,    75,    76,
      77,    78,    79,    80,    81,     0,     0,     0,    82,    83,
      84,     0,     0,     0,     0,     0,     0,    18,     0,     0,
       0,    19,    20,    21,     0,     0,     0,    22,    23,    24,
       0,     0,     0,    25,    26,     0,    27,     4,     5,     6,
       7,     8,     9,    10,     0,     0,     0,    11,     0,     0,
      12,    13,     0,    14,     0,    15,   186,     0,     0,     0,
      16,    17,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    18,     0,     0,     0,    19,    20,    21,     0,
       0,     0,    22,    23,    24,     0,     0,     0,    25,    26,
       0,    27,     4,     5,     6,     7,     8,     9,    10,     0,
       0,     0,    11,     0,     0,    12,    13,   225,    14,     0,
      15,     0,     0,     0,     0,    16,    17,     0,     0,     0,
       0,     0,     4,     5,     6,     7,     8,     9,    10,     0,
       0,     0,    11,     0,     0,    12,    13,    18,    14,   227,
      15,    19,    20,    21,     0,    16,    17,    22,    23,    24,
       0,     0,     0,    25,    26,     0,    27,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    18,     0,     0,
       0,    19,    20,    21,     0,     0,     0,    22,    23,    24,
       0,     0,     0,    25,    26,     0,    27,     4,     5,     6,
       7,     8,     9,    10,     0,     0,     0,    11,     0,     0,
      12,    13,   237,    14,     0,    15,     0,     0,     0,     0,
      16,    17,     0,     0,     0,     0,     0,     4,     5,     6,
       7,     8,     9,    10,     0,     0,     0,    11,     0,     0,
      12,    13,    18,    14,   238,    15,    19,    20,    21,     0,
      16,    17,    22,    23,    24,     0,     0,     0,    25,    26,
       0,    27,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    18,     0,     0,     0,    19,    20,    21,     0,
       0,     0,    22,    23,    24,     0,     0,     0,    25,    26,
       0,    27,     4,     5,     6,     7,     8,     9,    10,     0,
       0,     0,    11,     0,   243,    12,    13,     0,    14,     0,
      15,     0,     0,     0,     0,    16,    17,     0,     0,     0,
       0,     0,     4,     5,     6,     7,     8,     9,    10,     0,
       0,     0,    11,     0,   249,    12,    13,    18,    14,     0,
      15,    19,    20,    21,     0,    16,    17,    22,    23,    24,
       0,     0,     0,    25,    26,     0,    27,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    18,     0,     0,
       0,    19,    20,    21,     0,     0,     0,    22,    23,    24,
       0,     0,     0,    25,    26,     0,    27,     4,     5,     6,
       7,     8,     9,    10,     0,     0,     0,    11,     0,   250,
      12,    13,     0,    14,     0,    15,     0,     0,     0,     0,
      16,    17,     0,     0,     0,     0,     0,     4,     5,     6,
       7,     8,     9,    10,     0,     0,     0,    11,     0,     0,
      12,    13,    18,    14,     0,    15,    19,    20,    21,     0,
      16,    17,    22,    23,    24,     0,     0,     0,    25,    26,
       0,    27,     4,     5,     6,     7,     8,     9,     0,     0,
       0,     0,    18,     0,     0,     0,    19,    20,    21,     0,
       0,     0,    22,    23,    24,     0,     0,     0,    25,    26,
       0,    27,     4,     5,     6,     7,     8,     9,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    18,     0,     0,
       0,    19,    20,    21,     0,     0,     0,    22,    23,    24,
       0,     0,     0,    33,    26,     0,    27,     4,     5,     6,
       7,     8,     9,     0,     0,     0,     0,    18,     0,     0,
       0,    19,    20,    21,     0,     0,     0,    22,    23,    24,
       0,     0,    47,     0,    26,     0,    27,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    18,     0,     0,    60,    19,    20,    21,     0,
       0,     0,    22,    23,    24,     0,     0,     0,     0,    26,
       0,    27,    61,    62,     0,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    60,     0,     0,    82,    83,    84,
       0,     0,     0,     0,     0,    85,     0,     0,     0,     0,
       0,    61,    62,     0,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    60,     0,     0,    82,    83,    84,     0,
       0,     0,     0,     0,    91,     0,     0,   200,     0,     0,
      61,    62,     0,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    60,     0,     0,    82,    83,    84,     0,     0,
       0,     0,   201,     0,     0,     0,     0,     0,     0,    61,
      62,     0,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    60,     0,     0,    82,    83,    84,     0,     0,     0,
       0,   230,     0,     0,     0,     0,     0,     0,    61,    62,
       0,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      60,     0,     0,    82,    83,    84,     0,     0,     0,     0,
     234,     0,     0,     0,     0,     0,     0,    61,    62,     0,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    60,
       0,     0,    82,    83,    84,     0,     0,   105,     0,     0,
       0,     0,     0,     0,     0,     0,    61,    62,     0,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    60,     0,
       0,    82,    83,    84,     0,     0,   143,     0,     0,     0,
       0,     0,     0,     0,     0,    61,    62,     0,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    60,     0,     0,
      82,    83,    84,     0,     0,   148,     0,     0,     0,     0,
       0,     0,     0,     0,    61,    62,     0,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    60,     0,     0,    82,
      83,    84,     0,     0,   150,     0,     0,     0,     0,     0,
       0,     0,     0,    61,    62,     0,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    60,     0,     0,    82,    83,
      84,     0,     0,   181,     0,     0,     0,     0,     0,     0,
       0,     0,    61,    62,     0,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    60,     0,     0,    82,    83,    84,
       0,     0,   183,     0,     0,     0,     0,     0,     0,     0,
       0,    61,    62,     0,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    60,     0,     0,    82,    83,    84,     0,
       0,   197,     0,     0,     0,     0,     0,     0,     0,     0,
      61,    62,     0,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    60,     0,     0,    82,    83,    84,     0,     0,
     199,     0,     0,     0,     0,     0,     0,     0,     0,    61,
      62,     0,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    60,     0,     0,    82,    83,    84,     0,     0,   204,
       0,     0,     0,     0,     0,     0,     0,     0,    61,    62,
       0,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      60,     0,     0,    82,    83,    84,     0,     0,   236,     0,
       0,     0,     0,     0,     0,     0,     0,    61,    62,     0,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    60,
       0,     0,    82,    83,    84,     0,     0,   245,     0,     0,
       0,     0,     0,     0,     0,   101,    61,    62,     0,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    60,     0,
       0,    82,    83,    84,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    61,    62,     0,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,   112,    60,     0,
      82,    83,    84,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    61,    62,   171,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    60,     0,     0,
      82,    83,    84,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   188,    61,    62,     0,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    60,     0,     0,    82,
      83,    84,     0,     0,     0,     0,     0,     0,     0,     0,
     203,     0,     0,    61,    62,     0,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    60,     0,     0,    82,    83,
      84,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    61,    62,     0,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    60,     0,     0,    82,    83,    84,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    -1,     0,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,     0,     0,     0,    82,    83,    84
};

static const yytype_int16 yycheck[] =
{
      15,    30,    26,    14,    12,    62,    56,    57,    58,    66,
      18,     0,    20,    21,    22,     7,    24,     7,    26,    27,
      60,     7,    30,    31,    62,    63,     7,     7,    36,    60,
      38,    58,     7,    60,     7,    49,    50,    51,    52,     7,
      32,     7,    56,    57,    58,    53,    22,    23,    10,    11,
      60,    62,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    64,   104,    57,    69,    64,    60,
      60,    60,    97,    14,    92,    60,    94,    60,   112,   109,
     110,     7,    64,   101,   102,    64,   104,    60,    62,    66,
      61,   109,    62,     7,   112,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,     7,    31,    31,    56,    57,    58,   143,   137,
       7,   139,   140,    60,   142,    14,   144,   145,    47,    48,
      49,    50,    51,    52,    61,   174,    60,    56,    57,    58,
      52,     7,   160,   161,    56,    57,    58,    29,    68,   167,
      60,   185,    61,   171,    60,    60,   174,   182,    61,   184,
      12,    61,    -1,   185,   187,    -1,    -1,   185,    -1,    -1,
     188,    -1,    -1,    -1,    -1,    -1,   194,   216,    -1,    -1,
      -1,   206,   200,   208,    -1,   203,    -1,   205,    -1,    -1,
      -1,    -1,    -1,   218,    -1,    -1,    -1,   222,   216,    -1,
      -1,    -1,    -1,   228,     3,     4,     5,     6,     7,     8,
      -1,    -1,    -1,    -1,   232,    -1,    -1,   242,    -1,   244,
     245,    -1,     3,     4,     5,     6,     7,     8,    -1,    -1,
      -1,    -1,    -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,     6,     7,     8,    -1,    -1,    -1,    48,
      -1,    32,    -1,    52,    53,    54,    -1,    -1,    -1,    58,
      59,    60,    25,    -1,    -1,    -1,    65,    48,    67,    -1,
      69,    52,    53,    54,    -1,    -1,    -1,    58,    59,    60,
      -1,    -1,    -1,    -1,    65,    48,    67,    -1,    69,    52,
      53,    54,    -1,    -1,    -1,    58,    59,    60,    -1,    -1,
      -1,    -1,    65,    -1,    67,    -1,    69,     3,     4,     5,
       6,     7,     8,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,
       8,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    -1,    -1,    -1,    56,    57,    58,    -1,    -1,
      -1,    -1,    48,    -1,    -1,    -1,    52,    53,    54,    -1,
      -1,    -1,    58,    59,    60,    -1,    -1,    -1,    -1,    65,
      48,    67,    -1,    69,    52,    53,    54,    -1,    -1,    -1,
      58,    59,    60,    -1,    -1,    -1,    -1,    65,    -1,    67,
      -1,    69,     3,     4,     5,     6,     7,     8,     9,    -1,
      -1,    -1,    13,    -1,    -1,    16,    17,    -1,    19,    -1,
      21,    -1,    23,    24,    -1,    26,    27,    45,    46,    47,
      48,    49,    50,    51,    52,    -1,    -1,    -1,    56,    57,
      58,    -1,    -1,    -1,    -1,    -1,    -1,    48,    -1,    -1,
      -1,    52,    53,    54,    -1,    -1,    -1,    58,    59,    60,
      -1,    -1,    -1,    64,    65,    -1,    67,     3,     4,     5,
       6,     7,     8,     9,    -1,    -1,    -1,    13,    -1,    -1,
      16,    17,    -1,    19,    -1,    21,    22,    -1,    -1,    -1,
      26,    27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    48,    -1,    -1,    -1,    52,    53,    54,    -1,
      -1,    -1,    58,    59,    60,    -1,    -1,    -1,    64,    65,
      -1,    67,     3,     4,     5,     6,     7,     8,     9,    -1,
      -1,    -1,    13,    -1,    -1,    16,    17,    18,    19,    -1,
      21,    -1,    -1,    -1,    -1,    26,    27,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,     7,     8,     9,    -1,
      -1,    -1,    13,    -1,    -1,    16,    17,    48,    19,    20,
      21,    52,    53,    54,    -1,    26,    27,    58,    59,    60,
      -1,    -1,    -1,    64,    65,    -1,    67,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,    -1,    -1,
      -1,    52,    53,    54,    -1,    -1,    -1,    58,    59,    60,
      -1,    -1,    -1,    64,    65,    -1,    67,     3,     4,     5,
       6,     7,     8,     9,    -1,    -1,    -1,    13,    -1,    -1,
      16,    17,    18,    19,    -1,    21,    -1,    -1,    -1,    -1,
      26,    27,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,     7,     8,     9,    -1,    -1,    -1,    13,    -1,    -1,
      16,    17,    48,    19,    20,    21,    52,    53,    54,    -1,
      26,    27,    58,    59,    60,    -1,    -1,    -1,    64,    65,
      -1,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    48,    -1,    -1,    -1,    52,    53,    54,    -1,
      -1,    -1,    58,    59,    60,    -1,    -1,    -1,    64,    65,
      -1,    67,     3,     4,     5,     6,     7,     8,     9,    -1,
      -1,    -1,    13,    -1,    15,    16,    17,    -1,    19,    -1,
      21,    -1,    -1,    -1,    -1,    26,    27,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,     7,     8,     9,    -1,
      -1,    -1,    13,    -1,    15,    16,    17,    48,    19,    -1,
      21,    52,    53,    54,    -1,    26,    27,    58,    59,    60,
      -1,    -1,    -1,    64,    65,    -1,    67,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,    -1,    -1,
      -1,    52,    53,    54,    -1,    -1,    -1,    58,    59,    60,
      -1,    -1,    -1,    64,    65,    -1,    67,     3,     4,     5,
       6,     7,     8,     9,    -1,    -1,    -1,    13,    -1,    15,
      16,    17,    -1,    19,    -1,    21,    -1,    -1,    -1,    -1,
      26,    27,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,     7,     8,     9,    -1,    -1,    -1,    13,    -1,    -1,
      16,    17,    48,    19,    -1,    21,    52,    53,    54,    -1,
      26,    27,    58,    59,    60,    -1,    -1,    -1,    64,    65,
      -1,    67,     3,     4,     5,     6,     7,     8,    -1,    -1,
      -1,    -1,    48,    -1,    -1,    -1,    52,    53,    54,    -1,
      -1,    -1,    58,    59,    60,    -1,    -1,    -1,    64,    65,
      -1,    67,     3,     4,     5,     6,     7,     8,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,    -1,    -1,
      -1,    52,    53,    54,    -1,    -1,    -1,    58,    59,    60,
      -1,    -1,    -1,    64,    65,    -1,    67,     3,     4,     5,
       6,     7,     8,    -1,    -1,    -1,    -1,    48,    -1,    -1,
      -1,    52,    53,    54,    -1,    -1,    -1,    58,    59,    60,
      -1,    -1,    63,    -1,    65,    -1,    67,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    48,    -1,    -1,    14,    52,    53,    54,    -1,
      -1,    -1,    58,    59,    60,    -1,    -1,    -1,    -1,    65,
      -1,    67,    31,    32,    -1,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    14,    -1,    -1,    56,    57,    58,
      -1,    -1,    -1,    -1,    -1,    64,    -1,    -1,    -1,    -1,
      -1,    31,    32,    -1,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    14,    -1,    -1,    56,    57,    58,    -1,
      -1,    -1,    -1,    -1,    64,    -1,    -1,    28,    -1,    -1,
      31,    32,    -1,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    14,    -1,    -1,    56,    57,    58,    -1,    -1,
      -1,    -1,    63,    -1,    -1,    -1,    -1,    -1,    -1,    31,
      32,    -1,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    14,    -1,    -1,    56,    57,    58,    -1,    -1,    -1,
      -1,    63,    -1,    -1,    -1,    -1,    -1,    -1,    31,    32,
      -1,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      14,    -1,    -1,    56,    57,    58,    -1,    -1,    -1,    -1,
      63,    -1,    -1,    -1,    -1,    -1,    -1,    31,    32,    -1,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    14,
      -1,    -1,    56,    57,    58,    -1,    -1,    61,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    31,    32,    -1,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    14,    -1,
      -1,    56,    57,    58,    -1,    -1,    61,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    31,    32,    -1,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    14,    -1,    -1,
      56,    57,    58,    -1,    -1,    61,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    31,    32,    -1,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    14,    -1,    -1,    56,
      57,    58,    -1,    -1,    61,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    31,    32,    -1,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    14,    -1,    -1,    56,    57,
      58,    -1,    -1,    61,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    31,    32,    -1,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    14,    -1,    -1,    56,    57,    58,
      -1,    -1,    61,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    31,    32,    -1,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    14,    -1,    -1,    56,    57,    58,    -1,
      -1,    61,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      31,    32,    -1,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    14,    -1,    -1,    56,    57,    58,    -1,    -1,
      61,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,
      32,    -1,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    14,    -1,    -1,    56,    57,    58,    -1,    -1,    61,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,    32,
      -1,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      14,    -1,    -1,    56,    57,    58,    -1,    -1,    61,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,    32,    -1,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    14,
      -1,    -1,    56,    57,    58,    -1,    -1,    61,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    30,    31,    32,    -1,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    14,    -1,
      -1,    56,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    31,    32,    -1,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    14,    -1,
      56,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    14,    -1,    -1,
      56,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    30,    31,    32,    -1,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    14,    -1,    -1,    56,
      57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      28,    -1,    -1,    31,    32,    -1,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    14,    -1,    -1,    56,    57,
      58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    31,    32,    -1,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    14,    -1,    -1,    56,    57,    58,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    32,    -1,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    -1,    -1,    -1,    56,    57,    58
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    71,    72,     0,     3,     4,     5,     6,     7,     8,
       9,    13,    16,    17,    19,    21,    26,    27,    48,    52,
      53,    54,    58,    59,    60,    64,    65,    67,    73,    87,
      60,    60,     7,    64,    87,     7,    60,     7,    60,    72,
       7,    64,     7,    64,    87,    87,    87,    63,    87,    91,
       7,    87,    32,    69,    87,    92,    93,    94,    95,    87,
      14,    31,    32,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    56,    57,    58,    64,    92,    93,    87,    14,
      62,    64,    60,    87,    60,    87,    23,    24,    83,    64,
      64,    30,    62,    63,    60,    61,     7,    87,    66,    62,
      62,    66,    53,    87,    87,    87,    87,    87,    87,    87,
      87,    87,    87,    87,    87,    87,    87,    87,    87,    87,
      87,    87,    87,    87,    87,     7,    57,    60,     7,    60,
      88,    61,    62,    61,    58,    60,     7,    87,    61,    87,
      61,     7,    85,    86,    72,    22,    23,    87,    87,    92,
      31,    69,    87,    95,     7,    69,    95,    31,    25,    89,
      93,    33,     7,    87,    60,    87,    87,    72,    87,    87,
      14,    61,    79,    61,    77,    60,    22,    84,    30,    61,
      87,    87,     7,    87,    29,    90,    87,    61,    92,    61,
      28,    63,    81,    28,    61,    60,    80,    72,    78,    72,
      89,    85,    87,    87,    68,    61,    60,    87,    10,    11,
      82,    87,    74,    87,    72,    18,    72,    20,    61,    92,
      63,    72,    60,    12,    63,    72,    61,    18,    20,    72,
      61,    87,    76,    15,    75,    61,    72,    72,    72,    15,
      15
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    70,    71,    72,    72,    73,    74,    73,    75,    73,
      76,    73,    77,    73,    78,    73,    79,    73,    80,    73,
      73,    73,    73,    73,    73,    73,    73,    73,    73,    73,
      81,    81,    82,    82,    83,    84,    83,    85,    86,    86,
      87,    87,    87,    87,    87,    87,    87,    87,    87,    87,
      87,    87,    87,    87,    87,    87,    87,    87,    87,    87,
      87,    87,    87,    87,    87,    87,    87,    87,    87,    87,
      87,    87,    87,    87,    87,    87,    87,    87,    87,    87,
      87,    87,    87,    87,    87,    87,    87,    87,    87,    88,
      89,    89,    90,    90,    91,    91,    92,    92,    93,    93,
      93,    93,    94,    94,    94,    94,    94,    95,    95
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     0,     2,     8,     0,     9,     0,    11,
       0,    11,     0,     7,     0,     8,     0,     7,     0,     8,
       2,     2,     3,     2,     3,     3,     2,     1,     4,     5,
       0,     6,     0,     2,     2,     0,     4,     5,     0,     1,
       1,     1,     1,     1,     1,     1,     2,     3,     4,     5,
       6,     5,     8,     5,     7,     1,     1,     3,     5,     4,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       2,     2,     2,     3,     3,     3,     2,     5,     6,     0,
       1,     1,     0,     2,     3,     5,     0,     1,     1,     2,
       3,     4,     3,     3,     3,     4,     1,     2,     4
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
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






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
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
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: statements  */
#line 118 "src/parser.y"
                { prog_start = (yyvsp[0].stmt); }
#line 1709 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 3: /* statements: %empty  */
#line 123 "src/parser.y"
                { (yyval.stmt) = 0; }
#line 1715 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 4: /* statements: statements statement  */
#line 125 "src/parser.y"
                {
		    if ((yyvsp[-1].stmt)) {
			Stmt *tmp = (yyvsp[-1].stmt);
			
			while (tmp->next)
			    tmp = tmp->next;
			tmp->next = (yyvsp[0].stmt);
			(yyval.stmt) = (yyvsp[-1].stmt);
		    } else
			(yyval.stmt) = (yyvsp[0].stmt);
		}
#line 1731 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 5: /* statement: tIF '(' expr ')' statements elseifs elsepart tENDIF  */
#line 140 "src/parser.y"
                {

		    (yyval.stmt) = alloc_stmt(STMT_COND);
		    (yyval.stmt)->s.cond.arms = alloc_cond_arm((yyvsp[-5].expr), (yyvsp[-3].stmt));
		    (yyval.stmt)->s.cond.arms->next = (yyvsp[-2].arm);
		    (yyval.stmt)->s.cond.otherwise = (yyvsp[-1].stmt);
		}
#line 1743 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 6: /* $@1: %empty  */
#line 148 "src/parser.y"
                {
		    push_loop_name((yyvsp[-4].string));
		}
#line 1751 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 7: /* statement: tFOR tID tIN '(' expr ')' $@1 statements tENDFOR  */
#line 152 "src/parser.y"
                {
		    (yyval.stmt) = alloc_stmt(STMT_LIST);
		    (yyval.stmt)->s.list.id = find_id((yyvsp[-7].string));
		    (yyval.stmt)->s.list.index = -1;
		    (yyval.stmt)->s.list.expr = (yyvsp[-4].expr);
		    (yyval.stmt)->s.list.body = (yyvsp[-1].stmt);
		    pop_loop_name();
		}
#line 1764 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 8: /* $@2: %empty  */
#line 161 "src/parser.y"
                {
		    push_loop_name((yyvsp[-6].string));
		    push_loop_name((yyvsp[-4].string));
		}
#line 1773 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 9: /* statement: tFOR tID ',' tID tIN '(' expr ')' $@2 statements tENDFOR  */
#line 166 "src/parser.y"
                {
		    (yyval.stmt) = alloc_stmt(STMT_LIST);
		    (yyval.stmt)->s.list.id = find_id((yyvsp[-9].string));
		    (yyval.stmt)->s.list.index = find_id((yyvsp[-7].string));
		    (yyval.stmt)->s.list.expr = (yyvsp[-4].expr);
		    (yyval.stmt)->s.list.body = (yyvsp[-1].stmt);
		    pop_loop_name();
		    pop_loop_name();
		}
#line 1787 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 10: /* $@3: %empty  */
#line 176 "src/parser.y"
                {
		    push_loop_name((yyvsp[-6].string));
		}
#line 1795 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 11: /* statement: tFOR tID tIN '[' expr tTO expr ']' $@3 statements tENDFOR  */
#line 180 "src/parser.y"
                {
		    (yyval.stmt) = alloc_stmt(STMT_RANGE);
		    (yyval.stmt)->s.range.id = find_id((yyvsp[-9].string));
		    (yyval.stmt)->s.range.from = (yyvsp[-6].expr);
		    (yyval.stmt)->s.range.to = (yyvsp[-4].expr);
		    (yyval.stmt)->s.range.body = (yyvsp[-1].stmt);
		    pop_loop_name();
		}
#line 1808 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 12: /* $@4: %empty  */
#line 189 "src/parser.y"
                {
		    push_loop_name(0);
		}
#line 1816 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 13: /* statement: tWHILE '(' expr ')' $@4 statements tENDWHILE  */
#line 193 "src/parser.y"
                {
		    (yyval.stmt) = alloc_stmt(STMT_WHILE);
		    (yyval.stmt)->s.loop.id = -1;
		    (yyval.stmt)->s.loop.condition = (yyvsp[-4].expr);
		    (yyval.stmt)->s.loop.body = (yyvsp[-1].stmt);
		    pop_loop_name();
		}
#line 1828 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 14: /* $@5: %empty  */
#line 201 "src/parser.y"
                {
		    push_loop_name((yyvsp[-3].string));
		}
#line 1836 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 15: /* statement: tWHILE tID '(' expr ')' $@5 statements tENDWHILE  */
#line 205 "src/parser.y"
                {
		    (yyval.stmt) = alloc_stmt(STMT_WHILE);
		    (yyval.stmt)->s.loop.id = find_id((yyvsp[-6].string));
		    (yyval.stmt)->s.loop.condition = (yyvsp[-4].expr);
		    (yyval.stmt)->s.loop.body = (yyvsp[-1].stmt);
		    pop_loop_name();
		}
#line 1848 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 16: /* $@6: %empty  */
#line 213 "src/parser.y"
                {
		    suspend_loop_scope();
		}
#line 1856 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 17: /* statement: tFORK '(' expr ')' $@6 statements tENDFORK  */
#line 217 "src/parser.y"
                {
		    (yyval.stmt) = alloc_stmt(STMT_FORK);
		    (yyval.stmt)->s.fork.id = -1;
		    (yyval.stmt)->s.fork.time = (yyvsp[-4].expr);
		    (yyval.stmt)->s.fork.body = (yyvsp[-1].stmt);
		    resume_loop_scope();
		}
#line 1868 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 18: /* $@7: %empty  */
#line 225 "src/parser.y"
                {
		    suspend_loop_scope();
		}
#line 1876 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 19: /* statement: tFORK tID '(' expr ')' $@7 statements tENDFORK  */
#line 229 "src/parser.y"
                {
		    (yyval.stmt) = alloc_stmt(STMT_FORK);
		    (yyval.stmt)->s.fork.id = find_id((yyvsp[-6].string));
		    (yyval.stmt)->s.fork.time = (yyvsp[-4].expr);
		    (yyval.stmt)->s.fork.body = (yyvsp[-1].stmt);
		    resume_loop_scope();
		}
#line 1888 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 20: /* statement: expr ';'  */
#line 237 "src/parser.y"
                {
		    (yyval.stmt) = alloc_stmt(STMT_EXPR);
		    (yyval.stmt)->s.expr = (yyvsp[-1].expr);
		}
#line 1897 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 21: /* statement: tBREAK ';'  */
#line 242 "src/parser.y"
                {
		    check_loop_name(0, LOOP_BREAK);
		    (yyval.stmt) = alloc_stmt(STMT_BREAK);
		    (yyval.stmt)->s.exit = -1;
		}
#line 1907 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 22: /* statement: tBREAK tID ';'  */
#line 248 "src/parser.y"
                {
		    check_loop_name((yyvsp[-1].string), LOOP_BREAK);
		    (yyval.stmt) = alloc_stmt(STMT_BREAK);
		    (yyval.stmt)->s.exit = find_id((yyvsp[-1].string));
		}
#line 1917 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 23: /* statement: tCONTINUE ';'  */
#line 254 "src/parser.y"
                {
		    check_loop_name(0, LOOP_CONTINUE);
		    (yyval.stmt) = alloc_stmt(STMT_CONTINUE);
		    (yyval.stmt)->s.exit = -1;
		}
#line 1927 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 24: /* statement: tCONTINUE tID ';'  */
#line 260 "src/parser.y"
                {
		    check_loop_name((yyvsp[-1].string), LOOP_CONTINUE);
		    (yyval.stmt) = alloc_stmt(STMT_CONTINUE);
		    (yyval.stmt)->s.exit = find_id((yyvsp[-1].string));
		}
#line 1937 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 25: /* statement: tRETURN expr ';'  */
#line 266 "src/parser.y"
                {
		    (yyval.stmt) = alloc_stmt(STMT_RETURN);
		    (yyval.stmt)->s.expr = (yyvsp[-1].expr);
		}
#line 1946 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 26: /* statement: tRETURN ';'  */
#line 271 "src/parser.y"
                {
		    (yyval.stmt) = alloc_stmt(STMT_RETURN);
		    (yyval.stmt)->s.expr = 0;
		}
#line 1955 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 27: /* statement: ';'  */
#line 276 "src/parser.y"
                { (yyval.stmt) = 0; }
#line 1961 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 28: /* statement: tTRY statements excepts tENDTRY  */
#line 278 "src/parser.y"
                {
		    (yyval.stmt) = alloc_stmt(STMT_TRY_EXCEPT);
		    (yyval.stmt)->s._catch.body = (yyvsp[-2].stmt);
		    (yyval.stmt)->s._catch.excepts = (yyvsp[-1].except);
		}
#line 1971 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 29: /* statement: tTRY statements tFINALLY statements tENDTRY  */
#line 284 "src/parser.y"
                {
		    (yyval.stmt) = alloc_stmt(STMT_TRY_FINALLY);
		    (yyval.stmt)->s.finally.body = (yyvsp[-3].stmt);
		    (yyval.stmt)->s.finally.handler = (yyvsp[-1].stmt);
		}
#line 1981 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 30: /* elseifs: %empty  */
#line 293 "src/parser.y"
                { (yyval.arm) = 0; }
#line 1987 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 31: /* elseifs: elseifs tELSEIF '(' expr ')' statements  */
#line 295 "src/parser.y"
                {
		    Cond_Arm *this_arm = alloc_cond_arm((yyvsp[-2].expr), (yyvsp[0].stmt));
		    
		    if ((yyvsp[-5].arm)) {
		        Cond_Arm *tmp = (yyvsp[-5].arm);

			while (tmp->next)
			    tmp = tmp->next;
			tmp->next = this_arm;
			(yyval.arm) = (yyvsp[-5].arm);
		    } else
			(yyval.arm) = this_arm;
		}
#line 2005 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 32: /* elsepart: %empty  */
#line 312 "src/parser.y"
                { (yyval.stmt) = 0; }
#line 2011 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 33: /* elsepart: tELSE statements  */
#line 314 "src/parser.y"
                { (yyval.stmt) = (yyvsp[0].stmt); }
#line 2017 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 34: /* excepts: tEXCEPT except  */
#line 319 "src/parser.y"
                { (yyval.except) = (yyvsp[0].except); }
#line 2023 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 35: /* $@8: %empty  */
#line 321 "src/parser.y"
                {
		    Except_Arm *tmp = (yyvsp[-1].except);
		    int        count = 1;
		    
		    while (tmp->next) {
			tmp = tmp->next;
			count++;
		    }
		    if (!tmp->codes)
			yyerror("Unreachable EXCEPT clause");
		    else if (count > 255)
			yyerror("Too many EXCEPT clauses (max. 255)");
		}
#line 2041 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 36: /* excepts: excepts tEXCEPT $@8 except  */
#line 335 "src/parser.y"
                {
		    Except_Arm *tmp = (yyvsp[-3].except);
		    
		    while (tmp->next)
			tmp = tmp->next;

		    tmp->next = (yyvsp[0].except);
		    (yyval.except) = (yyvsp[-3].except);
		}
#line 2055 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 37: /* except: opt_id '(' codes ')' statements  */
#line 348 "src/parser.y"
                { (yyval.except) = alloc_except((yyvsp[-4].string) ? find_id((yyvsp[-4].string)) : -1, (yyvsp[-2].args), (yyvsp[0].stmt)); }
#line 2061 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 38: /* opt_id: %empty  */
#line 353 "src/parser.y"
                { (yyval.string) = 0; }
#line 2067 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 39: /* opt_id: tID  */
#line 355 "src/parser.y"
                { (yyval.string) = (yyvsp[0].string); }
#line 2073 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 40: /* expr: tINTEGER  */
#line 360 "src/parser.y"
                {
		    (yyval.expr) = alloc_var(TYPE_INT);
		    (yyval.expr)->e.var.v.num = (yyvsp[0].integer);
		}
#line 2082 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 41: /* expr: tFLOAT  */
#line 365 "src/parser.y"
                {
		    (yyval.expr) = alloc_var(TYPE_FLOAT);
		    (yyval.expr)->e.var.v.fnum = (yyvsp[0].real);
		}
#line 2091 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 42: /* expr: tSTRING  */
#line 370 "src/parser.y"
                {
		    (yyval.expr) = alloc_var(TYPE_STR);
		    (yyval.expr)->e.var.v.str = (yyvsp[0].string);
		}
#line 2100 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 43: /* expr: tOBJECT  */
#line 375 "src/parser.y"
                {
		    (yyval.expr) = alloc_var(TYPE_OBJ);
		    (yyval.expr)->e.var.v.obj = (yyvsp[0].object);
		}
#line 2109 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 44: /* expr: tERROR  */
#line 380 "src/parser.y"
                {
		    (yyval.expr) = alloc_var(TYPE_ERR);
		    (yyval.expr)->e.var.v.err = (yyvsp[0].error);
		}
#line 2118 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 45: /* expr: tID  */
#line 385 "src/parser.y"
                {
		    (yyval.expr) = alloc_expr(EXPR_ID);
		    (yyval.expr)->e.id = find_id((yyvsp[0].string));
		}
#line 2127 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 46: /* expr: '$' tID  */
#line 390 "src/parser.y"
                {
		    /* Treat $foo like #0.("foo") */
		    Expr *obj = alloc_var(TYPE_OBJ);
		    Expr *prop = alloc_var(TYPE_STR);
		    obj->e.var.v.obj = 0;
		    prop->e.var.v.str = (yyvsp[0].string);
		    (yyval.expr) = alloc_binary(EXPR_PROP, obj, prop);
		}
#line 2140 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 47: /* expr: expr '.' tID  */
#line 399 "src/parser.y"
                {
		    /* Treat foo.bar like foo.("bar") for simplicity */
		    Expr *prop = alloc_var(TYPE_STR);
		    prop->e.var.v.str = (yyvsp[0].string);
		    (yyval.expr) = alloc_binary(EXPR_PROP, (yyvsp[-2].expr), prop);
		}
#line 2151 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 48: /* expr: expr '.' ':' tID  */
#line 406 "src/parser.y"
                {
            /* Treat foo.:bar (waif properties) like foo.(":bar") 
               (we should be using  WAIF_PROP_PREFIX here...) */
		    Expr *prop = alloc_var(TYPE_STR);
			char *newstr;
            asprintf(&newstr, "%c%s", WAIF_PROP_PREFIX, (yyvsp[0].string));
			dealloc_string((yyvsp[0].string));
		    prop->e.var.v.str = alloc_string(newstr);
			free(newstr);
		    (yyval.expr) = alloc_binary(EXPR_PROP, (yyvsp[-3].expr), prop);
		}
#line 2167 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 49: /* expr: expr '.' '(' expr ')'  */
#line 418 "src/parser.y"
                {
		    (yyval.expr) = alloc_binary(EXPR_PROP, (yyvsp[-4].expr), (yyvsp[-1].expr));
		}
#line 2175 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 50: /* expr: expr ':' tID '(' arglist ')'  */
#line 422 "src/parser.y"
                {
		    /* treat foo:bar(args) like foo:("bar")(args) */
		    Expr *verb = alloc_var(TYPE_STR);
		    verb->e.var.v.str = (yyvsp[-3].string);
		    (yyval.expr) = alloc_verb((yyvsp[-5].expr), verb, (yyvsp[-1].args));
		}
#line 2186 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 51: /* expr: '$' tID '(' arglist ')'  */
#line 429 "src/parser.y"
                {
		    /* treat $bar(args) like #0:("bar")(args) */
		    Expr *obj = alloc_var(TYPE_OBJ);
		    Expr *verb = alloc_var(TYPE_STR);
		    obj->e.var.v.obj = 0;
		    verb->e.var.v.str = (yyvsp[-3].string);
		    (yyval.expr) = alloc_verb(obj, verb, (yyvsp[-1].args));
		}
#line 2199 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 52: /* expr: expr ':' '(' expr ')' '(' arglist ')'  */
#line 438 "src/parser.y"
                {
		    (yyval.expr) = alloc_verb((yyvsp[-7].expr), (yyvsp[-4].expr), (yyvsp[-1].args));
		}
#line 2207 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 53: /* expr: expr '[' dollars_up expr ']'  */
#line 442 "src/parser.y"
                {
		    dollars_ok--;
		    (yyval.expr) = alloc_binary(EXPR_INDEX, (yyvsp[-4].expr), (yyvsp[-1].expr));
		}
#line 2216 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 54: /* expr: expr '[' dollars_up expr tTO expr ']'  */
#line 447 "src/parser.y"
                {
		    dollars_ok--;
		    (yyval.expr) = alloc_expr(EXPR_RANGE);
		    (yyval.expr)->e.range.base = (yyvsp[-6].expr);
		    (yyval.expr)->e.range.from = (yyvsp[-3].expr);
		    (yyval.expr)->e.range.to = (yyvsp[-1].expr);
		}
#line 2228 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 55: /* expr: '^'  */
#line 455 "src/parser.y"
                {
		    if (!dollars_ok)
			yyerror("Illegal context for `^' expression.");
		    (yyval.expr) = alloc_expr(EXPR_FIRST);
		}
#line 2238 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 56: /* expr: '$'  */
#line 461 "src/parser.y"
                {
		    if (!dollars_ok)
			yyerror("Illegal context for `$' expression.");
		    (yyval.expr) = alloc_expr(EXPR_LAST);
		}
#line 2248 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 57: /* expr: expr '=' expr  */
#line 467 "src/parser.y"
                {
		    Expr *e = (yyvsp[-2].expr);

		    if (e->kind == EXPR_LIST) {
			e->kind = EXPR_SCATTER;
			if (e->e.list) {
			    e->e.scatter = scatter_from_arglist(e->e.list);
			    vet_scatter(e->e.scatter);
			} else
			    yyerror("Empty list in scattering assignment.");
		    } else {
			if (e->kind == EXPR_RANGE)
			    e = e->e.range.base;
			while (e->kind == EXPR_INDEX)
			    e = e->e.bin.lhs;
			if (e->kind != EXPR_ID  &&  e->kind != EXPR_PROP)
			    yyerror("Illegal expression on left side of"
				    " assignment.");
		    }
		    (yyval.expr) = alloc_binary(EXPR_ASGN, (yyvsp[-2].expr), (yyvsp[0].expr));
	        }
#line 2274 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 58: /* expr: '{' scatter '}' '=' expr  */
#line 489 "src/parser.y"
                {
		    Expr       *e = alloc_expr(EXPR_SCATTER);

		    e->e.scatter = (yyvsp[-3].scatter);
		    vet_scatter((yyvsp[-3].scatter));
		    (yyval.expr) = alloc_binary(EXPR_ASGN, e, (yyvsp[0].expr));
		}
#line 2286 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 59: /* expr: tID '(' arglist ')'  */
#line 497 "src/parser.y"
                {
		    unsigned f_no;

		    (yyval.expr) = alloc_expr(EXPR_CALL);
		    if ((f_no = number_func_by_name((yyvsp[-3].string))) == FUNC_NOT_FOUND) {
			/* Replace with call_function("$1", @args) */
			Expr           *fname = alloc_var(TYPE_STR);
			Arg_List       *a = alloc_arg_list(ARG_NORMAL, fname);

			fname->e.var.v.str = (yyvsp[-3].string);
			a->next = (yyvsp[-1].args);
			warning("Unknown built-in function: ", (yyvsp[-3].string));
			(yyval.expr)->e.call.func = number_func_by_name("call_function");
			(yyval.expr)->e.call.args = a;
		    } else {
			(yyval.expr)->e.call.func = f_no;
			(yyval.expr)->e.call.args = (yyvsp[-1].args);
			dealloc_string((yyvsp[-3].string));
		    }
		}
#line 2311 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 60: /* expr: expr '+' expr  */
#line 518 "src/parser.y"
                {
		    (yyval.expr) = alloc_binary(EXPR_PLUS, (yyvsp[-2].expr), (yyvsp[0].expr));
		}
#line 2319 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 61: /* expr: expr '-' expr  */
#line 522 "src/parser.y"
                {
		    (yyval.expr) = alloc_binary(EXPR_MINUS, (yyvsp[-2].expr), (yyvsp[0].expr));
		}
#line 2327 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 62: /* expr: expr '*' expr  */
#line 526 "src/parser.y"
                {
		    (yyval.expr) = alloc_binary(EXPR_TIMES, (yyvsp[-2].expr), (yyvsp[0].expr));
		}
#line 2335 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 63: /* expr: expr '/' expr  */
#line 530 "src/parser.y"
                {
		    (yyval.expr) = alloc_binary(EXPR_DIVIDE, (yyvsp[-2].expr), (yyvsp[0].expr));
		}
#line 2343 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 64: /* expr: expr '%' expr  */
#line 534 "src/parser.y"
                {
		    (yyval.expr) = alloc_binary(EXPR_MOD, (yyvsp[-2].expr), (yyvsp[0].expr));
		}
#line 2351 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 65: /* expr: expr '^' expr  */
#line 538 "src/parser.y"
                {
		    (yyval.expr) = alloc_binary(EXPR_EXP, (yyvsp[-2].expr), (yyvsp[0].expr));
		}
#line 2359 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 66: /* expr: expr tAND expr  */
#line 542 "src/parser.y"
                {
		    (yyval.expr) = alloc_binary(EXPR_AND, (yyvsp[-2].expr), (yyvsp[0].expr));
		}
#line 2367 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 67: /* expr: expr tOR expr  */
#line 546 "src/parser.y"
                {
		    (yyval.expr) = alloc_binary(EXPR_OR, (yyvsp[-2].expr), (yyvsp[0].expr));
		}
#line 2375 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 68: /* expr: expr tBITOR expr  */
#line 550 "src/parser.y"
                {
		    (yyval.expr) = alloc_binary(EXPR_BITOR, (yyvsp[-2].expr), (yyvsp[0].expr));
		}
#line 2383 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 69: /* expr: expr tBITAND expr  */
#line 554 "src/parser.y"
                {
		    (yyval.expr) = alloc_binary(EXPR_BITAND, (yyvsp[-2].expr), (yyvsp[0].expr));
		}
#line 2391 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 70: /* expr: expr tBITXOR expr  */
#line 558 "src/parser.y"
                {
		    (yyval.expr) = alloc_binary(EXPR_BITXOR, (yyvsp[-2].expr), (yyvsp[0].expr));
		}
#line 2399 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 71: /* expr: expr tEQ expr  */
#line 562 "src/parser.y"
                {
		    (yyval.expr) = alloc_binary(EXPR_EQ, (yyvsp[-2].expr), (yyvsp[0].expr));
		}
#line 2407 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 72: /* expr: expr tNE expr  */
#line 566 "src/parser.y"
                {
		    (yyval.expr) = alloc_binary(EXPR_NE, (yyvsp[-2].expr), (yyvsp[0].expr));
		}
#line 2415 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 73: /* expr: expr '<' expr  */
#line 570 "src/parser.y"
                {
		    (yyval.expr) = alloc_binary(EXPR_LT, (yyvsp[-2].expr), (yyvsp[0].expr));
		}
#line 2423 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 74: /* expr: expr tLE expr  */
#line 574 "src/parser.y"
                {
		    (yyval.expr) = alloc_binary(EXPR_LE, (yyvsp[-2].expr), (yyvsp[0].expr));
		}
#line 2431 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 75: /* expr: expr '>' expr  */
#line 578 "src/parser.y"
                {
		    (yyval.expr) = alloc_binary(EXPR_GT, (yyvsp[-2].expr), (yyvsp[0].expr));
		}
#line 2439 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 76: /* expr: expr tGE expr  */
#line 582 "src/parser.y"
                {
		    (yyval.expr) = alloc_binary(EXPR_GE, (yyvsp[-2].expr), (yyvsp[0].expr));
		}
#line 2447 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 77: /* expr: expr tIN expr  */
#line 586 "src/parser.y"
                {
		    (yyval.expr) = alloc_binary(EXPR_IN, (yyvsp[-2].expr), (yyvsp[0].expr));
		}
#line 2455 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 78: /* expr: expr tBITSHL expr  */
#line 590 "src/parser.y"
                {
		    (yyval.expr) = alloc_binary(EXPR_BITSHL, (yyvsp[-2].expr), (yyvsp[0].expr));
		}
#line 2463 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 79: /* expr: expr tBITSHR expr  */
#line 594 "src/parser.y"
                {
		    (yyval.expr) = alloc_binary(EXPR_BITSHR, (yyvsp[-2].expr), (yyvsp[0].expr));
		}
#line 2471 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 80: /* expr: '-' expr  */
#line 598 "src/parser.y"
                {
		    if ((yyvsp[0].expr)->kind == EXPR_VAR
			&& ((yyvsp[0].expr)->e.var.type == TYPE_INT
			    || (yyvsp[0].expr)->e.var.type == TYPE_FLOAT)) {
			switch ((yyvsp[0].expr)->e.var.type) {
			  case TYPE_INT:
			    (yyvsp[0].expr)->e.var.v.num = -(yyvsp[0].expr)->e.var.v.num;
			    break;
			  case TYPE_FLOAT:
			    (yyvsp[0].expr)->e.var.v.fnum = -(yyvsp[0].expr)->e.var.v.fnum;
			    break;
			  default:
			    break;
			}
		        (yyval.expr) = (yyvsp[0].expr);
		    } else {
			(yyval.expr) = alloc_expr(EXPR_NEGATE);
			(yyval.expr)->e.expr = (yyvsp[0].expr);
		    }
		}
#line 2496 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 81: /* expr: '!' expr  */
#line 619 "src/parser.y"
                {
		    (yyval.expr) = alloc_expr(EXPR_NOT);
		    (yyval.expr)->e.expr = (yyvsp[0].expr);
		}
#line 2505 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 82: /* expr: '~' expr  */
#line 624 "src/parser.y"
                {
		    (yyval.expr) = alloc_expr(EXPR_COMPLEMENT);
		    (yyval.expr)->e.expr = (yyvsp[0].expr);
		}
#line 2514 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 83: /* expr: '(' expr ')'  */
#line 629 "src/parser.y"
                { (yyval.expr) = (yyvsp[-1].expr); }
#line 2520 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 84: /* expr: '{' arglist '}'  */
#line 631 "src/parser.y"
                {
		    (yyval.expr) = alloc_expr(EXPR_LIST);
		    (yyval.expr)->e.list = (yyvsp[-1].args);
		}
#line 2529 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 85: /* expr: '[' maplist ']'  */
#line 636 "src/parser.y"
                {
		    (yyval.expr) = alloc_expr(EXPR_MAP);
		    (yyval.expr)->e.map = (yyvsp[-1].map);
		}
#line 2538 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 86: /* expr: '[' ']'  */
#line 641 "src/parser.y"
                {
		    /* [] is the expression for an empty map */
		    (yyval.expr) = alloc_expr(EXPR_MAP);
		    (yyval.expr)->e.map = 0;
		}
#line 2548 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 87: /* expr: expr '?' expr '|' expr  */
#line 647 "src/parser.y"
                {
		    (yyval.expr) = alloc_expr(EXPR_COND);
		    (yyval.expr)->e.cond.condition = (yyvsp[-4].expr);
		    (yyval.expr)->e.cond.consequent = (yyvsp[-2].expr);
		    (yyval.expr)->e.cond.alternate = (yyvsp[0].expr);
		}
#line 2559 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 88: /* expr: '`' expr '!' codes default '\''  */
#line 654 "src/parser.y"
                {
		    (yyval.expr) = alloc_expr(EXPR_CATCH);
		    (yyval.expr)->e._catch._try = (yyvsp[-4].expr);
		    (yyval.expr)->e._catch.codes = (yyvsp[-2].args);
		    (yyval.expr)->e._catch.except = (yyvsp[-1].expr);
		}
#line 2570 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 89: /* dollars_up: %empty  */
#line 664 "src/parser.y"
                { dollars_ok++; }
#line 2576 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 90: /* codes: tANY  */
#line 669 "src/parser.y"
                { (yyval.args) = 0; }
#line 2582 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 91: /* codes: ne_arglist  */
#line 671 "src/parser.y"
                { (yyval.args) = (yyvsp[0].args); }
#line 2588 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 92: /* default: %empty  */
#line 676 "src/parser.y"
                { (yyval.expr) = 0; }
#line 2594 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 93: /* default: tARROW expr  */
#line 678 "src/parser.y"
                { (yyval.expr) = (yyvsp[0].expr); }
#line 2600 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 94: /* maplist: expr tMAP expr  */
#line 683 "src/parser.y"
                { (yyval.map) = alloc_map_list((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 2606 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 95: /* maplist: maplist ',' expr tMAP expr  */
#line 685 "src/parser.y"
                {
		    Map_List *this_map = alloc_map_list((yyvsp[-2].expr), (yyvsp[0].expr));

		    if ((yyvsp[-4].map)) {
			Map_List *tmp = (yyvsp[-4].map);

			while (tmp->next)
			    tmp = tmp->next;
			tmp->next = this_map;
			(yyval.map) = (yyvsp[-4].map);
		    } else
			(yyval.map) = this_map;
		}
#line 2624 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 96: /* arglist: %empty  */
#line 702 "src/parser.y"
                { (yyval.args) = 0; }
#line 2630 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 97: /* arglist: ne_arglist  */
#line 704 "src/parser.y"
                { (yyval.args) = (yyvsp[0].args); }
#line 2636 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 98: /* ne_arglist: expr  */
#line 709 "src/parser.y"
                { (yyval.args) = alloc_arg_list(ARG_NORMAL, (yyvsp[0].expr)); }
#line 2642 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 99: /* ne_arglist: '@' expr  */
#line 711 "src/parser.y"
                { (yyval.args) = alloc_arg_list(ARG_SPLICE, (yyvsp[0].expr)); }
#line 2648 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 100: /* ne_arglist: ne_arglist ',' expr  */
#line 713 "src/parser.y"
                {
		    Arg_List *this_arg = alloc_arg_list(ARG_NORMAL, (yyvsp[0].expr));

		    if ((yyvsp[-2].args)) {
			Arg_List *tmp = (yyvsp[-2].args);

			while (tmp->next)
			    tmp = tmp->next;
			tmp->next = this_arg;
			(yyval.args) = (yyvsp[-2].args);
		    } else
			(yyval.args) = this_arg;
		}
#line 2666 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 101: /* ne_arglist: ne_arglist ',' '@' expr  */
#line 727 "src/parser.y"
                {
		    Arg_List *this_arg = alloc_arg_list(ARG_SPLICE, (yyvsp[0].expr));

		    if ((yyvsp[-3].args)) {
			Arg_List *tmp = (yyvsp[-3].args);

			while (tmp->next)
			    tmp = tmp->next;
			tmp->next = this_arg;
			(yyval.args) = (yyvsp[-3].args);
		    } else
			(yyval.args) = this_arg;
		}
#line 2684 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 102: /* scatter: ne_arglist ',' scatter_item  */
#line 744 "src/parser.y"
                {
		    Scatter    *sc = scatter_from_arglist((yyvsp[-2].args));

		    if (sc)
			(yyval.scatter) = add_scatter_item(sc, (yyvsp[0].scatter));
		    else
			(yyval.scatter) = (yyvsp[0].scatter);
		}
#line 2697 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 103: /* scatter: scatter ',' scatter_item  */
#line 753 "src/parser.y"
                {
		    (yyval.scatter) = add_scatter_item((yyvsp[-2].scatter), (yyvsp[0].scatter));
		}
#line 2705 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 104: /* scatter: scatter ',' tID  */
#line 757 "src/parser.y"
                {
		    (yyval.scatter) = add_scatter_item((yyvsp[-2].scatter), alloc_scatter(SCAT_REQUIRED,
							    find_id((yyvsp[0].string)), 0));
		}
#line 2714 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 105: /* scatter: scatter ',' '@' tID  */
#line 762 "src/parser.y"
                {
		    (yyval.scatter) = add_scatter_item((yyvsp[-3].scatter), alloc_scatter(SCAT_REST,
							    find_id((yyvsp[0].string)), 0));
		}
#line 2723 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 106: /* scatter: scatter_item  */
#line 767 "src/parser.y"
                {   (yyval.scatter) = (yyvsp[0].scatter);  }
#line 2729 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 107: /* scatter_item: '?' tID  */
#line 772 "src/parser.y"
                {
		    (yyval.scatter) = alloc_scatter(SCAT_OPTIONAL, find_id((yyvsp[0].string)), 0);
		}
#line 2737 "/home/seven/toaststunt/build-asan/parser.cc"
    break;

  case 108: /* scatter_item: '?' tID '=' expr  */
#line 776 "src/parser.y"
                {
		    (yyval.scatter) = alloc_scatter(SCAT_OPTIONAL, find_id((yyvsp[-2].string)), (yyvsp[0].expr));
		}
#line 2745 "/home/seven/toaststunt/build-asan/parser.cc"
    break;


#line 2749 "/home/seven/toaststunt/build-asan/parser.cc"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
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

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 781 "src/parser.y"


static int              lineno, nerrors, must_rename_keywords;
static Parser_Client    client;
static void            *client_data;
static Names           *local_names;

static int
find_id(char *name)
{
    int slot = find_or_add_name(&local_names, name);

    dealloc_string(name);
    return slot;
}

static void
yyerror(const char *s)
{
    error(s, 0);
}

static const char *
fmt_error(const char *s, const char *t)
{
    static Stream      *str = 0;

    if (str == 0)
	str = new_stream(100);
    if (t)
	stream_printf(str, "Line %d:  %s%s", lineno, s, t);
    else
	stream_printf(str, "Line %d:  %s", lineno, s);
    return reset_stream(str);
}

static void
error(const char *s, const char *t)
{
    nerrors++;
    (*(client.error))(client_data, fmt_error(s, t));
}

static void
warning(const char *s, const char *t)
{
    if (client.warning)
	(*(client.warning))(client_data, fmt_error(s, t));
    else
	error(s, t);
}

static int unget_buffer[5], unget_count;

static int
lex_getc(void)
{
    if (unget_count > 0)
	return unget_buffer[--unget_count];
    else
	return (*(client.getch))(client_data);
}

static void
lex_ungetc(int c)
{
    unget_buffer[unget_count++] = c;
}

static int
follow(int expect, int ifyes, int ifno)     /* look ahead for >=, etc. */
{
    int c = lex_getc();

    if (c == expect)
	return ifyes;
    lex_ungetc(c);
    return ifno;
}

static int
check_two_dots(void)     /* look ahead for .. but don't consume */
{
    int c1 = lex_getc();
    int c2 = lex_getc();

    lex_ungetc(c2);
    lex_ungetc(c1);

    return c1 == '.' && c2 == '.';
}

static Stream  *token_stream = 0;

static int
yylex(void)
{
    int c;

    reset_stream(token_stream);

start_over:

    do {
	c = lex_getc();
	if (c == '\n') lineno++;
    } while (isspace(c));

    if (c == '/') {
	c = lex_getc();
	if (c == '*') {
	    for (;;) {
		c = lex_getc();
		if (c == '*') {
		    c = lex_getc();
		    if (c == '/')
			goto start_over;
		}
		if (c == EOF) {
		    yyerror("End of program while in a comment");
		    return c;
		}
	    }
	} else {
	    lex_ungetc(c);
	    return '/';
	}
    }

    if (c == '#') {
	int            negative = 0;
	Objid          oid = 0;

	c = lex_getc();
	if (c == '-') {
	    negative = 1;
	    c = lex_getc();
	}
	if (!isdigit(c)) {
	    yyerror("Malformed object number");
	    lex_ungetc(c);
	    return 0;
	}
	do {
	    oid = oid * 10 + (c - '0');
	    c = lex_getc();
	} while (isdigit(c));
	lex_ungetc(c);

	yylval.object = negative ? -oid : oid;
	return tOBJECT;
    }

    if (isdigit(c) || (c == '.'  &&  language_version >= DBV_Float)) {
	Num	n = 0;
	int	type = tINTEGER;

	while (isdigit(c)) {
	    n = n * 10 + (c - '0');
	    stream_add_char(token_stream, c);
	    c = lex_getc();
	}

	if (language_version >= DBV_Float && c == '.') {
	    /* maybe floating-point (but maybe `..') */
	    int cc;

	    lex_ungetc(cc = lex_getc()); /* peek ahead */
	    if (isdigit(cc)) {  /* definitely floating-point */
		type = tFLOAT;
		do {
		    stream_add_char(token_stream, c);
		    c = lex_getc();
		} while (isdigit(c));
	    } else if (stream_length(token_stream) == 0) {
		/* no digits before or after `.'; not a number at all */
		goto normal_dot;
	    } else if (cc != '.') {
		/* some digits before dot, not `..' */
		type = tFLOAT;
		stream_add_char(token_stream, c);
		c = lex_getc();
	    }
	}

	if (language_version >= DBV_Float && (c == 'e' || c == 'E')) {
	    /* better be an exponent */
	    type = tFLOAT;
	    stream_add_char(token_stream, c);
	    c = lex_getc();
	    if (c == '+' || c == '-') {
		stream_add_char(token_stream, c);
		c = lex_getc();
	    }
	    if (!isdigit(c)) {
		yyerror("Malformed floating-point literal");
		lex_ungetc(c);
		return 0;
	    }
	    do {
		stream_add_char(token_stream, c);
		c = lex_getc();
	    } while (isdigit(c));
	}
	
	lex_ungetc(c);

	if (type == tINTEGER)
	    yylval.integer = n;
	else {
	    double	d;
	    
	    d = strtod(reset_stream(token_stream), 0);
	    if (!IS_REAL(d)) {
		yyerror("Floating-point literal out of range");
		d = 0.0;
	    }
	    yylval.real = d; 
	}
	return type;
    }
    
    if (isalpha(c) || c == '_') {
	char	       *buf;
	Keyword	       *k;

	stream_add_char(token_stream, c);
	while (isalnum(c = lex_getc()) || c == '_')
	    stream_add_char(token_stream, c);
	lex_ungetc(c);
	buf = reset_stream(token_stream);

	k = find_keyword(buf);
	if (k) {
	    if (k->version <= language_version) {
		int	t = k->token;

		if (t == tERROR)
		    yylval.error = k->error;
		return t;
	    } else {  /* New keyword being used as an identifier */
		if (!must_rename_keywords)
		    warning("Renaming old use of new keyword: ", buf);
		must_rename_keywords = 1;
	    }
	}
	
	yylval.string = alloc_string(buf);
	return tID;
    }

    if (c == '"') {
	while(1) {
	    c = lex_getc();
	    if (c == '"')
		break;
	    if (c == '\\')
		c = lex_getc();
	    if (c == '\n' || c == EOF) {
		yyerror("Missing quote");
		break;
	    }
	    stream_add_char(token_stream, c);
	}
	yylval.string = alloc_string(reset_stream(token_stream));
	return tSTRING;
    }

    switch(c) {
      case '^':         return check_two_dots() ? '^'
			     : follow('.', tBITXOR, '^');
      case '>':         return follow('>', 1, 0) ? tBITSHR
			     : follow('=', tGE, '>');
      case '<':         return follow('<', 1, 0) ? tBITSHL
			     : follow('=', tLE, '<');
      case '=':         return follow('=', 1, 0) ? tEQ
			     : follow('>', tARROW, '=');
      case '|':         return follow('.', 1, 0) ? tBITOR
			     : follow('|', tOR, '|');
      case '&':         return follow('.', 1, 0) ? tBITAND
			     : follow('&', tAND, '&');
      case '-':         return follow('>', tMAP, '-');
      case '!':         return follow('=', tNE, '!');
      normal_dot:
      case '.':         return follow('.', tTO, '.');
      default:          return c;
    }
}

static Scatter *
add_scatter_item(Scatter *first, Scatter *last)
{
    Scatter    *tmp = first;

    while (tmp->next)
	tmp = tmp->next;
    tmp->next = last;

    return first;
}

static Scatter *
scatter_from_arglist(Arg_List *a)
{
    Scatter    *sc = 0, **scp;
    Arg_List   *anext;

    for (scp = &sc; a; a = anext, scp = &((*scp)->next)) {
	if (a->expr->kind == EXPR_ID) {
	    *scp = alloc_scatter(a->kind == ARG_NORMAL ? SCAT_REQUIRED
						       : SCAT_REST,
				 a->expr->e.id, 0);
	    anext = a->next;
	    dealloc_node(a->expr);
	    dealloc_node(a);
	} else {
	    yyerror("Scattering assignment targets must be simple variables.");
	    return 0;
	}
    }

    return sc;
}

static void
vet_scatter(Scatter *sc)
{
    int seen_rest = 0, count = 0;

    for (; sc; sc = sc->next) {
	if (sc->kind == SCAT_REST) {
	    if (seen_rest)
		yyerror("More than one `@' target in scattering assignment.");
	    else
		seen_rest = 1;
	}
	count++;
    }

    if (count > 255)
	yyerror("Too many targets in scattering assignment.");
}

struct loop_entry {
    struct loop_entry  *next;
    const char         *name;
    int                 is_barrier;
};

static struct loop_entry *loop_stack;

static void
push_loop_name(const char *name)
{
    struct loop_entry *entry = (struct loop_entry *)mymalloc(sizeof(struct loop_entry), M_AST);

    entry->next = loop_stack;
    entry->name = (name ? str_dup(name) : 0);
    entry->is_barrier = 0;
    loop_stack = entry;
}

static void
pop_loop_name(void)
{
    if (!loop_stack)
	errlog("PARSER: Empty loop stack in POP_LOOP_NAME!\n");
    else if (loop_stack->is_barrier)
	errlog("PARSER: Tried to pop loop-scope barrier!\n");
    else {
	struct loop_entry      *entry = loop_stack;

	loop_stack = loop_stack->next;
	if (entry->name)
	    free_str(entry->name);
	myfree(entry, M_AST);
    }
}

static void
suspend_loop_scope(void)
{
    struct loop_entry *entry = (struct loop_entry *)mymalloc(sizeof(struct loop_entry), M_AST);

    entry->next = loop_stack;
    entry->name = 0;
    entry->is_barrier = 1;
    loop_stack = entry;
}

static void
resume_loop_scope(void)
{
    if (!loop_stack)
	errlog("PARSER: Empty loop stack in RESUME_LOOP_SCOPE!\n");
    else if (!loop_stack->is_barrier)
	errlog("PARSER: Tried to resume non-loop-scope barrier!\n");
    else {
	struct loop_entry      *entry = loop_stack;

	loop_stack = loop_stack->next;
	myfree(entry, M_AST);
    }
}

static void
check_loop_name(const char *name, enum loop_exit_kind kind)
{
    struct loop_entry  *entry;

    if (!name) {
	if (!loop_stack  ||  loop_stack->is_barrier) {
	    if (kind == LOOP_BREAK)
		yyerror("No enclosing loop for `break' statement");
	    else
		yyerror("No enclosing loop for `continue' statement");
	}
	return;
    }

    for (entry = loop_stack; entry && !entry->is_barrier; entry = entry->next)
	if (entry->name  &&  strcasecmp(entry->name, name) == 0)
	    return;

    if (kind == LOOP_BREAK)
	error("Invalid loop name in `break' statement: ", name);
    else
	error("Invalid loop name in `continue' statement: ", name);
}

Program *
parse_program(DB_Version version, Parser_Client c, void *data)
{
    extern int  yyparse();
    Program    *prog;

    if (token_stream == 0)
	token_stream = new_stream(1024);
    unget_count = 0;
    nerrors = 0;
    must_rename_keywords = 0;
    lineno = 1;
    client = c;
    client_data = data;
    local_names = new_builtin_names(version);
    dollars_ok = 0; /* true when the special symbols `^' and `$' are valid */
    loop_stack = 0;
    language_version = version;

    begin_code_allocation();
    yyparse();
    end_code_allocation(nerrors > 0);
    if (loop_stack) {
	if (nerrors == 0)
	    errlog("PARSER: Non-empty loop-scope stack!\n");
	while (loop_stack) {
	    struct loop_entry *entry = loop_stack;

	    loop_stack = loop_stack->next;
	    if (entry->name)
		free_str(entry->name);
	    myfree(entry, M_AST);
	}
    }

    if (nerrors == 0) {
	if (must_rename_keywords) {
	    /* One or more new keywords were used as identifiers in this code,
	     * possibly as local variable names (but maybe only as property or
	     * verb names).  Such local variables must be renamed to avoid a
	     * conflict in the new world.  We just add underscores to the end
	     * until it stops conflicting with any other local variable.
	     */
	    unsigned i;

	    for (i = first_user_slot(version); i < local_names->size; i++) {
		const char	*name = local_names->names[i];
	    
		if (find_keyword(name)) { /* Got one... */
		    stream_add_string(token_stream, name);
		    do {
			stream_add_char(token_stream, '_');
		    } while (find_name(local_names,
				       stream_contents(token_stream)) >= 0);
		    free_str(name);
		    local_names->names[i] =
			str_dup(reset_stream(token_stream));
		}
	    }
	}

	prog = generate_code(prog_start, version);
	prog->num_var_names = local_names->size;
	prog->var_names = local_names->names;

	myfree(local_names, M_NAMES);
	free_stmt(prog_start);

	return prog;
    } else {
	free_names(local_names);
	return 0;
    }
}

struct parser_state {
    Var         code;           /* a list of strings */
    int         cur_string;     /* which string? */
    int         cur_char;       /* which character in that string? */
    Var         errors;         /* a list of strings */
};

static void
my_error(void *data, const char *msg)
{
    struct parser_state *state = (struct parser_state *) data;
    Var                 v;

    v.type = TYPE_STR;
    v.v.str = str_dup(msg);
    state->errors = listappend(state->errors, v);
}

static int
my_getc(void *data)
{
    struct parser_state *state = (struct parser_state *) data;
    Var                 code;
    char                c;

    code = state->code;
    if (task_timed_out  ||  state->cur_string > code.v.list[0].v.num)
	return EOF;
    else if (!(c = code.v.list[state->cur_string].v.str[state->cur_char])) {
	state->cur_string++;
	state->cur_char = 0;
	return '\n';
    } else {
	state->cur_char++;
	return c;
    }
}

static Parser_Client list_parser_client = { my_error, 0, my_getc };

Program *
parse_list_as_program(Var code, Var *errors)
{
    struct parser_state state;
    Program            *program;

    state.code = code;
    state.cur_string = 1;
    state.cur_char = 0;
    state.errors = new_list(0);
    program = parse_program(current_db_version, list_parser_client, &state);
    *errors = state.errors;

    return program;
}
