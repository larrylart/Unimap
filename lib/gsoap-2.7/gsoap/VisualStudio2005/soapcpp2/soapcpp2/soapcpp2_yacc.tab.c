
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
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
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 64 "soapcpp2_yacc.y"


#include "soapcpp2.h"

#ifdef WIN32
#ifndef __STDC__
#define __STDC__
#endif
#define YYINCLUDED_STDLIB_H
#ifdef WIN32_WITHOUT_SOLARIS_FLEX
extern int soapcpp2lex();
#else
extern int yylex();
#endif
#else
extern int yylex();
#endif

extern int is_XML(Tnode*);

#define MAXNEST 16	/* max. nesting depth of scopes */

struct Scope
{	Table	*table;
	Entry	*entry;
	Node	node;
	LONG64	val;
	int	offset;
	Bool	grow;	/* true if offset grows with declarations */
	Bool	mask;	/* true if enum is mask */
}	stack[MAXNEST],	/* stack of tables and offsets */
	*sp;		/* current scope stack pointer */

Table	*classtable = (Table*)0,
	*enumtable = (Table*)0,
	*typetable = (Table*)0,
	*booltable = (Table*)0,
	*templatetable = (Table*)0;

char	*namespaceid = NULL;
int	transient = 0;
int	permission = 0;
int	custom_header = 1;
int	custom_fault = 1;
Pragma	*pragmas = NULL;
Tnode	*qname = NULL;
Tnode	*xml = NULL;

/* function prototypes for support routine section */
static Entry	*undefined(Symbol*);
static Tnode	*mgtype(Tnode*, Tnode*);
static Node	op(const char*, Node, Node), iop(const char*, Node, Node), relop(const char*, Node, Node);
static void	mkscope(Table*, int), enterscope(Table*, int), exitscope();
static int	integer(Tnode*), real(Tnode*), numeric(Tnode*);
static void	add_soap(), add_XML(), add_qname(), add_header(Table*), add_fault(Table*), add_response(Entry*, Entry*), add_result(Tnode*);
extern char	*c_storage(Storage), *c_type(Tnode*), *c_ident(Tnode*);
extern int	is_primitive_or_string(Tnode*), is_stdstr(Tnode*), is_binary(Tnode*), is_external(Tnode*), is_mutable(Tnode*);

/* Temporaries used in semantic rules */
int	i;
char	*s, *s1, *s2;
Symbol	*sym;
Entry	*p, *q;
Tnode	*t;
Node	tmp, c;
Pragma	**pp;



/* Line 189 of yacc.c  */
#line 143 "soapcpp2_yacc.tab.c"

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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     PRAGMA = 258,
     AUTO = 259,
     DOUBLE = 260,
     INT = 261,
     STRUCT = 262,
     BREAK = 263,
     ELSE = 264,
     LONG = 265,
     SWITCH = 266,
     CASE = 267,
     ENUM = 268,
     REGISTER = 269,
     TYPEDEF = 270,
     CHAR = 271,
     EXTERN = 272,
     RETURN = 273,
     UNION = 274,
     CONST = 275,
     FLOAT = 276,
     SHORT = 277,
     UNSIGNED = 278,
     CONTINUE = 279,
     FOR = 280,
     SIGNED = 281,
     VOID = 282,
     DEFAULT = 283,
     GOTO = 284,
     SIZEOF = 285,
     VOLATILE = 286,
     DO = 287,
     IF = 288,
     STATIC = 289,
     WHILE = 290,
     CLASS = 291,
     PRIVATE = 292,
     PROTECTED = 293,
     PUBLIC = 294,
     VIRTUAL = 295,
     INLINE = 296,
     OPERATOR = 297,
     LLONG = 298,
     BOOL = 299,
     CFALSE = 300,
     CTRUE = 301,
     WCHAR = 302,
     TIME = 303,
     USING = 304,
     NAMESPACE = 305,
     ULLONG = 306,
     MUSTUNDERSTAND = 307,
     SIZE = 308,
     FRIEND = 309,
     TEMPLATE = 310,
     EXPLICIT = 311,
     TYPENAME = 312,
     RESTRICT = 313,
     null = 314,
     NONE = 315,
     ID = 316,
     LAB = 317,
     TYPE = 318,
     LNG = 319,
     DBL = 320,
     CHR = 321,
     STR = 322,
     RA = 323,
     LA = 324,
     OA = 325,
     XA = 326,
     AA = 327,
     MA = 328,
     DA = 329,
     TA = 330,
     NA = 331,
     PA = 332,
     OR = 333,
     AN = 334,
     NE = 335,
     EQ = 336,
     GE = 337,
     LE = 338,
     RS = 339,
     LS = 340,
     AR = 341,
     PP = 342,
     NN = 343
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 138 "soapcpp2_yacc.y"
	Symbol	*sym;
	LONG64	i;
	double	r;
	char	c;
	char	*s;
	Tnode	*typ;
	Storage	sto;
	Node	rec;
	Entry	*e;



/* Line 214 of yacc.c  */
#line 280 "soapcpp2_yacc.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 292 "soapcpp2_yacc.tab.c"

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
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
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
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
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
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1010

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  114
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  61
/* YYNRULES -- Number of rules.  */
#define YYNRULES  230
/* YYNRULES -- Number of states.  */
#define YYNSTATES  358

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   343

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   108,     2,     2,   113,    99,    86,     2,
     110,   111,    97,    95,    68,    96,     2,    98,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    81,   105,
      89,    69,    90,    80,   112,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   106,     2,   107,    85,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   103,    84,   104,   109,     2,     2,     2,
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
      65,    66,    67,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    82,    83,    87,    88,    91,    92,    93,
      94,   100,   101,   102
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     6,     7,    13,    15,    16,    19,    22,
      24,    27,    29,    31,    33,    34,    38,    43,    48,    53,
      58,    60,    62,    63,    64,    65,    67,    70,    74,    77,
      80,    84,    89,    95,    98,   100,   102,   104,   107,   110,
     113,   116,   119,   122,   125,   128,   131,   134,   137,   140,
     143,   146,   149,   152,   155,   158,   161,   164,   167,   170,
     173,   176,   179,   182,   185,   188,   191,   194,   197,   200,
     203,   206,   210,   214,   217,   219,   223,   231,   232,   233,
     235,   237,   241,   248,   249,   251,   255,   260,   261,   264,
     267,   269,   271,   274,   277,   279,   281,   283,   285,   287,
     289,   291,   293,   295,   297,   299,   301,   303,   305,   307,
     315,   321,   327,   335,   337,   341,   347,   353,   356,   359,
     365,   372,   375,   378,   385,   392,   401,   404,   407,   409,
     414,   417,   420,   423,   425,   427,   430,   433,   436,   438,
     441,   442,   443,   444,   445,   447,   448,   450,   452,   454,
     456,   458,   460,   462,   464,   466,   468,   470,   472,   474,
     476,   478,   479,   481,   482,   485,   486,   488,   489,   492,
     495,   496,   501,   505,   507,   508,   511,   513,   516,   520,
     525,   529,   530,   532,   534,   537,   540,   544,   546,   552,
     554,   556,   560,   562,   564,   568,   570,   572,   576,   580,
     584,   588,   592,   596,   600,   604,   608,   612,   616,   620,
     624,   628,   632,   636,   638,   641,   644,   647,   650,   653,
     656,   661,   663,   667,   669,   671,   673,   675,   677,   679,
     681
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     115,     0,    -1,   116,   117,    -1,    -1,    50,    61,   103,
     118,   104,    -1,   118,    -1,    -1,   118,   119,    -1,   127,
     105,    -1,   120,    -1,     1,   105,    -1,   122,    -1,   123,
      -1,     3,    -1,    -1,   127,   105,   121,    -1,    37,    81,
     124,   121,    -1,    38,    81,   125,   121,    -1,    39,    81,
     126,   121,    -1,   122,   121,   123,   121,    -1,   106,    -1,
     107,    -1,    -1,    -1,    -1,   141,    -1,   141,   128,    -1,
     141,   129,   134,    -1,   132,   134,    -1,   133,   134,    -1,
     127,    68,   128,    -1,   127,    68,   129,   134,    -1,   158,
      61,   160,   162,   161,    -1,   158,   131,    -1,    61,    -1,
      63,    -1,    61,    -1,    42,   108,    -1,    42,   109,    -1,
      42,    69,    -1,    42,    79,    -1,    42,    78,    -1,    42,
      77,    -1,    42,    76,    -1,    42,    75,    -1,    42,    74,
      -1,    42,    73,    -1,    42,    72,    -1,    42,    71,    -1,
      42,    70,    -1,    42,    82,    -1,    42,    83,    -1,    42,
      84,    -1,    42,    85,    -1,    42,    86,    -1,    42,    88,
      -1,    42,    87,    -1,    42,    89,    -1,    42,    92,    -1,
      42,    90,    -1,    42,    91,    -1,    42,    94,    -1,    42,
      93,    -1,    42,    95,    -1,    42,    96,    -1,    42,    97,
      -1,    42,    98,    -1,    42,    99,    -1,    42,   101,    -1,
      42,   102,    -1,    42,   100,    -1,    42,   106,   107,    -1,
      42,   110,   111,    -1,    42,   140,    -1,    63,    -1,   157,
     109,    63,    -1,   135,   110,   153,   136,   111,   155,   156,
      -1,    -1,    -1,   137,    -1,   138,    -1,   138,    68,   137,
      -1,   142,   158,   139,   160,   162,   161,    -1,    -1,    61,
      -1,   142,   158,   159,    -1,   142,   158,    61,   159,    -1,
      -1,   154,   141,    -1,   143,   141,    -1,   154,    -1,   143,
      -1,   154,   142,    -1,   143,   142,    -1,    27,    -1,    44,
      -1,    16,    -1,    47,    -1,    22,    -1,     6,    -1,    10,
      -1,    43,    -1,    51,    -1,    53,    -1,    21,    -1,     5,
      -1,    26,    -1,    23,    -1,    48,    -1,    55,    89,   147,
     130,    90,    36,   130,    -1,    36,   103,   149,   121,   104,
      -1,   145,   103,   149,   121,   104,    -1,   145,    81,   148,
     103,   149,   121,   104,    -1,   145,    -1,   145,    81,   148,
      -1,     7,   103,   149,   121,   104,    -1,   144,   103,   149,
     121,   104,    -1,     7,    61,    -1,     7,    63,    -1,    19,
     103,   150,   121,   104,    -1,    19,   130,   103,   150,   121,
     104,    -1,    19,    61,    -1,    19,    63,    -1,    13,   103,
     149,   127,   152,   104,    -1,   146,   103,   149,   127,   152,
     104,    -1,    13,    97,   130,   103,   151,   127,   152,   104,
      -1,    13,    61,    -1,    13,    63,    -1,    63,    -1,    63,
      89,   140,    90,    -1,     7,   130,    -1,    36,   130,    -1,
      13,   130,    -1,    36,    -1,    57,    -1,    38,   148,    -1,
      37,   148,    -1,    39,   148,    -1,    63,    -1,     7,    61,
      -1,    -1,    -1,    -1,    -1,    68,    -1,    -1,     4,    -1,
      14,    -1,    34,    -1,    56,    -1,    17,    -1,    15,    -1,
      40,    -1,    20,    -1,    54,    -1,    41,    -1,    52,    -1,
      18,    -1,   112,    -1,   113,    -1,    31,    -1,    -1,    20,
      -1,    -1,    69,    64,    -1,    -1,    40,    -1,    -1,   158,
      97,    -1,   158,    86,    -1,    -1,   106,   166,   107,   159,
      -1,   106,   107,   159,    -1,   159,    -1,    -1,    69,   166,
      -1,   163,    -1,   163,   164,    -1,   163,   164,    81,    -1,
     163,   164,    81,   164,    -1,   163,    81,   164,    -1,    -1,
      67,    -1,    64,    -1,    95,    64,    -1,    96,    64,    -1,
     165,    68,   165,    -1,   166,    -1,   169,    80,   167,    81,
     166,    -1,   168,    -1,   165,    -1,   169,    82,   170,    -1,
     170,    -1,   168,    -1,   171,    83,   172,    -1,   172,    -1,
     170,    -1,   172,    84,   172,    -1,   172,    85,   172,    -1,
     172,    86,   172,    -1,   172,    88,   172,    -1,   172,    87,
     172,    -1,   172,    89,   172,    -1,   172,    92,   172,    -1,
     172,    90,   172,    -1,   172,    91,   172,    -1,   172,    94,
     172,    -1,   172,    93,   172,    -1,   172,    95,   172,    -1,
     172,    96,   172,    -1,   172,    97,   172,    -1,   172,    98,
     172,    -1,   172,    99,   172,    -1,   173,    -1,   108,   173,
      -1,   109,   173,    -1,    96,   173,    -1,    95,   173,    -1,
      97,   173,    -1,    86,   173,    -1,    30,   110,   140,   111,
      -1,   174,    -1,   110,   165,   111,    -1,    61,    -1,    64,
      -1,    59,    -1,    65,    -1,    66,    -1,    67,    -1,    45,
      -1,    46,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   211,   211,   227,   241,   243,   245,   251,   253,   254,
     255,   262,   263,   265,   288,   291,   293,   295,   297,   299,
     302,   305,   308,   311,   314,   317,   318,   319,   321,   322,
     323,   324,   327,   447,   463,   464,   466,   467,   468,   469,
     470,   471,   472,   473,   474,   475,   476,   477,   478,   479,
     480,   481,   482,   483,   484,   485,   486,   487,   488,   489,
     490,   491,   492,   493,   494,   495,   496,   497,   498,   499,
     500,   501,   502,   503,   514,   524,   541,   599,   601,   602,
     604,   605,   607,   686,   693,   708,   710,   713,   717,   727,
     774,   780,   784,   794,   841,   842,   843,   844,   845,   846,
     847,   848,   849,   850,   851,   852,   853,   854,   855,   856,
     864,   886,   897,   916,   919,   932,   954,   973,   988,  1003,
    1026,  1046,  1061,  1076,  1098,  1117,  1136,  1144,  1152,  1170,
    1180,  1197,  1216,  1233,  1234,  1236,  1237,  1238,  1239,  1246,
    1248,  1255,  1263,  1269,  1270,  1272,  1280,  1281,  1282,  1283,
    1284,  1285,  1286,  1287,  1288,  1289,  1290,  1291,  1292,  1296,
    1297,  1299,  1300,  1302,  1303,  1305,  1306,  1308,  1309,  1316,
    1321,  1323,  1336,  1340,  1348,  1349,  1360,  1365,  1370,  1375,
    1380,  1386,  1387,  1389,  1390,  1391,  1400,  1401,  1404,  1409,
    1412,  1415,  1418,  1420,  1423,  1426,  1428,  1431,  1432,  1433,
    1434,  1435,  1436,  1437,  1438,  1439,  1440,  1441,  1442,  1443,
    1444,  1445,  1446,  1447,  1450,  1455,  1460,  1470,  1471,  1478,
    1482,  1487,  1490,  1491,  1498,  1502,  1506,  1510,  1514,  1518,
    1522
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "PRAGMA", "AUTO", "DOUBLE", "INT",
  "STRUCT", "BREAK", "ELSE", "LONG", "SWITCH", "CASE", "ENUM", "REGISTER",
  "TYPEDEF", "CHAR", "EXTERN", "RETURN", "UNION", "CONST", "FLOAT",
  "SHORT", "UNSIGNED", "CONTINUE", "FOR", "SIGNED", "VOID", "DEFAULT",
  "GOTO", "SIZEOF", "VOLATILE", "DO", "IF", "STATIC", "WHILE", "CLASS",
  "PRIVATE", "PROTECTED", "PUBLIC", "VIRTUAL", "INLINE", "OPERATOR",
  "LLONG", "BOOL", "CFALSE", "CTRUE", "WCHAR", "TIME", "USING",
  "NAMESPACE", "ULLONG", "MUSTUNDERSTAND", "SIZE", "FRIEND", "TEMPLATE",
  "EXPLICIT", "TYPENAME", "RESTRICT", "null", "NONE", "ID", "LAB", "TYPE",
  "LNG", "DBL", "CHR", "STR", "','", "'='", "RA", "LA", "OA", "XA", "AA",
  "MA", "DA", "TA", "NA", "PA", "'?'", "':'", "OR", "AN", "'|'", "'^'",
  "'&'", "NE", "EQ", "'<'", "'>'", "GE", "LE", "RS", "LS", "'+'", "'-'",
  "'*'", "'/'", "'%'", "AR", "PP", "NN", "'{'", "'}'", "';'", "'['", "']'",
  "'!'", "'~'", "'('", "')'", "'@'", "'$'", "$accept", "prog", "s1",
  "exts", "exts1", "ext", "pragma", "decls", "t1", "t2", "t3", "t4", "t5",
  "dclrs", "dclr", "fdclr", "id", "name", "constr", "destr", "func",
  "fname", "fargso", "fargs", "farg", "arg", "texp", "spec", "tspec",
  "type", "struct", "class", "enum", "tname", "base", "s2", "s3", "s4",
  "s5", "s6", "store", "constobj", "abstract", "virtual", "ptrs", "array",
  "arrayck", "init", "occurs", "patt", "cint", "expr", "cexp", "qexp",
  "oexp", "obex", "aexp", "abex", "rexp", "lexp", "pexp", 0
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
     315,   316,   317,   318,   319,   320,   321,   322,    44,    61,
     323,   324,   325,   326,   327,   328,   329,   330,   331,   332,
      63,    58,   333,   334,   124,    94,    38,   335,   336,    60,
      62,   337,   338,   339,   340,    43,    45,    42,    47,    37,
     341,   342,   343,   123,   125,    59,    91,    93,    33,   126,
      40,    41,    64,    36
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   114,   115,   116,   117,   117,   118,   118,   119,   119,
     119,   119,   119,   120,   121,   121,   121,   121,   121,   121,
     122,   123,   124,   125,   126,   127,   127,   127,   127,   127,
     127,   127,   128,   129,   130,   130,   131,   131,   131,   131,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   131,   131,   132,   133,   134,   135,   136,   136,
     137,   137,   138,   139,   139,   140,   140,   141,   141,   141,
     142,   142,   142,   142,   143,   143,   143,   143,   143,   143,
     143,   143,   143,   143,   143,   143,   143,   143,   143,   143,
     143,   143,   143,   143,   143,   143,   143,   143,   143,   143,
     143,   143,   143,   143,   143,   143,   143,   143,   143,   143,
     144,   145,   146,   147,   147,   148,   148,   148,   148,   148,
     149,   150,   151,   152,   152,   153,   154,   154,   154,   154,
     154,   154,   154,   154,   154,   154,   154,   154,   154,   154,
     154,   155,   155,   156,   156,   157,   157,   158,   158,   158,
     159,   159,   159,   160,   161,   161,   162,   162,   162,   162,
     162,   163,   163,   164,   164,   164,   165,   165,   166,   166,
     167,   168,   168,   169,   170,   170,   171,   172,   172,   172,
     172,   172,   172,   172,   172,   172,   172,   172,   172,   172,
     172,   172,   172,   172,   173,   173,   173,   173,   173,   173,
     173,   173,   174,   174,   174,   174,   174,   174,   174,   174,
     174
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     0,     5,     1,     0,     2,     2,     1,
       2,     1,     1,     1,     0,     3,     4,     4,     4,     4,
       1,     1,     0,     0,     0,     1,     2,     3,     2,     2,
       3,     4,     5,     2,     1,     1,     1,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     3,     3,     2,     1,     3,     7,     0,     0,     1,
       1,     3,     6,     0,     1,     3,     4,     0,     2,     2,
       1,     1,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     7,
       5,     5,     7,     1,     3,     5,     5,     2,     2,     5,
       6,     2,     2,     6,     6,     8,     2,     2,     1,     4,
       2,     2,     2,     1,     1,     2,     2,     2,     1,     2,
       0,     0,     0,     0,     1,     0,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     1,     0,     2,     0,     1,     0,     2,     2,
       0,     4,     3,     1,     0,     2,     1,     2,     3,     4,
       3,     0,     1,     1,     2,     2,     3,     1,     5,     1,
       1,     3,     1,     1,     3,     1,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     1,     2,     2,     2,     2,     2,     2,
       4,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     6,     1,     0,     2,     0,     0,     0,    13,
     146,   105,    99,     0,   100,     0,   147,   151,    96,   150,
     157,     0,   153,   104,    98,   107,   106,    94,   160,   148,
       0,   152,   155,   101,    95,    97,   108,   102,   156,   103,
     154,     0,   149,   128,    20,    21,   158,   159,     7,     9,
      11,    12,     0,    77,    77,   167,    87,     0,   113,     0,
      87,     0,     6,    10,   117,   118,   140,   130,   126,   127,
       0,   140,   132,   121,   122,   141,     0,    34,    35,   140,
     131,     0,     0,   167,     8,    28,     0,    29,    26,    77,
       0,   152,   128,    89,   140,     0,   140,   140,    88,     0,
       0,    87,     0,    87,    87,   141,    87,   133,   134,     0,
       0,   167,    91,    90,    30,    77,   145,    27,     0,   170,
     169,   168,    33,    87,     0,     0,     0,     0,   138,   114,
      87,    87,    75,     4,     0,     0,     0,     0,    87,     0,
     142,   143,     0,    87,     0,     0,   129,   170,    93,    92,
      31,    78,    39,    49,    48,    47,    46,    45,    44,    43,
      42,    41,    40,    50,    51,    52,    53,    54,    56,    55,
      57,    59,    60,    58,    62,    61,    63,    64,    65,    66,
      67,    70,    68,    69,     0,    37,    38,     0,    73,     0,
     173,   181,     0,   139,   136,   135,   137,   140,     0,   143,
      22,    23,    24,   115,     0,    87,    87,   167,     0,   119,
       0,   110,     0,   170,    85,     0,    79,    80,   167,    71,
      72,     0,   229,   230,   225,   223,   224,   226,   227,   228,
       0,     0,     0,     0,   170,     0,     0,     0,     0,   189,
       0,   192,     0,   195,   213,   221,   182,   174,   176,   116,
      87,   111,     0,    87,    87,    87,    87,    15,   143,   123,
     120,     0,    86,   161,     0,    83,     0,   219,   217,   216,
     218,   172,   214,   215,     0,   187,   170,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    32,   183,     0,
       0,     0,   177,     0,   124,    16,    17,    18,    19,     0,
     109,   162,   163,    81,    84,   170,     0,     0,   222,   171,
     190,     0,   191,   194,   197,   198,   199,   201,   200,   202,
     204,   205,   203,   207,   206,   208,   209,   210,   211,   212,
     175,   180,   184,   185,   178,   112,   125,     0,    76,   181,
     220,   186,     0,   179,   164,   174,   188,    82
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     5,     6,    48,    49,   137,   138,    51,
     253,   254,   255,   139,   114,   115,    67,   122,    53,    54,
      85,    86,   215,   216,   217,   315,   110,    55,   111,    56,
      57,    58,    59,   109,   129,   101,   104,   206,   208,   151,
      60,   312,   348,    61,    90,   190,   191,   297,   247,   248,
     302,   274,   275,   321,   239,   240,   241,   242,   243,   244,
     245
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -281
static const yytype_int16 yypact[] =
{
    -281,    21,   -21,  -281,   -12,  -281,   207,   -31,   -40,  -281,
    -281,  -281,  -281,   -33,  -281,    36,  -281,  -281,  -281,  -281,
    -281,    -2,  -281,  -281,  -281,  -281,  -281,  -281,  -281,  -281,
      -1,   -16,  -281,  -281,  -281,  -281,  -281,  -281,  -281,  -281,
    -281,   -11,  -281,     3,  -281,  -281,  -281,  -281,  -281,  -281,
    -281,  -281,    16,  -281,  -281,    11,   897,     8,   -50,    47,
     897,    44,  -281,  -281,    58,    61,  -281,  -281,    58,    61,
      81,  -281,  -281,    58,    61,  -281,    73,  -281,  -281,  -281,
    -281,    67,   897,  -281,  -281,  -281,    -4,  -281,  -281,  -281,
     -17,  -281,    94,  -281,  -281,    19,  -281,  -281,  -281,    62,
     320,   650,    97,   834,   650,  -281,   650,  -281,  -281,    81,
      72,  -281,   897,   897,  -281,  -281,  -281,  -281,   430,   -86,
    -281,  -281,  -281,   650,   124,    19,    19,    19,  -281,   106,
     650,   834,  -281,  -281,   116,   134,   135,    84,   760,    32,
    -281,   150,   115,   650,   127,   142,  -281,   -43,  -281,  -281,
    -281,   897,  -281,  -281,  -281,  -281,  -281,  -281,  -281,  -281,
    -281,  -281,  -281,  -281,  -281,  -281,  -281,  -281,  -281,  -281,
    -281,  -281,  -281,  -281,  -281,  -281,  -281,  -281,  -281,  -281,
    -281,  -281,  -281,  -281,   128,  -281,  -281,   125,  -281,    22,
    -281,   170,   136,  -281,  -281,  -281,  -281,  -281,   138,   150,
    -281,  -281,  -281,  -281,   132,   540,   834,   140,   141,  -281,
     148,  -281,   217,   151,  -281,   145,  -281,   196,  -281,  -281,
    -281,   155,  -281,  -281,  -281,  -281,  -281,  -281,  -281,  -281,
      82,    82,    82,    82,   151,    82,    82,    82,   159,   -34,
      85,   184,   186,   305,  -281,  -281,  -281,   202,     9,  -281,
     650,  -281,   168,   540,   540,   540,   540,  -281,   150,  -281,
    -281,    81,  -281,   253,   897,    59,   897,  -281,  -281,  -281,
    -281,  -281,  -281,  -281,   -45,  -281,   151,    82,    82,    82,
      82,    82,    82,    82,    82,    82,    82,    82,    82,    82,
      82,    82,    82,    82,    82,    82,    82,  -281,  -281,    27,
     210,   230,   214,   192,  -281,  -281,  -281,  -281,  -281,   193,
    -281,  -281,   229,  -281,  -281,   151,   188,    82,  -281,  -281,
     232,   220,   184,   305,   519,   533,   627,   544,   544,    76,
      76,    76,    76,   107,   107,    37,    37,  -281,  -281,  -281,
    -281,  -281,  -281,  -281,    27,  -281,  -281,   238,  -281,   170,
    -281,  -281,    82,  -281,  -281,   202,  -281,  -281
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -281,  -281,  -281,  -281,   241,  -281,  -281,   -96,     7,   101,
    -281,  -281,  -281,    -5,   251,   252,   -15,  -281,  -281,  -281,
     -38,  -281,  -281,    45,  -281,  -281,  -114,    95,  -101,   -80,
    -281,  -281,  -281,  -281,    55,   -57,   203,  -281,  -182,  -281,
     -77,  -281,  -281,  -281,  -104,  -138,     0,   -44,   -39,  -281,
    -280,  -232,  -186,  -281,  -281,  -281,    39,  -281,    -3,   -37,
    -281
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -197
static const yytype_int16 yytable[] =
{
      72,    52,   112,   238,   188,   113,    76,   147,   142,   214,
     144,   148,   149,    50,   103,    80,    87,   252,   213,   341,
     189,     3,   106,   317,   -36,   118,   124,   192,    64,     4,
      65,    95,   112,   112,   198,   113,   113,   123,   112,   130,
     131,   113,   204,   120,   119,   320,  -193,   210,  -193,     7,
     218,   117,   221,    96,   121,   102,   125,   126,   127,    73,
      77,    74,    78,   189,   353,    63,   318,   222,   223,   120,
      66,   112,    62,   298,   113,   262,   309,   150,    81,   -25,
     121,   224,   128,   225,    83,   351,   226,   227,   228,   229,
     299,   298,    82,  -166,   145,    52,   271,    68,   141,    69,
      83,    75,    79,   107,   300,   301,   116,    50,   230,   257,
     340,    94,   221,   -74,   265,   -25,   -25,   231,   232,   233,
     314,    84,   300,   301,   108,   132,   199,   222,   223,   234,
     235,   236,   237,    70,   293,   294,   295,   205,   319,    71,
     250,   224,    77,   225,    78,   120,   226,   227,   228,   229,
      97,    93,   316,    99,   303,    98,   121,   305,   306,   307,
     308,   -34,   146,   218,   -35,   277,   356,   278,   230,   289,
     290,   291,   292,   293,   294,   295,   105,   231,   232,   233,
     194,   195,   196,    82,   112,   193,   112,   113,   203,   113,
     235,   236,   237,   267,   268,   269,   270,   200,   272,   273,
     140,   258,   291,   292,   293,   294,   295,    -5,     8,   197,
       9,    10,    11,    12,    13,   201,   202,    14,   207,   209,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,   211,   212,    26,    27,   219,   220,   246,    28,    45,
     249,    29,   251,    30,  -144,   259,   310,    31,    32,   -87,
      33,    34,   260,   261,    35,    36,   263,   189,    37,    38,
      39,    40,    41,    42,   264,   266,   276,  -196,   -87,   279,
      43,   296,   304,   311,   342,   -87,   323,   324,   325,   326,
     327,   328,   329,   330,   331,   332,   333,   334,   335,   336,
     337,   338,   339,   -87,   343,   344,   345,   346,   347,   350,
     317,   352,   354,   100,   -87,   256,    88,    89,   143,   313,
     355,   357,   -87,    44,    45,   349,  -165,   322,     0,    46,
      47,     8,     0,     9,    10,    11,    12,    13,     0,     0,
      14,     0,     0,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,     0,     0,    26,    27,     0,     0,
       0,    28,     0,     0,    29,     0,    30,     0,     0,     0,
      31,    32,   -87,    33,    34,     0,     0,    35,    36,     0,
       0,    37,    38,    39,    40,    41,    42,     0,     0,     0,
       0,   -87,     0,    43,     0,     0,     0,     0,   -87,   280,
     281,   282,   283,   284,   285,   286,   287,   288,   289,   290,
     291,   292,   293,   294,   295,     0,   -87,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   -87,     0,     0,
       0,     0,     0,     0,   133,   -87,    44,    45,     0,  -165,
       0,     0,    46,    47,    10,    11,    12,    13,     0,     0,
      14,     0,     0,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,     0,     0,    26,    27,     0,     0,
       0,    28,     0,     0,    29,     0,    30,     0,     0,     0,
      91,    32,     0,    33,    34,     0,     0,    35,    36,     0,
       0,    37,    38,    39,    40,    41,    42,     0,     0,     0,
       0,     0,     0,    92,     0,     0,     0,     0,     0,   152,
     153,   154,   155,   156,   157,   158,   159,   160,   161,   162,
       0,     0,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,     0,     0,     0,   184,     0,   185,   186,
     187,     0,    46,    47,    10,    11,    12,    13,     0,     0,
      14,     0,     0,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,     0,     0,    26,    27,     0,     0,
       0,    28,     0,     0,    29,     0,    30,   134,   135,   136,
      31,    32,     0,    33,    34,     0,     0,    35,    36,     0,
       0,    37,    38,    39,    40,    41,    42,     0,     0,     0,
       0,     0,     0,    43,   281,   282,   283,   284,   285,   286,
     287,   288,   289,   290,   291,   292,   293,   294,   295,   282,
     283,   284,   285,   286,   287,   288,   289,   290,   291,   292,
     293,   294,   295,   285,   286,   287,   288,   289,   290,   291,
     292,   293,   294,   295,   -14,     0,    44,   -14,     0,  -165,
       0,     0,    46,    47,    10,    11,    12,    13,     0,     0,
      14,     0,     0,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,     0,     0,    26,    27,     0,     0,
       0,    28,     0,     0,    29,     0,    30,   134,   135,   136,
      31,    32,     0,    33,    34,     0,     0,    35,    36,     0,
       0,    37,    38,    39,    40,    41,    42,     0,     0,     0,
       0,     0,     0,    43,   283,   284,   285,   286,   287,   288,
     289,   290,   291,   292,   293,   294,   295,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   -14,     0,    44,     0,     0,  -165,
       0,     0,    46,    47,    10,    11,    12,    13,     0,     0,
      14,     0,     0,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,     0,     0,    26,    27,     0,     0,
       0,    28,     0,     0,    29,     0,    30,   134,   135,   136,
      31,    32,     0,    33,    34,     0,     0,    35,    36,     0,
       0,    37,    38,    39,    40,    41,    42,     0,     0,     0,
       0,     0,     0,    43,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    10,    11,
      12,    13,     0,     0,    14,     0,     0,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,     0,     0,
      26,    27,     0,     0,     0,    28,    44,   -14,    29,  -165,
      30,     0,    46,    47,    31,    32,     0,    33,    34,     0,
       0,    35,    36,     0,     0,    37,    38,    39,    40,    41,
      42,     0,     0,     0,     0,     0,     0,    43,     0,     0,
       0,    10,    11,    12,    13,     0,     0,    14,     0,     0,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,     0,     0,    26,    27,     0,     0,     0,    28,     0,
       0,    29,     0,    30,     0,     0,     0,    91,    32,     0,
      33,    34,     0,  -165,    35,    36,    46,    47,    37,    38,
      39,    40,    41,    42,     0,     0,     0,     0,     0,     0,
      92,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    46,
      47
};

static const yytype_int16 yycheck[] =
{
      15,     6,    82,   189,   118,    82,    21,   111,   104,   147,
     106,   112,   113,     6,    71,    30,    54,   199,    61,   299,
     106,     0,    79,    68,   110,    42,     7,   123,    61,    50,
      63,    81,   112,   113,   130,   112,   113,    94,   118,    96,
      97,   118,   138,    86,    61,   277,    80,   143,    82,    61,
     151,    89,    30,   103,    97,    70,    37,    38,    39,    61,
      61,    63,    63,   106,   344,   105,   111,    45,    46,    86,
     103,   151,   103,    64,   151,   213,   258,   115,    89,    68,
      97,    59,    63,    61,    68,   317,    64,    65,    66,    67,
      81,    64,    89,   109,   109,   100,   234,    61,   103,    63,
      68,   103,   103,    36,    95,    96,   110,   100,    86,   205,
     296,   103,    30,   110,   218,   104,   105,    95,    96,    97,
      61,   105,    95,    96,    57,    63,   131,    45,    46,   107,
     108,   109,   110,    97,    97,    98,    99,   105,   276,   103,
     197,    59,    61,    61,    63,    86,    64,    65,    66,    67,
     103,    56,   266,   109,   250,    60,    97,   253,   254,   255,
     256,   103,    90,   264,   103,    80,   352,    82,    86,    93,
      94,    95,    96,    97,    98,    99,   103,    95,    96,    97,
     125,   126,   127,    89,   264,    61,   266,   264,   104,   266,
     108,   109,   110,   230,   231,   232,   233,    81,   235,   236,
     103,   206,    95,    96,    97,    98,    99,     0,     1,   103,
       3,     4,     5,     6,     7,    81,    81,    10,    68,   104,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,   104,    90,    26,    27,   107,   111,    67,    31,   107,
     104,    34,   104,    36,   104,   104,   261,    40,    41,    42,
      43,    44,   104,    36,    47,    48,   111,   106,    51,    52,
      53,    54,    55,    56,    68,   110,   107,    83,    61,    83,
      63,    69,   104,    20,    64,    68,   279,   280,   281,   282,
     283,   284,   285,   286,   287,   288,   289,   290,   291,   292,
     293,   294,   295,    86,    64,    81,   104,   104,    69,   111,
      68,    81,    64,    62,    97,   204,    55,    55,   105,   264,
     349,   355,   105,   106,   107,   315,   109,   278,    -1,   112,
     113,     1,    -1,     3,     4,     5,     6,     7,    -1,    -1,
      10,    -1,    -1,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    -1,    -1,    26,    27,    -1,    -1,
      -1,    31,    -1,    -1,    34,    -1,    36,    -1,    -1,    -1,
      40,    41,    42,    43,    44,    -1,    -1,    47,    48,    -1,
      -1,    51,    52,    53,    54,    55,    56,    -1,    -1,    -1,
      -1,    61,    -1,    63,    -1,    -1,    -1,    -1,    68,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,    -1,    86,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    97,    -1,    -1,
      -1,    -1,    -1,    -1,   104,   105,   106,   107,    -1,   109,
      -1,    -1,   112,   113,     4,     5,     6,     7,    -1,    -1,
      10,    -1,    -1,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    -1,    -1,    26,    27,    -1,    -1,
      -1,    31,    -1,    -1,    34,    -1,    36,    -1,    -1,    -1,
      40,    41,    -1,    43,    44,    -1,    -1,    47,    48,    -1,
      -1,    51,    52,    53,    54,    55,    56,    -1,    -1,    -1,
      -1,    -1,    -1,    63,    -1,    -1,    -1,    -1,    -1,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      -1,    -1,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,    -1,    -1,    -1,   106,    -1,   108,   109,
     110,    -1,   112,   113,     4,     5,     6,     7,    -1,    -1,
      10,    -1,    -1,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    -1,    -1,    26,    27,    -1,    -1,
      -1,    31,    -1,    -1,    34,    -1,    36,    37,    38,    39,
      40,    41,    -1,    43,    44,    -1,    -1,    47,    48,    -1,
      -1,    51,    52,    53,    54,    55,    56,    -1,    -1,    -1,
      -1,    -1,    -1,    63,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   104,    -1,   106,   107,    -1,   109,
      -1,    -1,   112,   113,     4,     5,     6,     7,    -1,    -1,
      10,    -1,    -1,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    -1,    -1,    26,    27,    -1,    -1,
      -1,    31,    -1,    -1,    34,    -1,    36,    37,    38,    39,
      40,    41,    -1,    43,    44,    -1,    -1,    47,    48,    -1,
      -1,    51,    52,    53,    54,    55,    56,    -1,    -1,    -1,
      -1,    -1,    -1,    63,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   104,    -1,   106,    -1,    -1,   109,
      -1,    -1,   112,   113,     4,     5,     6,     7,    -1,    -1,
      10,    -1,    -1,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    -1,    -1,    26,    27,    -1,    -1,
      -1,    31,    -1,    -1,    34,    -1,    36,    37,    38,    39,
      40,    41,    -1,    43,    44,    -1,    -1,    47,    48,    -1,
      -1,    51,    52,    53,    54,    55,    56,    -1,    -1,    -1,
      -1,    -1,    -1,    63,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     4,     5,
       6,     7,    -1,    -1,    10,    -1,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    -1,    -1,
      26,    27,    -1,    -1,    -1,    31,   106,   107,    34,   109,
      36,    -1,   112,   113,    40,    41,    -1,    43,    44,    -1,
      -1,    47,    48,    -1,    -1,    51,    52,    53,    54,    55,
      56,    -1,    -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,
      -1,     4,     5,     6,     7,    -1,    -1,    10,    -1,    -1,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    -1,    -1,    26,    27,    -1,    -1,    -1,    31,    -1,
      -1,    34,    -1,    36,    -1,    -1,    -1,    40,    41,    -1,
      43,    44,    -1,   109,    47,    48,   112,   113,    51,    52,
      53,    54,    55,    56,    -1,    -1,    -1,    -1,    -1,    -1,
      63,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   112,
     113
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,   115,   116,     0,    50,   117,   118,    61,     1,     3,
       4,     5,     6,     7,    10,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    26,    27,    31,    34,
      36,    40,    41,    43,    44,    47,    48,    51,    52,    53,
      54,    55,    56,    63,   106,   107,   112,   113,   119,   120,
     122,   123,   127,   132,   133,   141,   143,   144,   145,   146,
     154,   157,   103,   105,    61,    63,   103,   130,    61,    63,
      97,   103,   130,    61,    63,   103,   130,    61,    63,   103,
     130,    89,    89,    68,   105,   134,   135,   134,   128,   129,
     158,    40,    63,   141,   103,    81,   103,   103,   141,   109,
     118,   149,   130,   149,   150,   103,   149,    36,    57,   147,
     140,   142,   143,   154,   128,   129,   110,   134,    42,    61,
      86,    97,   131,   149,     7,    37,    38,    39,    63,   148,
     149,   149,    63,   104,    37,    38,    39,   121,   122,   127,
     103,   127,   121,   150,   121,   130,    90,   158,   142,   142,
     134,   153,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   106,   108,   109,   110,   140,   106,
     159,   160,   121,    61,   148,   148,   148,   103,   121,   127,
      81,    81,    81,   104,   121,   105,   151,    68,   152,   104,
     121,   104,    90,    61,   159,   136,   137,   138,   142,   107,
     111,    30,    45,    46,    59,    61,    64,    65,    66,    67,
      86,    95,    96,    97,   107,   108,   109,   110,   166,   168,
     169,   170,   171,   172,   173,   174,    67,   162,   163,   104,
     149,   104,   152,   124,   125,   126,   123,   121,   127,   104,
     104,    36,   159,   111,    68,   158,   110,   173,   173,   173,
     173,   159,   173,   173,   165,   166,   107,    80,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,    69,   161,    64,    81,
      95,    96,   164,   121,   104,   121,   121,   121,   121,   152,
     130,    20,   155,   137,    61,   139,   140,    68,   111,   159,
     165,   167,   170,   172,   172,   172,   172,   172,   172,   172,
     172,   172,   172,   172,   172,   172,   172,   172,   172,   172,
     166,   164,    64,    64,    81,   104,   104,    69,   156,   160,
     111,   165,    81,   164,    64,   162,   166,   161
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
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
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
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
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


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

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
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

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
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
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

/* Line 1455 of yacc.c  */
#line 211 "soapcpp2_yacc.y"
    { if (lflag)
    			  {	custom_header = 0;
    			  	custom_fault = 0;
			  }
			  else
			  {	add_header(sp->table);
			  	add_fault(sp->table);
			  }
			  compile(sp->table);
			  freetable(classtable);
			  freetable(enumtable);
			  freetable(typetable);
			  freetable(booltable);
			  freetable(templatetable);
			;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 227 "soapcpp2_yacc.y"
    { classtable = mktable((Table*)0);
			  enumtable = mktable((Table*)0);
			  typetable = mktable((Table*)0);
			  booltable = mktable((Table*)0);
			  templatetable = mktable((Table*)0);
			  p = enter(booltable, lookup("false"));
			  p->info.typ = mkint();
			  p->info.val.i = 0;
			  p = enter(booltable, lookup("true"));
			  p->info.typ = mkint();
			  p->info.val.i = 1;
			  mkscope(mktable(mktable((Table*)0)), 0);
			;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 242 "soapcpp2_yacc.y"
    { namespaceid = (yyvsp[(2) - (5)].sym)->name; ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 243 "soapcpp2_yacc.y"
    { ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 245 "soapcpp2_yacc.y"
    { add_soap();
			  if (!lflag)
			  {	add_qname();
			  	add_XML();
			  }
			;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 251 "soapcpp2_yacc.y"
    { ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 253 "soapcpp2_yacc.y"
    { ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 254 "soapcpp2_yacc.y"
    { ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 255 "soapcpp2_yacc.y"
    { synerror("input before ; skipped");
			  while (sp > stack)
			  {	freetable(sp->table);
			  	exitscope();
			  }
			  yyerrok;
			;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 262 "soapcpp2_yacc.y"
    { ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 263 "soapcpp2_yacc.y"
    { ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 265 "soapcpp2_yacc.y"
    { if ((yyvsp[(1) - (1)].s)[1] >= 'a' && (yyvsp[(1) - (1)].s)[1] <= 'z')
			  {	for (pp = &pragmas; *pp; pp = &(*pp)->next)
			          ;
				*pp = (Pragma*)emalloc(sizeof(Pragma));
				(*pp)->pragma = (char*)emalloc(strlen((yyvsp[(1) - (1)].s))+1);
				strcpy((*pp)->pragma, (yyvsp[(1) - (1)].s));
				(*pp)->next = NULL;
			  }
			  else if ((i = atoi((yyvsp[(1) - (1)].s)+2)) > 0)
				yylineno = i;
			  else
			  {	sprintf(errbuf, "directive '%s' ignored (use #import to import files and/or use option -i)", (yyvsp[(1) - (1)].s));
			  	semwarn(errbuf);
			  }
			;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 288 "soapcpp2_yacc.y"
    { transient &= ~6;
			  permission = 0;
			;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 292 "soapcpp2_yacc.y"
    { ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 294 "soapcpp2_yacc.y"
    { ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 296 "soapcpp2_yacc.y"
    { ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 298 "soapcpp2_yacc.y"
    { ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 300 "soapcpp2_yacc.y"
    { ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 302 "soapcpp2_yacc.y"
    { transient |= 1;
			;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 305 "soapcpp2_yacc.y"
    { transient &= ~1;
			;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 308 "soapcpp2_yacc.y"
    { permission = Sprivate;
			;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 311 "soapcpp2_yacc.y"
    { permission = Sprotected;
			;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 314 "soapcpp2_yacc.y"
    { permission = 0;
			;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 317 "soapcpp2_yacc.y"
    { ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 318 "soapcpp2_yacc.y"
    { ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 320 "soapcpp2_yacc.y"
    { ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 321 "soapcpp2_yacc.y"
    { ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 322 "soapcpp2_yacc.y"
    { ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 323 "soapcpp2_yacc.y"
    { ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 325 "soapcpp2_yacc.y"
    { ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 328 "soapcpp2_yacc.y"
    { if (((yyvsp[(3) - (5)].rec).sto & Stypedef) && sp->table->level == GLOBAL)
			  {	if (((yyvsp[(3) - (5)].rec).typ->type != Tstruct && (yyvsp[(3) - (5)].rec).typ->type != Tunion && (yyvsp[(3) - (5)].rec).typ->type != Tenum) || strcmp((yyvsp[(2) - (5)].sym)->name, (yyvsp[(3) - (5)].rec).typ->id->name))
				{	p = enter(typetable, (yyvsp[(2) - (5)].sym));
					p->info.typ = mksymtype((yyvsp[(3) - (5)].rec).typ, (yyvsp[(2) - (5)].sym));
			  		if ((yyvsp[(3) - (5)].rec).sto & Sextern)
						p->info.typ->transient = -1;
					else
						p->info.typ->transient = (yyvsp[(3) - (5)].rec).typ->transient;
			  		p->info.sto = (yyvsp[(3) - (5)].rec).sto;
					p->info.typ->pattern = (yyvsp[(4) - (5)].rec).pattern;
					if ((yyvsp[(4) - (5)].rec).minOccurs != -1)
					{	p->info.typ->minLength = (yyvsp[(4) - (5)].rec).minOccurs;
					}
					if ((yyvsp[(4) - (5)].rec).maxOccurs > 1)
						p->info.typ->maxLength = (yyvsp[(4) - (5)].rec).maxOccurs;
				}
				(yyvsp[(2) - (5)].sym)->token = TYPE;
			  }
			  else
			  {	p = enter(sp->table, (yyvsp[(2) - (5)].sym));
			  	p->info.typ = (yyvsp[(3) - (5)].rec).typ;
			  	p->info.sto = ((yyvsp[(3) - (5)].rec).sto | permission);
				if ((yyvsp[(5) - (5)].rec).hasval)
				{	p->info.hasval = True;
					switch ((yyvsp[(3) - (5)].rec).typ->type)
					{	case Tchar:
						case Tuchar:
						case Tshort:
						case Tushort:
						case Tint:
						case Tuint:
						case Tlong:
						case Tulong:
						case Tllong:
						case Tullong:
						case Tenum:
						case Ttime:
							if ((yyvsp[(5) - (5)].rec).typ->type == Tint || (yyvsp[(5) - (5)].rec).typ->type == Tchar || (yyvsp[(5) - (5)].rec).typ->type == Tenum)
								sp->val = p->info.val.i = (yyvsp[(5) - (5)].rec).val.i;
							else
							{	semerror("type error in initialization constant");
								p->info.hasval = False;
							}
							break;
						case Tfloat:
						case Tdouble:
						case Tldouble:
							if ((yyvsp[(5) - (5)].rec).typ->type == Tfloat || (yyvsp[(5) - (5)].rec).typ->type == Tdouble || (yyvsp[(5) - (5)].rec).typ->type == Tldouble)
								p->info.val.r = (yyvsp[(5) - (5)].rec).val.r;
							else if ((yyvsp[(5) - (5)].rec).typ->type == Tint)
								p->info.val.r = (double)(yyvsp[(5) - (5)].rec).val.i;
							else
							{	semerror("type error in initialization constant");
								p->info.hasval = False;
							}
							break;
						default:
							if ((yyvsp[(3) - (5)].rec).typ->type == Tpointer
							 && ((Tnode*)(yyvsp[(3) - (5)].rec).typ->ref)->type == Tchar
							 && (yyvsp[(5) - (5)].rec).typ->type == Tpointer
							 && ((Tnode*)(yyvsp[(5) - (5)].rec).typ->ref)->type == Tchar)
								p->info.val.s = (yyvsp[(5) - (5)].rec).val.s;
							else if (bflag
							 && (yyvsp[(3) - (5)].rec).typ->type == Tarray
							 && ((Tnode*)(yyvsp[(3) - (5)].rec).typ->ref)->type == Tchar
							 && (yyvsp[(5) - (5)].rec).typ->type == Tpointer
							 && ((Tnode*)(yyvsp[(5) - (5)].rec).typ->ref)->type == Tchar)
							{	if ((yyvsp[(3) - (5)].rec).typ->width / ((Tnode*)(yyvsp[(3) - (5)].rec).typ->ref)->width - 1 < strlen((yyvsp[(5) - (5)].rec).val.s))
								{	semerror("char[] initialization constant too long");
									p->info.val.s = "";
								}

								else
									p->info.val.s = (yyvsp[(5) - (5)].rec).val.s;
							}
							else if ((yyvsp[(3) - (5)].rec).typ->type == Tpointer
							      && ((Tnode*)(yyvsp[(3) - (5)].rec).typ->ref)->id == lookup("std::string"))
							      	p->info.val.s = (yyvsp[(5) - (5)].rec).val.s;
							else if ((yyvsp[(3) - (5)].rec).typ->id == lookup("std::string"))
							      	p->info.val.s = (yyvsp[(5) - (5)].rec).val.s;
							else if ((yyvsp[(3) - (5)].rec).typ->type == Tpointer
							      && (yyvsp[(5) - (5)].rec).typ->type == Tint
							      && (yyvsp[(5) - (5)].rec).val.i == 0)
								p->info.val.i = 0;
							else
							{	semerror("type error in initialization constant");
								p->info.hasval = False;
							}
							break;
					}
				}
				else
					p->info.val.i = sp->val;
			        if ((yyvsp[(4) - (5)].rec).minOccurs < 0)
			        {	if (((yyvsp[(3) - (5)].rec).sto & Sattribute) || (yyvsp[(3) - (5)].rec).typ->type == Tpointer || (yyvsp[(3) - (5)].rec).typ->type == Ttemplate || !strncmp((yyvsp[(2) - (5)].sym)->name, "__size", 6))
			        		p->info.minOccurs = 0;
			        	else
			        		p->info.minOccurs = 1;
				}
				else
					p->info.minOccurs = (yyvsp[(4) - (5)].rec).minOccurs;
				p->info.maxOccurs = (yyvsp[(4) - (5)].rec).maxOccurs;
				if (sp->mask)
					sp->val <<= 1;
				else
					sp->val++;
			  	p->info.offset = sp->offset;
				if ((yyvsp[(3) - (5)].rec).sto & Sextern)
					p->level = GLOBAL;
				else if ((yyvsp[(3) - (5)].rec).sto & Stypedef)
					;
			  	else if (sp->grow)
					sp->offset += p->info.typ->width;
				else if (p->info.typ->width > sp->offset)
					sp->offset = p->info.typ->width;
			  }
			  sp->entry = p;
			;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 447 "soapcpp2_yacc.y"
    { if ((yyvsp[(1) - (2)].rec).sto & Stypedef)
			  {	sprintf(errbuf, "invalid typedef qualifier for '%s'", (yyvsp[(2) - (2)].sym)->name);
				semwarn(errbuf);
			  }
			  p = enter(sp->table, (yyvsp[(2) - (2)].sym));
			  p->info.typ = (yyvsp[(1) - (2)].rec).typ;
			  p->info.sto = (yyvsp[(1) - (2)].rec).sto;
			  p->info.hasval = False;
			  p->info.offset = sp->offset;
			  if (sp->grow)
				sp->offset += p->info.typ->width;
			  else if (p->info.typ->width > sp->offset)
				sp->offset = p->info.typ->width;
			  sp->entry = p;
			;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 463 "soapcpp2_yacc.y"
    { (yyval.sym) = (yyvsp[(1) - (1)].sym); ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 464 "soapcpp2_yacc.y"
    { (yyval.sym) = (yyvsp[(1) - (1)].sym); ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 466 "soapcpp2_yacc.y"
    { (yyval.sym) = (yyvsp[(1) - (1)].sym); ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 467 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator!"); ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 468 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator~"); ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 469 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator="); ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 470 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator+="); ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 471 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator-="); ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 472 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator*="); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 473 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator/="); ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 474 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator%="); ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 475 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator&="); ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 476 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator^="); ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 477 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator|="); ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 478 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator<<="); ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 479 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator>>="); ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 480 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator||"); ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 481 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator&&"); ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 482 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator|"); ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 483 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator^"); ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 484 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator&"); ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 485 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator=="); ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 486 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator!="); ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 487 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator<"); ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 488 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator<="); ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 489 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator>"); ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 490 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator>="); ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 491 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator<<"); ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 492 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator>>"); ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 493 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator+"); ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 494 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator-"); ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 495 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator*"); ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 496 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator/"); ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 497 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator%"); ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 498 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator++"); ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 499 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator--"); ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 500 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator->"); ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 501 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator[]"); ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 502 "soapcpp2_yacc.y"
    { (yyval.sym) = lookup("operator()"); ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 503 "soapcpp2_yacc.y"
    { s1 = c_storage((yyvsp[(2) - (2)].rec).sto);
			  s2 = c_type((yyvsp[(2) - (2)].rec).typ);
			  s = (char*)emalloc(strlen(s1) + strlen(s2) + 10);
			  strcpy(s, "operator ");
			  strcat(s, s1);
			  strcat(s, s2);
			  (yyval.sym) = lookup(s);
			  if (!(yyval.sym))
				(yyval.sym) = install(s, ID);
			;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 514 "soapcpp2_yacc.y"
    { if (!(p = entry(classtable, (yyvsp[(1) - (1)].sym))))
			  	semerror("invalid constructor");
			  sp->entry = enter(sp->table, (yyvsp[(1) - (1)].sym));
			  sp->entry->info.typ = mknone();
			  sp->entry->info.sto = Snone;
			  sp->entry->info.offset = sp->offset;
			  sp->node.typ = mkvoid();
			  sp->node.sto = Snone;
			;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 525 "soapcpp2_yacc.y"
    { if (!(p = entry(classtable, (yyvsp[(3) - (3)].sym))))
			  	semerror("invalid destructor");
			  s = (char*)emalloc(strlen((yyvsp[(3) - (3)].sym)->name) + 2);
			  strcpy(s, "~");
			  strcat(s, (yyvsp[(3) - (3)].sym)->name);
			  sym = lookup(s);
			  if (!sym)
				sym = install(s, ID);
			  sp->entry = enter(sp->table, sym);
			  sp->entry->info.typ = mknone();
			  sp->entry->info.sto = (yyvsp[(1) - (3)].sto);
			  sp->entry->info.offset = sp->offset;
			  sp->node.typ = mkvoid();
			  sp->node.sto = Snone;
			;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 542 "soapcpp2_yacc.y"
    { if ((yyvsp[(1) - (7)].e)->level == GLOBAL)
			  {	if (!((yyvsp[(1) - (7)].e)->info.sto & Sextern) && sp->entry && sp->entry->info.typ->type == Tpointer && ((Tnode*)sp->entry->info.typ->ref)->type == Tchar)
			  	{	sprintf(errbuf, "last output parameter of remote method function prototype '%s' is a pointer to a char which will only return one byte: use char** instead to return a string", (yyvsp[(1) - (7)].e)->sym->name);
					semwarn(errbuf);
				}
				if ((yyvsp[(1) - (7)].e)->info.sto & Sextern)
				 	(yyvsp[(1) - (7)].e)->info.typ = mkmethod((yyvsp[(1) - (7)].e)->info.typ, sp->table);
			  	else if (sp->entry && (sp->entry->info.typ->type == Tpointer || sp->entry->info.typ->type == Treference || sp->entry->info.typ->type == Tarray || is_transient(sp->entry->info.typ)))
				{	if ((yyvsp[(1) - (7)].e)->info.typ->type == Tint)
					{	sp->entry->info.sto = (Storage)((int)sp->entry->info.sto | (int)Sreturn);
						(yyvsp[(1) - (7)].e)->info.typ = mkfun(sp->entry);
						(yyvsp[(1) - (7)].e)->info.typ->id = (yyvsp[(1) - (7)].e)->sym;
						if (!is_transient(sp->entry->info.typ))
						{	if (!is_response(sp->entry->info.typ))
							{	if (!is_XML(sp->entry->info.typ))
									add_response((yyvsp[(1) - (7)].e), sp->entry);
							}
							else
								add_result(sp->entry->info.typ);
						}
					}
					else
					{	sprintf(errbuf, "return type of remote method function prototype '%s' must be integer", (yyvsp[(1) - (7)].e)->sym->name);
						semerror(errbuf);
					}
				}
			  	else
			  	{	sprintf(errbuf, "last output parameter of remote method function prototype '%s' is a return parameter and must be a pointer or reference", (yyvsp[(1) - (7)].e)->sym->name);
					semerror(errbuf);
			  	}
				if (!((yyvsp[(1) - (7)].e)->info.sto & Sextern))
			  	{	unlinklast(sp->table);
			  		if ((p = entry(classtable, (yyvsp[(1) - (7)].e)->sym)))
					{	if (p->info.typ->ref)
						{	sprintf(errbuf, "remote method name clash: struct/class '%s' already declared at line %d", (yyvsp[(1) - (7)].e)->sym->name, p->lineno);
							semerror(errbuf);
						}
						else
						{	p->info.typ->ref = sp->table;
							p->info.typ->width = sp->offset;
						}
					}
			  		else
			  		{	p = enter(classtable, (yyvsp[(1) - (7)].e)->sym);
						p->info.typ = mkstruct(sp->table, sp->offset);
						p->info.typ->id = (yyvsp[(1) - (7)].e)->sym;
			  		}
			  	}
			  }
			  else if ((yyvsp[(1) - (7)].e)->level == INTERNAL)
			  {	(yyvsp[(1) - (7)].e)->info.typ = mkmethod((yyvsp[(1) - (7)].e)->info.typ, sp->table);
				(yyvsp[(1) - (7)].e)->info.sto = (Storage)((int)(yyvsp[(1) - (7)].e)->info.sto | (int)(yyvsp[(6) - (7)].sto) | (int)(yyvsp[(7) - (7)].sto));
			  	transient &= ~1;
			  }
			  exitscope();
			;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 599 "soapcpp2_yacc.y"
    { (yyval.e) = sp->entry; ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 601 "soapcpp2_yacc.y"
    { ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 602 "soapcpp2_yacc.y"
    { ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 604 "soapcpp2_yacc.y"
    { ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 605 "soapcpp2_yacc.y"
    { ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 608 "soapcpp2_yacc.y"
    { if ((yyvsp[(4) - (6)].rec).sto & Stypedef)
			  	semwarn("typedef in function argument");
			  p = enter(sp->table, (yyvsp[(3) - (6)].sym));
			  p->info.typ = (yyvsp[(4) - (6)].rec).typ;
			  p->info.sto = (yyvsp[(4) - (6)].rec).sto;
			  if ((yyvsp[(5) - (6)].rec).minOccurs < 0)
			  {	if (((yyvsp[(4) - (6)].rec).sto & Sattribute) || (yyvsp[(4) - (6)].rec).typ->type == Tpointer)
			        	p->info.minOccurs = 0;
			       	else
			        	p->info.minOccurs = 1;
			  }
			  else
				p->info.minOccurs = (yyvsp[(5) - (6)].rec).minOccurs;
			  p->info.maxOccurs = (yyvsp[(5) - (6)].rec).maxOccurs;
			  if ((yyvsp[(6) - (6)].rec).hasval)
			  {	p->info.hasval = True;
				switch ((yyvsp[(4) - (6)].rec).typ->type)
				{	case Tchar:
					case Tuchar:
					case Tshort:
					case Tushort:
					case Tint:
					case Tuint:
					case Tlong:
					case Tulong:
					case Tenum:
					case Ttime:
						if ((yyvsp[(6) - (6)].rec).typ->type == Tint || (yyvsp[(6) - (6)].rec).typ->type == Tchar || (yyvsp[(6) - (6)].rec).typ->type == Tenum)
							sp->val = p->info.val.i = (yyvsp[(6) - (6)].rec).val.i;
						else
						{	semerror("type error in initialization constant");
							p->info.hasval = False;
						}
						break;
					case Tfloat:
					case Tdouble:
					case Tldouble:
						if ((yyvsp[(6) - (6)].rec).typ->type == Tfloat || (yyvsp[(6) - (6)].rec).typ->type == Tdouble || (yyvsp[(6) - (6)].rec).typ->type == Tldouble)
							p->info.val.r = (yyvsp[(6) - (6)].rec).val.r;
						else if ((yyvsp[(6) - (6)].rec).typ->type == Tint)
							p->info.val.r = (double)(yyvsp[(6) - (6)].rec).val.i;
						else
						{	semerror("type error in initialization constant");
							p->info.hasval = False;
						}
						break;
					default:
						if ((yyvsp[(4) - (6)].rec).typ->type == Tpointer
						 && ((Tnode*)(yyvsp[(4) - (6)].rec).typ->ref)->type == Tchar
						 && (yyvsp[(6) - (6)].rec).typ->type == Tpointer
						 && ((Tnode*)(yyvsp[(6) - (6)].rec).typ->ref)->type == Tchar)
							p->info.val.s = (yyvsp[(6) - (6)].rec).val.s;
						else if ((yyvsp[(4) - (6)].rec).typ->type == Tpointer
						      && ((Tnode*)(yyvsp[(4) - (6)].rec).typ->ref)->id == lookup("std::string"))
						      	p->info.val.s = (yyvsp[(6) - (6)].rec).val.s;
						else if ((yyvsp[(4) - (6)].rec).typ->id == lookup("std::string"))
						      	p->info.val.s = (yyvsp[(6) - (6)].rec).val.s;
						else if ((yyvsp[(4) - (6)].rec).typ->type == Tpointer
						      && (yyvsp[(6) - (6)].rec).typ->type == Tint
						      && (yyvsp[(6) - (6)].rec).val.i == 0)
							p->info.val.i = 0;
						else
						{	semerror("type error in initialization constant");
							p->info.hasval = False;
						}
						break;
				}
			  }
			  p->info.offset = sp->offset;
			  if ((yyvsp[(4) - (6)].rec).sto & Sextern)
				p->level = GLOBAL;
			  else if (sp->grow)
				sp->offset += p->info.typ->width;
			  else if (p->info.typ->width > sp->offset)
				sp->offset = p->info.typ->width;
			  sp->entry = p;
			;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 686 "soapcpp2_yacc.y"
    { if (sp->table->level != PARAM)
			    (yyval.sym) = gensymidx("param", (int)++sp->val);
			  else if (eflag)
				(yyval.sym) = gensymidx("_param", (int)++sp->val);
			  else
				(yyval.sym) = gensym("_param");
			;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 693 "soapcpp2_yacc.y"
    { if (vflag != 1 && *(yyvsp[(1) - (1)].sym)->name == '_' && sp->table->level == GLOBAL)
			  { sprintf(errbuf, "SOAP 1.2 does not support anonymous parameters '%s'", (yyvsp[(1) - (1)].sym)->name);
			    semwarn(errbuf);
			  }
			  (yyval.sym) = (yyvsp[(1) - (1)].sym);
			;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 709 "soapcpp2_yacc.y"
    { (yyval.rec) = (yyvsp[(3) - (3)].rec); ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 711 "soapcpp2_yacc.y"
    { (yyval.rec) = (yyvsp[(4) - (4)].rec); ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 713 "soapcpp2_yacc.y"
    { (yyval.rec).typ = mkint();
			  (yyval.rec).sto = Snone;
			  sp->node = (yyval.rec);
			;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 717 "soapcpp2_yacc.y"
    { (yyval.rec).typ = (yyvsp[(2) - (2)].rec).typ;
			  (yyval.rec).sto = (Storage)((int)(yyvsp[(1) - (2)].sto) | (int)(yyvsp[(2) - (2)].rec).sto);
			  if (((yyval.rec).sto & Sattribute) && !is_primitive_or_string((yyvsp[(2) - (2)].rec).typ) && !is_stdstr((yyvsp[(2) - (2)].rec).typ) && !is_binary((yyvsp[(2) - (2)].rec).typ) && !is_external((yyvsp[(2) - (2)].rec).typ))
			  {	semwarn("invalid attribute type");
			  	(yyval.rec).sto &= ~Sattribute;
			  }
			  sp->node = (yyval.rec);
			  if ((yyvsp[(1) - (2)].sto) & Sextern)
				transient = 0;
			;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 727 "soapcpp2_yacc.y"
    { if ((yyvsp[(1) - (2)].typ)->type == Tint)
				switch ((yyvsp[(2) - (2)].rec).typ->type)
				{ case Tchar:	(yyval.rec).typ = (yyvsp[(2) - (2)].rec).typ; break;
				  case Tshort:	(yyval.rec).typ = (yyvsp[(2) - (2)].rec).typ; break;
				  case Tint:	(yyval.rec).typ = (yyvsp[(1) - (2)].typ); break;
				  case Tlong:	(yyval.rec).typ = (yyvsp[(2) - (2)].rec).typ; break;
				  case Tllong:	(yyval.rec).typ = (yyvsp[(2) - (2)].rec).typ; break;
				  default:	semwarn("illegal use of 'signed'");
						(yyval.rec).typ = (yyvsp[(2) - (2)].rec).typ;
				}
			  else if ((yyvsp[(1) - (2)].typ)->type == Tuint)
				switch ((yyvsp[(2) - (2)].rec).typ->type)
				{ case Tchar:	(yyval.rec).typ = mkuchar(); break;
				  case Tshort:	(yyval.rec).typ = mkushort(); break;
				  case Tint:	(yyval.rec).typ = (yyvsp[(1) - (2)].typ); break;
				  case Tlong:	(yyval.rec).typ = mkulong(); break;
				  case Tllong:	(yyval.rec).typ = mkullong(); break;
				  default:	semwarn("illegal use of 'unsigned'");
						(yyval.rec).typ = (yyvsp[(2) - (2)].rec).typ;
				}
			  else if ((yyvsp[(1) - (2)].typ)->type == Tlong)
				switch ((yyvsp[(2) - (2)].rec).typ->type)
				{ case Tint:	(yyval.rec).typ = (yyvsp[(1) - (2)].typ); break;
				  case Tlong:	(yyval.rec).typ = mkllong(); break;
				  case Tuint:	(yyval.rec).typ = mkulong(); break;
				  case Tulong:	(yyval.rec).typ = mkullong(); break;
				  case Tdouble:	(yyval.rec).typ = mkldouble(); break;
				  default:	semwarn("illegal use of 'long'");
						(yyval.rec).typ = (yyvsp[(2) - (2)].rec).typ;
				}
			  else if ((yyvsp[(1) - (2)].typ)->type == Tulong)
				switch ((yyvsp[(2) - (2)].rec).typ->type)
				{ case Tint:	(yyval.rec).typ = (yyvsp[(1) - (2)].typ); break;
				  case Tlong:	(yyval.rec).typ = mkullong(); break;
				  case Tuint:	(yyval.rec).typ = (yyvsp[(1) - (2)].typ); break;
				  case Tulong:	(yyval.rec).typ = mkullong(); break;
				  default:	semwarn("illegal use of 'long'");
						(yyval.rec).typ = (yyvsp[(2) - (2)].rec).typ;
				}
			  else if ((yyvsp[(2) - (2)].rec).typ->type == Tint)
				(yyval.rec).typ = (yyvsp[(1) - (2)].typ);
			  else
			  	semwarn("invalid type (missing ';'?)");
			  (yyval.rec).sto = (yyvsp[(2) - (2)].rec).sto;
			  sp->node = (yyval.rec);
			;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 774 "soapcpp2_yacc.y"
    { (yyval.rec).typ = mkint();
			  (yyval.rec).sto = (yyvsp[(1) - (1)].sto);
			  sp->node = (yyval.rec);
			  if ((yyvsp[(1) - (1)].sto) & Sextern)
				transient = 0;
			;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 780 "soapcpp2_yacc.y"
    { (yyval.rec).typ = (yyvsp[(1) - (1)].typ);
			  (yyval.rec).sto = Snone;
			  sp->node = (yyval.rec);
			;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 784 "soapcpp2_yacc.y"
    { (yyval.rec).typ = (yyvsp[(2) - (2)].rec).typ;
			  (yyval.rec).sto = (Storage)((int)(yyvsp[(1) - (2)].sto) | (int)(yyvsp[(2) - (2)].rec).sto);
			  if (((yyval.rec).sto & Sattribute) && !is_primitive_or_string((yyvsp[(2) - (2)].rec).typ) && !is_stdstr((yyvsp[(2) - (2)].rec).typ) && !is_binary((yyvsp[(2) - (2)].rec).typ) && !is_external((yyvsp[(2) - (2)].rec).typ))
			  {	semwarn("invalid attribute type");
			  	(yyval.rec).sto &= ~Sattribute;
			  }
			  sp->node = (yyval.rec);
			  if ((yyvsp[(1) - (2)].sto) & Sextern)
				transient = 0;
			;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 794 "soapcpp2_yacc.y"
    { if ((yyvsp[(1) - (2)].typ)->type == Tint)
				switch ((yyvsp[(2) - (2)].rec).typ->type)
				{ case Tchar:	(yyval.rec).typ = (yyvsp[(2) - (2)].rec).typ; break;
				  case Tshort:	(yyval.rec).typ = (yyvsp[(2) - (2)].rec).typ; break;
				  case Tint:	(yyval.rec).typ = (yyvsp[(1) - (2)].typ); break;
				  case Tlong:	(yyval.rec).typ = (yyvsp[(2) - (2)].rec).typ; break;
				  case Tllong:	(yyval.rec).typ = (yyvsp[(2) - (2)].rec).typ; break;
				  default:	semwarn("illegal use of 'signed'");
						(yyval.rec).typ = (yyvsp[(2) - (2)].rec).typ;
				}
			  else if ((yyvsp[(1) - (2)].typ)->type == Tuint)
				switch ((yyvsp[(2) - (2)].rec).typ->type)
				{ case Tchar:	(yyval.rec).typ = mkuchar(); break;
				  case Tshort:	(yyval.rec).typ = mkushort(); break;
				  case Tint:	(yyval.rec).typ = (yyvsp[(1) - (2)].typ); break;
				  case Tlong:	(yyval.rec).typ = mkulong(); break;
				  case Tllong:	(yyval.rec).typ = mkullong(); break;
				  default:	semwarn("illegal use of 'unsigned'");
						(yyval.rec).typ = (yyvsp[(2) - (2)].rec).typ;
				}
			  else if ((yyvsp[(1) - (2)].typ)->type == Tlong)
				switch ((yyvsp[(2) - (2)].rec).typ->type)
				{ case Tint:	(yyval.rec).typ = (yyvsp[(1) - (2)].typ); break;
				  case Tlong:	(yyval.rec).typ = mkllong(); break;
				  case Tuint:	(yyval.rec).typ = mkulong(); break;
				  case Tulong:	(yyval.rec).typ = mkullong(); break;
				  case Tdouble:	(yyval.rec).typ = mkldouble(); break;
				  default:	semwarn("illegal use of 'long'");
						(yyval.rec).typ = (yyvsp[(2) - (2)].rec).typ;
				}
			  else if ((yyvsp[(1) - (2)].typ)->type == Tulong)
				switch ((yyvsp[(2) - (2)].rec).typ->type)
				{ case Tint:	(yyval.rec).typ = (yyvsp[(1) - (2)].typ); break;
				  case Tlong:	(yyval.rec).typ = mkullong(); break;
				  case Tuint:	(yyval.rec).typ = (yyvsp[(1) - (2)].typ); break;
				  case Tulong:	(yyval.rec).typ = mkullong(); break;
				  default:	semwarn("illegal use of 'long'");
						(yyval.rec).typ = (yyvsp[(2) - (2)].rec).typ;
				}
			  else if ((yyvsp[(2) - (2)].rec).typ->type == Tint)
				(yyval.rec).typ = (yyvsp[(1) - (2)].typ);
			  else
			  	semwarn("invalid type");
			  (yyval.rec).sto = (yyvsp[(2) - (2)].rec).sto;
			  sp->node = (yyval.rec);
			;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 841 "soapcpp2_yacc.y"
    { (yyval.typ) = mkvoid(); ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 842 "soapcpp2_yacc.y"
    { (yyval.typ) = mkbool(); ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 843 "soapcpp2_yacc.y"
    { (yyval.typ) = mkchar(); ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 844 "soapcpp2_yacc.y"
    { (yyval.typ) = mkwchart(); ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 845 "soapcpp2_yacc.y"
    { (yyval.typ) = mkshort(); ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 846 "soapcpp2_yacc.y"
    { (yyval.typ) = mkint(); ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 847 "soapcpp2_yacc.y"
    { (yyval.typ) = mklong(); ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 848 "soapcpp2_yacc.y"
    { (yyval.typ) = mkllong(); ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 849 "soapcpp2_yacc.y"
    { (yyval.typ) = mkullong(); ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 850 "soapcpp2_yacc.y"
    { (yyval.typ) = mkulong(); ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 851 "soapcpp2_yacc.y"
    { (yyval.typ) = mkfloat(); ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 852 "soapcpp2_yacc.y"
    { (yyval.typ) = mkdouble(); ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 853 "soapcpp2_yacc.y"
    { (yyval.typ) = mkint(); ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 854 "soapcpp2_yacc.y"
    { (yyval.typ) = mkuint(); ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 855 "soapcpp2_yacc.y"
    { (yyval.typ) = mktimet(); ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 857 "soapcpp2_yacc.y"
    { if (!(p = entry(templatetable, (yyvsp[(7) - (7)].sym))))
			  {	p = enter(templatetable, (yyvsp[(7) - (7)].sym));
			  	p->info.typ = mktemplate(NULL, (yyvsp[(7) - (7)].sym));
			  	(yyvsp[(7) - (7)].sym)->token = TYPE;
			  }
			  (yyval.typ) = p->info.typ;
			;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 865 "soapcpp2_yacc.y"
    { sym = gensym("_Struct");
			  sprintf(errbuf, "anonymous class will be named '%s'", sym->name);
			  semwarn(errbuf);
			  if ((p = entry(classtable, sym)))
			  {	if (p->info.typ->ref || p->info.typ->type != Tclass)
				{	sprintf(errbuf, "class '%s' already declared at line %d", sym->name, p->lineno);
					semerror(errbuf);
				}
			  }
			  else
			  {	p = enter(classtable, sym);
				p->info.typ = mkclass((Table*)0, 0);
			  }
			  sym->token = TYPE;
			  sp->table->sym = sym;
			  p->info.typ->ref = sp->table;
			  p->info.typ->width = sp->offset;
			  p->info.typ->id = sym;
			  (yyval.typ) = p->info.typ;
			  exitscope();
			;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 887 "soapcpp2_yacc.y"
    { p = reenter(classtable, (yyvsp[(1) - (5)].e)->sym);
			  sp->table->sym = p->sym;
			  p->info.typ->ref = sp->table;
			  p->info.typ->width = sp->offset;
			  p->info.typ->id = p->sym;
			  if (p->info.typ->base)
			  	sp->table->prev = (Table*)entry(classtable, p->info.typ->base)->info.typ->ref;
			  (yyval.typ) = p->info.typ;
			  exitscope();
			;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 898 "soapcpp2_yacc.y"
    { p = reenter(classtable, (yyvsp[(1) - (7)].e)->sym);
			  sp->table->sym = p->sym;
			  if (!(yyvsp[(3) - (7)].e))
				semerror("invalid base class");
			  else
			  {	sp->table->prev = (Table*)(yyvsp[(3) - (7)].e)->info.typ->ref;
				if (!sp->table->prev && !(yyvsp[(3) - (7)].e)->info.typ->transient)
				{	sprintf(errbuf, "class '%s' has incomplete type", (yyvsp[(3) - (7)].e)->sym->name);
					semerror(errbuf);
				}
			  	p->info.typ->base = (yyvsp[(3) - (7)].e)->info.typ->id;
			  }
			  p->info.typ->ref = sp->table;
			  p->info.typ->width = sp->offset;
			  p->info.typ->id = p->sym;
			  (yyval.typ) = p->info.typ;
			  exitscope();
			;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 916 "soapcpp2_yacc.y"
    { (yyvsp[(1) - (1)].e)->info.typ->id = (yyvsp[(1) - (1)].e)->sym;
			  (yyval.typ) = (yyvsp[(1) - (1)].e)->info.typ;
			;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 920 "soapcpp2_yacc.y"
    { if (!(yyvsp[(3) - (3)].e))
				semerror("invalid base class");
			  else
			  {	if (!(yyvsp[(3) - (3)].e)->info.typ->ref && !(yyvsp[(3) - (3)].e)->info.typ->transient)
				{	sprintf(errbuf, "class '%s' has incomplete type", (yyvsp[(3) - (3)].e)->sym->name);
					semerror(errbuf);
				}
			  	(yyvsp[(1) - (3)].e)->info.typ->base = (yyvsp[(3) - (3)].e)->info.typ->id;
			  }
			  (yyvsp[(1) - (3)].e)->info.typ->id = (yyvsp[(1) - (3)].e)->sym;
			  (yyval.typ) = (yyvsp[(1) - (3)].e)->info.typ;
			;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 933 "soapcpp2_yacc.y"
    { sym = gensym("_Struct");
			  sprintf(errbuf, "anonymous struct will be named '%s'", sym->name);
			  semwarn(errbuf);
			  if ((p = entry(classtable, sym)))
			  {	if (p->info.typ->ref || p->info.typ->type != Tstruct)
				{	sprintf(errbuf, "struct '%s' already declared at line %d", sym->name, p->lineno);
					semerror(errbuf);
				}
				else
				{	p->info.typ->ref = sp->table;
					p->info.typ->width = sp->offset;
				}
			  }
			  else
			  {	p = enter(classtable, sym);
				p->info.typ = mkstruct(sp->table, sp->offset);
			  }
			  p->info.typ->id = sym;
			  (yyval.typ) = p->info.typ;
			  exitscope();
			;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 955 "soapcpp2_yacc.y"
    { if ((p = entry(classtable, (yyvsp[(1) - (5)].e)->sym)) && p->info.typ->ref)
			  {	if (is_mutable(p->info.typ))
			  	{	if (merge((Table*)p->info.typ->ref, sp->table))
					{	sprintf(errbuf, "member name clash in struct '%s' declared at line %d", (yyvsp[(1) - (5)].e)->sym->name, p->lineno);
						semerror(errbuf);
					}
			  		p->info.typ->width += sp->offset;
				}
			  }
			  else
			  {	p = reenter(classtable, (yyvsp[(1) - (5)].e)->sym);
			  	p->info.typ->ref = sp->table;
			  	p->info.typ->width = sp->offset;
			  	p->info.typ->id = p->sym;
			  }
			  (yyval.typ) = p->info.typ;
			  exitscope();
			;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 973 "soapcpp2_yacc.y"
    { if ((p = entry(classtable, (yyvsp[(2) - (2)].sym))))
			  {	if (p->info.typ->type == Tstruct)
			  		(yyval.typ) = p->info.typ;
			  	else
				{	sprintf(errbuf, "'struct %s' redeclaration (line %d)", (yyvsp[(2) - (2)].sym)->name, p->lineno);
			  		semerror(errbuf);
			  		(yyval.typ) = mkint();
				}
			  }
			  else
			  {	p = enter(classtable, (yyvsp[(2) - (2)].sym));
			  	(yyval.typ) = p->info.typ = mkstruct((Table*)0, 0);
				p->info.typ->id = (yyvsp[(2) - (2)].sym);
			  }
			;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 988 "soapcpp2_yacc.y"
    { if ((p = entry(classtable, (yyvsp[(2) - (2)].sym))))
			  {	if (p->info.typ->type == Tstruct)
					(yyval.typ) = p->info.typ;
			  	else
				{	sprintf(errbuf, "'struct %s' redeclaration (line %d)", (yyvsp[(2) - (2)].sym)->name, p->lineno);
			  		semerror(errbuf);
			  		(yyval.typ) = mkint();
				}
			  }
			  else
			  {	p = enter(classtable, (yyvsp[(2) - (2)].sym));
			  	(yyval.typ) = p->info.typ = mkstruct((Table*)0, 0);
				p->info.typ->id = (yyvsp[(2) - (2)].sym);
			  }
			;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 1004 "soapcpp2_yacc.y"
    { sym = gensym("_Union");
			  sprintf(errbuf, "anonymous union will be named '%s'", sym->name);
			  semwarn(errbuf);
			  (yyval.typ) = mkunion(sp->table, sp->offset);
			  if ((p = entry(classtable, sym)))
			  {	if ((Table*) p->info.typ->ref)
				{	sprintf(errbuf, "union or struct '%s' already declared at line %d", sym->name, p->lineno);
					semerror(errbuf);
				}
				else
				{	p->info.typ->ref = sp->table;
					p->info.typ->width = sp->offset;
				}
			  }
			  else
			  {	p = enter(classtable, sym);
				p->info.typ = mkunion(sp->table, sp->offset);
			  }
			  p->info.typ->id = sym;
			  (yyval.typ) = p->info.typ;
			  exitscope();
			;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 1027 "soapcpp2_yacc.y"
    { if ((p = entry(classtable, (yyvsp[(2) - (6)].sym))))
			  {	if (p->info.typ->ref || p->info.typ->type != Tunion)
			  	{	sprintf(errbuf, "union '%s' already declared at line %d", (yyvsp[(2) - (6)].sym)->name, p->lineno);
					semerror(errbuf);
				}
				else
				{	p = reenter(classtable, (yyvsp[(2) - (6)].sym));
					p->info.typ->ref = sp->table;
					p->info.typ->width = sp->offset;
				}
			  }
			  else
			  {	p = enter(classtable, (yyvsp[(2) - (6)].sym));
				p->info.typ = mkunion(sp->table, sp->offset);
			  }
			  p->info.typ->id = (yyvsp[(2) - (6)].sym);
			  (yyval.typ) = p->info.typ;
			  exitscope();
			;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 1046 "soapcpp2_yacc.y"
    { if ((p = entry(classtable, (yyvsp[(2) - (2)].sym))))
			  {	if (p->info.typ->type == Tunion)
					(yyval.typ) = p->info.typ;
			  	else
				{	sprintf(errbuf, "'union %s' redeclaration (line %d)", (yyvsp[(2) - (2)].sym)->name, p->lineno);
			  		semerror(errbuf);
			  		(yyval.typ) = mkint();
				}
			  }
			  else
			  {	p = enter(classtable, (yyvsp[(2) - (2)].sym));
			  	(yyval.typ) = p->info.typ = mkunion((Table*) 0, 0);
				p->info.typ->id = (yyvsp[(2) - (2)].sym);
			  }
			;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 1061 "soapcpp2_yacc.y"
    { if ((p = entry(classtable, (yyvsp[(2) - (2)].sym))))
			  {	if (p->info.typ->type == Tunion)
					(yyval.typ) = p->info.typ;
			  	else
				{	sprintf(errbuf, "'union %s' redeclaration (line %d)", (yyvsp[(2) - (2)].sym)->name, p->lineno);
			  		semerror(errbuf);
			  		(yyval.typ) = mkint();
				}
			  }
			  else
			  {	p = enter(classtable, (yyvsp[(2) - (2)].sym));
			  	(yyval.typ) = p->info.typ = mkunion((Table*) 0, 0);
				p->info.typ->id = (yyvsp[(2) - (2)].sym);
			  }
			;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 1077 "soapcpp2_yacc.y"
    { sym = gensym("_Enum");
			  sprintf(errbuf, "anonymous enum will be named '%s'", sym->name);
			  semwarn(errbuf);
			  if ((p = entry(enumtable, sym)))
			  {	if ((Table*) p->info.typ->ref)
				{	sprintf(errbuf, "enum '%s' already declared at line %d", sym->name, p->lineno);
					semerror(errbuf);
				}
				else
				{	p->info.typ->ref = sp->table;
					p->info.typ->width = 4; /* 4 = enum */
				}
			  }
			  else
			  {	p = enter(enumtable, sym);
				p->info.typ = mkenum(sp->table);
			  }
			  p->info.typ->id = sym;
			  (yyval.typ) = p->info.typ;
			  exitscope();
			;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 1099 "soapcpp2_yacc.y"
    { if ((p = entry(enumtable, (yyvsp[(1) - (6)].e)->sym)))
			  {	if ((Table*) p->info.typ->ref)
				{	sprintf(errbuf, "enum '%s' already declared at line %d", (yyvsp[(1) - (6)].e)->sym->name, p->lineno);
					semerror(errbuf);
				}
				else
				{	p->info.typ->ref = sp->table;
					p->info.typ->width = 4; /* 4 = enum */
				}
			  }
			  else
			  {	p = enter(enumtable, (yyvsp[(1) - (6)].e)->sym);
				p->info.typ = mkenum(sp->table);
			  }
			  p->info.typ->id = (yyvsp[(1) - (6)].e)->sym;
			  (yyval.typ) = p->info.typ;
			  exitscope();
			;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 1118 "soapcpp2_yacc.y"
    { if ((p = entry(enumtable, (yyvsp[(3) - (8)].sym))))
			  {	if (p->info.typ->ref)
				{	sprintf(errbuf, "enum '%s' already declared at line %d", (yyvsp[(3) - (8)].sym)->name, p->lineno);
					semerror(errbuf);
				}
				else
				{	p->info.typ->ref = sp->table;
					p->info.typ->width = 8; /* 8 = mask */
				}
			  }
			  else
			  {	p = enter(enumtable, (yyvsp[(3) - (8)].sym));
				p->info.typ = mkmask(sp->table);
			  }
			  p->info.typ->id = (yyvsp[(3) - (8)].sym);
			  (yyval.typ) = p->info.typ;
			  exitscope();
			;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 1136 "soapcpp2_yacc.y"
    { if ((p = entry(enumtable, (yyvsp[(2) - (2)].sym))))
			  	(yyval.typ) = p->info.typ;
			  else
			  {	p = enter(enumtable, (yyvsp[(2) - (2)].sym));
			  	(yyval.typ) = p->info.typ = mkenum((Table*)0);
				p->info.typ->id = (yyvsp[(2) - (2)].sym);
			  }
			;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 1144 "soapcpp2_yacc.y"
    { if ((p = entry(enumtable, (yyvsp[(2) - (2)].sym))))
				(yyval.typ) = p->info.typ;
			  else
			  {	p = enter(enumtable, (yyvsp[(2) - (2)].sym));
			  	(yyval.typ) = p->info.typ = mkenum((Table*)0);
				p->info.typ->id = (yyvsp[(2) - (2)].sym);
			  }
			;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 1152 "soapcpp2_yacc.y"
    { if ((p = entry(typetable, (yyvsp[(1) - (1)].sym))))
				(yyval.typ) = p->info.typ;
			  else if ((p = entry(classtable, (yyvsp[(1) - (1)].sym))))
			  	(yyval.typ) = p->info.typ;
			  else if ((p = entry(enumtable, (yyvsp[(1) - (1)].sym))))
			  	(yyval.typ) = p->info.typ;
			  else if ((yyvsp[(1) - (1)].sym) == lookup("std::string") || (yyvsp[(1) - (1)].sym) == lookup("std::wstring"))
			  {	p = enter(classtable, (yyvsp[(1) - (1)].sym));
				(yyval.typ) = p->info.typ = mkclass((Table*)0, 0);
			  	p->info.typ->id = (yyvsp[(1) - (1)].sym);
			  	p->info.typ->transient = -2;
			  }
			  else
			  {	sprintf(errbuf, "unknown type '%s'", (yyvsp[(1) - (1)].sym)->name);
				semerror(errbuf);
				(yyval.typ) = mkint();
			  }
			;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 1171 "soapcpp2_yacc.y"
    { if ((p = entry(templatetable, (yyvsp[(1) - (4)].sym))))
				(yyval.typ) = mktemplate((yyvsp[(3) - (4)].rec).typ, (yyvsp[(1) - (4)].sym));
			  else
			  {	sprintf(errbuf, "invalid template '%s'", (yyvsp[(1) - (4)].sym)->name);
				semerror(errbuf);
				(yyval.typ) = mkint();
			  }
			;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 1180 "soapcpp2_yacc.y"
    { if ((p = entry(classtable, (yyvsp[(2) - (2)].sym))))
			  {	if (p->info.typ->ref)
			   	{	if (!is_mutable(p->info.typ))
					{	sprintf(errbuf, "struct '%s' already declared at line %d", (yyvsp[(2) - (2)].sym)->name, p->lineno);
						semerror(errbuf);
					}
				}
				else
					p = reenter(classtable, (yyvsp[(2) - (2)].sym));
			  }
			  else
			  {	p = enter(classtable, (yyvsp[(2) - (2)].sym));
				p->info.typ = mkstruct((Table*)0, 0);
			  }
			  (yyval.e) = p;
			;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 1197 "soapcpp2_yacc.y"
    { if ((p = entry(classtable, (yyvsp[(2) - (2)].sym))))
			  {	if (p->info.typ->ref)
			   	{	if (!is_mutable(p->info.typ))
					{	sprintf(errbuf, "class '%s' already declared at line %d", (yyvsp[(2) - (2)].sym)->name, p->lineno);
						semerror(errbuf);
					}
				}
				else
					p = reenter(classtable, (yyvsp[(2) - (2)].sym));
			  }
			  else
			  {	p = enter(classtable, (yyvsp[(2) - (2)].sym));
				p->info.typ = mkclass((Table*)0, 0);
				p->info.typ->id = p->sym;
			  }
			  (yyvsp[(2) - (2)].sym)->token = TYPE;
			  (yyval.e) = p;
			;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 1216 "soapcpp2_yacc.y"
    { if ((p = entry(enumtable, (yyvsp[(2) - (2)].sym))))
			  {	if (p->info.typ->ref)
				{	sprintf(errbuf, "enum '%s' already declared at line %d", (yyvsp[(2) - (2)].sym)->name, p->lineno);
					semerror(errbuf);
				}
				/*
				else
					p = reenter(classtable, $2);
			  	*/
			  }
			  else
			  {	p = enter(enumtable, (yyvsp[(2) - (2)].sym));
				p->info.typ = mkenum(0);
			  }
			  (yyval.e) = p;
			;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 1233 "soapcpp2_yacc.y"
    { ;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 1234 "soapcpp2_yacc.y"
    { ;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 1236 "soapcpp2_yacc.y"
    { (yyval.e) = (yyvsp[(2) - (2)].e); ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 1237 "soapcpp2_yacc.y"
    { (yyval.e) = (yyvsp[(2) - (2)].e); ;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 1238 "soapcpp2_yacc.y"
    { (yyval.e) = (yyvsp[(2) - (2)].e); ;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 1239 "soapcpp2_yacc.y"
    { (yyval.e) = entry(classtable, (yyvsp[(1) - (1)].sym));
			  if (!(yyval.e))
			  {	p = entry(typetable, (yyvsp[(1) - (1)].sym));
			  	if (p && (p->info.typ->type == Tclass || p->info.typ->type == Tstruct))
					(yyval.e) = p;
			  }
			;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 1246 "soapcpp2_yacc.y"
    { (yyval.e) = entry(classtable, (yyvsp[(2) - (2)].sym)); ;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 1248 "soapcpp2_yacc.y"
    { if (transient == -2)
			  	transient = 0;
			  permission = 0;
			  enterscope(mktable(NULL), 0);
			  sp->entry = NULL;
			;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 1255 "soapcpp2_yacc.y"
    { if (transient == -2)
			  	transient = 0;
			  permission = 0;
			  enterscope(mktable(NULL), 0);
			  sp->entry = NULL;
			  sp->grow = False;
			;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 1263 "soapcpp2_yacc.y"
    { enterscope(mktable(NULL), 0);
			  sp->entry = NULL;
			  sp->mask = True;
			  sp->val = 1;
			;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 1269 "soapcpp2_yacc.y"
    { ;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 1270 "soapcpp2_yacc.y"
    { ;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 1272 "soapcpp2_yacc.y"
    { if (sp->table->level == INTERNAL)
			  	transient |= 1;
			  permission = 0;
			  enterscope(mktable(NULL), 0);
			  sp->entry = NULL;
			  sp->table->level = PARAM;
			;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 1280 "soapcpp2_yacc.y"
    { (yyval.sto) = Sauto; ;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 1281 "soapcpp2_yacc.y"
    { (yyval.sto) = Sregister; ;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 1282 "soapcpp2_yacc.y"
    { (yyval.sto) = Sstatic; ;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 1283 "soapcpp2_yacc.y"
    { (yyval.sto) = Sexplicit; ;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 1284 "soapcpp2_yacc.y"
    { (yyval.sto) = Sextern; transient = 1; ;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 1285 "soapcpp2_yacc.y"
    { (yyval.sto) = Stypedef; ;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 1286 "soapcpp2_yacc.y"
    { (yyval.sto) = Svirtual; ;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 1287 "soapcpp2_yacc.y"
    { (yyval.sto) = Sconst; ;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 1288 "soapcpp2_yacc.y"
    { (yyval.sto) = Sfriend; ;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 1289 "soapcpp2_yacc.y"
    { (yyval.sto) = Sinline; ;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 1290 "soapcpp2_yacc.y"
    { (yyval.sto) = SmustUnderstand; ;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 1291 "soapcpp2_yacc.y"
    { (yyval.sto) = Sreturn; ;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 1292 "soapcpp2_yacc.y"
    { (yyval.sto) = Sattribute;
			  if (eflag)
			   	semwarn("SOAP RPC encoding does not support XML attributes");
			;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 1296 "soapcpp2_yacc.y"
    { (yyval.sto) = Sspecial; ;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 1297 "soapcpp2_yacc.y"
    { (yyval.sto) = Sextern; transient = -2; ;}
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 1299 "soapcpp2_yacc.y"
    { (yyval.sto) = Snone; ;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 1300 "soapcpp2_yacc.y"
    { (yyval.sto) = Sconstobj; ;}
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 1302 "soapcpp2_yacc.y"
    { (yyval.sto) = Snone; ;}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 1303 "soapcpp2_yacc.y"
    { (yyval.sto) = Sabstract; ;}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 1305 "soapcpp2_yacc.y"
    { (yyval.sto) = Snone; ;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 1306 "soapcpp2_yacc.y"
    { (yyval.sto) = Svirtual; ;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 1308 "soapcpp2_yacc.y"
    { (yyval.rec) = tmp = sp->node; ;}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 1309 "soapcpp2_yacc.y"
    { /* handle const pointers, such as const char* */
			  if (/*tmp.typ->type == Tchar &&*/ (tmp.sto & Sconst))
			  	tmp.sto = (tmp.sto & ~Sconst) | Sconstptr;
			  tmp.typ = mkpointer(tmp.typ);
			  tmp.typ->transient = transient;
			  (yyval.rec) = tmp;
			;}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 1316 "soapcpp2_yacc.y"
    { tmp.typ = mkreference(tmp.typ);
			  tmp.typ->transient = transient;
			  (yyval.rec) = tmp;
			;}
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 1321 "soapcpp2_yacc.y"
    { (yyval.rec) = tmp;	/* tmp is inherited */
			;}
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 1324 "soapcpp2_yacc.y"
    { if (!bflag && (yyvsp[(4) - (4)].rec).typ->type == Tchar)
			  {	sprintf(errbuf, "char["SOAP_LONG_FORMAT"] will be serialized as an array of "SOAP_LONG_FORMAT" bytes: use soapcpp2 option -b to enable char[] string serialization or use char* for strings", (yyvsp[(2) - (4)].rec).val.i, (yyvsp[(2) - (4)].rec).val.i);
			  	semwarn(errbuf);
			  }
			  if ((yyvsp[(2) - (4)].rec).hasval && (yyvsp[(2) - (4)].rec).typ->type == Tint && (yyvsp[(2) - (4)].rec).val.i > 0 && (yyvsp[(4) - (4)].rec).typ->width > 0)
				(yyval.rec).typ = mkarray((yyvsp[(4) - (4)].rec).typ, (int) (yyvsp[(2) - (4)].rec).val.i * (yyvsp[(4) - (4)].rec).typ->width);
			  else
			  {	(yyval.rec).typ = mkarray((yyvsp[(4) - (4)].rec).typ, 0);
			  	semerror("undetermined array size");
			  }
			  (yyval.rec).sto = (yyvsp[(4) - (4)].rec).sto;
			;}
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 1336 "soapcpp2_yacc.y"
    { (yyval.rec).typ = mkpointer((yyvsp[(3) - (3)].rec).typ); /* zero size array = pointer */
			  (yyval.rec).sto = (yyvsp[(3) - (3)].rec).sto;
			;}
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 1340 "soapcpp2_yacc.y"
    { if ((yyvsp[(1) - (1)].rec).typ->type == Tstruct || (yyvsp[(1) - (1)].rec).typ->type == Tclass)
				if (!(yyvsp[(1) - (1)].rec).typ->ref && !(yyvsp[(1) - (1)].rec).typ->transient && !((yyvsp[(1) - (1)].rec).sto & Stypedef))
			   	{	sprintf(errbuf, "struct/class '%s' has incomplete type", (yyvsp[(1) - (1)].rec).typ->id->name);
					semerror(errbuf);
				}
			  (yyval.rec) = (yyvsp[(1) - (1)].rec);
			;}
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 1348 "soapcpp2_yacc.y"
    { (yyval.rec).hasval = False; ;}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 1349 "soapcpp2_yacc.y"
    { if ((yyvsp[(2) - (2)].rec).hasval)
			  {	(yyval.rec).typ = (yyvsp[(2) - (2)].rec).typ;
				(yyval.rec).hasval = True;
				(yyval.rec).val = (yyvsp[(2) - (2)].rec).val;
			  }
			  else
			  {	(yyval.rec).hasval = False;
				semerror("initialization expression not constant");
			  }
			;}
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 1361 "soapcpp2_yacc.y"
    { (yyval.rec).minOccurs = -1;
			  (yyval.rec).maxOccurs = 1;
			  (yyval.rec).pattern = (yyvsp[(1) - (1)].s);
			;}
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 1366 "soapcpp2_yacc.y"
    { (yyval.rec).minOccurs = (long)(yyvsp[(2) - (2)].i);
			  (yyval.rec).maxOccurs = 1;
			  (yyval.rec).pattern = (yyvsp[(1) - (2)].s);
			;}
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 1371 "soapcpp2_yacc.y"
    { (yyval.rec).minOccurs = (long)(yyvsp[(2) - (3)].i);
			  (yyval.rec).maxOccurs = 1;
			  (yyval.rec).pattern = (yyvsp[(1) - (3)].s);
			;}
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 1376 "soapcpp2_yacc.y"
    { (yyval.rec).minOccurs = (long)(yyvsp[(2) - (4)].i);
			  (yyval.rec).maxOccurs = (long)(yyvsp[(4) - (4)].i);
			  (yyval.rec).pattern = (yyvsp[(1) - (4)].s);
			;}
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 1381 "soapcpp2_yacc.y"
    { (yyval.rec).minOccurs = -1;
			  (yyval.rec).maxOccurs = (long)(yyvsp[(3) - (3)].i);
			  (yyval.rec).pattern = (yyvsp[(1) - (3)].s);
			;}
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 1386 "soapcpp2_yacc.y"
    { (yyval.s) = NULL; ;}
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 1387 "soapcpp2_yacc.y"
    { (yyval.s) = (yyvsp[(1) - (1)].s); ;}
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 1389 "soapcpp2_yacc.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); ;}
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 1390 "soapcpp2_yacc.y"
    { (yyval.i) = (yyvsp[(2) - (2)].i); ;}
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 1391 "soapcpp2_yacc.y"
    { (yyval.i) = -(yyvsp[(2) - (2)].i); ;}
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 1400 "soapcpp2_yacc.y"
    { (yyval.rec) = (yyvsp[(3) - (3)].rec); ;}
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 1401 "soapcpp2_yacc.y"
    { (yyval.rec) = (yyvsp[(1) - (1)].rec); ;}
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 1405 "soapcpp2_yacc.y"
    { (yyval.rec).typ = (yyvsp[(3) - (5)].rec).typ;
			  (yyval.rec).sto = Snone;
			  (yyval.rec).hasval = False;
			;}
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 1412 "soapcpp2_yacc.y"
    { (yyval.rec) = (yyvsp[(1) - (1)].rec); ;}
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 1415 "soapcpp2_yacc.y"
    { (yyval.rec).hasval = False;
			  (yyval.rec).typ = mkint();
			;}
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 1418 "soapcpp2_yacc.y"
    { (yyval.rec) = (yyvsp[(1) - (1)].rec); ;}
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 1420 "soapcpp2_yacc.y"
    { (yyval.rec) = (yyvsp[(1) - (1)].rec); ;}
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 1423 "soapcpp2_yacc.y"
    { (yyval.rec).hasval = False;
			  (yyval.rec).typ = mkint();
			;}
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 1426 "soapcpp2_yacc.y"
    { (yyval.rec) = (yyvsp[(1) - (1)].rec); ;}
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 1428 "soapcpp2_yacc.y"
    { (yyval.rec) = (yyvsp[(1) - (1)].rec); ;}
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 1431 "soapcpp2_yacc.y"
    { (yyval.rec) = iop("|", (yyvsp[(1) - (3)].rec), (yyvsp[(3) - (3)].rec)); ;}
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 1432 "soapcpp2_yacc.y"
    { (yyval.rec) = iop("^", (yyvsp[(1) - (3)].rec), (yyvsp[(3) - (3)].rec)); ;}
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 1433 "soapcpp2_yacc.y"
    { (yyval.rec) = iop("&", (yyvsp[(1) - (3)].rec), (yyvsp[(3) - (3)].rec)); ;}
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 1434 "soapcpp2_yacc.y"
    { (yyval.rec) = relop("==", (yyvsp[(1) - (3)].rec), (yyvsp[(3) - (3)].rec)); ;}
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 1435 "soapcpp2_yacc.y"
    { (yyval.rec) = relop("!=", (yyvsp[(1) - (3)].rec), (yyvsp[(3) - (3)].rec)); ;}
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 1436 "soapcpp2_yacc.y"
    { (yyval.rec) = relop("<", (yyvsp[(1) - (3)].rec), (yyvsp[(3) - (3)].rec)); ;}
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 1437 "soapcpp2_yacc.y"
    { (yyval.rec) = relop("<=", (yyvsp[(1) - (3)].rec), (yyvsp[(3) - (3)].rec)); ;}
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 1438 "soapcpp2_yacc.y"
    { (yyval.rec) = relop(">", (yyvsp[(1) - (3)].rec), (yyvsp[(3) - (3)].rec)); ;}
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 1439 "soapcpp2_yacc.y"
    { (yyval.rec) = relop(">=", (yyvsp[(1) - (3)].rec), (yyvsp[(3) - (3)].rec)); ;}
    break;

  case 206:

/* Line 1455 of yacc.c  */
#line 1440 "soapcpp2_yacc.y"
    { (yyval.rec) = iop("<<", (yyvsp[(1) - (3)].rec), (yyvsp[(3) - (3)].rec)); ;}
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 1441 "soapcpp2_yacc.y"
    { (yyval.rec) = iop(">>", (yyvsp[(1) - (3)].rec), (yyvsp[(3) - (3)].rec)); ;}
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 1442 "soapcpp2_yacc.y"
    { (yyval.rec) = op("+", (yyvsp[(1) - (3)].rec), (yyvsp[(3) - (3)].rec)); ;}
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 1443 "soapcpp2_yacc.y"
    { (yyval.rec) = op("-", (yyvsp[(1) - (3)].rec), (yyvsp[(3) - (3)].rec)); ;}
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 1444 "soapcpp2_yacc.y"
    { (yyval.rec) = op("*", (yyvsp[(1) - (3)].rec), (yyvsp[(3) - (3)].rec)); ;}
    break;

  case 211:

/* Line 1455 of yacc.c  */
#line 1445 "soapcpp2_yacc.y"
    { (yyval.rec) = op("/", (yyvsp[(1) - (3)].rec), (yyvsp[(3) - (3)].rec)); ;}
    break;

  case 212:

/* Line 1455 of yacc.c  */
#line 1446 "soapcpp2_yacc.y"
    { (yyval.rec) = iop("%", (yyvsp[(1) - (3)].rec), (yyvsp[(3) - (3)].rec)); ;}
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 1447 "soapcpp2_yacc.y"
    { (yyval.rec) = (yyvsp[(1) - (1)].rec); ;}
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 1450 "soapcpp2_yacc.y"
    { if ((yyvsp[(2) - (2)].rec).hasval)
				(yyval.rec).val.i = !(yyvsp[(2) - (2)].rec).val.i;
			  (yyval.rec).typ = (yyvsp[(2) - (2)].rec).typ;
			  (yyval.rec).hasval = (yyvsp[(2) - (2)].rec).hasval;
			;}
    break;

  case 215:

/* Line 1455 of yacc.c  */
#line 1455 "soapcpp2_yacc.y"
    { if ((yyvsp[(2) - (2)].rec).hasval)
				(yyval.rec).val.i = ~(yyvsp[(2) - (2)].rec).val.i;
			  (yyval.rec).typ = (yyvsp[(2) - (2)].rec).typ;
			  (yyval.rec).hasval = (yyvsp[(2) - (2)].rec).hasval;
			;}
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 1460 "soapcpp2_yacc.y"
    { if ((yyvsp[(2) - (2)].rec).hasval) {
				if (integer((yyvsp[(2) - (2)].rec).typ))
					(yyval.rec).val.i = -(yyvsp[(2) - (2)].rec).val.i;
				else if (real((yyvsp[(2) - (2)].rec).typ))
					(yyval.rec).val.r = -(yyvsp[(2) - (2)].rec).val.r;
				else	typerror("string?");
			  }
			  (yyval.rec).typ = (yyvsp[(2) - (2)].rec).typ;
			  (yyval.rec).hasval = (yyvsp[(2) - (2)].rec).hasval;
			;}
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 1470 "soapcpp2_yacc.y"
    { (yyval.rec) = (yyvsp[(2) - (2)].rec); ;}
    break;

  case 218:

/* Line 1455 of yacc.c  */
#line 1471 "soapcpp2_yacc.y"
    { if ((yyvsp[(2) - (2)].rec).typ->type == Tpointer) {
			  	(yyval.rec).typ = (Tnode*) (yyvsp[(2) - (2)].rec).typ->ref;
			  } else
			  	typerror("dereference of non-pointer type");
			  (yyval.rec).sto = Snone;
			  (yyval.rec).hasval = False;
			;}
    break;

  case 219:

/* Line 1455 of yacc.c  */
#line 1478 "soapcpp2_yacc.y"
    { (yyval.rec).typ = mkpointer((yyvsp[(2) - (2)].rec).typ);
			  (yyval.rec).sto = Snone;
			  (yyval.rec).hasval = False;
			;}
    break;

  case 220:

/* Line 1455 of yacc.c  */
#line 1483 "soapcpp2_yacc.y"
    { (yyval.rec).hasval = True;
			  (yyval.rec).typ = mkint();
			  (yyval.rec).val.i = (yyvsp[(3) - (4)].rec).typ->width;
			;}
    break;

  case 221:

/* Line 1455 of yacc.c  */
#line 1487 "soapcpp2_yacc.y"
    { (yyval.rec) = (yyvsp[(1) - (1)].rec); ;}
    break;

  case 222:

/* Line 1455 of yacc.c  */
#line 1490 "soapcpp2_yacc.y"
    { (yyval.rec) = (yyvsp[(2) - (3)].rec); ;}
    break;

  case 223:

/* Line 1455 of yacc.c  */
#line 1491 "soapcpp2_yacc.y"
    { if ((p = enumentry((yyvsp[(1) - (1)].sym))) == (Entry*) 0)
				p = undefined((yyvsp[(1) - (1)].sym));
			  else
			  	(yyval.rec).hasval = True;
			  (yyval.rec).typ = p->info.typ;
			  (yyval.rec).val = p->info.val;
			;}
    break;

  case 224:

/* Line 1455 of yacc.c  */
#line 1498 "soapcpp2_yacc.y"
    { (yyval.rec).typ = mkint();
			  (yyval.rec).hasval = True;
			  (yyval.rec).val.i = (yyvsp[(1) - (1)].i);
			;}
    break;

  case 225:

/* Line 1455 of yacc.c  */
#line 1502 "soapcpp2_yacc.y"
    { (yyval.rec).typ = mkint();
			  (yyval.rec).hasval = True;
			  (yyval.rec).val.i = 0;
			;}
    break;

  case 226:

/* Line 1455 of yacc.c  */
#line 1506 "soapcpp2_yacc.y"
    { (yyval.rec).typ = mkfloat();
			  (yyval.rec).hasval = True;
			  (yyval.rec).val.r = (yyvsp[(1) - (1)].r);
			;}
    break;

  case 227:

/* Line 1455 of yacc.c  */
#line 1510 "soapcpp2_yacc.y"
    { (yyval.rec).typ = mkchar();
			  (yyval.rec).hasval = True;
			  (yyval.rec).val.i = (yyvsp[(1) - (1)].c);
			;}
    break;

  case 228:

/* Line 1455 of yacc.c  */
#line 1514 "soapcpp2_yacc.y"
    { (yyval.rec).typ = mkstring();
			  (yyval.rec).hasval = True;
			  (yyval.rec).val.s = (yyvsp[(1) - (1)].s);
			;}
    break;

  case 229:

/* Line 1455 of yacc.c  */
#line 1518 "soapcpp2_yacc.y"
    { (yyval.rec).typ = mkbool();
			  (yyval.rec).hasval = True;
			  (yyval.rec).val.i = 0;
			;}
    break;

  case 230:

/* Line 1455 of yacc.c  */
#line 1522 "soapcpp2_yacc.y"
    { (yyval.rec).typ = mkbool();
			  (yyval.rec).hasval = True;
			  (yyval.rec).val.i = 1;
			;}
    break;



/* Line 1455 of yacc.c  */
#line 4546 "soapcpp2_yacc.tab.c"
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

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
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



/* Line 1675 of yacc.c  */
#line 1528 "soapcpp2_yacc.y"


/*
 * ???
 */
int
yywrap()
{	return 1;
}

/******************************************************************************\

	Support routines

\******************************************************************************/

static Node
op(const char *op, Node p, Node q)
{	Node	r;
	Tnode	*typ;
	r.typ = p.typ;
	r.sto = Snone;
	if (p.hasval && q.hasval) {
		if (integer(p.typ) && integer(q.typ))
			switch (op[0]) {
			case '|':	r.val.i = p.val.i |  q.val.i; break;
			case '^':	r.val.i = p.val.i ^  q.val.i; break;
			case '&':	r.val.i = p.val.i &  q.val.i; break;
			case '<':	r.val.i = p.val.i << q.val.i; break;
			case '>':	r.val.i = p.val.i >> q.val.i; break;
			case '+':	r.val.i = p.val.i +  q.val.i; break;
			case '-':	r.val.i = p.val.i -  q.val.i; break;
			case '*':	r.val.i = p.val.i *  q.val.i; break;
			case '/':	r.val.i = p.val.i /  q.val.i; break;
			case '%':	r.val.i = p.val.i %  q.val.i; break;
			default:	typerror(op);
			}
		else if (real(p.typ) && real(q.typ))
			switch (op[0]) {
			case '+':	r.val.r = p.val.r + q.val.r; break;
			case '-':	r.val.r = p.val.r - q.val.r; break;
			case '*':	r.val.r = p.val.r * q.val.r; break;
			case '/':	r.val.r = p.val.r / q.val.r; break;
			default:	typerror(op);
			}
		else	semerror("illegal constant operation");
		r.hasval = True;
	} else {
		typ = mgtype(p.typ, q.typ);
		r.hasval = False;
	}
	return r;
}

static Node
iop(const char *iop, Node p, Node q)
{	if (integer(p.typ) && integer(q.typ))
		return op(iop, p, q);
	typerror("integer operands only");
	return p;
}

static Node
relop(const char *op, Node p, Node q)
{	Node	r;
	Tnode	*typ;
	r.typ = mkint();
	r.sto = Snone;
	r.hasval = False;
	if (p.typ->type != Tpointer || p.typ != q.typ)
		typ = mgtype(p.typ, q.typ);
	return r;
}

/******************************************************************************\

	Scope management

\******************************************************************************/

/*
mkscope - initialize scope stack with a new table and offset
*/
static void
mkscope(Table *table, int offset)
{	sp = stack-1;
	enterscope(table, offset);
}

/*
enterscope - enter a new scope by pushing a new table and offset on the stack
*/
static void
enterscope(Table *table, int offset)
{	if (++sp == stack+MAXNEST)
		execerror("maximum scope depth exceeded");
	sp->table = table;
	sp->val = 0;
	sp->offset = offset;
	sp->grow = True;	/* by default, offset grows */
	sp->mask = False;
}

/*
exitscope - exit a scope by popping the table and offset from the stack
*/
static void
exitscope()
{	check(sp-- != stack, "exitscope() has no matching enterscope()");
}

/******************************************************************************\

	Undefined symbol

\******************************************************************************/

static Entry*
undefined(Symbol *sym)
{	Entry	*p;
	sprintf(errbuf, "undefined identifier '%s'", sym->name);
	semwarn(errbuf);
	p = enter(sp->table, sym);
	p->level = GLOBAL;
	p->info.typ = mkint();
	p->info.sto = Sextern;
	p->info.hasval = False;
	return p;
}

/*
mgtype - return most general type among two numerical types
*/
Tnode*
mgtype(Tnode *typ1, Tnode *typ2)
{	if (numeric(typ1) && numeric(typ2)) {
		if (typ1->type < typ2->type)
			return typ2;
	} else	typerror("non-numeric type");
	return typ1;
}

/******************************************************************************\

	Type checks

\******************************************************************************/

static int
integer(Tnode *typ)
{	switch (typ->type) {
	case Tchar:
	case Tshort:
	case Tint:
	case Tlong:	return True;
	default:	break;
	}
	return False;
}

static int
real(Tnode *typ)
{	switch (typ->type) {
	case Tfloat:
	case Tdouble:
	case Tldouble:	return True;
	default:	break;
	}
	return False;
}

static int
numeric(Tnode *typ)
{	return integer(typ) || real(typ);
}

static void
add_fault(Table *gt)
{ Table *t;
  Entry *p1, *p2, *p3, *p4;
  Symbol *s1, *s2, *s3, *s4;
  imported = NULL;
  s1 = lookup("SOAP_ENV__Code");
  p1 = entry(classtable, s1);
  if (!p1 || !p1->info.typ->ref)
  { t = mktable((Table*)0);
    if (!p1)
    { p1 = enter(classtable, s1);
      p1->info.typ = mkstruct(t, 3*4);
      p1->info.typ->id = s1;
    }
    else
      p1->info.typ->ref = t;
    p2 = enter(t, lookup("SOAP_ENV__Value"));
    p2->info.typ = qname;
    p2->info.minOccurs = 0;
    p2 = enter(t, lookup("SOAP_ENV__Subcode"));
    p2->info.typ = mkpointer(p1->info.typ);
    p2->info.minOccurs = 0;
  }
  s2 = lookup("SOAP_ENV__Detail");
  p2 = entry(classtable, s2);
  if (!p2 || !p2->info.typ->ref)
  { t = mktable((Table*)0);
    if (!p2)
    { p2 = enter(classtable, s2);
      p2->info.typ = mkstruct(t, 3*4);
      p2->info.typ->id = s2;
    }
    else
      p2->info.typ->ref = t;
    p3 = enter(t, lookup("__type"));
    p3->info.typ = mkint();
    p3->info.minOccurs = 0;
    p3 = enter(t, lookup("fault"));
    p3->info.typ = mkpointer(mkvoid());
    p3->info.minOccurs = 0;
    p3 = enter(t, lookup("__any"));
    p3->info.typ = xml;
    p3->info.minOccurs = 0;
    custom_fault = 0;
  }
  s4 = lookup("SOAP_ENV__Reason");
  p4 = entry(classtable, s4);
  if (!p4 || !p4->info.typ->ref)
  { t = mktable((Table*)0);
    if (!p4)
    { p4 = enter(classtable, s4);
      p4->info.typ = mkstruct(t, 4);
      p4->info.typ->id = s4;
    }
    else
      p4->info.typ->ref = t;
    p3 = enter(t, lookup("SOAP_ENV__Text"));
    p3->info.typ = mkstring();
    p3->info.minOccurs = 0;
  }
  s3 = lookup("SOAP_ENV__Fault");
  p3 = entry(classtable, s3);
  if (!p3 || !p3->info.typ->ref)
  { t = mktable(NULL);
    if (!p3)
    { p3 = enter(classtable, s3);
      p3->info.typ = mkstruct(t, 9*4);
      p3->info.typ->id = s3;
    }
    else
      p3->info.typ->ref = t;
    p3 = enter(t, lookup("faultcode"));
    p3->info.typ = qname;
    p3->info.minOccurs = 0;
    p3 = enter(t, lookup("faultstring"));
    p3->info.typ = mkstring();
    p3->info.minOccurs = 0;
    p3 = enter(t, lookup("faultactor"));
    p3->info.typ = mkstring();
    p3->info.minOccurs = 0;
    p3 = enter(t, lookup("detail"));
    p3->info.typ = mkpointer(p2->info.typ);
    p3->info.minOccurs = 0;
    p3 = enter(t, s1);
    p3->info.typ = mkpointer(p1->info.typ);
    p3->info.minOccurs = 0;
    p3 = enter(t, s4);
    p3->info.typ = mkpointer(p4->info.typ);
    p3->info.minOccurs = 0;
    p3 = enter(t, lookup("SOAP_ENV__Node"));
    p3->info.typ = mkstring();
    p3->info.minOccurs = 0;
    p3 = enter(t, lookup("SOAP_ENV__Role"));
    p3->info.typ = mkstring();
    p3->info.minOccurs = 0;
    p3 = enter(t, lookup("SOAP_ENV__Detail"));
    p3->info.typ = mkpointer(p2->info.typ);
    p3->info.minOccurs = 0;
  }
}

static void
add_soap()
{ Symbol *s = lookup("soap");
  p = enter(classtable, s);
  p->info.typ = mkstruct(NULL, 0);
  p->info.typ->transient = -2;
  p->info.typ->id = s;
}

static void
add_XML()
{ Symbol *s = lookup("_XML");
  p = enter(typetable, s);
  xml = p->info.typ = mksymtype(mkstring(), s);
  p->info.sto = Stypedef;
}

static void
add_qname()
{ Symbol *s = lookup("_QName");
  p = enter(typetable, s);
  qname = p->info.typ = mksymtype(mkstring(), s);
  p->info.sto = Stypedef;
}

static void
add_header(Table *gt)
{ Table *t;
  Entry *p;
  Symbol *s = lookup("SOAP_ENV__Header");
  imported = NULL;
  p = entry(classtable, s);
  if (!p || !p->info.typ->ref)
  { t = mktable((Table*)0);
    if (!p)
      p = enter(classtable, s);
    p->info.typ = mkstruct(t, 0);
    p->info.typ->id = s;
    custom_header = 0;
  }
}

static void
add_response(Entry *fun, Entry *ret)
{ Table *t;
  Entry *p, *q;
  Symbol *s;
  size_t n = strlen(fun->sym->name);
  char *r = (char*)emalloc(n+9);
  strcpy(r, fun->sym->name);
  strcat(r, "Response");
  if (!(s = lookup(r)))
    s = install(r, ID);
  free(r);
  t = mktable((Table*)0);
  q = enter(t, ret->sym);
  q->info = ret->info;
  if (q->info.typ->type == Treference)
    q->info.typ = (Tnode*)q->info.typ->ref;
  p = enter(classtable, s);
  p->info.typ = mkstruct(t, 4);
  p->info.typ->id = s;
  fun->info.typ->response = p;
}

static void
add_result(Tnode *typ)
{ Entry *p;
  if (!typ->ref || !((Tnode*)typ->ref)->ref)
  { semwarn("response struct/class must be declared before used in function prototype");
    return;
  }
  for (p = ((Table*)((Tnode*)typ->ref)->ref)->list; p; p = p->next)
    if (p->info.sto & Sreturn)
      return;
  for (p = ((Table*)((Tnode*)typ->ref)->ref)->list; p; p = p->next)
  { if (p->info.typ->type != Tfun && !(p->info.sto & Sattribute) && !is_transient(p->info.typ) && !(p->info.sto & (Sprivate|Sprotected)))
      p->info.sto = (Storage)((int)p->info.sto | (int)Sreturn);
      return;
  }
}

