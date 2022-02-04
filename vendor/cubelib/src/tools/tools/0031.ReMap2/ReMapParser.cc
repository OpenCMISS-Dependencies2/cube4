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
#define yylex   remapparserlex

// First part of user declarations.

#line 39 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:404

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "ReMapParser.hh"

// User implementation prologue.
#line 144 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:412



#include "ReMapDriver.h"
#include "ReMapScanner.h"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <stdlib.h>

using namespace std;
using namespace cube;

#include "CubeMetric.h"
#include "CubeCnode.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <utility>
#include <string>
#include <vector>
#include "CubeServices.h"
#include "ReMapParseContext.h"
#include "CubeMetric.h"
#include "Cube.h"
#include "CubeCartesian.h"



remapparser::ReMapParser::location_type remapparserlloc;



/* this "connects" the bison parser in the driver to the flex scanner class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the driver context. */
#undef yylex
#define yylex ReMapLexer.lex

// Workaround for Sun Studio C++ compilers on Solaris
#if defined(__SVR4) &&  defined(__SUNPRO_CC)
  #include <ieeefp.h>

  #define isinf(x)  (fpclass(x) == FP_NINF || fpclass(x) == FP_PINF)
  #define isnan(x)  isnand(x)
#endif


#line 101 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:412


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
#if REMAPPARSERDEBUG

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

#else // !REMAPPARSERDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE(Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void>(0)
# define YY_STACK_PRINT()                static_cast<void>(0)

#endif // !REMAPPARSERDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


namespace remapparser {
#line 187 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:479

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  ReMapParser::yytnamerr_ (const char *yystr)
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
  ReMapParser::ReMapParser (class ReMapDriver& driver_yyarg, class ReMapParseContext& parseContext_yyarg, class ReMapScanner& ReMapLexer_yyarg, class cube::Cube& cube_yyarg)
    :
#if REMAPPARSERDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      driver (driver_yyarg),
      parseContext (parseContext_yyarg),
      ReMapLexer (ReMapLexer_yyarg),
      cube (cube_yyarg)
  {}

  ReMapParser::~ReMapParser ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/

  inline
  ReMapParser::syntax_error::syntax_error (const location_type& l, const std::string& m)
    : std::runtime_error (m)
    , location (l)
  {}

  // basic_symbol.
  template <typename Base>
  inline
  ReMapParser::basic_symbol<Base>::basic_symbol ()
    : value ()
  {}

  template <typename Base>
  inline
  ReMapParser::basic_symbol<Base>::basic_symbol (const basic_symbol& other)
    : Base (other)
    , value ()
    , location (other.location)
  {
    value = other.value;
  }


  template <typename Base>
  inline
  ReMapParser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const semantic_type& v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  inline
  ReMapParser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const location_type& l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
  inline
  ReMapParser::basic_symbol<Base>::~basic_symbol ()
  {
    clear ();
  }

  template <typename Base>
  inline
  void
  ReMapParser::basic_symbol<Base>::clear ()
  {
    Base::clear ();
  }

  template <typename Base>
  inline
  bool
  ReMapParser::basic_symbol<Base>::empty () const
  {
    return Base::type_get () == empty_symbol;
  }

  template <typename Base>
  inline
  void
  ReMapParser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move(s);
    value = s.value;
    location = s.location;
  }

  // by_type.
  inline
  ReMapParser::by_type::by_type ()
    : type (empty_symbol)
  {}

  inline
  ReMapParser::by_type::by_type (const by_type& other)
    : type (other.type)
  {}

  inline
  ReMapParser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  inline
  void
  ReMapParser::by_type::clear ()
  {
    type = empty_symbol;
  }

  inline
  void
  ReMapParser::by_type::move (by_type& that)
  {
    type = that.type;
    that.clear ();
  }

  inline
  int
  ReMapParser::by_type::type_get () const
  {
    return type;
  }


  // by_state.
  inline
  ReMapParser::by_state::by_state ()
    : state (empty_state)
  {}

  inline
  ReMapParser::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  inline
  void
  ReMapParser::by_state::clear ()
  {
    state = empty_state;
  }

  inline
  void
  ReMapParser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  inline
  ReMapParser::by_state::by_state (state_type s)
    : state (s)
  {}

  inline
  ReMapParser::symbol_number_type
  ReMapParser::by_state::type_get () const
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[state];
  }

  inline
  ReMapParser::stack_symbol_type::stack_symbol_type ()
  {}


  inline
  ReMapParser::stack_symbol_type::stack_symbol_type (state_type s, symbol_type& that)
    : super_type (s, that.location)
  {
    value = that.value;
    // that is emptied.
    that.type = empty_symbol;
  }

  inline
  ReMapParser::stack_symbol_type&
  ReMapParser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    return *this;
  }


  template <typename Base>
  inline
  void
  ReMapParser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    YYUSE (yysym.type_get ());
  }

#if REMAPPARSERDEBUG
  template <typename Base>
  void
  ReMapParser::yy_print_ (std::ostream& yyo,
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
  ReMapParser::yypush_ (const char* m, state_type s, symbol_type& sym)
  {
    stack_symbol_type t (s, sym);
    yypush_ (m, t);
  }

  inline
  void
  ReMapParser::yypush_ (const char* m, stack_symbol_type& s)
  {
    if (m)
      YY_SYMBOL_PRINT (m, s);
    yystack_.push (s);
  }

  inline
  void
  ReMapParser::yypop_ (unsigned int n)
  {
    yystack_.pop (n);
  }

#if REMAPPARSERDEBUG
  std::ostream&
  ReMapParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  ReMapParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  ReMapParser::debug_level_type
  ReMapParser::debug_level () const
  {
    return yydebug_;
  }

  void
  ReMapParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // REMAPPARSERDEBUG

  inline ReMapParser::state_type
  ReMapParser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  inline bool
  ReMapParser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  ReMapParser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  ReMapParser::parse ()
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
    #line 66 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:741
{
    // initialize the initial location object
    yyla.location.begin.filename = yyla.location.end.filename = &driver.streamname;
}

#line 569 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:741

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
#line 201 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:859
    {  
    if (parseContext.metricTypeSeen)
      error(remapparserlloc,"Multiple metric type attributes defines!"); 
    else parseContext.metricTypeSeen = true;
    parseContext.metricType = parseContext.str.str(); }
#line 683 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
    break;

  case 3:
#line 209 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:859
    {  
    if (parseContext.metricVizTypeSeen)
      error(remapparserlloc,"Multiple metric visibilty type attributes defines!"); 
    else parseContext.metricVizTypeSeen = true;
    parseContext.metricVizType = parseContext.str.str(); }
#line 693 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
    break;

  case 4:
#line 217 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:859
    {  
    if (parseContext.metricConvertibleSeen)
      error(remapparserlloc,"Multiple metric convertible type attributes defines!"); 
    else parseContext.metricConvertibleSeen = true;
    parseContext.metricConvertible = true; }
#line 703 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
    break;

  case 5:
#line 223 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:859
    {  
    if (parseContext.metricConvertibleSeen)
      error(remapparserlloc,"Multiple metric convertible type attributes defines!"); 
    else parseContext.metricConvertibleSeen = true;
    parseContext.metricConvertible = false; }
#line 713 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
    break;

  case 6:
#line 233 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:859
    {  
    if (parseContext.metricCacheableSeen)
      error(remapparserlloc,"Multiple metric cacheable type attributes defines!"); 
    else parseContext.metricCacheableSeen = true;
    parseContext.metricCacheable = true; }
#line 723 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
    break;

  case 7:
#line 239 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:859
    {  
    if (parseContext.metricCacheableSeen)
      error(remapparserlloc,"Multiple metric cacheable type attributes defines!"); 
    else parseContext.metricCacheableSeen = true;
    parseContext.metricCacheable = false; }
#line 733 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
    break;

  case 9:
#line 250 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:859
    {  
    if (parseContext.metricsTitleSeen)
      error(remapparserlloc,"Multiple metrics titles defined!"); 
    else parseContext.metricsTitleSeen = true;
    parseContext.metricsTitle = parseContext.str.str(); 
}
#line 744 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
    break;

  case 11:
#line 261 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:859
    {  
    if (parseContext.cubeplTypeSeen)
      error(remapparserlloc,"Multiple cubepl type attributes defines!"); 
    else parseContext.cubeplTypeSeen = true;
    parseContext.rowwise = false; 
    
}
#line 756 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
    break;

  case 15:
#line 280 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:859
    {  
    if (parseContext.expressionAggrPlusSeen)
      error(remapparserlloc,"Multiple cubepl plus-aggregation attributes defines!"); 
    else parseContext.expressionAggrPlusSeen = true;
    parseContext.cubeplAggrType = ReMapParseContext::CUBEPL_AGGR_PLUS; 
}
#line 767 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
    break;

  case 16:
#line 289 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:859
    {  
    if (parseContext.expressionAggrMinusSeen)
      error(remapparserlloc,"Multiple cubepl minus-aggregation attributes defines!"); 
    else parseContext.expressionAggrMinusSeen = true;
    parseContext.cubeplAggrType = ReMapParseContext::CUBEPL_AGGR_MINUS; 
    
}
#line 779 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
    break;

  case 17:
#line 299 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:859
    {  
    if (parseContext.expressionAggrAggrSeen)
      error(remapparserlloc,"Multiple cubepl aggr-aggregation attributes defines!"); 
    else parseContext.expressionAggrAggrSeen = true;
    parseContext.cubeplAggrType = ReMapParseContext::CUBEPL_AGGR_AGGR; 
    
}
#line 791 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
    break;

  case 18:
#line 312 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:859
    {
	if (parseContext.murlSeen) 
	  error(remapparserlloc,"Multiple murl tags defined!"); 
	else parseContext.murlSeen = true;
	parseContext.murl = parseContext.str.str(); }
#line 801 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
    break;

  case 19:
#line 321 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:859
    { 
	if (parseContext.dispnameSeen) 
	  error(remapparserlloc,"Multiple disp_name tags defined!"); 
	else parseContext.dispnameSeen = true;
	parseContext.disp_name = parseContext.str.str(); }
#line 811 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
    break;

  case 20:
#line 328 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:859
    { 
	if (parseContext.uniqnameSeen) 
	  error(remapparserlloc,"Multiple uniq_name tags defined!"); 
	else parseContext.uniqnameSeen = true;
	parseContext.uniq_name = parseContext.str.str(); }
#line 821 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
    break;

  case 21:
#line 334 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:859
    { 
	if (parseContext.dtypeSeen) 
	  error(remapparserlloc,"Multiple dtype tags defined!"); 
	else parseContext.dtypeSeen = true;
	parseContext.dtype = parseContext.str.str(); }
#line 831 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
    break;

  case 22:
#line 341 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:859
    {
	if (parseContext.uomSeen) 
	  error(remapparserlloc,"Multiple uom tags defined!"); 
	else parseContext.uomSeen = true;
	parseContext.uom = parseContext.str.str(); }
#line 841 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
    break;

  case 23:
#line 348 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:859
    {
	if (parseContext.valSeen) 
	  error(remapparserlloc,"Multiple val tags defined!"); 
	else parseContext.valSeen = true;
	parseContext.val = parseContext.str.str(); }
#line 851 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
    break;

  case 24:
#line 355 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:859
    {
	if (parseContext.urlSeen) 
	  error(remapparserlloc,"Multiple url tags defined!");
	else parseContext.urlSeen = true;
	parseContext.url = parseContext.str.str(); }
#line 861 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
    break;

  case 25:
#line 365 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:859
    {
	if (parseContext.expressionSeen) 
	  error(remapparserlloc,"Multiple expressions defined!"); 
	else parseContext.expressionSeen = true;
	parseContext.expression = services::escapeFromXML(parseContext.str.str()); 
	}
#line 872 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
    break;

  case 26:
#line 373 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:859
    {
    if (parseContext.expressionInitSeen) 
      error(remapparserlloc,"Multiple expression initializations defined!"); 
    else parseContext.expressionInitSeen = true;
    parseContext.expressionInit = services::escapeFromXML(parseContext.str.str()); 
    }
#line 883 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
    break;

  case 27:
#line 381 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:859
    {
	if ( parseContext.cubeplAggrType == ReMapParseContext::CUBEPL_AGGR_PLUS)
	    parseContext.expressionAggrPlus = services::escapeFromXML(parseContext.str.str()); 
	else
	if ( parseContext.cubeplAggrType == ReMapParseContext::CUBEPL_AGGR_MINUS)
	    parseContext.expressionAggrMinus = services::escapeFromXML(parseContext.str.str()); 
	else
	if ( parseContext.cubeplAggrType == ReMapParseContext::CUBEPL_AGGR_AGGR)
	    parseContext.expressionAggrAggr = services::escapeFromXML(parseContext.str.str()); 
	}
#line 898 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
    break;

  case 28:
#line 392 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:859
    {
	if (parseContext.descrSeen) 
	  error(remapparserlloc,"Multiple descr tags defined!");
	else parseContext.descrSeen = true;
	parseContext.descr = parseContext.str.str(); 
	}
#line 909 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
    break;

  case 37:
#line 428 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:859
    {parseContext.murlSeen = false; cube.def_mirror(services::escapeFromXML(parseContext.murl));}
#line 915 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
    break;

  case 38:
#line 429 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:859
    {parseContext.murlSeen = false; cube.def_mirror(services::escapeFromXML(parseContext.murl));}
#line 921 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
    break;

  case 39:
#line 436 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:859
    {
    if (parseContext.metricsTitle != "")
        cube.set_metrics_title(services::escapeFromXML(parseContext.metricsTitle));
}
#line 930 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
    break;

  case 43:
#line 452 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:859
    {
	//check for attributes
	parseContext.idSeen = false;
	//check for tags
	if (parseContext.metricTypeSeen) parseContext.metricTypeSeen = false;
	else parseContext.metricType = "";
	if (parseContext.metricVizTypeSeen) parseContext.metricVizTypeSeen = false;
	else parseContext.metricVizType = "";
	if (parseContext.dispnameSeen) parseContext.dispnameSeen = false;
	else parseContext.disp_name = "";
	if (parseContext.uniqnameSeen) parseContext.uniqnameSeen = false;
	else parseContext.uniq_name = "";
	if (parseContext.dtypeSeen) parseContext.dtypeSeen = false;
	else parseContext.dtype = "";
	if (parseContext.uomSeen) parseContext.uomSeen = false;
	else parseContext.uom = "";
	if (parseContext.valSeen) parseContext.valSeen = false;
	else parseContext.val = "";
	if (parseContext.urlSeen) parseContext.urlSeen = false;
	else parseContext.url = "";
	if (parseContext.descrSeen) parseContext.descrSeen = false;
	else parseContext.descr = "";
	if (parseContext.expressionSeen) parseContext.expressionSeen = false;
	else parseContext.expression = "";
	if (parseContext.expressionInitSeen) parseContext.expressionInitSeen = false;
	else parseContext.expressionInit = "";
	if (parseContext.expressionAggrPlusSeen) parseContext.expressionAggrPlusSeen = false;
	else parseContext.expressionAggrPlus = "";
	if (parseContext.expressionAggrMinusSeen) parseContext.expressionAggrMinusSeen = false;
	else parseContext.expressionAggrMinus = "";
	if (parseContext.expressionAggrAggrSeen) parseContext.expressionAggrAggrSeen = false;
	else parseContext.expressionAggrAggr = "";
	if (parseContext.metricConvertibleSeen) parseContext.metricConvertibleSeen = false;
	else parseContext.metricConvertible = true;
	if (parseContext.metricCacheableSeen) parseContext.metricCacheableSeen = false;
	else parseContext.metricCacheable = true;
	if (parseContext.cubeplTypeSeen) parseContext.cubeplTypeSeen = false;
	else parseContext.rowwise = true;
/* 	while ((int)parseContext.metricVec.size()<=parseContext.id)  */
/* 		parseContext.metricVec.push_back(NULL); */
/* 	if (parseContext.metricVec[parseContext.id]!=NULL) */
/* 	  error(remapparserlloc,"Re-declared metric!"); */
	parseContext.currentMetric = 
		cube.def_met(
			services::escapeFromXML(parseContext.disp_name),
			services::escapeFromXML(parseContext.uniq_name),
			services::escapeFromXML(parseContext.dtype),
			services::escapeFromXML(parseContext.uom),
			services::escapeFromXML(parseContext.val),
			services::escapeFromXML(parseContext.url),
			services::escapeFromXML(parseContext.descr),
			parseContext.currentMetric,
			cube::Metric::get_type_of_metric(parseContext.metricType),
			services::escapeFromXML(parseContext.expression),
			services::escapeFromXML(parseContext.expressionInit),
			services::escapeFromXML(parseContext.expressionAggrPlus),
			services::escapeFromXML(parseContext.expressionAggrMinus),
			services::escapeFromXML(parseContext.expressionAggrAggr),
			parseContext.rowwise,
			cube::Metric::get_viz_type_of_metric(parseContext.metricVizType));

	parseContext.currentMetric->setConvertible( parseContext.metricConvertible );
        parseContext.currentMetric->setCacheable( parseContext.metricCacheable );

	parseContext.metricVec.push_back(parseContext.currentMetric);
}
#line 1001 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
    break;

  case 64:
#line 554 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:859
    {
	if (parseContext.currentMetric==NULL)
	  error(remapparserlloc,"Metric definitions are not correctly nested!");
	parseContext.currentMetric = 
		(parseContext.currentMetric)->get_parent();
}
#line 1012 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
    break;


#line 1016 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:859
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
  ReMapParser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what());
  }

  // Generate an error message.
  std::string
  ReMapParser::yysyntax_error_ (state_type yystate, const symbol_type& yyla) const
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


  const signed char ReMapParser::yypact_ninf_ = -44;

  const signed char ReMapParser::yytable_ninf_ = -1;

  const signed char
  ReMapParser::yypact_[] =
  {
     -19,   -43,   -37,     9,    -9,   -44,     8,     7,   -25,    -8,
     -44,   -44,   -44,   -44,    22,   -10,   -44,    -7,   -25,   -44,
     -26,    -6,   -44,   -44,   -44,   -44,    32,    33,    26,    28,
     -44,   -44,   -44,   -44,    29,   -26,   -44,   -44,   -44,   -44,
     -44,   -44,   -44,   -44,   -44,   -44,   -14,   -44,   -12,    24,
     -11,    20,    21,    17,    16,    18,    14,    13,   -44,   -44,
     -44,   -44,   -44,   -44,   -44,   -44,   -44,   -44,   -44,   -14,
     -44,    47,    41,   -44,    19,    42,   -44,   -44,   -44,   -44,
     -44,   -44,   -44,   -44,   -44,   -44,   -44,   -44,    38,   -44,
     -44,   -44,    35,   -44,   -44
  };

  const unsigned char
  ReMapParser::yydefact_[] =
  {
       0,     8,    32,     0,     0,    30,     0,     0,    35,     0,
      33,     1,    29,     9,     0,     0,    37,     0,    36,    31,
      44,     0,    40,    18,    34,    38,     0,     0,     0,     0,
      46,    47,    48,    49,     0,    44,    64,    39,    41,    42,
       2,     3,     5,     4,     7,     6,    50,    45,    10,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    54,    55,
      56,    57,    58,    59,    61,    62,    63,    60,    43,    51,
      52,     0,     0,    26,     0,     0,    12,    13,    14,    19,
      20,    21,    22,    23,    24,    28,    53,    11,     0,    15,
      16,    17,     0,    25,    27
  };

  const signed char
  ReMapParser::yypgoto_[] =
  {
     -44,   -44,   -44,   -44,   -44,   -44,   -44,   -44,   -44,   -44,
     -44,    37,   -44,   -44,   -44,   -44,   -44,   -44,   -44,   -44,
     -44,   -44,   -44,   -44,   -44,   -44,   -44,   -44,    52,   -44,
      36,    23,   -44,   -44,   -44,    -5,   -44
  };

  const signed char
  ReMapParser::yydefgoto_[] =
  {
      -1,    30,    31,    32,    33,     7,    72,    75,    76,    77,
      78,    16,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,     3,     4,     9,    10,    17,    18,     5,    21,
      22,    34,    35,    68,    69,    70,    39
  };

  const unsigned char
  ReMapParser::yytable_[] =
  {
      48,     1,    49,     6,    50,     8,    20,    36,    51,    11,
      52,     1,    53,    13,    54,    37,    55,    14,    56,    15,
      57,     2,    26,    27,    28,    29,    89,    90,    91,    42,
      43,    44,    45,    19,    20,    23,    24,    40,    41,    46,
      71,    73,    74,    79,    81,    82,    80,    84,    85,    83,
      87,    88,    92,    93,    94,    25,    12,    38,    47,     0,
       0,     0,     0,     0,    86
  };

  const signed char
  ReMapParser::yycheck_[] =
  {
      14,    20,    16,    46,    18,    42,    12,    13,    22,     0,
      24,    20,    26,     5,    28,    21,    30,    10,    32,    44,
      34,    40,    48,    49,    50,    51,     7,     8,     9,     3,
       4,     3,     4,    41,    12,    45,    43,     5,     5,    10,
      52,    17,    53,    23,    27,    29,    25,    33,    35,    31,
       3,    10,    10,    15,    19,    18,     4,    21,    35,    -1,
      -1,    -1,    -1,    -1,    69
  };

  const unsigned char
  ReMapParser::yystos_[] =
  {
       0,    20,    40,    76,    77,    82,    46,    59,    42,    78,
      79,     0,    82,     5,    10,    44,    65,    80,    81,    41,
      12,    83,    84,    45,    43,    65,    48,    49,    50,    51,
      55,    56,    57,    58,    85,    86,    13,    21,    84,    90,
       5,     5,     3,     4,     3,     4,    10,    85,    14,    16,
      18,    22,    24,    26,    28,    30,    32,    34,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    87,    88,
      89,    52,    60,    17,    53,    61,    62,    63,    64,    23,
      25,    27,    29,    31,    33,    35,    89,     3,    10,     7,
       8,     9,    10,    15,    19
  };

  const unsigned char
  ReMapParser::yyr1_[] =
  {
       0,    54,    55,    56,    57,    57,    58,    58,    59,    59,
      60,    60,    61,    61,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      76,    77,    78,    78,    79,    80,    80,    81,    81,    82,
      83,    83,    83,    84,    85,    85,    86,    86,    86,    86,
      87,    87,    88,    88,    89,    89,    89,    89,    89,    89,
      89,    89,    89,    89,    90
  };

  const unsigned char
  ReMapParser::yyr2_[] =
  {
       0,     2,     2,     2,     2,     2,     2,     2,     0,     2,
       0,     2,     1,     1,     1,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     4,     2,     4,     2,     2,
       1,     3,     0,     1,     3,     0,     1,     1,     2,     5,
       1,     2,     2,     4,     0,     2,     1,     1,     1,     1,
       0,     1,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1
  };



  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const ReMapParser::yytname_[] =
  {
  "\"end of file\"", "error", "$undefined", "\"false\"", "\"true\"",
  "\"attribute value\"", "\"error\"", "\"plus\"", "\"minus\"", "\"aggr\"",
  "\">\"", "\"/>\"", "\"<metric\"", "\"</metric>\"", "\"<cubepl\"",
  "\"</cubepl>\"", "\"<cubeplinit>\"", "\"</cubeplinit>\"",
  "\"<cubeplaggr\"", "\"</cubeplaggr>\"", "\"<metrics\"", "\"</metrics>\"",
  "\"<disp_name>\"", "\"</disp_name>\"", "\"<uniq_name>\"",
  "\"</uniq_name>\"", "\"<dtype>\"", "\"</dtype>\"", "\"<uom>\"",
  "\"</uom>\"", "\"<val>\"", "\"</val>\"", "\"<url>\"", "\"</url>\"",
  "\"<descr>\"", "\"</descr>\"", "\"<program\"", "\"</program>\"",
  "\"<name>\"", "\"</name>\"", "\"<doc>\"", "\"</doc>\"", "\"<mirrors>\"",
  "\"</mirrors>\"", "\"<murl>\"", "\"</murl>\"",
  "\"attribute name title\"", "\"attribute name metricId\"",
  "\"attribute name type\"", "\"attribute name viztype\"",
  "\"attribute name convertible\"", "\"attribute name cacheable\"",
  "\"attribute name rowwise\"", "\"attribute name cubeplaggrtype\"",
  "$accept", "metric_type_attr", "metric_viz_type_attr",
  "metric_convertible_attr", "metric_cacheable_attr", "metrics_title_attr",
  "expression_attr", "expression_aggr_attr", "expression_aggr_attr_plus",
  "expression_aggr_attr_minus", "expression_aggr_attr_aggr", "murl_tag",
  "disp_name_tag", "uniq_name_tag", "dtype_tag", "uom_tag", "val_tag",
  "url_tag", "expression_tag", "expression_init_tag",
  "expression_aggr_tag", "descr_tag", "document", "doc_tag",
  "mirrors_tag_opt", "mirrors_tag", "murl_tags_opt", "murl_tags",
  "metrics_tag", "metric_tag", "metric_begin", "metric_attrs",
  "metric_attr", "tags_of_metric_opt", "tags_of_metric", "tag_of_metric",
  "metric_end", YY_NULLPTR
  };

#if REMAPPARSERDEBUG
  const unsigned short int
  ReMapParser::yyrline_[] =
  {
       0,   201,   201,   209,   217,   223,   233,   239,   248,   250,
     259,   261,   272,   274,   276,   280,   289,   299,   312,   321,
     328,   334,   341,   348,   355,   365,   373,   381,   392,   405,
     407,   413,   415,   416,   420,   423,   424,   428,   429,   435,
     444,   445,   446,   452,   521,   522,   525,   526,   527,   528,
     531,   532,   536,   537,   541,   542,   543,   544,   545,   546,
     547,   548,   549,   550,   554
  };

  // Print the state stack on the debug stream.
  void
  ReMapParser::yystack_print_ ()
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
  ReMapParser::yy_reduce_print_ (int yyrule)
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
#endif // REMAPPARSERDEBUG

  // Symbol number corresponding to token number t.
  inline
  ReMapParser::token_number_type
  ReMapParser::yytranslate_ (int t)
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
      45,    46,    47,    48,    49,    50,    51,    52,    53
    };
    const unsigned int user_token_number_max_ = 308;
    const token_number_type undef_token_ = 2;

    if (static_cast<int>(t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned int> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }


} // remapparser
#line 1517 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.cc" // lalr1.cc:1167
#line 563 "../../cubelib/build-frontend/../src/tools/tools/0031.ReMap2/ReMapParser.yy" // lalr1.cc:1168
 /*** Additional Code ***/


void remapparser::ReMapParser::error(const ReMapParser::location_type& l,
			    const std::string& m)
{
   if (strstr(m.c_str(),"expecting <?xml")!=NULL) {
     driver.error_just_message("The cube file is probably empty or filled with wrong content. The file has ended before the header of cube started. \n");
    }
   if (strstr(m.c_str()," expecting <metric")!=NULL) {
     driver.error_just_message("The cube file doesn't contain any information about metric dimension.");
    }
      driver.error(l, m);

}



