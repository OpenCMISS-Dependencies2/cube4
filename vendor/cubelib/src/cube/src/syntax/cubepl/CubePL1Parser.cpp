// A Bison parser, made by GNU Bison 3.0.4.

// Skeleton implementation for Bison LALR(1) parsers in C++

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

// Take the name prefix into account.
#define yylex   cubeplparserlex

// First part of user declarations.

#line 39 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:404

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "CubePL1Parser.h"

// User implementation prologue.
#line 263 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:412

#include "CubeTypes.h"
#include "CubeSysres.h"
#include "CubeLocation.h"
#include "CubeLocationGroup.h"
#include "CubeSystemTreeNode.h"
#include "CubePL1Scanner.h"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <stdlib.h>

using namespace std;
using namespace cube;

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <utility>
#include <string>
#include <vector>
#include "CubePL1ParseContext.h"

/* this "connects" the bison parser in the driver to the flex scanner class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the driver context. */
#undef yylex
#define yylex CubePL1Lexer.lex

// Workaround for Sun Studio C++ compilers on Solaris
#if defined(__SVR4) &&  defined(__SUNPRO_CC)
  #include <ieeefp.h>

  #define isinf(x)  (fpclass(x) == FP_NINF || fpclass(x) == FP_PINF)
  #define isnan(x)  isnand(x)
#endif


#line 91 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:412


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (/*CONSTCOND*/ false)
# endif


// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if CUBEPLPARSERDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << std::endl;                  \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !CUBEPLPARSERDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE(Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void>(0)
# define YY_STACK_PRINT()                static_cast<void>(0)

#endif // !CUBEPLPARSERDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


namespace cubeplparser {
#line 177 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:479

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  CubePL1Parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr = "";
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
              // Fall through.
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }


  /// Build a parser object.
  CubePL1Parser::CubePL1Parser (class CubePL1ParseContext& parseContext_yyarg, class CubePL1Scanner& CubePL1Lexer_yyarg)
    :
#if CUBEPLPARSERDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      parseContext (parseContext_yyarg),
      CubePL1Lexer (CubePL1Lexer_yyarg)
  {}

  CubePL1Parser::~CubePL1Parser ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/

  inline
  CubePL1Parser::syntax_error::syntax_error (const location_type& l, const std::string& m)
    : std::runtime_error (m)
    , location (l)
  {}

  // basic_symbol.
  template <typename Base>
  inline
  CubePL1Parser::basic_symbol<Base>::basic_symbol ()
    : value ()
  {}

  template <typename Base>
  inline
  CubePL1Parser::basic_symbol<Base>::basic_symbol (const basic_symbol& other)
    : Base (other)
    , value ()
    , location (other.location)
  {
    value = other.value;
  }


  template <typename Base>
  inline
  CubePL1Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const semantic_type& v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  inline
  CubePL1Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const location_type& l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
  inline
  CubePL1Parser::basic_symbol<Base>::~basic_symbol ()
  {
    clear ();
  }

  template <typename Base>
  inline
  void
  CubePL1Parser::basic_symbol<Base>::clear ()
  {
    Base::clear ();
  }

  template <typename Base>
  inline
  bool
  CubePL1Parser::basic_symbol<Base>::empty () const
  {
    return Base::type_get () == empty_symbol;
  }

  template <typename Base>
  inline
  void
  CubePL1Parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move(s);
    value = s.value;
    location = s.location;
  }

  // by_type.
  inline
  CubePL1Parser::by_type::by_type ()
    : type (empty_symbol)
  {}

  inline
  CubePL1Parser::by_type::by_type (const by_type& other)
    : type (other.type)
  {}

  inline
  CubePL1Parser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  inline
  void
  CubePL1Parser::by_type::clear ()
  {
    type = empty_symbol;
  }

  inline
  void
  CubePL1Parser::by_type::move (by_type& that)
  {
    type = that.type;
    that.clear ();
  }

  inline
  int
  CubePL1Parser::by_type::type_get () const
  {
    return type;
  }


  // by_state.
  inline
  CubePL1Parser::by_state::by_state ()
    : state (empty_state)
  {}

  inline
  CubePL1Parser::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  inline
  void
  CubePL1Parser::by_state::clear ()
  {
    state = empty_state;
  }

  inline
  void
  CubePL1Parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  inline
  CubePL1Parser::by_state::by_state (state_type s)
    : state (s)
  {}

  inline
  CubePL1Parser::symbol_number_type
  CubePL1Parser::by_state::type_get () const
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[state];
  }

  inline
  CubePL1Parser::stack_symbol_type::stack_symbol_type ()
  {}


  inline
  CubePL1Parser::stack_symbol_type::stack_symbol_type (state_type s, symbol_type& that)
    : super_type (s, that.location)
  {
    value = that.value;
    // that is emptied.
    that.type = empty_symbol;
  }

  inline
  CubePL1Parser::stack_symbol_type&
  CubePL1Parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    return *this;
  }


  template <typename Base>
  inline
  void
  CubePL1Parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    YYUSE (yysym.type_get ());
  }

#if CUBEPLPARSERDEBUG
  template <typename Base>
  void
  CubePL1Parser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  inline
  void
  CubePL1Parser::yypush_ (const char* m, state_type s, symbol_type& sym)
  {
    stack_symbol_type t (s, sym);
    yypush_ (m, t);
  }

  inline
  void
  CubePL1Parser::yypush_ (const char* m, stack_symbol_type& s)
  {
    if (m)
      YY_SYMBOL_PRINT (m, s);
    yystack_.push (s);
  }

  inline
  void
  CubePL1Parser::yypop_ (unsigned int n)
  {
    yystack_.pop (n);
  }

#if CUBEPLPARSERDEBUG
  std::ostream&
  CubePL1Parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  CubePL1Parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  CubePL1Parser::debug_level_type
  CubePL1Parser::debug_level () const
  {
    return yydebug_;
  }

  void
  CubePL1Parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // CUBEPLPARSERDEBUG

  inline CubePL1Parser::state_type
  CubePL1Parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  inline bool
  CubePL1Parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  CubePL1Parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  CubePL1Parser::parse ()
  {
    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


    // User initialization code.
    #line 141 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:741
{
    // initialize the initial location object
}

#line 556 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:741

    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, yyla);

    // A new symbol was pushed on the stack.
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << std::endl;

    // Accept?
    if (yystack_[0].state == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    // Backup.
  yybackup:

    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
        try
          {
            yyla.type = yytranslate_ (yylex (&yyla.value, &yyla.location));
          }
        catch (const syntax_error& yyexc)
          {
            error (yyexc);
            goto yyerrlab1;
          }
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, yyla);
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_(yystack_[yylen].state, yyr1_[yyn]);
      /* If YYLEN is nonzero, implement the default value of the
         action: '$$ = $1'.  Otherwise, use the top of the stack.

         Otherwise, the following line sets YYLHS.VALUE to garbage.
         This behavior is undocumented and Bison users should not rely
         upon it.  */
      if (yylen)
        yylhs.value = yystack_[yylen - 1].value;
      else
        yylhs.value = yystack_[0].value;

      // Compute the default @$.
      {
        slice<stack_symbol_type, stack_type> slice (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, slice, yylen);
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
      try
        {
          switch (yyn)
            {
  case 2:
#line 312 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
	    if (!parseContext.test_modus)
	    {
		parseContext.result = parseContext._stack.top();
		parseContext._stack.pop();
    	    }
    	    parseContext.syntax_ok &= true;
	}
#line 673 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 3:
#line 321 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
    	    parseContext.syntax_ok &= true;
	}
#line 681 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 23:
#line 350 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    GeneralEvaluation * _arg1 = parseContext._stack.top();
		    parseContext._stack.pop();
		    parseContext._stack.push( new EncapsulationEvaluation(_arg1) );
		}
		parseContext.syntax_ok &= true;
	    }
#line 695 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 24:
#line 360 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    GeneralEvaluation * _arg1 = parseContext._stack.top();
		    parseContext._stack.pop();
		    GeneralEvaluation * _abs = new AbsEvaluation();
		    _abs->addArgument(_arg1);
		    parseContext._stack.push( _abs );
		}
		parseContext.syntax_ok &= true;
	    }
#line 711 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 25:
#line 373 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    GeneralEvaluation * _arg1 = parseContext._stack.top();
		    parseContext._stack.pop();
		    GeneralEvaluation * _arg2 = parseContext._stack.top();
		    parseContext._stack.pop();
		    parseContext._stack.push( new PlusEvaluation(_arg1, _arg2) );
		}
		parseContext.syntax_ok &= true;
	    }
#line 727 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 26:
#line 386 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    GeneralEvaluation * _arg2 = parseContext._stack.top();
		    parseContext._stack.pop();
		    GeneralEvaluation * _arg1 = parseContext._stack.top();
		    parseContext._stack.pop();
		    parseContext._stack.push( new MinusEvaluation(_arg1, _arg2) );
		}
		parseContext.syntax_ok &= true;
	    }
#line 743 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 27:
#line 400 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    GeneralEvaluation * _arg2 = parseContext._stack.top();
		    parseContext._stack.pop();
		    GeneralEvaluation * _arg1 = parseContext._stack.top();
		    parseContext._stack.pop();
		    parseContext._stack.push( new DivideEvaluation(_arg1, _arg2) );
		}
		parseContext.syntax_ok &= true;
	    }
#line 759 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 28:
#line 413 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    GeneralEvaluation * _arg1 = parseContext._stack.top();
		    parseContext._stack.pop();
		    GeneralEvaluation * _arg2 = parseContext._stack.top();
		    parseContext._stack.pop();
		    parseContext._stack.push( new MultEvaluation(_arg1, _arg2) );
		}
		parseContext.syntax_ok &= true;
	    }
#line 775 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 29:
#line 429 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    GeneralEvaluation * power = parseContext._stack.top();
		    parseContext._stack.pop();
		    GeneralEvaluation * base = parseContext._stack.top();
		    parseContext._stack.pop();
		    parseContext._stack.push( new PowerEvaluation(base, power) );
		}
		parseContext.syntax_ok &= true;
	    }
#line 791 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 30:
#line 441 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    GeneralEvaluation * arg = parseContext._stack.top();
		    parseContext._stack.pop();
		    parseContext._stack.push( new NegativeEvaluation(arg) );
		}
		parseContext.syntax_ok &= true;
	    }
#line 805 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 33:
#line 455 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {  
		if (!parseContext.test_modus)
		{
			parseContext._stack.push(new ArgumentEvaluation(cube::FIRST_ARGUMENT) );
		}
		parseContext.syntax_ok &= true;
	    }
#line 817 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 34:
#line 464 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {  
		if (!parseContext.test_modus)
		{
			parseContext._stack.push(new ArgumentEvaluation(cube::SECOND_ARGUMENT) );
		}
		parseContext.syntax_ok &= true;
	    }
#line 829 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 48:
#line 492 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
			if (!parseContext.test_modus)
			{
			    GeneralEvaluation * _arg1 = parseContext._stack.top();
			    parseContext._stack.pop();
			    parseContext._stack.push( new EncapsulationEvaluation(_arg1) );
			}
			parseContext.syntax_ok &= true;
		    }
#line 843 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 49:
#line 502 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
			if (!parseContext.test_modus)
			{
			    GeneralEvaluation * _arg1 = parseContext._stack.top();
			    parseContext._stack.pop();
			    parseContext._stack.push( new NotEvaluation(_arg1) );
			}
			parseContext.syntax_ok &= true;
		    }
#line 857 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 50:
#line 513 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    GeneralEvaluation * _arg2 = parseContext._stack.top();
		    parseContext._stack.pop();
		    GeneralEvaluation * _arg1 = parseContext._stack.top();
		    parseContext._stack.pop();
		    parseContext._stack.push( new AndEvaluation(_arg1, _arg2) );

		}
		parseContext.syntax_ok &= true;
	    }
#line 874 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 51:
#line 526 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    GeneralEvaluation * _arg2 = parseContext._stack.top();
		    parseContext._stack.pop();
		    GeneralEvaluation * _arg1 = parseContext._stack.top();
		    parseContext._stack.pop();
		    parseContext._stack.push( new OrEvaluation(_arg1, _arg2) );

		}
		parseContext.syntax_ok &= true;
	    }
#line 891 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 52:
#line 540 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    GeneralEvaluation * _arg2 = parseContext._stack.top();
		    parseContext._stack.pop();
		    GeneralEvaluation * _arg1 = parseContext._stack.top();
		    parseContext._stack.pop();
		    parseContext._stack.push( new XorEvaluation(_arg1, _arg2) );

		}
		parseContext.syntax_ok &= true;
	    }
#line 908 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 53:
#line 553 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    parseContext._stack.push( new ConstantEvaluation(1) );
		}
		parseContext.syntax_ok &= true;
	    }
#line 920 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 54:
#line 561 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    parseContext._stack.push( new ConstantEvaluation(0) );
		}
		parseContext.syntax_ok &= true;
	    }
#line 932 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 55:
#line 570 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    GeneralEvaluation * _arg2 = parseContext._stack.top();
		    parseContext._stack.pop();
		    GeneralEvaluation * _arg1 = parseContext._stack.top();
		    parseContext._stack.pop();
		    parseContext._stack.push( new EqualEvaluation(_arg1, _arg2) );

		}
		parseContext.syntax_ok &= true;
	    }
#line 949 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 56:
#line 584 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    GeneralEvaluation * _arg2 = parseContext._stack.top();
		    parseContext._stack.pop();
		    GeneralEvaluation * _arg1 = parseContext._stack.top();
		    parseContext._stack.pop();
		    parseContext._stack.push( new NotEqualEvaluation(_arg1, _arg2) );
		}
		parseContext.syntax_ok &= true;
	    }
#line 965 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 57:
#line 598 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    GeneralEvaluation * _arg2 = parseContext._stack.top();
		    parseContext._stack.pop();
		    GeneralEvaluation * _arg1 = parseContext._stack.top();
		    parseContext._stack.pop();
		    parseContext._stack.push( new BiggerEvaluation(_arg1, _arg2) );
		}
		parseContext.syntax_ok &= true;
	    }
#line 981 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 58:
#line 611 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    GeneralEvaluation * _arg2 = parseContext._stack.top();
		    parseContext._stack.pop();
		    GeneralEvaluation * _arg1 = parseContext._stack.top();
		    parseContext._stack.pop();
		    parseContext._stack.push( new SmallerEvaluation(_arg1, _arg2) );
		}
		parseContext.syntax_ok &= true;
	    }
#line 997 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 59:
#line 625 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    GeneralEvaluation * _arg2 = parseContext._stack.top();
		    parseContext._stack.pop();
		    GeneralEvaluation * _arg1 = parseContext._stack.top();
		    parseContext._stack.pop();
		    parseContext._stack.push( new HalfBiggerEvaluation(_arg1, _arg2) );
		}
		parseContext.syntax_ok &= true;
	    }
#line 1013 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 60:
#line 639 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    GeneralEvaluation * _arg2 = parseContext._stack.top();
		    parseContext._stack.pop();
		    GeneralEvaluation * _arg1 = parseContext._stack.top();
		    parseContext._stack.pop();
		    parseContext._stack.push( new HalfSmallerEvaluation(_arg1, _arg2) );
		}
		parseContext.syntax_ok &= true;
	    }
#line 1029 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 64:
#line 662 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {
		    if (!parseContext.test_modus)
		    {
			GeneralEvaluation * _value2 =   parseContext._stack.top();
			parseContext._stack.pop();
			GeneralEvaluation * _value1 =   parseContext._stack.top();
			parseContext._stack.pop();
			parseContext._stack.push( new StringEqualityEvaluation(_value1, _value2 ) );
		    }
		    parseContext.syntax_ok &= true;
		}
#line 1045 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 65:
#line 675 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {
		    if (!parseContext.test_modus)
		    {
			GeneralEvaluation * _value2 =   parseContext._stack.top();
			parseContext._stack.pop();
			GeneralEvaluation * _value1 =   parseContext._stack.top();
			parseContext._stack.pop();
			parseContext._stack.push( new StringSemiEqualityEvaluation(_value1, _value2 ) );
		    }
		    parseContext.syntax_ok &= true;
		}
#line 1061 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 66:
#line 687 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {
		    if (!parseContext.test_modus)
		    {
			string _regexp =  parseContext.strings_stack.top();
			parseContext.strings_stack.pop();
			GeneralEvaluation * _value =   parseContext._stack.top();
			parseContext._stack.pop();
			parseContext._stack.push( new RegexEvaluation(_value, new StringConstantEvaluation(_regexp) ) );
		    }
		    parseContext.syntax_ok &= true;
		}
#line 1077 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 67:
#line 702 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {
		    string _tmp = parseContext.str.str();
		    int _result = 0;
            try
            {
                std::regex self_regex( _tmp );
            }
            catch ( const std::regex_error& e )
            {
                _result=-1;
            }
		    if (_result != 0 )
		    {
                parseContext.syntax_ok &= false;
		    }
		    if (_result==0 && !parseContext.test_modus)
		    {
                parseContext.strings_stack.push( _tmp );
                parseContext.str.str("");
		    }	
		    parseContext.syntax_ok &= true;
		}
#line 1104 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 68:
#line 730 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {
		    if (!parseContext.test_modus)
		    {		    
		    parseContext._stack.push( new StringConstantEvaluation( parseContext.str.str() ));
		    parseContext.str.str("");
		    }
		    parseContext.syntax_ok &= true;		    
		}
#line 1117 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 72:
#line 746 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
			if (!parseContext.test_modus)
			{
			    GeneralEvaluation * _arg1 = parseContext._stack.top();
			    parseContext._stack.pop();
			    parseContext._stack.push( new LowerCaseEvaluation(_arg1) );
			}
			parseContext.syntax_ok &= true;
		    }
#line 1131 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 73:
#line 757 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
			if (!parseContext.test_modus)
			{
			    GeneralEvaluation * _arg1 = parseContext._stack.top();
			    parseContext._stack.pop();
			    parseContext._stack.push( new UpperCaseEvaluation(_arg1) );
			}
			parseContext.syntax_ok &= true;
		    }
#line 1145 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 74:
#line 769 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
			if (!parseContext.test_modus)
			{
			    GeneralEvaluation * _arg1 = parseContext._stack.top();
			    parseContext._stack.pop();
			    parseContext._stack.push( new EnvEvaluation(_arg1) );
			}
			parseContext.syntax_ok &= true;
		    }
#line 1159 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 75:
#line 783 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {
		    if (!parseContext.test_modus)
		    {
			std::string uniq_name = parseContext.name_of_metric;			      
			Metric * _met = parseContext.cube->get_met(uniq_name);
			if (_met != NULL)
			{
			    GeneralEvaluation * _property = parseContext._stack.top();
			    parseContext._stack.pop();
			    MetricGetEvaluation * _metric_get_eval = new MetricGetEvaluation( _met, _property );
			    parseContext._stack.push( _metric_get_eval );
			}else
			{
			    cout << " Metric with uniq name " << uniq_name << " doesn't exists in cube" << endl;
			}
		    }
		    parseContext.syntax_ok &= true;
		}
#line 1182 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 78:
#line 808 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
			if (!parseContext.test_modus)
			{
				    GeneralEvaluation * _arg1 = parseContext._stack.top();
				    parseContext._stack.pop();
				    parseContext.function_call.top()->addArgument( _arg1 );
				    parseContext._stack.push( parseContext.function_call.top() );
				    parseContext.function_call.pop();
			}
			parseContext.syntax_ok &= true;
		}
#line 1198 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 79:
#line 823 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {  
		    if (!parseContext.test_modus)
		    {
			    parseContext.function_call.push( new SqrtEvaluation());
		    }
		    parseContext.syntax_ok &= true;
		}
#line 1210 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 80:
#line 831 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {  
		    if (!parseContext.test_modus)
		    {
			    parseContext.function_call.push( new SinEvaluation());
		    }
		    parseContext.syntax_ok &= true;
		}
#line 1222 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 81:
#line 839 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {  
		    if (!parseContext.test_modus)
		    {
			    parseContext.function_call.push( new ASinEvaluation());
		    }
		    parseContext.syntax_ok &= true;
		}
#line 1234 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 82:
#line 847 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {  
		    if (!parseContext.test_modus)
		    {
			    parseContext.function_call.push( new CosEvaluation());
		    }
		    parseContext.syntax_ok &= true;
		}
#line 1246 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 83:
#line 855 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {  
		    if (!parseContext.test_modus)
		    {
			    parseContext.function_call.push( new ACosEvaluation());
		    }
		    parseContext.syntax_ok &= true;
		}
#line 1258 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 84:
#line 863 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {  
		    if (!parseContext.test_modus)
		    {
			    parseContext.function_call.push( new ExpEvaluation());
		    }
		    parseContext.syntax_ok &= true;
		}
#line 1270 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 85:
#line 871 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {  
		    if (!parseContext.test_modus)
		    {
			    parseContext.function_call.push( new LnEvaluation());
		    }
		    parseContext.syntax_ok &= true;
		}
#line 1282 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 86:
#line 879 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {  
		    if (!parseContext.test_modus)
		    {
			    parseContext.function_call.push( new TanEvaluation());
		    }
		    parseContext.syntax_ok &= true;
		}
#line 1294 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 87:
#line 887 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {  
		    if (!parseContext.test_modus)
		    {
			    parseContext.function_call.push( new ATanEvaluation());
		    }
		    parseContext.syntax_ok &= true;
		}
#line 1306 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 88:
#line 895 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {  
		    if (!parseContext.test_modus)
		    {
			    parseContext.function_call.push( new RandomEvaluation());
		    }
		    parseContext.syntax_ok &= true;
		}
#line 1318 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 89:
#line 903 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {  
		    if (!parseContext.test_modus)
		    {
			    parseContext.function_call.push( new AbsEvaluation());
		    }
		    parseContext.syntax_ok &= true;
		}
#line 1330 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 90:
#line 911 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {  
		    if (!parseContext.test_modus)
		    {
			    parseContext.function_call.push( new SgnEvaluation());
		    }
		    parseContext.syntax_ok &= true;
		}
#line 1342 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 91:
#line 919 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {  
		    if (!parseContext.test_modus)
		    {
			    parseContext.function_call.push( new PosEvaluation());
		    }
		    parseContext.syntax_ok &= true;
		}
#line 1354 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 92:
#line 927 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {  
		    if (!parseContext.test_modus)
		    {
			    parseContext.function_call.push( new NegEvaluation());
		    }
		    parseContext.syntax_ok &= true;
		}
#line 1366 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 93:
#line 935 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {  
		    if (!parseContext.test_modus)
		    {
			    parseContext.function_call.push( new FloorEvaluation());
		    }
		    parseContext.syntax_ok &= true;
		}
#line 1378 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 94:
#line 943 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {  
		    if (!parseContext.test_modus)
		    {
			    parseContext.function_call.push( new CeilEvaluation());
		    }
		    parseContext.syntax_ok &= true;
		}
#line 1390 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 95:
#line 954 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
			if (!parseContext.test_modus)
			{
				    GeneralEvaluation * _arg2 = parseContext._stack.top();
				    parseContext._stack.pop();
				    GeneralEvaluation * _arg1 = parseContext._stack.top();
				    parseContext._stack.pop();
				    parseContext.function_call.top()->addArgument( _arg1 );
				    parseContext.function_call.top()->addArgument( _arg2 );
				    parseContext._stack.push( parseContext.function_call.top() );
				    parseContext.function_call.pop();
			}
			parseContext.syntax_ok &= true;
		}
#line 1409 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 96:
#line 970 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {  
		    if (!parseContext.test_modus)
		    {
			    parseContext.function_call.push( new MinEvaluation());
		    }
		    parseContext.syntax_ok &= true;
		}
#line 1421 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 97:
#line 978 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {  
		    if (!parseContext.test_modus)
		    {
			    parseContext.function_call.push( new MaxEvaluation());
		    }
		    parseContext.syntax_ok &= true;
		}
#line 1433 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 98:
#line 989 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {
	       if (!parseContext.test_modus)
	       {
		double _value = atof( parseContext.value.c_str());
		parseContext._stack.push( new ConstantEvaluation(_value));
		parseContext.value = "";
		}
		parseContext.syntax_ok &= true;
	    }
#line 1447 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 102:
#line 1005 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
	    if (!parseContext.test_modus)
	    {
		Metric * _met = parseContext.cube->get_met(parseContext.name_of_metric);
		if (_met != NULL)
		{	    
		    parseContext._stack.push( new DirectMetricEvaluation(cube::CONTEXT_METRIC,  parseContext.cube, _met ));
		    _met->isUsedByOthers( true );
		}
		else
		{
		    cerr << "Cannot connect to the metric " << parseContext.name_of_metric << ". Seems that this metric is not created yet. Value of metric::" << parseContext.name_of_metric << "() will be always 0" << endl;
		    parseContext._stack.push( new ConstantEvaluation( 0 ));
		
		}
		parseContext.value = "";
    	    }
    	    parseContext.syntax_ok &= true;
	}
#line 1471 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 103:
#line 1025 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
	    if (!parseContext.test_modus)
	    {
		Metric * _met = parseContext.cube->get_met(parseContext.name_of_metric);
		if (_met != NULL)
		{	    
		    cube::CalcFlavorModificator * mod = parseContext.calcFlavorModificators.top();
		    parseContext.calcFlavorModificators.pop();
		    parseContext._stack.push( new DirectMetricEvaluation(cube::CONTEXT_METRIC,  parseContext.cube, _met, mod ));
		    _met->isUsedByOthers( true );
		}
		else
		{
		    cerr << "Cannot connect to the metric " << parseContext.name_of_metric << ". Seems that this metric is not created yet. Value of metric::"<<parseContext.name_of_metric << "( ?  ) will be always 0" << endl;
		    parseContext._stack.push( new ConstantEvaluation( 0 ));
		
		}
		parseContext.value = "";
    	    }
    	    parseContext.syntax_ok &= true;
	}
#line 1497 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 104:
#line 1047 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
	    if (!parseContext.test_modus)
	    {
		Metric * _met = parseContext.cube->get_met(parseContext.name_of_metric);
		if (_met != NULL)
		{	    
		    cube::CalcFlavorModificator * mod2 = parseContext.calcFlavorModificators.top();
		    parseContext.calcFlavorModificators.pop();
		    cube::CalcFlavorModificator * mod1 = parseContext.calcFlavorModificators.top();
		    parseContext.calcFlavorModificators.pop();

		    parseContext._stack.push( new DirectMetricEvaluation(cube::CONTEXT_METRIC,  parseContext.cube, _met, mod1, mod2));
		    _met->isUsedByOthers( true );
		}
		else
		{
		    cerr << "Cannot connect to the metric " << parseContext.name_of_metric << ". Seems that this metric is not created yet. Value of metric::"<<parseContext.name_of_metric << "( ?, ? ) will be always 0" << endl;
		    parseContext._stack.push( new ConstantEvaluation( 0 ));
		
		}
		parseContext.value = "";
    	    }
    	    parseContext.syntax_ok &= true;
	}
#line 1526 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 105:
#line 1075 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
	    if (!parseContext.test_modus)
	    {
		Metric * _met = parseContext.cube->get_met(parseContext.name_of_metric);
		if (_met != NULL)
		{	    
		    parseContext._stack.push( new DirectMetricEvaluation(cube::FIXED_METRIC_FULL_AGGR,  parseContext.cube, _met ));
		    _met->isUsedByOthers( true );
		}
		else
		{
		    cerr << "Cannot connect to the metric " << parseContext.name_of_metric << ". Seems that this metric is not created yet. Value of metric::"<<parseContext.name_of_metric << "() will be always 0" << endl;
		    parseContext._stack.push( new ConstantEvaluation( 0 ));
		
		}
		parseContext.value = "";
    	    }
    	    parseContext.syntax_ok &= true;
	}
#line 1550 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 106:
#line 1095 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
	    if (!parseContext.test_modus)
	    {
		Metric * _met = parseContext.cube->get_met(parseContext.name_of_metric);
		if (_met != NULL)
		{	    
		    cube::CalcFlavorModificator * mod = parseContext.calcFlavorModificators.top();
		    parseContext.calcFlavorModificators.pop();
		    parseContext._stack.push( new DirectMetricEvaluation(cube::FIXED_METRIC_AGGR_SYS,  parseContext.cube, _met, mod ));
		    _met->isUsedByOthers( true );

		}
		else
		{
		    cube::CalcFlavorModificator * mod = parseContext.calcFlavorModificators.top();
		    parseContext.calcFlavorModificators.pop();
		    cerr << "Cannot connect to the metric " << parseContext.name_of_metric << ". Seems that this metric is not created yet. Value of metric::fixed::"<<parseContext.name_of_metric << "( "; mod->print(); cout << " ) will be always 0" << endl;
		    

		    delete mod;
		    
		    parseContext._stack.push( new ConstantEvaluation( 0 ));
		
		}
		parseContext.value = "";
    	    }
    	    parseContext.syntax_ok &= true;
	}
#line 1583 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 107:
#line 1124 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
	    if (!parseContext.test_modus)
	    {
		Metric * _met = parseContext.cube->get_met(parseContext.name_of_metric);
		if (_met != NULL)
		{	    
		    cube::CalcFlavorModificator * mod2 = parseContext.calcFlavorModificators.top();
		    parseContext.calcFlavorModificators.pop();
		    cube::CalcFlavorModificator * mod1 = parseContext.calcFlavorModificators.top();
		    parseContext.calcFlavorModificators.pop();

		    parseContext._stack.push( new DirectMetricEvaluation(cube::FIXED_METRIC_NO_AGGR,  parseContext.cube, _met, mod1, mod2));
		    _met->isUsedByOthers( true );
		}
		else
		{
		    cube::CalcFlavorModificator * mod2 = parseContext.calcFlavorModificators.top();
		    parseContext.calcFlavorModificators.pop();
		    cube::CalcFlavorModificator * mod1 = parseContext.calcFlavorModificators.top();
		    parseContext.calcFlavorModificators.pop();
		    cerr << "Cannot connect to the metric " << parseContext.name_of_metric << ". Seems that this metric is not created yet. Value of metric::fixed::"<<parseContext.name_of_metric << "( "; mod1->print(); cout << ","; mod2->print(); cout <<") will be always 0" << endl;

		    
		    delete mod2;
		    delete mod1;
		    
		    parseContext._stack.push( new ConstantEvaluation( 0 ));
		
		}
		parseContext.value = "";
    	    }
    	    parseContext.syntax_ok &= true;
	}
#line 1621 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 108:
#line 1161 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
	    if (!parseContext.test_modus)
	    {
		Metric * _met = parseContext.cube->get_met(parseContext.name_of_metric);
		if (_met != NULL)
		{	    
		    cube::CalcFlavorModificator * mod = parseContext.calcFlavorModificators.top();
		    parseContext.calcFlavorModificators.pop();
		    GeneralEvaluation * _cnode_id = parseContext._stack.top();
		    parseContext._stack.pop();

		    parseContext._stack.push( new DirectMetricEvaluation(cube::METRIC_CALL_CALLPATH,  parseContext.cube, _met, _cnode_id, mod ));
		    _met->isUsedByOthers( true );

		}
		else
		{
		    cube::CalcFlavorModificator * mod = parseContext.calcFlavorModificators.top();
		    parseContext.calcFlavorModificators.pop();

		    GeneralEvaluation * _cnode_id = parseContext._stack.top();
		    parseContext._stack.pop();

		    cerr << "Cannot connect to the metric " << parseContext.name_of_metric << ". Seems that this metric is not created yet. Value of metric::call::"<<parseContext.name_of_metric << "( "; _cnode_id->print(); cout << ","; mod->print(); cout <<") will be always 0" << endl;
		    delete mod;
		    delete _cnode_id;
		    
		    parseContext._stack.push( new ConstantEvaluation( 0 ));
		
		}
		parseContext.value = "";
    	    }
    	    parseContext.syntax_ok &= true;
	}
#line 1660 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 109:
#line 1196 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
	    if (!parseContext.test_modus)
	    {
		Metric * _met = parseContext.cube->get_met(parseContext.name_of_metric);
		if (_met != NULL)
		{	    
		    cube::CalcFlavorModificator * mod2 = parseContext.calcFlavorModificators.top();
		    parseContext.calcFlavorModificators.pop();
		    cube::CalcFlavorModificator * mod1 = parseContext.calcFlavorModificators.top();
		    parseContext.calcFlavorModificators.pop();
		    GeneralEvaluation * _sysres_id = parseContext._stack.top();
		    parseContext._stack.pop();
		    GeneralEvaluation * _cnode_id = parseContext._stack.top();
		    parseContext._stack.pop();
		    parseContext._stack.push( new DirectMetricEvaluation(cube::METRIC_CALL_FULL,  parseContext.cube, _met, _cnode_id, mod1, _sysres_id,  mod2));
		    _met->isUsedByOthers( true );
		}
		else
		{
		    cube::CalcFlavorModificator * mod2 = parseContext.calcFlavorModificators.top();
		    parseContext.calcFlavorModificators.pop();
		    cube::CalcFlavorModificator * mod1 = parseContext.calcFlavorModificators.top();
		    parseContext.calcFlavorModificators.pop();
		    GeneralEvaluation * _sysres_id = parseContext._stack.top();
		    parseContext._stack.pop();
		    GeneralEvaluation * _cnode_id = parseContext._stack.top();
		    parseContext._stack.pop();
		    cerr << "Cannot connect to the metric " << parseContext.name_of_metric << ". Seems that this metric is not created yet. Value of metric::call::"<<parseContext.name_of_metric << "( "; _cnode_id->print(); cout << ","; mod1->print(); cout << ","; _sysres_id->print(); cout << ","; mod2->print(); cout <<") will be always 0" << endl;

		    
		    delete mod2;
		    delete mod1;
		    delete _cnode_id;
		    delete _sysres_id;
		    
		    parseContext._stack.push( new ConstantEvaluation( 0 ));
		
		}
		parseContext.value = "";
    	    }
    	    parseContext.syntax_ok &= true;
	}
#line 1707 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 110:
#line 1243 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
				    if (!parseContext.test_modus)
				    {
					parseContext.calcFlavorModificators.push(new CalcFlavorModificatorIncl());
				    }
				    parseContext.syntax_ok &= true;
				}
#line 1719 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 111:
#line 1251 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
				    if (!parseContext.test_modus)
				    {
					parseContext.calcFlavorModificators.push(new CalcFlavorModificatorExcl());
				    }
				    parseContext.syntax_ok &= true;
				}
#line 1731 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 112:
#line 1259 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
				    if (!parseContext.test_modus)
				    {
					parseContext.calcFlavorModificators.push(new CalcFlavorModificatorSame());
				    }
				    parseContext.syntax_ok &= true;
				}
#line 1743 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 113:
#line 1270 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{

		    LambdaCalculEvaluation * _l_calc = new LambdaCalculEvaluation();

		    GeneralEvaluation * return_expr = parseContext._stack.top();
		    parseContext._stack.pop();

		    int _num_of_statements = parseContext._number_of_statements.top();
		    parseContext._number_of_statements.pop();

		    vector<GeneralEvaluation*> _tmp_statements;
		    for (int i=0; i<_num_of_statements; i++)
		    {
			_tmp_statements.push_back( parseContext._statements.top());
			parseContext._statements.pop();
		    }
		    for (int i=_num_of_statements-1; i>=0; i--)
		    {
			_l_calc->addArgument( _tmp_statements[i]);
		    }
		    _l_calc->addArgument( return_expr);
		    parseContext._stack.push(_l_calc);
		    // create lambda calculation using number_of_statements.
		}
		parseContext.syntax_ok &= true;
	    }
#line 1776 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 115:
#line 1302 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    parseContext._number_of_statements.push(0);
		}
		parseContext.syntax_ok &= true;
	    }
#line 1788 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 118:
#line 1319 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    int _tmp = parseContext._number_of_statements.top();
		    parseContext._number_of_statements.pop();
		    _tmp++;
		    parseContext._number_of_statements.push(_tmp);
		}
		parseContext.syntax_ok &= true;
	    }
#line 1803 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 119:
#line 1330 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    int _tmp = parseContext._number_of_statements.top();
		    parseContext._number_of_statements.pop();
		    _tmp++;
		    parseContext._number_of_statements.push(_tmp);
		}
		parseContext.syntax_ok &= true;
	    }
#line 1818 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 120:
#line 1341 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
            if (!parseContext.test_modus)
            {
                int _tmp = parseContext._number_of_statements.top();
                parseContext._number_of_statements.pop();
                _tmp++;
                parseContext._number_of_statements.push(_tmp);
            }
		parseContext.syntax_ok &= true;
	    }
#line 1833 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 121:
#line 1352 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {
		parseContext.syntax_ok &= true;
	    }
#line 1841 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 122:
#line 1356 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {
		parseContext.syntax_ok &= true;
	    }
#line 1849 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 123:
#line 1360 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {
            if (!parseContext.test_modus)
            {
                int _tmp = parseContext._number_of_statements.top();
                parseContext._number_of_statements.pop();
                _tmp++;
                parseContext._number_of_statements.push(_tmp);
            }
		parseContext.syntax_ok &= true;
	    }
#line 1864 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 124:
#line 1371 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {
		parseContext.syntax_ok &= true;
	    }
#line 1872 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 125:
#line 1375 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {
		parseContext.syntax_ok &= true;
	    }
#line 1880 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 126:
#line 1379 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {
		parseContext.syntax_ok &= true;
	    }
#line 1888 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 127:
#line 1383 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {
		parseContext.syntax_ok &= true;
	    }
#line 1896 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 128:
#line 1389 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {
	    if (!parseContext.test_modus)
	    {
		std::string uniq_name = parseContext.name_of_metric;			      
		Metric * _met = parseContext.cube->get_met(uniq_name);
		GeneralEvaluation * _value = parseContext._stack.top();
		parseContext._stack.pop();
		GeneralEvaluation * _property = parseContext._stack.top();
		parseContext._stack.pop();
		MetricSetEvaluation * _metric_set_eval = new MetricSetEvaluation( _met, _property , _value);
		parseContext._statements.push( _metric_set_eval );
		if (_met == NULL)
		{
		    cout << " Metric with uniq name " << uniq_name << " doesn't exists in cube. metric::set has no action." << endl;
		}
	    }
	    parseContext.syntax_ok &= true;
}
#line 1919 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 133:
#line 1420 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    int _num_of_statements = parseContext._number_of_statements.top();
		    parseContext._number_of_statements.pop();

		    vector<GeneralEvaluation*> _tmp_statements;
		    for (int i=0; i<_num_of_statements; i++)
		    {
			_tmp_statements.push_back( parseContext._statements.top());
			parseContext._statements.pop();
		    }
		    GeneralEvaluation * _condition = parseContext._stack.top();
		    parseContext._stack.pop();

		    ShortIfEvaluation * _s_if_calc = new ShortIfEvaluation(_condition);

		    for (int i=_num_of_statements-1; i>=0; i--)
		    {
			_s_if_calc->addArgument( _tmp_statements[i]);
		    }
		    parseContext._statements.push(_s_if_calc);
		}
		parseContext.syntax_ok &= true;
	    }
#line 1949 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 134:
#line 1447 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    int _num_of_false_statements = parseContext._number_of_statements.top();
		    parseContext._number_of_statements.pop();
		    int _num_of_true_statements = parseContext._number_of_statements.top();
		    parseContext._number_of_statements.pop();

		    vector<GeneralEvaluation*> _tmp_false_statements;
		    for (int i=0; i<_num_of_false_statements; i++)
		    {
			_tmp_false_statements.push_back( parseContext._statements.top());
			parseContext._statements.pop();
		    }
		    vector<GeneralEvaluation*> _tmp_true_statements;
		    for (int i=0; i<_num_of_true_statements; i++)
		    {
			_tmp_true_statements.push_back( parseContext._statements.top());
			parseContext._statements.pop();
		    }
		    GeneralEvaluation * _condition = parseContext._stack.top();
		    parseContext._stack.pop();

		    FullIfEvaluation * _f_if_calc = new FullIfEvaluation(_condition, _num_of_true_statements, _num_of_false_statements);

		    for (int i=_num_of_true_statements-1; i>=0; i--)
		    {
			_f_if_calc->addArgument( _tmp_true_statements[i]);
		    }
		    for (int i=_num_of_false_statements-1; i>=0; i--)
		    {
			_f_if_calc->addArgument( _tmp_false_statements[i]);
		    }
		    parseContext._statements.push(_f_if_calc);
		}
		parseContext.syntax_ok &= true;
	    }
#line 1991 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 135:
#line 1486 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    std::stack<std::stack<GeneralEvaluation*> > statement_blocks;
		    std::stack<GeneralEvaluation*>  statement_conditions;
 
		    int _num_of_false_statements = parseContext._number_of_statements.top();
		    parseContext._number_of_statements.pop();
		    std::stack<GeneralEvaluation*> _tmp_false_statements;
		    for (int i=0; i<_num_of_false_statements; i++)
		    {
			_tmp_false_statements.push( parseContext._statements.top());
			parseContext._statements.pop();
		    }
		    statement_blocks.push( _tmp_false_statements);

		    for (unsigned i=0; i<parseContext.number_elseif + 1 /* +1 is for the initial condition*/; ++i)
		    {
			int _num_of_statements = parseContext._number_of_statements.top();
			parseContext._number_of_statements.pop();
			std::stack<GeneralEvaluation*> _tmp_statements;
			for (int j=0; j<_num_of_statements; j++)
			{
			    _tmp_statements.push( parseContext._statements.top());
			    parseContext._statements.pop();
			}

			GeneralEvaluation * _condition = parseContext._stack.top();
			parseContext._stack.pop();
			statement_blocks.push( _tmp_statements);
			statement_conditions.push( _condition );
		    }
		    ExtendedIfEvaluation * _ext_if_calc = new ExtendedIfEvaluation(statement_conditions, statement_blocks);
		    parseContext._statements.push(_ext_if_calc);

		    parseContext.number_elseif= 0;
		}
		parseContext.syntax_ok &= true;
	    }
#line 2035 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 136:
#line 1529 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    std::stack<std::stack<GeneralEvaluation*> > statement_blocks;
		    std::stack<GeneralEvaluation*>  statement_conditions;

		    for (unsigned i=0; i<parseContext.number_elseif + 1 /* +1 is for the initial condition*/; ++i)
		    {
			int _num_of_statements = parseContext._number_of_statements.top();
			parseContext._number_of_statements.pop();
			std::stack<GeneralEvaluation*> _tmp_statements;
			for (int j=0; j<_num_of_statements; j++)
			{
			    _tmp_statements.push( parseContext._statements.top());
			    parseContext._statements.pop();
			}

			GeneralEvaluation * _condition = parseContext._stack.top();
			parseContext._stack.pop();
			statement_blocks.push( _tmp_statements);
			statement_conditions.push( _condition );
		    }
		    ExtendedIfEvaluation * _ext_if_calc = new ExtendedIfEvaluation(statement_conditions, statement_blocks);
		    parseContext._statements.push(_ext_if_calc);

		    parseContext.number_elseif= 0;
		}
		parseContext.syntax_ok &= true;
	    }
#line 2069 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 139:
#line 1566 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    parseContext.number_elseif++;
		}
		parseContext.syntax_ok &= true;
	    }
#line 2081 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 143:
#line 1585 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    parseContext._number_of_statements.push(0);
		}
		parseContext.syntax_ok &= true;
	    }
#line 2093 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 144:
#line 1595 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    int _num_of_statements = parseContext._number_of_statements.top();
		    parseContext._number_of_statements.pop();

		    vector<GeneralEvaluation*> _tmp_statements;
		    for (int i=0; i<_num_of_statements; i++)
		    {
			_tmp_statements.push_back( parseContext._statements.top());
			parseContext._statements.pop();
		    }
		    GeneralEvaluation * _condition = parseContext._stack.top();
		    parseContext._stack.pop();

		    WhileEvaluation * _while_calc = new WhileEvaluation(_condition);

		    for (int i=_num_of_statements-1; i>=0; i--)
		    {
			_while_calc->addArgument( _tmp_statements[i]);
		    }
		    parseContext._statements.push(_while_calc);

		}
		parseContext.syntax_ok &= true;
	    }
#line 2124 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 145:
#line 1624 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
			GeneralEvaluation * _value = parseContext._stack.top();
			parseContext._stack.pop();
			GeneralEvaluation * _index = parseContext._stack.top();
			parseContext._stack.pop();
			std::string _string = parseContext.string_constants.top();
			parseContext.string_constants.pop();
			AssignmentEvaluation * _assign_eval = new AssignmentEvaluation( _string , _index, _value, parseContext.cube->get_cubepl_memory_manager()  );
			parseContext._statements.push( _assign_eval );
		}
		parseContext.syntax_ok &= true;
	    }
#line 2143 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 148:
#line 1644 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {
		if (!parseContext.test_modus)
		{      
		    parseContext._stack.push( new ConstantEvaluation(0) ); // pushed index 0 if array index is not specified
		}
		parseContext.syntax_ok &= true;
	      }
#line 2155 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 153:
#line 1664 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {
			    if (!parseContext.test_modus)
			    {
			      std::string expression = parseContext.str.str();
			      parseContext.str.str("");
			      std::string uniq_name = parseContext.name_of_metric;			      
			      Metric * _met = parseContext.cube->get_met(uniq_name);
			      if (_met == NULL)
			      {
				_met = parseContext.cube->def_met("",
								      uniq_name,
								     "DOUBLE",
								      "",
								      "",
								      "",
								      "",
								      NULL,
								      cube::CUBE_METRIC_POSTDERIVED,
								      expression,
								      "",
								      "",
								      "",
								      "",
								      true,
								      cube::CUBE_METRIC_GHOST
								  );	
			      }else
			      {
				cout << " Metric with uniq name " << uniq_name << " already exists in cube" << endl;
			      }
			   }else
			  {  
			      std::string expression = parseContext.str.str();
			      std::string                  cubepl_program = string( "<cubepl>" ) + expression + string( "</cubepl>" );
			      cubeplparser::CubePL1Driver* driver         = new cubeplparser::CubePL1Driver( parseContext.cube ); // create driver for this cube
			      std::string nested_error;
			      if (! driver->test( cubepl_program, nested_error ))
			      {
				parseContext.syntax_ok &= false;
			      }
			      delete driver;
			    }
			    parseContext.syntax_ok &= true;
			    
		    }
#line 2205 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 154:
#line 1711 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {
			      if (!parseContext.test_modus)
			      {
				std::string expression = parseContext.str.str();
				parseContext.str.str("");
				std::string uniq_name = parseContext.name_of_metric;			      
				Metric * _met = parseContext.cube->get_met(uniq_name);
				if (_met == NULL)
				{
				  _met = parseContext.cube->def_met("",
								      uniq_name,
								     "DOUBLE",
								      "",
								      "",
								      "",
								      "",
								      NULL,
								      cube::CUBE_METRIC_PREDERIVED_INCLUSIVE,
								      expression,
								      "",
								      "",
								      "",
								      "",
								      true, 
								      cube::CUBE_METRIC_GHOST
								    );	
				}else
				{
				  cout << " Metric with uniq name " << uniq_name << " already exists in cube" << endl;
				}
			    }else
			    {  
				std::string expression = parseContext.str.str();
				std::string                  cubepl_program = string( "<cubepl>" ) + expression + string( "</cubepl>" );
				cubeplparser::CubePL1Driver* driver         = new cubeplparser::CubePL1Driver( parseContext.cube ); // create driver for this cube
				std::string nested_error;
				if (! driver->test( cubepl_program, nested_error ))
				{
				  parseContext.syntax_ok = false;
				}
				delete driver;
			      }
			      parseContext.syntax_ok &= true;
			    }
#line 2254 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 155:
#line 1757 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {
				if (!parseContext.test_modus)
				{
				  std::string expression = parseContext.str.str();
				  parseContext.str.str("");
				  std::string uniq_name = parseContext.name_of_metric;			      
				  Metric * _met = parseContext.cube->get_met(uniq_name);

				  if (_met == NULL)
				  {
				    _met = parseContext.cube->def_met("",
									uniq_name,
								      "DOUBLE",
									"",
									"",
									"",
									"",
									NULL,
									cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
									expression,
									"",
									"",
									"",
								      "",
									true,
									cube::CUBE_METRIC_GHOST
								      );	
				  }else
				  {
				    cout << " Metric with uniq name " << uniq_name << " already exists in cube" << endl;
				  }
			      }else
			      {  
				  std::string expression = parseContext.str.str();
				  std::string                  cubepl_program = string( "<cubepl>" ) + expression + string( "</cubepl>" );
				  cubeplparser::CubePL1Driver* driver         = new cubeplparser::CubePL1Driver( parseContext.cube ); // create driver for this cube
				  std::string nested_error;
				  if (! driver->test( cubepl_program, nested_error ))
				  {
				    parseContext.syntax_ok = false;
				  }
				  delete driver;
			      }
			      parseContext.syntax_ok &= true;
			    }
#line 2304 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 158:
#line 1814 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {
                if (!parseContext.test_modus)
                {
                  std::string expression = parseContext.str.str();
                  parseContext.str.str("");
                  std::string uniq_name = parseContext.name_of_metric;                 
                  Metric * _met = parseContext.cube->get_met(uniq_name);

                    if (_met == NULL)
                    {
                        cout << " Metric with uniq name " << uniq_name << " doesn't  exists yet. Cannot assign CubePL initialization expression. Skip." << endl;
                    }else 
                    {
                        _met->set_init_expression( expression );
                    }
                  }else
                  {  
                  std::string expression = parseContext.str.str();
                  std::string                  cubepl_program = string( "<cubepl>" ) + expression + string( "</cubepl>" );
                  cubeplparser::CubePL1Driver* driver         = new cubeplparser::CubePL1Driver( parseContext.cube ); // create driver for this cube
                  std::string nested_error;
                  if (! driver->test( cubepl_program, nested_error ))
                  {
                    parseContext.syntax_ok = false;
                  }
                  delete driver;
                  }
                  parseContext.syntax_ok &= true;
                }
#line 2338 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 159:
#line 1846 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {
                if (!parseContext.test_modus)
                {
                  std::string expression = parseContext.str.str();
                  parseContext.str.str("");
                  std::string uniq_name = parseContext.name_of_metric;                 
                  Metric * _met = parseContext.cube->get_met(uniq_name);

                    if (_met == NULL)
                    {
                        cout << " Metric with uniq name " << uniq_name << " doesn't  exists yet. Cannot assign CubePL initialization expression. Skip." << endl;
                    }else 
                    {
                        _met->set_aggr_plus_expression( expression );
                    }
                  }else
                  {  
                  std::string expression = parseContext.str.str();
                  std::string                  cubepl_program = string( "<cubepl>" ) + expression + string( "</cubepl>" );
                  cubeplparser::CubePL1Driver* driver         = new cubeplparser::CubePL1Driver( parseContext.cube ); // create driver for this cube
                  std::string nested_error;
                  if (! driver->test( cubepl_program, nested_error ))
                  {
                    parseContext.syntax_ok = false;
                  }
                  delete driver;
                  }
                  parseContext.syntax_ok &= true;
                }
#line 2372 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 160:
#line 1878 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {
                if (!parseContext.test_modus)
                {
                  std::string expression = parseContext.str.str();
                  parseContext.str.str("");
                  std::string uniq_name = parseContext.name_of_metric;                 
                  Metric * _met = parseContext.cube->get_met(uniq_name);

                    if (_met == NULL)
                    {
                        cout << " Metric with uniq name " << uniq_name << " doesn't  exists yet. Cannot assign CubePL initialization expression. Skip." << endl;
                    }else 
                    {
                        _met->set_aggr_minus_expression( expression );
                    }
                  }else
                  {  
                  std::string expression = parseContext.str.str();
                  std::string                  cubepl_program = string( "<cubepl>" ) + expression + string( "</cubepl>" );
                  cubeplparser::CubePL1Driver* driver         = new cubeplparser::CubePL1Driver( parseContext.cube ); // create driver for this cube
                  std::string nested_error;
                  if (! driver->test( cubepl_program, nested_error ))
                  {
                    parseContext.syntax_ok = false;
                  }
                  delete driver;
                  }
                  parseContext.syntax_ok &= true;
                }
#line 2406 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 161:
#line 1912 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		    if (!parseContext.test_modus)
		    {
			    GeneralEvaluation * _index = parseContext._stack.top();
			    parseContext._stack.pop();
			    std::string _string = parseContext.string_constants.top();
			    parseContext.string_constants.pop();
			    VariableEvaluation * _var_eval = new VariableEvaluation(  _string, _index, parseContext.cube->get_cubepl_memory_manager()   );
			    parseContext._stack.push( _var_eval );
		    }
		    parseContext.syntax_ok &= true;
		}
#line 2423 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 162:
#line 1925 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		  if (!parseContext.test_modus)
		  {
			  GeneralEvaluation * _index = parseContext._stack.top();
			  parseContext._stack.pop();
			  std::string _string = parseContext.string_constants.top();
			  parseContext.string_constants.pop();
			  VariableEvaluation * _var_eval = new VariableEvaluation(  _string, _index, parseContext.cube->get_cubepl_memory_manager()  );
			  parseContext._stack.push( _var_eval );
		  }
		  parseContext.syntax_ok &= true;
	      }
#line 2440 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 163:
#line 1940 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    { 
		if (!parseContext.test_modus)
		{
		    parseContext._stack.push( new ConstantEvaluation(0) ); // pushed index 0 if array index is not specified
		}
		parseContext.syntax_ok &= true;
	    }
#line 2452 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 165:
#line 1953 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {
        if (!parseContext.test_modus)
        {
            std::string _string = parseContext.string_constants.top();
            parseContext.string_constants.pop();
            SizeOfVariableEvaluation * _sizeof_var_eval = new SizeOfVariableEvaluation(  _string, parseContext.cube->get_cubepl_memory_manager()  );
            parseContext._stack.push( _sizeof_var_eval );
        }
        parseContext.syntax_ok &= true;            
	    }
#line 2467 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 166:
#line 1965 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {
        if (!parseContext.test_modus)
        {
            std::string _string = parseContext.string_constants.top();
            parseContext.string_constants.pop();
            DefinedVariableEvaluation * _sizeof_var_eval = new DefinedVariableEvaluation(  _string, parseContext.cube->get_cubepl_memory_manager()  );
            parseContext._stack.push( _sizeof_var_eval );
        }
	    }
#line 2481 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 167:
#line 1975 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {
        if (!parseContext.test_modus)
        {        
            std::string _string = parseContext.string_constants.top();
           parseContext.cube->get_cubepl_memory_manager()->register_variable( _string , CUBEPL_VARIABLE);
        }
        parseContext.syntax_ok &= true;              
        }
#line 2494 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 168:
#line 1984 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {
        if (!parseContext.test_modus)
        {        
            std::string _string = parseContext.string_constants.top();
            parseContext.cube->get_cubepl_memory_manager()->register_variable( _string , CUBEPL_GLOBAL_VARIABLE);
        }
        parseContext.syntax_ok &= true;              
        }
#line 2507 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;

  case 169:
#line 1994 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:859
    {
	    if (!parseContext.test_modus)
	    {  
		parseContext.string_constants.push(parseContext.value);
		parseContext.value = "";
	    }
	    parseContext.syntax_ok &= true; 
	}
#line 2520 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
    break;


#line 2524 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:859
            default:
              break;
            }
        }
      catch (const syntax_error& yyexc)
        {
          error (yyexc);
          YYERROR;
        }
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, yylhs);
    }
    goto yynewstate;

  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yyla.location, yysyntax_error_ (yystack_[0].state, yyla));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;
    yyerror_range[1].location = yystack_[yylen - 1].location;
    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yyerror_range[1].location = yystack_[0].location;
          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", error_token);
    }
    goto yynewstate;

    // Accept.
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    // Abort.
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  void
  CubePL1Parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what());
  }

  // Generate an error message.
  std::string
  CubePL1Parser::yysyntax_error_ (state_type yystate, const symbol_type& yyla) const
  {
    // Number of reported tokens (one for the "unexpected", one per
    // "expected").
    size_t yycount = 0;
    // Its maximum.
    enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
    // Arguments of yyformat.
    char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];

    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state
         merging (from LALR or IELR) and default reductions corrupt the
         expected token list.  However, the list is correct for
         canonical LR with one exception: it will still contain any
         token that will not be accepted due to an error action in a
         later state.
    */
    if (!yyla.empty ())
      {
        int yytoken = yyla.type_get ();
        yyarg[yycount++] = yytname_[yytoken];
        int yyn = yypact_[yystate];
        if (!yy_pact_value_is_default_ (yyn))
          {
            /* Start YYX at -YYN if negative to avoid negative indexes in
               YYCHECK.  In other words, skip the first -YYN actions for
               this state because they are default actions.  */
            int yyxbegin = yyn < 0 ? -yyn : 0;
            // Stay within bounds of both yycheck and yytname.
            int yychecklim = yylast_ - yyn + 1;
            int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
            for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
              if (yycheck_[yyx + yyn] == yyx && yyx != yyterror_
                  && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
                {
                  if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                    {
                      yycount = 1;
                      break;
                    }
                  else
                    yyarg[yycount++] = yytname_[yyx];
                }
          }
      }

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
        YYCASE_(0, YY_("syntax error"));
        YYCASE_(1, YY_("syntax error, unexpected %s"));
        YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    size_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += yytnamerr_ (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const short int CubePL1Parser::yypact_ninf_ = -222;

  const signed char CubePL1Parser::yytable_ninf_ = -1;

  const short int
  CubePL1Parser::yypact_[] =
  {
      18,   275,     7,  -222,   417,   417,  -222,    22,    49,   417,
    -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,
    -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,
       9,   -41,   -23,   -14,    60,    78,    96,    48,  -222,  -222,
     266,  -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,
    -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,
     123,  -222,   124,  -222,  -222,  -222,  -222,  -222,  -222,   117,
    -222,  -222,  -222,  -222,  -222,  -222,     5,   209,    62,    62,
      21,    62,   134,   135,   136,   417,   417,   417,   141,  -222,
     417,   417,   417,   417,   417,   417,   417,   142,   149,   150,
     153,   152,    81,    90,    91,    93,    99,    92,   168,   169,
    -222,  -222,  -222,  -222,  -222,  -222,   175,  -222,  -222,   154,
    -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,
    -222,  -222,  -222,   190,   199,  -222,   187,    -5,    24,   417,
     216,   224,   244,   131,     5,     5,   202,   202,  -222,   251,
      31,    62,    62,   346,   346,    62,   196,   267,   196,   196,
     196,   268,   417,   211,   117,  -222,     4,   117,   417,  -222,
    -222,   222,  -222,  -222,  -222,  -222,    13,  -222,    14,    47,
    -222,  -222,  -222,   243,  -222,   417,   250,   257,   346,   270,
    -222,  -222,    66,    76,  -222,  -222,  -222,  -222,  -222,  -222,
    -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,
      88,   261,   201,  -222,   -28,  -222,  -222,  -222,   131,   163,
    -222,  -222,   272,   175,   237,   240,  -222,   276,   231,   417,
    -222,   -39,  -222,   -39,   -39,  -222,   258,  -222,  -222,    11,
      94,   346,   417,   417,   417,   417,   417,   417,   417,   417,
     198,  -222,   346,   346,   346,   175,   278,  -222,  -222,   281,
     282,   229,  -222,   346,  -222,  -222,  -222,  -222,   191,   287,
     309,    32,  -222,  -222,    97,   231,   231,   231,   231,   231,
     231,   231,   231,  -222,  -222,  -222,  -222,  -222,  -222,   417,
     196,   196,   131,   119,  -222,  -222,  -222,  -222,   417,  -222,
     200,  -222,  -222,   310,   175,   120,  -222,  -222,  -222,   -39,
     311,  -222
  };

  const unsigned char
  CubePL1Parser::yydefact_[] =
  {
       0,     0,     0,     3,     0,     0,   115,     0,     0,     0,
      33,    34,    79,    80,    81,    82,    83,    86,    87,    84,
      85,    89,    88,    90,    91,    92,    93,    94,    96,    97,
       0,     0,     0,     0,     0,     0,     0,     0,    98,    68,
       0,     4,     5,     6,     7,     8,     9,    10,    11,    18,
      31,    32,    20,    21,    69,    70,    71,    22,    12,    76,
       0,    77,     0,    13,    14,    99,   100,   101,    15,   116,
      19,   161,   162,    16,    17,     1,    30,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     2,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     123,   118,   129,   130,   132,   131,     0,   119,   120,     0,
     146,   147,   124,   150,   151,   152,   125,   126,   127,   121,
     122,    23,   169,     0,     0,    24,     0,     0,     0,     0,
       0,     0,     0,     0,    25,    26,    28,    27,    29,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   116,   143,   133,   116,     0,   165,
     166,   163,   102,   110,   111,   112,     0,   105,     0,     0,
      74,    72,    73,     0,    78,     0,     0,     0,     0,     0,
      53,    54,     0,     0,    35,    42,    43,    44,    45,    46,
      36,    37,    38,    39,    40,    41,    47,    61,    62,    63,
       0,     0,     0,   153,     0,   158,   159,   160,     0,     0,
     113,   117,     0,     0,   136,   137,   134,     0,   145,     0,
     103,     0,   106,     0,     0,    75,     0,   167,   168,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   140,     0,     0,     0,     0,   148,   157,   156,     0,
       0,     0,   114,     0,   141,   135,   138,   142,     0,     0,
       0,     0,    95,    48,     0,    57,    55,    64,    65,    59,
      60,    56,    58,    67,    66,    50,    51,    52,   144,     0,
       0,     0,     0,     0,   164,   104,   107,   108,     0,    49,
       0,   154,   155,     0,     0,     0,   149,   128,   139,     0,
       0,   109
  };

  const short int
  CubePL1Parser::yypgoto_[] =
  {
    -222,  -222,    -1,  -222,  -222,  -222,  -222,  -222,  -222,  -222,
    -222,  -222,  -222,  -222,  -139,  -222,  -222,  -222,  -222,  -222,
    -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,
    -222,  -222,  -142,  -222,  -222,  -222,  -222,  -222,  -222,  -222,
    -222,  -222,  -222,  -222,  -222,  -222,  -222,  -222,  -133,  -222,
    -222,  -222,  -123,  -222,  -222,  -222,  -222,  -222,  -222,  -222,
      98,  -222,  -222,   100,  -221,  -222,  -222,  -222,  -222,  -222,
    -222,  -222,  -222,  -222,  -222,  -149,  -222,  -222,  -222,  -222,
    -222,  -222,  -222,  -222,  -222,  -222,  -222,   -48
  };

  const short int
  CubePL1Parser::yydefgoto_[] =
  {
      -1,     2,   192,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,   193,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   284,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,   176,    68,
     163,    69,   108,   109,   110,   111,   112,   113,   114,   115,
     224,   225,   116,   226,   166,   167,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   213,   258,   126,   127,   128,
      70,    71,    72,    73,    74,   129,   130,   133
  };

  const unsigned short int
  CubePL1Parser::yytable_[] =
  {
      40,   183,   264,    76,    77,   178,   172,    75,    80,   215,
     216,   217,    92,    93,    94,   210,    90,    91,    92,    93,
      94,     1,   131,    81,   230,   232,    90,    91,    92,    93,
      94,   134,    78,   136,   288,   177,    90,    91,    92,    93,
      94,   221,    82,   297,   227,   135,   173,   174,   175,   240,
     222,   223,    90,    91,    92,    93,    94,   259,   260,    79,
      83,   242,   243,   244,   245,   246,   247,   248,   249,    84,
      85,    90,    91,    92,    93,    94,   261,   250,   231,   233,
     173,   174,   175,   308,   140,   141,   142,   251,    86,   144,
     145,   146,   147,   148,   149,   150,   185,   298,   269,   255,
     270,   271,   274,   186,   187,   273,    87,   211,   299,   173,
     174,   175,   234,   285,   286,   287,   242,   243,   244,   245,
     246,   247,   248,   249,   293,    90,    91,    92,    93,    94,
     304,    88,   250,    95,    96,   252,   253,   254,   179,    97,
      98,   301,   302,   132,   137,   138,   139,   252,   253,   254,
     303,   143,   151,   252,   253,   254,   252,   253,   254,   152,
     153,   219,    99,   154,   156,   100,   155,   228,    90,    91,
      92,    93,    94,   157,   158,   161,   310,   159,   252,   253,
     254,   101,   262,   160,   236,   309,   162,   239,   164,   165,
     102,   103,   104,   105,   106,   107,    90,    91,    92,    93,
      94,   169,   171,   168,   294,    90,    91,    92,    93,    94,
     170,    94,   212,   306,    90,    91,    92,    93,    94,    39,
     131,    90,    91,    92,    93,    94,   220,   180,   268,    90,
      91,    92,    93,    94,   229,   181,    90,    91,    92,    93,
      94,   275,   276,   277,   278,   279,   280,   281,   282,    90,
      91,    92,    93,    94,   235,   182,    90,    91,    92,    93,
      94,   237,   184,    90,    91,    92,    93,    94,   238,   272,
      89,    90,    91,    92,    93,    94,   256,   214,   218,     3,
     241,     4,   263,   257,   223,     5,   222,   283,   300,     6,
     289,   267,   290,   291,   292,     7,     8,   305,   295,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
     296,   307,   311,   266,   265,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    30,
       0,     0,    31,    32,    33,    34,    35,    36,     0,     0,
       0,     0,     4,     0,    37,    38,   188,     0,     0,     0,
       6,     0,     0,    39,     0,     0,     7,     8,     0,     0,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   189,     0,     0,     0,   190,   191,
      30,     0,     0,    31,    32,    33,    34,    35,    36,     0,
       0,     0,     0,     4,     0,    37,    38,     5,     0,     0,
       0,     6,     0,     0,    39,     0,     0,     7,     8,     0,
       0,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    30,     0,     0,    31,    32,    33,    34,    35,    36,
       0,     0,     0,     0,     0,     0,    37,    38,     0,     0,
       0,     0,     0,     0,     0,    39
  };

  const short int
  CubePL1Parser::yycheck_[] =
  {
       1,   143,   223,     4,     5,   138,    11,     0,     9,   158,
     159,   160,     7,     8,     9,   154,     5,     6,     7,     8,
       9,     3,    11,    14,    11,    11,     5,     6,     7,     8,
       9,    79,    10,    81,   255,    11,     5,     6,     7,     8,
       9,   164,    83,    11,   167,    24,    85,    86,    87,   188,
      46,    47,     5,     6,     7,     8,     9,    85,    86,    10,
      83,    50,    51,    52,    53,    54,    55,    56,    57,    83,
      10,     5,     6,     7,     8,     9,   218,    66,    65,    65,
      85,    86,    87,   304,    85,    86,    87,    11,    10,    90,
      91,    92,    93,    94,    95,    96,    65,    65,   231,    11,
     233,   234,   241,   151,   152,    11,    10,   155,    11,    85,
      86,    87,    65,   252,   253,   254,    50,    51,    52,    53,
      54,    55,    56,    57,   263,     5,     6,     7,     8,     9,
      11,    83,    66,    10,    10,    59,    60,    61,   139,    22,
      23,   290,   291,    81,    10,    10,    10,    59,    60,    61,
     292,    10,    10,    59,    60,    61,    59,    60,    61,    10,
      10,   162,    45,    10,    83,    48,    14,   168,     5,     6,
       7,     8,     9,    83,    83,    83,   309,    84,    59,    60,
      61,    64,    19,    84,   185,    65,    18,   188,    19,    14,
      73,    74,    75,    76,    77,    78,     5,     6,     7,     8,
       9,    11,    15,    49,    13,     5,     6,     7,     8,     9,
      11,     9,    16,    13,     5,     6,     7,     8,     9,    88,
      11,     5,     6,     7,     8,     9,    15,    11,   229,     5,
       6,     7,     8,     9,    12,    11,     5,     6,     7,     8,
       9,   242,   243,   244,   245,   246,   247,   248,   249,     5,
       6,     7,     8,     9,    11,    11,     5,     6,     7,     8,
       9,    11,    11,     5,     6,     7,     8,     9,    11,    11,
       4,     5,     6,     7,     8,     9,    15,    10,    10,     4,
      10,     6,    10,    82,    47,    10,    46,    89,   289,    14,
      12,    15,    11,    11,    65,    20,    21,   298,    11,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      11,    11,    11,   225,   224,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    64,
      -1,    -1,    67,    68,    69,    70,    71,    72,    -1,    -1,
      -1,    -1,     6,    -1,    79,    80,    10,    -1,    -1,    -1,
      14,    -1,    -1,    88,    -1,    -1,    20,    21,    -1,    -1,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    58,    -1,    -1,    -1,    62,    63,
      64,    -1,    -1,    67,    68,    69,    70,    71,    72,    -1,
      -1,    -1,    -1,     6,    -1,    79,    80,    10,    -1,    -1,
      -1,    14,    -1,    -1,    88,    -1,    -1,    20,    21,    -1,
      -1,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    64,    -1,    -1,    67,    68,    69,    70,    71,    72,
      -1,    -1,    -1,    -1,    -1,    -1,    79,    80,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    88
  };

  const unsigned char
  CubePL1Parser::yystos_[] =
  {
       0,     3,    93,     4,     6,    10,    14,    20,    21,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      64,    67,    68,    69,    70,    71,    72,    79,    80,    88,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   141,   143,
     172,   173,   174,   175,   176,     0,    94,    94,    10,    10,
      94,    14,    83,    83,    83,    10,    10,    10,    83,     4,
       5,     6,     7,     8,     9,    10,    10,    22,    23,    45,
      48,    64,    73,    74,    75,    76,    77,    78,   144,   145,
     146,   147,   148,   149,   150,   151,   154,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   169,   170,   171,   177,
     178,    11,    81,   179,   179,    24,   179,    10,    10,    10,
      94,    94,    94,    10,    94,    94,    94,    94,    94,    94,
      94,    10,    10,    10,    10,    14,    83,    83,    83,    84,
      84,    83,    18,   142,    19,    14,   156,   157,    49,    11,
      11,    15,    11,    85,    86,    87,   140,    11,   140,    94,
      11,    11,    11,   124,    11,    65,   179,   179,    10,    58,
      62,    63,    94,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     106,   179,    16,   167,    10,   167,   167,   167,    10,    94,
      15,   144,    46,    47,   152,   153,   155,   144,    94,    12,
      11,    65,    11,    65,    65,    11,    94,    11,    11,    94,
     106,    10,    50,    51,    52,    53,    54,    55,    56,    57,
      66,    11,    59,    60,    61,    11,    15,    82,   168,    85,
      86,   124,    19,    10,   156,   155,   152,    15,    94,   140,
     140,   140,    11,    11,   106,    94,    94,    94,    94,    94,
      94,    94,    94,    89,   123,   106,   106,   106,   156,    12,
      11,    11,    65,   106,    13,    11,    11,    11,    65,    11,
      94,   167,   167,   124,    11,    94,    13,    11,   156,    65,
     140,    11
  };

  const unsigned char
  CubePL1Parser::yyr1_[] =
  {
       0,    92,    93,    93,    94,    94,    94,    94,    94,    94,
      94,    94,    94,    94,    94,    94,    94,    94,    94,    94,
      94,    94,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   103,   104,   105,   106,   106,   106,   106,   106,
     106,   106,   106,   106,   106,   106,   106,   106,   107,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   119,   119,   120,   121,   122,   123,   124,   125,
     125,   125,   126,   127,   128,   129,   130,   130,   131,   132,
     132,   132,   132,   132,   132,   132,   132,   132,   132,   132,
     132,   132,   132,   132,   132,   133,   134,   134,   135,   136,
     136,   136,   137,   137,   137,   138,   138,   138,   139,   139,
     140,   140,   140,   141,   142,   143,   144,   144,   145,   145,
     145,   145,   145,   145,   145,   145,   145,   145,   146,   147,
     147,   147,   147,   148,   149,   150,   151,   152,   152,   153,
     154,   155,   156,   157,   158,   159,   160,   160,   161,   162,
     163,   163,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   172,   173,   174,   175,   176,   177,   178,   179
  };

  const unsigned char
  CubePL1Parser::yyr2_[] =
  {
       0,     2,     3,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     3,     3,     3,     3,     3,     3,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     4,
       3,     3,     3,     1,     1,     3,     3,     3,     3,     3,
       3,     1,     1,     1,     3,     3,     3,     1,     1,     1,
       1,     1,     4,     4,     4,     5,     1,     1,     4,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     6,     1,     1,     1,     1,
       1,     1,     4,     5,     7,     4,     5,     7,     7,    11,
       1,     1,     1,     4,     3,     1,     0,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     7,     1,
       1,     1,     1,     2,     3,     4,     3,     1,     2,     5,
       4,     2,     3,     1,     5,     3,     1,     1,     4,     7,
       1,     1,     1,     3,     6,     6,     2,     1,     3,     3,
       3,     1,     1,     4,     7,     4,     4,     4,     4,     1
  };



  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const CubePL1Parser::yytname_[] =
  {
  "\"end of file\"", "error", "$undefined", "\"<cubepl>\"",
  "\"</cubepl>\"", "\"+\"", "\"-\"", "\"*\"", "\"/\"", "\"^\"", "\"(\"",
  "\")\"", "\"[\"", "\"]\"", "\"{\"", "\"}\"", "\"<<\"", "\">>\"",
  "\"return\"", "\";\"", "\"sizeof\"", "\"defined\"", "\"local\"",
  "\"global\"", "\"|\"", "\"arg1\"", "\"arg2\"", "\"sqrt\"", "\"sin\"",
  "\"asin\"", "\"cos\"", "\"acos\"", "\"tan\"", "\"atan\"", "\"exp\"",
  "\"log\"", "\"abs\"", "\"random\"", "\"sgn\"", "\"pos\"", "\"neg\"",
  "\"floor\"", "\"ceil\"", "\"min\"", "\"max\"", "\"if\"", "\"elseif\"",
  "\"else\"", "\"while\"", "\"=\"", "\">\"", "\"==\"", "\"eq\"", "\"seq\"",
  "\">=\"", "\"<=\"", "\"!=\"", "\"<\"", "\"not\"", "\"and\"", "\"or\"",
  "\"xor\"", "\"true\"", "\"false\"", "\"$\"", "\",\"", "\"=~\"",
  "\"metric::\"", "\"metric::fixed::\"", "\"metric::call::\"", "\"env\"",
  "\"lowercase\"", "\"uppercase\"", "\"cube::metric::postderived::\"",
  "\"cube::metric::prederived::\"", "\"cube::init::metric::\"",
  "\"cube::metric::plus::\"", "\"cube::metric::minus::\"",
  "\"cube::metric::set::\"", "\"cube::metric::get::\"", "CONSTANT",
  "STRING_CONSTANT", "STRING_TEXT", "METRIC_NAME", "INIT_METRIC_NAME",
  "\"i\"", "\"e\"", "SAME_MODIFICATOR", "QUOTED_STRING", "REGEXP_STRING",
  "FUNC2", "SIGN", "$accept", "document", "expression",
  "enclosed_expression", "absolute_value", "sum", "subtract", "division",
  "multiplication", "power", "negation", "arguments", "argument1",
  "argument2", "boolean_expression", "enclosed_boolean_expression",
  "and_expression", "or_expression", "xor_expression", "true_expression",
  "false_expression", "equality", "not_equality", "bigger", "smaller",
  "halfbigger", "halfsmaller", "string_operation", "string_equality",
  "string_semi_equality", "regexp", "regexp_expression", "quoted_string",
  "string_function", "lowercase", "uppercase", "getenv", "metric_get",
  "function_call", "one_variable_function", "function_name",
  "two_variables_function", "function2_name", "constant", "metric_refs",
  "context_metric", "fixed_metric", "metric_call",
  "calculationFlavourModificator", "lambda_calcul", "return_expression",
  "lambda_start", "list_of_statements", "statement", "metric_set",
  "if_statement", "simple_if_statement", "full_if_statement",
  "elseif_full_if_statement", "elseif_if_statement", "elseifs",
  "elseif_enclosed_list_of_statements", "if_condition",
  "else_enclosed_list_of_statements", "enclosed_list_of_statements",
  "enclosed_list_of_statements_start", "while_statement", "assignment",
  "assignment_target", "put_variable", "put_variable_indexed",
  "create_ghost_metric", "create_postderived_ghost_metric",
  "create_prederived_inclusive_ghost_metric",
  "create_prederived_exclusive_ghost_metric", "GhostMetricDefinition",
  "CubePLGhostText", "init_metric", "flex_aggr_plus", "flex_aggr_minus",
  "get_variable", "get_variable_simple", "get_variable_indexed",
  "sizeof_variable", "defined_variable", "local_variable",
  "global_variable", "string_constant", YY_NULLPTR
  };

#if CUBEPLPARSERDEBUG
  const unsigned short int
  CubePL1Parser::yyrline_[] =
  {
       0,   311,   311,   320,   327,   328,   329,   330,   331,   332,
     333,   334,   335,   336,   337,   338,   339,   340,   341,   342,
     343,   344,   345,   349,   359,   372,   385,   399,   412,   428,
     440,   452,   452,   454,   463,   475,   476,   477,   478,   479,
     480,   481,   482,   483,   484,   485,   486,   487,   491,   501,
     512,   525,   539,   552,   560,   569,   583,   597,   610,   624,
     638,   654,   655,   656,   661,   674,   686,   701,   729,   740,
     740,   740,   745,   756,   768,   782,   805,   805,   807,   822,
     830,   838,   846,   854,   862,   870,   878,   886,   894,   902,
     910,   918,   926,   934,   942,   953,   969,   977,   988,   999,
     999,   999,  1004,  1024,  1046,  1074,  1094,  1123,  1160,  1195,
    1242,  1250,  1258,  1269,  1299,  1301,  1310,  1312,  1318,  1329,
    1340,  1351,  1355,  1359,  1370,  1374,  1378,  1382,  1388,  1412,
    1413,  1414,  1415,  1419,  1446,  1485,  1528,  1561,  1562,  1565,
    1575,  1578,  1581,  1584,  1594,  1623,  1640,  1640,  1643,  1653,
    1657,  1658,  1659,  1663,  1710,  1756,  1804,  1807,  1813,  1845,
    1877,  1911,  1924,  1939,  1948,  1952,  1964,  1974,  1983,  1993
  };

  // Print the state stack on the debug stream.
  void
  CubePL1Parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  CubePL1Parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):" << std::endl;
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // CUBEPLPARSERDEBUG

  // Symbol number corresponding to token number t.
  inline
  CubePL1Parser::token_number_type
  CubePL1Parser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
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
      80,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      81,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      82,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      83,    84,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      88,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      89,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      87,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    85,
      86,    90,    91
    };
    const unsigned int user_token_number_max_ = 682;
    const token_number_type undef_token_ = 2;

    if (static_cast<int>(t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned int> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }


} // cubeplparser
#line 3276 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.cpp" // lalr1.cc:1167
#line 2003 "../../cubelib/build-frontend/../src/cube/src/syntax/cubepl/CubePL1Parser.yy" // lalr1.cc:1168
 /*** Additional Code ***/


void cubeplparser::CubePL1Parser::error(const CubePL1Parser::location_type& l,
				  const std::string& m)
{

    CubePL1Parser::location_type _l = l;


    _l.end.column = (_l.end.column<9)?1: _l.end.column-8;
    _l.begin.column = _l.end.column-1; 

    parseContext.syntax_ok = false;
    stringstream sstr;
    string       str;
    sstr << _l;
    sstr >> str;
    parseContext.error_message = str + ":" + m;
    parseContext.error_place = _l;

/*   if (strstr(m.c_str(),"expecting <?xml")!=NULL) {
     driver.error_just_message("The cube file is probably empty or filled with wrong content. The file has ended before the header of cube started. \n");
    }
   if (strstr(m.c_str()," expecting </row>")!=NULL) {
     driver.error_just_message("One of the possible reasons is \n    1) that the severity value is malformed. CUBE expects the \"double\" value in C_LOCALE with dot instead of comma;. \n    2) that the CUBE file is not properly ended. Probably the writing of CUBE file was interrupted.");
    }
   if (strstr(m.c_str()," expecting <matrix")!=NULL || 
        (strstr(m.c_str()," expecting <severity>")!=NULL) ) {
     driver.error_just_message("The cube file has probably a proper structure, but doesn't contain any severity values.");
    }
   if (strstr(m.c_str()," expecting <metric")!=NULL) {
     driver.error_just_message("The cube file doesn't contain any information about metric dimension.");
    }
   if (strstr(m.c_str()," expecting <region")!=NULL) {
     driver.error_just_message("The cube file doesn't contain any information about program dimension.");
    }
   if (strstr(m.c_str()," expecting <machine")!=NULL) {
     driver.error_just_message("The cube file doesn't contain any information about system dimension.");
    }
   if (strstr(m.c_str()," expecting <thread")!=NULL) {
     driver.error_just_message("The system dimension of the cube file is malformed. It contains a process without any threads.");
    }
   if (strstr(m.c_str()," expecting <process")!=NULL) {
     driver.error_just_message("The system dimension of the cube file is malformed. It contains a node without any processes.");
    }
   if (strstr(m.c_str()," expecting <node")!=NULL) {
     driver.error_just_message("The system dimension of the cube file is malformed. It contains a machine without any computing nodes.");
    }
      driver.error(l, m);
*/
}




