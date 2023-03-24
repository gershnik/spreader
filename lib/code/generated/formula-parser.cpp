// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.


// Take the name prefix into account.
#define yylex   sprflex



#include "formula-parser.h"


// Unqualified %code blocks.
#line 38 "lib/code/src/formula.y"


    #include <spreader/compiler.h>

    using namespace Spreader;

    SPR_CLANG_WARNING_DISABLE("-Wunused-but-set-variable")

#line 57 "lib/code/generated/formula-parser.cpp"


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


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif



// Enable debugging if requested.
#if SPRFDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
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
      yy_stack_print_ ();                \
  } while (false)

#else // !SPRFDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !SPRFDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 10 "lib/code/src/formula.y"
namespace Spreader { namespace FormulaParser {
#line 131 "lib/code/generated/formula-parser.cpp"

  /// Build a parser object.
  Parser::Parser (Builder & builder_yyarg, yyscan_t yyscanner_yyarg)
#if SPRFDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      builder (builder_yyarg),
      yyscanner (yyscanner_yyarg)
  {}

  Parser::~Parser ()
  {}

  Parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/



  // by_state.
  Parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  Parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  Parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  Parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  Parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  Parser::symbol_kind_type
  Parser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  Parser::stack_symbol_type::stack_symbol_type ()
  {}

  Parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_argument_list: // argument_list
        value.YY_MOVE_OR_COPY< ArgumentList > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_expression: // expression
      case symbol_kind::S_array: // array
      case symbol_kind::S_cell_reference: // cell_reference
        value.YY_MOVE_OR_COPY< AstNodePtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ERROR_CONSTANT: // ERROR_CONSTANT
        value.YY_MOVE_OR_COPY< Error > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_FUNC_START: // FUNC_START
        value.YY_MOVE_OR_COPY< FunctionId > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_row: // row
      case symbol_kind::S_column: // column
        value.YY_MOVE_OR_COPY< ReferenceTypeAndValue > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_scalar: // scalar
        value.YY_MOVE_OR_COPY< Scalar > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_COLUMN: // COLUMN
      case symbol_kind::S_ROW: // ROW
        value.YY_MOVE_OR_COPY< SizeType > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_STRING_CONSTANT: // STRING_CONSTANT
        value.YY_MOVE_OR_COPY< String > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_LOGICAL_CONSTANT: // LOGICAL_CONSTANT
        value.YY_MOVE_OR_COPY< bool > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_NUMERICAL_CONSTANT: // NUMERICAL_CONSTANT
      case symbol_kind::S_PLUS_NUMERICAL_CONSTANT: // PLUS_NUMERICAL_CONSTANT
      case symbol_kind::S_MINUS_NUMERICAL_CONSTANT: // MINUS_NUMERICAL_CONSTANT
        value.YY_MOVE_OR_COPY< double > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_array_row: // array_row
        value.YY_MOVE_OR_COPY< std::vector<Scalar> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_array_columns: // array_columns
        value.YY_MOVE_OR_COPY< std::vector<std::vector<Scalar>> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  Parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s)
  {
    switch (that.kind ())
    {
      case symbol_kind::S_argument_list: // argument_list
        value.move< ArgumentList > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_expression: // expression
      case symbol_kind::S_array: // array
      case symbol_kind::S_cell_reference: // cell_reference
        value.move< AstNodePtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ERROR_CONSTANT: // ERROR_CONSTANT
        value.move< Error > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_FUNC_START: // FUNC_START
        value.move< FunctionId > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_row: // row
      case symbol_kind::S_column: // column
        value.move< ReferenceTypeAndValue > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_scalar: // scalar
        value.move< Scalar > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_COLUMN: // COLUMN
      case symbol_kind::S_ROW: // ROW
        value.move< SizeType > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_STRING_CONSTANT: // STRING_CONSTANT
        value.move< String > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_LOGICAL_CONSTANT: // LOGICAL_CONSTANT
        value.move< bool > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_NUMERICAL_CONSTANT: // NUMERICAL_CONSTANT
      case symbol_kind::S_PLUS_NUMERICAL_CONSTANT: // PLUS_NUMERICAL_CONSTANT
      case symbol_kind::S_MINUS_NUMERICAL_CONSTANT: // MINUS_NUMERICAL_CONSTANT
        value.move< double > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_array_row: // array_row
        value.move< std::vector<Scalar> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_array_columns: // array_columns
        value.move< std::vector<std::vector<Scalar>> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  Parser::stack_symbol_type&
  Parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_argument_list: // argument_list
        value.copy< ArgumentList > (that.value);
        break;

      case symbol_kind::S_expression: // expression
      case symbol_kind::S_array: // array
      case symbol_kind::S_cell_reference: // cell_reference
        value.copy< AstNodePtr > (that.value);
        break;

      case symbol_kind::S_ERROR_CONSTANT: // ERROR_CONSTANT
        value.copy< Error > (that.value);
        break;

      case symbol_kind::S_FUNC_START: // FUNC_START
        value.copy< FunctionId > (that.value);
        break;

      case symbol_kind::S_row: // row
      case symbol_kind::S_column: // column
        value.copy< ReferenceTypeAndValue > (that.value);
        break;

      case symbol_kind::S_scalar: // scalar
        value.copy< Scalar > (that.value);
        break;

      case symbol_kind::S_COLUMN: // COLUMN
      case symbol_kind::S_ROW: // ROW
        value.copy< SizeType > (that.value);
        break;

      case symbol_kind::S_STRING_CONSTANT: // STRING_CONSTANT
        value.copy< String > (that.value);
        break;

      case symbol_kind::S_LOGICAL_CONSTANT: // LOGICAL_CONSTANT
        value.copy< bool > (that.value);
        break;

      case symbol_kind::S_NUMERICAL_CONSTANT: // NUMERICAL_CONSTANT
      case symbol_kind::S_PLUS_NUMERICAL_CONSTANT: // PLUS_NUMERICAL_CONSTANT
      case symbol_kind::S_MINUS_NUMERICAL_CONSTANT: // MINUS_NUMERICAL_CONSTANT
        value.copy< double > (that.value);
        break;

      case symbol_kind::S_array_row: // array_row
        value.copy< std::vector<Scalar> > (that.value);
        break;

      case symbol_kind::S_array_columns: // array_columns
        value.copy< std::vector<std::vector<Scalar>> > (that.value);
        break;

      default:
        break;
    }

    return *this;
  }

  Parser::stack_symbol_type&
  Parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_argument_list: // argument_list
        value.move< ArgumentList > (that.value);
        break;

      case symbol_kind::S_expression: // expression
      case symbol_kind::S_array: // array
      case symbol_kind::S_cell_reference: // cell_reference
        value.move< AstNodePtr > (that.value);
        break;

      case symbol_kind::S_ERROR_CONSTANT: // ERROR_CONSTANT
        value.move< Error > (that.value);
        break;

      case symbol_kind::S_FUNC_START: // FUNC_START
        value.move< FunctionId > (that.value);
        break;

      case symbol_kind::S_row: // row
      case symbol_kind::S_column: // column
        value.move< ReferenceTypeAndValue > (that.value);
        break;

      case symbol_kind::S_scalar: // scalar
        value.move< Scalar > (that.value);
        break;

      case symbol_kind::S_COLUMN: // COLUMN
      case symbol_kind::S_ROW: // ROW
        value.move< SizeType > (that.value);
        break;

      case symbol_kind::S_STRING_CONSTANT: // STRING_CONSTANT
        value.move< String > (that.value);
        break;

      case symbol_kind::S_LOGICAL_CONSTANT: // LOGICAL_CONSTANT
        value.move< bool > (that.value);
        break;

      case symbol_kind::S_NUMERICAL_CONSTANT: // NUMERICAL_CONSTANT
      case symbol_kind::S_PLUS_NUMERICAL_CONSTANT: // PLUS_NUMERICAL_CONSTANT
      case symbol_kind::S_MINUS_NUMERICAL_CONSTANT: // MINUS_NUMERICAL_CONSTANT
        value.move< double > (that.value);
        break;

      case symbol_kind::S_array_row: // array_row
        value.move< std::vector<Scalar> > (that.value);
        break;

      case symbol_kind::S_array_columns: // array_columns
        value.move< std::vector<std::vector<Scalar>> > (that.value);
        break;

      default:
        break;
    }

    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  Parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if SPRFDEBUG
  template <typename Base>
  void
  Parser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " (";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  Parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  Parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  Parser::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if SPRFDEBUG
  std::ostream&
  Parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  Parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  Parser::debug_level_type
  Parser::debug_level () const
  {
    return yydebug_;
  }

  void
  Parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // SPRFDEBUG

  Parser::state_type
  Parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  Parser::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  Parser::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  Parser::operator() ()
  {
    return parse ();
  }

  int
  Parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            symbol_type yylookahead (yylex (yyscanner));
            yyla.move (yylookahead);
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

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
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case symbol_kind::S_argument_list: // argument_list
        yylhs.value.emplace< ArgumentList > ();
        break;

      case symbol_kind::S_expression: // expression
      case symbol_kind::S_array: // array
      case symbol_kind::S_cell_reference: // cell_reference
        yylhs.value.emplace< AstNodePtr > ();
        break;

      case symbol_kind::S_ERROR_CONSTANT: // ERROR_CONSTANT
        yylhs.value.emplace< Error > ();
        break;

      case symbol_kind::S_FUNC_START: // FUNC_START
        yylhs.value.emplace< FunctionId > ();
        break;

      case symbol_kind::S_row: // row
      case symbol_kind::S_column: // column
        yylhs.value.emplace< ReferenceTypeAndValue > ();
        break;

      case symbol_kind::S_scalar: // scalar
        yylhs.value.emplace< Scalar > ();
        break;

      case symbol_kind::S_COLUMN: // COLUMN
      case symbol_kind::S_ROW: // ROW
        yylhs.value.emplace< SizeType > ();
        break;

      case symbol_kind::S_STRING_CONSTANT: // STRING_CONSTANT
        yylhs.value.emplace< String > ();
        break;

      case symbol_kind::S_LOGICAL_CONSTANT: // LOGICAL_CONSTANT
        yylhs.value.emplace< bool > ();
        break;

      case symbol_kind::S_NUMERICAL_CONSTANT: // NUMERICAL_CONSTANT
      case symbol_kind::S_PLUS_NUMERICAL_CONSTANT: // PLUS_NUMERICAL_CONSTANT
      case symbol_kind::S_MINUS_NUMERICAL_CONSTANT: // MINUS_NUMERICAL_CONSTANT
        yylhs.value.emplace< double > ();
        break;

      case symbol_kind::S_array_row: // array_row
        yylhs.value.emplace< std::vector<Scalar> > ();
        break;

      case symbol_kind::S_array_columns: // array_columns
        yylhs.value.emplace< std::vector<std::vector<Scalar>> > ();
        break;

      default:
        break;
    }



      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // formula: expression
#line 79 "lib/code/src/formula.y"
                        { builder.onRootNode(std::move(yystack_[0].value.as < AstNodePtr > ()));  }
#line 784 "lib/code/generated/formula-parser.cpp"
    break;

  case 3: // expression: '(' expression ')'
#line 83 "lib/code/src/formula.y"
                                                { yystack_[1].value.as < AstNodePtr > ()->setBracketed(true); yylhs.value.as < AstNodePtr > () = std::move(yystack_[1].value.as < AstNodePtr > ()); }
#line 790 "lib/code/generated/formula-parser.cpp"
    break;

  case 4: // expression: scalar
#line 84 "lib/code/src/formula.y"
                                                { yylhs.value.as < AstNodePtr > () = AstNodePtr(new ScalarNode         (std::move(yystack_[0].value.as < Scalar > ()))); }
#line 796 "lib/code/generated/formula-parser.cpp"
    break;

  case 5: // expression: array
#line 85 "lib/code/src/formula.y"
                                                { yylhs.value.as < AstNodePtr > () = std::move(yystack_[0].value.as < AstNodePtr > ()); }
#line 802 "lib/code/generated/formula-parser.cpp"
    break;

  case 6: // expression: cell_reference
#line 86 "lib/code/src/formula.y"
                                                { yylhs.value.as < AstNodePtr > () = std::move(yystack_[0].value.as < AstNodePtr > ()); }
#line 808 "lib/code/generated/formula-parser.cpp"
    break;

  case 7: // expression: OP_ADD expression
#line 87 "lib/code/src/formula.y"
                                                { yylhs.value.as < AstNodePtr > () = AstNodePtr(new AffirmNode         (std::move(yystack_[0].value.as < AstNodePtr > ()))); }
#line 814 "lib/code/generated/formula-parser.cpp"
    break;

  case 8: // expression: OP_SUB expression
#line 88 "lib/code/src/formula.y"
                                                { yylhs.value.as < AstNodePtr > () = AstNodePtr(new NegateNode         (std::move(yystack_[0].value.as < AstNodePtr > ()))); }
#line 820 "lib/code/generated/formula-parser.cpp"
    break;

  case 9: // expression: expression OP_POW expression
#line 89 "lib/code/src/formula.y"
                                                { yylhs.value.as < AstNodePtr > () = AstNodePtr(new PowerNode          (std::move(yystack_[2].value.as < AstNodePtr > ()), std::move(yystack_[0].value.as < AstNodePtr > ()))); }
#line 826 "lib/code/generated/formula-parser.cpp"
    break;

  case 10: // expression: expression OP_MUL expression
#line 90 "lib/code/src/formula.y"
                                                { yylhs.value.as < AstNodePtr > () = AstNodePtr(new MultiplyNode       (std::move(yystack_[2].value.as < AstNodePtr > ()), std::move(yystack_[0].value.as < AstNodePtr > ()))); }
#line 832 "lib/code/generated/formula-parser.cpp"
    break;

  case 11: // expression: expression OP_DIV expression
#line 91 "lib/code/src/formula.y"
                                                { yylhs.value.as < AstNodePtr > () = AstNodePtr(new DivideNode         (std::move(yystack_[2].value.as < AstNodePtr > ()), std::move(yystack_[0].value.as < AstNodePtr > ()))); }
#line 838 "lib/code/generated/formula-parser.cpp"
    break;

  case 12: // expression: expression OP_ADD expression
#line 92 "lib/code/src/formula.y"
                                                { yylhs.value.as < AstNodePtr > () = AstNodePtr(new AddNode            (std::move(yystack_[2].value.as < AstNodePtr > ()), std::move(yystack_[0].value.as < AstNodePtr > ()))); }
#line 844 "lib/code/generated/formula-parser.cpp"
    break;

  case 13: // expression: expression PLUS_NUMERICAL_CONSTANT
#line 93 "lib/code/src/formula.y"
                                                { yylhs.value.as < AstNodePtr > () = AstNodePtr(new AddNode            (std::move(yystack_[1].value.as < AstNodePtr > ()), AstNodePtr(new ScalarNode(yystack_[0].value.as < double > ())))); }
#line 850 "lib/code/generated/formula-parser.cpp"
    break;

  case 14: // expression: expression OP_SUB expression
#line 94 "lib/code/src/formula.y"
                                                { yylhs.value.as < AstNodePtr > () = AstNodePtr(new SubtractNode       (std::move(yystack_[2].value.as < AstNodePtr > ()), std::move(yystack_[0].value.as < AstNodePtr > ()))); }
#line 856 "lib/code/generated/formula-parser.cpp"
    break;

  case 15: // expression: expression MINUS_NUMERICAL_CONSTANT
#line 95 "lib/code/src/formula.y"
                                                { yylhs.value.as < AstNodePtr > () = AstNodePtr(new SubtractNode       (std::move(yystack_[1].value.as < AstNodePtr > ()), AstNodePtr(new ScalarNode(yystack_[0].value.as < double > ())))); }
#line 862 "lib/code/generated/formula-parser.cpp"
    break;

  case 16: // expression: expression OP_AMP expression
#line 96 "lib/code/src/formula.y"
                                                { yylhs.value.as < AstNodePtr > () = AstNodePtr(new AmpersandNode      (std::move(yystack_[2].value.as < AstNodePtr > ()), std::move(yystack_[0].value.as < AstNodePtr > ()))); }
#line 868 "lib/code/generated/formula-parser.cpp"
    break;

  case 17: // expression: expression OP_EQ expression
#line 97 "lib/code/src/formula.y"
                                                { yylhs.value.as < AstNodePtr > () = AstNodePtr(new EqualsNode         (std::move(yystack_[2].value.as < AstNodePtr > ()), std::move(yystack_[0].value.as < AstNodePtr > ()))); }
#line 874 "lib/code/generated/formula-parser.cpp"
    break;

  case 18: // expression: expression OP_NE expression
#line 98 "lib/code/src/formula.y"
                                                { yylhs.value.as < AstNodePtr > () = AstNodePtr(new NotEqualsNode      (std::move(yystack_[2].value.as < AstNodePtr > ()), std::move(yystack_[0].value.as < AstNodePtr > ()))); }
#line 880 "lib/code/generated/formula-parser.cpp"
    break;

  case 19: // expression: expression OP_LT expression
#line 99 "lib/code/src/formula.y"
                                                { yylhs.value.as < AstNodePtr > () = AstNodePtr(new LessNode           (std::move(yystack_[2].value.as < AstNodePtr > ()), std::move(yystack_[0].value.as < AstNodePtr > ()))); }
#line 886 "lib/code/generated/formula-parser.cpp"
    break;

  case 20: // expression: expression OP_LE expression
#line 100 "lib/code/src/formula.y"
                                                { yylhs.value.as < AstNodePtr > () = AstNodePtr(new LessEqualsNode     (std::move(yystack_[2].value.as < AstNodePtr > ()), std::move(yystack_[0].value.as < AstNodePtr > ()))); }
#line 892 "lib/code/generated/formula-parser.cpp"
    break;

  case 21: // expression: expression OP_GT expression
#line 101 "lib/code/src/formula.y"
                                                { yylhs.value.as < AstNodePtr > () = AstNodePtr(new GreaterNode        (std::move(yystack_[2].value.as < AstNodePtr > ()), std::move(yystack_[0].value.as < AstNodePtr > ()))); }
#line 898 "lib/code/generated/formula-parser.cpp"
    break;

  case 22: // expression: expression OP_GE expression
#line 102 "lib/code/src/formula.y"
                                                { yylhs.value.as < AstNodePtr > () = AstNodePtr(new GreaterEqualsNode  (std::move(yystack_[2].value.as < AstNodePtr > ()), std::move(yystack_[0].value.as < AstNodePtr > ()))); }
#line 904 "lib/code/generated/formula-parser.cpp"
    break;

  case 23: // expression: expression OP_PCT
#line 103 "lib/code/src/formula.y"
                                                { yylhs.value.as < AstNodePtr > () = AstNodePtr(new PercentNode        (std::move(yystack_[1].value.as < AstNodePtr > ()))); }
#line 910 "lib/code/generated/formula-parser.cpp"
    break;

  case 24: // expression: FUNC_START argument_list ')'
#line 104 "lib/code/src/formula.y"
                                                { auto func = createTrueFunctionNode(*this, yystack_[2].value.as < FunctionId > (), std::move(yystack_[1].value.as < ArgumentList > ())); 
                                                  if (!func) {
                                                      YYERROR;
                                                  }
                                                  yylhs.value.as < AstNodePtr > () = std::move(func);
                                                }
#line 921 "lib/code/generated/formula-parser.cpp"
    break;

  case 25: // scalar: ERROR_CONSTANT
#line 114 "lib/code/src/formula.y"
                                                { yylhs.value.as < Scalar > () = yystack_[0].value.as < Error > (); }
#line 927 "lib/code/generated/formula-parser.cpp"
    break;

  case 26: // scalar: LOGICAL_CONSTANT
#line 115 "lib/code/src/formula.y"
                                                { yylhs.value.as < Scalar > () = yystack_[0].value.as < bool > (); }
#line 933 "lib/code/generated/formula-parser.cpp"
    break;

  case 27: // scalar: PLUS_NUMERICAL_CONSTANT
#line 116 "lib/code/src/formula.y"
                                                { yylhs.value.as < Scalar > () = yystack_[0].value.as < double > (); }
#line 939 "lib/code/generated/formula-parser.cpp"
    break;

  case 28: // scalar: MINUS_NUMERICAL_CONSTANT
#line 117 "lib/code/src/formula.y"
                                                { yylhs.value.as < Scalar > () = -yystack_[0].value.as < double > (); }
#line 945 "lib/code/generated/formula-parser.cpp"
    break;

  case 29: // scalar: NUMERICAL_CONSTANT
#line 118 "lib/code/src/formula.y"
                                                { yylhs.value.as < Scalar > () = yystack_[0].value.as < double > (); }
#line 951 "lib/code/generated/formula-parser.cpp"
    break;

  case 30: // scalar: ROW
#line 119 "lib/code/src/formula.y"
                                                { yylhs.value.as < Scalar > () = double(yystack_[0].value.as < SizeType > () + 1); }
#line 957 "lib/code/generated/formula-parser.cpp"
    break;

  case 31: // scalar: STRING_CONSTANT
#line 120 "lib/code/src/formula.y"
                                                { yylhs.value.as < Scalar > () = std::move(yystack_[0].value.as < String > ()); }
#line 963 "lib/code/generated/formula-parser.cpp"
    break;

  case 32: // array: '{' array_columns '}'
#line 125 "lib/code/src/formula.y"
                                                {  
                                                    const Size size{SizeType(yystack_[1].value.as < std::vector<std::vector<Scalar>> > ()[0].size()), SizeType(yystack_[1].value.as < std::vector<std::vector<Scalar>> > ().size())};
                                                    std::vector<std::vector<Scalar>> & src = yystack_[1].value.as < std::vector<std::vector<Scalar>> > ();
                                                    auto arr = Array::create(size, [&src, size](Scalar * data) {
                                                        Point pt;
                                                        for(pt.y = 0; pt.y < size.height; ++pt.y)
                                                            for(pt.x = 0; pt.x < size.width; ++pt.x)
                                                                new (data + pt.y * size.width + pt.x) Scalar(std::move(src[pt.y][pt.x]));
                                                    });
                                                    yylhs.value.as < AstNodePtr > () = AstNodePtr(new ArrayNode(std::move(arr)));
                                                }
#line 979 "lib/code/generated/formula-parser.cpp"
    break;

  case 33: // array_columns: array_row
#line 139 "lib/code/src/formula.y"
                                                { yylhs.value.as < std::vector<std::vector<Scalar>> > ().emplace_back(std::move(yystack_[0].value.as < std::vector<Scalar> > ())); }
#line 985 "lib/code/generated/formula-parser.cpp"
    break;

  case 34: // array_columns: array_columns ';' array_row
#line 140 "lib/code/src/formula.y"
                                                { 
                                                    if (yystack_[2].value.as < std::vector<std::vector<Scalar>> > ()[0].size() != yystack_[0].value.as < std::vector<Scalar> > ().size()) {
                                                        error("row dimensions mismatch");
                                                        YYERROR;
                                                    }
                                                    yystack_[2].value.as < std::vector<std::vector<Scalar>> > ().emplace_back(std::move(yystack_[0].value.as < std::vector<Scalar> > ()));
                                                    yylhs.value.as < std::vector<std::vector<Scalar>> > () = std::move(yystack_[2].value.as < std::vector<std::vector<Scalar>> > ());
                                                }
#line 998 "lib/code/generated/formula-parser.cpp"
    break;

  case 35: // array_row: scalar
#line 152 "lib/code/src/formula.y"
                                                { yylhs.value.as < std::vector<Scalar> > ().emplace_back(std::move(yystack_[0].value.as < Scalar > ())); }
#line 1004 "lib/code/generated/formula-parser.cpp"
    break;

  case 36: // array_row: array_row ',' scalar
#line 153 "lib/code/src/formula.y"
                                                { yystack_[2].value.as < std::vector<Scalar> > ().emplace_back(std::move(yystack_[0].value.as < Scalar > ())); yylhs.value.as < std::vector<Scalar> > () = std::move(yystack_[2].value.as < std::vector<Scalar> > ()); }
#line 1010 "lib/code/generated/formula-parser.cpp"
    break;

  case 37: // row: ROW
#line 160 "lib/code/src/formula.y"
                                                { yylhs.value.as < ReferenceTypeAndValue > () = ReferenceTypeAndValue{ReferenceType::Relative, yystack_[0].value.as < SizeType > () - builder.evalPoint().y}; }
#line 1016 "lib/code/generated/formula-parser.cpp"
    break;

  case 38: // row: '$' ROW
#line 161 "lib/code/src/formula.y"
                                                { yylhs.value.as < ReferenceTypeAndValue > () = ReferenceTypeAndValue{ReferenceType::Absolute, yystack_[0].value.as < SizeType > ()}; }
#line 1022 "lib/code/generated/formula-parser.cpp"
    break;

  case 39: // column: COLUMN
#line 166 "lib/code/src/formula.y"
                                                { yylhs.value.as < ReferenceTypeAndValue > () = ReferenceTypeAndValue{ReferenceType::Relative, yystack_[0].value.as < SizeType > () - builder.evalPoint().x}; }
#line 1028 "lib/code/generated/formula-parser.cpp"
    break;

  case 40: // column: '$' COLUMN
#line 167 "lib/code/src/formula.y"
                                                { yylhs.value.as < ReferenceTypeAndValue > () = ReferenceTypeAndValue{ReferenceType::Absolute, yystack_[0].value.as < SizeType > ()}; }
#line 1034 "lib/code/generated/formula-parser.cpp"
    break;

  case 41: // column: OUT_OF_RANGE_COLUMN
#line 168 "lib/code/src/formula.y"
                                                { error("column out of range"); YYERROR; }
#line 1040 "lib/code/generated/formula-parser.cpp"
    break;

  case 42: // column: '$' OUT_OF_RANGE_COLUMN
#line 169 "lib/code/src/formula.y"
                                                { error("column out of range"); YYERROR; }
#line 1046 "lib/code/generated/formula-parser.cpp"
    break;

  case 43: // cell_reference: column row
#line 174 "lib/code/src/formula.y"
                                                { auto idx = builder.addReference(CellReference(yystack_[1].value.as < ReferenceTypeAndValue > (), yystack_[0].value.as < ReferenceTypeAndValue > ()));   
                                                  yylhs.value.as < AstNodePtr > () = AstNodePtr(new ReferenceNode(idx)); }
#line 1053 "lib/code/generated/formula-parser.cpp"
    break;

  case 44: // cell_reference: column ':' column
#line 176 "lib/code/src/formula.y"
                                                { auto idx = builder.addReference(ColumnReference(yystack_[2].value.as < ReferenceTypeAndValue > (), yystack_[0].value.as < ReferenceTypeAndValue > ())); 
                                                  yylhs.value.as < AstNodePtr > () = AstNodePtr(new ReferenceNode(idx)); }
#line 1060 "lib/code/generated/formula-parser.cpp"
    break;

  case 45: // cell_reference: row ':' row
#line 178 "lib/code/src/formula.y"
                                                { auto idx = builder.addReference(RowReference(yystack_[2].value.as < ReferenceTypeAndValue > (), yystack_[0].value.as < ReferenceTypeAndValue > ()));    
                                                  yylhs.value.as < AstNodePtr > () = AstNodePtr(new ReferenceNode(idx)); }
#line 1067 "lib/code/generated/formula-parser.cpp"
    break;

  case 46: // cell_reference: column row ':' column row
#line 180 "lib/code/src/formula.y"
                                                { auto idx = builder.addReference(AreaReference(yystack_[4].value.as < ReferenceTypeAndValue > (), yystack_[3].value.as < ReferenceTypeAndValue > (), yystack_[1].value.as < ReferenceTypeAndValue > (), yystack_[0].value.as < ReferenceTypeAndValue > ())); 
                                                  yylhs.value.as < AstNodePtr > () = AstNodePtr(new ReferenceNode(idx)); }
#line 1074 "lib/code/generated/formula-parser.cpp"
    break;

  case 47: // argument_list: %empty
#line 188 "lib/code/src/formula.y"
                                                {  }
#line 1080 "lib/code/generated/formula-parser.cpp"
    break;

  case 48: // argument_list: expression
#line 189 "lib/code/src/formula.y"
                                                { yylhs.value.as < ArgumentList > ().add(std::move(yystack_[0].value.as < AstNodePtr > ())); }
#line 1086 "lib/code/generated/formula-parser.cpp"
    break;

  case 49: // argument_list: argument_list ','
#line 190 "lib/code/src/formula.y"
                                                { yylhs.value.as < ArgumentList > ().add(std::move(yystack_[1].value.as < ArgumentList > ()));
                                                  if (yylhs.value.as < ArgumentList > ().size() == 0)
                                                      yylhs.value.as < ArgumentList > ().add(AstNodePtr(new ScalarNode(Scalar::Blank{})));
                                                  yylhs.value.as < ArgumentList > ().add(AstNodePtr(new ScalarNode(Scalar::Blank{})));
                                                }
#line 1096 "lib/code/generated/formula-parser.cpp"
    break;

  case 50: // argument_list: argument_list ',' expression
#line 195 "lib/code/src/formula.y"
                                                { yylhs.value.as < ArgumentList > ().add(std::move(yystack_[2].value.as < ArgumentList > ()));
                                                  if (yylhs.value.as < ArgumentList > ().size() == 0)
                                                      yylhs.value.as < ArgumentList > ().add(AstNodePtr(new ScalarNode(Scalar::Blank{})));
                                                  yylhs.value.as < ArgumentList > ().add(std::move(yystack_[0].value.as < AstNodePtr > ()));
                                                }
#line 1106 "lib/code/generated/formula-parser.cpp"
    break;


#line 1110 "lib/code/generated/formula-parser.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
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
        std::string msg = YY_("syntax error");
        error (YY_MOVE (msg));
      }


    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
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
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;


      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
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


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  Parser::error (const syntax_error& yyexc)
  {
    error (yyexc.what ());
  }

#if SPRFDEBUG || 0
  const char *
  Parser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytname_[yysymbol];
  }
#endif // #if SPRFDEBUG || 0









  const signed char Parser::yypact_ninf_ = -32;

  const signed char Parser::yytable_ninf_ = -38;

  const short
  Parser::yypact_[] =
  {
      58,   -32,   -32,   -32,   -32,   -32,   -32,   -32,   -17,   -32,
      58,    58,    58,    58,     8,    45,    20,   102,   -32,   -32,
     -10,    -8,   -32,   102,    21,     1,     1,    82,   -32,   -32,
      53,    -3,   -32,   -32,   -32,   -32,   -32,   -32,    58,    58,
      58,    58,    58,    58,    58,    58,    58,    58,    58,    58,
     -32,    12,   -32,    34,    10,    17,   -32,    58,   -32,   -32,
       8,     8,   122,   122,   142,   162,   170,    52,   150,   128,
     128,     3,     3,    86,   -32,    71,   -32,    10,   102,    -3,
     -32,    12,   -32
  };

  const signed char
  Parser::yydefact_[] =
  {
       0,    25,    26,    29,    27,    28,    31,    39,    30,    41,
      47,     0,     0,     0,     0,     0,     0,     2,     4,     5,
       0,     0,     6,    48,     0,     7,     8,     0,    30,    35,
       0,    33,    40,    38,    42,     1,    13,    15,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      23,     0,    37,     0,     0,    43,    24,    49,     3,    32,
       0,     0,    19,    21,    22,    20,    18,    17,    16,    12,
      14,    10,    11,     9,    45,     0,    44,     0,    50,    34,
      36,     0,    46
  };

  const signed char
  Parser::yypgoto_[] =
  {
     -32,   -32,    -7,   -13,   -32,   -32,    26,   -21,   -31,   -32,
     -32
  };

  const signed char
  Parser::yydefgoto_[] =
  {
       0,    16,    17,    18,    19,    30,    31,    20,    21,    22,
      24
  };

  const signed char
  Parser::yytable_[] =
  {
      55,    29,    52,    23,    25,    26,    27,    36,    37,    36,
      37,     1,     2,     3,     4,     5,     6,   -37,    28,     7,
      35,     9,    52,    76,    51,    53,    54,    49,    50,    61,
      74,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    75,    33,    53,    81,    29,    80,    56,
      78,    77,     0,    57,    32,    33,    34,     0,    36,    37,
      82,     1,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    44,    45,    46,    47,    48,    49,    50,    11,    12,
      32,     0,    34,    59,    60,    13,    79,    14,    36,    37,
       0,    15,    36,    37,     0,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    36,    37,
      58,    50,     0,     0,     0,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    36,    37,
       0,     0,     0,     0,    36,    37,     0,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    36,    37,
      47,    48,    49,    50,     0,     0,    36,    37,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    36,    37,
      45,    46,    47,    48,    49,    50,    36,    37,     0,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    43,    44,
      45,    46,    47,    48,    49,    50
  };

  const signed char
  Parser::yycheck_[] =
  {
      21,    14,    10,    10,    11,    12,    13,     6,     7,     6,
       7,     3,     4,     5,     6,     7,     8,    34,    10,     9,
       0,    11,    10,    54,    34,    33,    34,    24,    25,    32,
      51,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    33,    10,    33,    77,    60,    61,    28,
      57,    34,    -1,    32,     9,    10,    11,    -1,     6,     7,
      81,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    19,    20,    21,    22,    23,    24,    25,    20,    21,
       9,    -1,    11,    30,    31,    27,    60,    29,     6,     7,
      -1,    33,     6,     7,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,     6,     7,
      28,    25,    -1,    -1,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,     6,     7,
      -1,    -1,    -1,    -1,     6,     7,    -1,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,     6,     7,
      22,    23,    24,    25,    -1,    -1,     6,     7,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,     6,     7,
      20,    21,    22,    23,    24,    25,     6,     7,    -1,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    18,    19,
      20,    21,    22,    23,    24,    25
  };

  const signed char
  Parser::yystos_[] =
  {
       0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    20,    21,    27,    29,    33,    36,    37,    38,    39,
      42,    43,    44,    37,    45,    37,    37,    37,    10,    38,
      40,    41,     9,    10,    11,     0,     6,     7,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    34,    10,    33,    34,    42,    28,    32,    28,    30,
      31,    32,    37,    37,    37,    37,    37,    37,    37,    37,
      37,    37,    37,    37,    42,    33,    43,    34,    37,    41,
      38,    43,    42
  };

  const signed char
  Parser::yyr1_[] =
  {
       0,    35,    36,    37,    37,    37,    37,    37,    37,    37,
      37,    37,    37,    37,    37,    37,    37,    37,    37,    37,
      37,    37,    37,    37,    37,    38,    38,    38,    38,    38,
      38,    38,    39,    40,    40,    41,    41,    42,    42,    43,
      43,    43,    43,    44,    44,    44,    44,    45,    45,    45,
      45
  };

  const signed char
  Parser::yyr2_[] =
  {
       0,     2,     1,     3,     1,     1,     1,     2,     2,     3,
       3,     3,     3,     2,     3,     2,     3,     3,     3,     3,
       3,     3,     3,     2,     3,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     3,     1,     3,     1,     2,     1,
       2,     1,     2,     2,     3,     3,     5,     0,     1,     2,
       3
  };


#if SPRFDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const Parser::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "ERROR_CONSTANT",
  "LOGICAL_CONSTANT", "NUMERICAL_CONSTANT", "PLUS_NUMERICAL_CONSTANT",
  "MINUS_NUMERICAL_CONSTANT", "STRING_CONSTANT", "COLUMN", "ROW",
  "OUT_OF_RANGE_COLUMN", "FUNC_START", "OP_LT", "OP_GT", "OP_GE", "OP_LE",
  "OP_NE", "OP_EQ", "OP_AMP", "OP_ADD", "OP_SUB", "OP_MUL", "OP_DIV",
  "OP_POW", "OP_PCT", "UNARY_SIGN", "'('", "')'", "'{'", "'}'", "';'",
  "','", "'$'", "':'", "$accept", "formula", "expression", "scalar",
  "array", "array_columns", "array_row", "row", "column", "cell_reference",
  "argument_list", YY_NULLPTR
  };
#endif


#if SPRFDEBUG
  const unsigned char
  Parser::yyrline_[] =
  {
       0,    79,    79,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   114,   115,   116,   117,   118,
     119,   120,   125,   139,   140,   152,   153,   160,   161,   166,
     167,   168,   169,   174,   176,   178,   180,   188,   189,   190,
     195
  };

  void
  Parser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  Parser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // SPRFDEBUG


#line 10 "lib/code/src/formula.y"
} } // Spreader::FormulaParser
#line 1493 "lib/code/generated/formula-parser.cpp"

#line 203 "lib/code/src/formula.y"


void Spreader::FormulaParser::Parser::error(const std::string & ) {

    builder.onSyntaxError();
}
