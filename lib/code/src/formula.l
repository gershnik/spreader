/* Copyright (c) 2022, Eugene Gershnik
 * SPDX-License-Identifier: BSD-3-Clause
 */

%option 8bit reentrant noyywrap batch warn 
%option case-insensitive
%option nodefault
%option prefix="sprf"
%option noinput nounput

%top{
    #include "formula-parser.h"
    #include <spreader/floating-decimal.h>
    #include <spreader/compiler.h>

    using namespace Spreader;
    using namespace Spreader::FormulaParser;

    SPR_CLANG_WARNING_DISABLE("-Wsign-compare")
    SPR_CLANG_WARNING_DISABLE("-Wmisleading-indentation")
    SPR_MSVC_WARNING_DISABLE(4018)

    #if !__has_include(<unistd.h>)
        #define YY_NO_UNISTD_H
    #endif
}

%{
    #include <spreader/cell-grid.h>

    static auto parseColumn(const char * str) -> std::optional<SizeType>;
    static auto parseRow(const char * str) -> std::optional<SizeType>;
    static auto parseDouble(const char * str) -> std::optional<double>;
%}

whitespace      [ \t\n]
digit           [0-9] 
nzdigit         [1-9]

sign_opt        [+-]?
dseq            ({digit}+)
dseq_opt        ({digit}*)
frac            (({dseq_opt}"."{dseq})|{dseq}"."?)
exp             (E{sign_opt}{dseq})
exp_opt         ({exp}?)
number          (({frac}{exp_opt})|({dseq}{exp}))

quot            "\""
notquot         [^"]
doublequot      ({quot}{quot})
%x IN_STRING


/*column       ([A-Z]{1,2}|[A-W][A-Z]{1,2}|"X"[A-E][A-Z]|"XF"[A-D])*/
column          ([A-Z]+)
row             ({nzdigit}{digit}*)

func_prefix     [A-Z][A-Z.]*\(


%%

%{
    using namespace Spreader;
    StringBuilder stringBuilder;
%}

{whitespace}+               {};

"<"                         return Parser::make_OP_LT();
">"                         return Parser::make_OP_GT();
">="                        return Parser::make_OP_GE();
"<="                        return Parser::make_OP_LE();
"<>"                        return Parser::make_OP_NE();
"="                         return Parser::make_OP_EQ();
"&"                         return Parser::make_OP_AMP();
"+"                         return Parser::make_OP_ADD();
"-"                         return Parser::make_OP_SUB();
"*"                         return Parser::make_OP_MUL();
"/"                         return Parser::make_OP_DIV();
"^"                         return Parser::make_OP_POW();
"%"                         return Parser::make_OP_PCT();

"TRUE"                      return Parser::make_LOGICAL_CONSTANT(true);
"FALSE"                     return Parser::make_LOGICAL_CONSTANT(false);

"#DIV/0!"                   return Parser::make_ERROR_CONSTANT(Error::DivisionByZero);
"#N/A"                      return Parser::make_ERROR_CONSTANT(Error::InvalidArgs); 
"#NAME?"                    return Parser::make_ERROR_CONSTANT(Error::InvalidName);
"#NULL!"                    return Parser::make_ERROR_CONSTANT(Error::NullRange);
"#NUM!"                     return Parser::make_ERROR_CONSTANT(Error::NotANumber); 
"#REF!"                     return Parser::make_ERROR_CONSTANT(Error::InvalidReference); 
"#VALUE!"                   return Parser::make_ERROR_CONSTANT(Error::InvalidValue);
"#SPILL!"                   return Parser::make_ERROR_CONSTANT(Error::Spill);
"#GETTING_DATA"             return Parser::make_ERROR_CONSTANT(Error::GettingData);

{quot}                      BEGIN(IN_STRING); stringBuilder.clear();
<IN_STRING>{
    {doublequot}            stringBuilder.append(U'"');
    {notquot}+              stringBuilder.append(yytext, yyleng);
    {quot}                  BEGIN(INITIAL); return Parser::make_STRING_CONSTANT(stringBuilder.build());
}

{column}                    {
                                if (auto res = parseColumn(yytext))
                                    return Parser::make_COLUMN(*res);
                                return Parser::make_OUT_OF_RANGE_COLUMN();
                            }
{row}                       {
                                if (auto res = parseRow(yytext))
                                    return Parser::make_ROW(*res);
                                if (auto res = parseDouble(yytext))
                                    return Parser::make_NUMERICAL_CONSTANT(*res);
                                REJECT;
                            }

{number}                    {
                                if (auto res = parseDouble(yytext))
                                    return Parser::make_NUMERICAL_CONSTANT(*res);
                                REJECT;
                            }
"+"{number}                 {
                                if (auto res = parseDouble(yytext + 1))
                                    return Parser::make_PLUS_NUMERICAL_CONSTANT(*res);
                                REJECT;
                            }
"-"{number}                 {
                                if (auto res = parseDouble(yytext + 1))
                                    return Parser::make_MINUS_NUMERICAL_CONSTANT(*res);
                                REJECT;
                            }


{func_prefix}               {
                                if (auto id = functionPrefixToId(yytext))
                                    return Parser::make_FUNC_START(*id);
                                REJECT;
                            }

.                           return Parser::symbol_type(yytext[0]);

%%

static auto parseDouble(const char * str) -> std::optional<double> {
    
    FloatingDecimal::Builder builder;
    auto c = *str;
    for (;;) {
        if (c == '.') {
            builder.markDecimalPoint();
        } else if (c == 'e' || c == 'E') {
            break;
        } else {
            if (!builder.addMantissaDigit(c - '0'))
                return std::nullopt;
        }
        c = *++str;
        if (!c)
            return builder.build().toDouble();
    }

    c = *++str;
    if (c == '-' || c == '+') {
        if (c == '-')
            builder.setExponentNegative();
        c = *++str;
    }
    for(;;) {
        if (!builder.addExponentDigit(c - '0'))
            return std::nullopt;
        c = *++str;
        if (!c)
            return builder.build().toDouble();
    }
}

static auto parseColumn(const char * str) -> std::optional<SizeType> {

    SizeType value = 0;
    for( ; *str; ++str) {
        auto c = *str;
        SizeType digit = (c >= 'A' ? c - 'A' : c - 'a') + 1;
        if ((CellGrid::maxSize().width - digit) / 26 < value) {
            return std::nullopt;
        }
        value = (value * 26) + digit;
    }
    SPR_ASSERT_LOGIC(value > 0);
    return value - 1;
}

static auto parseRow(const char * str) -> std::optional<SizeType> {

    SizeType value = 0;
    for( ; *str; ++str) {
        SizeType digit = *str - '0';
        if ((CellGrid::maxSize().height - digit) / 10 < value) {
            return std::nullopt;
        }
        value = (value * 10) + digit;
    }
    SPR_ASSERT_LOGIC(value > 0);
    return value - 1;
}
