/* A Bison parser, made by GNU Bison 1.875a.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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
     kCLASS = 258,
     kMODULE = 259,
     kDEF = 260,
     kUNDEF = 261,
     kBEGIN = 262,
     kRESCUE = 263,
     kENSURE = 264,
     kEND = 265,
     kIF = 266,
     kUNLESS = 267,
     kTHEN = 268,
     kELSIF = 269,
     kELSE = 270,
     kCASE = 271,
     kWHEN = 272,
     kWHILE = 273,
     kUNTIL = 274,
     kFOR = 275,
     kBREAK = 276,
     kNEXT = 277,
     kREDO = 278,
     kRETRY = 279,
     kIN = 280,
     kDO = 281,
     kDO_COND = 282,
     kDO_BLOCK = 283,
     kRETURN = 284,
     kYIELD = 285,
     kSUPER = 286,
     kSELF = 287,
     kNIL = 288,
     kTRUE = 289,
     kFALSE = 290,
     kAND = 291,
     kOR = 292,
     kNOT = 293,
     kIF_MOD = 294,
     kUNLESS_MOD = 295,
     kWHILE_MOD = 296,
     kUNTIL_MOD = 297,
     kRESCUE_MOD = 298,
     kALIAS = 299,
     kDEFINED = 300,
     klBEGIN = 301,
     klEND = 302,
     k__LINE__ = 303,
     k__FILE__ = 304,
     tIDENTIFIER = 305,
     tFID = 306,
     tGVAR = 307,
     tIVAR = 308,
     tCONSTANT = 309,
     tCVAR = 310,
     tINTEGER = 311,
     tFLOAT = 312,
     tSTRING_CONTENT = 313,
     tNTH_REF = 314,
     tBACK_REF = 315,
     tREGEXP_END = 316,
     tUPLUS = 317,
     tUMINUS = 318,
     tPOW = 319,
     tCMP = 320,
     tEQ = 321,
     tEQQ = 322,
     tNEQ = 323,
     tGEQ = 324,
     tLEQ = 325,
     tANDOP = 326,
     tOROP = 327,
     tMATCH = 328,
     tNMATCH = 329,
     tDOT2 = 330,
     tDOT3 = 331,
     tAREF = 332,
     tASET = 333,
     tLSHFT = 334,
     tRSHFT = 335,
     tCOLON2 = 336,
     tCOLON3 = 337,
     tOP_ASGN = 338,
     tASSOC = 339,
     tLPAREN = 340,
     tLPAREN_ARG = 341,
     tRPAREN = 342,
     tLBRACK = 343,
     tLBRACE = 344,
     tLBRACE_ARG = 345,
     tSTAR = 346,
     tAMPER = 347,
     tSYMBEG = 348,
     tSTRING_BEG = 349,
     tXSTRING_BEG = 350,
     tREGEXP_BEG = 351,
     tWORDS_BEG = 352,
     tQWORDS_BEG = 353,
     tSTRING_DBEG = 354,
     tSTRING_DVAR = 355,
     tSTRING_END = 356,
     tLOWEST = 357,
     tUMINUS_NUM = 358,
     tLAST_TOKEN = 359
   };
#endif
#define kCLASS 258
#define kMODULE 259
#define kDEF 260
#define kUNDEF 261
#define kBEGIN 262
#define kRESCUE 263
#define kENSURE 264
#define kEND 265
#define kIF 266
#define kUNLESS 267
#define kTHEN 268
#define kELSIF 269
#define kELSE 270
#define kCASE 271
#define kWHEN 272
#define kWHILE 273
#define kUNTIL 274
#define kFOR 275
#define kBREAK 276
#define kNEXT 277
#define kREDO 278
#define kRETRY 279
#define kIN 280
#define kDO 281
#define kDO_COND 282
#define kDO_BLOCK 283
#define kRETURN 284
#define kYIELD 285
#define kSUPER 286
#define kSELF 287
#define kNIL 288
#define kTRUE 289
#define kFALSE 290
#define kAND 291
#define kOR 292
#define kNOT 293
#define kIF_MOD 294
#define kUNLESS_MOD 295
#define kWHILE_MOD 296
#define kUNTIL_MOD 297
#define kRESCUE_MOD 298
#define kALIAS 299
#define kDEFINED 300
#define klBEGIN 301
#define klEND 302
#define k__LINE__ 303
#define k__FILE__ 304
#define tIDENTIFIER 305
#define tFID 306
#define tGVAR 307
#define tIVAR 308
#define tCONSTANT 309
#define tCVAR 310
#define tINTEGER 311
#define tFLOAT 312
#define tSTRING_CONTENT 313
#define tNTH_REF 314
#define tBACK_REF 315
#define tREGEXP_END 316
#define tUPLUS 317
#define tUMINUS 318
#define tPOW 319
#define tCMP 320
#define tEQ 321
#define tEQQ 322
#define tNEQ 323
#define tGEQ 324
#define tLEQ 325
#define tANDOP 326
#define tOROP 327
#define tMATCH 328
#define tNMATCH 329
#define tDOT2 330
#define tDOT3 331
#define tAREF 332
#define tASET 333
#define tLSHFT 334
#define tRSHFT 335
#define tCOLON2 336
#define tCOLON3 337
#define tOP_ASGN 338
#define tASSOC 339
#define tLPAREN 340
#define tLPAREN_ARG 341
#define tRPAREN 342
#define tLBRACK 343
#define tLBRACE 344
#define tLBRACE_ARG 345
#define tSTAR 346
#define tAMPER 347
#define tSYMBEG 348
#define tSTRING_BEG 349
#define tXSTRING_BEG 350
#define tREGEXP_BEG 351
#define tWORDS_BEG 352
#define tQWORDS_BEG 353
#define tSTRING_DBEG 354
#define tSTRING_DVAR 355
#define tSTRING_END 356
#define tLOWEST 357
#define tUMINUS_NUM 358
#define tLAST_TOKEN 359




/* Copy the first part of user declarations.  */
#line 13 "parse.y"


#define YYDEBUG 1

#include "ruby.h"
#include "env.h"
#include "intern.h"
#include "node.h"
#include "st.h"
#include <stdio.h>
#include <errno.h>
#include <ctype.h>

#define yyparse ruby_yyparse
#define yylex ruby_yylex
#define yyerror ruby_yyerror
#define yylval ruby_yylval
#define yychar ruby_yychar
#define yydebug ruby_yydebug

#define ID_SCOPE_SHIFT 3
#define ID_SCOPE_MASK 0x07
#define ID_LOCAL    0x01
#define ID_INSTANCE 0x02
#define ID_GLOBAL   0x03
#define ID_ATTRSET  0x04
#define ID_CONST    0x05
#define ID_CLASS    0x06
#define ID_JUNK     0x07
#define ID_INTERNAL ID_JUNK

#define is_notop_id(id) ((id)>tLAST_TOKEN)
#define is_local_id(id) (is_notop_id(id)&&((id)&ID_SCOPE_MASK)==ID_LOCAL)
#define is_global_id(id) (is_notop_id(id)&&((id)&ID_SCOPE_MASK)==ID_GLOBAL)
#define is_instance_id(id) (is_notop_id(id)&&((id)&ID_SCOPE_MASK)==ID_INSTANCE)
#define is_attrset_id(id) (is_notop_id(id)&&((id)&ID_SCOPE_MASK)==ID_ATTRSET)
#define is_const_id(id) (is_notop_id(id)&&((id)&ID_SCOPE_MASK)==ID_CONST)
#define is_class_id(id) (is_notop_id(id)&&((id)&ID_SCOPE_MASK)==ID_CLASS)
#define is_junk_id(id) (is_notop_id(id)&&((id)&ID_SCOPE_MASK)==ID_JUNK)

#define is_asgn_or_id(id) ((is_notop_id(id)) && \
	(((id)&ID_SCOPE_MASK) == ID_GLOBAL || \
	 ((id)&ID_SCOPE_MASK) == ID_INSTANCE || \
	 ((id)&ID_SCOPE_MASK) == ID_CLASS))

NODE *ruby_eval_tree_begin = 0;
NODE *ruby_eval_tree = 0;

char *ruby_sourcefile;		/* current source file */
int   ruby_sourceline;		/* current line no. */

static int yylex();
static int yyerror();

static enum lex_state {
    EXPR_BEG,			/* ignore newline, +/- is a sign. */
    EXPR_END,			/* newline significant, +/- is a operator. */
    EXPR_ARG,			/* newline significant, +/- is a operator. */
    EXPR_CMDARG,		/* newline significant, +/- is a operator. */
    EXPR_ENDARG,		/* newline significant, +/- is a operator. */
    EXPR_MID,			/* newline significant, +/- is a operator. */
    EXPR_FNAME,			/* ignore newline, no reserved words. */
    EXPR_DOT,			/* right after `.' or `::', no reserved words. */
    EXPR_CLASS,			/* immediate after `class', no here document. */
} lex_state;
static NODE *lex_strterm;

#ifdef HAVE_LONG_LONG
typedef unsigned LONG_LONG stack_type;
#else
typedef unsigned long stack_type;
#endif

static stack_type cond_stack = 0;
#define COND_PUSH(n) (cond_stack = (cond_stack<<1)|((n)&1))
#define COND_POP() (cond_stack >>= 1)
#define COND_LEXPOP() do {\
    int last = COND_P();\
    cond_stack >>= 1;\
    if (last) cond_stack |= 1;\
} while (0)
#define COND_P() (cond_stack&1)

static stack_type cmdarg_stack = 0;
#define CMDARG_PUSH(n) (cmdarg_stack = (cmdarg_stack<<1)|((n)&1))
#define CMDARG_POP() (cmdarg_stack >>= 1)
#define CMDARG_LEXPOP() do {\
    int last = CMDARG_P();\
    cmdarg_stack >>= 1;\
    if (last) cmdarg_stack |= 1;\
} while (0)
#define CMDARG_P() (cmdarg_stack&1)

static int class_nest = 0;
static int in_single = 0;
static int in_def = 0;
static int compile_for_eval = 0;
static ID cur_mid = 0;
static int quoted_term;
#define quoted_term_char ((unsigned char)quoted_term)
#define WHEN_QUOTED_TERM(x) ((quoted_term >= 0) && (x))
#define QUOTED_TERM_P(c) WHEN_QUOTED_TERM((c) == quoted_term_char)

static NODE *cond();
static NODE *logop();
static int cond_negative();

static NODE *newline_node();
static void fixpos();

static int value_expr0();
static void void_expr0();
static void void_stmts();
static NODE *remove_begin();
#define value_expr(node) value_expr0((node) = remove_begin(node))
#define void_expr(node) void_expr0((node) = remove_begin(node))

static NODE *block_append();
static NODE *list_append();
static NODE *list_concat();
static NODE *arg_concat();
static NODE *arg_prepend();
static NODE *literal_concat();
static NODE *new_evstr();
static NODE *evstr2dstr();
static NODE *call_op();
static int in_defined = 0;

static NODE *negate_lit();
static NODE *ret_args();
static NODE *arg_blk_pass();
static NODE *new_call();
static NODE *new_fcall();
static NODE *new_super();
static NODE *new_yield();

static NODE *gettable();
static NODE *assignable();
static NODE *aryset();
static NODE *attrset();
static void rb_backref_error();
static NODE *node_assign();

static NODE *match_gen();
static void local_push();
static void local_pop();
static int  local_append();
static int  local_cnt();
static int  local_id();
static ID  *local_tbl();
static ID   internal_id();

static struct RVarmap *dyna_push();
static void dyna_pop();
static int dyna_in_block();
static NODE *dyna_init();

static void top_local_init();
static void top_local_setup();

#define RE_OPTION_ONCE 0x80

#define NODE_STRTERM NODE_ZARRAY	/* nothing to gc */
#define NODE_HEREDOC NODE_ARRAY 	/* 1, 3 to gc */
#define ESCAPED_TERM (1 << CHAR_BIT)
#define SIGN_EXTEND(x,n) (((1<<(n))-1-((x)&~(~0<<(n))))^~(~0<<(n)))
#define nd_func u1.id
#if SIZEOF_SHORT == 2
#define nd_term(node) ((signed short)(node)->u2.id)
#else
#define nd_term(node) SIGN_EXTEND((node)->u2.id, CHAR_BIT*2)
#endif
#define nd_paren(node) (char)((node)->u2.id >> CHAR_BIT*2)
#define nd_nest u3.id



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
#line 190 "parse.y"
typedef union YYSTYPE {
    NODE *node;
    ID id;
    int num;
    struct RVarmap *vars;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 468 "parse.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 480 "parse.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
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
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
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
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   9315

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  132
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  142
/* YYNRULES -- Number of rules. */
#define YYNRULES  493
/* YYNRULES -- Number of states. */
#define YYNSTATES  881

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   359

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     130,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,   129,   117,     2,     2,     2,   115,   110,     2,
     128,   125,   113,   111,   126,   112,   124,   114,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   105,   131,
     107,   103,   106,   104,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   122,     2,   123,   109,     2,   127,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   120,   108,   121,   118,     2,     2,     2,
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
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   116,   119
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     4,     7,    12,    15,    17,    19,    23,
      26,    27,    32,    36,    40,    44,    47,    51,    55,    59,
      63,    67,    68,    74,    79,    83,    87,    91,    98,   104,
     110,   116,   120,   124,   128,   132,   134,   136,   140,   144,
     147,   150,   152,   154,   156,   158,   161,   164,   167,   169,
     174,   179,   180,   181,   188,   191,   195,   200,   206,   211,
     217,   220,   223,   225,   229,   231,   235,   237,   240,   244,
     247,   250,   252,   254,   258,   261,   265,   267,   272,   276,
     280,   284,   288,   290,   292,   297,   301,   305,   309,   313,
     315,   317,   319,   322,   324,   328,   330,   332,   334,   336,
     338,   340,   342,   344,   345,   350,   352,   354,   356,   358,
     360,   362,   364,   366,   368,   370,   372,   374,   376,   378,
     380,   382,   384,   386,   388,   390,   392,   394,   396,   398,
     400,   402,   404,   406,   408,   410,   412,   414,   416,   418,
     420,   422,   424,   426,   428,   430,   432,   434,   436,   438,
     440,   442,   444,   446,   448,   450,   452,   454,   456,   458,
     460,   462,   464,   466,   468,   470,   472,   474,   476,   478,
     480,   482,   484,   488,   494,   498,   505,   511,   517,   523,
     529,   533,   537,   541,   545,   549,   553,   557,   561,   565,
     570,   575,   578,   581,   585,   589,   593,   597,   601,   605,
     609,   613,   617,   621,   625,   629,   633,   636,   639,   643,
     647,   651,   655,   656,   661,   667,   669,   671,   673,   676,
     679,   685,   688,   692,   696,   701,   706,   713,   715,   717,
     719,   722,   728,   731,   737,   742,   750,   754,   756,   761,
     765,   771,   779,   782,   788,   793,   800,   808,   818,   822,
     824,   825,   828,   830,   831,   835,   836,   841,   844,   847,
     849,   851,   855,   859,   864,   867,   869,   871,   873,   875,
     877,   879,   881,   883,   885,   886,   891,   892,   897,   901,
     905,   908,   913,   917,   921,   923,   928,   932,   934,   935,
     942,   945,   947,   950,   957,   964,   965,   966,   974,   975,
     976,   984,   990,   995,  1001,  1002,  1003,  1013,  1014,  1021,
    1022,  1023,  1032,  1033,  1039,  1040,  1047,  1048,  1049,  1059,
    1061,  1063,  1065,  1067,  1069,  1071,  1073,  1075,  1078,  1080,
    1082,  1084,  1086,  1092,  1094,  1097,  1099,  1101,  1103,  1106,
    1108,  1112,  1113,  1114,  1121,  1124,  1129,  1134,  1137,  1142,
    1147,  1151,  1154,  1156,  1157,  1158,  1165,  1166,  1167,  1174,
    1180,  1182,  1187,  1190,  1192,  1194,  1201,  1203,  1205,  1207,
    1210,  1212,  1215,  1217,  1219,  1221,  1223,  1225,  1227,  1230,
    1234,  1238,  1242,  1246,  1250,  1251,  1255,  1257,  1260,  1264,
    1268,  1269,  1273,  1274,  1277,  1278,  1281,  1283,  1284,  1288,
    1289,  1295,  1297,  1299,  1301,  1303,  1304,  1307,  1309,  1311,
    1313,  1315,  1319,  1321,  1323,  1326,  1329,  1331,  1333,  1335,
    1337,  1339,  1341,  1343,  1345,  1347,  1349,  1351,  1353,  1355,
    1357,  1359,  1361,  1362,  1367,  1370,  1375,  1378,  1385,  1390,
    1395,  1398,  1403,  1406,  1409,  1411,  1412,  1414,  1416,  1418,
    1420,  1422,  1424,  1428,  1432,  1434,  1438,  1441,  1443,  1446,
    1449,  1451,  1453,  1454,  1460,  1462,  1465,  1468,  1470,  1474,
    1478,  1480,  1482,  1484,  1486,  1488,  1490,  1492,  1494,  1496,
    1498,  1500,  1502,  1503,  1505,  1506,  1508,  1509,  1511,  1513,
    1515,  1517,  1519,  1522
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
     133,     0,    -1,    -1,   134,   136,    -1,   136,   218,   202,
     221,    -1,   137,   268,    -1,   273,    -1,   138,    -1,   137,
     272,   138,    -1,     1,   138,    -1,    -1,    44,   159,   139,
     159,    -1,    44,    52,    52,    -1,    44,    52,    60,    -1,
      44,    52,    59,    -1,     6,   160,    -1,   138,    39,   142,
      -1,   138,    40,   142,    -1,   138,    41,   142,    -1,   138,
      42,   142,    -1,   138,    43,   138,    -1,    -1,    46,   140,
     120,   136,   121,    -1,    47,   120,   136,   121,    -1,   155,
     103,   143,    -1,   149,   103,   143,    -1,   246,    83,   143,
      -1,   198,   122,   167,   123,    83,   143,    -1,   198,   124,
      50,    83,   143,    -1,   198,   124,    54,    83,   143,    -1,
     198,    81,    50,    83,   143,    -1,   247,    83,   143,    -1,
     155,   103,   180,    -1,   149,   103,   166,    -1,   149,   103,
     180,    -1,   141,    -1,   143,    -1,   141,    36,   141,    -1,
     141,    37,   141,    -1,    38,   141,    -1,   117,   143,    -1,
     164,    -1,   141,    -1,   148,    -1,   144,    -1,    29,   170,
      -1,    21,   170,    -1,    22,   170,    -1,   208,    -1,   208,
     124,   265,   172,    -1,   208,    81,   265,   172,    -1,    -1,
      -1,    90,   146,   204,   147,   136,   121,    -1,   264,   172,
      -1,   264,   172,   145,    -1,   198,   124,   265,   172,    -1,
     198,   124,   265,   172,   145,    -1,   198,    81,   265,   172,
      -1,   198,    81,   265,   172,   145,    -1,    31,   172,    -1,
      30,   172,    -1,   151,    -1,    85,   150,   125,    -1,   151,
      -1,    85,   150,   125,    -1,   153,    -1,   153,   152,    -1,
     153,    91,   154,    -1,   153,    91,    -1,    91,   154,    -1,
      91,    -1,   154,    -1,    85,   150,   125,    -1,   152,   126,
      -1,   153,   152,   126,    -1,   244,    -1,   198,   122,   167,
     123,    -1,   198,   124,    50,    -1,   198,    81,    50,    -1,
     198,   124,    54,    -1,   198,    81,    54,    -1,   247,    -1,
     244,    -1,   198,   122,   167,   123,    -1,   198,   124,    50,
      -1,   198,    81,    50,    -1,   198,   124,    54,    -1,   198,
      81,    54,    -1,   247,    -1,    50,    -1,    54,    -1,    82,
     156,    -1,   156,    -1,   198,    81,   156,    -1,    50,    -1,
      54,    -1,    51,    -1,   162,    -1,   163,    -1,   158,    -1,
     240,    -1,   159,    -1,    -1,   160,   126,   161,   159,    -1,
     108,    -1,   109,    -1,   110,    -1,    65,    -1,    66,    -1,
      67,    -1,    73,    -1,   106,    -1,    69,    -1,   107,    -1,
      70,    -1,    79,    -1,    80,    -1,   111,    -1,   112,    -1,
     113,    -1,    91,    -1,   114,    -1,   115,    -1,    64,    -1,
     118,    -1,    62,    -1,    63,    -1,    77,    -1,    78,    -1,
     127,    -1,    48,    -1,    49,    -1,    46,    -1,    47,    -1,
      44,    -1,    36,    -1,     7,    -1,    21,    -1,    16,    -1,
       3,    -1,     5,    -1,    45,    -1,    26,    -1,    15,    -1,
      14,    -1,    10,    -1,     9,    -1,    35,    -1,    20,    -1,
      25,    -1,     4,    -1,    22,    -1,    33,    -1,    38,    -1,
      37,    -1,    23,    -1,     8,    -1,    24,    -1,    29,    -1,
      32,    -1,    31,    -1,    13,    -1,    34,    -1,     6,    -1,
      17,    -1,    30,    -1,    39,    -1,    40,    -1,    41,    -1,
      42,    -1,    43,    -1,   155,   103,   164,    -1,   155,   103,
     164,    43,   164,    -1,   246,    83,   164,    -1,   198,   122,
     167,   123,    83,   164,    -1,   198,   124,    50,    83,   164,
      -1,   198,   124,    54,    83,   164,    -1,   198,    81,    50,
      83,   164,    -1,   198,    81,    54,    83,   164,    -1,   247,
      83,   164,    -1,   164,    75,   164,    -1,   164,    76,   164,
      -1,   164,   111,   164,    -1,   164,   112,   164,    -1,   164,
     113,   164,    -1,   164,   114,   164,    -1,   164,   115,   164,
      -1,   164,    64,   164,    -1,   116,    56,    64,   164,    -1,
     116,    57,    64,   164,    -1,    62,   164,    -1,    63,   164,
      -1,   164,   108,   164,    -1,   164,   109,   164,    -1,   164,
     110,   164,    -1,   164,    65,   164,    -1,   164,   106,   164,
      -1,   164,    69,   164,    -1,   164,   107,   164,    -1,   164,
      70,   164,    -1,   164,    66,   164,    -1,   164,    67,   164,
      -1,   164,    68,   164,    -1,   164,    73,   164,    -1,   164,
      74,   164,    -1,   117,   164,    -1,   118,   164,    -1,   164,
      79,   164,    -1,   164,    80,   164,    -1,   164,    71,   164,
      -1,   164,    72,   164,    -1,    -1,    45,   269,   165,   164,
      -1,   164,   104,   164,   105,   164,    -1,   181,    -1,   164,
      -1,   273,    -1,   148,   269,    -1,   179,   270,    -1,   179,
     126,    91,   164,   269,    -1,   262,   270,    -1,    91,   164,
     269,    -1,   128,   273,   125,    -1,   128,   170,   269,   125,
      -1,   128,   208,   269,   125,    -1,   128,   179,   126,   208,
     269,   125,    -1,   273,    -1,   168,    -1,   148,    -1,   179,
     178,    -1,   179,   126,    91,   166,   178,    -1,   262,   178,
      -1,   262,   126,    91,   166,   178,    -1,   179,   126,   262,
     178,    -1,   179,   126,   262,   126,    91,   164,   178,    -1,
      91,   166,   178,    -1,   177,    -1,   166,   126,   179,   178,
      -1,   166,   126,   177,    -1,   166,   126,    91,   166,   178,
      -1,   166,   126,   179,   126,    91,   166,   178,    -1,   262,
     178,    -1,   262,   126,    91,   166,   178,    -1,   166,   126,
     262,   178,    -1,   166,   126,   179,   126,   262,   178,    -1,
     166,   126,   262,   126,    91,   166,   178,    -1,   166,   126,
     179,   126,   262,   126,    91,   166,   178,    -1,    91,   166,
     178,    -1,   177,    -1,    -1,   173,   174,    -1,   170,    -1,
      -1,    86,   175,   125,    -1,    -1,    86,   171,   176,   125,
      -1,    92,   166,    -1,   126,   177,    -1,   273,    -1,   166,
      -1,   179,   126,   166,    -1,   179,   126,   166,    -1,   179,
     126,    91,   166,    -1,    91,   166,    -1,   222,    -1,   223,
      -1,   226,    -1,   227,    -1,   228,    -1,   231,    -1,   245,
      -1,   247,    -1,    51,    -1,    -1,     7,   182,   135,    10,
      -1,    -1,    86,   141,   183,   125,    -1,    85,   136,   125,
      -1,   198,    81,    54,    -1,    82,   156,    -1,   198,   122,
     167,   123,    -1,    88,   167,   123,    -1,    89,   261,   121,
      -1,    29,    -1,    30,   128,   170,   125,    -1,    30,   128,
     125,    -1,    30,    -1,    -1,    45,   269,   128,   184,   141,
     125,    -1,   264,   210,    -1,   209,    -1,   209,   210,    -1,
      11,   142,   199,   136,   201,    10,    -1,    12,   142,   199,
     136,   202,    10,    -1,    -1,    -1,    18,   185,   142,   200,
     186,   136,    10,    -1,    -1,    -1,    19,   187,   142,   200,
     188,   136,    10,    -1,    16,   142,   268,   215,    10,    -1,
      16,   268,   215,    10,    -1,    16,   268,    15,   136,    10,
      -1,    -1,    -1,    20,   203,    25,   189,   142,   200,   190,
     136,    10,    -1,    -1,     3,   157,   248,   191,   135,    10,
      -1,    -1,    -1,     3,    79,   141,   192,   271,   193,   135,
      10,    -1,    -1,     4,   157,   194,   135,    10,    -1,    -1,
       5,   158,   195,   250,   135,    10,    -1,    -1,    -1,     5,
     259,   267,   196,   158,   197,   250,   135,    10,    -1,    21,
      -1,    22,    -1,    23,    -1,    24,    -1,   181,    -1,   271,
      -1,   105,    -1,    13,    -1,   271,    13,    -1,   271,    -1,
     105,    -1,    27,    -1,   202,    -1,    14,   142,   199,   136,
     201,    -1,   273,    -1,    15,   136,    -1,   155,    -1,   149,
      -1,   273,    -1,   108,   108,    -1,    72,    -1,   108,   203,
     108,    -1,    -1,    -1,    28,   206,   204,   207,   136,    10,
      -1,   148,   205,    -1,   208,   124,   265,   169,    -1,   208,
      81,   265,   169,    -1,   264,   168,    -1,   198,   124,   265,
     169,    -1,   198,    81,   265,   168,    -1,   198,    81,   266,
      -1,    31,   168,    -1,    31,    -1,    -1,    -1,   120,   211,
     204,   212,   136,   121,    -1,    -1,    -1,    26,   213,   204,
     214,   136,    10,    -1,    17,   216,   199,   136,   217,    -1,
     179,    -1,   179,   126,    91,   166,    -1,    91,   166,    -1,
     202,    -1,   215,    -1,     8,   219,   220,   199,   136,   218,
      -1,   273,    -1,   179,    -1,   273,    -1,    84,   155,    -1,
     273,    -1,     9,   136,    -1,   273,    -1,   243,    -1,   240,
      -1,   242,    -1,   224,    -1,   225,    -1,   224,   225,    -1,
      94,   233,   101,    -1,    95,   234,   101,    -1,    96,   234,
      61,    -1,    97,   129,   101,    -1,    97,   229,   101,    -1,
      -1,   229,   230,   129,    -1,   235,    -1,   230,   235,    -1,
      98,   129,   101,    -1,    98,   232,   101,    -1,    -1,   232,
      58,   129,    -1,    -1,   233,   235,    -1,    -1,   234,   235,
      -1,    58,    -1,    -1,   100,   236,   238,    -1,    -1,    99,
     239,   237,   136,   121,    -1,    52,    -1,    53,    -1,    55,
      -1,   247,    -1,    -1,    93,   241,    -1,   158,    -1,    53,
      -1,    52,    -1,    55,    -1,    93,   234,   101,    -1,    56,
      -1,    57,    -1,   116,    56,    -1,   116,    57,    -1,    50,
      -1,    53,    -1,    52,    -1,    54,    -1,    55,    -1,    33,
      -1,    32,    -1,    34,    -1,    35,    -1,    49,    -1,    48,
      -1,   244,    -1,   244,    -1,    59,    -1,    60,    -1,   271,
      -1,    -1,   107,   249,   142,   271,    -1,     1,   271,    -1,
     128,   251,   269,   125,    -1,   251,   271,    -1,   253,   126,
     255,   126,   256,   258,    -1,   253,   126,   255,   258,    -1,
     253,   126,   256,   258,    -1,   253,   258,    -1,   255,   126,
     256,   258,    -1,   255,   258,    -1,   256,   258,    -1,   257,
      -1,    -1,    54,    -1,    53,    -1,    52,    -1,    55,    -1,
      50,    -1,   252,    -1,   253,   126,   252,    -1,    50,   103,
     166,    -1,   254,    -1,   255,   126,   254,    -1,    91,    50,
      -1,    91,    -1,    92,    50,    -1,   126,   257,    -1,   273,
      -1,   245,    -1,    -1,   128,   260,   141,   269,   125,    -1,
     273,    -1,   262,   270,    -1,   179,   270,    -1,   263,    -1,
     262,   126,   263,    -1,   166,    84,   166,    -1,    50,    -1,
      54,    -1,    51,    -1,    50,    -1,    54,    -1,    51,    -1,
     162,    -1,    50,    -1,    51,    -1,   162,    -1,   124,    -1,
      81,    -1,    -1,   272,    -1,    -1,   130,    -1,    -1,   130,
      -1,   126,    -1,   131,    -1,   130,    -1,   271,    -1,   272,
     131,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   327,   327,   327,   352,   372,   379,   380,   384,   388,
     394,   394,   398,   402,   409,   414,   418,   427,   436,   448,
     460,   465,   464,   478,   486,   491,   497,   522,   539,   551,
     563,   575,   580,   584,   589,   594,   597,   598,   602,   606,
     610,   614,   617,   624,   625,   626,   630,   634,   640,   641,
     645,   652,   656,   651,   666,   671,   683,   688,   700,   705,
     717,   722,   729,   730,   736,   737,   743,   747,   751,   755,
     759,   763,   769,   770,   776,   780,   786,   790,   794,   798,
     802,   806,   812,   819,   823,   827,   831,   835,   839,   845,
     852,   856,   859,   863,   867,   873,   874,   875,   876,   881,
     888,   889,   892,   896,   896,   902,   903,   904,   905,   906,
     907,   908,   909,   910,   911,   912,   913,   914,   915,   916,
     917,   918,   919,   920,   921,   922,   923,   924,   925,   926,
     927,   930,   930,   930,   930,   931,   931,   931,   931,   931,
     931,   931,   932,   932,   932,   932,   932,   932,   932,   933,
     933,   933,   933,   933,   933,   934,   934,   934,   934,   934,
     934,   934,   935,   935,   935,   935,   935,   936,   936,   936,
     936,   936,   939,   943,   947,   972,   989,  1001,  1013,  1025,
    1029,  1034,  1040,  1046,  1050,  1054,  1058,  1062,  1066,  1070,
    1074,  1078,  1087,  1091,  1095,  1099,  1103,  1107,  1111,  1115,
    1119,  1123,  1127,  1131,  1135,  1139,  1143,  1147,  1151,  1155,
    1159,  1163,  1167,  1167,  1172,  1177,  1183,  1190,  1191,  1196,
    1200,  1205,  1209,  1216,  1220,  1224,  1229,  1236,  1237,  1240,
    1245,  1249,  1254,  1259,  1264,  1269,  1275,  1279,  1282,  1286,
    1290,  1295,  1300,  1305,  1310,  1315,  1320,  1325,  1330,  1334,
    1337,  1337,  1349,  1350,  1350,  1355,  1355,  1362,  1368,  1372,
    1375,  1379,  1385,  1389,  1393,  1399,  1400,  1401,  1402,  1403,
    1404,  1405,  1406,  1407,  1412,  1411,  1421,  1421,  1426,  1430,
    1434,  1438,  1445,  1454,  1458,  1462,  1466,  1470,  1474,  1474,
    1479,  1484,  1485,  1494,  1507,  1520,  1520,  1520,  1530,  1530,
    1530,  1540,  1547,  1551,  1555,  1555,  1555,  1563,  1562,  1579,
    1584,  1578,  1601,  1600,  1617,  1616,  1633,  1634,  1633,  1648,
    1652,  1656,  1660,  1666,  1673,  1674,  1675,  1676,  1679,  1680,
    1681,  1684,  1685,  1694,  1695,  1701,  1702,  1705,  1706,  1710,
    1714,  1721,  1725,  1720,  1735,  1744,  1748,  1754,  1759,  1764,
    1769,  1773,  1777,  1784,  1788,  1783,  1796,  1800,  1795,  1809,
    1817,  1818,  1822,  1828,  1829,  1832,  1843,  1846,  1847,  1850,
    1854,  1857,  1865,  1868,  1869,  1873,  1876,  1889,  1890,  1896,
    1902,  1925,  1958,  1962,  1969,  1972,  1978,  1979,  1985,  1989,
    1996,  1999,  2006,  2009,  2016,  2019,  2025,  2027,  2026,  2038,
    2037,  2055,  2056,  2057,  2058,  2062,  2071,  2078,  2079,  2080,
    2081,  2084,  2110,  2111,  2112,  2116,  2122,  2123,  2124,  2125,
    2126,  2127,  2128,  2129,  2130,  2131,  2132,  2135,  2141,  2147,
    2148,  2151,  2156,  2155,  2163,  2166,  2171,  2177,  2181,  2185,
    2189,  2193,  2197,  2201,  2205,  2210,  2215,  2219,  2223,  2227,
    2231,  2242,  2243,  2249,  2259,  2264,  2270,  2278,  2284,  2294,
    2298,  2301,  2311,  2311,  2336,  2337,  2341,  2350,  2351,  2357,
    2363,  2364,  2365,  2368,  2369,  2370,  2371,  2374,  2375,  2376,
    2379,  2380,  2383,  2384,  2387,  2388,  2391,  2392,  2393,  2396,
    2397,  2400,  2401,  2404
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "kCLASS", "kMODULE", "kDEF", "kUNDEF", 
  "kBEGIN", "kRESCUE", "kENSURE", "kEND", "kIF", "kUNLESS", "kTHEN", 
  "kELSIF", "kELSE", "kCASE", "kWHEN", "kWHILE", "kUNTIL", "kFOR", 
  "kBREAK", "kNEXT", "kREDO", "kRETRY", "kIN", "kDO", "kDO_COND", 
  "kDO_BLOCK", "kRETURN", "kYIELD", "kSUPER", "kSELF", "kNIL", "kTRUE", 
  "kFALSE", "kAND", "kOR", "kNOT", "kIF_MOD", "kUNLESS_MOD", "kWHILE_MOD", 
  "kUNTIL_MOD", "kRESCUE_MOD", "kALIAS", "kDEFINED", "klBEGIN", "klEND", 
  "k__LINE__", "k__FILE__", "tIDENTIFIER", "tFID", "tGVAR", "tIVAR", 
  "tCONSTANT", "tCVAR", "tINTEGER", "tFLOAT", "tSTRING_CONTENT", 
  "tNTH_REF", "tBACK_REF", "tREGEXP_END", "tUPLUS", "tUMINUS", "tPOW", 
  "tCMP", "tEQ", "tEQQ", "tNEQ", "tGEQ", "tLEQ", "tANDOP", "tOROP", 
  "tMATCH", "tNMATCH", "tDOT2", "tDOT3", "tAREF", "tASET", "tLSHFT", 
  "tRSHFT", "tCOLON2", "tCOLON3", "tOP_ASGN", "tASSOC", "tLPAREN", 
  "tLPAREN_ARG", "tRPAREN", "tLBRACK", "tLBRACE", "tLBRACE_ARG", "tSTAR", 
  "tAMPER", "tSYMBEG", "tSTRING_BEG", "tXSTRING_BEG", "tREGEXP_BEG", 
  "tWORDS_BEG", "tQWORDS_BEG", "tSTRING_DBEG", "tSTRING_DVAR", 
  "tSTRING_END", "tLOWEST", "'='", "'?'", "':'", "'>'", "'<'", "'|'", 
  "'^'", "'&'", "'+'", "'-'", "'*'", "'/'", "'%'", "tUMINUS_NUM", "'!'", 
  "'~'", "tLAST_TOKEN", "'{'", "'}'", "'['", "']'", "'.'", "')'", "','", 
  "'`'", "'('", "' '", "'\\n'", "';'", "$accept", "program", "@1", 
  "bodystmt", "compstmt", "stmts", "stmt", "@2", "@3", "expr", 
  "expr_value", "command_call", "block_command", "cmd_brace_block", "@4", 
  "@5", "command", "mlhs", "mlhs_entry", "mlhs_basic", "mlhs_item", 
  "mlhs_head", "mlhs_node", "lhs", "cname", "cpath", "fname", "fitem", 
  "undef_list", "@6", "op", "reswords", "arg", "@7", "arg_value", 
  "aref_args", "paren_args", "opt_paren_args", "call_args", "call_args2", 
  "command_args", "@8", "open_args", "@9", "@10", "block_arg", 
  "opt_block_arg", "args", "mrhs", "primary", "@11", "@12", "@13", "@14", 
  "@15", "@16", "@17", "@18", "@19", "@20", "@21", "@22", "@23", "@24", 
  "@25", "@26", "primary_value", "then", "do", "if_tail", "opt_else", 
  "block_var", "opt_block_var", "do_block", "@27", "@28", "block_call", 
  "method_call", "brace_block", "@29", "@30", "@31", "@32", "case_body", 
  "when_args", "cases", "opt_rescue", "exc_list", "exc_var", "opt_ensure", 
  "literal", "strings", "string", "string1", "xstring", "regexp", "words", 
  "word_list", "word", "qwords", "qword_list", "string_contents", 
  "xstring_contents", "string_content", "@33", "@34", "string_dvar", 
  "term_push", "symbol", "sym", "dsym", "numeric", "variable", "var_ref", 
  "var_lhs", "backref", "superclass", "@35", "f_arglist", "f_args", 
  "f_norm_arg", "f_arg", "f_opt", "f_optarg", "f_rest_arg", "f_block_arg", 
  "opt_f_block_arg", "singleton", "@36", "assoc_list", "assocs", "assoc", 
  "operation", "operation2", "operation3", "dot_or_colon", "opt_terms", 
  "opt_nl", "trailer", "term", "terms", "none", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,    61,    63,    58,    62,    60,   124,    94,
      38,    43,    45,    42,    47,    37,   358,    33,   126,   359,
     123,   125,    91,    93,    46,    41,    44,    96,    40,    32,
      10,    59
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned short yyr1[] =
{
       0,   132,   134,   133,   135,   136,   137,   137,   137,   137,
     139,   138,   138,   138,   138,   138,   138,   138,   138,   138,
     138,   140,   138,   138,   138,   138,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   138,   141,   141,   141,   141,
     141,   141,   142,   143,   143,   143,   143,   143,   144,   144,
     144,   146,   147,   145,   148,   148,   148,   148,   148,   148,
     148,   148,   149,   149,   150,   150,   151,   151,   151,   151,
     151,   151,   152,   152,   153,   153,   154,   154,   154,   154,
     154,   154,   154,   155,   155,   155,   155,   155,   155,   155,
     156,   156,   157,   157,   157,   158,   158,   158,   158,   158,
     159,   159,   160,   161,   160,   162,   162,   162,   162,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   162,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   162,   162,
     162,   163,   163,   163,   163,   163,   163,   163,   163,   163,
     163,   163,   163,   163,   163,   163,   163,   163,   163,   163,
     163,   163,   163,   163,   163,   163,   163,   163,   163,   163,
     163,   163,   163,   163,   163,   163,   163,   163,   163,   163,
     163,   163,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   165,   164,   164,   164,   166,   167,   167,   167,
     167,   167,   167,   168,   168,   168,   168,   169,   169,   170,
     170,   170,   170,   170,   170,   170,   170,   170,   171,   171,
     171,   171,   171,   171,   171,   171,   171,   171,   171,   171,
     173,   172,   174,   175,   174,   176,   174,   177,   178,   178,
     179,   179,   180,   180,   180,   181,   181,   181,   181,   181,
     181,   181,   181,   181,   182,   181,   183,   181,   181,   181,
     181,   181,   181,   181,   181,   181,   181,   181,   184,   181,
     181,   181,   181,   181,   181,   185,   186,   181,   187,   188,
     181,   181,   181,   181,   189,   190,   181,   191,   181,   192,
     193,   181,   194,   181,   195,   181,   196,   197,   181,   181,
     181,   181,   181,   198,   199,   199,   199,   199,   200,   200,
     200,   201,   201,   202,   202,   203,   203,   204,   204,   204,
     204,   206,   207,   205,   208,   208,   208,   209,   209,   209,
     209,   209,   209,   211,   212,   210,   213,   214,   210,   215,
     216,   216,   216,   217,   217,   218,   218,   219,   219,   220,
     220,   221,   221,   222,   222,   222,   223,   224,   224,   225,
     226,   227,   228,   228,   229,   229,   230,   230,   231,   231,
     232,   232,   233,   233,   234,   234,   235,   236,   235,   237,
     235,   238,   238,   238,   238,   239,   240,   241,   241,   241,
     241,   242,   243,   243,   243,   243,   244,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   245,   246,   247,
     247,   248,   249,   248,   248,   250,   250,   251,   251,   251,
     251,   251,   251,   251,   251,   251,   252,   252,   252,   252,
     252,   253,   253,   254,   255,   255,   256,   256,   257,   258,
     258,   259,   260,   259,   261,   261,   261,   262,   262,   263,
     264,   264,   264,   265,   265,   265,   265,   266,   266,   266,
     267,   267,   268,   268,   269,   269,   270,   270,   270,   271,
     271,   272,   272,   273
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     2,     4,     2,     1,     1,     3,     2,
       0,     4,     3,     3,     3,     2,     3,     3,     3,     3,
       3,     0,     5,     4,     3,     3,     3,     6,     5,     5,
       5,     3,     3,     3,     3,     1,     1,     3,     3,     2,
       2,     1,     1,     1,     1,     2,     2,     2,     1,     4,
       4,     0,     0,     6,     2,     3,     4,     5,     4,     5,
       2,     2,     1,     3,     1,     3,     1,     2,     3,     2,
       2,     1,     1,     3,     2,     3,     1,     4,     3,     3,
       3,     3,     1,     1,     4,     3,     3,     3,     3,     1,
       1,     1,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     4,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     5,     3,     6,     5,     5,     5,     5,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     4,
       4,     2,     2,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     2,     3,     3,
       3,     3,     0,     4,     5,     1,     1,     1,     2,     2,
       5,     2,     3,     3,     4,     4,     6,     1,     1,     1,
       2,     5,     2,     5,     4,     7,     3,     1,     4,     3,
       5,     7,     2,     5,     4,     6,     7,     9,     3,     1,
       0,     2,     1,     0,     3,     0,     4,     2,     2,     1,
       1,     3,     3,     4,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     0,     4,     0,     4,     3,     3,
       2,     4,     3,     3,     1,     4,     3,     1,     0,     6,
       2,     1,     2,     6,     6,     0,     0,     7,     0,     0,
       7,     5,     4,     5,     0,     0,     9,     0,     6,     0,
       0,     8,     0,     5,     0,     6,     0,     0,     9,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     1,     1,
       1,     1,     5,     1,     2,     1,     1,     1,     2,     1,
       3,     0,     0,     6,     2,     4,     4,     2,     4,     4,
       3,     2,     1,     0,     0,     6,     0,     0,     6,     5,
       1,     4,     2,     1,     1,     6,     1,     1,     1,     2,
       1,     2,     1,     1,     1,     1,     1,     1,     2,     3,
       3,     3,     3,     3,     0,     3,     1,     2,     3,     3,
       0,     3,     0,     2,     0,     2,     1,     0,     3,     0,
       5,     1,     1,     1,     1,     0,     2,     1,     1,     1,
       1,     3,     1,     1,     2,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     4,     2,     4,     2,     6,     4,     4,
       2,     4,     2,     2,     1,     0,     1,     1,     1,     1,
       1,     1,     3,     3,     1,     3,     2,     1,     2,     2,
       1,     1,     0,     5,     1,     2,     2,     1,     3,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     1,     0,     1,     0,     1,     1,     1,
       1,     1,     2,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short yydefact[] =
{
       2,     0,     0,     1,     0,     0,     0,     0,     0,   274,
       0,     0,   482,   295,   298,     0,   319,   320,   321,   322,
     284,   287,   352,   422,   421,   423,   424,     0,     0,   484,
      21,     0,   426,   425,   416,   273,   418,   417,   419,   420,
     412,   413,   429,   430,     0,     0,     0,     0,     0,   493,
     493,    71,   394,   392,   394,   394,   384,   390,     0,     0,
       0,     3,   482,     7,    35,    36,    44,    43,     0,    62,
       0,    66,    72,     0,    41,   215,     0,    48,   291,   265,
     266,   376,   377,   267,   268,   269,   270,   374,   375,   373,
     427,   271,     0,   272,   250,     6,     9,   319,   320,   284,
     287,   352,   484,    90,    91,     0,     0,     0,     0,    93,
       0,   323,     0,   427,   272,     0,   312,   140,   151,   141,
     164,   137,   157,   147,   146,   162,   145,   144,   139,   165,
     149,   138,   152,   156,   158,   150,   143,   159,   166,   161,
     160,   153,   163,   148,   136,   155,   154,   167,   168,   169,
     170,   171,   135,   142,   133,   134,   131,   132,    95,    97,
      96,   126,   127,   124,   108,   109,   110,   113,   115,   111,
     128,   129,   116,   117,   121,   112,   114,   105,   106,   107,
     118,   119,   120,   122,   123,   125,   130,   462,   314,    98,
      99,   461,     0,   160,   153,   163,   148,   131,   132,    95,
      96,     0,   100,   102,    15,   101,     0,    42,     0,     0,
       0,   427,     0,   272,     0,   490,   489,   482,     0,   491,
     483,     0,     0,   336,   335,     0,     0,   427,   272,     0,
       0,     0,   229,   216,   260,    46,   237,   493,   493,   467,
      47,    45,     0,    61,     0,   493,   351,    60,    39,     0,
      10,   485,   212,     0,     0,   191,     0,   192,    90,    91,
     280,     0,     0,     0,    62,   276,     0,   484,     0,   486,
     486,   217,   486,     0,   486,   464,    70,     0,    76,    82,
     409,   408,   410,   407,     0,   406,     0,     0,     0,     0,
       0,     0,     0,   414,   415,    40,   206,   207,     5,   483,
       0,     0,     0,     0,     0,     0,     0,   341,   344,     0,
      74,     0,    69,    67,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   493,     0,     0,     0,   356,   353,   292,   378,
       0,     0,   347,    54,   290,     0,   309,    92,   414,   415,
       0,   432,   307,   431,     0,   493,     0,     0,     0,   445,
     481,   480,   316,   103,     0,   493,   326,   325,     0,   324,
       0,     0,   493,     0,     0,     0,     0,     0,     0,     0,
       0,   492,     0,     0,     0,   493,     0,   304,   493,   257,
       0,     0,   230,   259,     0,   232,   286,     0,   253,   252,
     251,   229,   484,   493,   484,     0,    12,    14,    13,     0,
     288,     0,     0,     0,     0,     0,     0,   278,    63,     0,
     484,   218,   282,   488,   487,   219,   488,   221,   488,   466,
     283,   465,     0,   493,     0,   396,   405,   397,   411,   395,
     379,   393,   380,   381,   382,   383,     0,   386,   388,     0,
     389,     0,     0,     8,    16,    17,    18,    19,    20,    37,
      38,   493,     0,    25,    33,     0,    34,     0,    68,    75,
      24,   172,   260,    32,   188,   196,   201,   202,   203,   198,
     200,   210,   211,   204,   205,   181,   182,   208,   209,     0,
     197,   199,   193,   194,   195,   183,   184,   185,   186,   187,
     473,   478,   474,   479,   250,   350,     0,   473,   475,   474,
     476,   493,   473,   474,   250,   250,   493,   493,    26,   174,
      31,   180,    51,    55,     0,   434,     0,     0,    90,    91,
      94,     0,     0,   493,     0,   484,   450,   448,   447,   446,
     449,   457,     0,   445,     0,     0,   451,   493,   454,   493,
     493,   444,     0,     0,   275,   493,   493,   366,   493,   327,
     172,   477,   279,     0,   473,   474,   493,     0,     0,     0,
     360,     0,   302,   330,   329,   296,   328,   299,   477,   279,
       0,   473,   474,     0,     0,   236,   469,     0,   261,   258,
     493,     0,     0,   468,   285,     0,    41,     0,   255,     0,
     249,   493,     0,     0,     0,     0,     0,   223,    11,     0,
     213,     0,    23,    63,   277,   222,     0,   261,    79,    81,
       0,   473,   474,   399,     0,   385,   387,   391,   189,   190,
     339,     0,   342,   337,   264,     0,    73,     0,     0,     0,
       0,   349,    58,   281,     0,     0,   228,   348,    56,   227,
     346,    50,   345,    49,   357,   354,   493,   310,     0,     0,
     281,   313,     0,     0,   456,   458,   484,     0,   436,     0,
     440,   460,     0,   442,     0,   443,   317,   104,   367,   493,
     368,     0,   493,   333,     0,     0,   331,     0,   281,     0,
       0,     0,   301,   303,   362,     0,     0,     0,     0,   281,
       0,   493,     0,   234,   493,   493,     0,     0,   254,     0,
     242,   224,     0,   484,   493,   493,   225,     0,    22,   484,
      77,     0,   401,   402,   403,   398,   404,   338,     0,     0,
       0,   262,   173,   214,    30,   178,   179,    59,     0,    28,
     176,    29,   177,    57,     0,     0,    52,     0,   433,   308,
     463,   453,     0,   315,   452,   493,   493,   459,     0,   455,
     493,   445,     0,     0,     0,   370,   334,     0,     4,   372,
       0,   293,     0,   294,     0,   493,     0,     0,   305,   231,
       0,   233,   248,     0,   239,   493,   493,   256,     0,     0,
     289,   220,     0,   340,     0,   263,    27,   175,     0,     0,
       0,     0,   435,     0,   438,   439,   441,     0,   369,     0,
      83,    89,     0,   371,     0,   361,   363,   364,   359,   297,
     300,     0,   493,   493,     0,   238,     0,   244,   493,   226,
     400,   343,   358,   355,     0,   311,   493,     0,     0,   493,
       0,   493,   493,     0,   235,   240,     0,   493,     0,   243,
      53,   437,   318,   477,    88,     0,   473,   474,   365,   332,
     306,   493,     0,   245,   493,    84,   241,     0,   246,   493,
     247
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     1,     2,   374,   375,    62,    63,   419,   253,    64,
     208,    65,    66,   533,   666,   810,    67,    68,   263,    69,
      70,    71,    72,   209,   109,   110,   202,   203,   204,   563,
     520,   190,    74,   421,   234,   268,   656,   657,   235,   608,
     243,   244,   410,   609,   717,   599,   402,   269,   476,    75,
     206,   429,   619,   221,   707,   222,   708,   593,   831,   537,
     534,   757,   367,   369,   562,   771,   256,   378,   585,   695,
     696,   226,   642,   308,   471,   739,    77,    78,   354,   527,
     755,   526,   754,   390,   581,   828,   566,   689,   774,   778,
      79,    80,    81,    82,    83,    84,    85,   290,   456,    86,
     292,   286,   284,   449,   634,   731,   735,   633,    87,   285,
      88,    89,   211,    91,   212,   213,   362,   536,   554,   555,
     556,   557,   558,   559,   560,   767,   680,   192,   368,   273,
     270,   239,   115,   541,   515,   372,   218,   252,   435,   379,
     220,    95
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -648
static const short yypact[] =
{
    -648,    97,  2201,  -648,  5482,  7847,  8144,  3851,  5141,  -648,
    6821,  6821,  3732,  -648,  -648,  7946,  5688,  5688,  -648,  -648,
    5688,  4484,  4587,  -648,  -648,  -648,  -648,  6821,  5025,    -5,
    -648,     2,  -648,  -648,  3969,  4072,  -648,  -648,  4175,  -648,
    -648,  -648,  -648,  -648,  7542,  7542,    34,  3142,  6821,  6924,
    7542,  8243,  4909,  -648,  -648,  -648,    80,    98,    56,  7645,
    7542,  -648,   331,   721,   498,  -648,  -648,   129,   151,  -648,
      49,  8045,  -648,   164,  9114,   409,   422,    12,    75,  -648,
    -648,   144,  -648,  -648,  -648,  -648,  -648,  -648,  -648,  -648,
     414,  -648,   189,   424,    68,  -648,   721,  -648,  -648,  -648,
      27,   147,    -5,   127,   376,  6821,    34,  3273,   445,  -648,
      67,  -648,   470,  -648,  -648,    68,  -648,  -648,  -648,  -648,
    -648,  -648,  -648,  -648,  -648,  -648,  -648,  -648,  -648,  -648,
    -648,  -648,  -648,  -648,  -648,  -648,  -648,  -648,  -648,  -648,
      26,    37,   100,   135,  -648,  -648,  -648,  -648,  -648,  -648,
    -648,  -648,  -648,  -648,  -648,  -648,   165,   309,   476,  -648,
     477,  -648,  -648,  -648,  -648,  -648,  -648,  -648,  -648,  -648,
    -648,  -648,  -648,  -648,  -648,  -648,  -648,  -648,  -648,  -648,
    -648,  -648,  -648,  -648,  -648,  -648,  -648,  -648,  -648,  -648,
    -648,  -648,   506,  -648,  -648,  -648,  -648,  -648,  -648,  -648,
    -648,  4909,  -648,  -648,   158,  -648,  2570,   498,    62,   225,
     522,   177,   255,   179,    62,  -648,  -648,   331,   195,  -648,
     223,  6821,  6821,  -648,  -648,   523,   336,    46,    92,  7542,
    7542,  7542,  -648,  9114,   294,  -648,  -648,   263,   280,  -648,
    -648,  -648,  5374,  -648,  5791,  5688,  -648,  -648,  -648,   217,
    -648,  -648,   308,   321,  3404,  -648,   526,   379,  -648,  -648,
    -648,  3142,   340,   343,   346,   498,  7542,    -5,   400,   -30,
      57,  -648,   210,   407,    57,  -648,  -648,   546,   555,   556,
    -648,  -648,  -648,  -648,   393,  -648,   412,   437,   591,   420,
     519,   454,    14,   495,   497,  -648,  -648,  -648,  -648,  3629,
    6821,  6821,  6821,  6821,  5482,  6821,  6821,  -648,  -648,  7027,
    -648,  3142,  8243,   444,  7027,  7542,  7542,  7542,  7542,  7542,
    7542,  7542,  7542,  7542,  7542,  7542,  7542,  7542,  7542,  7542,
    7542,  7542,  7542,  7542,  7542,  7542,  7542,  7542,  7542,  7542,
    7542,  1575,  6924,  8306,  8372,  8372,  -648,  -648,  -648,  -648,
    7645,  7645,  -648,   484,  -648,   308,   498,   573,  -648,  -648,
     331,  -648,  -648,  -648,  8438,  6924,  8372,  2570,  6821,   571,
    -648,  -648,  -648,  -648,   566,   578,  -648,  -648,  2692,   609,
    7542,  8504,  6924,  8570,  7542,  7542,  2936,   623,  3526,  7130,
     631,  -648,    21,    21,  8636,  6924,  8702,  -648,   517,  -648,
    7542,  5894,  -648,  -648,  5997,  -648,  -648,   528,  5585,  -648,
    -648,   129,    -5,   532,   252,   534,  -648,  -648,  -648,  5141,
    -648,  7542,  3404,   544,  8504,  8570,   535,  -648,   540,   542,
     737,  -648,  -648,  7233,  -648,  -648,  7542,  -648,  7542,  -648,
    -648,  -648,  8768,  6924,  8834,  -648,  -648,  -648,  -648,  -648,
    -648,  -648,  -648,  -648,  -648,  -648,    40,  -648,  -648,   545,
    -648,  7542,  7542,   721,  -648,  -648,  -648,  -648,  -648,  -648,
    -648,    52,  7542,  -648,   543,   550,  -648,   557,  -648,  -648,
    -648,  8240,  -648,  -648,   379,  9183,  9183,  9183,  9183,   608,
     608,  9200,  2012,  9183,  9183,  9131,  9131,    16,    16,  2419,
     608,   608,    31,    31,   500,   334,   334,   379,   379,   379,
    2320,  4690,  2439,  4793,   147,  -648,   548,   464,  -648,   465,
    -648,  4587,  -648,  -648,  1156,  1156,    52,    52,  -648,  9114,
    -648,  9114,  -648,  -648,   331,  -648,  6821,  2570,   386,   275,
    -648,   147,   562,   147,   676,    72,   590,  -648,  -648,  -648,
    -648,   644,   651,   819,  2570,   331,  -648,   576,  -648,   586,
     599,  -648,  5257,  5141,  -648,  7542,   689,  -648,   539,  -648,
    2300,  4278,  4381,   603,   274,   284,   689,   717,   724,  7542,
     605,    62,  -648,  -648,  -648,  -648,  -648,  -648,    91,   140,
     616,   300,   358,  6821,   654,  -648,  -648,  7542,   294,  -648,
     629,  7542,   294,  -648,  -648,  7542,  9030,    36,  -648,   624,
    -648,   639,   645,  6100,  8372,  8372,   647,  -648,  -648,  6821,
    9114,   633,  -648,   458,  -648,  -648,  7542,  -648,   386,   275,
     646,   306,   356,  -648,   685,  -648,  -648,  -648,   379,   379,
    -648,  7748,  -648,  -648,  -648,  7336,  -648,  7542,  7542,  7645,
    7542,  -648,   484,   572,  7645,  7645,  -648,  -648,   484,  -648,
    -648,  -648,  -648,  -648,  -648,  -648,    52,  -648,   331,   763,
    -648,  -648,   650,  7542,  -648,  -648,    -5,   766,  -648,   819,
    -648,  -648,   238,  -648,   691,  -648,  -648,  -648,   655,   700,
    -648,  3039,   776,  -648,  6821,   779,  -648,  7542,   406,  7542,
    7542,   781,  -648,  -648,  -648,  7439,  2814,  3526,  3526,   361,
      21,   517,  6203,  -648,   517,   517,  6306,   661,  -648,  6409,
    -648,  -648,   129,   252,   147,   147,  -648,   167,  -648,   737,
     583,  3404,  -648,  -648,  -648,  -648,  -648,  -648,   684,  3526,
    7542,   668,  9114,  9114,  -648,  9114,  9114,  -648,  7645,  -648,
    9114,  -648,  9114,  -648,  3526,  3404,  -648,  2570,  -648,  -648,
    -648,  -648,   670,  -648,  -648,   671,   599,  -648,   590,  -648,
     599,   571,  7542,  8243,    62,  -648,  -648,  3526,  -648,  -648,
      62,  -648,  7542,  -648,  7542,   398,   786,   788,  -648,  -648,
    7542,  -648,  -648,  7542,  -648,   673,   674,  -648,  7542,   694,
    -648,  -648,   693,  -648,   811,  -648,  -648,  9114,   812,   703,
    3404,   815,  -648,   238,  -648,  -648,  -648,  2570,  -648,   628,
     555,   556,  2570,  -648,  2692,  -648,  -648,  -648,  -648,  -648,
    -648,  3526,  9051,   517,  6512,  -648,  6615,  -648,   517,  -648,
    -648,  -648,  -648,  -648,   706,  -648,   599,   820,  8900,  6924,
    8966,   578,   539,   822,  -648,  -648,  7542,   708,  7542,  -648,
    -648,  -648,  -648,    28,   275,   712,   101,   160,  -648,  -648,
    -648,   517,  6718,  -648,   517,   583,  -648,  7542,  -648,   517,
    -648
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -648,  -648,  -648,  -302,  1211,  -648,    66,  -648,  -648,    42,
     137,   -56,  -648,  -488,  -648,  -648,    29,   -13,  -206,   -41,
     768,  -648,   -25,  1229,   -32,   831,     4,   -18,  -648,  -648,
      10,  -648,   -16,  -648,  1444,  -306,    -9,  -485,   118,  -648,
       5,  -648,  -648,  -648,  -648,    -1,   253,    41,   541,    15,
    -648,  -648,  -648,  -648,  -648,  -648,  -648,  -648,  -648,  -648,
    -648,  -648,  -648,  -648,  -648,  -648,   174,  -209,  -369,     7,
    -489,   213,  -424,  -648,  -648,  -648,  -238,  -648,   782,  -648,
    -648,  -648,  -648,  -362,  -648,  -648,     6,  -648,  -648,  -648,
    -648,  -648,  -648,   785,  -648,  -648,  -648,  -648,  -648,  -648,
    -648,  -648,   554,  -226,  -648,  -648,  -648,  -648,    39,  -648,
    -648,  -648,  1005,   855,  1357,  1200,  -648,  -648,    99,   310,
     196,  -648,  -645,   197,  -647,  -346,  -509,  -648,  -648,  -648,
      -8,  -373,   731,  -262,  -648,  -648,   -40,    30,   248,    61,
     817,   664
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -494
static const short yytable[] =
{
     233,   233,   223,   295,   233,   386,   264,   414,   238,   238,
     250,   188,   238,   246,   260,   236,   236,   189,   189,   236,
     111,   111,   298,   561,   587,   577,   276,   247,   255,   257,
     111,   603,   766,   233,   233,   770,   516,   769,   189,   660,
     662,   -86,   274,   296,   297,   232,   232,   205,   583,   232,
     683,   685,   207,   207,   207,   426,   283,   237,   237,   542,
     451,   237,   189,   603,   457,   544,   111,   205,   360,   248,
      96,   -83,   459,   219,   357,   376,   573,   692,   267,   514,
     315,   521,   524,   525,   258,   352,   111,   701,   259,   590,
     265,   272,   246,   344,   346,   315,   433,     3,   445,   353,
     434,   346,   664,   665,   543,   477,   352,  -422,   305,   306,
     328,   329,   293,   294,   -85,   460,   -86,   -89,  -421,   514,
     400,   521,   254,   219,   640,   251,   584,   336,   337,   338,
     339,   340,   355,   -86,   543,   240,   345,   630,   241,   446,
     447,   335,   336,   337,   338,   339,   340,   356,   214,   217,
    -422,   215,   216,  -470,   -83,   242,  -473,   307,   -86,   -86,
     641,  -421,   716,   543,   747,   -88,   846,   377,   769,   635,
     753,   363,   -76,   -87,   361,   310,    76,   387,    76,   112,
     112,  -423,   543,   436,   210,   210,   210,   434,   347,   225,
     210,   210,   215,   216,   210,   347,   245,   215,   216,   -86,
     -89,   210,   251,   305,   306,   283,   -85,   561,  -416,   289,
     388,   189,   389,   233,   233,   296,  -424,   -79,   -82,  -473,
     264,    76,   210,   210,  -423,   277,   233,   291,   233,   233,
     636,   -85,   -85,   210,   238,   669,   238,   238,    53,   660,
     662,   236,   756,   236,   236,   277,  -426,  -470,   -88,  -416,
     430,  -416,   677,   473,   309,  -470,   814,   815,   480,  -424,
    -428,   816,   385,   207,   207,   -87,   -81,   314,  -474,   416,
     264,   232,   350,   232,   411,   245,   417,   418,   219,   210,
     -83,    76,   -89,   237,   373,   237,   413,   478,   768,  -426,
     -87,   -87,   800,   233,   528,   530,   826,   431,   481,   484,
     485,   486,   487,   488,   489,   490,   491,   492,   493,   494,
     495,   496,   497,   498,   499,   500,   501,   502,   503,   504,
     505,   506,   507,   508,   509,   -85,   233,   111,   380,   551,
     552,   -78,   540,   614,   529,   531,   438,   861,   384,   603,
     434,   788,   207,   207,   207,   207,   603,   469,   470,   233,
     475,   513,   724,   725,   391,   475,  -279,   699,   392,   393,
     407,   397,   409,   412,   570,   463,   233,   700,   529,   531,
     468,   267,   706,   233,   513,   723,   615,   -85,   400,   233,
      76,   -80,   251,   -87,   233,   233,   -84,   -87,   233,   401,
    -425,   513,   606,   600,   267,   210,   210,  -279,   315,  -279,
     611,   618,  -471,  -474,   513,   620,   404,   610,   -85,   -78,
     545,   267,  -477,   691,   -78,   389,   210,   233,   210,   210,
     233,   535,   233,   827,   267,   561,   -78,   233,    76,   189,
     580,   -78,   -78,  -425,   513,    76,   420,   464,   465,   466,
     467,   422,   612,   315,   616,   638,   639,   338,   339,   340,
     265,   445,   513,   586,   586,   811,   233,  -419,   205,   -80,
     625,   215,   216,   603,   -80,   427,   -87,  -477,   428,   -84,
     445,   -64,   267,    76,   210,   210,   210,   210,    76,   210,
     210,   -80,   -80,   210,   -80,    76,   277,   -77,   210,   782,
    -323,   405,   446,   447,   448,   445,  -471,  -428,  -419,   603,
    -419,   358,   359,   341,  -471,   651,  -477,   351,  -477,   -84,
    -477,   446,   447,   450,  -473,   847,   210,   -83,   437,   652,
     439,   454,   441,   432,   210,   210,   658,   -89,   440,   661,
     663,  -323,   651,  -323,   305,   306,   446,   447,   452,   210,
     -76,    76,   210,   865,   342,   687,   343,   654,   655,   233,
     -82,   364,    76,   694,   691,   458,   210,  -416,  -419,   461,
      76,   462,    76,   233,   315,   822,   686,   -85,   -87,   210,
     479,   824,   189,   189,   532,   672,   564,   445,   207,   328,
     329,   233,   210,   -65,   -73,   233,   565,   370,   543,   233,
     -78,   -80,   365,   744,   366,   667,    76,   233,   749,   751,
    -416,  -419,   205,   381,   394,   600,   688,   424,   287,   288,
     729,   336,   337,   338,   339,   340,   678,   210,   446,   447,
     455,   546,   569,   547,   548,   549,   550,   442,   223,   233,
     371,   742,   743,   745,   746,   207,  -427,  -272,   750,   752,
     389,   582,   722,   594,   382,   395,   383,   396,   382,   445,
     425,   595,   453,   604,  -280,   748,   111,   233,   613,   617,
     623,   727,   551,   552,  -281,   622,   -73,   624,   443,  -260,
     444,   653,   315,   668,   637,   -84,   645,  -427,  -272,  -427,
    -272,   745,   646,   750,   752,   670,   671,   328,   329,   233,
     446,   447,   806,   673,   674,  -280,   233,  -280,   -77,   553,
     233,   675,   679,   233,   691,  -281,   762,  -281,   796,   848,
     210,    76,   682,   271,   275,   794,   333,   334,   335,   336,
     337,   338,   339,   340,   233,   684,   698,   702,    76,   758,
     710,   705,   807,    94,   703,    94,   207,   732,   733,   709,
     734,    94,    94,    94,    42,    43,   230,    94,    94,   718,
     849,    94,   850,   799,   728,   712,   233,   795,    94,   801,
     300,   301,   302,   303,   304,   719,   807,   210,   233,   730,
     721,   586,   726,   759,   832,   760,   763,   233,    94,    94,
      94,   772,   233,   552,   773,   777,   797,   210,   111,   781,
      94,   783,   803,   210,  -261,   812,   829,   813,   830,   834,
     836,   315,   316,   317,   318,   319,   320,   321,   322,   323,
     324,   325,   326,   327,   840,   225,   328,   329,   233,   839,
     233,   841,   842,   210,   843,   845,   857,   860,   210,   210,
     862,   780,   870,   233,   872,   875,    94,   116,    94,   313,
     233,   330,   233,   331,   332,   333,   334,   335,   336,   337,
     338,   339,   340,   713,   738,   483,   233,   868,   513,   869,
     348,   233,   191,   676,   720,    76,   349,   251,   210,   546,
     817,   547,   548,   549,   550,   764,   765,     0,   267,   299,
      76,    76,    76,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   403,   403,     0,     0,    76,     0,     0,     0,   415,
     551,   552,     0,    76,     0,     0,     0,     0,     0,     0,
       0,     0,   210,     0,     0,     0,     0,     0,    76,    76,
       0,    76,     0,     0,     0,     0,     0,    94,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   819,     0,     0,
       0,    76,    94,    94,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   789,     0,     0,   791,   792,     0,
       0,     0,     0,    94,     0,    94,    94,     0,     0,     0,
       0,     0,     0,     0,    76,    94,     0,     0,     0,     0,
       0,    76,    94,     0,     0,     0,    76,     0,    76,     0,
       0,     0,     0,     0,     0,    76,   271,    90,     0,    90,
     113,   113,   113,     0,     0,     0,     0,     0,     0,     0,
     227,     0,     0,   210,     0,     0,     0,     0,     0,   271,
      94,    94,    94,    94,    94,    94,    94,    94,     0,   567,
      94,     0,    94,     0,     0,    94,   271,     0,   835,   837,
       0,     0,    90,     0,     0,     0,   278,     0,     0,   271,
       0,     0,   403,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    94,     0,     0,   278,   403,     0,     0,
       0,    94,    94,     0,     0,   854,   855,     0,     0,     0,
       0,   859,     0,     0,     0,     0,    94,     0,    94,    94,
       0,     0,     0,     0,     0,     0,     0,   271,     0,    94,
     873,     0,    90,    94,     0,     0,     0,    94,     0,    94,
       0,     0,     0,     0,   876,     0,    94,   878,     0,     0,
       0,     0,   880,     0,     0,   643,     0,     0,     0,    94,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    94,     0,     0,  -493,     0,     0,     0,
       0,     0,     0,     0,  -493,  -493,  -493,     0,     0,  -493,
    -493,  -493,     0,  -493,    94,     0,     0,     0,     0,     0,
       0,     0,     0,  -493,     0,   659,     0,     0,   659,   659,
     643,   643,  -493,  -493,     0,  -493,  -493,  -493,  -493,  -493,
       0,     0,    93,     0,    93,   114,   114,   659,     0,     0,
       0,    90,     0,    61,     0,   228,     0,     0,     0,     0,
       0,   681,     0,   681,   681,     0,     0,     0,     0,   690,
     693,    73,   693,    73,     0,     0,     0,  -493,     0,     0,
     693,     0,     0,     0,   224,     0,     0,    93,     0,     0,
       0,   279,     0,     0,     0,     0,     0,     0,   262,    90,
       0,  -493,     0,     0,   403,     0,    90,    94,    94,     0,
       0,   279,     0,     0,     0,   403,    73,  -493,     0,     0,
    -493,  -493,     0,     0,   245,    94,  -493,  -493,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    90,     0,     0,    93,     0,    90,
       0,     0,     0,     0,     0,     0,    90,   278,   262,     0,
       0,     0,     0,     0,    94,     0,     0,     0,     0,     0,
     643,     0,     0,     0,     0,     0,    73,     0,     0,     0,
       0,     0,     0,     0,    94,     0,     0,     0,     0,     0,
      94,     0,     0,   775,     0,     0,   779,     0,     0,    92,
       0,    92,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    90,     0,     0,   403,     0,     0,   403,   403,
      94,     0,     0,    90,     0,    94,    94,     0,   659,   659,
       0,    90,     0,    90,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    92,     0,    93,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    94,     0,     0,    94,     0,    90,     0,   681,
     681,     0,     0,     0,   681,    73,     0,    94,    94,    94,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   693,
       0,     0,     0,     0,    93,     0,     0,     0,     0,   403,
     403,    93,    94,     0,    92,   423,     0,     0,     0,     0,
      94,     0,   262,     0,     0,     0,     0,     0,     0,    94,
       0,     0,     0,    73,     0,    94,    94,     0,    94,     0,
      73,     0,     0,     0,     0,     0,   403,   403,     0,    93,
       0,     0,   403,     0,    93,     0,     0,     0,    94,     0,
     681,    93,   279,   271,     0,   567,   693,     0,     0,     0,
       0,   403,   262,     0,     0,     0,     0,     0,    73,     0,
       0,     0,     0,    73,     0,   403,     0,     0,   403,     0,
      73,    94,    90,   403,     0,     0,     0,     0,    94,     0,
       0,     0,     0,    94,     0,    94,     0,     0,     0,    90,
       0,     0,    94,    92,     0,     0,     0,    93,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    93,     0,
      94,     0,     0,     0,     0,     0,    93,     0,    93,   568,
       0,     0,     0,     0,     0,     0,    73,   576,     0,   578,
       0,     0,     0,     0,     0,     0,     0,    73,     0,     0,
       0,    92,     0,     0,     0,    73,     0,    73,    92,     0,
       0,     0,    93,     0,     0,   510,   511,     0,     0,   512,
       0,     0,     0,   621,     0,     0,     0,   161,   162,   163,
     164,   165,   166,     0,   167,   168,   227,     0,   169,     0,
       0,    73,   170,   171,   172,   173,    92,     0,     0,     0,
       0,    92,     0,     0,     0,     0,   174,     0,    92,     0,
       0,     0,     0,   398,   399,     0,     0,     0,     0,     0,
       0,   175,   176,   177,   178,   179,   180,   181,   182,   183,
     184,     0,     0,   185,     0,     0,    90,     0,     0,     0,
       0,     0,   186,     0,     0,     0,     0,     0,     0,     0,
       0,    90,    90,    90,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    92,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    92,    90,    93,     0,     0,
       0,     0,     0,    92,    90,    92,     0,     0,     0,     0,
       0,     0,     0,   474,    93,     0,     0,     0,   482,    90,
      90,     0,    90,     0,     0,     0,    73,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   820,    92,
       0,     0,    90,    73,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    90,     0,     0,     0,     0,
       0,     0,    90,     0,     0,     0,     0,    90,     0,    90,
       0,     0,     0,   482,   736,     0,    90,     0,     0,     0,
       0,   228,     0,     0,   596,   598,     0,     0,   602,     0,
       0,     0,   607,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     224,     0,     0,     0,     0,     0,     0,   627,     0,     0,
     602,     0,   627,     0,     0,     0,     0,     0,     0,     0,
       0,    93,     0,     0,    92,     0,     0,     0,     0,     0,
       0,     0,   776,     0,     0,     0,    93,    93,    93,     0,
       0,    92,     0,     0,     0,     0,   644,   785,   786,   787,
      73,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    93,     0,     0,     0,    73,    73,    73,     0,    93,
       0,     0,   802,     0,     0,     0,     0,     0,     0,     0,
     804,     0,     0,     0,    93,    93,     0,    93,     0,     0,
      73,     0,     0,     0,     0,   808,   809,     0,    73,     0,
       0,     0,     0,   821,     0,     0,     0,    93,     0,     0,
       0,     0,     0,    73,    73,     0,    73,     0,   823,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   818,     0,     0,     0,    73,     0,     0,   482,
      93,     0,     0,     0,     0,     0,     0,    93,     0,     0,
       0,   844,    93,   704,    93,     0,     0,     0,     0,     0,
       0,    93,     0,   851,     0,   852,     0,     0,     0,    73,
       0,   711,   853,     0,     0,   714,    73,     0,    92,   715,
       0,    73,     0,    73,     0,     0,     0,   598,     0,     0,
      73,     0,     0,    92,    92,    92,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   315,   316,   317,   318,
     319,   320,   321,   322,     0,   324,   325,     0,    92,   741,
       0,   328,   329,     0,     0,     0,    92,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    92,    92,     0,    92,     0,     0,   761,   331,   332,
     333,   334,   335,   336,   337,   338,   339,   340,     0,     0,
       0,     0,     0,     0,    92,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   627,
       0,     0,     0,     0,     0,     0,   602,     0,     0,     0,
       0,     0,     0,   602,     0,     0,     0,    92,     0,     0,
       0,     0,     0,     0,    92,     0,     0,     0,     0,    92,
       0,    92,     0,     0,   805,     0,     0,     0,    92,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -493,     4,     0,     5,     6,     7,     8,     9,     0,
       0,     0,    10,    11,     0,     0,   627,    12,     0,    13,
      14,    15,    16,    17,    18,    19,     0,     0,   825,     0,
      20,    21,    22,    23,    24,    25,    26,   833,     0,    27,
       0,     0,   838,     0,     0,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,     0,
      42,    43,     0,    44,    45,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   598,     0,
     602,     0,     0,    46,     0,     0,    47,    48,     0,    49,
      50,     0,    51,     0,    52,    53,    54,    55,    56,    57,
     871,     0,   874,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   602,    58,    59,    60,
    -477,   879,     0,     0,     0,     0,     0,     0,  -477,  -477,
    -477,  -493,  -493,     0,  -477,  -477,     0,  -477,     0,     0,
       0,     0,     0,   647,     0,     0,  -477,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -477,  -477,     0,  -477,
    -477,  -477,  -477,  -477,   315,   316,   317,   318,   319,   320,
     321,   322,   323,   324,   325,   326,   327,     0,     0,   328,
     329,     0,     0,     0,  -477,  -477,  -477,  -477,  -477,  -477,
    -477,  -477,  -477,  -477,  -477,  -477,  -477,     0,     0,  -477,
    -477,  -477,     0,   649,   330,     0,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,     0,     0,     0,     0,
       0,     0,     0,   -86,  -477,     0,  -477,  -477,  -477,  -477,
    -477,  -477,  -477,  -477,  -477,  -477,     0,     0,     0,  -279,
    -477,  -477,  -477,     0,  -477,  -477,   -79,  -279,  -279,  -279,
    -477,  -477,     0,  -279,  -279,     0,  -279,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -279,  -279,     0,  -279,  -279,
    -279,  -279,  -279,   315,   316,   317,   318,   319,   320,   321,
     322,   323,   324,   325,   326,   327,     0,     0,   328,   329,
       0,     0,     0,  -279,  -279,  -279,  -279,  -279,  -279,  -279,
    -279,  -279,  -279,  -279,  -279,  -279,     0,     0,  -279,  -279,
    -279,     0,   650,   330,   648,   331,   332,   333,   334,   335,
     336,   337,   338,   339,   340,     0,     0,     0,     0,     0,
       0,     0,   -88,  -279,     0,  -279,  -279,  -279,  -279,  -279,
    -279,  -279,  -279,  -279,  -279,     0,     0,     0,     0,     0,
    -279,  -279,     0,  -279,  -279,   -81,     0,     0,     0,  -279,
    -279,     4,     0,     5,     6,     7,     8,     9,  -493,  -493,
    -493,    10,    11,     0,     0,  -493,    12,     0,    13,    14,
      15,    16,    17,    18,    19,     0,     0,     0,     0,    20,
      21,    22,    23,    24,    25,    26,     0,     0,    27,     0,
       0,     0,     0,     0,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,     0,    42,
      43,     0,    44,    45,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    46,     0,     0,    47,    48,     0,    49,    50,
       0,    51,     0,    52,    53,    54,    55,    56,    57,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    58,    59,    60,     0,
       0,     0,     0,     4,     0,     5,     6,     7,     8,     9,
    -493,  -493,  -493,    10,    11,     0,  -493,  -493,    12,     0,
      13,    14,    15,    16,    17,    18,    19,     0,     0,     0,
       0,    20,    21,    22,    23,    24,    25,    26,     0,     0,
      27,     0,     0,     0,     0,     0,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
       0,    42,    43,     0,    44,    45,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    46,     0,     0,    47,    48,     0,
      49,    50,     0,    51,     0,    52,    53,    54,    55,    56,
      57,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    58,    59,
      60,     0,     0,     0,     0,     4,     0,     5,     6,     7,
       8,     9,  -493,  -493,  -493,    10,    11,     0,     0,  -493,
      12,  -493,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,    20,    21,    22,    23,    24,    25,    26,
       0,     0,    27,     0,     0,     0,     0,     0,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,     0,    42,    43,     0,    44,    45,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    46,     0,     0,    47,
      48,     0,    49,    50,     0,    51,     0,    52,    53,    54,
      55,    56,    57,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      58,    59,    60,     0,     0,     0,     0,     4,     0,     5,
       6,     7,     8,     9,  -493,  -493,  -493,    10,    11,     0,
       0,  -493,    12,     0,    13,    14,    15,    16,    17,    18,
      19,     0,     0,     0,     0,    20,    21,    22,    23,    24,
      25,    26,     0,     0,    27,     0,     0,     0,     0,     0,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,     0,    42,    43,     0,    44,    45,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    46,     0,
       0,    47,    48,     0,    49,    50,     0,    51,     0,    52,
      53,    54,    55,    56,    57,     0,     0,     0,     0,     0,
       4,     0,     5,     6,     7,     8,     9,     0,  -493,  -493,
      10,    11,    58,    59,    60,    12,     0,    13,    14,    15,
      16,    17,    18,    19,     0,     0,  -493,  -493,    20,    21,
      22,    23,    24,    25,    26,     0,     0,    27,     0,     0,
       0,     0,     0,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,     0,    42,    43,
       0,    44,    45,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    46,     0,     0,    47,    48,     0,    49,    50,     0,
      51,     0,    52,    53,    54,    55,    56,    57,     0,     0,
       0,     0,     0,     4,     0,     5,     6,     7,     8,     9,
       0,     0,     0,    10,    11,    58,    59,    60,    12,     0,
      13,    14,    15,    16,    17,    18,    19,     0,     0,  -493,
    -493,    20,    21,    22,    23,    24,    25,    26,     0,     0,
      27,     0,     0,     0,     0,     0,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
       0,    42,    43,     0,    44,    45,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    46,     0,     0,   261,    48,     0,
      49,    50,     0,    51,     0,    52,    53,    54,    55,    56,
      57,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    58,    59,
      60,     0,     0,     0,     0,     0,     0,  -493,     0,     0,
       0,     0,  -493,  -493,     4,     0,     5,     6,     7,     8,
       9,     0,     0,     0,    10,    11,     0,     0,     0,    12,
       0,    13,    14,    15,    16,    17,    18,    19,     0,     0,
       0,     0,    20,    21,    22,    23,    24,    25,    26,     0,
       0,    27,     0,     0,     0,     0,     0,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,     0,    42,    43,     0,    44,    45,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    46,     0,     0,    47,    48,
       0,    49,    50,     0,    51,     0,    52,    53,    54,    55,
      56,    57,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    58,
      59,    60,     0,     0,     0,     0,     0,     0,  -493,     0,
       0,     0,     0,  -493,  -493,     4,     0,     5,     6,     7,
       8,     9,     0,     0,     0,    10,    11,     0,     0,     0,
      12,     0,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,    20,    21,    22,    23,    24,    25,    26,
       0,     0,    27,     0,     0,     0,     0,     0,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,     0,    42,    43,     0,    44,    45,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    46,     0,     0,    47,
      48,     0,    49,    50,     0,    51,     0,    52,    53,    54,
      55,    56,    57,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      58,    59,    60,     0,     0,  -493,     0,     4,     0,     5,
       6,     7,     8,     9,  -493,  -493,  -493,    10,    11,     0,
       0,     0,    12,     0,    13,    14,    15,    16,    17,    18,
      19,     0,     0,     0,     0,    20,    21,    22,    23,    24,
      25,    26,     0,     0,    27,     0,     0,     0,     0,     0,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,     0,    42,    43,     0,    44,    45,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    46,     0,
       0,    47,    48,     0,    49,    50,     0,    51,     0,    52,
      53,    54,    55,    56,    57,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     8,     9,     0,     0,     0,
      10,    11,    58,    59,    60,    12,     0,    13,    14,    15,
      16,    17,    18,    19,     0,     0,  -493,  -493,    20,    21,
      22,    23,    24,    25,    26,     0,     0,    27,     0,     0,
       0,     0,     0,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,     0,    42,    43,
       0,    44,    45,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    46,     0,     0,    47,    48,     0,    49,    50,     0,
      51,     0,    52,    53,    54,    55,    56,    57,     0,     0,
       0,     0,     0,     0,     0,     5,     6,     7,     0,     9,
       0,     0,     0,    10,    11,    58,    59,    60,    12,     0,
      13,    14,    15,    16,    17,    18,    19,     0,     0,     0,
     391,    20,    21,    22,    23,    24,    25,    26,     0,     0,
      27,     0,     0,     0,     0,     0,     0,    29,     0,     0,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
       0,    42,    43,     0,    44,    45,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    46,     0,     0,   107,    48,     0,
      49,    50,     0,     0,     0,    52,    53,    54,    55,    56,
      57,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    58,    59,
      60,     0,     0,     0,   117,   118,   119,   120,   121,   122,
     123,   124,   215,   216,   125,   126,   127,   128,   129,     0,
       0,   130,   131,   132,   133,   134,   135,   136,     0,     0,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   152,   153,   154,   155,   156,
     157,   158,   159,    36,    37,   160,    39,     0,     0,     0,
       0,     0,     0,   161,   162,   163,   164,   165,   166,     0,
     167,   168,     0,     0,   169,     0,     0,     0,   170,   171,
     172,   173,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   174,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,     0,     0,   185,
       0,     0,  -470,  -470,  -470,     0,  -470,     0,   186,   187,
    -470,  -470,     0,     0,     0,  -470,     0,  -470,  -470,  -470,
    -470,  -470,  -470,  -470,     0,  -470,     0,     0,  -470,  -470,
    -470,  -470,  -470,  -470,  -470,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -470,     0,     0,  -470,  -470,  -470,
    -470,  -470,  -470,  -470,  -470,  -470,  -470,     0,  -470,  -470,
       0,  -470,  -470,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -470,     0,     0,  -470,  -470,     0,  -470,  -470,     0,
    -470,  -470,  -470,  -470,  -470,  -470,  -470,  -470,     0,     0,
       0,     0,     0,     0,     0,  -472,  -472,  -472,     0,  -472,
       0,     0,     0,  -472,  -472,  -470,  -470,  -470,  -472,  -470,
    -472,  -472,  -472,  -472,  -472,  -472,  -472,  -470,  -472,     0,
       0,  -472,  -472,  -472,  -472,  -472,  -472,  -472,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -472,     0,     0,
    -472,  -472,  -472,  -472,  -472,  -472,  -472,  -472,  -472,  -472,
       0,  -472,  -472,     0,  -472,  -472,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -472,     0,     0,  -472,  -472,     0,
    -472,  -472,     0,  -472,  -472,  -472,  -472,  -472,  -472,  -472,
    -472,     0,     0,     0,     0,     0,     0,     0,  -471,  -471,
    -471,     0,  -471,     0,     0,     0,  -471,  -471,  -472,  -472,
    -472,  -471,  -472,  -471,  -471,  -471,  -471,  -471,  -471,  -471,
    -472,  -471,     0,     0,  -471,  -471,  -471,  -471,  -471,  -471,
    -471,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -471,     0,     0,  -471,  -471,  -471,  -471,  -471,  -471,  -471,
    -471,  -471,  -471,     0,  -471,  -471,     0,  -471,  -471,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -471,     0,     0,
    -471,  -471,     0,  -471,  -471,     0,  -471,  -471,  -471,  -471,
    -471,  -471,  -471,  -471,     0,     0,     0,     0,     0,     0,
       0,  -473,  -473,  -473,     0,  -473,     0,     0,     0,  -473,
    -473,  -471,  -471,  -471,  -473,  -471,  -473,  -473,  -473,  -473,
    -473,  -473,  -473,  -471,     0,     0,     0,  -473,  -473,  -473,
    -473,  -473,  -473,  -473,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -473,     0,     0,  -473,  -473,  -473,  -473,
    -473,  -473,  -473,  -473,  -473,  -473,     0,  -473,  -473,     0,
    -473,  -473,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -473,   697,     0,  -473,  -473,     0,  -473,  -473,     0,  -473,
    -473,  -473,  -473,  -473,  -473,  -473,  -473,     0,     0,     0,
       0,   -86,     0,     0,  -474,  -474,  -474,     0,  -474,     0,
       0,     0,  -474,  -474,  -473,  -473,  -473,  -474,     0,  -474,
    -474,  -474,  -474,  -474,  -474,  -474,  -473,     0,     0,     0,
    -474,  -474,  -474,  -474,  -474,  -474,  -474,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -474,     0,     0,  -474,
    -474,  -474,  -474,  -474,  -474,  -474,  -474,  -474,  -474,     0,
    -474,  -474,     0,  -474,  -474,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -474,   650,     0,  -474,  -474,     0,  -474,
    -474,     0,  -474,  -474,  -474,  -474,  -474,  -474,  -474,  -474,
       0,     0,     0,     0,   -88,     0,     0,  -250,  -250,  -250,
       0,  -250,     0,     0,     0,  -250,  -250,  -474,  -474,  -474,
    -250,     0,  -250,  -250,  -250,  -250,  -250,  -250,  -250,  -474,
       0,     0,     0,  -250,  -250,  -250,  -250,  -250,  -250,  -250,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -250,
       0,     0,  -250,  -250,  -250,  -250,  -250,  -250,  -250,  -250,
    -250,  -250,     0,  -250,  -250,     0,  -250,  -250,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -250,     0,     0,  -250,
    -250,     0,  -250,  -250,     0,  -250,  -250,  -250,  -250,  -250,
    -250,  -250,  -250,     0,     0,     0,     0,     0,     0,     0,
    -250,  -250,  -250,     0,  -250,     0,     0,     0,  -250,  -250,
    -250,  -250,  -250,  -250,     0,  -250,  -250,  -250,  -250,  -250,
    -250,  -250,   242,     0,     0,     0,  -250,  -250,  -250,  -250,
    -250,  -250,  -250,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -250,     0,     0,  -250,  -250,  -250,  -250,  -250,
    -250,  -250,  -250,  -250,  -250,     0,  -250,  -250,     0,  -250,
    -250,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -250,
       0,     0,  -250,  -250,     0,  -250,  -250,     0,  -250,  -250,
    -250,  -250,  -250,  -250,  -250,  -250,     0,     0,     0,     0,
       0,     0,     0,  -475,  -475,  -475,     0,  -475,     0,     0,
       0,  -475,  -475,  -250,  -250,  -250,  -475,     0,  -475,  -475,
    -475,  -475,  -475,  -475,  -475,   245,     0,     0,     0,  -475,
    -475,  -475,  -475,  -475,  -475,  -475,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -475,     0,     0,  -475,  -475,
    -475,  -475,  -475,  -475,  -475,  -475,  -475,  -475,     0,  -475,
    -475,     0,  -475,  -475,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -475,     0,     0,  -475,  -475,     0,  -475,  -475,
       0,  -475,  -475,  -475,  -475,  -475,  -475,  -475,  -475,     0,
       0,     0,     0,     0,     0,     0,  -476,  -476,  -476,     0,
    -476,     0,     0,     0,  -476,  -476,  -475,  -475,  -475,  -476,
       0,  -476,  -476,  -476,  -476,  -476,  -476,  -476,  -475,     0,
       0,     0,  -476,  -476,  -476,  -476,  -476,  -476,  -476,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -476,     0,
       0,  -476,  -476,  -476,  -476,  -476,  -476,  -476,  -476,  -476,
    -476,     0,  -476,  -476,     0,  -476,  -476,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -476,     0,     0,  -476,  -476,
       0,  -476,  -476,     0,  -476,  -476,  -476,  -476,  -476,  -476,
    -476,  -476,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -476,
    -476,  -476,   117,   118,   119,   120,   121,   122,   123,   124,
       0,  -476,   125,   126,   127,   128,   129,     0,     0,   130,
     131,   132,   133,   134,   135,   136,     0,     0,   137,   138,
     139,   193,   194,   195,   196,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   154,   155,   197,   198,   199,
     159,   280,   281,   200,   282,     0,     0,     0,     0,     0,
       0,   161,   162,   163,   164,   165,   166,     0,   167,   168,
       0,     0,   169,     0,     0,     0,   170,   171,   172,   173,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     174,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   175,   176,   177,   178,   179,
     180,   181,   182,   183,   184,     0,     0,   185,   117,   118,
     119,   120,   121,   122,   123,   124,   186,     0,   125,   126,
     127,   128,   129,     0,     0,   130,   131,   132,   133,   134,
     135,   136,     0,     0,   137,   138,   139,   193,   194,   195,
     196,   144,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   154,   155,   197,   198,   199,   159,   249,     0,   200,
       0,     0,     0,     0,     0,     0,     0,   161,   162,   163,
     164,   165,   166,     0,   167,   168,     0,     0,   169,     0,
       0,     0,   170,   171,   172,   173,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   174,     0,   201,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   175,   176,   177,   178,   179,   180,   181,   182,   183,
     184,     0,     0,   185,   117,   118,   119,   120,   121,   122,
     123,   124,   186,     0,   125,   126,   127,   128,   129,     0,
       0,   130,   131,   132,   133,   134,   135,   136,     0,     0,
     137,   138,   139,   193,   194,   195,   196,   144,   145,   146,
     147,   148,   149,   150,   151,   152,   153,   154,   155,   197,
     198,   199,   159,     0,     0,   200,     0,     0,     0,     0,
       0,     0,     0,   161,   162,   163,   164,   165,   166,     0,
     167,   168,     0,     0,   169,     0,     0,     0,   170,   171,
     172,   173,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   174,     0,   201,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,     0,     0,   185,
     117,   118,   119,   120,   121,   122,   123,   124,   186,     0,
     125,   126,   127,   128,   129,     0,     0,   130,   131,   132,
     133,   134,   135,   136,     0,     0,   137,   138,   139,   193,
     194,   195,   196,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,   197,   198,   199,   159,     0,
       0,   200,     0,     0,     0,     0,     0,     0,     0,   161,
     162,   163,   164,   165,   166,     0,   167,   168,     0,     0,
     169,     0,     0,     0,   170,   171,   172,   173,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   174,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   175,   176,   177,   178,   179,   180,   181,
     182,   183,   184,     0,     0,   185,     0,     5,     6,     7,
       0,     9,     0,     0,   186,    10,    11,     0,     0,     0,
      12,     0,    13,    14,    15,    97,    98,    18,    19,     0,
       0,     0,     0,    99,    21,    22,    23,    24,    25,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,     0,    42,    43,     0,    44,    45,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    46,     0,     0,   107,
      48,     0,    49,    50,     0,   229,   230,    52,    53,    54,
      55,    56,    57,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     5,     6,     7,     8,     9,
      58,   231,    60,    10,    11,     0,     0,     0,    12,   406,
      13,    14,    15,    16,    17,    18,    19,     0,     0,     0,
       0,    20,    21,    22,    23,    24,    25,    26,     0,     0,
      27,     0,     0,     0,     0,     0,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
       0,    42,    43,     0,    44,    45,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    46,     0,     0,    47,    48,     0,
      49,    50,     0,    51,     0,    52,    53,    54,    55,    56,
      57,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    58,    59,
      60,    12,     0,    13,    14,    15,    16,    17,    18,    19,
       0,     0,     0,     0,    20,    21,    22,    23,    24,    25,
      26,     0,     0,    27,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,     0,    42,    43,     0,    44,    45,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    46,     0,     0,
     107,    48,     0,    49,    50,     0,   605,   230,    52,    53,
      54,    55,    56,    57,     0,     0,     0,     0,     0,     0,
       0,     5,     6,     7,     0,     9,     0,     0,     0,    10,
      11,    58,    59,    60,    12,     0,    13,    14,    15,    97,
      98,    18,    19,     0,     0,     0,     0,    99,    21,    22,
      23,    24,    25,    26,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    29,     0,     0,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,     0,    42,    43,     0,
      44,    45,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      46,     0,     0,   107,    48,     0,    49,    50,     0,   229,
     230,    52,    53,    54,    55,    56,    57,     0,     0,     0,
       0,     0,     0,     0,     5,     6,     7,     0,     9,     0,
       0,     0,    10,    11,    58,   231,    60,    12,     0,    13,
      14,    15,    97,    98,    18,    19,     0,     0,     0,     0,
      99,    21,    22,    23,    24,    25,    26,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    29,     0,     0,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,     0,
      42,    43,     0,    44,    45,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    46,     0,     0,   107,   408,     0,    49,
      50,     0,   229,   230,    52,    53,    54,    55,    56,    57,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,    58,   231,    60,
      12,     0,    13,    14,    15,    97,    98,    18,    19,     0,
       0,     0,     0,    99,   100,   101,    23,    24,    25,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,     0,    42,    43,     0,    44,    45,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    46,     0,     0,   107,
      48,     0,    49,    50,     0,   597,   230,    52,    53,    54,
      55,    56,    57,     0,     0,     0,     0,     0,     0,     0,
       5,     6,     7,     0,     9,     0,     0,     0,    10,    11,
      58,   231,    60,    12,     0,    13,    14,    15,    97,    98,
      18,    19,     0,     0,     0,     0,    99,   100,   101,    23,
      24,    25,    26,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    29,     0,     0,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,     0,    42,    43,     0,    44,
      45,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    46,
       0,     0,   107,    48,     0,    49,    50,     0,   601,   230,
      52,    53,    54,    55,    56,    57,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    58,   231,    60,    12,     0,    13,    14,
      15,    97,    98,    18,    19,     0,     0,     0,     0,    99,
      21,    22,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,     0,    42,
      43,     0,    44,    45,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    46,     0,     0,   107,    48,     0,    49,    50,
       0,   597,   230,    52,    53,    54,    55,    56,    57,     0,
       0,     0,     0,     0,     0,     0,     5,     6,     7,     0,
       9,     0,     0,     0,    10,    11,    58,   231,    60,    12,
       0,    13,    14,    15,    97,    98,    18,    19,     0,     0,
       0,     0,    99,   100,   101,    23,    24,    25,    26,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    29,     0,
       0,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,     0,    42,    43,     0,    44,    45,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    46,     0,     0,   107,    48,
       0,    49,    50,     0,   790,   230,    52,    53,    54,    55,
      56,    57,     0,     0,     0,     0,     0,     0,     0,     5,
       6,     7,     0,     9,     0,     0,     0,    10,    11,    58,
     231,    60,    12,     0,    13,    14,    15,    97,    98,    18,
      19,     0,     0,     0,     0,    99,   100,   101,    23,    24,
      25,    26,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    29,     0,     0,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,     0,    42,    43,     0,    44,    45,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    46,     0,
       0,   107,    48,     0,    49,    50,     0,   793,   230,    52,
      53,    54,    55,    56,    57,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    58,   231,    60,    12,     0,    13,    14,    15,
      97,    98,    18,    19,     0,     0,     0,     0,    99,   100,
     101,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,     0,    42,    43,
       0,    44,    45,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    46,     0,     0,   107,    48,     0,    49,    50,     0,
     798,   230,    52,    53,    54,    55,    56,    57,     0,     0,
       0,     0,     0,     0,     0,     5,     6,     7,     0,     9,
       0,     0,     0,    10,    11,    58,   231,    60,    12,     0,
      13,    14,    15,    97,    98,    18,    19,     0,     0,     0,
       0,    99,   100,   101,    23,    24,    25,    26,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    29,     0,     0,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
       0,    42,    43,     0,    44,    45,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    46,     0,     0,   107,    48,     0,
      49,    50,     0,   856,   230,    52,    53,    54,    55,    56,
      57,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    58,   231,
      60,    12,     0,    13,    14,    15,    97,    98,    18,    19,
       0,     0,     0,     0,    99,   100,   101,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,     0,    42,    43,     0,    44,    45,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    46,     0,     0,
     107,    48,     0,    49,    50,     0,   858,   230,    52,    53,
      54,    55,    56,    57,     0,     0,     0,     0,     0,     0,
       0,     5,     6,     7,     0,     9,     0,     0,     0,    10,
      11,    58,   231,    60,    12,     0,    13,    14,    15,    97,
      98,    18,    19,     0,     0,     0,     0,    99,   100,   101,
      23,    24,    25,    26,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    29,     0,     0,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,     0,    42,    43,     0,
      44,    45,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      46,     0,     0,   107,    48,     0,    49,    50,     0,   877,
     230,    52,    53,    54,    55,    56,    57,     0,     0,     0,
       0,     0,     0,     0,     5,     6,     7,     0,     9,     0,
       0,     0,    10,    11,    58,   231,    60,    12,     0,    13,
      14,    15,    16,    17,    18,    19,     0,     0,     0,     0,
      20,    21,    22,    23,    24,    25,    26,     0,     0,    27,
       0,     0,     0,     0,     0,     0,    29,     0,     0,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,     0,
      42,    43,     0,    44,    45,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    46,     0,     0,   107,    48,     0,    49,
      50,     0,     0,     0,    52,    53,    54,    55,    56,    57,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,    58,    59,    60,
      12,     0,    13,    14,    15,    97,    98,    18,    19,     0,
       0,     0,     0,    99,    21,    22,    23,    24,    25,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,     0,    42,    43,     0,    44,    45,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    46,     0,     0,   107,
      48,     0,    49,    50,     0,   266,     0,    52,    53,    54,
      55,    56,    57,     0,     0,     0,     0,     0,     0,     0,
       5,     6,     7,     0,     9,     0,     0,     0,    10,    11,
      58,   231,    60,    12,     0,    13,    14,    15,    16,    17,
      18,    19,     0,     0,     0,     0,    20,    21,    22,    23,
      24,    25,    26,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    29,     0,     0,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,     0,    42,    43,     0,    44,
      45,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    46,
       0,     0,   107,    48,     0,    49,    50,     0,   472,     0,
      52,    53,    54,    55,    56,    57,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    58,   231,    60,    12,     0,    13,    14,
      15,    97,    98,    18,    19,     0,     0,     0,     0,    99,
     100,   101,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,     0,    42,
      43,     0,    44,    45,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    46,     0,     0,   107,    48,     0,    49,    50,
       0,   579,     0,    52,    53,    54,    55,    56,    57,     0,
       0,     0,     0,     0,     0,     0,     5,     6,     7,     0,
       9,     0,     0,     0,    10,    11,    58,   231,    60,    12,
       0,    13,    14,    15,    97,    98,    18,    19,     0,     0,
       0,     0,    99,   100,   101,    23,    24,    25,    26,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    29,     0,
       0,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,     0,    42,    43,     0,    44,    45,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    46,     0,     0,   107,    48,
       0,    49,    50,     0,   626,     0,    52,    53,    54,    55,
      56,    57,     0,     0,     0,     0,     0,     0,     0,     5,
       6,     7,     0,     9,     0,     0,     0,    10,    11,    58,
     231,    60,    12,     0,    13,    14,    15,    97,    98,    18,
      19,     0,     0,     0,     0,    99,   100,   101,    23,    24,
      25,    26,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    29,     0,     0,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,     0,    42,    43,     0,    44,    45,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    46,     0,
       0,   107,    48,     0,    49,    50,     0,   740,     0,    52,
      53,    54,    55,    56,    57,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    58,   231,    60,    12,     0,    13,    14,    15,
      97,    98,    18,    19,     0,     0,     0,     0,    99,   100,
     101,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,     0,    42,    43,
       0,    44,    45,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    46,     0,     0,   107,    48,     0,    49,    50,     0,
     784,     0,    52,    53,    54,    55,    56,    57,     0,     0,
       0,     0,     0,     0,     0,     5,     6,     7,     0,     9,
       0,     0,     0,    10,    11,    58,   231,    60,    12,     0,
      13,    14,    15,    97,    98,    18,    19,     0,     0,     0,
       0,    99,   100,   101,    23,    24,    25,    26,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    29,     0,     0,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
       0,    42,    43,     0,    44,    45,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    46,     0,     0,   107,    48,     0,
      49,    50,     0,     0,     0,    52,    53,    54,    55,    56,
      57,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    58,   231,
      60,    12,     0,    13,    14,    15,    16,    17,    18,    19,
       0,     0,     0,     0,    20,    21,    22,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,     0,    42,    43,     0,    44,    45,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    46,     0,     0,
     107,    48,     0,    49,    50,     0,     0,     0,    52,    53,
      54,    55,    56,    57,     0,     0,     0,     0,     0,     0,
       0,     5,     6,     7,     0,     9,     0,     0,     0,    10,
      11,    58,   231,    60,    12,     0,    13,    14,    15,    97,
      98,    18,    19,     0,     0,     0,     0,    99,   100,   101,
      23,    24,    25,    26,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   102,     0,     0,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,     0,    42,    43,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      46,     0,     0,    47,    48,     0,    49,    50,     0,    51,
       0,    52,    53,    54,    55,    56,    57,     0,     0,     0,
       5,     6,     7,     0,     9,     0,   737,     0,    10,    11,
       0,     0,     0,    12,   108,    13,    14,    15,    97,    98,
      18,    19,     0,     0,     0,     0,    99,   100,   101,    23,
      24,    25,    26,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   102,     0,     0,    32,    33,   103,    35,    36,
      37,   104,    39,    40,    41,     0,    42,    43,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   105,     0,     0,   106,
       0,     0,   107,    48,     0,    49,    50,     0,     0,     0,
      52,    53,    54,    55,    56,    57,     0,     0,     0,     5,
       6,     7,     0,     9,     0,     0,     0,    10,    11,     0,
       0,     0,    12,   108,    13,    14,    15,    97,    98,    18,
      19,     0,     0,     0,     0,    99,   100,   101,    23,    24,
      25,    26,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   102,     0,     0,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,     0,    42,    43,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    46,     0,
       0,    47,    48,     0,    49,    50,     0,    51,     0,    52,
      53,    54,    55,    56,    57,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,     0,     0,
       0,    12,   108,    13,    14,    15,    97,    98,    18,    19,
       0,     0,     0,     0,    99,   100,   101,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     102,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,     0,    42,    43,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    46,     0,     0,
     311,    48,     0,    49,    50,     0,   312,     0,    52,    53,
      54,    55,    56,    57,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,     0,     0,     0,
      12,   108,    13,    14,    15,    97,    98,    18,    19,     0,
       0,     0,     0,    99,   100,   101,    23,    24,    25,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   102,
       0,     0,    32,    33,   103,    35,    36,    37,   104,    39,
      40,    41,     0,    42,    43,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   106,     0,     0,   107,
      48,     0,    49,    50,     0,     0,     0,    52,    53,    54,
      55,    56,    57,     0,     0,     0,     5,     6,     7,     0,
       9,     0,     0,     0,    10,    11,     0,     0,     0,    12,
     108,    13,    14,    15,    97,    98,    18,    19,     0,     0,
       0,     0,    99,   100,   101,    23,    24,    25,    26,     0,
       0,     0,     0,   647,     0,     0,     0,     0,   102,     0,
       0,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,     0,    42,    43,   315,   316,   317,   318,   319,   320,
     321,   322,   323,   324,   325,   326,   327,     0,     0,   328,
     329,     0,     0,     0,     0,    46,     0,     0,   107,    48,
       0,    49,    50,     0,     0,     0,    52,    53,    54,    55,
      56,    57,     0,     0,   330,     0,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   517,   518,     0,   108,
     519,     0,     0,     0,     0,     0,  -216,     0,   161,   162,
     163,   164,   165,   166,     0,   167,   168,     0,     0,   169,
       0,     0,     0,   170,   171,   172,   173,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   174,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   522,   518,   185,     0,   523,     0,     0,     0,
       0,     0,     0,   186,   161,   162,   163,   164,   165,   166,
       0,   167,   168,     0,     0,   169,     0,     0,     0,   170,
     171,   172,   173,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   174,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   175,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   538,   511,
     185,     0,   539,     0,     0,     0,     0,     0,     0,   186,
     161,   162,   163,   164,   165,   166,     0,   167,   168,     0,
       0,   169,     0,     0,     0,   170,   171,   172,   173,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   174,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   571,   511,   185,     0,   572,     0,
       0,     0,     0,     0,     0,   186,   161,   162,   163,   164,
     165,   166,     0,   167,   168,     0,     0,   169,     0,     0,
       0,   170,   171,   172,   173,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   174,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     574,   518,   185,     0,   575,     0,     0,     0,     0,     0,
       0,   186,   161,   162,   163,   164,   165,   166,     0,   167,
     168,     0,     0,   169,     0,     0,     0,   170,   171,   172,
     173,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   174,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   588,   511,   185,     0,
     589,     0,     0,     0,     0,     0,     0,   186,   161,   162,
     163,   164,   165,   166,     0,   167,   168,     0,     0,   169,
       0,     0,     0,   170,   171,   172,   173,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   174,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   591,   518,   185,     0,   592,     0,     0,     0,
       0,     0,     0,   186,   161,   162,   163,   164,   165,   166,
       0,   167,   168,     0,     0,   169,     0,     0,     0,   170,
     171,   172,   173,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   174,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   175,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   628,   511,
     185,     0,   629,     0,     0,     0,     0,     0,     0,   186,
     161,   162,   163,   164,   165,   166,     0,   167,   168,     0,
       0,   169,     0,     0,     0,   170,   171,   172,   173,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   174,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   631,   518,   185,     0,   632,     0,
       0,     0,     0,     0,     0,   186,   161,   162,   163,   164,
     165,   166,     0,   167,   168,     0,     0,   169,     0,     0,
       0,   170,   171,   172,   173,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   174,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     863,   511,   185,     0,   864,     0,     0,     0,     0,     0,
       0,   186,   161,   162,   163,   164,   165,   166,     0,   167,
     168,     0,     0,   169,     0,     0,     0,   170,   171,   172,
     173,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   174,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   866,   518,   185,     0,
     867,     0,     0,     0,     0,     0,     0,   186,   161,   162,
     163,   164,   165,   166,     0,   167,   168,     0,     0,   169,
       0,     0,     0,   170,   171,   172,   173,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   174,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,     0,     0,   185,     0,     0,     0,     0,     0,
       0,     0,     0,   186,   315,   316,   317,   318,   319,   320,
     321,   322,   323,   324,   325,   326,   327,     0,     0,   328,
     329,     0,     0,     0,  -216,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,     0,     0,
     328,   329,     0,     0,   330,     0,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   330,  -216,   331,   332,   333,
     334,   335,   336,   337,   338,   339,   340,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   594,   315,   316,
     317,   318,   319,   320,   321,   322,   323,   324,   325,   326,
     327,     0,     0,   328,   329,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,  -494,  -494,     0,     0,
     328,   329,     0,     0,     0,     0,     0,     0,   330,     0,
     331,   332,   333,   334,   335,   336,   337,   338,   339,   340,
       0,     0,     0,     0,     0,     0,     0,   331,   332,   333,
     334,   335,   336,   337,   338,   339,   340,   315,  -494,  -494,
    -494,  -494,   320,   321,     0,     0,  -494,  -494,     0,     0,
       0,     0,   328,   329,   315,   316,   317,   318,   319,   320,
     321,     0,     0,   324,   325,     0,     0,     0,     0,   328,
     329,     0,     0,     0,     0,     0,     0,     0,     0,   331,
     332,   333,   334,   335,   336,   337,   338,   339,   340,     0,
       0,     0,     0,     0,     0,     0,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340
};

static const short yycheck[] =
{
      16,    17,    15,    59,    20,   214,    47,   245,    16,    17,
      28,     7,    20,    22,    46,    16,    17,     7,     8,    20,
       5,     6,    62,   369,   393,   387,    51,    22,    44,    45,
      15,   404,   679,    49,    50,   682,   342,   682,    28,   524,
     525,    13,    50,    59,    60,    16,    17,     8,    27,    20,
     559,   560,    10,    11,    12,   261,    52,    16,    17,   365,
     286,    20,    52,   436,   290,   367,    51,    28,     1,    27,
       4,    25,    58,    12,   106,    13,   382,   566,    49,   341,
      64,   343,   344,   345,    50,    94,    71,   576,    54,   395,
      48,    50,   101,    81,    26,    64,   126,     0,    58,    94,
     130,    26,   526,   527,   366,   311,   115,    81,    36,    37,
      79,    80,    56,    57,    13,   101,    25,    25,    81,   381,
      84,   383,   120,    62,    72,   130,   105,   111,   112,   113,
     114,   115,   102,   105,   396,    17,   124,   443,    20,    99,
     100,   110,   111,   112,   113,   114,   115,   105,    11,    12,
     124,   130,   131,    26,   108,   128,   128,    28,   130,   131,
     108,   124,   126,   425,   652,    25,   813,   105,   813,   129,
     658,   110,   126,    13,   107,   126,     2,   217,     4,     5,
       6,    81,   444,   126,    10,    11,    12,   130,   120,    15,
      16,    17,   130,   131,    20,   120,   128,   130,   131,   108,
     108,    27,   130,    36,    37,   201,   105,   553,    81,   129,
      15,   201,    17,   229,   230,   231,    81,   126,   126,   128,
     261,    47,    48,    49,   124,    51,   242,   129,   244,   245,
     456,   130,   131,    59,   242,   537,   244,   245,    94,   724,
     725,   242,   666,   244,   245,    71,    81,   120,   108,   122,
     266,   124,   554,   309,   103,   128,   765,   766,   314,   124,
      83,   770,    83,   221,   222,   105,   126,   103,   128,    52,
     311,   242,    83,   244,   245,   128,    59,    60,   217,   105,
     103,   107,   103,   242,   126,   244,   245,   312,    50,   124,
     130,   131,   125,   309,   350,   351,   785,   267,   314,   315,
     316,   317,   318,   319,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   335,
     336,   337,   338,   339,   340,    25,   342,   312,   103,    91,
      92,    25,   364,    81,   350,   351,   126,   846,    83,   712,
     130,   710,   300,   301,   302,   303,   719,   305,   306,   365,
     309,   341,   614,   615,   131,   314,    81,    83,   221,   222,
     242,    25,   244,   245,   380,   299,   382,    83,   384,   385,
     304,   342,   581,   389,   364,   613,   124,   103,    84,   395,
     206,    25,   130,    25,   400,   401,    25,   103,   404,   126,
      81,   381,   408,   401,   365,   221,   222,   122,    64,   124,
     408,   419,    26,   128,   394,   421,   126,   408,   108,   103,
     368,   382,    26,    15,   108,    17,   242,   433,   244,   245,
     436,   360,   438,   785,   395,   771,   126,   443,   254,   419,
     389,   125,   126,   124,   424,   261,   128,   300,   301,   302,
     303,   120,   412,    64,   414,   461,   462,   113,   114,   115,
     408,    58,   442,   392,   393,   757,   472,    81,   419,   103,
     430,   130,   131,   836,   108,   125,   108,    81,   125,   108,
      58,   125,   443,   299,   300,   301,   302,   303,   304,   305,
     306,   125,   126,   309,   126,   311,   312,   126,   314,    83,
      81,   238,    99,   100,   101,    58,   120,    83,   122,   872,
     124,    56,    57,    81,   128,   514,   120,    83,   122,   103,
     124,    99,   100,   101,   128,   817,   342,   103,   270,   514,
     272,   101,   274,   123,   350,   351,   521,   103,   121,   524,
     525,   122,   541,   124,    36,    37,    99,   100,   101,   365,
     126,   367,   368,   849,   122,   563,   124,    83,    83,   565,
     126,    81,   378,    14,    15,   101,   382,    81,    81,    64,
     386,    64,   388,   579,    64,   774,   562,   103,   103,   395,
     126,   780,   562,   563,    90,   545,    10,    58,   536,    79,
      80,   597,   408,   125,   126,   601,     8,    81,   850,   605,
     126,   126,   122,   649,   124,   534,   422,   613,   654,   655,
     124,   124,   563,    81,    81,   613,   565,    81,    54,    55,
     626,   111,   112,   113,   114,   115,   555,   443,    99,   100,
     101,    50,    13,    52,    53,    54,    55,    81,   641,   645,
     124,   647,   648,   649,   650,   593,    81,    81,   654,   655,
      17,    10,   613,   126,   122,   122,   124,   124,   122,    58,
     124,   398,    61,   125,    81,    83,   641,   673,   126,   125,
     125,   619,    91,    92,    81,   121,   126,   125,   122,   126,
     124,   123,    64,   536,   129,   103,   126,   122,   122,   124,
     124,   697,   125,   699,   700,   123,    10,    79,    80,   705,
      99,   100,   748,   103,    50,   122,   712,   124,   126,   128,
     716,    50,   126,   719,    15,   122,   676,   124,   716,    81,
     536,   537,   126,    49,    50,   716,   108,   109,   110,   111,
     112,   113,   114,   115,   740,   126,   123,    10,   554,   668,
     593,   126,   748,     2,    10,     4,   694,    52,    53,   123,
      55,    10,    11,    12,    59,    60,    92,    16,    17,   125,
     122,    20,   124,   723,   121,   126,   772,   716,    27,   729,
      39,    40,    41,    42,    43,   126,   782,   593,   784,   123,
     125,   710,   125,    10,   790,   125,    10,   793,    47,    48,
      49,   126,   798,    92,    84,     9,   125,   613,   773,    10,
      59,    10,   108,   619,   126,   125,    10,   126,    10,   126,
     126,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,   121,   641,    79,    80,   834,   125,
     836,    10,    10,   649,   121,    10,   834,   121,   654,   655,
      10,   694,    10,   849,   126,   123,   105,     6,   107,    71,
     856,   104,   858,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   600,   641,   314,   872,   851,   848,   852,
      78,   877,     7,   553,   611,   691,    81,   130,   694,    50,
     771,    52,    53,    54,    55,   679,   679,    -1,   849,    62,
     706,   707,   708,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   237,   238,    -1,    -1,   731,    -1,    -1,    -1,   245,
      91,    92,    -1,   739,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   748,    -1,    -1,    -1,    -1,    -1,   754,   755,
      -1,   757,    -1,    -1,    -1,    -1,    -1,   206,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   773,    -1,    -1,
      -1,   777,   221,   222,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   711,    -1,    -1,   714,   715,    -1,
      -1,    -1,    -1,   242,    -1,   244,   245,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   810,   254,    -1,    -1,    -1,    -1,
      -1,   817,   261,    -1,    -1,    -1,   822,    -1,   824,    -1,
      -1,    -1,    -1,    -1,    -1,   831,   342,     2,    -1,     4,
       5,     6,     7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      15,    -1,    -1,   849,    -1,    -1,    -1,    -1,    -1,   365,
     299,   300,   301,   302,   303,   304,   305,   306,    -1,   375,
     309,    -1,   311,    -1,    -1,   314,   382,    -1,   795,   796,
      -1,    -1,    47,    -1,    -1,    -1,    51,    -1,    -1,   395,
      -1,    -1,   398,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   342,    -1,    -1,    71,   413,    -1,    -1,
      -1,   350,   351,    -1,    -1,   832,   833,    -1,    -1,    -1,
      -1,   838,    -1,    -1,    -1,    -1,   365,    -1,   367,   368,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   443,    -1,   378,
     857,    -1,   107,   382,    -1,    -1,    -1,   386,    -1,   388,
      -1,    -1,    -1,    -1,   871,    -1,   395,   874,    -1,    -1,
      -1,    -1,   879,    -1,    -1,   471,    -1,    -1,    -1,   408,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   422,    -1,    -1,     0,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     8,     9,    10,    -1,    -1,    13,
      14,    15,    -1,    17,   443,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    27,    -1,   521,    -1,    -1,   524,   525,
     526,   527,    36,    37,    -1,    39,    40,    41,    42,    43,
      -1,    -1,     2,    -1,     4,     5,     6,   543,    -1,    -1,
      -1,   206,    -1,     2,    -1,    15,    -1,    -1,    -1,    -1,
      -1,   557,    -1,   559,   560,    -1,    -1,    -1,    -1,   565,
     566,     2,   568,     4,    -1,    -1,    -1,    81,    -1,    -1,
     576,    -1,    -1,    -1,    15,    -1,    -1,    47,    -1,    -1,
      -1,    51,    -1,    -1,    -1,    -1,    -1,    -1,    47,   254,
      -1,   105,    -1,    -1,   600,    -1,   261,   536,   537,    -1,
      -1,    71,    -1,    -1,    -1,   611,    47,   121,    -1,    -1,
     124,   125,    -1,    -1,   128,   554,   130,   131,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   299,    -1,    -1,   107,    -1,   304,
      -1,    -1,    -1,    -1,    -1,    -1,   311,   312,   107,    -1,
      -1,    -1,    -1,    -1,   593,    -1,    -1,    -1,    -1,    -1,
     666,    -1,    -1,    -1,    -1,    -1,   107,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   613,    -1,    -1,    -1,    -1,    -1,
     619,    -1,    -1,   689,    -1,    -1,   692,    -1,    -1,     2,
      -1,     4,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   367,    -1,    -1,   711,    -1,    -1,   714,   715,
     649,    -1,    -1,   378,    -1,   654,   655,    -1,   724,   725,
      -1,   386,    -1,   388,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    47,    -1,   206,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   691,    -1,    -1,   694,    -1,   422,    -1,   765,
     766,    -1,    -1,    -1,   770,   206,    -1,   706,   707,   708,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   785,
      -1,    -1,    -1,    -1,   254,    -1,    -1,    -1,    -1,   795,
     796,   261,   731,    -1,   107,   254,    -1,    -1,    -1,    -1,
     739,    -1,   261,    -1,    -1,    -1,    -1,    -1,    -1,   748,
      -1,    -1,    -1,   254,    -1,   754,   755,    -1,   757,    -1,
     261,    -1,    -1,    -1,    -1,    -1,   832,   833,    -1,   299,
      -1,    -1,   838,    -1,   304,    -1,    -1,    -1,   777,    -1,
     846,   311,   312,   849,    -1,   851,   852,    -1,    -1,    -1,
      -1,   857,   311,    -1,    -1,    -1,    -1,    -1,   299,    -1,
      -1,    -1,    -1,   304,    -1,   871,    -1,    -1,   874,    -1,
     311,   810,   537,   879,    -1,    -1,    -1,    -1,   817,    -1,
      -1,    -1,    -1,   822,    -1,   824,    -1,    -1,    -1,   554,
      -1,    -1,   831,   206,    -1,    -1,    -1,   367,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   378,    -1,
     849,    -1,    -1,    -1,    -1,    -1,   386,    -1,   388,   378,
      -1,    -1,    -1,    -1,    -1,    -1,   367,   386,    -1,   388,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   378,    -1,    -1,
      -1,   254,    -1,    -1,    -1,   386,    -1,   388,   261,    -1,
      -1,    -1,   422,    -1,    -1,    50,    51,    -1,    -1,    54,
      -1,    -1,    -1,   422,    -1,    -1,    -1,    62,    63,    64,
      65,    66,    67,    -1,    69,    70,   641,    -1,    73,    -1,
      -1,   422,    77,    78,    79,    80,   299,    -1,    -1,    -1,
      -1,   304,    -1,    -1,    -1,    -1,    91,    -1,   311,    -1,
      -1,    -1,    -1,   229,   230,    -1,    -1,    -1,    -1,    -1,
      -1,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,    -1,    -1,   118,    -1,    -1,   691,    -1,    -1,    -1,
      -1,    -1,   127,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   706,   707,   708,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   367,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   378,   731,   537,    -1,    -1,
      -1,    -1,    -1,   386,   739,   388,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   309,   554,    -1,    -1,    -1,   314,   754,
     755,    -1,   757,    -1,    -1,    -1,   537,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   773,   422,
      -1,    -1,   777,   554,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   810,    -1,    -1,    -1,    -1,
      -1,    -1,   817,    -1,    -1,    -1,    -1,   822,    -1,   824,
      -1,    -1,    -1,   389,   634,    -1,   831,    -1,    -1,    -1,
      -1,   641,    -1,    -1,   400,   401,    -1,    -1,   404,    -1,
      -1,    -1,   408,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     641,    -1,    -1,    -1,    -1,    -1,    -1,   433,    -1,    -1,
     436,    -1,   438,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   691,    -1,    -1,   537,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   691,    -1,    -1,    -1,   706,   707,   708,    -1,
      -1,   554,    -1,    -1,    -1,    -1,   472,   706,   707,   708,
     691,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   731,    -1,    -1,    -1,   706,   707,   708,    -1,   739,
      -1,    -1,   731,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     739,    -1,    -1,    -1,   754,   755,    -1,   757,    -1,    -1,
     731,    -1,    -1,    -1,    -1,   754,   755,    -1,   739,    -1,
      -1,    -1,    -1,   773,    -1,    -1,    -1,   777,    -1,    -1,
      -1,    -1,    -1,   754,   755,    -1,   757,    -1,   777,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   773,    -1,    -1,    -1,   777,    -1,    -1,   565,
     810,    -1,    -1,    -1,    -1,    -1,    -1,   817,    -1,    -1,
      -1,   810,   822,   579,   824,    -1,    -1,    -1,    -1,    -1,
      -1,   831,    -1,   822,    -1,   824,    -1,    -1,    -1,   810,
      -1,   597,   831,    -1,    -1,   601,   817,    -1,   691,   605,
      -1,   822,    -1,   824,    -1,    -1,    -1,   613,    -1,    -1,
     831,    -1,    -1,   706,   707,   708,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    64,    65,    66,    67,
      68,    69,    70,    71,    -1,    73,    74,    -1,   731,   645,
      -1,    79,    80,    -1,    -1,    -1,   739,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   754,   755,    -1,   757,    -1,    -1,   673,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,    -1,    -1,
      -1,    -1,    -1,    -1,   777,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   705,
      -1,    -1,    -1,    -1,    -1,    -1,   712,    -1,    -1,    -1,
      -1,    -1,    -1,   719,    -1,    -1,    -1,   810,    -1,    -1,
      -1,    -1,    -1,    -1,   817,    -1,    -1,    -1,    -1,   822,
      -1,   824,    -1,    -1,   740,    -1,    -1,    -1,   831,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     0,     1,    -1,     3,     4,     5,     6,     7,    -1,
      -1,    -1,    11,    12,    -1,    -1,   772,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,   784,    -1,
      29,    30,    31,    32,    33,    34,    35,   793,    -1,    38,
      -1,    -1,   798,    -1,    -1,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    -1,
      59,    60,    -1,    62,    63,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   834,    -1,
     836,    -1,    -1,    82,    -1,    -1,    85,    86,    -1,    88,
      89,    -1,    91,    -1,    93,    94,    95,    96,    97,    98,
     856,    -1,   858,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   872,   116,   117,   118,
       0,   877,    -1,    -1,    -1,    -1,    -1,    -1,     8,     9,
      10,   130,   131,    -1,    14,    15,    -1,    17,    -1,    -1,
      -1,    -1,    -1,    43,    -1,    -1,    26,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    36,    37,    -1,    39,
      40,    41,    42,    43,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    -1,    -1,    79,
      80,    -1,    -1,    -1,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    -1,    -1,    79,
      80,    81,    -1,    83,   104,    -1,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   103,   104,    -1,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,    -1,    -1,    -1,     0,
     120,   121,   122,    -1,   124,   125,   126,     8,     9,    10,
     130,   131,    -1,    14,    15,    -1,    17,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    36,    37,    -1,    39,    40,
      41,    42,    43,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    -1,    -1,    79,    80,
      -1,    -1,    -1,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    -1,    -1,    79,    80,
      81,    -1,    83,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   103,   104,    -1,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,    -1,    -1,    -1,    -1,    -1,
     121,   122,    -1,   124,   125,   126,    -1,    -1,    -1,   130,
     131,     1,    -1,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    -1,    -1,    15,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    38,    -1,
      -1,    -1,    -1,    -1,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    -1,    59,
      60,    -1,    62,    63,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    82,    -1,    -1,    85,    86,    -1,    88,    89,
      -1,    91,    -1,    93,    94,    95,    96,    97,    98,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   116,   117,   118,    -1,
      -1,    -1,    -1,     1,    -1,     3,     4,     5,     6,     7,
     130,   131,    10,    11,    12,    -1,    14,    15,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
      38,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      -1,    59,    60,    -1,    62,    63,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    82,    -1,    -1,    85,    86,    -1,
      88,    89,    -1,    91,    -1,    93,    94,    95,    96,    97,
      98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   116,   117,
     118,    -1,    -1,    -1,    -1,     1,    -1,     3,     4,     5,
       6,     7,   130,   131,    10,    11,    12,    -1,    -1,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    -1,    59,    60,    -1,    62,    63,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    82,    -1,    -1,    85,
      86,    -1,    88,    89,    -1,    91,    -1,    93,    94,    95,
      96,    97,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     116,   117,   118,    -1,    -1,    -1,    -1,     1,    -1,     3,
       4,     5,     6,     7,   130,   131,    10,    11,    12,    -1,
      -1,    15,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    -1,    59,    60,    -1,    62,    63,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,    -1,
      -1,    85,    86,    -1,    88,    89,    -1,    91,    -1,    93,
      94,    95,    96,    97,    98,    -1,    -1,    -1,    -1,    -1,
       1,    -1,     3,     4,     5,     6,     7,    -1,     9,    10,
      11,    12,   116,   117,   118,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,   130,   131,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    38,    -1,    -1,
      -1,    -1,    -1,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    59,    60,
      -1,    62,    63,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    82,    -1,    -1,    85,    86,    -1,    88,    89,    -1,
      91,    -1,    93,    94,    95,    96,    97,    98,    -1,    -1,
      -1,    -1,    -1,     1,    -1,     3,     4,     5,     6,     7,
      -1,    -1,    -1,    11,    12,   116,   117,   118,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,   130,
     131,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
      38,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      -1,    59,    60,    -1,    62,    63,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    82,    -1,    -1,    85,    86,    -1,
      88,    89,    -1,    91,    -1,    93,    94,    95,    96,    97,
      98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   116,   117,
     118,    -1,    -1,    -1,    -1,    -1,    -1,   125,    -1,    -1,
      -1,    -1,   130,   131,     1,    -1,     3,     4,     5,     6,
       7,    -1,    -1,    -1,    11,    12,    -1,    -1,    -1,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    59,    60,    -1,    62,    63,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    82,    -1,    -1,    85,    86,
      -1,    88,    89,    -1,    91,    -1,    93,    94,    95,    96,
      97,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   116,
     117,   118,    -1,    -1,    -1,    -1,    -1,    -1,   125,    -1,
      -1,    -1,    -1,   130,   131,     1,    -1,     3,     4,     5,
       6,     7,    -1,    -1,    -1,    11,    12,    -1,    -1,    -1,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    -1,    59,    60,    -1,    62,    63,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    82,    -1,    -1,    85,
      86,    -1,    88,    89,    -1,    91,    -1,    93,    94,    95,
      96,    97,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     116,   117,   118,    -1,    -1,   121,    -1,     1,    -1,     3,
       4,     5,     6,     7,   130,   131,    10,    11,    12,    -1,
      -1,    -1,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    -1,    59,    60,    -1,    62,    63,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,    -1,
      -1,    85,    86,    -1,    88,    89,    -1,    91,    -1,    93,
      94,    95,    96,    97,    98,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,     7,    -1,    -1,    -1,
      11,    12,   116,   117,   118,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,   130,   131,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    38,    -1,    -1,
      -1,    -1,    -1,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    59,    60,
      -1,    62,    63,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    82,    -1,    -1,    85,    86,    -1,    88,    89,    -1,
      91,    -1,    93,    94,    95,    96,    97,    98,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,
      -1,    -1,    -1,    11,    12,   116,   117,   118,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
     131,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
      38,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      -1,    59,    60,    -1,    62,    63,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    82,    -1,    -1,    85,    86,    -1,
      88,    89,    -1,    -1,    -1,    93,    94,    95,    96,    97,
      98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   116,   117,
     118,    -1,    -1,    -1,     3,     4,     5,     6,     7,     8,
       9,    10,   130,   131,    13,    14,    15,    16,    17,    -1,
      -1,    20,    21,    22,    23,    24,    25,    26,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    -1,    -1,    -1,
      -1,    -1,    -1,    62,    63,    64,    65,    66,    67,    -1,
      69,    70,    -1,    -1,    73,    -1,    -1,    -1,    77,    78,
      79,    80,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    91,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,    -1,    -1,   118,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,   127,   128,
      11,    12,    -1,    -1,    -1,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    26,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    59,    60,
      -1,    62,    63,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    82,    -1,    -1,    85,    86,    -1,    88,    89,    -1,
      91,    92,    93,    94,    95,    96,    97,    98,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,
      -1,    -1,    -1,    11,    12,   116,   117,   118,    16,   120,
      18,    19,    20,    21,    22,    23,    24,   128,    26,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      -1,    59,    60,    -1,    62,    63,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    82,    -1,    -1,    85,    86,    -1,
      88,    89,    -1,    91,    92,    93,    94,    95,    96,    97,
      98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   116,   117,
     118,    16,   120,    18,    19,    20,    21,    22,    23,    24,
     128,    26,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    -1,    59,    60,    -1,    62,    63,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,    -1,    -1,
      85,    86,    -1,    88,    89,    -1,    91,    92,    93,    94,
      95,    96,    97,    98,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
      12,   116,   117,   118,    16,   120,    18,    19,    20,    21,
      22,    23,    24,   128,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    -1,    59,    60,    -1,
      62,    63,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      82,    83,    -1,    85,    86,    -1,    88,    89,    -1,    91,
      92,    93,    94,    95,    96,    97,    98,    -1,    -1,    -1,
      -1,   103,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
      -1,    -1,    11,    12,   116,   117,   118,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,   128,    -1,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    -1,
      59,    60,    -1,    62,    63,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    82,    83,    -1,    85,    86,    -1,    88,
      89,    -1,    91,    92,    93,    94,    95,    96,    97,    98,
      -1,    -1,    -1,    -1,   103,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   116,   117,   118,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,   128,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    -1,    59,    60,    -1,    62,    63,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    82,    -1,    -1,    85,
      86,    -1,    88,    89,    -1,    91,    92,    93,    94,    95,
      96,    97,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
     116,   117,   118,    16,    -1,    18,    19,    20,    21,    22,
      23,    24,   128,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    -1,    59,    60,    -1,    62,
      63,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,
      -1,    -1,    85,    86,    -1,    88,    89,    -1,    91,    92,
      93,    94,    95,    96,    97,    98,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   116,   117,   118,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,   128,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    -1,    59,
      60,    -1,    62,    63,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    82,    -1,    -1,    85,    86,    -1,    88,    89,
      -1,    91,    92,    93,    94,    95,    96,    97,    98,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,   116,   117,   118,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,   128,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    59,    60,    -1,    62,    63,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    82,    -1,    -1,    85,    86,
      -1,    88,    89,    -1,    91,    92,    93,    94,    95,    96,
      97,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   116,
     117,   118,     3,     4,     5,     6,     7,     8,     9,    10,
      -1,   128,    13,    14,    15,    16,    17,    -1,    -1,    20,
      21,    22,    23,    24,    25,    26,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    -1,    -1,    -1,
      -1,    62,    63,    64,    65,    66,    67,    -1,    69,    70,
      -1,    -1,    73,    -1,    -1,    -1,    77,    78,    79,    80,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      91,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,    -1,    -1,   118,     3,     4,
       5,     6,     7,     8,     9,    10,   127,    -1,    13,    14,
      15,    16,    17,    -1,    -1,    20,    21,    22,    23,    24,
      25,    26,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    -1,    54,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    62,    63,    64,
      65,    66,    67,    -1,    69,    70,    -1,    -1,    73,    -1,
      -1,    -1,    77,    78,    79,    80,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    91,    -1,    93,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,    -1,    -1,   118,     3,     4,     5,     6,     7,     8,
       9,    10,   127,    -1,    13,    14,    15,    16,    17,    -1,
      -1,    20,    21,    22,    23,    24,    25,    26,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    -1,    -1,    54,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    62,    63,    64,    65,    66,    67,    -1,
      69,    70,    -1,    -1,    73,    -1,    -1,    -1,    77,    78,
      79,    80,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    91,    -1,    93,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,    -1,    -1,   118,
       3,     4,     5,     6,     7,     8,     9,    10,   127,    -1,
      13,    14,    15,    16,    17,    -1,    -1,    20,    21,    22,
      23,    24,    25,    26,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    -1,
      -1,    54,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    62,
      63,    64,    65,    66,    67,    -1,    69,    70,    -1,    -1,
      73,    -1,    -1,    -1,    77,    78,    79,    80,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    91,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,    -1,    -1,   118,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,   127,    11,    12,    -1,    -1,    -1,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    -1,    59,    60,    -1,    62,    63,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    82,    -1,    -1,    85,
      86,    -1,    88,    89,    -1,    91,    92,    93,    94,    95,
      96,    97,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,
     116,   117,   118,    11,    12,    -1,    -1,    -1,    16,   125,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
      38,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      -1,    59,    60,    -1,    62,    63,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    82,    -1,    -1,    85,    86,    -1,
      88,    89,    -1,    91,    -1,    93,    94,    95,    96,    97,
      98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   116,   117,
     118,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    -1,    59,    60,    -1,    62,    63,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,    -1,    -1,
      85,    86,    -1,    88,    89,    -1,    91,    92,    93,    94,
      95,    96,    97,    98,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
      12,   116,   117,   118,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    -1,    59,    60,    -1,
      62,    63,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      82,    -1,    -1,    85,    86,    -1,    88,    89,    -1,    91,
      92,    93,    94,    95,    96,    97,    98,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
      -1,    -1,    11,    12,   116,   117,   118,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    -1,
      59,    60,    -1,    62,    63,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    82,    -1,    -1,    85,    86,    -1,    88,
      89,    -1,    91,    92,    93,    94,    95,    96,    97,    98,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   116,   117,   118,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    -1,    59,    60,    -1,    62,    63,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    82,    -1,    -1,    85,
      86,    -1,    88,    89,    -1,    91,    92,    93,    94,    95,
      96,    97,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
     116,   117,   118,    16,    -1,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    -1,    59,    60,    -1,    62,
      63,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,
      -1,    -1,    85,    86,    -1,    88,    89,    -1,    91,    92,
      93,    94,    95,    96,    97,    98,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   116,   117,   118,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    -1,    59,
      60,    -1,    62,    63,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    82,    -1,    -1,    85,    86,    -1,    88,    89,
      -1,    91,    92,    93,    94,    95,    96,    97,    98,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,   116,   117,   118,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    59,    60,    -1,    62,    63,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    82,    -1,    -1,    85,    86,
      -1,    88,    89,    -1,    91,    92,    93,    94,    95,    96,
      97,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,   116,
     117,   118,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    -1,    -1,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    -1,    59,    60,    -1,    62,    63,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,    -1,
      -1,    85,    86,    -1,    88,    89,    -1,    91,    92,    93,
      94,    95,    96,    97,    98,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   116,   117,   118,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    59,    60,
      -1,    62,    63,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    82,    -1,    -1,    85,    86,    -1,    88,    89,    -1,
      91,    92,    93,    94,    95,    96,    97,    98,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,
      -1,    -1,    -1,    11,    12,   116,   117,   118,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      -1,    59,    60,    -1,    62,    63,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    82,    -1,    -1,    85,    86,    -1,
      88,    89,    -1,    91,    92,    93,    94,    95,    96,    97,
      98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   116,   117,
     118,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    -1,    59,    60,    -1,    62,    63,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,    -1,    -1,
      85,    86,    -1,    88,    89,    -1,    91,    92,    93,    94,
      95,    96,    97,    98,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
      12,   116,   117,   118,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    -1,    59,    60,    -1,
      62,    63,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      82,    -1,    -1,    85,    86,    -1,    88,    89,    -1,    91,
      92,    93,    94,    95,    96,    97,    98,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
      -1,    -1,    11,    12,   116,   117,   118,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    -1,    -1,    38,
      -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    -1,
      59,    60,    -1,    62,    63,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    82,    -1,    -1,    85,    86,    -1,    88,
      89,    -1,    -1,    -1,    93,    94,    95,    96,    97,    98,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   116,   117,   118,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    -1,    59,    60,    -1,    62,    63,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    82,    -1,    -1,    85,
      86,    -1,    88,    89,    -1,    91,    -1,    93,    94,    95,
      96,    97,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
     116,   117,   118,    16,    -1,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    -1,    59,    60,    -1,    62,
      63,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,
      -1,    -1,    85,    86,    -1,    88,    89,    -1,    91,    -1,
      93,    94,    95,    96,    97,    98,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   116,   117,   118,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    -1,    59,
      60,    -1,    62,    63,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    82,    -1,    -1,    85,    86,    -1,    88,    89,
      -1,    91,    -1,    93,    94,    95,    96,    97,    98,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,   116,   117,   118,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    59,    60,    -1,    62,    63,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    82,    -1,    -1,    85,    86,
      -1,    88,    89,    -1,    91,    -1,    93,    94,    95,    96,
      97,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,   116,
     117,   118,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    -1,    -1,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    -1,    59,    60,    -1,    62,    63,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,    -1,
      -1,    85,    86,    -1,    88,    89,    -1,    91,    -1,    93,
      94,    95,    96,    97,    98,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   116,   117,   118,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    59,    60,
      -1,    62,    63,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    82,    -1,    -1,    85,    86,    -1,    88,    89,    -1,
      91,    -1,    93,    94,    95,    96,    97,    98,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,
      -1,    -1,    -1,    11,    12,   116,   117,   118,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      -1,    59,    60,    -1,    62,    63,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    82,    -1,    -1,    85,    86,    -1,
      88,    89,    -1,    -1,    -1,    93,    94,    95,    96,    97,
      98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   116,   117,
     118,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    -1,    59,    60,    -1,    62,    63,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,    -1,    -1,
      85,    86,    -1,    88,    89,    -1,    -1,    -1,    93,    94,
      95,    96,    97,    98,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
      12,   116,   117,   118,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    -1,    59,    60,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      82,    -1,    -1,    85,    86,    -1,    88,    89,    -1,    91,
      -1,    93,    94,    95,    96,    97,    98,    -1,    -1,    -1,
       3,     4,     5,    -1,     7,    -1,   108,    -1,    11,    12,
      -1,    -1,    -1,    16,   116,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    -1,    59,    60,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    79,    -1,    -1,    82,
      -1,    -1,    85,    86,    -1,    88,    89,    -1,    -1,    -1,
      93,    94,    95,    96,    97,    98,    -1,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,    -1,
      -1,    -1,    16,   116,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    -1,    -1,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    -1,    59,    60,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,    -1,
      -1,    85,    86,    -1,    88,    89,    -1,    91,    -1,    93,
      94,    95,    96,    97,    98,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,    -1,    -1,
      -1,    16,   116,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    -1,    59,    60,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,    -1,    -1,
      85,    86,    -1,    88,    89,    -1,    91,    -1,    93,    94,
      95,    96,    97,    98,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,    -1,    -1,    -1,
      16,   116,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    -1,    59,    60,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    82,    -1,    -1,    85,
      86,    -1,    88,    89,    -1,    -1,    -1,    93,    94,    95,
      96,    97,    98,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,    -1,    -1,    -1,    16,
     116,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    -1,    -1,    43,    -1,    -1,    -1,    -1,    45,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    59,    60,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    -1,    -1,    79,
      80,    -1,    -1,    -1,    -1,    82,    -1,    -1,    85,    86,
      -1,    88,    89,    -1,    -1,    -1,    93,    94,    95,    96,
      97,    98,    -1,    -1,   104,    -1,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,    50,    51,    -1,   116,
      54,    -1,    -1,    -1,    -1,    -1,   126,    -1,    62,    63,
      64,    65,    66,    67,    -1,    69,    70,    -1,    -1,    73,
      -1,    -1,    -1,    77,    78,    79,    80,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    91,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,    50,    51,   118,    -1,    54,    -1,    -1,    -1,
      -1,    -1,    -1,   127,    62,    63,    64,    65,    66,    67,
      -1,    69,    70,    -1,    -1,    73,    -1,    -1,    -1,    77,
      78,    79,    80,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    91,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,    50,    51,
     118,    -1,    54,    -1,    -1,    -1,    -1,    -1,    -1,   127,
      62,    63,    64,    65,    66,    67,    -1,    69,    70,    -1,
      -1,    73,    -1,    -1,    -1,    77,    78,    79,    80,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    91,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,    50,    51,   118,    -1,    54,    -1,
      -1,    -1,    -1,    -1,    -1,   127,    62,    63,    64,    65,
      66,    67,    -1,    69,    70,    -1,    -1,    73,    -1,    -1,
      -1,    77,    78,    79,    80,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    91,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
      50,    51,   118,    -1,    54,    -1,    -1,    -1,    -1,    -1,
      -1,   127,    62,    63,    64,    65,    66,    67,    -1,    69,
      70,    -1,    -1,    73,    -1,    -1,    -1,    77,    78,    79,
      80,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    91,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,    50,    51,   118,    -1,
      54,    -1,    -1,    -1,    -1,    -1,    -1,   127,    62,    63,
      64,    65,    66,    67,    -1,    69,    70,    -1,    -1,    73,
      -1,    -1,    -1,    77,    78,    79,    80,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    91,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,    50,    51,   118,    -1,    54,    -1,    -1,    -1,
      -1,    -1,    -1,   127,    62,    63,    64,    65,    66,    67,
      -1,    69,    70,    -1,    -1,    73,    -1,    -1,    -1,    77,
      78,    79,    80,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    91,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,    50,    51,
     118,    -1,    54,    -1,    -1,    -1,    -1,    -1,    -1,   127,
      62,    63,    64,    65,    66,    67,    -1,    69,    70,    -1,
      -1,    73,    -1,    -1,    -1,    77,    78,    79,    80,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    91,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,    50,    51,   118,    -1,    54,    -1,
      -1,    -1,    -1,    -1,    -1,   127,    62,    63,    64,    65,
      66,    67,    -1,    69,    70,    -1,    -1,    73,    -1,    -1,
      -1,    77,    78,    79,    80,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    91,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
      50,    51,   118,    -1,    54,    -1,    -1,    -1,    -1,    -1,
      -1,   127,    62,    63,    64,    65,    66,    67,    -1,    69,
      70,    -1,    -1,    73,    -1,    -1,    -1,    77,    78,    79,
      80,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    91,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,    50,    51,   118,    -1,
      54,    -1,    -1,    -1,    -1,    -1,    -1,   127,    62,    63,
      64,    65,    66,    67,    -1,    69,    70,    -1,    -1,    73,
      -1,    -1,    -1,    77,    78,    79,    80,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    91,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,    -1,    -1,   118,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   127,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    -1,    -1,    79,
      80,    -1,    -1,    -1,    84,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    -1,    -1,
      79,    80,    -1,    -1,   104,    -1,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   104,   126,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   126,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    -1,    -1,    79,    80,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    -1,    -1,
      79,    80,    -1,    -1,    -1,    -1,    -1,    -1,   104,    -1,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,    64,    65,    66,
      67,    68,    69,    70,    -1,    -1,    73,    74,    -1,    -1,
      -1,    -1,    79,    80,    64,    65,    66,    67,    68,    69,
      70,    -1,    -1,    73,    74,    -1,    -1,    -1,    -1,    79,
      80,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned short yystos[] =
{
       0,   133,   134,     0,     1,     3,     4,     5,     6,     7,
      11,    12,    16,    18,    19,    20,    21,    22,    23,    24,
      29,    30,    31,    32,    33,    34,    35,    38,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    59,    60,    62,    63,    82,    85,    86,    88,
      89,    91,    93,    94,    95,    96,    97,    98,   116,   117,
     118,   136,   137,   138,   141,   143,   144,   148,   149,   151,
     152,   153,   154,   155,   164,   181,   198,   208,   209,   222,
     223,   224,   225,   226,   227,   228,   231,   240,   242,   243,
     244,   245,   246,   247,   264,   273,   138,    21,    22,    29,
      30,    31,    45,    50,    54,    79,    82,    85,   116,   156,
     157,   181,   198,   244,   247,   264,   157,     3,     4,     5,
       6,     7,     8,     9,    10,    13,    14,    15,    16,    17,
      20,    21,    22,    23,    24,    25,    26,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      54,    62,    63,    64,    65,    66,    67,    69,    70,    73,
      77,    78,    79,    80,    91,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   118,   127,   128,   158,   162,
     163,   245,   259,    32,    33,    34,    35,    48,    49,    50,
      54,    93,   158,   159,   160,   240,   182,   141,   142,   155,
     198,   244,   246,   247,   142,   130,   131,   142,   268,   271,
     272,   185,   187,   149,   155,   198,   203,   244,   247,    91,
      92,   117,   148,   164,   166,   170,   177,   179,   262,   263,
     170,   170,   128,   172,   173,   128,   168,   172,   141,    52,
     159,   130,   269,   140,   120,   164,   198,   164,    50,    54,
     156,    85,   136,   150,   151,   141,    91,   148,   167,   179,
     262,   273,   179,   261,   262,   273,   154,   198,   244,   247,
      52,    53,    55,   158,   234,   241,   233,   234,   234,   129,
     229,   129,   232,    56,    57,   143,   164,   164,   268,   272,
      39,    40,    41,    42,    43,    36,    37,    28,   205,   103,
     126,    85,    91,   152,   103,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    79,    80,
     104,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,    81,   122,   124,    81,   124,    26,   120,   210,   225,
      83,    83,   168,   172,   210,   269,   141,   156,    56,    57,
       1,   107,   248,   271,    81,   122,   124,   194,   260,   195,
      81,   124,   267,   126,   135,   136,    13,   105,   199,   271,
     103,    81,   122,   124,    83,    83,   199,   268,    15,    17,
     215,   131,   142,   142,    81,   122,   124,    25,   166,   166,
      84,   126,   178,   273,   126,   178,   125,   170,    86,   170,
     174,   148,   170,   179,   208,   273,    52,    59,    60,   139,
     128,   165,   120,   136,    81,   124,   150,   125,   125,   183,
     164,   269,   123,   126,   130,   270,   126,   270,   126,   270,
     121,   270,    81,   122,   124,    58,    99,   100,   101,   235,
     101,   235,   101,    61,   101,   101,   230,   235,   101,    58,
     101,    64,    64,   138,   142,   142,   142,   142,   138,   141,
     141,   206,    91,   143,   166,   179,   180,   150,   154,   126,
     143,   164,   166,   180,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
      50,    51,    54,   162,   265,   266,   167,    50,    51,    54,
     162,   265,    50,    54,   265,   265,   213,   211,   143,   164,
     143,   164,    90,   145,   192,   271,   249,   191,    50,    54,
     156,   265,   167,   265,   135,   141,    50,    52,    53,    54,
      55,    91,    92,   128,   250,   251,   252,   253,   254,   255,
     256,   257,   196,   161,    10,     8,   218,   273,   136,    13,
     164,    50,    54,   167,    50,    54,   136,   215,   136,    91,
     179,   216,    10,    27,   105,   200,   271,   200,    50,    54,
     167,    50,    54,   189,   126,   178,   166,    91,   166,   177,
     262,    91,   166,   263,   125,    91,   164,   166,   171,   175,
     177,   262,   269,   126,    81,   124,   269,   125,   159,   184,
     164,   136,   121,   125,   125,   269,    91,   166,    50,    54,
     167,    50,    54,   239,   236,   129,   235,   129,   164,   164,
      72,   108,   204,   273,   166,   126,   125,    43,   105,    83,
      83,   168,   172,   123,    83,    83,   168,   169,   172,   273,
     169,   172,   169,   172,   204,   204,   146,   271,   142,   135,
     123,    10,   269,   103,    50,    50,   251,   135,   271,   126,
     258,   273,   126,   258,   126,   258,   158,   159,   179,   219,
     273,    15,   202,   273,    14,   201,   202,    83,   123,    83,
      83,   202,    10,    10,   166,   126,   199,   186,   188,   123,
     142,   166,   126,   178,   166,   166,   126,   176,   125,   126,
     178,   125,   148,   208,   265,   265,   125,   141,   121,   164,
     123,   237,    52,    53,    55,   238,   247,   108,   203,   207,
      91,   166,   164,   164,   143,   164,   164,   145,    83,   143,
     164,   143,   164,   145,   214,   212,   204,   193,   271,    10,
     125,   166,   269,    10,   252,   255,   256,   257,    50,   254,
     256,   197,   126,    84,   220,   273,   136,     9,   221,   273,
     142,    10,    83,    10,    91,   136,   136,   136,   200,   178,
      91,   178,   178,    91,   177,   179,   262,   125,    91,   269,
     125,   269,   136,   108,   136,   166,   143,   164,   136,   136,
     147,   135,   125,   126,   258,   258,   258,   250,   155,   198,
     244,   247,   199,   136,   199,   166,   202,   215,   217,    10,
      10,   190,   164,   166,   126,   178,   126,   178,   166,   125,
     121,    10,    10,   121,   136,    10,   256,   135,    81,   122,
     124,   136,   136,   136,   178,   178,    91,   262,    91,   178,
     121,   258,    10,    50,    54,   167,    50,    54,   218,   201,
      10,   166,   126,   178,   166,   123,   178,    91,   178,   166,
     178
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
#define YYERROR		goto yyerrlab1


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

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)         \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
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

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (cinluded).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
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
  unsigned int yylineno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylineno);
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
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
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

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

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
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

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
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

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



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
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
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

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
	short *yyss1 = yyss;


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
	short *yyss1 = yyss;
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
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
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
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
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

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

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
#line 327 "parse.y"
    {
			lex_state = EXPR_BEG;
                        top_local_init();
			if ((VALUE)ruby_class == rb_cObject) class_nest = 0;
			else class_nest = 1;
		    }
    break;

  case 3:
#line 334 "parse.y"
    {
			if (yyvsp[0].node && !compile_for_eval) {
                            /* last expression should not be void */
			    if (nd_type(yyvsp[0].node) != NODE_BLOCK) void_expr(yyvsp[0].node);
			    else {
				NODE *node = yyvsp[0].node;
				while (node->nd_next) {
				    node = node->nd_next;
				}
				void_expr(node->nd_head);
			    }
			}
			ruby_eval_tree = block_append(ruby_eval_tree, yyvsp[0].node);
                        top_local_setup();
			class_nest = 0;
		    }
    break;

  case 4:
#line 356 "parse.y"
    {
		        yyval.node = yyvsp[-3].node;
			if (yyvsp[-2].node) {
			    yyval.node = NEW_RESCUE(yyvsp[-3].node, yyvsp[-2].node, yyvsp[-1].node);
			}
			else if (yyvsp[-1].node) {
			    rb_warn("else without rescue is useless");
			    yyval.node = block_append(yyval.node, yyvsp[-1].node);
			}
			if (yyvsp[0].node) {
			    yyval.node = NEW_ENSURE(yyval.node, yyvsp[0].node);
			}
			fixpos(yyval.node, yyvsp[-3].node);
		    }
    break;

  case 5:
#line 373 "parse.y"
    {
			void_stmts(yyvsp[-1].node);
		        yyval.node = yyvsp[-1].node;
		    }
    break;

  case 7:
#line 381 "parse.y"
    {
			yyval.node = newline_node(yyvsp[0].node);
		    }
    break;

  case 8:
#line 385 "parse.y"
    {
			yyval.node = block_append(yyvsp[-2].node, newline_node(yyvsp[0].node));
		    }
    break;

  case 9:
#line 389 "parse.y"
    {
			yyval.node = yyvsp[0].node;
		    }
    break;

  case 10:
#line 394 "parse.y"
    {lex_state = EXPR_FNAME;}
    break;

  case 11:
#line 395 "parse.y"
    {
		        yyval.node = NEW_ALIAS(yyvsp[-2].id, yyvsp[0].id);
		    }
    break;

  case 12:
#line 399 "parse.y"
    {
		        yyval.node = NEW_VALIAS(yyvsp[-1].id, yyvsp[0].id);
		    }
    break;

  case 13:
#line 403 "parse.y"
    {
			char buf[3];

			sprintf(buf, "$%c", (char)yyvsp[0].node->nd_nth);
		        yyval.node = NEW_VALIAS(yyvsp[-1].id, rb_intern(buf));
		    }
    break;

  case 14:
#line 410 "parse.y"
    {
		        yyerror("can't make alias for the number variables");
		        yyval.node = 0;
		    }
    break;

  case 15:
#line 415 "parse.y"
    {
			yyval.node = yyvsp[0].node;
		    }
    break;

  case 16:
#line 419 "parse.y"
    {
			yyval.node = NEW_IF(cond(yyvsp[0].node), yyvsp[-2].node, 0);
		        fixpos(yyval.node, yyvsp[0].node);
			if (cond_negative(&yyval.node->nd_cond)) {
		            yyval.node->nd_else = yyval.node->nd_body;
		            yyval.node->nd_body = 0;
			}
		    }
    break;

  case 17:
#line 428 "parse.y"
    {
			yyval.node = NEW_UNLESS(cond(yyvsp[0].node), yyvsp[-2].node, 0);
		        fixpos(yyval.node, yyvsp[0].node);
			if (cond_negative(&yyval.node->nd_cond)) {
		            yyval.node->nd_body = yyval.node->nd_else;
		            yyval.node->nd_else = 0;
			}
		    }
    break;

  case 18:
#line 437 "parse.y"
    {
			if (yyvsp[-2].node && nd_type(yyvsp[-2].node) == NODE_BEGIN) {
			    yyval.node = NEW_WHILE(cond(yyvsp[0].node), yyvsp[-2].node->nd_body, 0);
			}
			else {
			    yyval.node = NEW_WHILE(cond(yyvsp[0].node), yyvsp[-2].node, 1);
			}
			if (cond_negative(&yyval.node->nd_cond)) {
			    nd_set_type(yyval.node, NODE_UNTIL);
			}
		    }
    break;

  case 19:
#line 449 "parse.y"
    {
			if (yyvsp[-2].node && nd_type(yyvsp[-2].node) == NODE_BEGIN) {
			    yyval.node = NEW_UNTIL(cond(yyvsp[0].node), yyvsp[-2].node->nd_body, 0);
			}
			else {
			    yyval.node = NEW_UNTIL(cond(yyvsp[0].node), yyvsp[-2].node, 1);
			}
			if (cond_negative(&yyval.node->nd_cond)) {
			    nd_set_type(yyval.node, NODE_WHILE);
			}
		    }
    break;

  case 20:
#line 461 "parse.y"
    {
			yyval.node = NEW_RESCUE(yyvsp[-2].node, NEW_RESBODY(0,yyvsp[0].node,0), 0);
		    }
    break;

  case 21:
#line 465 "parse.y"
    {
			if (in_def || in_single) {
			    yyerror("BEGIN in method");
			}
			local_push(0);
		    }
    break;

  case 22:
#line 472 "parse.y"
    {
			ruby_eval_tree_begin = block_append(ruby_eval_tree_begin,
						            NEW_PREEXE(yyvsp[-1].node));
		        local_pop();
		        yyval.node = 0;
		    }
    break;

  case 23:
#line 479 "parse.y"
    {
			if (compile_for_eval && (in_def || in_single)) {
			    yyerror("END in method; use at_exit");
			}

			yyval.node = NEW_ITER(0, NEW_POSTEXE(), yyvsp[-1].node);
		    }
    break;

  case 24:
#line 487 "parse.y"
    {
			value_expr(yyvsp[0].node);
			yyval.node = node_assign(yyvsp[-2].node, yyvsp[0].node);
		    }
    break;

  case 25:
#line 492 "parse.y"
    {
			value_expr(yyvsp[0].node);
			yyvsp[-2].node->nd_value = NEW_RESTARY(yyvsp[0].node);
			yyval.node = yyvsp[-2].node;
		    }
    break;

  case 26:
#line 498 "parse.y"
    {
			value_expr(yyvsp[0].node);
			if (yyvsp[-2].node) {
			    ID vid = yyvsp[-2].node->nd_vid;
			    if (yyvsp[-1].id == tOROP) {
				yyvsp[-2].node->nd_value = yyvsp[0].node;
				yyval.node = NEW_OP_ASGN_OR(gettable(vid), yyvsp[-2].node);
				if (is_asgn_or_id(vid)) {
				    yyval.node->nd_aid = vid;
				}
			    }
			    else if (yyvsp[-1].id == tANDOP) {
				yyvsp[-2].node->nd_value = yyvsp[0].node;
				yyval.node = NEW_OP_ASGN_AND(gettable(vid), yyvsp[-2].node);
			    }
			    else {
				yyval.node = yyvsp[-2].node;
				yyval.node->nd_value = call_op(gettable(vid),yyvsp[-1].id,1,yyvsp[0].node);
			    }
			}
			else {
			    yyval.node = 0;
			}
		    }
    break;

  case 27:
#line 523 "parse.y"
    {
                        NODE *args;

			value_expr(yyvsp[0].node);
		        args = NEW_LIST(yyvsp[0].node);
			yyvsp[-3].node = list_append(yyvsp[-3].node, NEW_NIL());
			list_concat(args, yyvsp[-3].node);
			if (yyvsp[-1].id == tOROP) {
			    yyvsp[-1].id = 0;
			}
			else if (yyvsp[-1].id == tANDOP) {
			    yyvsp[-1].id = 1;
			}
			yyval.node = NEW_OP_ASGN1(yyvsp[-5].node, yyvsp[-1].id, args);
		        fixpos(yyval.node, yyvsp[-5].node);
		    }
    break;

  case 28:
#line 540 "parse.y"
    {
			value_expr(yyvsp[0].node);
			if (yyvsp[-1].id == tOROP) {
			    yyvsp[-1].id = 0;
			}
			else if (yyvsp[-1].id == tANDOP) {
			    yyvsp[-1].id = 1;
			}
			yyval.node = NEW_OP_ASGN2(yyvsp[-4].node, yyvsp[-2].id, yyvsp[-1].id, yyvsp[0].node);
		        fixpos(yyval.node, yyvsp[-4].node);
		    }
    break;

  case 29:
#line 552 "parse.y"
    {
			value_expr(yyvsp[0].node);
			if (yyvsp[-1].id == tOROP) {
			    yyvsp[-1].id = 0;
			}
			else if (yyvsp[-1].id == tANDOP) {
			    yyvsp[-1].id = 1;
			}
			yyval.node = NEW_OP_ASGN2(yyvsp[-4].node, yyvsp[-2].id, yyvsp[-1].id, yyvsp[0].node);
		        fixpos(yyval.node, yyvsp[-4].node);
		    }
    break;

  case 30:
#line 564 "parse.y"
    {
			value_expr(yyvsp[0].node);
			if (yyvsp[-1].id == tOROP) {
			    yyvsp[-1].id = 0;
			}
			else if (yyvsp[-1].id == tANDOP) {
			    yyvsp[-1].id = 1;
			}
			yyval.node = NEW_OP_ASGN2(yyvsp[-4].node, yyvsp[-2].id, yyvsp[-1].id, yyvsp[0].node);
		        fixpos(yyval.node, yyvsp[-4].node);
		    }
    break;

  case 31:
#line 576 "parse.y"
    {
		        rb_backref_error(yyvsp[-2].node);
			yyval.node = 0;
		    }
    break;

  case 32:
#line 581 "parse.y"
    {
			yyval.node = node_assign(yyvsp[-2].node, NEW_SVALUE(yyvsp[0].node));
		    }
    break;

  case 33:
#line 585 "parse.y"
    {
			yyvsp[-2].node->nd_value = yyvsp[0].node;
			yyval.node = yyvsp[-2].node;
		    }
    break;

  case 34:
#line 590 "parse.y"
    {
			yyvsp[-2].node->nd_value = yyvsp[0].node;
			yyval.node = yyvsp[-2].node;
		    }
    break;

  case 37:
#line 599 "parse.y"
    {
			yyval.node = logop(NODE_AND, yyvsp[-2].node, yyvsp[0].node);
		    }
    break;

  case 38:
#line 603 "parse.y"
    {
			yyval.node = logop(NODE_OR, yyvsp[-2].node, yyvsp[0].node);
		    }
    break;

  case 39:
#line 607 "parse.y"
    {
			yyval.node = NEW_NOT(cond(yyvsp[0].node));
		    }
    break;

  case 40:
#line 611 "parse.y"
    {
			yyval.node = NEW_NOT(cond(yyvsp[0].node));
		    }
    break;

  case 42:
#line 618 "parse.y"
    {
			value_expr(yyval.node);
			yyval.node = yyvsp[0].node;
		    }
    break;

  case 45:
#line 627 "parse.y"
    {
			yyval.node = NEW_RETURN(ret_args(yyvsp[0].node));
		    }
    break;

  case 46:
#line 631 "parse.y"
    {
			yyval.node = NEW_BREAK(ret_args(yyvsp[0].node));
		    }
    break;

  case 47:
#line 635 "parse.y"
    {
			yyval.node = NEW_NEXT(ret_args(yyvsp[0].node));
		    }
    break;

  case 49:
#line 642 "parse.y"
    {
			yyval.node = new_call(yyvsp[-3].node, yyvsp[-1].id, yyvsp[0].node);
		    }
    break;

  case 50:
#line 646 "parse.y"
    {
			yyval.node = new_call(yyvsp[-3].node, yyvsp[-1].id, yyvsp[0].node);
		    }
    break;

  case 51:
#line 652 "parse.y"
    {
			yyval.vars = dyna_push();
			yyvsp[0].num = ruby_sourceline;
		    }
    break;

  case 52:
#line 656 "parse.y"
    {yyval.vars = ruby_dyna_vars;}
    break;

  case 53:
#line 659 "parse.y"
    {
			yyval.node = NEW_ITER(yyvsp[-3].node, 0, dyna_init(yyvsp[-1].node, yyvsp[-2].vars));
			nd_set_line(yyval.node, yyvsp[-5].num);
			dyna_pop(yyvsp[-4].vars);
		    }
    break;

  case 54:
#line 667 "parse.y"
    {
			yyval.node = new_fcall(yyvsp[-1].id, yyvsp[0].node);
		        fixpos(yyval.node, yyvsp[0].node);
		   }
    break;

  case 55:
#line 672 "parse.y"
    {
			yyval.node = new_fcall(yyvsp[-2].id, yyvsp[-1].node);
			if (yyvsp[0].node) {
			    if (nd_type(yyval.node) == NODE_BLOCK_PASS) {
				rb_compile_error("both block arg and actual block given");
			    }
			    yyvsp[0].node->nd_iter = yyval.node;
			    yyval.node = yyvsp[0].node;
			}
		        fixpos(yyval.node, yyvsp[-1].node);
		   }
    break;

  case 56:
#line 684 "parse.y"
    {
			yyval.node = new_call(yyvsp[-3].node, yyvsp[-1].id, yyvsp[0].node);
		        fixpos(yyval.node, yyvsp[-3].node);
		    }
    break;

  case 57:
#line 689 "parse.y"
    {
			yyval.node = new_call(yyvsp[-4].node, yyvsp[-2].id, yyvsp[-1].node);
			if (yyvsp[0].node) {
			    if (nd_type(yyval.node) == NODE_BLOCK_PASS) {
				rb_compile_error("both block arg and actual block given");
			    }
			    yyvsp[0].node->nd_iter = yyval.node;
			    yyval.node = yyvsp[0].node;
			}
		        fixpos(yyval.node, yyvsp[-4].node);
		   }
    break;

  case 58:
#line 701 "parse.y"
    {
			yyval.node = new_call(yyvsp[-3].node, yyvsp[-1].id, yyvsp[0].node);
		        fixpos(yyval.node, yyvsp[-3].node);
		    }
    break;

  case 59:
#line 706 "parse.y"
    {
			yyval.node = new_call(yyvsp[-4].node, yyvsp[-2].id, yyvsp[-1].node);
			if (yyvsp[0].node) {
			    if (nd_type(yyval.node) == NODE_BLOCK_PASS) {
				rb_compile_error("both block arg and actual block given");
			    }
			    yyvsp[0].node->nd_iter = yyval.node;
			    yyval.node = yyvsp[0].node;
			}
		        fixpos(yyval.node, yyvsp[-4].node);
		   }
    break;

  case 60:
#line 718 "parse.y"
    {
			yyval.node = new_super(yyvsp[0].node);
		        fixpos(yyval.node, yyvsp[0].node);
		    }
    break;

  case 61:
#line 723 "parse.y"
    {
			yyval.node = new_yield(yyvsp[0].node);
		        fixpos(yyval.node, yyvsp[0].node);
		    }
    break;

  case 63:
#line 731 "parse.y"
    {
			yyval.node = yyvsp[-1].node;
		    }
    break;

  case 65:
#line 738 "parse.y"
    {
			yyval.node = NEW_MASGN(NEW_LIST(yyvsp[-1].node), 0);
		    }
    break;

  case 66:
#line 744 "parse.y"
    {
			yyval.node = NEW_MASGN(yyvsp[0].node, 0);
		    }
    break;

  case 67:
#line 748 "parse.y"
    {
			yyval.node = NEW_MASGN(list_append(yyvsp[-1].node,yyvsp[0].node), 0);
		    }
    break;

  case 68:
#line 752 "parse.y"
    {
			yyval.node = NEW_MASGN(yyvsp[-2].node, yyvsp[0].node);
		    }
    break;

  case 69:
#line 756 "parse.y"
    {
			yyval.node = NEW_MASGN(yyvsp[-1].node, -1);
		    }
    break;

  case 70:
#line 760 "parse.y"
    {
			yyval.node = NEW_MASGN(0, yyvsp[0].node);
		    }
    break;

  case 71:
#line 764 "parse.y"
    {
			yyval.node = NEW_MASGN(0, -1);
		    }
    break;

  case 73:
#line 771 "parse.y"
    {
			yyval.node = yyvsp[-1].node;
		    }
    break;

  case 74:
#line 777 "parse.y"
    {
			yyval.node = NEW_LIST(yyvsp[-1].node);
		    }
    break;

  case 75:
#line 781 "parse.y"
    {
			yyval.node = list_append(yyvsp[-2].node, yyvsp[-1].node);
		    }
    break;

  case 76:
#line 787 "parse.y"
    {
			yyval.node = assignable(yyvsp[0].id, 0);
		    }
    break;

  case 77:
#line 791 "parse.y"
    {
			yyval.node = aryset(yyvsp[-3].node, yyvsp[-1].node);
		    }
    break;

  case 78:
#line 795 "parse.y"
    {
			yyval.node = attrset(yyvsp[-2].node, yyvsp[0].id);
		    }
    break;

  case 79:
#line 799 "parse.y"
    {
			yyval.node = attrset(yyvsp[-2].node, yyvsp[0].id);
		    }
    break;

  case 80:
#line 803 "parse.y"
    {
			yyval.node = attrset(yyvsp[-2].node, yyvsp[0].id);
		    }
    break;

  case 81:
#line 807 "parse.y"
    {
			if (in_def || in_single)
			    yyerror("dynamic constant assignment");
			yyval.node = NEW_CDECL(0, 0, NEW_COLON2(yyvsp[-2].node, yyvsp[0].id));
		    }
    break;

  case 82:
#line 813 "parse.y"
    {
		        rb_backref_error(yyvsp[0].node);
			yyval.node = 0;
		    }
    break;

  case 83:
#line 820 "parse.y"
    {
			yyval.node = assignable(yyvsp[0].id, 0);
		    }
    break;

  case 84:
#line 824 "parse.y"
    {
			yyval.node = aryset(yyvsp[-3].node, yyvsp[-1].node);
		    }
    break;

  case 85:
#line 828 "parse.y"
    {
			yyval.node = attrset(yyvsp[-2].node, yyvsp[0].id);
		    }
    break;

  case 86:
#line 832 "parse.y"
    {
			yyval.node = attrset(yyvsp[-2].node, yyvsp[0].id);
		    }
    break;

  case 87:
#line 836 "parse.y"
    {
			yyval.node = attrset(yyvsp[-2].node, yyvsp[0].id);
		    }
    break;

  case 88:
#line 840 "parse.y"
    {
			if (in_def || in_single)
			    yyerror("dynamic constant assignment");
			yyval.node = NEW_CDECL(0, 0, NEW_COLON2(yyvsp[-2].node, yyvsp[0].id));
		    }
    break;

  case 89:
#line 846 "parse.y"
    {
		        rb_backref_error(yyvsp[0].node);
			yyval.node = 0;
		    }
    break;

  case 90:
#line 853 "parse.y"
    {
			yyerror("class/module name must be CONSTANT");
		    }
    break;

  case 92:
#line 860 "parse.y"
    {
			yyval.node = NEW_COLON3(yyvsp[0].id);
		    }
    break;

  case 93:
#line 864 "parse.y"
    {
			yyval.node = NEW_COLON2(0, yyval.node);
		    }
    break;

  case 94:
#line 868 "parse.y"
    {
			yyval.node = NEW_COLON2(yyvsp[-2].node, yyvsp[0].id);
		    }
    break;

  case 98:
#line 877 "parse.y"
    {
			lex_state = EXPR_END;
			yyval.id = yyvsp[0].id;
		    }
    break;

  case 99:
#line 882 "parse.y"
    {
			lex_state = EXPR_END;
			yyval.id = yyvsp[0].id;
		    }
    break;

  case 102:
#line 893 "parse.y"
    {
			yyval.node = NEW_UNDEF(yyvsp[0].id);
		    }
    break;

  case 103:
#line 896 "parse.y"
    {lex_state = EXPR_FNAME;}
    break;

  case 104:
#line 897 "parse.y"
    {
			yyval.node = block_append(yyvsp[-3].node, NEW_UNDEF(yyvsp[0].id));
		    }
    break;

  case 105:
#line 902 "parse.y"
    { yyval.id = '|'; }
    break;

  case 106:
#line 903 "parse.y"
    { yyval.id = '^'; }
    break;

  case 107:
#line 904 "parse.y"
    { yyval.id = '&'; }
    break;

  case 108:
#line 905 "parse.y"
    { yyval.id = tCMP; }
    break;

  case 109:
#line 906 "parse.y"
    { yyval.id = tEQ; }
    break;

  case 110:
#line 907 "parse.y"
    { yyval.id = tEQQ; }
    break;

  case 111:
#line 908 "parse.y"
    { yyval.id = tMATCH; }
    break;

  case 112:
#line 909 "parse.y"
    { yyval.id = '>'; }
    break;

  case 113:
#line 910 "parse.y"
    { yyval.id = tGEQ; }
    break;

  case 114:
#line 911 "parse.y"
    { yyval.id = '<'; }
    break;

  case 115:
#line 912 "parse.y"
    { yyval.id = tLEQ; }
    break;

  case 116:
#line 913 "parse.y"
    { yyval.id = tLSHFT; }
    break;

  case 117:
#line 914 "parse.y"
    { yyval.id = tRSHFT; }
    break;

  case 118:
#line 915 "parse.y"
    { yyval.id = '+'; }
    break;

  case 119:
#line 916 "parse.y"
    { yyval.id = '-'; }
    break;

  case 120:
#line 917 "parse.y"
    { yyval.id = '*'; }
    break;

  case 121:
#line 918 "parse.y"
    { yyval.id = '*'; }
    break;

  case 122:
#line 919 "parse.y"
    { yyval.id = '/'; }
    break;

  case 123:
#line 920 "parse.y"
    { yyval.id = '%'; }
    break;

  case 124:
#line 921 "parse.y"
    { yyval.id = tPOW; }
    break;

  case 125:
#line 922 "parse.y"
    { yyval.id = '~'; }
    break;

  case 126:
#line 923 "parse.y"
    { yyval.id = tUPLUS; }
    break;

  case 127:
#line 924 "parse.y"
    { yyval.id = tUMINUS; }
    break;

  case 128:
#line 925 "parse.y"
    { yyval.id = tAREF; }
    break;

  case 129:
#line 926 "parse.y"
    { yyval.id = tASET; }
    break;

  case 130:
#line 927 "parse.y"
    { yyval.id = '`'; }
    break;

  case 172:
#line 940 "parse.y"
    {
			yyval.node = node_assign(yyvsp[-2].node, yyvsp[0].node);
		    }
    break;

  case 173:
#line 944 "parse.y"
    {
			yyval.node = node_assign(yyvsp[-4].node, NEW_RESCUE(yyvsp[-2].node, NEW_RESBODY(0,yyvsp[0].node,0), 0));
		    }
    break;

  case 174:
#line 948 "parse.y"
    {
			value_expr(yyvsp[0].node);
			if (yyvsp[-2].node) {
			    ID vid = yyvsp[-2].node->nd_vid;
			    if (yyvsp[-1].id == tOROP) {
				yyvsp[-2].node->nd_value = yyvsp[0].node;
				yyval.node = NEW_OP_ASGN_OR(gettable(vid), yyvsp[-2].node);
				if (is_asgn_or_id(vid)) {
				    yyval.node->nd_aid = vid;
				}
			    }
			    else if (yyvsp[-1].id == tANDOP) {
				yyvsp[-2].node->nd_value = yyvsp[0].node;
				yyval.node = NEW_OP_ASGN_AND(gettable(vid), yyvsp[-2].node);
			    }
			    else {
				yyval.node = yyvsp[-2].node;
				yyval.node->nd_value = call_op(gettable(vid),yyvsp[-1].id,1,yyvsp[0].node);
			    }
			}
			else {
			    yyval.node = 0;
			}
		    }
    break;

  case 175:
#line 973 "parse.y"
    {
                        NODE *args;

			value_expr(yyvsp[0].node);
			args = NEW_LIST(yyvsp[0].node);
			yyvsp[-3].node = list_append(yyvsp[-3].node, NEW_NIL());
			list_concat(args, yyvsp[-3].node);
			if (yyvsp[-1].id == tOROP) {
			    yyvsp[-1].id = 0;
			}
			else if (yyvsp[-1].id == tANDOP) {
			    yyvsp[-1].id = 1;
			}
			yyval.node = NEW_OP_ASGN1(yyvsp[-5].node, yyvsp[-1].id, args);
		        fixpos(yyval.node, yyvsp[-5].node);
		    }
    break;

  case 176:
#line 990 "parse.y"
    {
			value_expr(yyvsp[0].node);
			if (yyvsp[-1].id == tOROP) {
			    yyvsp[-1].id = 0;
			}
			else if (yyvsp[-1].id == tANDOP) {
			    yyvsp[-1].id = 1;
			}
			yyval.node = NEW_OP_ASGN2(yyvsp[-4].node, yyvsp[-2].id, yyvsp[-1].id, yyvsp[0].node);
		        fixpos(yyval.node, yyvsp[-4].node);
		    }
    break;

  case 177:
#line 1002 "parse.y"
    {
			value_expr(yyvsp[0].node);
			if (yyvsp[-1].id == tOROP) {
			    yyvsp[-1].id = 0;
			}
			else if (yyvsp[-1].id == tANDOP) {
			    yyvsp[-1].id = 1;
			}
			yyval.node = NEW_OP_ASGN2(yyvsp[-4].node, yyvsp[-2].id, yyvsp[-1].id, yyvsp[0].node);
		        fixpos(yyval.node, yyvsp[-4].node);
		    }
    break;

  case 178:
#line 1014 "parse.y"
    {
			value_expr(yyvsp[0].node);
			if (yyvsp[-1].id == tOROP) {
			    yyvsp[-1].id = 0;
			}
			else if (yyvsp[-1].id == tANDOP) {
			    yyvsp[-1].id = 1;
			}
			yyval.node = NEW_OP_ASGN2(yyvsp[-4].node, yyvsp[-2].id, yyvsp[-1].id, yyvsp[0].node);
		        fixpos(yyval.node, yyvsp[-4].node);
		    }
    break;

  case 179:
#line 1026 "parse.y"
    {
			yyerror("constant re-assignment");
		    }
    break;

  case 180:
#line 1030 "parse.y"
    {
		        rb_backref_error(yyvsp[-2].node);
			yyval.node = 0;
		    }
    break;

  case 181:
#line 1035 "parse.y"
    {
			value_expr(yyvsp[-2].node);
			value_expr(yyvsp[0].node);
			yyval.node = NEW_DOT2(yyvsp[-2].node, yyvsp[0].node);
		    }
    break;

  case 182:
#line 1041 "parse.y"
    {
			value_expr(yyvsp[-2].node);
			value_expr(yyvsp[0].node);
			yyval.node = NEW_DOT3(yyvsp[-2].node, yyvsp[0].node);
		    }
    break;

  case 183:
#line 1047 "parse.y"
    {
			yyval.node = call_op(yyvsp[-2].node, '+', 1, yyvsp[0].node);
		    }
    break;

  case 184:
#line 1051 "parse.y"
    {
		        yyval.node = call_op(yyvsp[-2].node, '-', 1, yyvsp[0].node);
		    }
    break;

  case 185:
#line 1055 "parse.y"
    {
		        yyval.node = call_op(yyvsp[-2].node, '*', 1, yyvsp[0].node);
		    }
    break;

  case 186:
#line 1059 "parse.y"
    {
			yyval.node = call_op(yyvsp[-2].node, '/', 1, yyvsp[0].node);
		    }
    break;

  case 187:
#line 1063 "parse.y"
    {
			yyval.node = call_op(yyvsp[-2].node, '%', 1, yyvsp[0].node);
		    }
    break;

  case 188:
#line 1067 "parse.y"
    {
			yyval.node = call_op(yyvsp[-2].node, tPOW, 1, yyvsp[0].node);
		    }
    break;

  case 189:
#line 1071 "parse.y"
    {
			yyval.node = call_op(call_op(yyvsp[-2].node, tPOW, 1, yyvsp[0].node), tUMINUS, 0, 0);
		    }
    break;

  case 190:
#line 1075 "parse.y"
    {
			yyval.node = call_op(call_op(yyvsp[-2].node, tPOW, 1, yyvsp[0].node), tUMINUS, 0, 0);
		    }
    break;

  case 191:
#line 1079 "parse.y"
    {
			if (yyvsp[0].node && nd_type(yyvsp[0].node) == NODE_LIT) {
			    yyval.node = yyvsp[0].node;
			}
			else {
			    yyval.node = call_op(yyvsp[0].node, tUPLUS, 0, 0);
			}
		    }
    break;

  case 192:
#line 1088 "parse.y"
    {
			yyval.node = call_op(yyvsp[0].node, tUMINUS, 0, 0);
		    }
    break;

  case 193:
#line 1092 "parse.y"
    {
		        yyval.node = call_op(yyvsp[-2].node, '|', 1, yyvsp[0].node);
		    }
    break;

  case 194:
#line 1096 "parse.y"
    {
			yyval.node = call_op(yyvsp[-2].node, '^', 1, yyvsp[0].node);
		    }
    break;

  case 195:
#line 1100 "parse.y"
    {
			yyval.node = call_op(yyvsp[-2].node, '&', 1, yyvsp[0].node);
		    }
    break;

  case 196:
#line 1104 "parse.y"
    {
			yyval.node = call_op(yyvsp[-2].node, tCMP, 1, yyvsp[0].node);
		    }
    break;

  case 197:
#line 1108 "parse.y"
    {
			yyval.node = call_op(yyvsp[-2].node, '>', 1, yyvsp[0].node);
		    }
    break;

  case 198:
#line 1112 "parse.y"
    {
			yyval.node = call_op(yyvsp[-2].node, tGEQ, 1, yyvsp[0].node);
		    }
    break;

  case 199:
#line 1116 "parse.y"
    {
			yyval.node = call_op(yyvsp[-2].node, '<', 1, yyvsp[0].node);
		    }
    break;

  case 200:
#line 1120 "parse.y"
    {
			yyval.node = call_op(yyvsp[-2].node, tLEQ, 1, yyvsp[0].node);
		    }
    break;

  case 201:
#line 1124 "parse.y"
    {
			yyval.node = call_op(yyvsp[-2].node, tEQ, 1, yyvsp[0].node);
		    }
    break;

  case 202:
#line 1128 "parse.y"
    {
			yyval.node = call_op(yyvsp[-2].node, tEQQ, 1, yyvsp[0].node);
		    }
    break;

  case 203:
#line 1132 "parse.y"
    {
			yyval.node = NEW_NOT(call_op(yyvsp[-2].node, tEQ, 1, yyvsp[0].node));
		    }
    break;

  case 204:
#line 1136 "parse.y"
    {
			yyval.node = match_gen(yyvsp[-2].node, yyvsp[0].node);
		    }
    break;

  case 205:
#line 1140 "parse.y"
    {
			yyval.node = NEW_NOT(match_gen(yyvsp[-2].node, yyvsp[0].node));
		    }
    break;

  case 206:
#line 1144 "parse.y"
    {
			yyval.node = NEW_NOT(cond(yyvsp[0].node));
		    }
    break;

  case 207:
#line 1148 "parse.y"
    {
			yyval.node = call_op(yyvsp[0].node, '~', 0, 0);
		    }
    break;

  case 208:
#line 1152 "parse.y"
    {
			yyval.node = call_op(yyvsp[-2].node, tLSHFT, 1, yyvsp[0].node);
		    }
    break;

  case 209:
#line 1156 "parse.y"
    {
			yyval.node = call_op(yyvsp[-2].node, tRSHFT, 1, yyvsp[0].node);
		    }
    break;

  case 210:
#line 1160 "parse.y"
    {
			yyval.node = logop(NODE_AND, yyvsp[-2].node, yyvsp[0].node);
		    }
    break;

  case 211:
#line 1164 "parse.y"
    {
			yyval.node = logop(NODE_OR, yyvsp[-2].node, yyvsp[0].node);
		    }
    break;

  case 212:
#line 1167 "parse.y"
    {in_defined = 1;}
    break;

  case 213:
#line 1168 "parse.y"
    {
		        in_defined = 0;
			yyval.node = NEW_DEFINED(yyvsp[0].node);
		    }
    break;

  case 214:
#line 1173 "parse.y"
    {
			yyval.node = NEW_IF(cond(yyvsp[-4].node), yyvsp[-2].node, yyvsp[0].node);
		        fixpos(yyval.node, yyvsp[-4].node);
		    }
    break;

  case 215:
#line 1178 "parse.y"
    {
			yyval.node = yyvsp[0].node;
		    }
    break;

  case 216:
#line 1184 "parse.y"
    {
			value_expr(yyvsp[0].node);
			yyval.node = yyvsp[0].node;
		    }
    break;

  case 218:
#line 1192 "parse.y"
    {
		        rb_warn("parenthesize argument(s) for future version");
			yyval.node = NEW_LIST(yyvsp[-1].node);
		    }
    break;

  case 219:
#line 1197 "parse.y"
    {
			yyval.node = yyvsp[-1].node;
		    }
    break;

  case 220:
#line 1201 "parse.y"
    {
			value_expr(yyvsp[-1].node);
			yyval.node = arg_concat(yyvsp[-4].node, yyvsp[-1].node);
		    }
    break;

  case 221:
#line 1206 "parse.y"
    {
			yyval.node = NEW_LIST(NEW_HASH(yyvsp[-1].node));
		    }
    break;

  case 222:
#line 1210 "parse.y"
    {
			value_expr(yyvsp[-1].node);
			yyval.node = NEW_RESTARY2(yyvsp[-1].node);
		    }
    break;

  case 223:
#line 1217 "parse.y"
    {
			yyval.node = yyvsp[-1].node;
		    }
    break;

  case 224:
#line 1221 "parse.y"
    {
			yyval.node = yyvsp[-2].node;
		    }
    break;

  case 225:
#line 1225 "parse.y"
    {
		        rb_warn("parenthesize argument for future version");
			yyval.node = NEW_LIST(yyvsp[-2].node);
		    }
    break;

  case 226:
#line 1230 "parse.y"
    {
		        rb_warn("parenthesize argument for future version");
			yyval.node = list_append(yyvsp[-4].node, yyvsp[-2].node);
		    }
    break;

  case 229:
#line 1241 "parse.y"
    {
		        rb_warn("parenthesize argument(s) for future version");
			yyval.node = NEW_LIST(yyvsp[0].node);
		    }
    break;

  case 230:
#line 1246 "parse.y"
    {
			yyval.node = arg_blk_pass(yyvsp[-1].node, yyvsp[0].node);
		    }
    break;

  case 231:
#line 1250 "parse.y"
    {
			yyval.node = arg_concat(yyvsp[-4].node, yyvsp[-1].node);
			yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
		    }
    break;

  case 232:
#line 1255 "parse.y"
    {
			yyval.node = NEW_LIST(NEW_HASH(yyvsp[-1].node));
			yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
		    }
    break;

  case 233:
#line 1260 "parse.y"
    {
			yyval.node = arg_concat(NEW_LIST(NEW_HASH(yyvsp[-4].node)), yyvsp[-1].node);
			yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
		    }
    break;

  case 234:
#line 1265 "parse.y"
    {
			yyval.node = list_append(yyvsp[-3].node, NEW_HASH(yyvsp[-1].node));
			yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
		    }
    break;

  case 235:
#line 1270 "parse.y"
    {
			value_expr(yyvsp[-1].node);
			yyval.node = arg_concat(list_append(yyvsp[-6].node, NEW_HASH(yyvsp[-4].node)), yyvsp[-1].node);
			yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
		    }
    break;

  case 236:
#line 1276 "parse.y"
    {
			yyval.node = arg_blk_pass(NEW_RESTARY(yyvsp[-1].node), yyvsp[0].node);
		    }
    break;

  case 238:
#line 1283 "parse.y"
    {
			yyval.node = arg_blk_pass(list_concat(NEW_LIST(yyvsp[-3].node),yyvsp[-1].node), yyvsp[0].node);
		    }
    break;

  case 239:
#line 1287 "parse.y"
    {
                        yyval.node = arg_blk_pass(yyvsp[-2].node, yyvsp[0].node);
                    }
    break;

  case 240:
#line 1291 "parse.y"
    {
			yyval.node = arg_concat(NEW_LIST(yyvsp[-4].node), yyvsp[-1].node);
			yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
		    }
    break;

  case 241:
#line 1296 "parse.y"
    {
                       yyval.node = arg_concat(list_concat(NEW_LIST(yyvsp[-6].node),yyvsp[-4].node), yyvsp[-1].node);
			yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
		    }
    break;

  case 242:
#line 1301 "parse.y"
    {
			yyval.node = NEW_LIST(NEW_HASH(yyvsp[-1].node));
			yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
		    }
    break;

  case 243:
#line 1306 "parse.y"
    {
			yyval.node = arg_concat(NEW_LIST(NEW_HASH(yyvsp[-4].node)), yyvsp[-1].node);
			yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
		    }
    break;

  case 244:
#line 1311 "parse.y"
    {
			yyval.node = list_append(NEW_LIST(yyvsp[-3].node), NEW_HASH(yyvsp[-1].node));
			yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
		    }
    break;

  case 245:
#line 1316 "parse.y"
    {
			yyval.node = list_append(list_concat(NEW_LIST(yyvsp[-5].node),yyvsp[-3].node), NEW_HASH(yyvsp[-1].node));
			yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
		    }
    break;

  case 246:
#line 1321 "parse.y"
    {
			yyval.node = arg_concat(list_append(NEW_LIST(yyvsp[-6].node), NEW_HASH(yyvsp[-4].node)), yyvsp[-1].node);
			yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
		    }
    break;

  case 247:
#line 1326 "parse.y"
    {
			yyval.node = arg_concat(list_append(list_concat(NEW_LIST(yyvsp[-8].node), yyvsp[-6].node), NEW_HASH(yyvsp[-4].node)), yyvsp[-1].node);
			yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
		    }
    break;

  case 248:
#line 1331 "parse.y"
    {
			yyval.node = arg_blk_pass(NEW_RESTARY(yyvsp[-1].node), yyvsp[0].node);
		    }
    break;

  case 250:
#line 1337 "parse.y"
    {
			yyval.num = cmdarg_stack;
			CMDARG_PUSH(1);
		    }
    break;

  case 251:
#line 1342 "parse.y"
    {
			/* CMDARG_POP() */
		        cmdarg_stack = yyvsp[-1].num;
			yyval.node = yyvsp[0].node;
		    }
    break;

  case 253:
#line 1350 "parse.y"
    {lex_state = EXPR_ENDARG;}
    break;

  case 254:
#line 1351 "parse.y"
    {
		        rb_warn("don't put space before argument parentheses");
			yyval.node = 0;
		    }
    break;

  case 255:
#line 1355 "parse.y"
    {lex_state = EXPR_ENDARG;}
    break;

  case 256:
#line 1356 "parse.y"
    {
		        rb_warn("don't put space before argument parentheses");
			yyval.node = yyvsp[-2].node;
		    }
    break;

  case 257:
#line 1363 "parse.y"
    {
			yyval.node = NEW_BLOCK_PASS(yyvsp[0].node);
		    }
    break;

  case 258:
#line 1369 "parse.y"
    {
			yyval.node = yyvsp[0].node;
		    }
    break;

  case 260:
#line 1376 "parse.y"
    {
			yyval.node = NEW_LIST(yyvsp[0].node);
		    }
    break;

  case 261:
#line 1380 "parse.y"
    {
			yyval.node = list_append(yyvsp[-2].node, yyvsp[0].node);
		    }
    break;

  case 262:
#line 1386 "parse.y"
    {
			yyval.node = list_append(yyvsp[-2].node, yyvsp[0].node);
		    }
    break;

  case 263:
#line 1390 "parse.y"
    {
			yyval.node = arg_concat(yyvsp[-3].node, yyvsp[0].node);
		    }
    break;

  case 264:
#line 1394 "parse.y"
    {
			yyval.node = NEW_SPLAT(yyvsp[0].node);
		    }
    break;

  case 273:
#line 1408 "parse.y"
    {
			yyval.node = NEW_FCALL(yyvsp[0].id, 0);
		    }
    break;

  case 274:
#line 1412 "parse.y"
    {
			yyvsp[0].num = ruby_sourceline;
		    }
    break;

  case 275:
#line 1417 "parse.y"
    {
			yyval.node = NEW_BEGIN(yyvsp[-1].node);
			nd_set_line(yyval.node, yyvsp[-3].num);
		    }
    break;

  case 276:
#line 1421 "parse.y"
    {lex_state = EXPR_ENDARG;}
    break;

  case 277:
#line 1422 "parse.y"
    {
		        rb_warning("(...) interpreted as grouped expression");
			yyval.node = yyvsp[-2].node;
		    }
    break;

  case 278:
#line 1427 "parse.y"
    {
			yyval.node = yyvsp[-1].node;
		    }
    break;

  case 279:
#line 1431 "parse.y"
    {
			yyval.node = NEW_COLON2(yyvsp[-2].node, yyvsp[0].id);
		    }
    break;

  case 280:
#line 1435 "parse.y"
    {
			yyval.node = NEW_COLON3(yyvsp[0].id);
		    }
    break;

  case 281:
#line 1439 "parse.y"
    {
			if (nd_type(yyvsp[-3].node) == NODE_SELF)
			    yyval.node = NEW_FCALL(tAREF, yyvsp[-1].node);
			else
			    yyval.node = NEW_CALL(yyvsp[-3].node, tAREF, yyvsp[-1].node);
		    }
    break;

  case 282:
#line 1446 "parse.y"
    {
		        if (yyvsp[-1].node == 0) {
			    yyval.node = NEW_ZARRAY(); /* zero length array*/
			}
			else {
			    yyval.node = yyvsp[-1].node;
			}
		    }
    break;

  case 283:
#line 1455 "parse.y"
    {
			yyval.node = NEW_HASH(yyvsp[-1].node);
		    }
    break;

  case 284:
#line 1459 "parse.y"
    {
			yyval.node = NEW_RETURN(0);
		    }
    break;

  case 285:
#line 1463 "parse.y"
    {
			yyval.node = new_yield(yyvsp[-1].node);
		    }
    break;

  case 286:
#line 1467 "parse.y"
    {
			yyval.node = NEW_YIELD(0, Qfalse);
		    }
    break;

  case 287:
#line 1471 "parse.y"
    {
			yyval.node = NEW_YIELD(0, Qfalse);
		    }
    break;

  case 288:
#line 1474 "parse.y"
    {in_defined = 1;}
    break;

  case 289:
#line 1475 "parse.y"
    {
		        in_defined = 0;
			yyval.node = NEW_DEFINED(yyvsp[-1].node);
		    }
    break;

  case 290:
#line 1480 "parse.y"
    {
			yyvsp[0].node->nd_iter = NEW_FCALL(yyvsp[-1].id, 0);
			yyval.node = yyvsp[0].node;
		    }
    break;

  case 292:
#line 1486 "parse.y"
    {
			if (yyvsp[-1].node && nd_type(yyvsp[-1].node) == NODE_BLOCK_PASS) {
			    rb_compile_error("both block arg and actual block given");
			}
			yyvsp[0].node->nd_iter = yyvsp[-1].node;
			yyval.node = yyvsp[0].node;
		        fixpos(yyval.node, yyvsp[-1].node);
		    }
    break;

  case 293:
#line 1498 "parse.y"
    {
			yyval.node = NEW_IF(cond(yyvsp[-4].node), yyvsp[-2].node, yyvsp[-1].node);
		        fixpos(yyval.node, yyvsp[-4].node);
			if (cond_negative(&yyval.node->nd_cond)) {
		            NODE *tmp = yyval.node->nd_body;
		            yyval.node->nd_body = yyval.node->nd_else;
		            yyval.node->nd_else = tmp;
			}
		    }
    break;

  case 294:
#line 1511 "parse.y"
    {
			yyval.node = NEW_UNLESS(cond(yyvsp[-4].node), yyvsp[-2].node, yyvsp[-1].node);
		        fixpos(yyval.node, yyvsp[-4].node);
			if (cond_negative(&yyval.node->nd_cond)) {
		            NODE *tmp = yyval.node->nd_body;
		            yyval.node->nd_body = yyval.node->nd_else;
		            yyval.node->nd_else = tmp;
			}
		    }
    break;

  case 295:
#line 1520 "parse.y"
    {COND_PUSH(1);}
    break;

  case 296:
#line 1520 "parse.y"
    {COND_POP();}
    break;

  case 297:
#line 1523 "parse.y"
    {
			yyval.node = NEW_WHILE(cond(yyvsp[-4].node), yyvsp[-1].node, 1);
		        fixpos(yyval.node, yyvsp[-4].node);
			if (cond_negative(&yyval.node->nd_cond)) {
			    nd_set_type(yyval.node, NODE_UNTIL);
			}
		    }
    break;

  case 298:
#line 1530 "parse.y"
    {COND_PUSH(1);}
    break;

  case 299:
#line 1530 "parse.y"
    {COND_POP();}
    break;

  case 300:
#line 1533 "parse.y"
    {
			yyval.node = NEW_UNTIL(cond(yyvsp[-4].node), yyvsp[-1].node, 1);
		        fixpos(yyval.node, yyvsp[-4].node);
			if (cond_negative(&yyval.node->nd_cond)) {
			    nd_set_type(yyval.node, NODE_WHILE);
			}
		    }
    break;

  case 301:
#line 1543 "parse.y"
    {
			yyval.node = NEW_CASE(yyvsp[-3].node, yyvsp[-1].node);
		        fixpos(yyval.node, yyvsp[-3].node);
		    }
    break;

  case 302:
#line 1548 "parse.y"
    {
			yyval.node = yyvsp[-1].node;
		    }
    break;

  case 303:
#line 1552 "parse.y"
    {
			yyval.node = yyvsp[-1].node;
		    }
    break;

  case 304:
#line 1555 "parse.y"
    {COND_PUSH(1);}
    break;

  case 305:
#line 1555 "parse.y"
    {COND_POP();}
    break;

  case 306:
#line 1558 "parse.y"
    {
			yyval.node = NEW_FOR(yyvsp[-7].node, yyvsp[-4].node, yyvsp[-1].node);
		        fixpos(yyval.node, yyvsp[-7].node);
		    }
    break;

  case 307:
#line 1563 "parse.y"
    {
			if (in_def || in_single)
			    yyerror("class definition in method body");
			class_nest++;
			local_push(0);
		        yyval.num = ruby_sourceline;
		    }
    break;

  case 308:
#line 1572 "parse.y"
    {
		        yyval.node = NEW_CLASS(yyvsp[-4].node, yyvsp[-1].node, yyvsp[-3].node);
		        nd_set_line(yyval.node, yyvsp[-2].num);
		        local_pop();
			class_nest--;
		    }
    break;

  case 309:
#line 1579 "parse.y"
    {
			yyval.num = in_def;
		        in_def = 0;
		    }
    break;

  case 310:
#line 1584 "parse.y"
    {
		        yyval.num = in_single;
		        in_single = 0;
			class_nest++;
			local_push(0);
		    }
    break;

  case 311:
#line 1592 "parse.y"
    {
		        yyval.node = NEW_SCLASS(yyvsp[-5].node, yyvsp[-1].node);
		        fixpos(yyval.node, yyvsp[-5].node);
		        local_pop();
			class_nest--;
		        in_def = yyvsp[-4].num;
		        in_single = yyvsp[-2].num;
		    }
    break;

  case 312:
#line 1601 "parse.y"
    {
			if (in_def || in_single)
			    yyerror("module definition in method body");
			class_nest++;
			local_push(0);
		        yyval.num = ruby_sourceline;
		    }
    break;

  case 313:
#line 1610 "parse.y"
    {
		        yyval.node = NEW_MODULE(yyvsp[-3].node, yyvsp[-1].node);
		        nd_set_line(yyval.node, yyvsp[-2].num);
		        local_pop();
			class_nest--;
		    }
    break;

  case 314:
#line 1617 "parse.y"
    {
			yyval.id = cur_mid;
			cur_mid = yyvsp[0].id;
			in_def++;
			local_push(0);
		    }
    break;

  case 315:
#line 1626 "parse.y"
    {
			yyval.node = NEW_DEFN(yyvsp[-4].id, yyvsp[-2].node, yyvsp[-1].node, NOEX_PRIVATE);
		        fixpos(yyval.node, yyvsp[-2].node);
		        local_pop();
			in_def--;
			cur_mid = yyvsp[-3].id;
		    }
    break;

  case 316:
#line 1633 "parse.y"
    {lex_state = EXPR_FNAME;}
    break;

  case 317:
#line 1634 "parse.y"
    {
			in_single++;
			local_push(0);
		        lex_state = EXPR_END; /* force for args */
		    }
    break;

  case 318:
#line 1642 "parse.y"
    {
			yyval.node = NEW_DEFS(yyvsp[-7].node, yyvsp[-4].id, yyvsp[-2].node, yyvsp[-1].node);
		        fixpos(yyval.node, yyvsp[-7].node);
		        local_pop();
			in_single--;
		    }
    break;

  case 319:
#line 1649 "parse.y"
    {
			yyval.node = NEW_BREAK(0);
		    }
    break;

  case 320:
#line 1653 "parse.y"
    {
			yyval.node = NEW_NEXT(0);
		    }
    break;

  case 321:
#line 1657 "parse.y"
    {
			yyval.node = NEW_REDO();
		    }
    break;

  case 322:
#line 1661 "parse.y"
    {
			yyval.node = NEW_RETRY();
		    }
    break;

  case 323:
#line 1667 "parse.y"
    {
			value_expr(yyvsp[0].node);
			yyval.node = yyvsp[0].node;
		    }
    break;

  case 332:
#line 1688 "parse.y"
    {
			yyval.node = NEW_IF(cond(yyvsp[-3].node), yyvsp[-1].node, yyvsp[0].node);
		        fixpos(yyval.node, yyvsp[-3].node);
		    }
    break;

  case 334:
#line 1696 "parse.y"
    {
			yyval.node = yyvsp[0].node;
		    }
    break;

  case 338:
#line 1707 "parse.y"
    {
			yyval.node = (NODE*)1;
		    }
    break;

  case 339:
#line 1711 "parse.y"
    {
			yyval.node = (NODE*)1;
		    }
    break;

  case 340:
#line 1715 "parse.y"
    {
			yyval.node = yyvsp[-1].node;
		    }
    break;

  case 341:
#line 1721 "parse.y"
    {
		        yyval.vars = dyna_push();
			yyvsp[0].num = ruby_sourceline;
		    }
    break;

  case 342:
#line 1725 "parse.y"
    {yyval.vars = ruby_dyna_vars;}
    break;

  case 343:
#line 1728 "parse.y"
    {
			yyval.node = NEW_ITER(yyvsp[-3].node, 0, dyna_init(yyvsp[-1].node, yyvsp[-2].vars));
			nd_set_line(yyval.node, yyvsp[-5].num);
			dyna_pop(yyvsp[-4].vars);
		    }
    break;

  case 344:
#line 1736 "parse.y"
    {
			if (yyvsp[-1].node && nd_type(yyvsp[-1].node) == NODE_BLOCK_PASS) {
			    rb_compile_error("both block arg and actual block given");
			}
			yyvsp[0].node->nd_iter = yyvsp[-1].node;
			yyval.node = yyvsp[0].node;
		        fixpos(yyval.node, yyvsp[-1].node);
		    }
    break;

  case 345:
#line 1745 "parse.y"
    {
			yyval.node = new_call(yyvsp[-3].node, yyvsp[-1].id, yyvsp[0].node);
		    }
    break;

  case 346:
#line 1749 "parse.y"
    {
			yyval.node = new_call(yyvsp[-3].node, yyvsp[-1].id, yyvsp[0].node);
		    }
    break;

  case 347:
#line 1755 "parse.y"
    {
			yyval.node = new_fcall(yyvsp[-1].id, yyvsp[0].node);
		        fixpos(yyval.node, yyvsp[0].node);
		    }
    break;

  case 348:
#line 1760 "parse.y"
    {
			yyval.node = new_call(yyvsp[-3].node, yyvsp[-1].id, yyvsp[0].node);
		        fixpos(yyval.node, yyvsp[-3].node);
		    }
    break;

  case 349:
#line 1765 "parse.y"
    {
			yyval.node = new_call(yyvsp[-3].node, yyvsp[-1].id, yyvsp[0].node);
		        fixpos(yyval.node, yyvsp[-3].node);
		    }
    break;

  case 350:
#line 1770 "parse.y"
    {
			yyval.node = new_call(yyvsp[-2].node, yyvsp[0].id, 0);
		    }
    break;

  case 351:
#line 1774 "parse.y"
    {
			yyval.node = new_super(yyvsp[0].node);
		    }
    break;

  case 352:
#line 1778 "parse.y"
    {
			yyval.node = NEW_ZSUPER();
		    }
    break;

  case 353:
#line 1784 "parse.y"
    {
		        yyval.vars = dyna_push();
			yyvsp[0].num = ruby_sourceline;
		    }
    break;

  case 354:
#line 1788 "parse.y"
    {yyval.vars = ruby_dyna_vars;}
    break;

  case 355:
#line 1790 "parse.y"
    {
			yyval.node = NEW_ITER(yyvsp[-3].node, 0, dyna_init(yyvsp[-1].node, yyvsp[-2].vars));
			nd_set_line(yyval.node, yyvsp[-5].num);
			dyna_pop(yyvsp[-4].vars);
		    }
    break;

  case 356:
#line 1796 "parse.y"
    {
		        yyval.vars = dyna_push();
			yyvsp[0].num = ruby_sourceline;
		    }
    break;

  case 357:
#line 1800 "parse.y"
    {yyval.vars = ruby_dyna_vars;}
    break;

  case 358:
#line 1802 "parse.y"
    {
			yyval.node = NEW_ITER(yyvsp[-3].node, 0, dyna_init(yyvsp[-1].node, yyvsp[-2].vars));
			nd_set_line(yyval.node, yyvsp[-5].num);
			dyna_pop(yyvsp[-4].vars);
		    }
    break;

  case 359:
#line 1812 "parse.y"
    {
			yyval.node = NEW_WHEN(yyvsp[-3].node, yyvsp[-1].node, yyvsp[0].node);
		    }
    break;

  case 361:
#line 1819 "parse.y"
    {
			yyval.node = list_append(yyvsp[-3].node, NEW_WHEN(yyvsp[0].node, 0, 0));
		    }
    break;

  case 362:
#line 1823 "parse.y"
    {
			yyval.node = NEW_LIST(NEW_WHEN(yyvsp[0].node, 0, 0));
		    }
    break;

  case 365:
#line 1835 "parse.y"
    {
		        if (yyvsp[-3].node) {
		            yyvsp[-3].node = node_assign(yyvsp[-3].node, NEW_GVAR(rb_intern("$!")));
			    yyvsp[-1].node = block_append(yyvsp[-3].node, yyvsp[-1].node);
			}
			yyval.node = NEW_RESBODY(yyvsp[-4].node, yyvsp[-1].node, yyvsp[0].node);
		        fixpos(yyval.node, yyvsp[-4].node?yyvsp[-4].node:yyvsp[-1].node);
		    }
    break;

  case 369:
#line 1851 "parse.y"
    {
			yyval.node = yyvsp[0].node;
		    }
    break;

  case 371:
#line 1858 "parse.y"
    {
			if (yyvsp[0].node)
			    yyval.node = yyvsp[0].node;
			else
			    /* place holder */
			    yyval.node = NEW_NIL();
		    }
    break;

  case 374:
#line 1870 "parse.y"
    {
			yyval.node = NEW_LIT(ID2SYM(yyvsp[0].id));
		    }
    break;

  case 376:
#line 1877 "parse.y"
    {
			NODE *node = yyvsp[0].node;
			if (!node) {
			    node = NEW_STR(rb_str_new(0, 0));
			}
			else {
			    node = evstr2dstr(node);
			}
			yyval.node = node;
		    }
    break;

  case 378:
#line 1891 "parse.y"
    {
			yyval.node = literal_concat(yyvsp[-1].node, yyvsp[0].node);
		    }
    break;

  case 379:
#line 1897 "parse.y"
    {
			yyval.node = yyvsp[-1].node;
		    }
    break;

  case 380:
#line 1903 "parse.y"
    {
			NODE *node = yyvsp[-1].node;
			if (!node) {
			    node = NEW_XSTR(rb_str_new(0, 0));
			}
			else {
			    switch (nd_type(node)) {
			      case NODE_STR:
				nd_set_type(node, NODE_XSTR);
				break;
			      case NODE_DSTR:
				nd_set_type(node, NODE_DXSTR);
				break;
			      default:
				node = NEW_NODE(NODE_DXSTR, rb_str_new(0, 0), 1, NEW_LIST(node));
				break;
			    }
			}
			yyval.node = node;
		    }
    break;

  case 381:
#line 1926 "parse.y"
    {
			int options = yyvsp[0].num;
			NODE *node = yyvsp[-1].node;
			if (!node) {
			    node = NEW_LIT(rb_reg_new("", 0, options & ~RE_OPTION_ONCE));
			}
			else switch (nd_type(node)) {
			  case NODE_STR:
			    {
				VALUE src = node->nd_lit;
				nd_set_type(node, NODE_LIT);
				node->nd_lit = rb_reg_new(RSTRING(src)->ptr,
							  RSTRING(src)->len,
							  options & ~RE_OPTION_ONCE);
			    }
			    break;
			  default:
			    node = NEW_NODE(NODE_DSTR, rb_str_new(0, 0), 1, NEW_LIST(node));
			  case NODE_DSTR:
			    if (options & RE_OPTION_ONCE) {
				nd_set_type(node, NODE_DREGX_ONCE);
			    }
			    else {
				nd_set_type(node, NODE_DREGX);
			    }
			    node->nd_cflag = options & ~RE_OPTION_ONCE;
			    break;
			}
			yyval.node = node;
		    }
    break;

  case 382:
#line 1959 "parse.y"
    {
			yyval.node = NEW_ZARRAY();
		    }
    break;

  case 383:
#line 1963 "parse.y"
    {
			yyval.node = yyvsp[-1].node;
		    }
    break;

  case 384:
#line 1969 "parse.y"
    {
			yyval.node = 0;
		    }
    break;

  case 385:
#line 1973 "parse.y"
    {
			yyval.node = list_append(yyvsp[-2].node, evstr2dstr(yyvsp[-1].node));
		    }
    break;

  case 387:
#line 1980 "parse.y"
    {
			yyval.node = literal_concat(yyvsp[-1].node, yyvsp[0].node);
		    }
    break;

  case 388:
#line 1986 "parse.y"
    {
			yyval.node = NEW_ZARRAY();
		    }
    break;

  case 389:
#line 1990 "parse.y"
    {
			yyval.node = yyvsp[-1].node;
		    }
    break;

  case 390:
#line 1996 "parse.y"
    {
			yyval.node = 0;
		    }
    break;

  case 391:
#line 2000 "parse.y"
    {
			yyval.node = list_append(yyvsp[-2].node, yyvsp[-1].node);
		    }
    break;

  case 392:
#line 2006 "parse.y"
    {
			yyval.node = 0;
		    }
    break;

  case 393:
#line 2010 "parse.y"
    {
			yyval.node = literal_concat(yyvsp[-1].node, yyvsp[0].node);
		    }
    break;

  case 394:
#line 2016 "parse.y"
    {
			yyval.node = 0;
		    }
    break;

  case 395:
#line 2020 "parse.y"
    {
			yyval.node = literal_concat(yyvsp[-1].node, yyvsp[0].node);
		    }
    break;

  case 397:
#line 2027 "parse.y"
    {
			yyval.node = lex_strterm;
			lex_strterm = 0;
			lex_state = EXPR_BEG;
		    }
    break;

  case 398:
#line 2033 "parse.y"
    {
			lex_strterm = yyvsp[-1].node;
		        yyval.node = NEW_EVSTR(yyvsp[0].node);
		    }
    break;

  case 399:
#line 2038 "parse.y"
    {
			yyval.node = lex_strterm;
			lex_strterm = 0;
			lex_state = EXPR_BEG;
		    }
    break;

  case 400:
#line 2044 "parse.y"
    {
			quoted_term = yyvsp[-3].num;
			lex_strterm = yyvsp[-2].node;
			if ((yyval.node = yyvsp[-1].node) && nd_type(yyval.node) == NODE_NEWLINE) {
			    yyval.node = yyval.node->nd_next;
			    rb_gc_force_recycle((VALUE)yyvsp[-1].node);
			}
			yyval.node = new_evstr(yyval.node);
		    }
    break;

  case 401:
#line 2055 "parse.y"
    {yyval.node = NEW_GVAR(yyvsp[0].id);}
    break;

  case 402:
#line 2056 "parse.y"
    {yyval.node = NEW_IVAR(yyvsp[0].id);}
    break;

  case 403:
#line 2057 "parse.y"
    {yyval.node = NEW_CVAR(yyvsp[0].id);}
    break;

  case 405:
#line 2062 "parse.y"
    {
			if ((yyval.num = quoted_term) == -1 &&
			    nd_type(lex_strterm) == NODE_STRTERM &&
			    !nd_paren(lex_strterm)) {
			    quoted_term = nd_term(lex_strterm);
			}
		    }
    break;

  case 406:
#line 2072 "parse.y"
    {
		        lex_state = EXPR_END;
			yyval.id = yyvsp[0].id;
		    }
    break;

  case 411:
#line 2085 "parse.y"
    {
		        lex_state = EXPR_END;
			if (!(yyval.node = yyvsp[-1].node)) {
			    yyerror("empty symbol literal");
			}
			else {
			    switch (nd_type(yyval.node)) {
			      case NODE_DSTR:
				nd_set_type(yyval.node, NODE_DSYM);
				break;
			      case NODE_STR:
				if (strlen(RSTRING(yyval.node->nd_lit)->ptr) == RSTRING(yyval.node->nd_lit)->len) {
				    yyval.node->nd_lit = ID2SYM(rb_intern(RSTRING(yyval.node->nd_lit)->ptr));
				    nd_set_type(yyval.node, NODE_LIT);
				    break;
				}
				/* fall through */
			      default:
				yyval.node = NEW_NODE(NODE_DSYM, rb_str_new(0, 0), 1, NEW_LIST(yyval.node));
				break;
			    }
			}
		    }
    break;

  case 414:
#line 2113 "parse.y"
    {
			yyval.node = negate_lit(yyvsp[0].node);
		    }
    break;

  case 415:
#line 2117 "parse.y"
    {
			yyval.node = negate_lit(yyvsp[0].node);
		    }
    break;

  case 421:
#line 2127 "parse.y"
    {yyval.id = kNIL;}
    break;

  case 422:
#line 2128 "parse.y"
    {yyval.id = kSELF;}
    break;

  case 423:
#line 2129 "parse.y"
    {yyval.id = kTRUE;}
    break;

  case 424:
#line 2130 "parse.y"
    {yyval.id = kFALSE;}
    break;

  case 425:
#line 2131 "parse.y"
    {yyval.id = k__FILE__;}
    break;

  case 426:
#line 2132 "parse.y"
    {yyval.id = k__LINE__;}
    break;

  case 427:
#line 2136 "parse.y"
    {
			yyval.node = gettable(yyvsp[0].id);
		    }
    break;

  case 428:
#line 2142 "parse.y"
    {
			yyval.node = assignable(yyvsp[0].id, 0);
		    }
    break;

  case 431:
#line 2152 "parse.y"
    {
			yyval.node = 0;
		    }
    break;

  case 432:
#line 2156 "parse.y"
    {
			lex_state = EXPR_BEG;
		    }
    break;

  case 433:
#line 2160 "parse.y"
    {
			yyval.node = yyvsp[-1].node;
		    }
    break;

  case 434:
#line 2163 "parse.y"
    {yyerrok; yyval.node = 0;}
    break;

  case 435:
#line 2167 "parse.y"
    {
			yyval.node = yyvsp[-2].node;
			lex_state = EXPR_BEG;
		    }
    break;

  case 436:
#line 2172 "parse.y"
    {
			yyval.node = yyvsp[-1].node;
		    }
    break;

  case 437:
#line 2178 "parse.y"
    {
			yyval.node = block_append(NEW_ARGS(yyvsp[-5].num, yyvsp[-3].node, yyvsp[-1].id), yyvsp[0].node);
		    }
    break;

  case 438:
#line 2182 "parse.y"
    {
			yyval.node = block_append(NEW_ARGS(yyvsp[-3].num, yyvsp[-1].node, -1), yyvsp[0].node);
		    }
    break;

  case 439:
#line 2186 "parse.y"
    {
			yyval.node = block_append(NEW_ARGS(yyvsp[-3].num, 0, yyvsp[-1].id), yyvsp[0].node);
		    }
    break;

  case 440:
#line 2190 "parse.y"
    {
			yyval.node = block_append(NEW_ARGS(yyvsp[-1].num, 0, -1), yyvsp[0].node);
		    }
    break;

  case 441:
#line 2194 "parse.y"
    {
			yyval.node = block_append(NEW_ARGS(0, yyvsp[-3].node, yyvsp[-1].id), yyvsp[0].node);
		    }
    break;

  case 442:
#line 2198 "parse.y"
    {
			yyval.node = block_append(NEW_ARGS(0, yyvsp[-1].node, -1), yyvsp[0].node);
		    }
    break;

  case 443:
#line 2202 "parse.y"
    {
			yyval.node = block_append(NEW_ARGS(0, 0, yyvsp[-1].id), yyvsp[0].node);
		    }
    break;

  case 444:
#line 2206 "parse.y"
    {
			yyval.node = block_append(NEW_ARGS(0, 0, -1), yyvsp[0].node);
		    }
    break;

  case 445:
#line 2210 "parse.y"
    {
			yyval.node = NEW_ARGS(0, 0, -1);
		    }
    break;

  case 446:
#line 2216 "parse.y"
    {
			yyerror("formal argument cannot be a constant");
		    }
    break;

  case 447:
#line 2220 "parse.y"
    {
                        yyerror("formal argument cannot be an instance variable");
		    }
    break;

  case 448:
#line 2224 "parse.y"
    {
                        yyerror("formal argument cannot be a global variable");
		    }
    break;

  case 449:
#line 2228 "parse.y"
    {
                        yyerror("formal argument cannot be a class variable");
		    }
    break;

  case 450:
#line 2232 "parse.y"
    {
			if (!is_local_id(yyvsp[0].id))
			    yyerror("formal argument must be local variable");
			else if (local_id(yyvsp[0].id))
			    yyerror("duplicate argument name");
			local_cnt(yyvsp[0].id);
			yyval.num = 1;
		    }
    break;

  case 452:
#line 2244 "parse.y"
    {
			yyval.num += 1;
		    }
    break;

  case 453:
#line 2250 "parse.y"
    {
			if (!is_local_id(yyvsp[-2].id))
			    yyerror("formal argument must be local variable");
			else if (local_id(yyvsp[-2].id))
			    yyerror("duplicate optional argument name");
			yyval.node = assignable(yyvsp[-2].id, yyvsp[0].node);
		    }
    break;

  case 454:
#line 2260 "parse.y"
    {
			yyval.node = NEW_BLOCK(yyvsp[0].node);
			yyval.node->nd_end = yyval.node;
		    }
    break;

  case 455:
#line 2265 "parse.y"
    {
			yyval.node = block_append(yyvsp[-2].node, yyvsp[0].node);
		    }
    break;

  case 456:
#line 2271 "parse.y"
    {
			if (!is_local_id(yyvsp[0].id))
			    yyerror("rest argument must be local variable");
			else if (local_id(yyvsp[0].id))
			    yyerror("duplicate rest argument name");
			yyval.id = local_cnt(yyvsp[0].id);
		    }
    break;

  case 457:
#line 2279 "parse.y"
    {
			yyval.id = -2;
		    }
    break;

  case 458:
#line 2285 "parse.y"
    {
			if (!is_local_id(yyvsp[0].id))
			    yyerror("block argument must be local variable");
			else if (local_id(yyvsp[0].id))
			    yyerror("duplicate block argument name");
			yyval.node = NEW_BLOCK_ARG(yyvsp[0].id);
		    }
    break;

  case 459:
#line 2295 "parse.y"
    {
			yyval.node = yyvsp[0].node;
		    }
    break;

  case 461:
#line 2302 "parse.y"
    {
			if (nd_type(yyvsp[0].node) == NODE_SELF) {
			    yyval.node = NEW_SELF();
			}
			else {
			    yyval.node = yyvsp[0].node;
		            value_expr(yyval.node);
			}
		    }
    break;

  case 462:
#line 2311 "parse.y"
    {lex_state = EXPR_BEG;}
    break;

  case 463:
#line 2312 "parse.y"
    {
			if (yyvsp[-2].node == 0) {
			    yyerror("can't define single method for ().");
			}
			else {
			    switch (nd_type(yyvsp[-2].node)) {
			      case NODE_STR:
			      case NODE_DSTR:
			      case NODE_XSTR:
			      case NODE_DXSTR:
			      case NODE_DREGX:
			      case NODE_LIT:
			      case NODE_ARRAY:
			      case NODE_ZARRAY:
				yyerror("can't define single method for literals");
			      default:
				value_expr(yyvsp[-2].node);
				break;
			    }
			}
			yyval.node = yyvsp[-2].node;
		    }
    break;

  case 465:
#line 2338 "parse.y"
    {
			yyval.node = yyvsp[-1].node;
		    }
    break;

  case 466:
#line 2342 "parse.y"
    {
			if (yyvsp[-1].node->nd_alen%2 != 0) {
			    yyerror("odd number list for Hash");
			}
			yyval.node = yyvsp[-1].node;
		    }
    break;

  case 468:
#line 2352 "parse.y"
    {
			yyval.node = list_concat(yyvsp[-2].node, yyvsp[0].node);
		    }
    break;

  case 469:
#line 2358 "parse.y"
    {
			yyval.node = list_append(NEW_LIST(yyvsp[-2].node), yyvsp[0].node);
		    }
    break;

  case 489:
#line 2396 "parse.y"
    {yyerrok;}
    break;

  case 492:
#line 2401 "parse.y"
    {yyerrok;}
    break;

  case 493:
#line 2404 "parse.y"
    {yyval.node = 0;}
    break;


    }

/* Line 999 of yacc.c.  */
#line 6780 "parse.c"

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
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("syntax error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
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
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyss < yyssp)
	    {
	      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
	      yydestruct (yystos[*yyssp], yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
      yydestruct (yytoken, &yylval);
      yychar = YYEMPTY;

    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
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

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      yyvsp--;
      yystate = *--yyssp;

      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


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


#line 2406 "parse.y"

#include "regex.h"
#include "util.h"

/* We remove any previous definition of `SIGN_EXTEND_CHAR',
   since ours (we hope) works properly with all combinations of
   machines, compilers, `char' and `unsigned char' argument types.
   (Per Bothner suggested the basic approach.)  */
#undef SIGN_EXTEND_CHAR
#if __STDC__
# define SIGN_EXTEND_CHAR(c) ((signed char)(c))
#else  /* not __STDC__ */
/* As in Harbison and Steele.  */
# define SIGN_EXTEND_CHAR(c) ((((unsigned char)(c)) ^ 128) - 128)
#endif
#define is_identchar(c) (SIGN_EXTEND_CHAR(c)!=-1&&(ISALNUM(c) || (c) == '_' || ismbchar(c)))

static char *tokenbuf = NULL;
static int   tokidx, toksiz = 0;

#define LEAVE_BS 1

static VALUE (*lex_gets)();	/* gets function */
static VALUE lex_input;		/* non-nil if File */
static VALUE lex_lastline;	/* gc protect */
static char *lex_pbeg;
static char *lex_p;
static char *lex_pend;

static int
yyerror(msg)
    char *msg;
{
    char *p, *pe, *buf;
    int len, i;

    rb_compile_error("%s", msg);
    p = lex_p;
    while (lex_pbeg <= p) {
	if (*p == '\n') break;
	p--;
    }
    p++;

    pe = lex_p;
    while (pe < lex_pend) {
	if (*pe == '\n') break;
	pe++;
    }

    len = pe - p;
    if (len > 4) {
	buf = ALLOCA_N(char, len+2);
	MEMCPY(buf, p, char, len);
	buf[len] = '\0';
	rb_compile_error_append("%s", buf);

	i = lex_p - p;
	p = buf; pe = p + len;

	while (p < pe) {
	    if (*p != '\t') *p = ' ';
	    p++;
	}
	buf[i] = '^';
	buf[i+1] = '\0';
	rb_compile_error_append("%s", buf);
    }

    return 0;
}

static int heredoc_end;
static int command_start = Qtrue;

int ruby_in_compile = 0;
int ruby__end__seen;

static VALUE ruby_debug_lines;

static NODE*
yycompile(f, line)
    char *f;
    int line;
{
    int n;
    NODE *node = 0;
    struct RVarmap *vp, *vars = ruby_dyna_vars;

    ruby_in_compile = 1;
    if (!compile_for_eval && rb_safe_level() == 0 &&
	rb_const_defined(rb_cObject, rb_intern("SCRIPT_LINES__"))) {
	VALUE hash, fname;

	hash = rb_const_get(rb_cObject, rb_intern("SCRIPT_LINES__"));
	if (TYPE(hash) == T_HASH) {
	    fname = rb_str_new2(f);
	    ruby_debug_lines = rb_hash_aref(hash, fname);
	    if (NIL_P(ruby_debug_lines)) {
		ruby_debug_lines = rb_ary_new();
		rb_hash_aset(hash, fname, ruby_debug_lines);
	    }
	}
	if (line > 1) {
	    VALUE str = rb_str_new(0,0);
	    while (line > 1) {
		rb_ary_push(ruby_debug_lines, str);
		line--;
	    }
	}
    }

    ruby__end__seen = 0;
    ruby_eval_tree = 0;
    heredoc_end = 0;
    lex_strterm = 0;
    quoted_term = -1;
    ruby_current_node = 0;
    ruby_sourcefile = rb_source_filename(f);
    n = yyparse();
    ruby_debug_lines = 0;
    compile_for_eval = 0;
    ruby_in_compile = 0;
    cond_stack = 0;
    cmdarg_stack = 0;
    command_start = 1;		  
    class_nest = 0;
    in_single = 0;
    in_def = 0;
    cur_mid = 0;

    vp = ruby_dyna_vars;
    ruby_dyna_vars = vars;
    lex_strterm = 0;
    while (vp && vp != vars) {
	struct RVarmap *tmp = vp;
	vp = vp->next;
	rb_gc_force_recycle((VALUE)tmp);
    }
    if (n == 0) node = ruby_eval_tree;
    else ruby_eval_tree_begin = 0;
    return node;
}

static int lex_gets_ptr;

static VALUE
lex_get_str(s)
    VALUE s;
{
    char *beg, *end, *pend;

    beg = RSTRING(s)->ptr;
    if (lex_gets_ptr) {
	if (RSTRING(s)->len == lex_gets_ptr) return Qnil;
	beg += lex_gets_ptr;
    }
    pend = RSTRING(s)->ptr + RSTRING(s)->len;
    end = beg;
    while (end < pend) {
	if (*end++ == '\n') break;
    }
    lex_gets_ptr = end - RSTRING(s)->ptr;
    return rb_str_new(beg, end - beg);
}

static VALUE
lex_getline()
{
    VALUE line = (*lex_gets)(lex_input);
    if (ruby_debug_lines && !NIL_P(line)) {
	rb_ary_push(ruby_debug_lines, line);
    }
    return line;
}

NODE*
rb_compile_string(f, s, line)
    const char *f;
    VALUE s;
    int line;
{
    lex_gets = lex_get_str;
    lex_gets_ptr = 0;
    lex_input = s;
    lex_pbeg = lex_p = lex_pend = 0;
    ruby_sourceline = line - 1;
    compile_for_eval = ruby_in_eval;

    return yycompile(f, line);
}

NODE*
rb_compile_cstr(f, s, len, line)
    const char *f, *s;
    int len, line;
{
    return rb_compile_string(f, rb_str_new(s, len), line);
}

NODE*
rb_compile_file(f, file, start)
    const char *f;
    VALUE file;
    int start;
{
    lex_gets = rb_io_gets;
    lex_input = file;
    lex_pbeg = lex_p = lex_pend = 0;
    ruby_sourceline = start - 1;

    return yycompile(f, start);
}

static inline int
nextc()
{
    int c;

    if (lex_p == lex_pend) {
	if (lex_input) {
	    VALUE v = lex_getline();

	    if (NIL_P(v)) return -1;
	    if (heredoc_end > 0) {
		ruby_sourceline = heredoc_end;
		heredoc_end = 0;
	    }
	    ruby_sourceline++;
	    lex_pbeg = lex_p = RSTRING(v)->ptr;
	    lex_pend = lex_p + RSTRING(v)->len;
	    lex_lastline = v;
	}
	else {
	    lex_lastline = 0;
	    return -1;
	}
    }
    c = (unsigned char)*lex_p++;
    if (c == '\r' && lex_p < lex_pend && *lex_p == '\n') {
	lex_p++;
	c = '\n';
    }

    return c;
}

static void
pushback(c)
    int c;
{
    if (c == -1) return;
    lex_p--;
}

#define was_bol() (lex_p == lex_pbeg + 1)
#define peek(c) (lex_p != lex_pend && (c) == *lex_p)

#define tokfix() (tokenbuf[tokidx]='\0')
#define tok() tokenbuf
#define toklen() tokidx
#define toklast() (tokidx>0?tokenbuf[tokidx-1]:0)

static char*
newtok()
{
    tokidx = 0;
    if (!tokenbuf) {
	toksiz = 60;
	tokenbuf = ALLOC_N(char, 60);
    }
    if (toksiz > 4096) {
	toksiz = 60;
	REALLOC_N(tokenbuf, char, 60);
    }
    return tokenbuf;
}

static void
tokadd(c)
    char c;
{
    tokenbuf[tokidx++] = c;
    if (tokidx >= toksiz) {
	toksiz *= 2;
	REALLOC_N(tokenbuf, char, toksiz);
    }
}

static int
read_escape()
{
    int c;

    switch (c = nextc()) {
      case '\\':	/* Backslash */
	return c;

      case 'n':	/* newline */
	return '\n';

      case 't':	/* horizontal tab */
	return '\t';

      case 'r':	/* carriage-return */
	return '\r';

      case 'f':	/* form-feed */
	return '\f';

      case 'v':	/* vertical tab */
	return '\13';

      case 'a':	/* alarm(bell) */
	return '\007';

      case 'e':	/* escape */
	return 033;

      case '0': case '1': case '2': case '3': /* octal constant */
      case '4': case '5': case '6': case '7':
	{
	    int numlen;

	    pushback(c);
	    c = scan_oct(lex_p, 3, &numlen);
	    lex_p += numlen;
	}
	return c;

      case 'x':	/* hex constant */
	{
	    int numlen;

	    c = scan_hex(lex_p, 2, &numlen);
	    if (numlen == 0) {
		yyerror("Invalid escape character syntax");
		return 0;
	    }
	    lex_p += numlen;
	}
	return c;

      case 'b':	/* backspace */
	return '\010';

      case 's':	/* space */
	return ' ';

      case 'M':
	if ((c = nextc()) != '-') {
	    yyerror("Invalid escape character syntax");
	    pushback(c);
	    return '\0';
	}
	if ((c = nextc()) == '\\') {
	    return read_escape() | 0x80;
	}
	else if (c == -1) goto eof;
	else {
	    return ((c & 0xff) | 0x80);
	}

      case 'C':
	if ((c = nextc()) != '-') {
	    yyerror("Invalid escape character syntax");
	    pushback(c);
	    return '\0';
	}
      case 'c':
	if ((c = nextc())== '\\') {
	    c = read_escape();
	}
	else if (c == '?')
	    return 0177;
	else if (c == -1) goto eof;
	return c & 0x9f;

      eof:
      case -1:
        yyerror("Invalid escape character syntax");
	return '\0';

      default:
	return c;
    }
}

static int
tokadd_escape(term)
    int term;
{
    int c;

    switch (c = nextc()) {
      case '\n':
	return 0;		/* just ignore */

      case '0': case '1': case '2': case '3': /* octal constant */
      case '4': case '5': case '6': case '7':
	{
	    int i;

	    tokadd('\\');
	    tokadd(c);
	    for (i=0; i<2; i++) {
		c = nextc();
		if (c == -1) goto eof;
		if (c < '0' || '7' < c) {
		    pushback(c);
		    break;
		}
		tokadd(c);
	    }
	}
	return 0;

      case 'x':	/* hex constant */
	{
	    int numlen;

	    tokadd('\\');
	    tokadd(c);
	    scan_hex(lex_p, 2, &numlen);
	    if (numlen == 0) {
		yyerror("Invalid escape character syntax");
		return -1;
	    }
	    while (numlen--)
		tokadd(nextc());
	}
	return 0;

      case 'M':
	if ((c = nextc()) != '-') {
	    yyerror("Invalid escape character syntax");
	    pushback(c);
	    return 0;
	}
	tokadd('\\'); tokadd('M'); tokadd('-');
	goto escaped;

      case 'C':
	if ((c = nextc()) != '-') {
	    yyerror("Invalid escape character syntax");
	    pushback(c);
	    return 0;
	}
	tokadd('\\'); tokadd('C'); tokadd('-');
	goto escaped;

      case 'c':
	tokadd('\\'); tokadd('c');
      escaped:
	if ((c = nextc()) == '\\') {
	    return tokadd_escape(term);
	}
	else if (c == -1) goto eof;
	tokadd(c);
	return 0;

      eof:
      case -1:
        yyerror("Invalid escape character syntax");
	return -1;

      default:
	if (c != '\\' || c != term)
	    tokadd('\\');
	tokadd(c);
    }
    return 0;
}

static int
regx_options()
{
    char kcode = 0;
    int options = 0;
    int c;

    newtok();
    while (c = nextc(), ISALPHA(c)) {
	switch (c) {
	  case 'i':
	    options |= RE_OPTION_IGNORECASE;
	    break;
	  case 'x':
	    options |= RE_OPTION_EXTENDED;
	    break;
	  case 'm':
	    options |= RE_OPTION_MULTILINE;
	    break;
	  case 'o':
	    options |= RE_OPTION_ONCE;
	    break;
	  case 'n':
	    kcode = 16;
	    break;
	  case 'e':
	    kcode = 32;
	    break;
	  case 's':
	    kcode = 48;
	    break;
	  case 'u':
	    kcode = 64;
	    break;
	  default:
	    tokadd(c);
	    break;
	}
    }
    pushback(c);
    if (toklen()) {
	tokfix();
	rb_compile_error("unknown regexp option%s - %s",
			 toklen() > 1 ? "s" : "", tok());
    }
    return options | kcode;
}

#define STR_FUNC_ESCAPE 0x01
#define STR_FUNC_EXPAND 0x02
#define STR_FUNC_REGEXP 0x04
#define STR_FUNC_QWORDS 0x08
#define STR_FUNC_SYMBOL 0x10
#define STR_FUNC_INDENT 0x20

enum string_type {
    str_squote = (0),
    str_dquote = (STR_FUNC_EXPAND),
    str_xquote = (STR_FUNC_ESCAPE|STR_FUNC_EXPAND),
    str_regexp = (STR_FUNC_REGEXP|STR_FUNC_ESCAPE|STR_FUNC_EXPAND),
    str_sword  = (STR_FUNC_QWORDS),
    str_dword  = (STR_FUNC_QWORDS|STR_FUNC_EXPAND),
    str_ssym   = (STR_FUNC_SYMBOL),
    str_dsym   = (STR_FUNC_SYMBOL|STR_FUNC_EXPAND),
};

static void
dispose_string(str)
    VALUE str;
{
    free(RSTRING(str)->ptr);
    rb_gc_force_recycle(str);
}

static int
tokadd_string(func, term, paren, nest)
    int func, term, paren, *nest;
{
    int c;

    while ((c = nextc()) != -1) {
	if (paren && c == paren) {
	    ++*nest;
	}
	else if (c == term) {
	    if (!nest || !*nest) {
		pushback(c);
		break;
	    }
	    --*nest;
	}
	else if ((func & STR_FUNC_EXPAND) && c == '#' && lex_p < lex_pend) {
	    int c2 = *lex_p;
	    if (c2 == '$' || c2 == '@' || c2 == '{') {
		pushback(c);
		break;
	    }
	}
	else if (c == '\\') {
	    c = nextc();
	    if (QUOTED_TERM_P(c)) {
		pushback(c);
		return c;
	    }
	    switch (c) {
	      case '\n':
		continue;

	      case '\\':
		if (func & STR_FUNC_ESCAPE) tokadd(c);
		break;

	      default:
		if (func & STR_FUNC_REGEXP) {
		    pushback(c);
		    if (tokadd_escape(term) < 0)
			return -1;
		    continue;
		}
		else if (func & STR_FUNC_EXPAND) {
		    pushback(c);
		    if (func & STR_FUNC_ESCAPE) tokadd('\\');
		    c = read_escape();
		}
		else if ((func & STR_FUNC_QWORDS) && ISSPACE(c)) {
		    /* ignore backslashed spaces in %w */
		}
		else if (c != term && !(paren && c == paren)) {
		    tokadd('\\');
		}
	    }
	}
	else if (ismbchar(c)) {
	    int i, len = mbclen(c)-1;

	    for (i = 0; i < len; i++) {
		tokadd(c);
		c = nextc();
	    }
	}
	else if ((func & STR_FUNC_QWORDS) && ISSPACE(c)) {
	    pushback(c);
	    break;
	}
	if (!c && (func & STR_FUNC_SYMBOL)) {
	    func &= ~STR_FUNC_SYMBOL;
	    rb_compile_error("symbol cannot contain '\\0'");
	    continue;
	}
	tokadd(c);
    }
    return c;
}

#define NEW_STRTERM(func, term, paren) \
	rb_node_newnode(NODE_STRTERM, (func), (term) | ((paren) << (CHAR_BIT * 2)), 0)

static int
parse_string(quote)
    NODE *quote;
{
    int func = quote->nd_func;
    int term = nd_term(quote);
    int paren = nd_paren(quote);
    int c, space = 0;

    if (func == -1) return tSTRING_END;
    c = nextc();
    if ((func & STR_FUNC_QWORDS) && ISSPACE(c)) {
	do {c = nextc();} while (ISSPACE(c));
	space = 1;
    }
    if ((c == term && !quote->nd_nest) ||
	(c == '\\' && WHEN_QUOTED_TERM(peek(quoted_term_char)) &&
	 (c = nextc()) == term)) {
	if (func & STR_FUNC_QWORDS) {
	    quote->nd_func = -1;
	    return ' ';
	}
	if (!(func & STR_FUNC_REGEXP)) return tSTRING_END;
	yylval.num = regx_options();
	return tREGEXP_END;
    }
    if (space) {
	pushback(c);
	return ' ';
    }
    newtok();
    if ((func & STR_FUNC_EXPAND) && c == '#') {
	switch (c = nextc()) {
	  case '$':
	  case '@':
	    pushback(c);
	    return tSTRING_DVAR;
	  case '{':
	    return tSTRING_DBEG;
	}
	tokadd('#');
    }
    pushback(c);
    if (tokadd_string(func, term, paren, &quote->nd_nest) == -1) {
	ruby_sourceline = nd_line(quote);
	rb_compile_error("unterminated string meets end of file");
	return tSTRING_END;
    }

    tokfix();
    yylval.node = NEW_STR(rb_str_new(tok(), toklen()));
    return tSTRING_CONTENT;
}

static int
heredoc_identifier()
{
    int c = nextc(), term, func = 0, len;

    if (c == '-') {
	c = nextc();
	func = STR_FUNC_INDENT;
    }
    switch (c) {
      case '\'':
	func |= str_squote; goto quoted;
      case '"':
	func |= str_dquote; goto quoted;
      case '`':
	func |= str_xquote;
      quoted:
	newtok();
	tokadd(func);
	term = c;
	while ((c = nextc()) != -1 && c != term) {
	    len = mbclen(c);
	    do {tokadd(c);} while (--len > 0 && (c = nextc()) != -1);
	}
	if (c == -1) {
	    rb_compile_error("unterminated here document identifier");
	    return 0;
	}
	break;

      default:
	if (!is_identchar(c)) {
	    pushback(c);
	    if (func & STR_FUNC_INDENT) {
		pushback('-');
	    }
	    return 0;
	}
	newtok();
	term = '"';
	tokadd(func |= str_dquote);
	do {
	    len = mbclen(c);
	    do {tokadd(c);} while (--len > 0 && (c = nextc()) != -1);
	} while ((c = nextc()) != -1 && is_identchar(c));
	pushback(c);
	break;
    }

    tokfix();
    len = lex_p - lex_pbeg;
    lex_p = lex_pend;
    lex_strterm = rb_node_newnode(NODE_HEREDOC,
				  rb_str_new(tok(), toklen()),	/* nd_lit */
				  len,				/* nd_nth */
				  lex_lastline);		/* nd_orig */
    return term == '`' ? tXSTRING_BEG : tSTRING_BEG;
}

static void
heredoc_restore(here)
    NODE *here;
{
    VALUE line = here->nd_orig;
    lex_lastline = line;
    lex_pbeg = RSTRING(line)->ptr;
    lex_pend = lex_pbeg + RSTRING(line)->len;
    lex_p = lex_pbeg + here->nd_nth;
    heredoc_end = ruby_sourceline;
    ruby_sourceline = nd_line(here);
    dispose_string(here->nd_lit);
    rb_gc_force_recycle((VALUE)here);
}

static int
whole_match_p(eos, len, indent)
    char *eos;
    int len, indent;
{
    char *p = lex_pbeg;
    int n;

    if (indent) {
	while (*p && ISSPACE(*p)) p++;
    }
    n= lex_pend - (p + len);
    if (n < 0 || (n > 0 && p[len] != '\n' && p[len] != '\r')) return Qfalse;
    if (strncmp(eos, p, len) == 0) return Qtrue;
    return Qfalse;
}

static int
here_document(here)
    NODE *here;
{
    int c, func, indent = 0;
    char *eos, *p, *pend;
    long len;
    VALUE str = 0;

    eos = RSTRING(here->nd_lit)->ptr;
    len = RSTRING(here->nd_lit)->len - 1;
    indent = (func = *eos++) & STR_FUNC_INDENT;

    if ((c = nextc()) == -1) {
      error:
	rb_compile_error("can't find string \"%s\" anywhere before EOF", eos);
	heredoc_restore(lex_strterm);
	lex_strterm = 0;
	return 0;
    }
    if (was_bol() && whole_match_p(eos, len, indent)) {
	heredoc_restore(lex_strterm);
	return tSTRING_END;
    }

    if (!(func & STR_FUNC_EXPAND)) {
	do {
	    p = RSTRING(lex_lastline)->ptr;
	    pend = lex_pend;
	    if (pend > p) {
		switch (pend[-1]) {
		  case '\n':
		    if (--pend == p || pend[-1] != '\r') {
			pend++;
			break;
		    }
		  case '\r':
		    --pend;
		}
	    }
	    if (str)
		rb_str_cat(str, p, pend - p);
	    else
		str = rb_str_new(p, pend - p);
	    if (pend < lex_pend) rb_str_cat(str, "\n", 1);
	    lex_p = lex_pend;
	    if (nextc() == -1) {
		if (str) dispose_string(str);
		goto error;
	    }
	} while (!whole_match_p(eos, len, indent));
    }
    else {
	newtok();
	if (c == '#') {
	    switch (c = nextc()) {
	      case '$':
	      case '@':
		pushback(c);
		return tSTRING_DVAR;
	      case '{':
		return tSTRING_DBEG;
	    }
	    tokadd('#');
	}
	do {
	    pushback(c);
	    if ((c = tokadd_string(func, '\n', 0, NULL)) == -1) goto error;
	    if (c != '\n') {
		yylval.node = NEW_STR(rb_str_new(tok(), toklen()));
		return tSTRING_CONTENT;
	    }
	    tokadd(nextc());
	    if ((c = nextc()) == -1) goto error;
	} while (!whole_match_p(eos, len, indent));
	str = rb_str_new(tok(), toklen());
    }
    heredoc_restore(lex_strterm);
    lex_strterm = NEW_STRTERM(-1, 0, 0);
    yylval.node = NEW_STR(str);
    return tSTRING_CONTENT;
}

#include "lex.c"

static void
arg_ambiguous()
{
    rb_warning("ambiguous first argument; put parentheses or even spaces");
}

#define IS_ARG() (lex_state == EXPR_ARG || lex_state == EXPR_CMDARG)

static int
yylex()
{
    register int c;
    int space_seen = 0;
    int cmd_state;

    if (lex_strterm) {
	int token;
	if (nd_type(lex_strterm) == NODE_HEREDOC) {
	    token = here_document(lex_strterm);
	    if (token == tSTRING_END) {
		lex_strterm = 0;
		lex_state = EXPR_END;
	    }
	}
	else {
	    token = parse_string(lex_strterm);
	    if (token == tSTRING_END || token == tREGEXP_END) {
		rb_gc_force_recycle((VALUE)lex_strterm);
		lex_strterm = 0;
		lex_state = EXPR_END;
	    }
	}
	return token;
    }
    cmd_state = command_start;
    command_start = Qfalse;
  retry:
    switch (c = nextc()) {
      case '\0':		/* NUL */
      case '\004':		/* ^D */
      case '\032':		/* ^Z */
      case -1:			/* end of script. */
	return 0;

	/* white spaces */
      case ' ': case '\t': case '\f': case '\r':
      case '\13': /* '\v' */
	space_seen++;
	goto retry;

      case '#':		/* it's a comment */
	while ((c = nextc()) != '\n') {
	    if (c == -1)
		return 0;
	}
	/* fall through */
      case '\n':
	switch (lex_state) {
	  case EXPR_BEG:
	  case EXPR_FNAME:
	  case EXPR_DOT:
	  case EXPR_CLASS:
	    goto retry;
	  default:
	    break;
	}
	command_start = Qtrue;
	lex_state = EXPR_BEG;
	return '\n';

      case '*':
	if ((c = nextc()) == '*') {
	    if ((c = nextc()) == '=') {
		yylval.id = tPOW;
		lex_state = EXPR_BEG;
		return tOP_ASGN;
	    }
	    pushback(c);
	    c = tPOW;
	}
	else {
	    if (c == '=') {
		yylval.id = '*';
		lex_state = EXPR_BEG;
		return tOP_ASGN;
	    }
	    pushback(c);
	    if (IS_ARG() && space_seen && !ISSPACE(c)){
		rb_warning("`*' interpreted as argument prefix");
		c = tSTAR;
	    }
	    else if (lex_state == EXPR_BEG || lex_state == EXPR_MID) {
		c = tSTAR;
	    }
	    else {
		c = '*';
	    }
	}
	switch (lex_state) {
	  case EXPR_FNAME: case EXPR_DOT:
	    lex_state = EXPR_ARG; break;
	  default:
	    lex_state = EXPR_BEG; break;
	}
	return c;

      case '!':
	lex_state = EXPR_BEG;
	if ((c = nextc()) == '=') {
	    return tNEQ;
	}
	if (c == '~') {
	    return tNMATCH;
	}
	pushback(c);
	return '!';

      case '=':
	if (was_bol()) {
	    /* skip embedded rd document */
	    if (strncmp(lex_p, "begin", 5) == 0 && ISSPACE(lex_p[5])) {
		for (;;) {
		    lex_p = lex_pend;
		    c = nextc();
		    if (c == -1) {
			rb_compile_error("embedded document meets end of file");
			return 0;
		    }
		    if (c != '=') continue;
		    if (strncmp(lex_p, "end", 3) == 0 &&
			(lex_p + 3 == lex_pend || ISSPACE(lex_p[3]))) {
			break;
		    }
		}
		lex_p = lex_pend;
		goto retry;
	    }
	}

	switch (lex_state) {
	  case EXPR_FNAME: case EXPR_DOT:
	    lex_state = EXPR_ARG; break;
	  default:
	    lex_state = EXPR_BEG; break;
	}
	if ((c = nextc()) == '=') {
	    if ((c = nextc()) == '=') {
		return tEQQ;
	    }
	    pushback(c);
	    return tEQ;
	}
	if (c == '~') {
	    return tMATCH;
	}
	else if (c == '>') {
	    return tASSOC;
	}
	pushback(c);
	return '=';

      case '<':
	c = nextc();
	if (c == '<' &&
	    lex_state != EXPR_END &&
	    lex_state != EXPR_DOT &&
	    lex_state != EXPR_ENDARG && 
	    lex_state != EXPR_CLASS &&
	    (!IS_ARG() || space_seen)) {
	    int token = heredoc_identifier();
	    if (token) return token;
	}
	switch (lex_state) {
	  case EXPR_FNAME: case EXPR_DOT:
	    lex_state = EXPR_ARG; break;
	  default:
	    lex_state = EXPR_BEG; break;
	}
	if (c == '=') {
	    if ((c = nextc()) == '>') {
		return tCMP;
	    }
	    pushback(c);
	    return tLEQ;
	}
	if (c == '<') {
	    if ((c = nextc()) == '=') {
		yylval.id = tLSHFT;
		lex_state = EXPR_BEG;
		return tOP_ASGN;
	    }
	    pushback(c);
	    return tLSHFT;
	}
	pushback(c);
	return '<';

      case '>':
	switch (lex_state) {
	  case EXPR_FNAME: case EXPR_DOT:
	    lex_state = EXPR_ARG; break;
	  default:
	    lex_state = EXPR_BEG; break;
	}
	if ((c = nextc()) == '=') {
	    return tGEQ;
	}
	if (c == '>') {
	    if ((c = nextc()) == '=') {
		yylval.id = tRSHFT;
		lex_state = EXPR_BEG;
		return tOP_ASGN;
	    }
	    pushback(c);
	    return tRSHFT;
	}
	pushback(c);
	return '>';

      case '"':
	lex_strterm = NEW_STRTERM(str_dquote, '"', 0);
	return tSTRING_BEG;

      case '`':
	if (lex_state == EXPR_FNAME) {
	    lex_state = EXPR_END;
	    return c;
	}
	if (lex_state == EXPR_DOT) {
	    if (cmd_state)
		lex_state = EXPR_CMDARG;
	    else
		lex_state = EXPR_ARG;
	    return c;
	}
	lex_strterm = NEW_STRTERM(str_xquote, '`', 0);
	return tXSTRING_BEG;

      case '\'':
	lex_strterm = NEW_STRTERM(str_squote, '\'', 0);
	return tSTRING_BEG;

      case '?':
	if (lex_state == EXPR_END || lex_state == EXPR_ENDARG) {
	    lex_state = EXPR_BEG;
	    return '?';
	}
	c = nextc();
	if (c == -1) {
	    rb_compile_error("incomplete character syntax");
	    return 0;
	}
	if (ISSPACE(c)){
	    if (!IS_ARG()){
		int c2 = 0;
		switch (c) {
		  case ' ':
		    c2 = 's';
		    break;
		  case '\n':
		    c2 = 'n';
		    break;
		  case '\t':
		    c2 = 't';
		    break;
		  case '\v':
		    c2 = 'v';
		    break;
		  case '\r':
		    c2 = 'r';
		    break;
		  case '\f':
		    c2 = 'f';
		    break;
		}
		if (c2) {
		    rb_warn("invalid character syntax; use ?\\%c", c2);
		}
	    }
	  ternary:
	    pushback(c);
	    lex_state = EXPR_BEG;
	    return '?';
	}
	else if (ismbchar(c)) {
	    rb_warn("multibyte character literal not supported yet; use ?\\%.3o", c);
	    goto ternary;
	}
	else if ((ISALNUM(c) || c == '_') && lex_p < lex_pend && is_identchar(*lex_p)) {
	    goto ternary;
	}
	else if (c == '\\') {
	    c = read_escape();
	}
	c &= 0xff;
	lex_state = EXPR_END;
	yylval.node = NEW_LIT(INT2FIX(c));
	return tINTEGER;

      case '&':
	if ((c = nextc()) == '&') {
	    lex_state = EXPR_BEG;
	    if ((c = nextc()) == '=') {
		yylval.id = tANDOP;
		lex_state = EXPR_BEG;
		return tOP_ASGN;
	    }
	    pushback(c);
	    return tANDOP;
	}
	else if (c == '=') {
	    yylval.id = '&';
	    lex_state = EXPR_BEG;
	    return tOP_ASGN;
	}
	pushback(c);
	if (IS_ARG() && space_seen && !ISSPACE(c)){
	    rb_warning("`&' interpreted as argument prefix");
	    c = tAMPER;
	}
	else if (lex_state == EXPR_BEG || lex_state == EXPR_MID) {
	    c = tAMPER;
	}
	else {
	    c = '&';
	}
	switch (lex_state) {
	  case EXPR_FNAME: case EXPR_DOT:
	    lex_state = EXPR_ARG; break;
	  default:
	    lex_state = EXPR_BEG;
	}
	return c;

      case '|':
	if ((c = nextc()) == '|') {
	    lex_state = EXPR_BEG;
	    if ((c = nextc()) == '=') {
		yylval.id = tOROP;
		lex_state = EXPR_BEG;
		return tOP_ASGN;
	    }
	    pushback(c);
	    return tOROP;
	}
	if (c == '=') {
	    yylval.id = '|';
	    lex_state = EXPR_BEG;
	    return tOP_ASGN;
	}
	if (lex_state == EXPR_FNAME || lex_state == EXPR_DOT) {
	    lex_state = EXPR_ARG;
	}
	else {
	    lex_state = EXPR_BEG;
	}
	pushback(c);
	return '|';

      case '+':
	c = nextc();
	if (lex_state == EXPR_FNAME || lex_state == EXPR_DOT) {
	    lex_state = EXPR_ARG;
	    if (c == '@') {
		return tUPLUS;
	    }
	    pushback(c);
	    return '+';
	}
	if (c == '=') {
	    yylval.id = '+';
	    lex_state = EXPR_BEG;
	    return tOP_ASGN;
	}
	if (lex_state == EXPR_BEG || lex_state == EXPR_MID ||
	    (IS_ARG() && space_seen && !ISSPACE(c))) {
	    if (IS_ARG()) arg_ambiguous();
	    lex_state = EXPR_BEG;
	    pushback(c);
	    if (ISDIGIT(c)) {
		c = '+';
		goto start_num;
	    }
	    return tUPLUS;
	}
	lex_state = EXPR_BEG;
	pushback(c);
	return '+';

      case '-':
	c = nextc();
	if (lex_state == EXPR_FNAME || lex_state == EXPR_DOT) {
	    lex_state = EXPR_ARG;
	    if (c == '@') {
		return tUMINUS;
	    }
	    pushback(c);
	    return '-';
	}
	if (c == '=') {
	    yylval.id = '-';
	    lex_state = EXPR_BEG;
	    return tOP_ASGN;
	}
	if (lex_state == EXPR_BEG || lex_state == EXPR_MID ||
	    (IS_ARG() && space_seen && !ISSPACE(c))) {
	    if (IS_ARG()) arg_ambiguous();
	    lex_state = EXPR_BEG;
	    pushback(c);
	    if (ISDIGIT(c)) {
		return tUMINUS_NUM;
	    }
	    return tUMINUS;
	}
	lex_state = EXPR_BEG;
	pushback(c);
	return '-';

      case '.':
	lex_state = EXPR_BEG;
	if ((c = nextc()) == '.') {
	    if ((c = nextc()) == '.') {
		return tDOT3;
	    }
	    pushback(c);
	    return tDOT2;
	}
	pushback(c);
	if (ISDIGIT(c)) {
	    yyerror("no .<digit> floating literal anymore; put 0 before dot");
	}
	lex_state = EXPR_DOT;
	return '.';

      start_num:
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
	{
	    int is_float, seen_point, seen_e, nondigit;

	    is_float = seen_point = seen_e = nondigit = 0;
	    lex_state = EXPR_END;
	    newtok();
	    if (c == '-' || c == '+') {
		tokadd(c);
		c = nextc();
	    }
	    if (c == '0') {
		int start = toklen();
		c = nextc();
		if (c == 'x' || c == 'X') {
		    /* hexadecimal */
		    c = nextc();
		    if (ISXDIGIT(c)) {
			do {
			    if (c == '_') {
				if (nondigit) break;
				nondigit = c;
				continue;
			    }
			    if (!ISXDIGIT(c)) break;
			    nondigit = 0;
			    tokadd(c);
			} while ((c = nextc()) != -1);
		    }
		    pushback(c);
		    tokfix();
		    if (toklen() == start) {
			yyerror("numeric literal without digits");
		    }
		    else if (nondigit) goto trailing_uc;
		    yylval.node = NEW_LIT(rb_cstr_to_inum(tok(), 16, Qfalse));
		    return tINTEGER;
		}
		if (c == 'b' || c == 'B') {
		    /* binary */
		    c = nextc();
		    if (c == '0' || c == '1') {
			do {
			    if (c == '_') {
				if (nondigit) break;
				nondigit = c;
				continue;
			    }
			    if (c != '0' && c != '1') break;
			    nondigit = 0;
			    tokadd(c);
			} while ((c = nextc()) != -1);
		    }
		    pushback(c);
		    tokfix();
		    if (toklen() == start) {
			yyerror("numeric literal without digits");
		    }
		    else if (nondigit) goto trailing_uc;
		    yylval.node = NEW_LIT(rb_cstr_to_inum(tok(), 2, Qfalse));
		    return tINTEGER;
		}
		if (c == 'd' || c == 'D') {
		    /* decimal */
		    c = nextc();
		    if (ISDIGIT(c)) {
			do {
			    if (c == '_') {
				if (nondigit) break;
				nondigit = c;
				continue;
			    }
			    if (!ISDIGIT(c)) break;
			    nondigit = 0;
			    tokadd(c);
			} while ((c = nextc()) != -1);
		    }
		    pushback(c);
		    tokfix();
		    if (toklen() == start) {
			yyerror("numeric literal without digits");
		    }
		    else if (nondigit) goto trailing_uc;
		    yylval.node = NEW_LIT(rb_cstr_to_inum(tok(), 10, Qfalse));
		    return tINTEGER;
		}
		if (c == '_') {
		    /* 0_0 */
		    goto octal_number;
		}
		if (c == 'o' || c == 'O') {
		    /* prefixed octal */
		    c = nextc();
		    if (c == '_') {
			yyerror("numeric literal without digits");
		    }
		}
		if (c >= '0' && c <= '7') {
		    /* octal */
		  octal_number:
	            do {
			if (c == '_') {
			    if (nondigit) break;
			    nondigit = c;
			    continue;
			}
			if (c < '0' || c > '7') break;
			nondigit = 0;
			tokadd(c);
		    } while ((c = nextc()) != -1);
		    if (toklen() > start) {
			pushback(c);
			tokfix();
			if (nondigit) goto trailing_uc;
			yylval.node = NEW_LIT(rb_cstr_to_inum(tok(), 8, Qfalse));
			return tINTEGER;
		    }
		    if (nondigit) {
			pushback(c);
			goto trailing_uc;
		    }
		}
		if (c > '7' && c <= '9') {
		    yyerror("Illegal octal digit");
		}
		else if (c == '.' || c == 'e' || c == 'E') {
		    tokadd('0');
		}
		else {
		    pushback(c);
		    yylval.node = NEW_LIT(INT2FIX(0));
		    return tINTEGER;
		}
	    }

	    for (;;) {
		switch (c) {
		  case '0': case '1': case '2': case '3': case '4':
		  case '5': case '6': case '7': case '8': case '9':
		    nondigit = 0;
		    tokadd(c);
		    break;

		  case '.':
		    if (nondigit) goto trailing_uc;
		    if (seen_point || seen_e) {
			goto decode_num;
		    }
		    else {
			int c0 = nextc();
			if (!ISDIGIT(c0)) {
			    pushback(c0);
			    goto decode_num;
			}
			c = c0;
		    }
		    tokadd('.');
		    tokadd(c);
		    is_float++;
		    seen_point++;
		    nondigit = 0;
		    break;

		  case 'e':
		  case 'E':
		    if (nondigit) {
			pushback(c);
			c = nondigit;
			goto decode_num;
		    }
		    if (seen_e) {
			goto decode_num;
		    }
		    tokadd(c);
		    seen_e++;
		    is_float++;
		    nondigit = c;
		    c = nextc();
		    if (c != '-' && c != '+') continue;
		    tokadd(c);
		    nondigit = c;
		    break;

		  case '_':	/* `_' in number just ignored */
		    if (nondigit) goto decode_num;
		    nondigit = c;
		    break;

		  default:
		    goto decode_num;
		}
		c = nextc();
	    }

	  decode_num:
	    pushback(c);
	    tokfix();
	    if (nondigit) {
		char tmp[30];
	      trailing_uc:
		sprintf(tmp, "trailing `%c' in number", nondigit);
		yyerror(tmp);
	    }
	    if (is_float) {
		double d = strtod(tok(), 0);
		if (errno == ERANGE) {
		    rb_warn("Float %s out of range", tok());
		    errno = 0;
		}
		yylval.node = NEW_LIT(rb_float_new(d));
		return tFLOAT;
	    }
	    yylval.node = NEW_LIT(rb_cstr_to_inum(tok(), 10, Qfalse));
	    return tINTEGER;
	}

      case ']':
      case '}':
      case ')':
	COND_LEXPOP();
	CMDARG_LEXPOP();
	lex_state = EXPR_END;
	return c;

      case ':':
	c = nextc();
	if (c == ':') {
	    if (lex_state == EXPR_BEG ||  lex_state == EXPR_MID ||
		(IS_ARG() && space_seen)) {
		lex_state = EXPR_BEG;
		return tCOLON3;
	    }
	    lex_state = EXPR_DOT;
	    return tCOLON2;
	}
	if (lex_state == EXPR_END || lex_state == EXPR_ENDARG || ISSPACE(c)) {
	    pushback(c);
	    lex_state = EXPR_BEG;
	    return ':';
	}
	switch (c) {
	  case '\'':
	    lex_strterm = NEW_STRTERM(str_ssym, c, 0);
	    break;
	  case '"':
	    lex_strterm = NEW_STRTERM(str_dsym, c, 0);
	    break;
	  default:
	    pushback(c);
	    break;
	}
	lex_state = EXPR_FNAME;
	return tSYMBEG;

      case '/':
	if (lex_state == EXPR_BEG || lex_state == EXPR_MID) {
	    lex_strterm = NEW_STRTERM(str_regexp, '/', 0);
	    return tREGEXP_BEG;
	}
	if ((c = nextc()) == '=') {
	    yylval.id = '/';
	    lex_state = EXPR_BEG;
	    return tOP_ASGN;
	}
	pushback(c);
	if (IS_ARG() && space_seen) {
	    if (!ISSPACE(c)) {
		arg_ambiguous();
		lex_strterm = NEW_STRTERM(str_regexp, '/', 0);
		return tREGEXP_BEG;
	    }
	}
	switch (lex_state) {
	  case EXPR_FNAME: case EXPR_DOT:
	    lex_state = EXPR_ARG; break;
	  default:
	    lex_state = EXPR_BEG; break;
	}
	return '/';

      case '^':
	if ((c = nextc()) == '=') {
	    yylval.id = '^';
	    lex_state = EXPR_BEG;
	    return tOP_ASGN;
	}
	switch (lex_state) {
	  case EXPR_FNAME: case EXPR_DOT:
	    lex_state = EXPR_ARG; break;
	  default:
	    lex_state = EXPR_BEG; break;
	}
	pushback(c);
	return '^';

      case ';':
	command_start = Qtrue;
      case ',':
	lex_state = EXPR_BEG;
	return c;

      case '~':
	if (lex_state == EXPR_FNAME || lex_state == EXPR_DOT) {
	    if ((c = nextc()) != '@') {
		pushback(c);
	    }
	}
	switch (lex_state) {
	  case EXPR_FNAME: case EXPR_DOT:
	    lex_state = EXPR_ARG; break;
	  default:
	    lex_state = EXPR_BEG; break;
	}
	return '~';

      case '(':
	command_start = Qtrue;
	if (lex_state == EXPR_BEG || lex_state == EXPR_MID) {
	    c = tLPAREN;
	}
	else if (space_seen) {
	    if (lex_state == EXPR_CMDARG) {
		c = tLPAREN_ARG;
	    }
	    else if (lex_state == EXPR_ARG) {
		c = tLPAREN_ARG;
	    }
	}
	COND_PUSH(0);
	CMDARG_PUSH(0);
	lex_state = EXPR_BEG;
	return c;

      case '[':
	if (lex_state == EXPR_FNAME || lex_state == EXPR_DOT) {
	    lex_state = EXPR_ARG;
	    if ((c = nextc()) == ']') {
		if ((c = nextc()) == '=') {
		    return tASET;
		}
		pushback(c);
		return tAREF;
	    }
	    pushback(c);
	    return '[';
	}
	else if (lex_state == EXPR_BEG || lex_state == EXPR_MID) {
	    c = tLBRACK;
	}
	else if (IS_ARG() && space_seen) {
	    c = tLBRACK;
	}
	lex_state = EXPR_BEG;
	COND_PUSH(0);
	CMDARG_PUSH(0);
	return c;

      case '{':
	if (IS_ARG() || lex_state == EXPR_END)
	    c = '{';          /* block (primary) */
	else if (lex_state == EXPR_ENDARG)
	    c = tLBRACE_ARG;  /* block (expr) */
	else
	    c = tLBRACE;      /* hash */
	COND_PUSH(0);
	CMDARG_PUSH(0);
	lex_state = EXPR_BEG;
	return c;

      case '\\':
	c = nextc();
	if (c == '\n') {
	    space_seen = 1;
	    goto retry; /* skip \\n */
	}
	pushback(c);
	if (QUOTED_TERM_P(c)) {
	    if (!(quoted_term & ESCAPED_TERM)) {
		rb_warn("escaped terminator '%c' inside string interpolation", c);
		quoted_term |= ESCAPED_TERM;
	    }
	    goto retry;
	}
	return '\\';

      case '%':
	if (lex_state == EXPR_BEG || lex_state == EXPR_MID) {
	    int term;
	    int paren;

	    c = nextc();
	  quotation:
	    if (c == '\\' && WHEN_QUOTED_TERM(peek(quoted_term_char))) {
		c = nextc();
		if (!(quoted_term & ESCAPED_TERM)) {
		    rb_warn("escaped terminator '%s%c' inside string interpolation",
			    (c == '\'' ? "\\" : ""), c);
		    quoted_term |= ESCAPED_TERM;
		}
	    }
	    if (!ISALNUM(c)) {
		term = c;
		c = 'Q';
	    }
	    else {
		term = nextc();
		if (ISALNUM(term) || ismbchar(term)) {
		    yyerror("unknown type of %string");
		    return 0;
		}
	    }
	    if (c == -1 || term == -1) {
		rb_compile_error("unterminated quoted string meets end of file");
		return 0;
	    }
	    paren = term;
	    if (term == '(') term = ')';
	    else if (term == '[') term = ']';
	    else if (term == '{') term = '}';
	    else if (term == '<') term = '>';
	    else paren = 0;

	    switch (c) {
	      case 'Q':
		lex_strterm = NEW_STRTERM(str_dquote, term, paren);
		return tSTRING_BEG;

	      case 'q':
		lex_strterm = NEW_STRTERM(str_squote, term, paren);
		return tSTRING_BEG;

	      case 'W':
		lex_strterm = NEW_STRTERM(str_dquote | STR_FUNC_QWORDS, term, paren);
		do {c = nextc();} while (ISSPACE(c));
		pushback(c);
		return tWORDS_BEG;

	      case 'w':
		lex_strterm = NEW_STRTERM(str_squote | STR_FUNC_QWORDS, term, paren);
		do {c = nextc();} while (ISSPACE(c));
		pushback(c);
		return tQWORDS_BEG;

	      case 'x':
		lex_strterm = NEW_STRTERM(str_xquote, term, paren);
		return tXSTRING_BEG;

	      case 'r':
		lex_strterm = NEW_STRTERM(str_regexp, term, paren);
		return tREGEXP_BEG;

	      case 's':
		lex_strterm = NEW_STRTERM(str_ssym, term, paren);
		lex_state = EXPR_FNAME;
		return tSYMBEG;

	      default:
		yyerror("unknown type of %string");
		return 0;
	    }
	}
	if ((c = nextc()) == '=') {
	    yylval.id = '%';
	    lex_state = EXPR_BEG;
	    return tOP_ASGN;
	}
	if (IS_ARG() && space_seen && !ISSPACE(c)) {
	    goto quotation;
	}
	switch (lex_state) {
	  case EXPR_FNAME: case EXPR_DOT:
	    lex_state = EXPR_ARG; break;
	  default:
	    lex_state = EXPR_BEG; break;
	}
	pushback(c);
	return '%';

      case '$':
	lex_state = EXPR_END;
	newtok();
	c = nextc();
	switch (c) {
	  case '_':		/* $_: last read line string */
	    c = nextc();
	    if (is_identchar(c)) {
		tokadd('$');
		tokadd('_');
		break;
	    }
	    pushback(c);
	    c = '_';
	    /* fall through */
	  case '~':		/* $~: match-data */
	    local_cnt(c);
	    /* fall through */
	  case '*':		/* $*: argv */
	  case '$':		/* $$: pid */
	  case '?':		/* $?: last status */
	  case '!':		/* $!: error string */
	  case '@':		/* $@: error position */
	  case '/':		/* $/: input record separator */
	  case '\\':		/* $\: output record separator */
	  case ';':		/* $;: field separator */
	  case ',':		/* $,: output field separator */
	  case '.':		/* $.: last read line number */
	  case '=':		/* $=: ignorecase */
	  case ':':		/* $:: load path */
	  case '<':		/* $<: reading filename */
	  case '>':		/* $>: default output handle */
	  case '\"':		/* $": already loaded files */
	    tokadd('$');
	    tokadd(c);
	    tokfix();
	    yylval.id = rb_intern(tok());
	    return tGVAR;

	  case '-':
	    tokadd('$');
	    tokadd(c);
	    c = nextc();
	    tokadd(c);
	    tokfix();
	    yylval.id = rb_intern(tok());
	    /* xxx shouldn't check if valid option variable */
	    return tGVAR;

	  case '&':		/* $&: last match */
	  case '`':		/* $`: string before last match */
	  case '\'':		/* $': string after last match */
	  case '+':		/* $+: string matches last paren. */
	    yylval.node = NEW_BACK_REF(c);
	    return tBACK_REF;

	  case '1': case '2': case '3':
	  case '4': case '5': case '6':
	  case '7': case '8': case '9':
	    tokadd('$');
	    do {
		tokadd(c);
		c = nextc();
	    } while (ISDIGIT(c));
	    pushback(c);
	    tokfix();
	    yylval.node = NEW_NTH_REF(atoi(tok()+1));
	    return tNTH_REF;

	  default:
	    if (!is_identchar(c)) {
		pushback(c);
		return '$';
	    }
	  case '0':
	    tokadd('$');
	}
	break;

      case '@':
	c = nextc();
	newtok();
	tokadd('@');
	if (c == '@') {
	    tokadd('@');
	    c = nextc();
	}
	if (ISDIGIT(c)) {
	    if (tokidx == 1) {
		rb_compile_error("`@%c' is not allowed as an instance variable name", c);
	    }
	    else {
		rb_compile_error("`@@%c' is not allowed as a class variable name", c);
	    }
	}
	if (!is_identchar(c)) {
	    pushback(c);
	    return '@';
	}
	break;

      case '_':
	if (was_bol() && whole_match_p("__END__", 7, 0)) {
	    ruby__end__seen = 1;
	    lex_lastline = 0;
	    return -1;
	}
	newtok();
	break;

      default:
	if (!is_identchar(c)) {
	    rb_compile_error("Invalid char `\\%03o' in expression", c);
	    goto retry;
	}

	newtok();
	break;
    }

    do {
	tokadd(c);
	if (ismbchar(c)) {
	    int i, len = mbclen(c)-1;

	    for (i = 0; i < len; i++) {
		c = nextc();
		tokadd(c);
	    }
	}
	c = nextc();
    } while (is_identchar(c));
    if ((c == '!' || c == '?') && is_identchar(tok()[0]) && !peek('=')) {
	tokadd(c);
    }
    else {
	pushback(c);
    }
    tokfix();

    {
	int result = 0;

	switch (tok()[0]) {
	  case '$':
	    lex_state = EXPR_END;
	    result = tGVAR;
	    break;
	  case '@':
	    lex_state = EXPR_END;
	    if (tok()[1] == '@')
		result = tCVAR;
	    else
		result = tIVAR;
	    break;

	  default:
	    if (toklast() == '!' || toklast() == '?') {
		result = tFID;
	    }
	    else {
		if (lex_state == EXPR_FNAME) {
		    if ((c = nextc()) == '=' && !peek('~') && !peek('>') &&
			(!peek('=') || (lex_p + 1 < lex_pend && lex_p[1] == '>'))) {
			result = tIDENTIFIER;
			tokadd(c);
			tokfix();
		    }
		    else {
			pushback(c);
		    }
		}
		if (result == 0 && ISUPPER(tok()[0])) {
		    result = tCONSTANT;
		}
		else {
		    result = tIDENTIFIER;
		}
	    }

	    if (lex_state != EXPR_DOT) {
		struct kwtable *kw;

		/* See if it is a reserved word.  */
		kw = rb_reserved_word(tok(), toklen());
		if (kw) {
		    enum lex_state state = lex_state;
		    lex_state = kw->state;
		    if (state == EXPR_FNAME) {
			yylval.id = rb_intern(kw->name);
		    }
		    if (kw->id[0] == kDO) {
			if (COND_P()) return kDO_COND;
			if (CMDARG_P() && state != EXPR_CMDARG)
			    return kDO_BLOCK;
			if (state == EXPR_ENDARG)
			    return kDO_BLOCK;
			return kDO;
		    }
		    if (state == EXPR_BEG)
			return kw->id[0];
		    else {
			if (kw->id[0] != kw->id[1])
			    lex_state = EXPR_BEG;
			return kw->id[1];
		    }
		}
	    }

	    if (lex_state == EXPR_BEG ||
		lex_state == EXPR_MID ||
		lex_state == EXPR_DOT ||
		lex_state == EXPR_ARG ||
		lex_state == EXPR_CMDARG) {
		if (cmd_state) {
		    lex_state = EXPR_CMDARG;
		}
		else {
		    lex_state = EXPR_ARG;
		}
	    }
	    else {
		lex_state = EXPR_END;
	    }
	}
	yylval.id = rb_intern(tok());
	if (is_local_id(yylval.id) &&
	    ((dyna_in_block() && rb_dvar_defined(yylval.id)) || local_id(yylval.id))) {
	    lex_state = EXPR_END;
	}
	return result;
    }
}

NODE*
rb_node_newnode(type, a0, a1, a2)
    enum node_type type;
    VALUE a0, a1, a2;
{
    NODE *n = (NODE*)rb_newobj();

    n->flags |= T_NODE;
    nd_set_type(n, type);
    nd_set_line(n, ruby_sourceline);
    n->nd_file = ruby_sourcefile;

    n->u1.value = a0;
    n->u2.value = a1;
    n->u3.value = a2;

    return n;
}

static enum node_type
nodetype(node)			/* for debug */
    NODE *node;
{
    return (enum node_type)nd_type(node);
}

static int
nodeline(node)
    NODE *node;
{
    return nd_line(node);
}

static NODE*
newline_node(node)
    NODE *node;
{
    NODE *nl = 0;
    if (node) {
	if (nd_type(node) == NODE_NEWLINE) return node;
        nl = NEW_NEWLINE(node);
        fixpos(nl, node);
        nl->nd_nth = nd_line(node);
    }
    return nl;
}

static void
fixpos(node, orig)
    NODE *node, *orig;
{
    if (!node) return;
    if (!orig) return;
    if (orig == (NODE*)1) return;
    node->nd_file = orig->nd_file;
    nd_set_line(node, nd_line(orig));
}

static void
parser_warning(node, mesg)
    NODE *node;
    const char *mesg;
{
    int line = ruby_sourceline;
    ruby_sourceline = nd_line(node);
    rb_warning(mesg);
    ruby_sourceline = line;
}

static void
parser_warn(node, mesg)
    NODE *node;
    const char *mesg;
{
    int line = ruby_sourceline;
    ruby_sourceline = nd_line(node);
    rb_warn(mesg);
    ruby_sourceline = line;
}

static NODE*
block_append(head, tail)
    NODE *head, *tail;
{
    NODE *end, *h = head;

    if (tail == 0) return head;

  again:
    if (h == 0) return tail;
    switch (nd_type(h)) {
      case NODE_NEWLINE:
	h = h->nd_next;
	goto again;
      case NODE_LIT:
      case NODE_STR:
	parser_warning(h, "unused literal ignored");
	return tail;
      default:
	end = NEW_BLOCK(head);
	end->nd_end = end;
	fixpos(end, head);
	head = end;
	break;
      case NODE_BLOCK:
	end = h->nd_end;
	break;
    }

    if (RTEST(ruby_verbose)) {
	NODE *nd = end->nd_head;
      newline:
	switch (nd_type(nd)) {
	  case NODE_RETURN:
	  case NODE_BREAK:
	  case NODE_NEXT:
	  case NODE_REDO:
	  case NODE_RETRY:
	    parser_warning(nd, "statement not reached");
	    break;

	case NODE_NEWLINE:
	    nd = nd->nd_next;
	    goto newline;

	  default:
	    break;
	}
    }

    if (nd_type(tail) != NODE_BLOCK) {
	tail = NEW_BLOCK(tail);
	tail->nd_end = tail;
    }
    end->nd_next = tail;
    head->nd_end = tail->nd_end;
    return head;
}

/* append item to the list */
static NODE*
list_append(list, item)
    NODE *list, *item;
{
    NODE *last;

    if (list == 0) return NEW_LIST(item);
    if (list->nd_next) {
	last = list->nd_next->nd_end;
    }
    else {
	last = list;
    }

    list->nd_alen += 1;
    last->nd_next = NEW_LIST(item);
    list->nd_next->nd_end = last->nd_next;
    return list;
}

/* concat two lists */
static NODE*
list_concat(head, tail)
    NODE *head, *tail;
{
    NODE *last;

    if (head->nd_next) {
	last = head->nd_next->nd_end;
    }
    else {
	last = head;
    }

    head->nd_alen += tail->nd_alen;
    last->nd_next = tail;
    if (tail->nd_next) {
	head->nd_next->nd_end = tail->nd_next->nd_end;
    }
    else {
	head->nd_next->nd_end = tail;
    }

    return head;
}

/* concat two string literals */
static NODE *
literal_concat(head, tail)
    NODE *head, *tail;
{
    enum node_type htype;

    if (!head) return tail;
    if (!tail) return head;

    htype = nd_type(head);
    if (htype == NODE_EVSTR) {
	NODE *node = NEW_DSTR(rb_str_new(0, 0));
	head = list_append(node, head);
    }
    switch (nd_type(tail)) {
      case NODE_STR:
	if (htype == NODE_STR) {
	    rb_str_concat(head->nd_lit, tail->nd_lit);
	    rb_gc_force_recycle((VALUE)tail);
	}
	else {
	    list_append(head, tail);
	}
	break;

      case NODE_DSTR:
	if (htype == NODE_STR) {
	    rb_str_concat(head->nd_lit, tail->nd_lit);
	    tail->nd_lit = head->nd_lit;
	    rb_gc_force_recycle((VALUE)head);
	    head = tail;
	}
	else {
	    nd_set_type(tail, NODE_ARRAY);
	    tail->nd_head = NEW_STR(tail->nd_lit);
	    list_concat(head, tail);
	}
	break;

      case NODE_EVSTR:
	if (htype == NODE_STR) {
	    nd_set_type(head, NODE_DSTR);
	}
	list_append(head, tail);
	break;
    }
    return head;
}

static NODE *
evstr2dstr(node)
    NODE *node;
{
    if (nd_type(node) == NODE_EVSTR) {
	node = list_append(NEW_DSTR(rb_str_new(0, 0)), node);
    }
    return node;
}

static NODE *
new_evstr(node)
    NODE *node;
{
    NODE *head = node;

  again:
    if (node) {
	switch (nd_type(node)) {
	  case NODE_STR: case NODE_DSTR: case NODE_EVSTR:
	    return node;
	  case NODE_NEWLINE:
	    node = node->nd_next;
	    goto again;
	}
    }
    return NEW_EVSTR(head);
}

static NODE *
call_op(recv, id, narg, arg1)
    NODE *recv;
    ID id;
    int narg;
    NODE *arg1;
{
    value_expr(recv);
    if (narg == 1) {
	value_expr(arg1);
	arg1 = NEW_LIST(arg1);
    }
    else {
	arg1 = 0;
    }
    return NEW_CALL(recv, id, arg1);
}

static NODE*
match_gen(node1, node2)
    NODE *node1;
    NODE *node2;
{
    local_cnt('~');

    value_expr(node1);
    value_expr(node2);
    if (node1) {
	switch (nd_type(node1)) {
	  case NODE_DREGX:
	  case NODE_DREGX_ONCE:
	    return NEW_MATCH2(node1, node2);

	  case NODE_LIT:
	    if (TYPE(node1->nd_lit) == T_REGEXP) {
		return NEW_MATCH2(node1, node2);
	    }
	}
    }

    if (node2) {
	switch (nd_type(node2)) {
	  case NODE_DREGX:
	  case NODE_DREGX_ONCE:
	    return NEW_MATCH3(node2, node1);

	  case NODE_LIT:
	    if (TYPE(node2->nd_lit) == T_REGEXP) {
		return NEW_MATCH3(node2, node1);
	    }
	}
    }

    return NEW_CALL(node1, tMATCH, NEW_LIST(node2));
}

static NODE*
gettable(id)
    ID id;
{
    if (id == kSELF) {
	return NEW_SELF();
    }
    else if (id == kNIL) {
	return NEW_NIL();
    }
    else if (id == kTRUE) {
	return NEW_TRUE();
    }
    else if (id == kFALSE) {
	return NEW_FALSE();
    }
    else if (id == k__FILE__) {
	return NEW_STR(rb_str_new2(ruby_sourcefile));
    }
    else if (id == k__LINE__) {
	return NEW_LIT(INT2FIX(ruby_sourceline));
    }
    else if (is_local_id(id)) {
	if (dyna_in_block() && rb_dvar_defined(id)) return NEW_DVAR(id);
	if (local_id(id)) return NEW_LVAR(id);
	/* method call without arguments */
#if 0
	/* Rite will warn this */
	rb_warn("ambiguous identifier; %s() or self.%s is better for method call",
		rb_id2name(id), rb_id2name(id));
#endif
	return NEW_VCALL(id);
    }
    else if (is_global_id(id)) {
	return NEW_GVAR(id);
    }
    else if (is_instance_id(id)) {
	return NEW_IVAR(id);
    }
    else if (is_const_id(id)) {
	return NEW_CONST(id);
    }
    else if (is_class_id(id)) {
	return NEW_CVAR(id);
    }
    rb_compile_error("identifier %s is not valid", rb_id2name(id));
    return 0;
}

static NODE*
assignable(id, val)
    ID id;
    NODE *val;
{
    value_expr(val);
    if (id == kSELF) {
	yyerror("Can't change the value of self");
    }
    else if (id == kNIL) {
	yyerror("Can't assign to nil");
    }
    else if (id == kTRUE) {
	yyerror("Can't assign to true");
    }
    else if (id == kFALSE) {
	yyerror("Can't assign to false");
    }
    else if (id == k__FILE__) {
	yyerror("Can't assign to __FILE__");
    }
    else if (id == k__LINE__) {
	yyerror("Can't assign to __LINE__");
    }
    else if (is_local_id(id)) {
	if (rb_dvar_curr(id)) {
	    return NEW_DASGN_CURR(id, val);
	}
	else if (rb_dvar_defined(id)) {
	    return NEW_DASGN(id, val);
	}
	else if (local_id(id) || !dyna_in_block()) {
	    return NEW_LASGN(id, val);
	}
	else{
	    rb_dvar_push(id, Qnil);
	    return NEW_DASGN_CURR(id, val);
	}
    }
    else if (is_global_id(id)) {
	return NEW_GASGN(id, val);
    }
    else if (is_instance_id(id)) {
	return NEW_IASGN(id, val);
    }
    else if (is_const_id(id)) {
	if (in_def || in_single)
	    yyerror("dynamic constant assignment");
	return NEW_CDECL(id, val, 0);
    }
    else if (is_class_id(id)) {
	if (in_def || in_single) return NEW_CVASGN(id, val);
	return NEW_CVDECL(id, val);
    }
    else {
	rb_bug("bad id for variable");
    }
    return 0;
}

static NODE *
aryset(recv, idx)
    NODE *recv, *idx;
{
    if (recv && nd_type(recv) == NODE_SELF)
	recv = (NODE *)1;
    else
	value_expr(recv);
    return NEW_ATTRASGN(recv, tASET, idx);
}

ID
rb_id_attrset(id)
    ID id;
{
    id &= ~ID_SCOPE_MASK;
    id |= ID_ATTRSET;
    return id;
}

static NODE *
attrset(recv, id)
    NODE *recv;
    ID id;
{
    if (recv && nd_type(recv) == NODE_SELF)
	recv = (NODE *)1;
    else
	value_expr(recv);
    return NEW_ATTRASGN(recv, rb_id_attrset(id), 0);
}

static void
rb_backref_error(node)
    NODE *node;
{
    switch (nd_type(node)) {
      case NODE_NTH_REF:
	rb_compile_error("Can't set variable $%d", node->nd_nth);
	break;
      case NODE_BACK_REF:
	rb_compile_error("Can't set variable $%c", (int)node->nd_nth);
	break;
    }
}

static NODE *
arg_concat(node1, node2)
    NODE *node1;
    NODE *node2;
{
    if (!node2) return node1;
    return NEW_ARGSCAT(node1, node2);
}

static NODE *
arg_add(node1, node2)
    NODE *node1;
    NODE *node2;
{
    if (!node1) return NEW_LIST(node2);
    if (nd_type(node1) == NODE_ARRAY) {
	return list_append(node1, node2);
    }
    else {
	return NEW_ARGSPUSH(node1, node2);
    }
}

static NODE*
node_assign(lhs, rhs)
    NODE *lhs, *rhs;
{
    if (!lhs) return 0;

    value_expr(rhs);
    switch (nd_type(lhs)) {
      case NODE_GASGN:
      case NODE_IASGN:
      case NODE_LASGN:
      case NODE_DASGN:
      case NODE_DASGN_CURR:
      case NODE_MASGN:
      case NODE_CDECL:
      case NODE_CVDECL:
      case NODE_CVASGN:
	lhs->nd_value = rhs;
	break;

      case NODE_ATTRASGN:
      case NODE_CALL:
	lhs->nd_args = arg_add(lhs->nd_args, rhs);
	break;

      default:
	/* should not happen */
	break;
    }

    return lhs;
}

static int
value_expr0(node)
    NODE *node;
{
    int cond = 0;

    while (node) {
	switch (nd_type(node)) {
	  case NODE_DEFN:
	  case NODE_DEFS:
	    parser_warning(node, "void value expression");
	    return Qfalse;

	  case NODE_RETURN:
	  case NODE_BREAK:
	  case NODE_NEXT:
	  case NODE_REDO:
	  case NODE_RETRY:
	    if (!cond) yyerror("void value expression");
	    /* or "control never reach"? */
	    return Qfalse;

	  case NODE_BLOCK:
	    while (node->nd_next) {
		node = node->nd_next;
	    }
	    node = node->nd_head;
	    break;

	  case NODE_BEGIN:
	    node = node->nd_body;
	    break;

	  case NODE_IF:
	    if (!value_expr(node->nd_body)) return Qfalse;
	    node = node->nd_else;
	    break;

	  case NODE_AND:
	  case NODE_OR:
	    cond = 1;
	    node = node->nd_2nd;
	    break;

	  case NODE_NEWLINE:
	    node = node->nd_next;
	    break;

	  default:
	    return Qtrue;
	}
    }

    return Qtrue;
}

static void
void_expr0(node)
    NODE *node;
{
    char *useless = 0;

    if (!RTEST(ruby_verbose)) return;

  again:
    if (!node) return;
    switch (nd_type(node)) {
      case NODE_NEWLINE:
	node = node->nd_next;
	goto again;

      case NODE_CALL:
	switch (node->nd_mid) {
	  case '+':
	  case '-':
	  case '*':
	  case '/':
	  case '%':
	  case tPOW:
	  case tUPLUS:
	  case tUMINUS:
	  case '|':
	  case '^':
	  case '&':
	  case tCMP:
	  case '>':
	  case tGEQ:
	  case '<':
	  case tLEQ:
	  case tEQ:
	  case tNEQ:
	    useless = rb_id2name(node->nd_mid);
	    break;
	}
	break;

      case NODE_LVAR:
      case NODE_DVAR:
      case NODE_GVAR:
      case NODE_IVAR:
      case NODE_CVAR:
      case NODE_NTH_REF:
      case NODE_BACK_REF:
	useless = "a variable";
	break;
      case NODE_CONST:
      case NODE_CREF:
	useless = "a constant";
	break;
      case NODE_LIT:
      case NODE_STR:
      case NODE_DSTR:
      case NODE_DREGX:
      case NODE_DREGX_ONCE:
	useless = "a literal";
	break;
      case NODE_COLON2:
      case NODE_COLON3:
	useless = "::";
	break;
      case NODE_DOT2:
	useless = "..";
	break;
      case NODE_DOT3:
	useless = "...";
	break;
      case NODE_SELF:
	useless = "self";
	break;
      case NODE_NIL:
	useless = "nil";
	break;
      case NODE_TRUE:
	useless = "true";
	break;
      case NODE_FALSE:
	useless = "false";
	break;
      case NODE_DEFINED:
	useless = "defined?";
	break;
    }

    if (useless) {
	int line = ruby_sourceline;

	ruby_sourceline = nd_line(node);
	rb_warn("useless use of %s in void context", useless);
	ruby_sourceline = line;
    }
}

static void
void_stmts(node)
    NODE *node;
{
    if (!RTEST(ruby_verbose)) return;
    if (!node) return;
    if (nd_type(node) != NODE_BLOCK) return;

    for (;;) {
	if (!node->nd_next) return;
	void_expr(node->nd_head);
	node = node->nd_next;
    }
}

static NODE *
remove_begin(node)
    NODE *node;
{
    NODE **n = &node;
    while (*n) {
	switch (nd_type(*n)) {
	  case NODE_NEWLINE:
	    n = &(*n)->nd_next;
	    continue;
	  case NODE_BEGIN:
	    *n = (*n)->nd_body;
	  default:
	    return node;
	}
    }
    return node;
}

static int
assign_in_cond(node)
    NODE *node;
{
    switch (nd_type(node)) {
      case NODE_MASGN:
	yyerror("multiple assignment in conditional");
	return 1;

      case NODE_LASGN:
      case NODE_DASGN:
      case NODE_GASGN:
      case NODE_IASGN:
	break;

      case NODE_NEWLINE:
      default:
	return 0;
    }

    switch (nd_type(node->nd_value)) {
      case NODE_LIT:
      case NODE_STR:
      case NODE_NIL:
      case NODE_TRUE:
      case NODE_FALSE:
	/* reports always */
	parser_warn(node->nd_value, "found = in conditional, should be ==");
	return 1;

      case NODE_DSTR:
      case NODE_XSTR:
      case NODE_DXSTR:
      case NODE_EVSTR:
      case NODE_DREGX:
      default:
	break;
    }
#if 0
    if (assign_in_cond(node->nd_value) == 0) {
	parser_warning(node->nd_value, "assignment in condition");
    }
#endif
    return 1;
}

static int
e_option_supplied()
{
    if (strcmp(ruby_sourcefile, "-e") == 0)
	return Qtrue;
    return Qfalse;
}

static void
warn_unless_e_option(node, str)
    NODE *node;
    const char *str;
{
    if (!e_option_supplied()) parser_warn(node, str);
}

static void
warning_unless_e_option(node, str)
    NODE *node;
    const char *str;
{
    if (!e_option_supplied()) parser_warning(node, str);
}

static NODE *cond0();

static NODE*
range_op(node)
    NODE *node;
{
    enum node_type type;

    if (!e_option_supplied()) return node;
    if (node == 0) return 0;

    value_expr(node);
    node = cond0(node);
    type = nd_type(node);
    if (type == NODE_NEWLINE) {
	node = node->nd_next;
	type = nd_type(node);
    }
    if (type == NODE_LIT && FIXNUM_P(node->nd_lit)) {
	warn_unless_e_option(node, "integer literal in conditional range");
	return call_op(node,tEQ,1,NEW_GVAR(rb_intern("$.")));
    }
    return node;
}

static int
literal_node(node)
    NODE *node;
{
    if (!node) return 1;	/* same as NODE_NIL */
    switch (nd_type(node)) {
      case NODE_LIT:
      case NODE_STR:
      case NODE_DSTR:
      case NODE_EVSTR:
      case NODE_DREGX:
      case NODE_DREGX_ONCE:
      case NODE_DSYM:
	return 2;
      case NODE_TRUE:
      case NODE_FALSE:
      case NODE_NIL:
	return 1;
    }
    return 0;
}

static NODE*
cond0(node)
    NODE *node;
{
    enum node_type type = nd_type(node);

    assign_in_cond(node);

    switch (type) {
      case NODE_DSTR:
      case NODE_EVSTR:
      case NODE_STR:
	rb_warn("string literal in condition");
	break;

      case NODE_DREGX:
      case NODE_DREGX_ONCE:
	warning_unless_e_option(node, "regex literal in condition");
	local_cnt('_');
	local_cnt('~');
	return NEW_MATCH2(node, NEW_GVAR(rb_intern("$_")));

      case NODE_AND:
      case NODE_OR:
	node->nd_1st = cond0(node->nd_1st);
	node->nd_2nd = cond0(node->nd_2nd);
	break;

      case NODE_DOT2:
      case NODE_DOT3:
	node->nd_beg = range_op(node->nd_beg);
	node->nd_end = range_op(node->nd_end);
	if (type == NODE_DOT2) nd_set_type(node,NODE_FLIP2);
	else if (type == NODE_DOT3) nd_set_type(node, NODE_FLIP3);
	node->nd_cnt = local_append(internal_id());
	if (!e_option_supplied()) {
	    int b = literal_node(node->nd_beg);
	    int e = literal_node(node->nd_end);
	    if ((b == 1 && e == 1) || (b + e >= 2 && RTEST(ruby_verbose))) {
		parser_warn(node, "range literal in condition");
	    }
	}
	break;

      case NODE_DSYM:
	parser_warning(node, "literal in condition");
	break;

      case NODE_LIT:
	if (TYPE(node->nd_lit) == T_REGEXP) {
	    warn_unless_e_option(node, "regex literal in condition");
	    nd_set_type(node, NODE_MATCH);
	    local_cnt('_');
	    local_cnt('~');
	}
	else {
	    parser_warning(node, "literal in condition");
	}
      default:
	break;
    }
    return node;
}

static NODE*
cond(node)
    NODE *node;
{
    if (node == 0) return 0;
    value_expr(node);
    if (nd_type(node) == NODE_NEWLINE){
	node->nd_next = cond0(node->nd_next);
	return node;
    }
    return cond0(node);
}

static NODE*
logop(type, left, right)
    enum node_type type;
    NODE *left, *right;
{
    value_expr(left);
    if (nd_type(left) == type) {
	NODE *node = left, *second;
	while ((second = node->nd_2nd) != 0 && nd_type(second) == type) {
	    node = second;
	}
	node->nd_2nd = NEW_NODE(type, second, right, 0);
	return left;
    }
    return NEW_NODE(type, left, right, 0);
}

static int
cond_negative(nodep)
    NODE **nodep;
{
    NODE *c = *nodep;

    if (!c) return 0;
    switch (nd_type(c)) {
      case NODE_NOT:
	*nodep = c->nd_body;
	return 1;
      case NODE_NEWLINE:
	if (c->nd_next && nd_type(c->nd_next) == NODE_NOT) {
	    c->nd_next = c->nd_next->nd_body;
	    return 1;
	}
    }
    return 0;
}

static void
no_blockarg(node)
    NODE *node;
{
    if (node && nd_type(node) == NODE_BLOCK_PASS) {
	rb_compile_error("block argument should not be given");
    }
}

static NODE *
ret_args(node)
    NODE *node;
{
    if (node) {
	no_blockarg(node);
	if (nd_type(node) == NODE_ARRAY && node->nd_next == 0) {
	    node = node->nd_head;
	}
	if (nd_type(node) == NODE_RESTARY) {
	    nd_set_type(node, NODE_SPLAT);
	}
    }
    return node;
}

static NODE *
new_yield(node)
    NODE *node;
{
    long state = Qtrue;

    if (node) {
	no_blockarg(node);
	if (nd_type(node) == NODE_ARRAY && node->nd_next == 0) {
	    node = node->nd_head;
	    state = Qfalse;
	}
	if (nd_type(node) == NODE_RESTARY) {
	    nd_set_type(node, NODE_SPLAT);
	    state = Qfalse;
	}
    }
    else {
	state = Qfalse;
    }
    return NEW_YIELD(node, state);
}

static NODE*
negate_lit(node)
    NODE *node;
{
    switch (TYPE(node->nd_lit)) {
      case T_FIXNUM:
	node->nd_lit = LONG2FIX(-FIX2LONG(node->nd_lit));
	break;
      case T_BIGNUM:
	node->nd_lit = rb_funcall(node->nd_lit,tUMINUS,0,0);
	break;
      case T_FLOAT:
	RFLOAT(node->nd_lit)->value = -RFLOAT(node->nd_lit)->value;
	break;
      default:
	break;
    }
    return node;
}

static NODE *
arg_blk_pass(node1, node2)
    NODE *node1;
    NODE *node2;
{
    if (node2) {
	node2->nd_head = node1;
	return node2;
    }
    return node1;
}

static NODE*
arg_prepend(node1, node2)
    NODE *node1, *node2;
{
    switch (nodetype(node2)) {
      case NODE_ARRAY:
	return list_concat(NEW_LIST(node1), node2);

      case NODE_RESTARY:
      case NODE_RESTARY2:
	return arg_concat(node1, node2->nd_head);

      case NODE_BLOCK_PASS:
	node2->nd_body = arg_prepend(node1, node2->nd_body);
	return node2;

      default:
	rb_bug("unknown nodetype(%d) for arg_prepend", nodetype(node2));
    }
    return 0;			/* not reached */
}

static NODE*
new_call(r,m,a)
    NODE *r;
    ID m;
    NODE *a;
{
    if (a && nd_type(a) == NODE_BLOCK_PASS) {
	a->nd_iter = NEW_CALL(r,m,a->nd_head);
	return a;
    }
    return NEW_CALL(r,m,a);
}

static NODE*
new_fcall(m,a)
    ID m;
    NODE *a;
{
    if (a && nd_type(a) == NODE_BLOCK_PASS) {
	a->nd_iter = NEW_FCALL(m,a->nd_head);
	return a;
    }
    return NEW_FCALL(m,a);
}

static NODE*
new_super(a)
    NODE *a;
{
    if (a && nd_type(a) == NODE_BLOCK_PASS) {
	a->nd_iter = NEW_SUPER(a->nd_head);
	return a;
    }
    return NEW_SUPER(a);
}

static struct local_vars {
    ID *tbl;
    int nofree;
    int cnt;
    int dlev;
    struct RVarmap* dyna_vars;
    struct local_vars *prev;
} *lvtbl;

static void
local_push(top)
    int top;
{
    struct local_vars *local;

    local = ALLOC(struct local_vars);
    local->prev = lvtbl;
    local->nofree = 0;
    local->cnt = 0;
    local->tbl = 0;
    local->dlev = 0;
    local->dyna_vars = ruby_dyna_vars;
    lvtbl = local;
    if (!top) {
	/* preserve reference for GC, but link should be cut. */
	rb_dvar_push(0, (VALUE)ruby_dyna_vars);
	ruby_dyna_vars->next = 0;
    }
}

static void
local_pop()
{
    struct local_vars *local = lvtbl->prev;

    if (lvtbl->tbl) {
	if (!lvtbl->nofree) free(lvtbl->tbl);
	else lvtbl->tbl[0] = lvtbl->cnt;
    }
    ruby_dyna_vars = lvtbl->dyna_vars;
    free(lvtbl);
    lvtbl = local;
}

static ID*
local_tbl()
{
    lvtbl->nofree = 1;
    return lvtbl->tbl;
}

static int
local_append(id)
    ID id;
{
    if (lvtbl->tbl == 0) {
	lvtbl->tbl = ALLOC_N(ID, 4);
	lvtbl->tbl[0] = 0;
	lvtbl->tbl[1] = '_';
	lvtbl->tbl[2] = '~';
	lvtbl->cnt = 2;
	if (id == '_') return 0;
	if (id == '~') return 1;
    }
    else {
	REALLOC_N(lvtbl->tbl, ID, lvtbl->cnt+2);
    }

    lvtbl->tbl[lvtbl->cnt+1] = id;
    return lvtbl->cnt++;
}

static int
local_cnt(id)
    ID id;
{
    int cnt, max;

    if (id == 0) return lvtbl->cnt;

    for (cnt=1, max=lvtbl->cnt+1; cnt<max;cnt++) {
	if (lvtbl->tbl[cnt] == id) return cnt-1;
    }
    return local_append(id);
}

static int
local_id(id)
    ID id;
{
    int i, max;

    if (lvtbl == 0) return Qfalse;
    for (i=3, max=lvtbl->cnt+1; i<max; i++) {
	if (lvtbl->tbl[i] == id) return Qtrue;
    }
    return Qfalse;
}

static void
top_local_init()
{
    local_push(1);
    lvtbl->cnt = ruby_scope->local_tbl?ruby_scope->local_tbl[0]:0;
    if (lvtbl->cnt > 0) {
	lvtbl->tbl = ALLOC_N(ID, lvtbl->cnt+3);
	MEMCPY(lvtbl->tbl, ruby_scope->local_tbl, ID, lvtbl->cnt+1);
    }
    else {
	lvtbl->tbl = 0;
    }
    if (ruby_dyna_vars)
	lvtbl->dlev = 1;
    else
	lvtbl->dlev = 0;
}

static void
top_local_setup()
{
    int len = lvtbl->cnt;
    int i;

    if (len > 0) {
	i = ruby_scope->local_tbl?ruby_scope->local_tbl[0]:0;

	if (i < len) {
	    if (i == 0 || (ruby_scope->flags & SCOPE_MALLOC) == 0) {
		VALUE *vars = ALLOC_N(VALUE, len+1);
		if (ruby_scope->local_vars) {
		    *vars++ = ruby_scope->local_vars[-1];
		    MEMCPY(vars, ruby_scope->local_vars, VALUE, i);
		    rb_mem_clear(vars+i, len-i);
		}
		else {
		    *vars++ = 0;
		    rb_mem_clear(vars, len);
		}
		ruby_scope->local_vars = vars;
		ruby_scope->flags |= SCOPE_MALLOC;
	    }
	    else {
		VALUE *vars = ruby_scope->local_vars-1;
		REALLOC_N(vars, VALUE, len+1);
		ruby_scope->local_vars = vars+1;
		rb_mem_clear(ruby_scope->local_vars+i, len-i);
	    }
	    if (ruby_scope->local_tbl && ruby_scope->local_vars[-1] == 0) {
		free(ruby_scope->local_tbl);
	    }
	    ruby_scope->local_vars[-1] = 0;
	    ruby_scope->local_tbl = local_tbl();
	}
    }
    local_pop();
}

static struct RVarmap*
dyna_push()
{
    struct RVarmap* vars = ruby_dyna_vars;

    rb_dvar_push(0, 0);
    lvtbl->dlev++;
    return vars;
}

static void
dyna_pop(vars)
    struct RVarmap* vars;
{
    lvtbl->dlev--;
    ruby_dyna_vars = vars;
}

static int
dyna_in_block()
{
    return (lvtbl->dlev > 0);
}

static NODE *
dyna_init(node, pre)
    NODE *node;
    struct RVarmap *pre;
{
    struct RVarmap *post = ruby_dyna_vars;
    NODE *var;

    if (!node || !post || pre == post) return node;
    for (var = 0; post != pre && post->id; post = post->next) {
	var = NEW_DASGN_CURR(post->id, var);
    }
    return block_append(var, node);
}

int
ruby_parser_stack_on_heap()
{
#if defined(YYBISON) && !defined(C_ALLOCA)
    return Qfalse;
#else
    return Qtrue;
#endif
}

void
rb_gc_mark_parser()
{
    if (!ruby_in_compile) return;

    rb_gc_mark_maybe((VALUE)yylval.node);
    rb_gc_mark(ruby_debug_lines);
    rb_gc_mark(lex_lastline);
    rb_gc_mark(lex_input);
    rb_gc_mark((VALUE)lex_strterm);
}

void
rb_parser_append_print()
{
    ruby_eval_tree =
	block_append(ruby_eval_tree,
		     NEW_FCALL(rb_intern("print"),
			       NEW_ARRAY(NEW_GVAR(rb_intern("$_")))));
}

void
rb_parser_while_loop(chop, split)
    int chop, split;
{
    if (split) {
	ruby_eval_tree =
	    block_append(NEW_GASGN(rb_intern("$F"),
				   NEW_CALL(NEW_GVAR(rb_intern("$_")),
					    rb_intern("split"), 0)),
				   ruby_eval_tree);
    }
    if (chop) {
	ruby_eval_tree =
	    block_append(NEW_CALL(NEW_GVAR(rb_intern("$_")),
				  rb_intern("chop!"), 0), ruby_eval_tree);
    }
    ruby_eval_tree = NEW_OPT_N(ruby_eval_tree);
}

static struct {
    ID token;
    char *name;
} op_tbl[] = {
    {tDOT2,	".."},
    {tDOT3,	"..."},
    {'+',	"+"},
    {'-',	"-"},
    {'+',	"+(binary)"},
    {'-',	"-(binary)"},
    {'*',	"*"},
    {'/',	"/"},
    {'%',	"%"},
    {tPOW,	"**"},
    {tUPLUS,	"+@"},
    {tUMINUS,	"-@"},
    {tUPLUS,	"+(unary)"},
    {tUMINUS,	"-(unary)"},
    {'|',	"|"},
    {'^',	"^"},
    {'&',	"&"},
    {tCMP,	"<=>"},
    {'>',	">"},
    {tGEQ,	">="},
    {'<',	"<"},
    {tLEQ,	"<="},
    {tEQ,	"=="},
    {tEQQ,	"==="},
    {tNEQ,	"!="},
    {tMATCH,	"=~"},
    {tNMATCH,	"!~"},
    {'!',	"!"},
    {'~',	"~"},
    {'!',	"!(unary)"},
    {'~',	"~(unary)"},
    {'!',	"!@"},
    {'~',	"~@"},
    {tAREF,	"[]"},
    {tASET,	"[]="},
    {tLSHFT,	"<<"},
    {tRSHFT,	">>"},
    {tCOLON2,	"::"},
    {'`',	"`"},
    {0,	0}
};

static st_table *sym_tbl;
static st_table *sym_rev_tbl;

void
Init_sym()
{
    sym_tbl = st_init_strtable_with_size(200);
    sym_rev_tbl = st_init_numtable_with_size(200);
}

static ID last_id = tLAST_TOKEN;

static ID
internal_id()
{
    return ID_INTERNAL | (++last_id << ID_SCOPE_SHIFT);
}

ID
rb_intern(name)
    const char *name;
{
    const char *m = name;
    ID id;
    int last;

    if (st_lookup(sym_tbl, (st_data_t)name, (st_data_t *)&id))
	return id;

    last = strlen(name)-1;
    id = 0;
    switch (*name) {
      case '$':
	id |= ID_GLOBAL;
	m++;
	if (!is_identchar(*m)) m++;
	break;
      case '@':
	if (name[1] == '@') {
	    m++;
	    id |= ID_CLASS;
	}
	else {
	    id |= ID_INSTANCE;
	}
	m++;
	break;
      default:
	if (name[0] != '_' && !ISALPHA(name[0]) && !ismbchar(name[0])) {
	    /* operators */
	    int i;

	    for (i=0; op_tbl[i].token; i++) {
		if (*op_tbl[i].name == *name &&
		    strcmp(op_tbl[i].name, name) == 0) {
		    id = op_tbl[i].token;
		    goto id_regist;
		}
	    }
	}

	if (name[last] == '=') {
	    /* attribute assignment */
	    char *buf = ALLOCA_N(char,last+1);

	    strncpy(buf, name, last);
	    buf[last] = '\0';
	    id = rb_intern(buf);
	    if (id > tLAST_TOKEN && !is_attrset_id(id)) {
		id = rb_id_attrset(id);
		goto id_regist;
	    }
	    id = ID_ATTRSET;
	}
	else if (ISUPPER(name[0])) {
	    id = ID_CONST;
        }
	else {
	    id = ID_LOCAL;
	}
	break;
    }
    while (m <= name + last && is_identchar(*m)) {
	m += mbclen(*m);
    }
    if (*m) id = ID_JUNK;
    id |= ++last_id << ID_SCOPE_SHIFT;
  id_regist:
    name = strdup(name);
    st_add_direct(sym_tbl, (st_data_t)name, id);
    st_add_direct(sym_rev_tbl, id, (st_data_t)name);
    return id;
}

char *
rb_id2name(id)
    ID id;
{
    char *name;

    if (id < tLAST_TOKEN) {
	int i = 0;

	for (i=0; op_tbl[i].token; i++) {
	    if (op_tbl[i].token == id)
		return op_tbl[i].name;
	}
    }

    if (st_lookup(sym_rev_tbl, id, (st_data_t *)&name))
	return name;

    if (is_attrset_id(id)) {
	ID id2 = (id & ~ID_SCOPE_MASK) | ID_LOCAL;

      again:
	name = rb_id2name(id2);
	if (name) {
	    char *buf = ALLOCA_N(char, strlen(name)+2);

	    strcpy(buf, name);
	    strcat(buf, "=");
	    rb_intern(buf);
	    return rb_id2name(id);
	}
	if (is_local_id(id2)) {
	    id2 = (id & ~ID_SCOPE_MASK) | ID_CONST;
	    goto again;
	}
    }
    return 0;
}

static int
symbols_i(key, value, ary)
    char *key;
    ID value;
    VALUE ary;
{
    rb_ary_push(ary, ID2SYM(value));
    return ST_CONTINUE;
}

VALUE
rb_sym_all_symbols()
{
    VALUE ary = rb_ary_new2(sym_tbl->num_entries);

    st_foreach(sym_tbl, symbols_i, ary);
    return ary;
}

int
rb_is_const_id(id)
    ID id;
{
    if (is_const_id(id)) return Qtrue;
    return Qfalse;
}

int
rb_is_class_id(id)
    ID id;
{
    if (is_class_id(id)) return Qtrue;
    return Qfalse;
}

int
rb_is_instance_id(id)
    ID id;
{
    if (is_instance_id(id)) return Qtrue;
    return Qfalse;
}

int
rb_is_local_id(id)
    ID id;
{
    if (is_local_id(id)) return Qtrue;
    return Qfalse;
}

int
rb_is_junk_id(id)
    ID id;
{
    if (is_junk_id(id)) return Qtrue;
    return Qfalse;
}

static void
special_local_set(c, val)
    char c;
    VALUE val;
{
    int cnt;

    top_local_init();
    cnt = local_cnt(c);
    top_local_setup();
    ruby_scope->local_vars[cnt] = val;
}

VALUE
rb_backref_get()
{
    VALUE *var = rb_svar(1);
    if (var) {
	return *var;
    }
    return Qnil;
}

void
rb_backref_set(val)
    VALUE val;
{
    VALUE *var = rb_svar(1);
    if (var) {
	*var = val;
    }
    else {
	special_local_set('~', val);
    }
}

VALUE
rb_lastline_get()
{
    VALUE *var = rb_svar(0);
    if (var) {
	return *var;
    }
    return Qnil;
}

void
rb_lastline_set(val)
    VALUE val;
{
    VALUE *var = rb_svar(0);
    if (var) {
	*var = val;
    }
    else {
	special_local_set('_', val);
    }
}


