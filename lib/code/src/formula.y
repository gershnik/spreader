// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

%require "3.8"
%language "c++"

%define api.value.type variant
%define api.token.constructor
//%define parse.assert - broken on unknown literal characters
%define api.namespace {Spreader::FormulaParser}
%define api.parser.class {Parser} 
%define api.prefix {sprf}
%define api.token.prefix {TOK_}

%parse-param {Builder & builder}
%param       {yyscan_t yyscanner}


%code requires {
    #include <spreader/scalar.h>
    #include <spreader/array.h>
    #include <spreader/reference.h>
    #include <functions/unary-operators.h>
    #include <functions/binary-operators.h>
    #include <functions/true-function.h>
    #include <formula-parser-builder.h>

    typedef void * yyscan_t;
}

%code provides {
    
    #define YY_DECL Spreader::FormulaParser::Parser::symbol_type sprflex(/*driver& drv*/yyscan_t yyscanner)
    YY_DECL;

}

%code {

    #include <spreader/compiler.h>

    using namespace Spreader;

    SPR_CLANG_WARNING_DISABLE("-Wunused-but-set-variable")
}


%start formula

%token <Error>                    ERROR_CONSTANT
%token <bool>                     LOGICAL_CONSTANT
%token <double>                   NUMERICAL_CONSTANT
%token <double>                   PLUS_NUMERICAL_CONSTANT
%token <double>                   MINUS_NUMERICAL_CONSTANT
%token <String>                   STRING_CONSTANT

%token <SizeType>                 COLUMN
%token <SizeType>                 ROW
%token                            OUT_OF_RANGE_COLUMN

%token <FunctionId>               FUNC_START            //func_name(


%left OP_LT OP_GT       // < >
%left OP_GE             // >=
%left OP_LE             // <=
%left OP_NE             // <>
%left OP_EQ             // =
%left OP_AMP            // &
%left OP_ADD OP_SUB     // + -
%left OP_MUL OP_DIV     // * /
%left OP_POW            // ^
%precedence OP_PCT      // %
%precedence UNARY_SIGN  //dummy for %prec below
%precedence PLUS_NUMERICAL_CONSTANT MINUS_NUMERICAL_CONSTANT

%%

formula: expression     { builder.onRootNode(std::move($1));  }

%nterm <AstNodePtr> expression;
expression:  
      '(' expression ')'                        { $2->setBracketed(true); $$ = std::move($2); } 
    | scalar                                    { $$ = AstNodePtr(new ScalarNode         (std::move($1))); }
    | array                                     { $$ = std::move($1); }
    | cell_reference                            { $$ = std::move($1); } 
    | OP_ADD expression  %prec UNARY_SIGN       { $$ = AstNodePtr(new AffirmNode         (std::move($2))); } 
    | OP_SUB expression  %prec UNARY_SIGN       { $$ = AstNodePtr(new NegateNode         (std::move($2))); } 
    | expression OP_POW expression              { $$ = AstNodePtr(new PowerNode          (std::move($1), std::move($3))); } 
    | expression OP_MUL expression              { $$ = AstNodePtr(new MultiplyNode       (std::move($1), std::move($3))); } 
    | expression OP_DIV expression              { $$ = AstNodePtr(new DivideNode         (std::move($1), std::move($3))); } 
    | expression OP_ADD expression              { $$ = AstNodePtr(new AddNode            (std::move($1), std::move($3))); } 
    | expression PLUS_NUMERICAL_CONSTANT        { $$ = AstNodePtr(new AddNode            (std::move($1), AstNodePtr(new ScalarNode($2)))); } 
    | expression OP_SUB expression              { $$ = AstNodePtr(new SubtractNode       (std::move($1), std::move($3))); } 
    | expression MINUS_NUMERICAL_CONSTANT       { $$ = AstNodePtr(new SubtractNode       (std::move($1), AstNodePtr(new ScalarNode($2)))); } 
    | expression OP_AMP expression              { $$ = AstNodePtr(new AmpersandNode      (std::move($1), std::move($3))); }
    | expression OP_EQ expression               { $$ = AstNodePtr(new EqualsNode         (std::move($1), std::move($3))); } 
    | expression OP_NE expression               { $$ = AstNodePtr(new NotEqualsNode      (std::move($1), std::move($3))); } 
    | expression OP_LT expression               { $$ = AstNodePtr(new LessNode           (std::move($1), std::move($3))); } 
    | expression OP_LE expression               { $$ = AstNodePtr(new LessEqualsNode     (std::move($1), std::move($3))); } 
    | expression OP_GT expression               { $$ = AstNodePtr(new GreaterNode        (std::move($1), std::move($3))); } 
    | expression OP_GE expression               { $$ = AstNodePtr(new GreaterEqualsNode  (std::move($1), std::move($3))); } 
    | expression OP_PCT                         { $$ = AstNodePtr(new PercentNode        (std::move($1))); } 
    | FUNC_START argument_list ')'              { auto func = createTrueFunctionNode(*this, $1, std::move($2)); 
                                                  if (!func) {
                                                      YYERROR;
                                                  }
                                                  $$ = std::move(func);
                                                }
    ;

%nterm <Scalar> scalar;
scalar:
      ERROR_CONSTANT                            { $$ = $1; }
    | LOGICAL_CONSTANT                          { $$ = $1; }
    | PLUS_NUMERICAL_CONSTANT                   { $$ = $1; }
    | MINUS_NUMERICAL_CONSTANT                  { $$ = -$1; }
    | NUMERICAL_CONSTANT                        { $$ = $1; }
    | ROW                                       { $$ = double($1 + 1); }
    | STRING_CONSTANT                           { $$ = std::move($1); }
    ;


%nterm <AstNodePtr> array;
array: '{' array_columns '}'                    {  
                                                    const Size size{SizeType($2[0].size()), SizeType($2.size())};
                                                    std::vector<std::vector<Scalar>> & src = $2;
                                                    auto arr = Array::create(size, [&src, size](Scalar * data) {
                                                        Point pt;
                                                        for(pt.y = 0; pt.y < size.height; ++pt.y)
                                                            for(pt.x = 0; pt.x < size.width; ++pt.x)
                                                                new (data + pt.y * size.width + pt.x) Scalar(std::move(src[pt.y][pt.x]));
                                                    });
                                                    $$ = AstNodePtr(new ArrayNode(std::move(arr)));
                                                };

%nterm <std::vector<std::vector<Scalar>>> array_columns;
array_columns:
      array_row                                 { $$.emplace_back(std::move($1)); }
    | array_columns ';' array_row               { 
                                                    if ($1[0].size() != $3.size()) {
                                                        error("row dimensions mismatch");
                                                        YYERROR;
                                                    }
                                                    $1.emplace_back(std::move($3));
                                                    $$ = std::move($1);
                                                }
    ;

%nterm <std::vector<Scalar>> array_row;
array_row: 
      scalar                                    { $$.emplace_back(std::move($1)); }
    | array_row ',' scalar                      { $1.emplace_back(std::move($3)); $$ = std::move($1); }
    ;



%nterm <ReferenceTypeAndValue> row;
row:
      ROW                                       { $$ = ReferenceTypeAndValue{ReferenceType::Relative, $1 - builder.evalPoint().y}; }
    | '$' ROW                                   { $$ = ReferenceTypeAndValue{ReferenceType::Absolute, $2}; }
    ;

%nterm <ReferenceTypeAndValue> column;
column:
      COLUMN                                    { $$ = ReferenceTypeAndValue{ReferenceType::Relative, $1 - builder.evalPoint().x}; }
    | '$' COLUMN                                { $$ = ReferenceTypeAndValue{ReferenceType::Absolute, $2}; }
    | OUT_OF_RANGE_COLUMN                       { error("column out of range"); YYERROR; }
    | '$' OUT_OF_RANGE_COLUMN                   { error("column out of range"); YYERROR; }
    ;

%nterm <AstNodePtr> cell_reference;
cell_reference:
      column row                                { auto idx = builder.addReference(CellReference($1, $2));   
                                                  $$ = AstNodePtr(new ReferenceNode(idx)); }
    | column ':' column                         { auto idx = builder.addReference(ColumnReference($1, $3)); 
                                                  $$ = AstNodePtr(new ReferenceNode(idx)); }
    | row ':' row                               { auto idx = builder.addReference(RowReference($1, $3));    
                                                  $$ = AstNodePtr(new ReferenceNode(idx)); }
    | column row ':' column row                 { auto idx = builder.addReference(AreaReference($1, $2, $4, $5)); 
                                                  $$ = AstNodePtr(new ReferenceNode(idx)); }
    ; 



%nterm <ArgumentList> argument_list;
argument_list:
      %empty                                    {  }
    | expression                                { $$.add(std::move($1)); }
    | argument_list ','                         { $$.add(std::move($1));
                                                  if ($$.size() == 0)
                                                      $$.add(AstNodePtr(new ScalarNode(Scalar::Blank{})));
                                                  $$.add(AstNodePtr(new ScalarNode(Scalar::Blank{})));
                                                }
    | argument_list ',' expression              { $$.add(std::move($1));
                                                  if ($$.size() == 0)
                                                      $$.add(AstNodePtr(new ScalarNode(Scalar::Blank{})));
                                                  $$.add(std::move($3));
                                                }
    ;


%%

void Spreader::FormulaParser::Parser::error(const std::string & ) {

    builder.onSyntaxError();
}
