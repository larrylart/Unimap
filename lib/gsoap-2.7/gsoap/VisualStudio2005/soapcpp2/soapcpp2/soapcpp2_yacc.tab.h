
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
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

/* Line 1676 of yacc.c  */
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



/* Line 1676 of yacc.c  */
#line 153 "soapcpp2_yacc.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


