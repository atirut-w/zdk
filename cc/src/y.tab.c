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
#line 1 "/home/atirut/Projects/zdk/cc/src/parser.y"

#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

/* externs from lexer for location */
extern int line;
extern int column;
/* start position of the most recently scanned token */
extern int token_line;
extern int token_column;

/* Build result */
static struct ASTNode *parse_tree = 0;


#line 88 "/home/atirut/Projects/zdk/cc/src/y.tab.c"

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

#include "y.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_IDENTIFIER = 3,                 /* IDENTIFIER  */
  YYSYMBOL_CONSTANT = 4,                   /* CONSTANT  */
  YYSYMBOL_STRING_LITERAL = 5,             /* STRING_LITERAL  */
  YYSYMBOL_TYPE_NAME = 6,                  /* TYPE_NAME  */
  YYSYMBOL_SIZEOF = 7,                     /* SIZEOF  */
  YYSYMBOL_PTR_OP = 8,                     /* PTR_OP  */
  YYSYMBOL_INC_OP = 9,                     /* INC_OP  */
  YYSYMBOL_DEC_OP = 10,                    /* DEC_OP  */
  YYSYMBOL_LEFT_OP = 11,                   /* LEFT_OP  */
  YYSYMBOL_RIGHT_OP = 12,                  /* RIGHT_OP  */
  YYSYMBOL_LE_OP = 13,                     /* LE_OP  */
  YYSYMBOL_GE_OP = 14,                     /* GE_OP  */
  YYSYMBOL_EQ_OP = 15,                     /* EQ_OP  */
  YYSYMBOL_NE_OP = 16,                     /* NE_OP  */
  YYSYMBOL_AND_OP = 17,                    /* AND_OP  */
  YYSYMBOL_OR_OP = 18,                     /* OR_OP  */
  YYSYMBOL_MUL_ASSIGN = 19,                /* MUL_ASSIGN  */
  YYSYMBOL_DIV_ASSIGN = 20,                /* DIV_ASSIGN  */
  YYSYMBOL_MOD_ASSIGN = 21,                /* MOD_ASSIGN  */
  YYSYMBOL_ADD_ASSIGN = 22,                /* ADD_ASSIGN  */
  YYSYMBOL_SUB_ASSIGN = 23,                /* SUB_ASSIGN  */
  YYSYMBOL_LEFT_ASSIGN = 24,               /* LEFT_ASSIGN  */
  YYSYMBOL_RIGHT_ASSIGN = 25,              /* RIGHT_ASSIGN  */
  YYSYMBOL_AND_ASSIGN = 26,                /* AND_ASSIGN  */
  YYSYMBOL_XOR_ASSIGN = 27,                /* XOR_ASSIGN  */
  YYSYMBOL_OR_ASSIGN = 28,                 /* OR_ASSIGN  */
  YYSYMBOL_TYPEDEF = 29,                   /* TYPEDEF  */
  YYSYMBOL_EXTERN = 30,                    /* EXTERN  */
  YYSYMBOL_STATIC = 31,                    /* STATIC  */
  YYSYMBOL_AUTO = 32,                      /* AUTO  */
  YYSYMBOL_REGISTER = 33,                  /* REGISTER  */
  YYSYMBOL_CHAR = 34,                      /* CHAR  */
  YYSYMBOL_SHORT = 35,                     /* SHORT  */
  YYSYMBOL_INT = 36,                       /* INT  */
  YYSYMBOL_LONG = 37,                      /* LONG  */
  YYSYMBOL_SIGNED = 38,                    /* SIGNED  */
  YYSYMBOL_UNSIGNED = 39,                  /* UNSIGNED  */
  YYSYMBOL_FLOAT = 40,                     /* FLOAT  */
  YYSYMBOL_DOUBLE = 41,                    /* DOUBLE  */
  YYSYMBOL_CONST = 42,                     /* CONST  */
  YYSYMBOL_VOLATILE = 43,                  /* VOLATILE  */
  YYSYMBOL_VOID = 44,                      /* VOID  */
  YYSYMBOL_STRUCT = 45,                    /* STRUCT  */
  YYSYMBOL_UNION = 46,                     /* UNION  */
  YYSYMBOL_ENUM = 47,                      /* ENUM  */
  YYSYMBOL_ELLIPSIS = 48,                  /* ELLIPSIS  */
  YYSYMBOL_CASE = 49,                      /* CASE  */
  YYSYMBOL_DEFAULT = 50,                   /* DEFAULT  */
  YYSYMBOL_IF = 51,                        /* IF  */
  YYSYMBOL_ELSE = 52,                      /* ELSE  */
  YYSYMBOL_SWITCH = 53,                    /* SWITCH  */
  YYSYMBOL_WHILE = 54,                     /* WHILE  */
  YYSYMBOL_DO = 55,                        /* DO  */
  YYSYMBOL_FOR = 56,                       /* FOR  */
  YYSYMBOL_GOTO = 57,                      /* GOTO  */
  YYSYMBOL_CONTINUE = 58,                  /* CONTINUE  */
  YYSYMBOL_BREAK = 59,                     /* BREAK  */
  YYSYMBOL_RETURN = 60,                    /* RETURN  */
  YYSYMBOL_61_ = 61,                       /* '('  */
  YYSYMBOL_62_ = 62,                       /* ')'  */
  YYSYMBOL_63_ = 63,                       /* '['  */
  YYSYMBOL_64_ = 64,                       /* ']'  */
  YYSYMBOL_65_ = 65,                       /* '.'  */
  YYSYMBOL_66_ = 66,                       /* ','  */
  YYSYMBOL_67_ = 67,                       /* '&'  */
  YYSYMBOL_68_ = 68,                       /* '*'  */
  YYSYMBOL_69_ = 69,                       /* '+'  */
  YYSYMBOL_70_ = 70,                       /* '-'  */
  YYSYMBOL_71_ = 71,                       /* '~'  */
  YYSYMBOL_72_ = 72,                       /* '!'  */
  YYSYMBOL_73_ = 73,                       /* '/'  */
  YYSYMBOL_74_ = 74,                       /* '%'  */
  YYSYMBOL_75_ = 75,                       /* '<'  */
  YYSYMBOL_76_ = 76,                       /* '>'  */
  YYSYMBOL_77_ = 77,                       /* '^'  */
  YYSYMBOL_78_ = 78,                       /* '|'  */
  YYSYMBOL_79_ = 79,                       /* '?'  */
  YYSYMBOL_80_ = 80,                       /* ':'  */
  YYSYMBOL_81_ = 81,                       /* '='  */
  YYSYMBOL_82_ = 82,                       /* ';'  */
  YYSYMBOL_83_ = 83,                       /* '{'  */
  YYSYMBOL_84_ = 84,                       /* '}'  */
  YYSYMBOL_YYACCEPT = 85,                  /* $accept  */
  YYSYMBOL_primary_expression = 86,        /* primary_expression  */
  YYSYMBOL_postfix_expression = 87,        /* postfix_expression  */
  YYSYMBOL_argument_expression_list = 88,  /* argument_expression_list  */
  YYSYMBOL_unary_expression = 89,          /* unary_expression  */
  YYSYMBOL_unary_operator = 90,            /* unary_operator  */
  YYSYMBOL_cast_expression = 91,           /* cast_expression  */
  YYSYMBOL_multiplicative_expression = 92, /* multiplicative_expression  */
  YYSYMBOL_additive_expression = 93,       /* additive_expression  */
  YYSYMBOL_shift_expression = 94,          /* shift_expression  */
  YYSYMBOL_relational_expression = 95,     /* relational_expression  */
  YYSYMBOL_equality_expression = 96,       /* equality_expression  */
  YYSYMBOL_and_expression = 97,            /* and_expression  */
  YYSYMBOL_exclusive_or_expression = 98,   /* exclusive_or_expression  */
  YYSYMBOL_inclusive_or_expression = 99,   /* inclusive_or_expression  */
  YYSYMBOL_logical_and_expression = 100,   /* logical_and_expression  */
  YYSYMBOL_logical_or_expression = 101,    /* logical_or_expression  */
  YYSYMBOL_conditional_expression = 102,   /* conditional_expression  */
  YYSYMBOL_assignment_expression = 103,    /* assignment_expression  */
  YYSYMBOL_assignment_operator = 104,      /* assignment_operator  */
  YYSYMBOL_expression = 105,               /* expression  */
  YYSYMBOL_constant_expression = 106,      /* constant_expression  */
  YYSYMBOL_declaration = 107,              /* declaration  */
  YYSYMBOL_declaration_specifiers = 108,   /* declaration_specifiers  */
  YYSYMBOL_init_declarator_list = 109,     /* init_declarator_list  */
  YYSYMBOL_init_declarator = 110,          /* init_declarator  */
  YYSYMBOL_storage_class_specifier = 111,  /* storage_class_specifier  */
  YYSYMBOL_type_specifier = 112,           /* type_specifier  */
  YYSYMBOL_struct_or_union_specifier = 113, /* struct_or_union_specifier  */
  YYSYMBOL_struct_or_union = 114,          /* struct_or_union  */
  YYSYMBOL_struct_declaration_list = 115,  /* struct_declaration_list  */
  YYSYMBOL_struct_declaration = 116,       /* struct_declaration  */
  YYSYMBOL_specifier_qualifier_list = 117, /* specifier_qualifier_list  */
  YYSYMBOL_struct_declarator_list = 118,   /* struct_declarator_list  */
  YYSYMBOL_struct_declarator = 119,        /* struct_declarator  */
  YYSYMBOL_enum_specifier = 120,           /* enum_specifier  */
  YYSYMBOL_enumerator_list = 121,          /* enumerator_list  */
  YYSYMBOL_enumerator = 122,               /* enumerator  */
  YYSYMBOL_type_qualifier = 123,           /* type_qualifier  */
  YYSYMBOL_declarator = 124,               /* declarator  */
  YYSYMBOL_direct_declarator = 125,        /* direct_declarator  */
  YYSYMBOL_pointer = 126,                  /* pointer  */
  YYSYMBOL_type_qualifier_list = 127,      /* type_qualifier_list  */
  YYSYMBOL_parameter_type_list = 128,      /* parameter_type_list  */
  YYSYMBOL_parameter_list = 129,           /* parameter_list  */
  YYSYMBOL_parameter_declaration = 130,    /* parameter_declaration  */
  YYSYMBOL_identifier_list = 131,          /* identifier_list  */
  YYSYMBOL_type_name = 132,                /* type_name  */
  YYSYMBOL_abstract_declarator = 133,      /* abstract_declarator  */
  YYSYMBOL_direct_abstract_declarator = 134, /* direct_abstract_declarator  */
  YYSYMBOL_initializer = 135,              /* initializer  */
  YYSYMBOL_initializer_list = 136,         /* initializer_list  */
  YYSYMBOL_statement = 137,                /* statement  */
  YYSYMBOL_labeled_statement = 138,        /* labeled_statement  */
  YYSYMBOL_compound_statement = 139,       /* compound_statement  */
  YYSYMBOL_declaration_list = 140,         /* declaration_list  */
  YYSYMBOL_statement_list = 141,           /* statement_list  */
  YYSYMBOL_expression_statement = 142,     /* expression_statement  */
  YYSYMBOL_selection_statement = 143,      /* selection_statement  */
  YYSYMBOL_iteration_statement = 144,      /* iteration_statement  */
  YYSYMBOL_jump_statement = 145,           /* jump_statement  */
  YYSYMBOL_translation_unit = 146,         /* translation_unit  */
  YYSYMBOL_external_declaration = 147,     /* external_declaration  */
  YYSYMBOL_function_definition = 148       /* function_definition  */
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
typedef yytype_int16 yy_state_t;

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
#define YYFINAL  61
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1240

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  85
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  64
/* YYNRULES -- Number of rules.  */
#define YYNRULES  212
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  350

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   315


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
       2,     2,     2,    72,     2,     2,     2,    74,    67,     2,
      61,    62,    68,    69,    66,    70,    65,    73,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    80,    82,
      75,    81,    76,    79,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    63,     2,    64,    77,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    83,    78,    84,    71,     2,     2,     2,
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
      55,    56,    57,    58,    59,    60
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    51,    51,    52,    53,    54,    58,    59,    60,    61,
      62,    63,    64,    65,    69,    70,    74,    75,    76,    77,
      78,    79,    83,    84,    85,    86,    87,    88,    92,    93,
      97,    98,    99,   100,   104,   105,   106,   110,   111,   112,
     116,   117,   118,   119,   120,   124,   125,   126,   130,   131,
     135,   136,   140,   141,   145,   146,   150,   151,   155,   156,
     160,   161,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   179,   180,   184,   188,   189,   193,   194,
     195,   196,   197,   198,   202,   203,   207,   208,   212,   213,
     214,   215,   216,   220,   221,   222,   223,   224,   225,   226,
     227,   228,   229,   230,   231,   235,   236,   237,   241,   242,
     246,   247,   251,   255,   256,   257,   258,   262,   263,   267,
     268,   269,   273,   274,   275,   279,   280,   284,   285,   289,
     290,   294,   295,   299,   300,   301,   302,   303,   304,   305,
     309,   310,   311,   312,   316,   317,   322,   323,   327,   328,
     332,   333,   334,   338,   339,   343,   344,   348,   349,   350,
     354,   355,   356,   357,   358,   359,   360,   361,   362,   366,
     367,   368,   372,   373,   377,   378,   379,   380,   381,   382,
     386,   387,   388,   392,   393,   394,   395,   399,   400,   404,
     405,   409,   410,   414,   415,   416,   420,   421,   422,   423,
     427,   428,   429,   430,   431,   435,   436,   440,   441,   445,
     446,   447,   448
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
  "\"end of file\"", "error", "\"invalid token\"", "IDENTIFIER",
  "CONSTANT", "STRING_LITERAL", "TYPE_NAME", "SIZEOF", "PTR_OP", "INC_OP",
  "DEC_OP", "LEFT_OP", "RIGHT_OP", "LE_OP", "GE_OP", "EQ_OP", "NE_OP",
  "AND_OP", "OR_OP", "MUL_ASSIGN", "DIV_ASSIGN", "MOD_ASSIGN",
  "ADD_ASSIGN", "SUB_ASSIGN", "LEFT_ASSIGN", "RIGHT_ASSIGN", "AND_ASSIGN",
  "XOR_ASSIGN", "OR_ASSIGN", "TYPEDEF", "EXTERN", "STATIC", "AUTO",
  "REGISTER", "CHAR", "SHORT", "INT", "LONG", "SIGNED", "UNSIGNED",
  "FLOAT", "DOUBLE", "CONST", "VOLATILE", "VOID", "STRUCT", "UNION",
  "ENUM", "ELLIPSIS", "CASE", "DEFAULT", "IF", "ELSE", "SWITCH", "WHILE",
  "DO", "FOR", "GOTO", "CONTINUE", "BREAK", "RETURN", "'('", "')'", "'['",
  "']'", "'.'", "','", "'&'", "'*'", "'+'", "'-'", "'~'", "'!'", "'/'",
  "'%'", "'<'", "'>'", "'^'", "'|'", "'?'", "':'", "'='", "';'", "'{'",
  "'}'", "$accept", "primary_expression", "postfix_expression",
  "argument_expression_list", "unary_expression", "unary_operator",
  "cast_expression", "multiplicative_expression", "additive_expression",
  "shift_expression", "relational_expression", "equality_expression",
  "and_expression", "exclusive_or_expression", "inclusive_or_expression",
  "logical_and_expression", "logical_or_expression",
  "conditional_expression", "assignment_expression", "assignment_operator",
  "expression", "constant_expression", "declaration",
  "declaration_specifiers", "init_declarator_list", "init_declarator",
  "storage_class_specifier", "type_specifier", "struct_or_union_specifier",
  "struct_or_union", "struct_declaration_list", "struct_declaration",
  "specifier_qualifier_list", "struct_declarator_list",
  "struct_declarator", "enum_specifier", "enumerator_list", "enumerator",
  "type_qualifier", "declarator", "direct_declarator", "pointer",
  "type_qualifier_list", "parameter_type_list", "parameter_list",
  "parameter_declaration", "identifier_list", "type_name",
  "abstract_declarator", "direct_abstract_declarator", "initializer",
  "initializer_list", "statement", "labeled_statement",
  "compound_statement", "declaration_list", "statement_list",
  "expression_statement", "selection_statement", "iteration_statement",
  "jump_statement", "translation_unit", "external_declaration",
  "function_definition", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-223)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    1047,  -223,  -223,  -223,  -223,  -223,  -223,  -223,  -223,  -223,
    -223,  -223,  -223,  -223,  -223,  -223,  -223,  -223,  -223,  -223,
    -223,     5,    42,   146,  -223,    31,  1193,  1193,  -223,    11,
    -223,  1193,   736,   -33,    26,   957,  -223,  -223,   -60,    58,
      35,  -223,  -223,   146,  -223,    19,  -223,   717,  -223,  -223,
      22,   255,  -223,   278,  -223,    31,  -223,   736,   408,   649,
     -33,  -223,  -223,    58,    28,   -41,  -223,  -223,  -223,  -223,
      42,  -223,   541,  -223,   736,   255,   255,   643,  -223,    72,
     255,   102,  -223,  -223,   880,   901,   901,   913,   131,    82,
     167,   172,   524,   182,   241,   163,   165,   558,   786,  -223,
    -223,  -223,  -223,  -223,  -223,  -223,  -223,  -223,   138,   345,
     913,  -223,    -1,    50,   212,     2,   135,   181,   180,   186,
     242,     4,  -223,  -223,    46,  -223,  -223,  -223,   348,   418,
    -223,  -223,  -223,  -223,   177,  -223,  -223,  -223,  -223,    18,
     198,   200,  -223,    49,  -223,  -223,  -223,  -223,   203,   -19,
     913,    58,  -223,  -223,   541,  -223,  -223,  -223,   661,  -223,
    -223,  -223,   913,    76,  -223,   190,  -223,   524,   786,  -223,
     913,  -223,  -223,   191,   524,   913,   913,   913,   219,   594,
     192,  -223,  -223,  -223,   114,    63,   123,   214,   274,  -223,
    -223,   798,   913,   275,  -223,  -223,  -223,  -223,  -223,  -223,
    -223,  -223,  -223,  -223,  -223,   913,  -223,   913,   913,   913,
     913,   913,   913,   913,   913,   913,   913,   913,   913,   913,
     913,   913,   913,   913,   913,   913,   913,  -223,  -223,   454,
    -223,  -223,  1002,   665,  -223,    43,  -223,   132,  -223,  1173,
    -223,   283,  -223,  -223,  -223,  -223,  -223,   -15,  -223,  -223,
      72,  -223,   913,  -223,   243,   524,  -223,    87,   128,   147,
     245,   594,  -223,  -223,  -223,  1089,   139,  -223,   913,  -223,
    -223,   150,  -223,   144,  -223,  -223,  -223,  -223,  -223,    -1,
      -1,    50,    50,   212,   212,   212,   212,     2,     2,   135,
     181,   180,   186,   242,   -12,  -223,  -223,  -223,   264,   268,
    -223,   240,   132,  1131,   831,  -223,  -223,  -223,   488,  -223,
    -223,  -223,  -223,  -223,   524,   524,   524,   913,   868,  -223,
    -223,   913,  -223,   913,  -223,  -223,  -223,  -223,   279,  -223,
     276,  -223,  -223,   290,  -223,  -223,   156,   524,   159,  -223,
    -223,  -223,  -223,   524,   261,  -223,   524,  -223,  -223,  -223
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,   133,   104,    88,    89,    90,    91,    92,    94,    95,
      96,    97,   100,   101,    98,    99,   129,   130,    93,   108,
     109,     0,     0,   140,   208,     0,    78,    80,   102,     0,
     103,    82,     0,   132,     0,     0,   205,   207,   124,     0,
       0,   144,   142,   141,    76,     0,    84,    86,    79,    81,
     107,     0,    83,     0,   187,     0,   212,     0,     0,     0,
     131,     1,   206,     0,   127,     0,   125,   134,   145,   143,
       0,    77,     0,   210,     0,     0,   114,     0,   110,     0,
     116,     2,     3,     4,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    22,
      23,    24,    25,    26,    27,   191,   183,     6,    16,    28,
       0,    30,    34,    37,    40,    45,    48,    50,    52,    54,
      56,    58,    60,    73,     0,   189,   174,   175,     0,     0,
     176,   177,   178,   179,    86,   188,   211,   153,   139,   152,
       0,   146,   148,     0,     2,   136,    28,    75,     0,     0,
       0,     0,   122,    85,     0,   169,    87,   209,     0,   113,
     106,   111,     0,     0,   117,   119,   115,     0,     0,    20,
       0,    17,    18,     0,     0,     0,     0,     0,     0,     0,
       0,   201,   202,   203,     0,     0,   155,     0,     0,    12,
      13,     0,     0,     0,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    62,     0,    19,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   192,   185,     0,
     184,   190,     0,     0,   150,   157,   151,   158,   137,     0,
     138,     0,   135,   123,   128,   126,   172,     0,   105,   120,
       0,   112,     0,   180,     0,     0,   182,     0,     0,     0,
       0,     0,   200,   204,     5,     0,   157,   156,     0,    11,
       8,     0,    14,     0,    10,    61,    31,    32,    33,    35,
      36,    38,    39,    43,    44,    41,    42,    46,    47,    49,
      51,    53,    55,    57,     0,    74,   186,   165,     0,     0,
     161,     0,   159,     0,     0,   147,   149,   154,     0,   170,
     118,   121,    21,   181,     0,     0,     0,     0,     0,    29,
       9,     0,     7,     0,   166,   160,   162,   167,     0,   163,
       0,   171,   173,   193,   195,   196,     0,     0,     0,    15,
      59,   168,   164,     0,     0,   198,     0,   194,   197,   199
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -223,  -223,  -223,  -223,   -48,  -223,   -91,    16,    23,    38,
      12,   124,   142,   134,   136,   151,  -223,   -55,   -70,  -223,
     -38,   -54,     6,     0,  -223,   304,  -223,   -27,  -223,  -223,
     301,   -67,   -24,  -223,   162,  -223,   333,   249,    47,   -13,
     -29,    -3,  -223,   -57,  -223,   171,  -223,   256,  -122,  -222,
    -151,  -223,   -74,  -223,   160,    29,   285,  -172,  -223,  -223,
    -223,  -223,   394,  -223
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,   107,   108,   271,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   205,
     124,   148,    54,    55,    45,    46,    26,    27,    28,    29,
      77,    78,    79,   163,   164,    30,    65,    66,    31,    32,
      33,    34,    43,   298,   141,   142,   143,   187,   299,   237,
     156,   247,   125,   126,   127,    57,   129,   130,   131,   132,
     133,    35,    36,    37
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      25,   140,   155,   246,   147,    60,    24,   261,    38,    40,
     161,   146,    47,   302,    50,   214,   215,   236,   178,   206,
      42,     1,   224,    63,    76,   151,    48,    49,    58,     1,
      59,    52,   147,   173,     1,    25,   169,   171,   172,   146,
      69,    24,   134,   152,   302,     1,     1,   151,    76,    76,
      76,   308,   159,    76,   226,   231,   166,   134,   139,   184,
     185,    64,   146,   135,   267,   243,   165,   207,   323,   309,
      41,    76,   208,   209,   186,     1,    74,   216,   217,   232,
     135,   233,   128,   225,   155,    70,    23,    22,    39,   318,
      68,   161,    22,   253,    51,   147,   244,    67,    80,    23,
     256,    71,   146,    22,   232,    75,   233,   147,   249,   150,
      23,   240,   226,    44,   146,   241,   276,   277,   278,   210,
     211,   272,    80,    80,    80,   264,   234,    80,   227,   226,
     185,    76,   185,    22,   135,   275,   235,   257,   258,   259,
      23,    76,   250,   175,   186,    80,   188,   189,   190,   314,
     218,   219,   162,   226,   273,   231,   295,   332,   251,   146,
     146,   146,   146,   146,   146,   146,   146,   146,   146,   146,
     146,   146,   146,   146,   146,   146,   146,   319,   147,   301,
     226,   313,   167,   266,   265,   146,   233,   294,    16,    17,
     315,    23,    56,   303,   226,   304,   263,   147,   311,   191,
     265,   192,   233,   193,   146,    80,    60,    73,   322,   316,
     226,   174,   320,   226,    23,    80,   321,   136,   344,    40,
     146,   346,   226,   212,   213,   226,   279,   280,   176,   235,
     287,   288,   139,   177,   157,   281,   282,   165,   155,   139,
     333,   334,   335,   179,   180,   181,   328,   182,   220,   147,
     330,   339,   283,   284,   285,   286,   146,   221,    72,   223,
     238,     2,   266,   345,   222,   139,   239,   242,   340,   347,
     252,   255,   349,   260,   262,   146,   268,   269,   274,   336,
     338,    81,    82,    83,     2,    84,   307,    85,    86,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,   139,   326,   312,   317,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,   324,    87,    88,    89,
     325,    90,    91,    92,    93,    94,    95,    96,    97,    98,
     342,   341,   343,   348,   289,    99,   100,   101,   102,   103,
     104,    81,    82,    83,     2,    84,   291,    85,    86,   292,
     105,    53,   106,   290,   194,   195,   196,   197,   198,   199,
     200,   201,   202,   203,   153,   293,   158,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,   149,    87,    88,    89,
     245,    90,    91,    92,    93,    94,    95,    96,    97,    98,
     306,   137,   310,   229,     2,    99,   100,   101,   102,   103,
     104,    81,    82,    83,   254,    84,   204,    85,    86,    62,
     105,    53,   228,     0,     0,     0,     0,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,     0,    81,    82,    83,
       0,    84,     0,    85,    86,     0,     0,    87,    88,    89,
     138,    90,    91,    92,    93,    94,    95,    96,    97,    98,
       0,     0,     0,     0,     0,    99,   100,   101,   102,   103,
     104,   144,    82,    83,     0,    84,     0,    85,    86,     0,
     105,    53,   230,    87,    88,    89,     0,    90,    91,    92,
      93,    94,    95,    96,    97,    98,     0,     0,     0,     0,
       0,    99,   100,   101,   102,   103,   104,    81,    82,    83,
       0,    84,     0,    85,    86,     0,   105,    53,   296,     0,
       0,     0,     0,     0,   144,    82,    83,     0,    84,    98,
      85,    86,     0,     0,     0,    99,   100,   101,   102,   103,
     104,   144,    82,    83,     0,    84,     0,    85,    86,     0,
       0,   154,   331,    87,    88,    89,     0,    90,    91,    92,
      93,    94,    95,    96,    97,    98,     0,     0,     0,     0,
       0,    99,   100,   101,   102,   103,   104,   144,    82,    83,
       0,    84,    98,    85,    86,     0,   105,    53,    99,   100,
     101,   102,   103,   104,     0,     0,     0,     0,     0,    98,
       0,     0,     0,     0,   154,    99,   100,   101,   102,   103,
     104,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     183,     0,     0,     0,     0,     0,     0,     0,     0,     2,
       0,     0,   144,    82,    83,    98,    84,     0,    85,    86,
       0,    99,   100,   101,   102,   103,   104,     2,   144,    82,
      83,     0,    84,     0,    85,    86,   105,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,     0,     0,     0,     0,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,     0,
      98,     0,     0,   145,     0,     0,    99,   100,   101,   102,
     103,   104,     0,     2,     0,     0,    98,   160,     0,   300,
       0,     0,    99,   100,   101,   102,   103,   104,     0,     0,
       0,     0,     2,     0,     0,   248,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,     0,     0,     0,     0,     0,   144,
      82,    83,     2,    84,     0,    85,    86,     0,    72,     0,
      53,   144,    82,    83,     0,    84,     0,    85,    86,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    53,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,   144,    82,    83,     0,    84,     0,
      85,    86,     0,     0,     0,     0,     0,    98,     0,     0,
       0,     0,     0,    99,   100,   101,   102,   103,   104,    98,
     270,     0,     0,     0,     0,    99,   100,   101,   102,   103,
     104,   144,    82,    83,     0,    84,     0,    85,    86,     0,
       0,     0,     0,   144,    82,    83,     0,    84,     0,    85,
      86,     0,    98,     0,     0,   329,     0,     0,    99,   100,
     101,   102,   103,   104,   144,    82,    83,     0,    84,     0,
      85,    86,     0,     0,     0,     0,   144,    82,    83,     0,
      84,     0,    85,    86,     0,     0,     0,     0,     0,    98,
     337,     0,     0,     0,     0,    99,   100,   101,   102,   103,
     104,   168,     0,     0,     0,     0,     0,    99,   100,   101,
     102,   103,   104,     0,     0,     0,     0,    61,     0,     0,
       1,     0,   170,     2,     0,     0,     0,     0,    99,   100,
     101,   102,   103,   104,    98,     0,     0,     0,     0,     0,
      99,   100,   101,   102,   103,   104,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,     1,     0,     0,     2,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    22,     0,
       0,     0,     0,     0,     0,    23,     0,     0,     0,     0,
       0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
       1,     0,     0,     2,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   232,   297,   233,     0,     0,     0,     0,
      23,     0,     0,     0,     0,     0,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,     2,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    22,     0,
       0,     0,     0,     0,     0,    23,     0,     0,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,     2,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     265,   297,   233,     0,     0,     0,     0,    23,     0,     0,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,     2,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   327,     0,     0,     0,     0,     0,     2,
       0,     0,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,   305,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21
};

static const yytype_int16 yycheck[] =
{
       0,    58,    72,   154,    59,    34,     0,   179,     3,    22,
      77,    59,    25,   235,     3,    13,    14,   139,    92,   110,
      23,     3,    18,    83,    51,    66,    26,    27,    61,     3,
      63,    31,    87,    87,     3,    35,    84,    85,    86,    87,
      43,    35,    55,    84,   266,     3,     3,    66,    75,    76,
      77,    66,    76,    80,    66,   129,    80,    70,    58,    97,
      98,     3,   110,    57,   186,    84,    79,    68,    80,    84,
      23,    98,    73,    74,    98,     3,    47,    75,    76,    61,
      74,    63,    53,    79,   154,    66,    68,    61,    83,   261,
      43,   158,    61,   167,    83,   150,   150,    62,    51,    68,
     174,    82,   150,    61,    61,    83,    63,   162,   162,    81,
      68,    62,    66,    82,   162,    66,   207,   208,   209,    69,
      70,   191,    75,    76,    77,    62,   139,    80,    82,    66,
     168,   158,   170,    61,   128,   205,   139,   175,   176,   177,
      68,   168,    66,    61,   168,    98,     8,     9,    10,    62,
      15,    16,    80,    66,   192,   229,   226,   308,    82,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   222,   223,   224,   268,   233,   233,
      66,   255,    80,   186,    61,   233,    63,   225,    42,    43,
      62,    68,    32,    61,    66,    63,    82,   252,   252,    61,
      61,    63,    63,    65,   252,   158,   235,    47,    64,    62,
      66,    80,    62,    66,    68,   168,    66,    57,    62,   232,
     268,    62,    66,    11,    12,    66,   210,   211,    61,   232,
     218,   219,   232,    61,    74,   212,   213,   250,   308,   239,
     314,   315,   316,    61,     3,    82,   303,    82,    67,   304,
     304,   321,   214,   215,   216,   217,   304,    77,    81,    17,
      62,     6,   265,   337,    78,   265,    66,    64,   323,   343,
      80,    80,   346,    54,    82,   323,    62,     3,     3,   317,
     318,     3,     4,     5,     6,     7,     3,     9,    10,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,   303,    64,    62,    61,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    62,    49,    50,    51,
      62,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      64,    62,    52,    82,   220,    67,    68,    69,    70,    71,
      72,     3,     4,     5,     6,     7,   222,     9,    10,   223,
      82,    83,    84,   221,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    70,   224,    75,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    63,    49,    50,    51,
     151,    53,    54,    55,    56,    57,    58,    59,    60,    61,
     239,     3,   250,   128,     6,    67,    68,    69,    70,    71,
      72,     3,     4,     5,   168,     7,    81,     9,    10,    35,
      82,    83,    84,    -1,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    -1,     3,     4,     5,
      -1,     7,    -1,     9,    10,    -1,    -1,    49,    50,    51,
      62,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      -1,    -1,    -1,    -1,    -1,    67,    68,    69,    70,    71,
      72,     3,     4,     5,    -1,     7,    -1,     9,    10,    -1,
      82,    83,    84,    49,    50,    51,    -1,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    -1,    -1,    -1,    -1,
      -1,    67,    68,    69,    70,    71,    72,     3,     4,     5,
      -1,     7,    -1,     9,    10,    -1,    82,    83,    84,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,    61,
       9,    10,    -1,    -1,    -1,    67,    68,    69,    70,    71,
      72,     3,     4,     5,    -1,     7,    -1,     9,    10,    -1,
      -1,    83,    84,    49,    50,    51,    -1,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    -1,    -1,    -1,    -1,
      -1,    67,    68,    69,    70,    71,    72,     3,     4,     5,
      -1,     7,    61,     9,    10,    -1,    82,    83,    67,    68,
      69,    70,    71,    72,    -1,    -1,    -1,    -1,    -1,    61,
      -1,    -1,    -1,    -1,    83,    67,    68,    69,    70,    71,
      72,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      82,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     6,
      -1,    -1,     3,     4,     5,    61,     7,    -1,     9,    10,
      -1,    67,    68,    69,    70,    71,    72,     6,     3,     4,
       5,    -1,     7,    -1,     9,    10,    82,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    -1,    -1,    -1,    -1,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    -1,
      61,    -1,    -1,    64,    -1,    -1,    67,    68,    69,    70,
      71,    72,    -1,     6,    -1,    -1,    61,    84,    -1,    64,
      -1,    -1,    67,    68,    69,    70,    71,    72,    -1,    -1,
      -1,    -1,     6,    -1,    -1,    84,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,     6,     7,    -1,     9,    10,    -1,    81,    -1,
      83,     3,     4,     5,    -1,     7,    -1,     9,    10,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,     3,     4,     5,    -1,     7,    -1,
       9,    10,    -1,    -1,    -1,    -1,    -1,    61,    -1,    -1,
      -1,    -1,    -1,    67,    68,    69,    70,    71,    72,    61,
      62,    -1,    -1,    -1,    -1,    67,    68,    69,    70,    71,
      72,     3,     4,     5,    -1,     7,    -1,     9,    10,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,     9,
      10,    -1,    61,    -1,    -1,    64,    -1,    -1,    67,    68,
      69,    70,    71,    72,     3,     4,     5,    -1,     7,    -1,
       9,    10,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    61,
      62,    -1,    -1,    -1,    -1,    67,    68,    69,    70,    71,
      72,    61,    -1,    -1,    -1,    -1,    -1,    67,    68,    69,
      70,    71,    72,    -1,    -1,    -1,    -1,     0,    -1,    -1,
       3,    -1,    61,     6,    -1,    -1,    -1,    -1,    67,    68,
      69,    70,    71,    72,    61,    -1,    -1,    -1,    -1,    -1,
      67,    68,    69,    70,    71,    72,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,     3,    -1,    -1,     6,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,
      -1,    -1,    -1,    -1,    -1,    68,    -1,    -1,    -1,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
       3,    -1,    -1,     6,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    61,    62,    63,    -1,    -1,    -1,    -1,
      68,    -1,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,     6,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,
      -1,    -1,    -1,    -1,    -1,    68,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,     6,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      61,    62,    63,    -1,    -1,    -1,    -1,    68,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,     6,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    62,    -1,    -1,    -1,    -1,    -1,     6,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     6,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    61,    68,   107,   108,   111,   112,   113,   114,
     120,   123,   124,   125,   126,   146,   147,   148,     3,    83,
     124,   123,   126,   127,    82,   109,   110,   124,   108,   108,
       3,    83,   108,    83,   107,   108,   139,   140,    61,    63,
     125,     0,   147,    83,     3,   121,   122,    62,   123,   126,
      66,    82,    81,   139,   140,    83,   112,   115,   116,   117,
     123,     3,     4,     5,     7,     9,    10,    49,    50,    51,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    67,
      68,    69,    70,    71,    72,    82,    84,    86,    87,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   105,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   124,   107,   139,     3,    62,   108,
     128,   129,   130,   131,     3,    64,    89,   102,   106,   121,
      81,    66,    84,   110,    83,   103,   135,   139,   115,   117,
      84,   116,    80,   118,   119,   124,   117,    80,    61,    89,
      61,    89,    89,   106,    80,    61,    61,    61,   137,    61,
       3,    82,    82,    82,   105,   105,   117,   132,     8,     9,
      10,    61,    63,    65,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    81,   104,    91,    68,    73,    74,
      69,    70,    11,    12,    13,    14,    75,    76,    15,    16,
      67,    77,    78,    17,    18,    79,    66,    82,    84,   141,
      84,   137,    61,    63,   124,   126,   133,   134,    62,    66,
      62,    66,    64,    84,   106,   122,   135,   136,    84,   106,
      66,    82,    80,   137,   132,    80,   137,   105,   105,   105,
      54,   142,    82,    82,    62,    61,   126,   133,    62,     3,
      62,    88,   103,   105,     3,   103,    91,    91,    91,    92,
      92,    93,    93,    94,    94,    94,    94,    95,    95,    96,
      97,    98,    99,   100,   105,   103,    84,    62,   128,   133,
      64,   106,   134,    61,    63,    48,   130,     3,    66,    84,
     119,   106,    62,   137,    62,    62,    62,    61,   142,    91,
      62,    66,    64,    80,    62,    62,    64,    62,   128,    64,
     106,    84,   135,   137,   137,   137,   105,    62,   105,   103,
     102,    62,    64,    52,    62,   137,    62,   137,    82,   137
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    85,    86,    86,    86,    86,    87,    87,    87,    87,
      87,    87,    87,    87,    88,    88,    89,    89,    89,    89,
      89,    89,    90,    90,    90,    90,    90,    90,    91,    91,
      92,    92,    92,    92,    93,    93,    93,    94,    94,    94,
      95,    95,    95,    95,    95,    96,    96,    96,    97,    97,
      98,    98,    99,    99,   100,   100,   101,   101,   102,   102,
     103,   103,   104,   104,   104,   104,   104,   104,   104,   104,
     104,   104,   104,   105,   105,   106,   107,   107,   108,   108,
     108,   108,   108,   108,   109,   109,   110,   110,   111,   111,
     111,   111,   111,   112,   112,   112,   112,   112,   112,   112,
     112,   112,   112,   112,   112,   113,   113,   113,   114,   114,
     115,   115,   116,   117,   117,   117,   117,   118,   118,   119,
     119,   119,   120,   120,   120,   121,   121,   122,   122,   123,
     123,   124,   124,   125,   125,   125,   125,   125,   125,   125,
     126,   126,   126,   126,   127,   127,   128,   128,   129,   129,
     130,   130,   130,   131,   131,   132,   132,   133,   133,   133,
     134,   134,   134,   134,   134,   134,   134,   134,   134,   135,
     135,   135,   136,   136,   137,   137,   137,   137,   137,   137,
     138,   138,   138,   139,   139,   139,   139,   140,   140,   141,
     141,   142,   142,   143,   143,   143,   144,   144,   144,   144,
     145,   145,   145,   145,   145,   146,   146,   147,   147,   148,
     148,   148,   148
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     1,     3,     1,     4,     3,     4,
       3,     3,     2,     2,     1,     3,     1,     2,     2,     2,
       2,     4,     1,     1,     1,     1,     1,     1,     1,     4,
       1,     3,     3,     3,     1,     3,     3,     1,     3,     3,
       1,     3,     3,     3,     3,     1,     3,     3,     1,     3,
       1,     3,     1,     3,     1,     3,     1,     3,     1,     5,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     1,     2,     3,     1,     2,
       1,     2,     1,     2,     1,     3,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     5,     4,     2,     1,     1,
       1,     2,     3,     2,     1,     2,     1,     1,     3,     1,
       2,     3,     4,     5,     2,     1,     3,     1,     3,     1,
       1,     2,     1,     1,     3,     4,     3,     4,     4,     3,
       1,     2,     2,     3,     1,     2,     1,     3,     1,     3,
       2,     2,     1,     1,     3,     1,     2,     1,     1,     2,
       3,     2,     3,     3,     4,     2,     3,     3,     4,     1,
       3,     4,     1,     3,     1,     1,     1,     1,     1,     1,
       3,     4,     3,     2,     3,     3,     4,     1,     2,     1,
       2,     1,     2,     5,     7,     5,     5,     7,     6,     7,
       3,     2,     2,     2,     3,     1,     2,     1,     1,     4,
       3,     3,     2
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
  case 2: /* primary_expression: IDENTIFIER  */
#line 51 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                  { (yyval.node) = ast_new_expr_ident((yyvsp[0].str), token_line, token_column); free((yyvsp[0].str)); }
#line 1672 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 3: /* primary_expression: CONSTANT  */
#line 52 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                  { (yyval.node) = ast_new_expr_const((yyvsp[0].str), AST_C_INT, token_line, token_column); free((yyvsp[0].str)); }
#line 1678 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 4: /* primary_expression: STRING_LITERAL  */
#line 53 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                  { (yyval.node) = ast_new_expr_string((yyvsp[0].str), token_line, token_column); free((yyvsp[0].str)); }
#line 1684 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 5: /* primary_expression: '(' expression ')'  */
#line 54 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                  { (yyval.node) = (yyvsp[-1].node); }
#line 1690 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 6: /* postfix_expression: primary_expression  */
#line 58 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                         { (yyval.node) = (yyvsp[0].node); }
#line 1696 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 7: /* postfix_expression: postfix_expression '[' expression ']'  */
#line 59 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                         { (yyval.node) = ast_new_expr_index((yyvsp[-3].node), (yyvsp[-1].node), line, column); }
#line 1702 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 8: /* postfix_expression: postfix_expression '(' ')'  */
#line 60 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                         { (yyval.node) = ast_new_expr_call((yyvsp[-2].node), 0, line, column); }
#line 1708 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 9: /* postfix_expression: postfix_expression '(' argument_expression_list ')'  */
#line 61 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                              { (yyval.node) = ast_new_expr_call((yyvsp[-3].node), (yyvsp[-1].list), line, column); }
#line 1714 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 10: /* postfix_expression: postfix_expression '.' IDENTIFIER  */
#line 62 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                         { (yyval.node) = ast_new_expr_member((yyvsp[-2].node), (yyvsp[0].str), 0, line, column); free((yyvsp[0].str)); }
#line 1720 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 11: /* postfix_expression: postfix_expression PTR_OP IDENTIFIER  */
#line 63 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                         { (yyval.node) = ast_new_expr_member((yyvsp[-2].node), (yyvsp[0].str), 1, line, column); free((yyvsp[0].str)); }
#line 1726 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 12: /* postfix_expression: postfix_expression INC_OP  */
#line 64 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                         { (yyval.node) = ast_new_expr_unary(OP_POST_INC, (yyvsp[-1].node), line, column); }
#line 1732 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 13: /* postfix_expression: postfix_expression DEC_OP  */
#line 65 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                         { (yyval.node) = ast_new_expr_unary(OP_POST_DEC, (yyvsp[-1].node), line, column); }
#line 1738 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 14: /* argument_expression_list: assignment_expression  */
#line 69 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                         { (yyval.list) = ast_list_append(0, (yyvsp[0].node)); }
#line 1744 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 15: /* argument_expression_list: argument_expression_list ',' assignment_expression  */
#line 70 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                             { (yyval.list) = ast_list_append((yyvsp[-2].list), (yyvsp[0].node)); }
#line 1750 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 16: /* unary_expression: postfix_expression  */
#line 74 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                         { (yyval.node) = (yyvsp[0].node); }
#line 1756 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 17: /* unary_expression: INC_OP unary_expression  */
#line 75 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                         { (yyval.node) = ast_new_expr_unary(OP_INC, (yyvsp[0].node), line, column); }
#line 1762 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 18: /* unary_expression: DEC_OP unary_expression  */
#line 76 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                         { (yyval.node) = ast_new_expr_unary(OP_DEC, (yyvsp[0].node), line, column); }
#line 1768 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 19: /* unary_expression: unary_operator cast_expression  */
#line 77 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                         { (yyval.node) = ast_new_expr_unary((yyvsp[-1].ival), (yyvsp[0].node), line, column); }
#line 1774 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 20: /* unary_expression: SIZEOF unary_expression  */
#line 78 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                         { (yyval.node) = ast_new_expr_unary(OP_SIZEOF, (yyvsp[0].node), line, column); }
#line 1780 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 21: /* unary_expression: SIZEOF '(' type_name ')'  */
#line 79 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                         { (yyval.node) = ast_new_expr_unary(OP_SIZEOF, 0, line, column); }
#line 1786 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 22: /* unary_operator: '&'  */
#line 83 "/home/atirut/Projects/zdk/cc/src/parser.y"
                 { (yyval.ival) = '&'; }
#line 1792 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 23: /* unary_operator: '*'  */
#line 84 "/home/atirut/Projects/zdk/cc/src/parser.y"
                 { (yyval.ival) = '*'; }
#line 1798 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 24: /* unary_operator: '+'  */
#line 85 "/home/atirut/Projects/zdk/cc/src/parser.y"
                 { (yyval.ival) = '+'; }
#line 1804 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 25: /* unary_operator: '-'  */
#line 86 "/home/atirut/Projects/zdk/cc/src/parser.y"
                 { (yyval.ival) = '-'; }
#line 1810 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 26: /* unary_operator: '~'  */
#line 87 "/home/atirut/Projects/zdk/cc/src/parser.y"
                 { (yyval.ival) = '~'; }
#line 1816 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 27: /* unary_operator: '!'  */
#line 88 "/home/atirut/Projects/zdk/cc/src/parser.y"
                 { (yyval.ival) = '!'; }
#line 1822 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 28: /* cast_expression: unary_expression  */
#line 92 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = (yyvsp[0].node); }
#line 1828 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 29: /* cast_expression: '(' type_name ')' cast_expression  */
#line 93 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = ast_new_expr_cast((yyvsp[-2].ival), 0, (yyvsp[0].node), line, column); }
#line 1834 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 30: /* multiplicative_expression: cast_expression  */
#line 97 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = (yyvsp[0].node); }
#line 1840 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 31: /* multiplicative_expression: multiplicative_expression '*' cast_expression  */
#line 98 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = ast_new_expr_binary('*', (yyvsp[-2].node), (yyvsp[0].node), line, column); }
#line 1846 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 32: /* multiplicative_expression: multiplicative_expression '/' cast_expression  */
#line 99 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = ast_new_expr_binary('/', (yyvsp[-2].node), (yyvsp[0].node), line, column); }
#line 1852 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 33: /* multiplicative_expression: multiplicative_expression '%' cast_expression  */
#line 100 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = ast_new_expr_binary('%', (yyvsp[-2].node), (yyvsp[0].node), line, column); }
#line 1858 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 34: /* additive_expression: multiplicative_expression  */
#line 104 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = (yyvsp[0].node); }
#line 1864 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 35: /* additive_expression: additive_expression '+' multiplicative_expression  */
#line 105 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                            { (yyval.node) = ast_new_expr_binary('+', (yyvsp[-2].node), (yyvsp[0].node), line, column); }
#line 1870 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 36: /* additive_expression: additive_expression '-' multiplicative_expression  */
#line 106 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                            { (yyval.node) = ast_new_expr_binary('-', (yyvsp[-2].node), (yyvsp[0].node), line, column); }
#line 1876 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 37: /* shift_expression: additive_expression  */
#line 110 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = (yyvsp[0].node); }
#line 1882 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 38: /* shift_expression: shift_expression LEFT_OP additive_expression  */
#line 111 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = ast_new_expr_binary(OP_SHL, (yyvsp[-2].node), (yyvsp[0].node), line, column); }
#line 1888 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 39: /* shift_expression: shift_expression RIGHT_OP additive_expression  */
#line 112 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = ast_new_expr_binary(OP_SHR, (yyvsp[-2].node), (yyvsp[0].node), line, column); }
#line 1894 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 40: /* relational_expression: shift_expression  */
#line 116 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = (yyvsp[0].node); }
#line 1900 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 41: /* relational_expression: relational_expression '<' shift_expression  */
#line 117 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = ast_new_expr_binary('<', (yyvsp[-2].node), (yyvsp[0].node), line, column); }
#line 1906 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 42: /* relational_expression: relational_expression '>' shift_expression  */
#line 118 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = ast_new_expr_binary('>', (yyvsp[-2].node), (yyvsp[0].node), line, column); }
#line 1912 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 43: /* relational_expression: relational_expression LE_OP shift_expression  */
#line 119 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = ast_new_expr_binary(OP_LE, (yyvsp[-2].node), (yyvsp[0].node), line, column); }
#line 1918 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 44: /* relational_expression: relational_expression GE_OP shift_expression  */
#line 120 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = ast_new_expr_binary(OP_GE, (yyvsp[-2].node), (yyvsp[0].node), line, column); }
#line 1924 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 45: /* equality_expression: relational_expression  */
#line 124 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = (yyvsp[0].node); }
#line 1930 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 46: /* equality_expression: equality_expression EQ_OP relational_expression  */
#line 125 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = ast_new_expr_binary(OP_EQ, (yyvsp[-2].node), (yyvsp[0].node), line, column); }
#line 1936 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 47: /* equality_expression: equality_expression NE_OP relational_expression  */
#line 126 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = ast_new_expr_binary(OP_NE, (yyvsp[-2].node), (yyvsp[0].node), line, column); }
#line 1942 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 48: /* and_expression: equality_expression  */
#line 130 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = (yyvsp[0].node); }
#line 1948 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 49: /* and_expression: and_expression '&' equality_expression  */
#line 131 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = ast_new_expr_binary('&', (yyvsp[-2].node), (yyvsp[0].node), line, column); }
#line 1954 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 50: /* exclusive_or_expression: and_expression  */
#line 135 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = (yyvsp[0].node); }
#line 1960 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 51: /* exclusive_or_expression: exclusive_or_expression '^' and_expression  */
#line 136 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = ast_new_expr_binary('^', (yyvsp[-2].node), (yyvsp[0].node), line, column); }
#line 1966 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 52: /* inclusive_or_expression: exclusive_or_expression  */
#line 140 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = (yyvsp[0].node); }
#line 1972 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 53: /* inclusive_or_expression: inclusive_or_expression '|' exclusive_or_expression  */
#line 141 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                              { (yyval.node) = ast_new_expr_binary('|', (yyvsp[-2].node), (yyvsp[0].node), line, column); }
#line 1978 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 54: /* logical_and_expression: inclusive_or_expression  */
#line 145 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = (yyvsp[0].node); }
#line 1984 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 55: /* logical_and_expression: logical_and_expression AND_OP inclusive_or_expression  */
#line 146 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                                { (yyval.node) = ast_new_expr_binary(OP_LAND, (yyvsp[-2].node), (yyvsp[0].node), line, column); }
#line 1990 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 56: /* logical_or_expression: logical_and_expression  */
#line 150 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = (yyvsp[0].node); }
#line 1996 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 57: /* logical_or_expression: logical_or_expression OR_OP logical_and_expression  */
#line 151 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                             { (yyval.node) = ast_new_expr_binary(OP_LOR, (yyvsp[-2].node), (yyvsp[0].node), line, column); }
#line 2002 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 58: /* conditional_expression: logical_or_expression  */
#line 155 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = (yyvsp[0].node); }
#line 2008 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 59: /* conditional_expression: logical_or_expression '?' expression ':' conditional_expression  */
#line 156 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                                          { (yyval.node) = ast_new_expr_cond((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node), line, column); }
#line 2014 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 60: /* assignment_expression: conditional_expression  */
#line 160 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = (yyvsp[0].node); }
#line 2020 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 61: /* assignment_expression: unary_expression assignment_operator assignment_expression  */
#line 161 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                                     { (yyval.node) = ast_new_expr_binary((yyvsp[-1].ival), (yyvsp[-2].node), (yyvsp[0].node), line, column); }
#line 2026 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 62: /* assignment_operator: '='  */
#line 165 "/home/atirut/Projects/zdk/cc/src/parser.y"
                       { (yyval.ival) = '='; }
#line 2032 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 63: /* assignment_operator: MUL_ASSIGN  */
#line 166 "/home/atirut/Projects/zdk/cc/src/parser.y"
                       { (yyval.ival) = OP_MUL_ASSIGN; }
#line 2038 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 64: /* assignment_operator: DIV_ASSIGN  */
#line 167 "/home/atirut/Projects/zdk/cc/src/parser.y"
                       { (yyval.ival) = OP_DIV_ASSIGN; }
#line 2044 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 65: /* assignment_operator: MOD_ASSIGN  */
#line 168 "/home/atirut/Projects/zdk/cc/src/parser.y"
                       { (yyval.ival) = OP_MOD_ASSIGN; }
#line 2050 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 66: /* assignment_operator: ADD_ASSIGN  */
#line 169 "/home/atirut/Projects/zdk/cc/src/parser.y"
                       { (yyval.ival) = OP_ADD_ASSIGN; }
#line 2056 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 67: /* assignment_operator: SUB_ASSIGN  */
#line 170 "/home/atirut/Projects/zdk/cc/src/parser.y"
                       { (yyval.ival) = OP_SUB_ASSIGN; }
#line 2062 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 68: /* assignment_operator: LEFT_ASSIGN  */
#line 171 "/home/atirut/Projects/zdk/cc/src/parser.y"
                       { (yyval.ival) = OP_SHL_ASSIGN; }
#line 2068 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 69: /* assignment_operator: RIGHT_ASSIGN  */
#line 172 "/home/atirut/Projects/zdk/cc/src/parser.y"
                       { (yyval.ival) = OP_SHR_ASSIGN; }
#line 2074 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 70: /* assignment_operator: AND_ASSIGN  */
#line 173 "/home/atirut/Projects/zdk/cc/src/parser.y"
                       { (yyval.ival) = OP_AND_ASSIGN; }
#line 2080 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 71: /* assignment_operator: XOR_ASSIGN  */
#line 174 "/home/atirut/Projects/zdk/cc/src/parser.y"
                       { (yyval.ival) = OP_XOR_ASSIGN; }
#line 2086 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 72: /* assignment_operator: OR_ASSIGN  */
#line 175 "/home/atirut/Projects/zdk/cc/src/parser.y"
                       { (yyval.ival) = OP_OR_ASSIGN; }
#line 2092 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 73: /* expression: assignment_expression  */
#line 179 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = (yyvsp[0].node); }
#line 2098 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 74: /* expression: expression ',' assignment_expression  */
#line 180 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                         { (yyval.node) = (yyvsp[0].node); }
#line 2104 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 75: /* constant_expression: conditional_expression  */
#line 184 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = (yyvsp[0].node); }
#line 2110 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 76: /* declaration: declaration_specifiers ';'  */
#line 188 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = ast_new_decl((yyvsp[-1].ival), 0, 0, line, column); }
#line 2116 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 77: /* declaration: declaration_specifiers init_declarator_list ';'  */
#line 189 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = (yyvsp[-1].node); if ((yyval.node)) { (yyval.node)->u.decl.spec_flags |= (yyvsp[-2].ival); } }
#line 2122 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 78: /* declaration_specifiers: storage_class_specifier  */
#line 193 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.ival) = (yyvsp[0].ival); }
#line 2128 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 79: /* declaration_specifiers: storage_class_specifier declaration_specifiers  */
#line 194 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.ival) = (yyvsp[-1].ival) | (yyvsp[0].ival); }
#line 2134 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 80: /* declaration_specifiers: type_specifier  */
#line 195 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.ival) = (yyvsp[0].ival); }
#line 2140 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 81: /* declaration_specifiers: type_specifier declaration_specifiers  */
#line 196 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.ival) = (yyvsp[-1].ival) | (yyvsp[0].ival); }
#line 2146 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 82: /* declaration_specifiers: type_qualifier  */
#line 197 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.ival) = (yyvsp[0].ival); }
#line 2152 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 83: /* declaration_specifiers: type_qualifier declaration_specifiers  */
#line 198 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.ival) = (yyvsp[-1].ival) | (yyvsp[0].ival); }
#line 2158 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 84: /* init_declarator_list: init_declarator  */
#line 202 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = (yyvsp[0].node); }
#line 2164 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 85: /* init_declarator_list: init_declarator_list ',' init_declarator  */
#line 203 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = (yyvsp[-2].node); }
#line 2170 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 86: /* init_declarator: declarator  */
#line 207 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = ast_new_decl(0, (yyvsp[0].decltor), 0, line, column); }
#line 2176 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 87: /* init_declarator: declarator '=' initializer  */
#line 208 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.node) = ast_new_decl(0, (yyvsp[-2].decltor), (yyvsp[0].node), line, column); }
#line 2182 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 88: /* storage_class_specifier: TYPEDEF  */
#line 212 "/home/atirut/Projects/zdk/cc/src/parser.y"
                    { (yyval.ival) = SPF_TYPEDEF; }
#line 2188 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 89: /* storage_class_specifier: EXTERN  */
#line 213 "/home/atirut/Projects/zdk/cc/src/parser.y"
                    { (yyval.ival) = SPF_EXTERN; }
#line 2194 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 90: /* storage_class_specifier: STATIC  */
#line 214 "/home/atirut/Projects/zdk/cc/src/parser.y"
                    { (yyval.ival) = SPF_STATIC; }
#line 2200 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 91: /* storage_class_specifier: AUTO  */
#line 215 "/home/atirut/Projects/zdk/cc/src/parser.y"
                    { (yyval.ival) = SPF_AUTO; }
#line 2206 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 92: /* storage_class_specifier: REGISTER  */
#line 216 "/home/atirut/Projects/zdk/cc/src/parser.y"
                    { (yyval.ival) = SPF_REGISTER; }
#line 2212 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 93: /* type_specifier: VOID  */
#line 220 "/home/atirut/Projects/zdk/cc/src/parser.y"
                    { (yyval.ival) = SPF_VOID; }
#line 2218 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 94: /* type_specifier: CHAR  */
#line 221 "/home/atirut/Projects/zdk/cc/src/parser.y"
                    { (yyval.ival) = SPF_CHAR; }
#line 2224 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 95: /* type_specifier: SHORT  */
#line 222 "/home/atirut/Projects/zdk/cc/src/parser.y"
                    { (yyval.ival) = SPF_SHORT; }
#line 2230 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 96: /* type_specifier: INT  */
#line 223 "/home/atirut/Projects/zdk/cc/src/parser.y"
                    { (yyval.ival) = SPF_INT; }
#line 2236 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 97: /* type_specifier: LONG  */
#line 224 "/home/atirut/Projects/zdk/cc/src/parser.y"
                    { (yyval.ival) = SPF_LONG; }
#line 2242 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 98: /* type_specifier: FLOAT  */
#line 225 "/home/atirut/Projects/zdk/cc/src/parser.y"
                    { (yyval.ival) = SPF_FLOAT; }
#line 2248 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 99: /* type_specifier: DOUBLE  */
#line 226 "/home/atirut/Projects/zdk/cc/src/parser.y"
                    { (yyval.ival) = SPF_DOUBLE; }
#line 2254 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 100: /* type_specifier: SIGNED  */
#line 227 "/home/atirut/Projects/zdk/cc/src/parser.y"
                    { (yyval.ival) = SPF_SIGNED; }
#line 2260 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 101: /* type_specifier: UNSIGNED  */
#line 228 "/home/atirut/Projects/zdk/cc/src/parser.y"
                    { (yyval.ival) = SPF_UNSIGNED; }
#line 2266 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 102: /* type_specifier: struct_or_union_specifier  */
#line 229 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                    { (yyval.ival) = 0; }
#line 2272 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 103: /* type_specifier: enum_specifier  */
#line 230 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                    { (yyval.ival) = 0; }
#line 2278 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 104: /* type_specifier: TYPE_NAME  */
#line 231 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                    { (yyval.ival) = 0; }
#line 2284 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 113: /* specifier_qualifier_list: type_specifier specifier_qualifier_list  */
#line 255 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                         { (yyval.ival) = (yyvsp[-1].ival) | (yyvsp[0].ival); }
#line 2290 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 114: /* specifier_qualifier_list: type_specifier  */
#line 256 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                         { (yyval.ival) = (yyvsp[0].ival); }
#line 2296 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 115: /* specifier_qualifier_list: type_qualifier specifier_qualifier_list  */
#line 257 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                         { (yyval.ival) = (yyvsp[-1].ival) | (yyvsp[0].ival); }
#line 2302 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 116: /* specifier_qualifier_list: type_qualifier  */
#line 258 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                         { (yyval.ival) = (yyvsp[0].ival); }
#line 2308 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 129: /* type_qualifier: CONST  */
#line 289 "/home/atirut/Projects/zdk/cc/src/parser.y"
                    { (yyval.ival) = SPF_CONST; }
#line 2314 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 130: /* type_qualifier: VOLATILE  */
#line 290 "/home/atirut/Projects/zdk/cc/src/parser.y"
                    { (yyval.ival) = SPF_VOLATILE; }
#line 2320 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 131: /* declarator: pointer direct_declarator  */
#line 294 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { ast_set_declarator_pointer((yyvsp[0].decltor), (yyvsp[-1].ival)); (yyval.decltor) = (yyvsp[0].decltor); }
#line 2326 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 132: /* declarator: direct_declarator  */
#line 295 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.decltor) = (yyvsp[0].decltor); }
#line 2332 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 133: /* direct_declarator: IDENTIFIER  */
#line 299 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.decltor) = ast_new_declarator((yyvsp[0].str)); free((yyvsp[0].str)); }
#line 2338 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 134: /* direct_declarator: '(' declarator ')'  */
#line 300 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.decltor) = (yyvsp[-1].decltor); }
#line 2344 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 135: /* direct_declarator: direct_declarator '[' constant_expression ']'  */
#line 301 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { ast_set_declarator_array((yyvsp[-3].decltor), (yyvsp[-1].node)); (yyval.decltor) = (yyvsp[-3].decltor); }
#line 2350 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 136: /* direct_declarator: direct_declarator '[' ']'  */
#line 302 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { ast_set_declarator_array((yyvsp[-2].decltor), 0); (yyval.decltor) = (yyvsp[-2].decltor); }
#line 2356 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 137: /* direct_declarator: direct_declarator '(' parameter_type_list ')'  */
#line 303 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { ast_set_declarator_function((yyvsp[-3].decltor), (yyvsp[-1].list)); (yyval.decltor) = (yyvsp[-3].decltor); }
#line 2362 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 138: /* direct_declarator: direct_declarator '(' identifier_list ')'  */
#line 304 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { ast_set_declarator_function((yyvsp[-3].decltor), 0); (yyval.decltor) = (yyvsp[-3].decltor); }
#line 2368 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 139: /* direct_declarator: direct_declarator '(' ')'  */
#line 305 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { ast_set_declarator_function((yyvsp[-2].decltor), 0); (yyval.decltor) = (yyvsp[-2].decltor); }
#line 2374 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 140: /* pointer: '*'  */
#line 309 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                             { (yyval.ival) = 1; }
#line 2380 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 141: /* pointer: '*' type_qualifier_list  */
#line 310 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                             { (yyval.ival) = 1; }
#line 2386 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 142: /* pointer: '*' pointer  */
#line 311 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                             { (yyval.ival) = 1 + (yyvsp[0].ival); }
#line 2392 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 143: /* pointer: '*' type_qualifier_list pointer  */
#line 312 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                             { (yyval.ival) = 1 + (yyvsp[0].ival); }
#line 2398 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 144: /* type_qualifier_list: type_qualifier  */
#line 316 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                             { (yyval.ival) = (yyvsp[0].ival); }
#line 2404 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 145: /* type_qualifier_list: type_qualifier_list type_qualifier  */
#line 317 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                             { (yyval.ival) = (yyvsp[-1].ival) | (yyvsp[0].ival); }
#line 2410 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 146: /* parameter_type_list: parameter_list  */
#line 322 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                             { (yyval.list) = (yyvsp[0].list); }
#line 2416 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 147: /* parameter_type_list: parameter_list ',' ELLIPSIS  */
#line 323 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                             { (yyval.list) = (yyvsp[-2].list); }
#line 2422 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 148: /* parameter_list: parameter_declaration  */
#line 327 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                             { (yyval.list) = ast_list_append(0, (yyvsp[0].node)); }
#line 2428 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 149: /* parameter_list: parameter_list ',' parameter_declaration  */
#line 328 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                   { (yyval.list) = ast_list_append((yyvsp[-2].list), (yyvsp[0].node)); }
#line 2434 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 150: /* parameter_declaration: declaration_specifiers declarator  */
#line 332 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                        { (yyval.node) = ast_new_decl((yyvsp[-1].ival), (yyvsp[0].decltor), 0, line, column); }
#line 2440 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 151: /* parameter_declaration: declaration_specifiers abstract_declarator  */
#line 333 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                        { (yyval.node) = ast_new_decl((yyvsp[-1].ival), 0, 0, line, column); }
#line 2446 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 152: /* parameter_declaration: declaration_specifiers  */
#line 334 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                        { (yyval.node) = ast_new_decl((yyvsp[0].ival), 0, 0, line, column); }
#line 2452 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 153: /* identifier_list: IDENTIFIER  */
#line 338 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                              { free((yyvsp[0].str)); (yyval.node) = 0; }
#line 2458 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 154: /* identifier_list: identifier_list ',' IDENTIFIER  */
#line 339 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                              { free((yyvsp[0].str)); (yyval.node) = 0; }
#line 2464 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 155: /* type_name: specifier_qualifier_list  */
#line 343 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                         { (yyval.ival) = (yyvsp[0].ival); }
#line 2470 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 156: /* type_name: specifier_qualifier_list abstract_declarator  */
#line 344 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                         { (yyval.ival) = (yyvsp[-1].ival); }
#line 2476 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 157: /* abstract_declarator: pointer  */
#line 348 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                         { (yyval.ival) = (yyvsp[0].ival); }
#line 2482 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 158: /* abstract_declarator: direct_abstract_declarator  */
#line 349 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                         { (yyval.ival) = 0; }
#line 2488 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 159: /* abstract_declarator: pointer direct_abstract_declarator  */
#line 350 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                         { (yyval.ival) = (yyvsp[-1].ival); }
#line 2494 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 160: /* direct_abstract_declarator: '(' abstract_declarator ')'  */
#line 354 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.ival) = 0; }
#line 2500 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 161: /* direct_abstract_declarator: '[' ']'  */
#line 355 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.ival) = 0; }
#line 2506 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 162: /* direct_abstract_declarator: '[' constant_expression ']'  */
#line 356 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.ival) = 0; }
#line 2512 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 163: /* direct_abstract_declarator: direct_abstract_declarator '[' ']'  */
#line 357 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.ival) = 0; }
#line 2518 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 164: /* direct_abstract_declarator: direct_abstract_declarator '[' constant_expression ']'  */
#line 358 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                                 { (yyval.ival) = 0; }
#line 2524 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 165: /* direct_abstract_declarator: '(' ')'  */
#line 359 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.ival) = 0; }
#line 2530 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 166: /* direct_abstract_declarator: '(' parameter_type_list ')'  */
#line 360 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.ival) = 0; }
#line 2536 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 167: /* direct_abstract_declarator: direct_abstract_declarator '(' ')'  */
#line 361 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                          { (yyval.ival) = 0; }
#line 2542 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 168: /* direct_abstract_declarator: direct_abstract_declarator '(' parameter_type_list ')'  */
#line 362 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                                 { (yyval.ival) = 0; }
#line 2548 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 169: /* initializer: assignment_expression  */
#line 366 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = (yyvsp[0].node); }
#line 2554 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 170: /* initializer: '{' initializer_list '}'  */
#line 367 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = 0; }
#line 2560 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 171: /* initializer: '{' initializer_list ',' '}'  */
#line 368 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = 0; }
#line 2566 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 172: /* initializer_list: initializer  */
#line 372 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.list) = 0; }
#line 2572 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 173: /* initializer_list: initializer_list ',' initializer  */
#line 373 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.list) = 0; }
#line 2578 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 174: /* statement: labeled_statement  */
#line 377 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = (yyvsp[0].node); }
#line 2584 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 175: /* statement: compound_statement  */
#line 378 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = (yyvsp[0].node); }
#line 2590 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 176: /* statement: expression_statement  */
#line 379 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = (yyvsp[0].node); }
#line 2596 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 177: /* statement: selection_statement  */
#line 380 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = (yyvsp[0].node); }
#line 2602 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 178: /* statement: iteration_statement  */
#line 381 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = (yyvsp[0].node); }
#line 2608 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 179: /* statement: jump_statement  */
#line 382 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = (yyvsp[0].node); }
#line 2614 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 180: /* labeled_statement: IDENTIFIER ':' statement  */
#line 386 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = ast_new_stmt_label((yyvsp[-2].str), (yyvsp[0].node), line, column); free((yyvsp[-2].str)); }
#line 2620 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 181: /* labeled_statement: CASE constant_expression ':' statement  */
#line 387 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = ast_new_stmt_case((yyvsp[-2].node), (yyvsp[0].node), line, column); }
#line 2626 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 182: /* labeled_statement: DEFAULT ':' statement  */
#line 388 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = ast_new_stmt_default((yyvsp[0].node), line, column); }
#line 2632 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 183: /* compound_statement: '{' '}'  */
#line 392 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = ast_new_stmt_compound(0, line, column); }
#line 2638 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 184: /* compound_statement: '{' statement_list '}'  */
#line 393 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = ast_new_stmt_compound((yyvsp[-1].list), line, column); }
#line 2644 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 185: /* compound_statement: '{' declaration_list '}'  */
#line 394 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = ast_new_stmt_compound((yyvsp[-1].list), line, column); }
#line 2650 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 186: /* compound_statement: '{' declaration_list statement_list '}'  */
#line 395 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = ast_new_stmt_compound(ast_list_concat((yyvsp[-2].list), (yyvsp[-1].list)), line, column); }
#line 2656 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 187: /* declaration_list: declaration  */
#line 399 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.list) = ast_list_append(0, (yyvsp[0].node)); }
#line 2662 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 188: /* declaration_list: declaration_list declaration  */
#line 400 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.list) = ast_list_append((yyvsp[-1].list), (yyvsp[0].node)); }
#line 2668 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 189: /* statement_list: statement  */
#line 404 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.list) = ast_list_append(0, (yyvsp[0].node)); }
#line 2674 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 190: /* statement_list: statement_list statement  */
#line 405 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.list) = ast_list_append((yyvsp[-1].list), (yyvsp[0].node)); }
#line 2680 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 191: /* expression_statement: ';'  */
#line 409 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = ast_new_stmt_expr(0, line, column); }
#line 2686 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 192: /* expression_statement: expression ';'  */
#line 410 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = ast_new_stmt_expr((yyvsp[-1].node), line, column); }
#line 2692 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 193: /* selection_statement: IF '(' expression ')' statement  */
#line 414 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = ast_new_stmt_if((yyvsp[-2].node), (yyvsp[0].node), 0, line, column); }
#line 2698 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 194: /* selection_statement: IF '(' expression ')' statement ELSE statement  */
#line 415 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = ast_new_stmt_if((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node), line, column); }
#line 2704 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 195: /* selection_statement: SWITCH '(' expression ')' statement  */
#line 416 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = ast_new_stmt_switch((yyvsp[-2].node), (yyvsp[0].node), line, column); }
#line 2710 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 196: /* iteration_statement: WHILE '(' expression ')' statement  */
#line 420 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = ast_new_stmt_while((yyvsp[-2].node), (yyvsp[0].node), line, column); }
#line 2716 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 197: /* iteration_statement: DO statement WHILE '(' expression ')' ';'  */
#line 421 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = ast_new_stmt_dowhile((yyvsp[-5].node), (yyvsp[-2].node), line, column); }
#line 2722 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 198: /* iteration_statement: FOR '(' expression_statement expression_statement ')' statement  */
#line 422 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                                          { (yyval.node) = ast_new_stmt_for((yyvsp[-3].node), (yyvsp[-2].node), 0, (yyvsp[0].node), line, column); }
#line 2728 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 199: /* iteration_statement: FOR '(' expression_statement expression_statement expression ')' statement  */
#line 423 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                                                     { (yyval.node) = ast_new_stmt_for((yyvsp[-4].node), (yyvsp[-3].node), (yyvsp[-2].node), (yyvsp[0].node), line, column); }
#line 2734 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 200: /* jump_statement: GOTO IDENTIFIER ';'  */
#line 427 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = ast_new_stmt_goto((yyvsp[-1].str), line, column); free((yyvsp[-1].str)); }
#line 2740 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 201: /* jump_statement: CONTINUE ';'  */
#line 428 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = ast_new_stmt_continue(line, column); }
#line 2746 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 202: /* jump_statement: BREAK ';'  */
#line 429 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = ast_new_stmt_break(line, column); }
#line 2752 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 203: /* jump_statement: RETURN ';'  */
#line 430 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = ast_new_stmt_return(0, line, column); }
#line 2758 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 204: /* jump_statement: RETURN expression ';'  */
#line 431 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = ast_new_stmt_return((yyvsp[-1].node), line, column); }
#line 2764 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 205: /* translation_unit: external_declaration  */
#line 435 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = ast_new_stmt_compound(ast_list_append(0, (yyvsp[0].node)), line, column); parse_tree = (yyval.node); }
#line 2770 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 206: /* translation_unit: translation_unit external_declaration  */
#line 436 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = (yyvsp[-1].node); (yyval.node)->u.stmt.stmts = ast_list_append((yyval.node)->u.stmt.stmts, (yyvsp[0].node)); parse_tree = (yyval.node); }
#line 2776 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 207: /* external_declaration: function_definition  */
#line 440 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = (yyvsp[0].node); }
#line 2782 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 208: /* external_declaration: declaration  */
#line 441 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                           { (yyval.node) = ast_new_external((yyvsp[0].node), 0, line, column); }
#line 2788 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 209: /* function_definition: declaration_specifiers declarator declaration_list compound_statement  */
#line 445 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                                                { (yyval.node) = ast_new_external(ast_new_decl((yyvsp[-3].ival), (yyvsp[-2].decltor), 0, line, column), (yyvsp[0].node), line, column); }
#line 2794 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 210: /* function_definition: declaration_specifiers declarator compound_statement  */
#line 446 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                               { (yyval.node) = ast_new_external(ast_new_decl((yyvsp[-2].ival), (yyvsp[-1].decltor), 0, line, column), (yyvsp[0].node), line, column); }
#line 2800 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 211: /* function_definition: declarator declaration_list compound_statement  */
#line 447 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                                         { (yyval.node) = ast_new_external(ast_new_decl(0, (yyvsp[-2].decltor), 0, line, column), (yyvsp[0].node), line, column); }
#line 2806 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;

  case 212: /* function_definition: declarator compound_statement  */
#line 448 "/home/atirut/Projects/zdk/cc/src/parser.y"
                                        { (yyval.node) = ast_new_external(ast_new_decl(0, (yyvsp[-1].decltor), 0, line, column), (yyvsp[0].node), line, column); }
#line 2812 "/home/atirut/Projects/zdk/cc/src/y.tab.c"
    break;


#line 2816 "/home/atirut/Projects/zdk/cc/src/y.tab.c"

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

#line 451 "/home/atirut/Projects/zdk/cc/src/parser.y"

#include <stdio.h>

extern char yytext[];
extern int line;
extern int column;
extern const char *yyfilename;
extern char current_line[];
extern int current_line_len;

/* compute digit width for gutter */
static int num_width(int n)
{
	int w;
	if (n <= 0) return 1;
	w = 0;
	while (n > 0)
	{
		w++;
		n /= 10;
	}
	return w;
}

/* print left gutter: either "<lineno> | " or "    | " with matching width */
static void print_gutter(FILE *out, int lineno)
{
	int width, i;
	width = num_width(line); /* width is based on current error line */
	if (lineno > 0)
	{
		fprintf(out, "%*d | ", width, lineno);
	}
	else
	{
		for (i = 0; i < width; i++) fputc(' ', out);
		fputs(" | ", out);
	}
}

static void print_caret_line(FILE *out)
{
	int vc, target, idx;

	/* gutter aligned to offending line width */
	print_gutter(out, 0);

	vc = 0;
	/* Place caret under the offending character (column is 1-based visually) */
	target = column > 0 ? column - 1 : 0;

	for (idx = 0; idx < current_line_len && vc < target; idx++)
	{
		if (current_line[idx] == '\t')
		{
			/* keep tabs so caret aligns visually */
			fputc('\t', out);
			vc += 8 - (vc % 8);
		}
		else
		{
			fputc(' ', out);
			vc++;
		}
	}
	fputc('^', out);
	fputc('\n', out);
}

int yyerror(char *s)
{
	const char *fname = yyfilename ? yyfilename : "<stdin>";

	/* error header */
	fprintf(stderr, "%s:%d:%d: error: %s\n", fname, line, column, s);

	/* offending line with gutter */
	if (current_line_len > 0)
	{
		print_gutter(stderr, line);
		fwrite(current_line, 1, (size_t)current_line_len, stderr);
		fputc('\n', stderr);
	}

	/* caret line with matching gutter */
	print_caret_line(stderr);
	return 0;
}

/* Accessor for the built AST */
struct ASTNode *parser_get_tree(void) { return parse_tree; }
