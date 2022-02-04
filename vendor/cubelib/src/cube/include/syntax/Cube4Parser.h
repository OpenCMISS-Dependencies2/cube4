// A Bison parser, made by GNU Bison 3.0.4.

// Skeleton interface for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

/**
 ** \file ../../cubelib/build-frontend/../src/cube/src/syntax/Cube4Parser.hpp
 ** Define the cubeparser::parser class.
 */

// C++ LALR(1) parser skeleton written by Akim Demaille.

#ifndef YY_CUBEPARSER_CUBELIB_BUILD_FRONTEND_SRC_CUBE_SRC_SYNTAX_CUBE4PARSER_HPP_INCLUDED
# define YY_CUBEPARSER_CUBELIB_BUILD_FRONTEND_SRC_CUBE_SRC_SYNTAX_CUBE4PARSER_HPP_INCLUDED
// //                    "%code requires" blocks.
#line 18 "../../cubelib/build-frontend/../src/cube/src/syntax/Cube4Parser.yy" // lalr1.cc:377
 
/*** C/C++ Declarations ***/

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <utility>
#include <string>
#include <vector>
#include "CubeTypes.h"
#include "CubeSysres.h"
#include "CubeLocation.h"
#include "CubeLocationGroup.h"
#include "CubeSystemTreeNode.h"

#include "CubeRegion.h"

#include "CubeError.h"
#include "CubeServices.h"
#include "CubeError.h"
#include "CubeParseContext.h"
#include "CubeMetric.h"
#include "Cube.h"
#include "CubeCartesian.h"


#line 71 "../../cubelib/build-frontend/../src/cube/src/syntax/Cube4Parser.hpp" // lalr1.cc:377


# include <cstdlib> // std::abort
# include <iostream>
# include <stdexcept>
# include <string>
# include <vector>
# include "stack.hh"
# include "location.hh"


#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
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

/* Debug traces.  */
#ifndef CUBEPARSERDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define CUBEPARSERDEBUG 1
#  else
#   define CUBEPARSERDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define CUBEPARSERDEBUG 1
# endif /* ! defined YYDEBUG */
#endif  /* ! defined CUBEPARSERDEBUG */


namespace cubeparser {
#line 151 "../../cubelib/build-frontend/../src/cube/src/syntax/Cube4Parser.hpp" // lalr1.cc:377





  /// A Bison parser.
  class Cube4Parser
  {
  public:
#ifndef CUBEPARSERSTYPE
    /// Symbol semantic values.
    union semantic_type
    {
    #line 95 "../../cubelib/build-frontend/../src/cube/src/syntax/Cube4Parser.yy" // lalr1.cc:377

#line 167 "../../cubelib/build-frontend/../src/cube/src/syntax/Cube4Parser.hpp" // lalr1.cc:377
    };
#else
    typedef CUBEPARSERSTYPE semantic_type;
#endif
    /// Symbol locations.
    typedef location location_type;

    /// Syntax errors thrown from user actions.
    struct syntax_error : std::runtime_error
    {
      syntax_error (const location_type& l, const std::string& m);
      location_type location;
    };

    /// Tokens.
    struct token
    {
      enum yytokentype
      {
        END = 0,
        FFALSE = 1002,
        TTRUE = 1003,
        ATTRIBUTE_VALUE = 1004,
        ERROR = 1005,
        ACUBEPL_AGGR_ATTR_PLUS = 1006,
        ACUBEPL_AGGR_ATTR_MINUS = 1007,
        ACUBEPL_AGGR_ATTR_AGGR = 1008,
        OPENTAG_RIGHT = 1009,
        CLOSETAG_RIGHT = 1010,
        ATTR_OPEN = 1011,
        DIM_OPEN = 1012,
        XML_OPEN = 1013,
        XML_CLOSE = 1014,
        CUBE_OPEN4 = 1015,
        CUBE_OPEN41 = 1016,
        CUBE_OPEN42 = 1017,
        CUBE_OPEN43 = 1018,
        CUBE_OPEN44 = 1019,
        CUBE_OPEN45 = 1020,
        CUBE_OPEN3 = 1021,
        CUBE_OPEN = 1022,
        CUBE_OPEN_NOT_SUPPORTED = 1000,
        CUBE_CLOSE = 1023,
        METRIC_OPEN = 1024,
        METRIC_CLOSE = 1025,
        EXPRESSION_OPEN = 1026,
        EXPRESSION_CLOSE = 1027,
        EXPRESSION_INIT_OPEN = 1028,
        EXPRESSION_INIT_CLOSE = 1029,
        EXPRESSION_AGGR_OPEN = 1030,
        EXPRESSION_AGGR_CLOSE = 1031,
        REGION_OPEN = 1032,
        REGION_CLOSE = 1033,
        CNODE_OPEN = 1034,
        CNODE_CLOSE = 1035,
        CNODE_PARAMETER_OPEN = 1036,
        SYSTEM_TREE_NODE_OPEN = 1037,
        SYSTEM_TREE_NODE_CLOSE = 1038,
        LOCATIONGROUP_OPEN = 1039,
        LOCATIONGROUP_CLOSE = 1040,
        LOCATION_OPEN = 1041,
        LOCATION_CLOSE = 1042,
        MACHINE_OPEN = 1043,
        MACHINE_CLOSE = 1044,
        NODE_OPEN = 1045,
        NODE_CLOSE = 1046,
        PROCESS_OPEN = 1047,
        PROCESS_CLOSE = 1048,
        THREAD_OPEN = 1049,
        THREAD_CLOSE = 1050,
        MATRIX_OPEN = 1051,
        MATRIX_CLOSE = 1052,
        ROW_OPEN = 1053,
        ROW_CLOSE = 1054,
        CART_OPEN = 1055,
        CART_CLOSE = 1056,
        COORD_OPEN = 1057,
        COORD_CLOSE = 1058,
        DOC_OPEN = 1059,
        DOC_CLOSE = 1060,
        MIRRORS_OPEN = 1061,
        MIRRORS_CLOSE = 1062,
        MURL_OPEN = 1063,
        MURL_CLOSE = 1064,
        METRICS_OPEN = 1065,
        METRICS_CLOSE = 1066,
        DISP_NAME_OPEN = 1067,
        DISP_NAME_CLOSE = 1068,
        UNIQ_NAME_OPEN = 1069,
        UNIQ_NAME_CLOSE = 1070,
        DTYPE_OPEN = 1071,
        DTYPE_CLOSE = 1072,
        UOM_OPEN = 1073,
        UOM_CLOSE = 1074,
        VAL_OPEN = 1075,
        VAL_CLOSE = 1076,
        URL_OPEN = 1077,
        URL_CLOSE = 1078,
        DESCR_OPEN = 1079,
        DESCR_CLOSE = 1080,
        PARADIGM_OPEN = 1081,
        PARADIGM_CLOSE = 1082,
        ROLE_OPEN = 1083,
        ROLE_CLOSE = 1084,
        PROGRAM_OPEN = 1085,
        PROGRAM_CLOSE = 1086,
        NAME_OPEN = 1087,
        NAME_CLOSE = 1088,
        MANGLED_NAME_OPEN = 1089,
        MANGLED_NAME_CLOSE = 1090,
        CLASS_OPEN = 1091,
        CLASS_CLOSE = 1092,
        TYPE_OPEN = 1093,
        TYPE_CLOSE = 1094,
        SYSTEM_OPEN = 1095,
        SYSTEM_CLOSE = 1096,
        RANK_OPEN = 1097,
        RANK_CLOSE = 1098,
        TOPOLOGIES_OPEN = 1099,
        TOPOLOGIES_CLOSE = 1100,
        SEVERITY_OPEN = 1101,
        SEVERITY_CLOSE = 1102,
        ANAME_VERSION = 1103,
        ANAME_ENCODING = 1104,
        ANAME_KEY = 1105,
        ANAME_PAR_TYPE = 1106,
        ANAME_PAR_KEY = 1107,
        ANAME_PAR_VALUE = 1108,
        ANAME_NAME = 1109,
        ANAME_TITLE = 1110,
        ANAME_FILE = 1111,
        ANAME_VALUE = 1112,
        ANAME_ID = 1113,
        ANAME_MOD = 1114,
        ANAME_BEGIN = 1115,
        ANAME_END = 1116,
        ANAME_LINE = 1117,
        ANAME_CALLEEID = 1118,
        ANAME_NDIMS = 1119,
        ANAME_SIZE = 1120,
        ANAME_PERIODIC = 1121,
        ANAME_LOCATIONID = 1122,
        ANAME_LOCATIONGROUPID = 1123,
        ANAME_STNODEID = 1124,
        ANAME_THRDID = 1125,
        ANAME_PROCID = 1126,
        ANAME_NODEID = 1127,
        ANAME_MACHID = 1128,
        ANAME_METRICID = 1129,
        ANAME_METRIC_TYPE = 1130,
        ANAME_METRIC_VIZ_TYPE = 1131,
        ANAME_METRIC_CONVERTIBLE = 1132,
        ANAME_METRIC_CACHEABLE = 1133,
        ANAME_CNODEID = 1134,
        ACUBEPL_ROWWISE = 1135,
        ACUBEPL_AGGR_ATTRIBUTE = 1136
      };
    };

    /// (External) token type, as returned by yylex.
    typedef token::yytokentype token_type;

    /// Symbol type: an internal symbol number.
    typedef int symbol_number_type;

    /// The symbol type number to denote an empty symbol.
    enum { empty_symbol = -2 };

    /// Internal symbol number for tokens (subsumed by symbol_number_type).
    typedef unsigned char token_number_type;

    /// A complete symbol.
    ///
    /// Expects its Base type to provide access to the symbol type
    /// via type_get().
    ///
    /// Provide access to semantic value and location.
    template <typename Base>
    struct basic_symbol : Base
    {
      /// Alias to Base.
      typedef Base super_type;

      /// Default constructor.
      basic_symbol ();

      /// Copy constructor.
      basic_symbol (const basic_symbol& other);

      /// Constructor for valueless symbols.
      basic_symbol (typename Base::kind_type t,
                    const location_type& l);

      /// Constructor for symbols with semantic value.
      basic_symbol (typename Base::kind_type t,
                    const semantic_type& v,
                    const location_type& l);

      /// Destroy the symbol.
      ~basic_symbol ();

      /// Destroy contents, and record that is empty.
      void clear ();

      /// Whether empty.
      bool empty () const;

      /// Destructive move, \a s is emptied into this.
      void move (basic_symbol& s);

      /// The semantic value.
      semantic_type value;

      /// The location.
      location_type location;

    private:
      /// Assignment operator.
      basic_symbol& operator= (const basic_symbol& other);
    };

    /// Type access provider for token (enum) based symbols.
    struct by_type
    {
      /// Default constructor.
      by_type ();

      /// Copy constructor.
      by_type (const by_type& other);

      /// The symbol type as needed by the constructor.
      typedef token_type kind_type;

      /// Constructor from (external) token numbers.
      by_type (kind_type t);

      /// Record that this symbol is empty.
      void clear ();

      /// Steal the symbol type from \a that.
      void move (by_type& that);

      /// The (internal) type number (corresponding to \a type).
      /// \a empty when empty.
      symbol_number_type type_get () const;

      /// The token.
      token_type token () const;

      /// The symbol type.
      /// \a empty_symbol when empty.
      /// An int, not token_number_type, to be able to store empty_symbol.
      int type;
    };

    /// "External" symbols: returned by the scanner.
    typedef basic_symbol<by_type> symbol_type;


    /// Build a parser object.
    Cube4Parser (class Driver& driver_yyarg, class ParseContext& parseContext_yyarg, class Cube4Scanner& Cube4Lexer_yyarg, class cube::Cube& cube_yyarg, bool& clustering_on_yyarg);
    virtual ~Cube4Parser ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if CUBEPARSERDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Report a syntax error.
    void error (const syntax_error& err);

  private:
    /// This class is not copyable.
    Cube4Parser (const Cube4Parser&);
    Cube4Parser& operator= (const Cube4Parser&);

    /// State numbers.
    typedef int state_type;

    /// Generate an error message.
    /// \param yystate   the state where the error occurred.
    /// \param yyla      the lookahead token.
    virtual std::string yysyntax_error_ (state_type yystate,
                                         const symbol_type& yyla) const;

    /// Compute post-reduction state.
    /// \param yystate   the current state
    /// \param yysym     the nonterminal to push on the stack
    state_type yy_lr_goto_state_ (state_type yystate, int yysym);

    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue);

    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue);

    static const short int yypact_ninf_;
    static const signed char yytable_ninf_;

    /// Convert a scanner token number \a t to a symbol number.
    static token_number_type yytranslate_ (int t);

    // Tables.
  // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
  // STATE-NUM.
  static const short int yypact_[];

  // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
  // Performed when YYTABLE does not specify something else to do.  Zero
  // means the default is an error.
  static const unsigned short int yydefact_[];

  // YYPGOTO[NTERM-NUM].
  static const short int yypgoto_[];

  // YYDEFGOTO[NTERM-NUM].
  static const short int yydefgoto_[];

  // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
  // positive, shift that token.  If negative, reduce the rule whose
  // number is the opposite.  If YYTABLE_NINF, syntax error.
  static const unsigned short int yytable_[];

  static const short int yycheck_[];

  // YYSTOS[STATE-NUM] -- The (internal number of the) accessing
  // symbol of state STATE-NUM.
  static const unsigned short int yystos_[];

  // YYR1[YYN] -- Symbol number of symbol that rule YYN derives.
  static const unsigned short int yyr1_[];

  // YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.
  static const unsigned char yyr2_[];


    /// Convert the symbol name \a n to a form suitable for a diagnostic.
    static std::string yytnamerr_ (const char *n);


    /// For a symbol, its name in clear.
    static const char* const yytname_[];
#if CUBEPARSERDEBUG
  // YYRLINE[YYN] -- Source line where rule number YYN was defined.
  static const unsigned short int yyrline_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r);
    /// Print the state stack on the debug stream.
    virtual void yystack_print_ ();

    // Debugging.
    int yydebug_;
    std::ostream* yycdebug_;

    /// \brief Display a symbol type, value and location.
    /// \param yyo    The output stream.
    /// \param yysym  The symbol.
    template <typename Base>
    void yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const;
#endif

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg     Why this token is reclaimed.
    ///                  If null, print nothing.
    /// \param yysym     The symbol.
    template <typename Base>
    void yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const;

  private:
    /// Type access provider for state based symbols.
    struct by_state
    {
      /// Default constructor.
      by_state ();

      /// The symbol type as needed by the constructor.
      typedef state_type kind_type;

      /// Constructor.
      by_state (kind_type s);

      /// Copy constructor.
      by_state (const by_state& other);

      /// Record that this symbol is empty.
      void clear ();

      /// Steal the symbol type from \a that.
      void move (by_state& that);

      /// The (internal) type number (corresponding to \a state).
      /// \a empty_symbol when empty.
      symbol_number_type type_get () const;

      /// The state number used to denote an empty symbol.
      enum { empty_state = -1 };

      /// The state.
      /// \a empty when empty.
      state_type state;
    };

    /// "Internal" symbol: element of the stack.
    struct stack_symbol_type : basic_symbol<by_state>
    {
      /// Superclass.
      typedef basic_symbol<by_state> super_type;
      /// Construct an empty symbol.
      stack_symbol_type ();
      /// Steal the contents from \a sym to build this.
      stack_symbol_type (state_type s, symbol_type& sym);
      /// Assignment, needed by push_back.
      stack_symbol_type& operator= (const stack_symbol_type& that);
    };

    /// Stack type.
    typedef stack<stack_symbol_type> stack_type;

    /// The stack.
    stack_type yystack_;

    /// Push a new state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the symbol
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, stack_symbol_type& s);

    /// Push a new look ahead token on the state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the state
    /// \param sym  the symbol (for its value and location).
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, state_type s, symbol_type& sym);

    /// Pop \a n symbols the three stacks.
    void yypop_ (unsigned int n = 1);

    /// Constants.
    enum
    {
      yyeof_ = 0,
      yylast_ = 350,     ///< Last index in yytable_.
      yynnts_ = 167,  ///< Number of nonterminal symbols.
      yyfinal_ = 10, ///< Termination state number.
      yyterror_ = 1,
      yyerrcode_ = 256,
      yyntokens_ = 139  ///< Number of tokens.
    };


    // User arguments.
    class Driver& driver;
    class ParseContext& parseContext;
    class Cube4Scanner& Cube4Lexer;
    class cube::Cube& cube;
    bool& clustering_on;
  };



} // cubeparser
#line 651 "../../cubelib/build-frontend/../src/cube/src/syntax/Cube4Parser.hpp" // lalr1.cc:377




#endif // !YY_CUBEPARSER_CUBELIB_BUILD_FRONTEND_SRC_CUBE_SRC_SYNTAX_CUBE4PARSER_HPP_INCLUDED
