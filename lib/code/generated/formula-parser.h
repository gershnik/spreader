// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton interface for Bison LALR(1) parsers in C++

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


/**
 ** \file lib/code/generated/formula-parser.h
 ** Define the Spreader::FormulaParser::parser class.
 */

// C++ LALR(1) parser skeleton written by Akim Demaille.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

#ifndef YY_SPRF_LIB_CODE_GENERATED_FORMULA_PARSER_H_INCLUDED
# define YY_SPRF_LIB_CODE_GENERATED_FORMULA_PARSER_H_INCLUDED
// "%code requires" blocks.
#line 19 "lib/code/src/formula.y"

    #include <spreader/scalar.h>
    #include <spreader/array.h>
    #include <spreader/reference.h>
    #include <functions/unary-operators.h>
    #include <functions/binary-operators.h>
    #include <functions/true-function.h>
    #include <formula-parser-builder.h>

    typedef void * yyscan_t;

#line 61 "lib/code/generated/formula-parser.h"


# include <cstdlib> // std::abort
# include <iostream>
# include <stdexcept>
# include <string>
# include <vector>

#if defined __cplusplus
# define YY_CPLUSPLUS __cplusplus
#else
# define YY_CPLUSPLUS 199711L
#endif

// Support move semantics when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_MOVE           std::move
# define YY_MOVE_OR_COPY   move
# define YY_MOVE_REF(Type) Type&&
# define YY_RVREF(Type)    Type&&
# define YY_COPY(Type)     Type
#else
# define YY_MOVE
# define YY_MOVE_OR_COPY   copy
# define YY_MOVE_REF(Type) Type&
# define YY_RVREF(Type)    const Type&
# define YY_COPY(Type)     const Type&
#endif

// Support noexcept when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_NOEXCEPT noexcept
# define YY_NOTHROW
#else
# define YY_NOEXCEPT
# define YY_NOTHROW throw ()
#endif

// Support constexpr when possible.
#if 201703 <= YY_CPLUSPLUS
# define YY_CONSTEXPR constexpr
#else
# define YY_CONSTEXPR
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

/* Debug traces.  */
#ifndef SPRFDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define SPRFDEBUG 1
#  else
#   define SPRFDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define SPRFDEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined SPRFDEBUG */

#line 10 "lib/code/src/formula.y"
namespace Spreader { namespace FormulaParser {
#line 205 "lib/code/generated/formula-parser.h"




  /// A Bison parser.
  class Parser
  {
  public:
#ifdef SPRFSTYPE
# ifdef __GNUC__
#  pragma GCC message "bison: do not #define SPRFSTYPE in C++, use %define api.value.type"
# endif
    typedef SPRFSTYPE value_type;
#else
  /// A buffer to store and retrieve objects.
  ///
  /// Sort of a variant, but does not keep track of the nature
  /// of the stored data, since that knowledge is available
  /// via the current parser state.
  class value_type
  {
  public:
    /// Type of *this.
    typedef value_type self_type;

    /// Empty construction.
    value_type () YY_NOEXCEPT
      : yyraw_ ()
    {}

    /// Construct and fill.
    template <typename T>
    value_type (YY_RVREF (T) t)
    {
      new (yyas_<T> ()) T (YY_MOVE (t));
    }

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    value_type (const self_type&) = delete;
    /// Non copyable.
    self_type& operator= (const self_type&) = delete;
#endif

    /// Destruction, allowed only if empty.
    ~value_type () YY_NOEXCEPT
    {}

# if 201103L <= YY_CPLUSPLUS
    /// Instantiate a \a T in here from \a t.
    template <typename T, typename... U>
    T&
    emplace (U&&... u)
    {
      return *new (yyas_<T> ()) T (std::forward <U>(u)...);
    }
# else
    /// Instantiate an empty \a T in here.
    template <typename T>
    T&
    emplace ()
    {
      return *new (yyas_<T> ()) T ();
    }

    /// Instantiate a \a T in here from \a t.
    template <typename T>
    T&
    emplace (const T& t)
    {
      return *new (yyas_<T> ()) T (t);
    }
# endif

    /// Instantiate an empty \a T in here.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build ()
    {
      return emplace<T> ();
    }

    /// Instantiate a \a T in here from \a t.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build (const T& t)
    {
      return emplace<T> (t);
    }

    /// Accessor to a built \a T.
    template <typename T>
    T&
    as () YY_NOEXCEPT
    {
      return *yyas_<T> ();
    }

    /// Const accessor to a built \a T (for %printer).
    template <typename T>
    const T&
    as () const YY_NOEXCEPT
    {
      return *yyas_<T> ();
    }

    /// Swap the content with \a that, of same type.
    ///
    /// Both variants must be built beforehand, because swapping the actual
    /// data requires reading it (with as()), and this is not possible on
    /// unconstructed variants: it would require some dynamic testing, which
    /// should not be the variant's responsibility.
    /// Swapping between built and (possibly) non-built is done with
    /// self_type::move ().
    template <typename T>
    void
    swap (self_type& that) YY_NOEXCEPT
    {
      std::swap (as<T> (), that.as<T> ());
    }

    /// Move the content of \a that to this.
    ///
    /// Destroys \a that.
    template <typename T>
    void
    move (self_type& that)
    {
# if 201103L <= YY_CPLUSPLUS
      emplace<T> (std::move (that.as<T> ()));
# else
      emplace<T> ();
      swap<T> (that);
# endif
      that.destroy<T> ();
    }

# if 201103L <= YY_CPLUSPLUS
    /// Move the content of \a that to this.
    template <typename T>
    void
    move (self_type&& that)
    {
      emplace<T> (std::move (that.as<T> ()));
      that.destroy<T> ();
    }
#endif

    /// Copy the content of \a that to this.
    template <typename T>
    void
    copy (const self_type& that)
    {
      emplace<T> (that.as<T> ());
    }

    /// Destroy the stored \a T.
    template <typename T>
    void
    destroy ()
    {
      as<T> ().~T ();
    }

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    value_type (const self_type&);
    /// Non copyable.
    self_type& operator= (const self_type&);
#endif

    /// Accessor to raw memory as \a T.
    template <typename T>
    T*
    yyas_ () YY_NOEXCEPT
    {
      void *yyp = yyraw_;
      return static_cast<T*> (yyp);
     }

    /// Const accessor to raw memory as \a T.
    template <typename T>
    const T*
    yyas_ () const YY_NOEXCEPT
    {
      const void *yyp = yyraw_;
      return static_cast<const T*> (yyp);
     }

    /// An auxiliary type to compute the largest semantic type.
    union union_type
    {
      // argument_list
      char dummy1[sizeof (ArgumentList)];

      // expression
      // array
      // cell_reference
      char dummy2[sizeof (AstNodePtr)];

      // ERROR_CONSTANT
      char dummy3[sizeof (Error)];

      // FUNC_START
      char dummy4[sizeof (FunctionId)];

      // row
      // column
      char dummy5[sizeof (ReferenceTypeAndValue)];

      // scalar
      char dummy6[sizeof (Scalar)];

      // COLUMN
      // ROW
      char dummy7[sizeof (SizeType)];

      // STRING_CONSTANT
      char dummy8[sizeof (String)];

      // LOGICAL_CONSTANT
      char dummy9[sizeof (bool)];

      // NUMERICAL_CONSTANT
      // PLUS_NUMERICAL_CONSTANT
      // MINUS_NUMERICAL_CONSTANT
      char dummy10[sizeof (double)];

      // array_row
      char dummy11[sizeof (std::vector<Scalar>)];

      // array_columns
      char dummy12[sizeof (std::vector<std::vector<Scalar>>)];
    };

    /// The size of the largest semantic type.
    enum { size = sizeof (union_type) };

    /// A buffer to store semantic values.
    union
    {
      /// Strongest alignment constraints.
      long double yyalign_me_;
      /// A buffer large enough to store any of the semantic values.
      char yyraw_[size];
    };
  };

#endif
    /// Backward compatibility (Bison 3.8).
    typedef value_type semantic_type;


    /// Syntax errors thrown from user actions.
    struct syntax_error : std::runtime_error
    {
      syntax_error (const std::string& m)
        : std::runtime_error (m)
      {}

      syntax_error (const syntax_error& s)
        : std::runtime_error (s.what ())
      {}

      ~syntax_error () YY_NOEXCEPT YY_NOTHROW;
    };

    /// Token kinds.
    struct token
    {
      enum token_kind_type
      {
        TOK_SPRFEMPTY = -2,
    TOK_YYEOF = 0,                 // "end of file"
    TOK_SPRFerror = 256,           // error
    TOK_SPRFUNDEF = 257,           // "invalid token"
    TOK_ERROR_CONSTANT = 258,      // ERROR_CONSTANT
    TOK_LOGICAL_CONSTANT = 259,    // LOGICAL_CONSTANT
    TOK_NUMERICAL_CONSTANT = 260,  // NUMERICAL_CONSTANT
    TOK_PLUS_NUMERICAL_CONSTANT = 261, // PLUS_NUMERICAL_CONSTANT
    TOK_MINUS_NUMERICAL_CONSTANT = 262, // MINUS_NUMERICAL_CONSTANT
    TOK_STRING_CONSTANT = 263,     // STRING_CONSTANT
    TOK_COLUMN = 264,              // COLUMN
    TOK_ROW = 265,                 // ROW
    TOK_OUT_OF_RANGE_COLUMN = 266, // OUT_OF_RANGE_COLUMN
    TOK_FUNC_START = 267,          // FUNC_START
    TOK_OP_LT = 268,               // OP_LT
    TOK_OP_GT = 269,               // OP_GT
    TOK_OP_GE = 270,               // OP_GE
    TOK_OP_LE = 271,               // OP_LE
    TOK_OP_NE = 272,               // OP_NE
    TOK_OP_EQ = 273,               // OP_EQ
    TOK_OP_AMP = 274,              // OP_AMP
    TOK_OP_ADD = 275,              // OP_ADD
    TOK_OP_SUB = 276,              // OP_SUB
    TOK_OP_MUL = 277,              // OP_MUL
    TOK_OP_DIV = 278,              // OP_DIV
    TOK_OP_POW = 279,              // OP_POW
    TOK_OP_PCT = 280,              // OP_PCT
    TOK_UNARY_SIGN = 281           // UNARY_SIGN
      };
      /// Backward compatibility alias (Bison 3.6).
      typedef token_kind_type yytokentype;
    };

    /// Token kind, as returned by yylex.
    typedef token::token_kind_type token_kind_type;

    /// Backward compatibility alias (Bison 3.6).
    typedef token_kind_type token_type;

    /// Symbol kinds.
    struct symbol_kind
    {
      enum symbol_kind_type
      {
        YYNTOKENS = 35, ///< Number of tokens.
        S_YYEMPTY = -2,
        S_YYEOF = 0,                             // "end of file"
        S_YYerror = 1,                           // error
        S_YYUNDEF = 2,                           // "invalid token"
        S_ERROR_CONSTANT = 3,                    // ERROR_CONSTANT
        S_LOGICAL_CONSTANT = 4,                  // LOGICAL_CONSTANT
        S_NUMERICAL_CONSTANT = 5,                // NUMERICAL_CONSTANT
        S_PLUS_NUMERICAL_CONSTANT = 6,           // PLUS_NUMERICAL_CONSTANT
        S_MINUS_NUMERICAL_CONSTANT = 7,          // MINUS_NUMERICAL_CONSTANT
        S_STRING_CONSTANT = 8,                   // STRING_CONSTANT
        S_COLUMN = 9,                            // COLUMN
        S_ROW = 10,                              // ROW
        S_OUT_OF_RANGE_COLUMN = 11,              // OUT_OF_RANGE_COLUMN
        S_FUNC_START = 12,                       // FUNC_START
        S_OP_LT = 13,                            // OP_LT
        S_OP_GT = 14,                            // OP_GT
        S_OP_GE = 15,                            // OP_GE
        S_OP_LE = 16,                            // OP_LE
        S_OP_NE = 17,                            // OP_NE
        S_OP_EQ = 18,                            // OP_EQ
        S_OP_AMP = 19,                           // OP_AMP
        S_OP_ADD = 20,                           // OP_ADD
        S_OP_SUB = 21,                           // OP_SUB
        S_OP_MUL = 22,                           // OP_MUL
        S_OP_DIV = 23,                           // OP_DIV
        S_OP_POW = 24,                           // OP_POW
        S_OP_PCT = 25,                           // OP_PCT
        S_UNARY_SIGN = 26,                       // UNARY_SIGN
        S_27_ = 27,                              // '('
        S_28_ = 28,                              // ')'
        S_29_ = 29,                              // '{'
        S_30_ = 30,                              // '}'
        S_31_ = 31,                              // ';'
        S_32_ = 32,                              // ','
        S_33_ = 33,                              // '$'
        S_34_ = 34,                              // ':'
        S_YYACCEPT = 35,                         // $accept
        S_formula = 36,                          // formula
        S_expression = 37,                       // expression
        S_scalar = 38,                           // scalar
        S_array = 39,                            // array
        S_array_columns = 40,                    // array_columns
        S_array_row = 41,                        // array_row
        S_row = 42,                              // row
        S_column = 43,                           // column
        S_cell_reference = 44,                   // cell_reference
        S_argument_list = 45                     // argument_list
      };
    };

    /// (Internal) symbol kind.
    typedef symbol_kind::symbol_kind_type symbol_kind_type;

    /// The number of tokens.
    static const symbol_kind_type YYNTOKENS = symbol_kind::YYNTOKENS;

    /// A complete symbol.
    ///
    /// Expects its Base type to provide access to the symbol kind
    /// via kind ().
    ///
    /// Provide access to semantic value.
    template <typename Base>
    struct basic_symbol : Base
    {
      /// Alias to Base.
      typedef Base super_type;

      /// Default constructor.
      basic_symbol () YY_NOEXCEPT
        : value ()
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      basic_symbol (basic_symbol&& that)
        : Base (std::move (that))
        , value ()
      {
        switch (this->kind ())
    {
      case symbol_kind::S_argument_list: // argument_list
        value.move< ArgumentList > (std::move (that.value));
        break;

      case symbol_kind::S_expression: // expression
      case symbol_kind::S_array: // array
      case symbol_kind::S_cell_reference: // cell_reference
        value.move< AstNodePtr > (std::move (that.value));
        break;

      case symbol_kind::S_ERROR_CONSTANT: // ERROR_CONSTANT
        value.move< Error > (std::move (that.value));
        break;

      case symbol_kind::S_FUNC_START: // FUNC_START
        value.move< FunctionId > (std::move (that.value));
        break;

      case symbol_kind::S_row: // row
      case symbol_kind::S_column: // column
        value.move< ReferenceTypeAndValue > (std::move (that.value));
        break;

      case symbol_kind::S_scalar: // scalar
        value.move< Scalar > (std::move (that.value));
        break;

      case symbol_kind::S_COLUMN: // COLUMN
      case symbol_kind::S_ROW: // ROW
        value.move< SizeType > (std::move (that.value));
        break;

      case symbol_kind::S_STRING_CONSTANT: // STRING_CONSTANT
        value.move< String > (std::move (that.value));
        break;

      case symbol_kind::S_LOGICAL_CONSTANT: // LOGICAL_CONSTANT
        value.move< bool > (std::move (that.value));
        break;

      case symbol_kind::S_NUMERICAL_CONSTANT: // NUMERICAL_CONSTANT
      case symbol_kind::S_PLUS_NUMERICAL_CONSTANT: // PLUS_NUMERICAL_CONSTANT
      case symbol_kind::S_MINUS_NUMERICAL_CONSTANT: // MINUS_NUMERICAL_CONSTANT
        value.move< double > (std::move (that.value));
        break;

      case symbol_kind::S_array_row: // array_row
        value.move< std::vector<Scalar> > (std::move (that.value));
        break;

      case symbol_kind::S_array_columns: // array_columns
        value.move< std::vector<std::vector<Scalar>> > (std::move (that.value));
        break;

      default:
        break;
    }

      }
#endif

      /// Copy constructor.
      basic_symbol (const basic_symbol& that);

      /// Constructors for typed symbols.
#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t)
        : Base (t)
      {}
#else
      basic_symbol (typename Base::kind_type t)
        : Base (t)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, ArgumentList&& v)
        : Base (t)
        , value (std::move (v))
      {}
#else
      basic_symbol (typename Base::kind_type t, const ArgumentList& v)
        : Base (t)
        , value (v)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AstNodePtr&& v)
        : Base (t)
        , value (std::move (v))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AstNodePtr& v)
        : Base (t)
        , value (v)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, Error&& v)
        : Base (t)
        , value (std::move (v))
      {}
#else
      basic_symbol (typename Base::kind_type t, const Error& v)
        : Base (t)
        , value (v)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, FunctionId&& v)
        : Base (t)
        , value (std::move (v))
      {}
#else
      basic_symbol (typename Base::kind_type t, const FunctionId& v)
        : Base (t)
        , value (v)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, ReferenceTypeAndValue&& v)
        : Base (t)
        , value (std::move (v))
      {}
#else
      basic_symbol (typename Base::kind_type t, const ReferenceTypeAndValue& v)
        : Base (t)
        , value (v)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, Scalar&& v)
        : Base (t)
        , value (std::move (v))
      {}
#else
      basic_symbol (typename Base::kind_type t, const Scalar& v)
        : Base (t)
        , value (v)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, SizeType&& v)
        : Base (t)
        , value (std::move (v))
      {}
#else
      basic_symbol (typename Base::kind_type t, const SizeType& v)
        : Base (t)
        , value (v)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, String&& v)
        : Base (t)
        , value (std::move (v))
      {}
#else
      basic_symbol (typename Base::kind_type t, const String& v)
        : Base (t)
        , value (v)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, bool&& v)
        : Base (t)
        , value (std::move (v))
      {}
#else
      basic_symbol (typename Base::kind_type t, const bool& v)
        : Base (t)
        , value (v)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, double&& v)
        : Base (t)
        , value (std::move (v))
      {}
#else
      basic_symbol (typename Base::kind_type t, const double& v)
        : Base (t)
        , value (v)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::vector<Scalar>&& v)
        : Base (t)
        , value (std::move (v))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::vector<Scalar>& v)
        : Base (t)
        , value (v)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::vector<std::vector<Scalar>>&& v)
        : Base (t)
        , value (std::move (v))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::vector<std::vector<Scalar>>& v)
        : Base (t)
        , value (v)
      {}
#endif

      /// Destroy the symbol.
      ~basic_symbol ()
      {
        clear ();
      }



      /// Destroy contents, and record that is empty.
      void clear () YY_NOEXCEPT
      {
        // User destructor.
        symbol_kind_type yykind = this->kind ();
        basic_symbol<Base>& yysym = *this;
        (void) yysym;
        switch (yykind)
        {
       default:
          break;
        }

        // Value type destructor.
switch (yykind)
    {
      case symbol_kind::S_argument_list: // argument_list
        value.template destroy< ArgumentList > ();
        break;

      case symbol_kind::S_expression: // expression
      case symbol_kind::S_array: // array
      case symbol_kind::S_cell_reference: // cell_reference
        value.template destroy< AstNodePtr > ();
        break;

      case symbol_kind::S_ERROR_CONSTANT: // ERROR_CONSTANT
        value.template destroy< Error > ();
        break;

      case symbol_kind::S_FUNC_START: // FUNC_START
        value.template destroy< FunctionId > ();
        break;

      case symbol_kind::S_row: // row
      case symbol_kind::S_column: // column
        value.template destroy< ReferenceTypeAndValue > ();
        break;

      case symbol_kind::S_scalar: // scalar
        value.template destroy< Scalar > ();
        break;

      case symbol_kind::S_COLUMN: // COLUMN
      case symbol_kind::S_ROW: // ROW
        value.template destroy< SizeType > ();
        break;

      case symbol_kind::S_STRING_CONSTANT: // STRING_CONSTANT
        value.template destroy< String > ();
        break;

      case symbol_kind::S_LOGICAL_CONSTANT: // LOGICAL_CONSTANT
        value.template destroy< bool > ();
        break;

      case symbol_kind::S_NUMERICAL_CONSTANT: // NUMERICAL_CONSTANT
      case symbol_kind::S_PLUS_NUMERICAL_CONSTANT: // PLUS_NUMERICAL_CONSTANT
      case symbol_kind::S_MINUS_NUMERICAL_CONSTANT: // MINUS_NUMERICAL_CONSTANT
        value.template destroy< double > ();
        break;

      case symbol_kind::S_array_row: // array_row
        value.template destroy< std::vector<Scalar> > ();
        break;

      case symbol_kind::S_array_columns: // array_columns
        value.template destroy< std::vector<std::vector<Scalar>> > ();
        break;

      default:
        break;
    }

        Base::clear ();
      }

#if SPRFDEBUG || 0
      /// The user-facing name of this symbol.
      const char *name () const YY_NOEXCEPT
      {
        return Parser::symbol_name (this->kind ());
      }
#endif // #if SPRFDEBUG || 0


      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// Whether empty.
      bool empty () const YY_NOEXCEPT;

      /// Destructive move, \a s is emptied into this.
      void move (basic_symbol& s);

      /// The semantic value.
      value_type value;

    private:
#if YY_CPLUSPLUS < 201103L
      /// Assignment operator.
      basic_symbol& operator= (const basic_symbol& that);
#endif
    };

    /// Type access provider for token (enum) based symbols.
    struct by_kind
    {
      /// The symbol kind as needed by the constructor.
      typedef token_kind_type kind_type;

      /// Default constructor.
      by_kind () YY_NOEXCEPT;

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      by_kind (by_kind&& that) YY_NOEXCEPT;
#endif

      /// Copy constructor.
      by_kind (const by_kind& that) YY_NOEXCEPT;

      /// Constructor from (external) token numbers.
      by_kind (kind_type t) YY_NOEXCEPT;



      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_kind& that);

      /// The (internal) type number (corresponding to \a type).
      /// \a empty when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// The symbol kind.
      /// \a S_YYEMPTY when empty.
      symbol_kind_type kind_;
    };

    /// Backward compatibility for a private implementation detail (Bison 3.6).
    typedef by_kind by_type;

    /// "External" symbols: returned by the scanner.
    struct symbol_type : basic_symbol<by_kind>
    {
      /// Superclass.
      typedef basic_symbol<by_kind> super_type;

      /// Empty symbol.
      symbol_type () YY_NOEXCEPT {}

      /// Constructor for valueless symbols, and symbols from each type.
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok)
        : super_type (token_kind_type (tok))
#else
      symbol_type (int tok)
        : super_type (token_kind_type (tok))
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, Error v)
        : super_type (token_kind_type (tok), std::move (v))
#else
      symbol_type (int tok, const Error& v)
        : super_type (token_kind_type (tok), v)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, FunctionId v)
        : super_type (token_kind_type (tok), std::move (v))
#else
      symbol_type (int tok, const FunctionId& v)
        : super_type (token_kind_type (tok), v)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, SizeType v)
        : super_type (token_kind_type (tok), std::move (v))
#else
      symbol_type (int tok, const SizeType& v)
        : super_type (token_kind_type (tok), v)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, String v)
        : super_type (token_kind_type (tok), std::move (v))
#else
      symbol_type (int tok, const String& v)
        : super_type (token_kind_type (tok), v)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, bool v)
        : super_type (token_kind_type (tok), std::move (v))
#else
      symbol_type (int tok, const bool& v)
        : super_type (token_kind_type (tok), v)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, double v)
        : super_type (token_kind_type (tok), std::move (v))
#else
      symbol_type (int tok, const double& v)
        : super_type (token_kind_type (tok), v)
#endif
      {}
    };

    /// Build a parser object.
    Parser (Builder & builder_yyarg, yyscan_t yyscanner_yyarg);
    virtual ~Parser ();

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    Parser (const Parser&) = delete;
    /// Non copyable.
    Parser& operator= (const Parser&) = delete;
#endif

    /// Parse.  An alias for parse ().
    /// \returns  0 iff parsing succeeded.
    int operator() ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if SPRFDEBUG
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
    /// \param msg    a description of the syntax error.
    virtual void error (const std::string& msg);

    /// Report a syntax error.
    void error (const syntax_error& err);

#if SPRFDEBUG || 0
    /// The user-facing name of the symbol whose (internal) number is
    /// YYSYMBOL.  No bounds checking.
    static const char *symbol_name (symbol_kind_type yysymbol);
#endif // #if SPRFDEBUG || 0


    // Implementation of make_symbol for each token kind.
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYEOF ()
      {
        return symbol_type (token::TOK_YYEOF);
      }
#else
      static
      symbol_type
      make_YYEOF ()
      {
        return symbol_type (token::TOK_YYEOF);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SPRFerror ()
      {
        return symbol_type (token::TOK_SPRFerror);
      }
#else
      static
      symbol_type
      make_SPRFerror ()
      {
        return symbol_type (token::TOK_SPRFerror);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SPRFUNDEF ()
      {
        return symbol_type (token::TOK_SPRFUNDEF);
      }
#else
      static
      symbol_type
      make_SPRFUNDEF ()
      {
        return symbol_type (token::TOK_SPRFUNDEF);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ERROR_CONSTANT (Error v)
      {
        return symbol_type (token::TOK_ERROR_CONSTANT, std::move (v));
      }
#else
      static
      symbol_type
      make_ERROR_CONSTANT (const Error& v)
      {
        return symbol_type (token::TOK_ERROR_CONSTANT, v);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LOGICAL_CONSTANT (bool v)
      {
        return symbol_type (token::TOK_LOGICAL_CONSTANT, std::move (v));
      }
#else
      static
      symbol_type
      make_LOGICAL_CONSTANT (const bool& v)
      {
        return symbol_type (token::TOK_LOGICAL_CONSTANT, v);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NUMERICAL_CONSTANT (double v)
      {
        return symbol_type (token::TOK_NUMERICAL_CONSTANT, std::move (v));
      }
#else
      static
      symbol_type
      make_NUMERICAL_CONSTANT (const double& v)
      {
        return symbol_type (token::TOK_NUMERICAL_CONSTANT, v);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PLUS_NUMERICAL_CONSTANT (double v)
      {
        return symbol_type (token::TOK_PLUS_NUMERICAL_CONSTANT, std::move (v));
      }
#else
      static
      symbol_type
      make_PLUS_NUMERICAL_CONSTANT (const double& v)
      {
        return symbol_type (token::TOK_PLUS_NUMERICAL_CONSTANT, v);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_MINUS_NUMERICAL_CONSTANT (double v)
      {
        return symbol_type (token::TOK_MINUS_NUMERICAL_CONSTANT, std::move (v));
      }
#else
      static
      symbol_type
      make_MINUS_NUMERICAL_CONSTANT (const double& v)
      {
        return symbol_type (token::TOK_MINUS_NUMERICAL_CONSTANT, v);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_STRING_CONSTANT (String v)
      {
        return symbol_type (token::TOK_STRING_CONSTANT, std::move (v));
      }
#else
      static
      symbol_type
      make_STRING_CONSTANT (const String& v)
      {
        return symbol_type (token::TOK_STRING_CONSTANT, v);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_COLUMN (SizeType v)
      {
        return symbol_type (token::TOK_COLUMN, std::move (v));
      }
#else
      static
      symbol_type
      make_COLUMN (const SizeType& v)
      {
        return symbol_type (token::TOK_COLUMN, v);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ROW (SizeType v)
      {
        return symbol_type (token::TOK_ROW, std::move (v));
      }
#else
      static
      symbol_type
      make_ROW (const SizeType& v)
      {
        return symbol_type (token::TOK_ROW, v);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OUT_OF_RANGE_COLUMN ()
      {
        return symbol_type (token::TOK_OUT_OF_RANGE_COLUMN);
      }
#else
      static
      symbol_type
      make_OUT_OF_RANGE_COLUMN ()
      {
        return symbol_type (token::TOK_OUT_OF_RANGE_COLUMN);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FUNC_START (FunctionId v)
      {
        return symbol_type (token::TOK_FUNC_START, std::move (v));
      }
#else
      static
      symbol_type
      make_FUNC_START (const FunctionId& v)
      {
        return symbol_type (token::TOK_FUNC_START, v);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_LT ()
      {
        return symbol_type (token::TOK_OP_LT);
      }
#else
      static
      symbol_type
      make_OP_LT ()
      {
        return symbol_type (token::TOK_OP_LT);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_GT ()
      {
        return symbol_type (token::TOK_OP_GT);
      }
#else
      static
      symbol_type
      make_OP_GT ()
      {
        return symbol_type (token::TOK_OP_GT);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_GE ()
      {
        return symbol_type (token::TOK_OP_GE);
      }
#else
      static
      symbol_type
      make_OP_GE ()
      {
        return symbol_type (token::TOK_OP_GE);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_LE ()
      {
        return symbol_type (token::TOK_OP_LE);
      }
#else
      static
      symbol_type
      make_OP_LE ()
      {
        return symbol_type (token::TOK_OP_LE);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_NE ()
      {
        return symbol_type (token::TOK_OP_NE);
      }
#else
      static
      symbol_type
      make_OP_NE ()
      {
        return symbol_type (token::TOK_OP_NE);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_EQ ()
      {
        return symbol_type (token::TOK_OP_EQ);
      }
#else
      static
      symbol_type
      make_OP_EQ ()
      {
        return symbol_type (token::TOK_OP_EQ);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_AMP ()
      {
        return symbol_type (token::TOK_OP_AMP);
      }
#else
      static
      symbol_type
      make_OP_AMP ()
      {
        return symbol_type (token::TOK_OP_AMP);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_ADD ()
      {
        return symbol_type (token::TOK_OP_ADD);
      }
#else
      static
      symbol_type
      make_OP_ADD ()
      {
        return symbol_type (token::TOK_OP_ADD);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_SUB ()
      {
        return symbol_type (token::TOK_OP_SUB);
      }
#else
      static
      symbol_type
      make_OP_SUB ()
      {
        return symbol_type (token::TOK_OP_SUB);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_MUL ()
      {
        return symbol_type (token::TOK_OP_MUL);
      }
#else
      static
      symbol_type
      make_OP_MUL ()
      {
        return symbol_type (token::TOK_OP_MUL);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_DIV ()
      {
        return symbol_type (token::TOK_OP_DIV);
      }
#else
      static
      symbol_type
      make_OP_DIV ()
      {
        return symbol_type (token::TOK_OP_DIV);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_POW ()
      {
        return symbol_type (token::TOK_OP_POW);
      }
#else
      static
      symbol_type
      make_OP_POW ()
      {
        return symbol_type (token::TOK_OP_POW);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_PCT ()
      {
        return symbol_type (token::TOK_OP_PCT);
      }
#else
      static
      symbol_type
      make_OP_PCT ()
      {
        return symbol_type (token::TOK_OP_PCT);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_UNARY_SIGN ()
      {
        return symbol_type (token::TOK_UNARY_SIGN);
      }
#else
      static
      symbol_type
      make_UNARY_SIGN ()
      {
        return symbol_type (token::TOK_UNARY_SIGN);
      }
#endif


  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    Parser (const Parser&);
    /// Non copyable.
    Parser& operator= (const Parser&);
#endif


    /// Stored state numbers (used for stacks).
    typedef signed char state_type;

    /// Compute post-reduction state.
    /// \param yystate   the current state
    /// \param yysym     the nonterminal to push on the stack
    static state_type yy_lr_goto_state_ (state_type yystate, int yysym);

    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT;

    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT;

    static const signed char yypact_ninf_;
    static const signed char yytable_ninf_;

    /// Convert a scanner token kind \a t to a symbol kind.
    /// In theory \a t should be a token_kind_type, but character literals
    /// are valid, yet not members of the token_kind_type enum.
    static symbol_kind_type yytranslate_ (int t) YY_NOEXCEPT;

#if SPRFDEBUG || 0
    /// For a symbol, its name in clear.
    static const char* const yytname_[];
#endif // #if SPRFDEBUG || 0


    // Tables.
    // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
    // STATE-NUM.
    static const short yypact_[];

    // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
    // Performed when YYTABLE does not specify something else to do.  Zero
    // means the default is an error.
    static const signed char yydefact_[];

    // YYPGOTO[NTERM-NUM].
    static const signed char yypgoto_[];

    // YYDEFGOTO[NTERM-NUM].
    static const signed char yydefgoto_[];

    // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
    // positive, shift that token.  If negative, reduce the rule whose
    // number is the opposite.  If YYTABLE_NINF, syntax error.
    static const signed char yytable_[];

    static const signed char yycheck_[];

    // YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
    // state STATE-NUM.
    static const signed char yystos_[];

    // YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.
    static const signed char yyr1_[];

    // YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.
    static const signed char yyr2_[];


#if SPRFDEBUG
    // YYRLINE[YYN] -- Source line where rule number YYN was defined.
    static const unsigned char yyrline_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r) const;
    /// Print the state stack on the debug stream.
    virtual void yy_stack_print_ () const;

    /// Debugging level.
    int yydebug_;
    /// Debug stream.
    std::ostream* yycdebug_;

    /// \brief Display a symbol kind, value and location.
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
      by_state () YY_NOEXCEPT;

      /// The symbol kind as needed by the constructor.
      typedef state_type kind_type;

      /// Constructor.
      by_state (kind_type s) YY_NOEXCEPT;

      /// Copy constructor.
      by_state (const by_state& that) YY_NOEXCEPT;

      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_state& that);

      /// The symbol kind (corresponding to \a state).
      /// \a symbol_kind::S_YYEMPTY when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// The state number used to denote an empty symbol.
      /// We use the initial state, as it does not have a value.
      enum { empty_state = 0 };

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
      /// Move or copy construction.
      stack_symbol_type (YY_RVREF (stack_symbol_type) that);
      /// Steal the contents from \a sym to build this.
      stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) sym);
#if YY_CPLUSPLUS < 201103L
      /// Assignment, needed by push_back by some old implementations.
      /// Moves the contents of that.
      stack_symbol_type& operator= (stack_symbol_type& that);

      /// Assignment, needed by push_back by other implementations.
      /// Needed by some other old implementations.
      stack_symbol_type& operator= (const stack_symbol_type& that);
#endif
    };

    /// A stack with random access from its top.
    template <typename T, typename S = std::vector<T> >
    class stack
    {
    public:
      // Hide our reversed order.
      typedef typename S::iterator iterator;
      typedef typename S::const_iterator const_iterator;
      typedef typename S::size_type size_type;
      typedef typename std::ptrdiff_t index_type;

      stack (size_type n = 200) YY_NOEXCEPT
        : seq_ (n)
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Non copyable.
      stack (const stack&) = delete;
      /// Non copyable.
      stack& operator= (const stack&) = delete;
#endif

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      const T&
      operator[] (index_type i) const
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      T&
      operator[] (index_type i)
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Steal the contents of \a t.
      ///
      /// Close to move-semantics.
      void
      push (YY_MOVE_REF (T) t)
      {
        seq_.push_back (T ());
        operator[] (0).move (t);
      }

      /// Pop elements from the stack.
      void
      pop (std::ptrdiff_t n = 1) YY_NOEXCEPT
      {
        for (; 0 < n; --n)
          seq_.pop_back ();
      }

      /// Pop all elements from the stack.
      void
      clear () YY_NOEXCEPT
      {
        seq_.clear ();
      }

      /// Number of elements on the stack.
      index_type
      size () const YY_NOEXCEPT
      {
        return index_type (seq_.size ());
      }

      /// Iterator on top of the stack (going downwards).
      const_iterator
      begin () const YY_NOEXCEPT
      {
        return seq_.begin ();
      }

      /// Bottom of the stack.
      const_iterator
      end () const YY_NOEXCEPT
      {
        return seq_.end ();
      }

      /// Present a slice of the top of a stack.
      class slice
      {
      public:
        slice (const stack& stack, index_type range) YY_NOEXCEPT
          : stack_ (stack)
          , range_ (range)
        {}

        const T&
        operator[] (index_type i) const
        {
          return stack_[range_ - i];
        }

      private:
        const stack& stack_;
        index_type range_;
      };

    private:
#if YY_CPLUSPLUS < 201103L
      /// Non copyable.
      stack (const stack&);
      /// Non copyable.
      stack& operator= (const stack&);
#endif
      /// The wrapped container.
      S seq_;
    };


    /// Stack type.
    typedef stack<stack_symbol_type> stack_type;

    /// The stack.
    stack_type yystack_;

    /// Push a new state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param sym  the symbol
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym);

    /// Push a new look ahead token on the state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the state
    /// \param sym  the symbol (for its value and location).
    /// \warning the contents of \a sym.value is stolen.
    void yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym);

    /// Pop \a n symbols from the stack.
    void yypop_ (int n = 1) YY_NOEXCEPT;

    /// Constants.
    enum
    {
      yylast_ = 195,     ///< Last index in yytable_.
      yynnts_ = 11,  ///< Number of nonterminal symbols.
      yyfinal_ = 35 ///< Termination state number.
    };


    // User arguments.
    Builder & builder;
    yyscan_t yyscanner;

  };

  inline
  Parser::symbol_kind_type
  Parser::yytranslate_ (int t) YY_NOEXCEPT
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const signed char
    translate_table[] =
    {
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    33,     2,     2,     2,
      27,    28,     2,     2,    32,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    34,    31,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    29,     2,    30,     2,     2,     2,     2,
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
      25,    26
    };
    // Last valid token kind.
    const int code_max = 281;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return static_cast <symbol_kind_type> (translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

  // basic_symbol.
  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value ()
  {
    switch (this->kind ())
    {
      case symbol_kind::S_argument_list: // argument_list
        value.copy< ArgumentList > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_expression: // expression
      case symbol_kind::S_array: // array
      case symbol_kind::S_cell_reference: // cell_reference
        value.copy< AstNodePtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ERROR_CONSTANT: // ERROR_CONSTANT
        value.copy< Error > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_FUNC_START: // FUNC_START
        value.copy< FunctionId > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_row: // row
      case symbol_kind::S_column: // column
        value.copy< ReferenceTypeAndValue > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_scalar: // scalar
        value.copy< Scalar > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_COLUMN: // COLUMN
      case symbol_kind::S_ROW: // ROW
        value.copy< SizeType > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_STRING_CONSTANT: // STRING_CONSTANT
        value.copy< String > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_LOGICAL_CONSTANT: // LOGICAL_CONSTANT
        value.copy< bool > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_NUMERICAL_CONSTANT: // NUMERICAL_CONSTANT
      case symbol_kind::S_PLUS_NUMERICAL_CONSTANT: // PLUS_NUMERICAL_CONSTANT
      case symbol_kind::S_MINUS_NUMERICAL_CONSTANT: // MINUS_NUMERICAL_CONSTANT
        value.copy< double > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_array_row: // array_row
        value.copy< std::vector<Scalar> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_array_columns: // array_columns
        value.copy< std::vector<std::vector<Scalar>> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

  }




  template <typename Base>
  Parser::symbol_kind_type
  Parser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  template <typename Base>
  bool
  Parser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  Parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    switch (this->kind ())
    {
      case symbol_kind::S_argument_list: // argument_list
        value.move< ArgumentList > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_expression: // expression
      case symbol_kind::S_array: // array
      case symbol_kind::S_cell_reference: // cell_reference
        value.move< AstNodePtr > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_ERROR_CONSTANT: // ERROR_CONSTANT
        value.move< Error > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_FUNC_START: // FUNC_START
        value.move< FunctionId > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_row: // row
      case symbol_kind::S_column: // column
        value.move< ReferenceTypeAndValue > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_scalar: // scalar
        value.move< Scalar > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_COLUMN: // COLUMN
      case symbol_kind::S_ROW: // ROW
        value.move< SizeType > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_STRING_CONSTANT: // STRING_CONSTANT
        value.move< String > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_LOGICAL_CONSTANT: // LOGICAL_CONSTANT
        value.move< bool > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_NUMERICAL_CONSTANT: // NUMERICAL_CONSTANT
      case symbol_kind::S_PLUS_NUMERICAL_CONSTANT: // PLUS_NUMERICAL_CONSTANT
      case symbol_kind::S_MINUS_NUMERICAL_CONSTANT: // MINUS_NUMERICAL_CONSTANT
        value.move< double > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_array_row: // array_row
        value.move< std::vector<Scalar> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_array_columns: // array_columns
        value.move< std::vector<std::vector<Scalar>> > (YY_MOVE (s.value));
        break;

      default:
        break;
    }

  }

  // by_kind.
  inline
  Parser::by_kind::by_kind () YY_NOEXCEPT
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  inline
  Parser::by_kind::by_kind (by_kind&& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  inline
  Parser::by_kind::by_kind (const by_kind& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {}

  inline
  Parser::by_kind::by_kind (token_kind_type t) YY_NOEXCEPT
    : kind_ (yytranslate_ (t))
  {}



  inline
  void
  Parser::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  inline
  void
  Parser::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  inline
  Parser::symbol_kind_type
  Parser::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }


  inline
  Parser::symbol_kind_type
  Parser::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


#line 10 "lib/code/src/formula.y"
} } // Spreader::FormulaParser
#line 2083 "lib/code/generated/formula-parser.h"


// "%code provides" blocks.
#line 31 "lib/code/src/formula.y"

    
    #define YY_DECL Spreader::FormulaParser::Parser::symbol_type sprflex(/*driver& drv*/yyscan_t yyscanner)
    YY_DECL;


#line 2094 "lib/code/generated/formula-parser.h"


#endif // !YY_SPRF_LIB_CODE_GENERATED_FORMULA_PARSER_H_INCLUDED
