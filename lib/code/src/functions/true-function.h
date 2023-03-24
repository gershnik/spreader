// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_TRUE_FUNCTION_H_INCLUDED
#define SPR_HEADER_TRUE_FUNCTION_H_INCLUDED

#include <spreader/ast-node.h>

namespace Spreader {
    
    namespace FormulaParser {
        class Parser;
    }

    enum class FunctionId : unsigned {
        Address,
        And,
        Average,
        AverageA,
        AverageIf,
        Ceil,
        Choose,
        Column,
        Concat,
        Concatenate,
        Count,
        CountA,
        Date,
        DateDif,
        Day,
        Days,
        EDate,
        EOMonth,
        ErrorType,
        Find,
        Floor,
        HLookup,
        Hour,
        If,
        Index,
        Indirect,
        Int,
        IsBlank,
        IsErr,
        IsError,
        IsEven,
        IsLogical,
        IsNA,
        IsNonText,
        IsNumber,
        IsOdd,
        IsoWeekNum,
        IsText,
        Left,
        Len,
        Lower,
        Match,
        Max,
        MaxA,
        Mid,
        Min,
        MinA,
        Minute,
        Mod,
        Month,
        MRound,
        Not,
        Now,
        Or,
        Replace,
        Right,
        Round,
        RoundDown,
        RoundUp,
        Row,
        Second,
        Sign,
        StDev_P,
        StDev_S,
        StDev,
        StDevA,
        StDevP,
        StDevPA,
        Substitute,
        Sum,
        SumIf,
        Switch,
        Time,
        Today,
        Transpose,
        Trim,
        Upper,
        VLookup,
        Weekday,
        WeekNum,
        XOr,
        Year,
        
        
        NumberOfFunctions
    };

    auto functionPrefixToId(const char * prefix) -> std::optional<FunctionId>;
    
    auto createTrueFunctionNode(FormulaParser::Parser & parser, FunctionId id, ArgumentList && args) -> AstNodePtr;
    
    class TrueFunctionNodeBase : public FunctionNode {
    public:
        void reconstructAfterChild(const ReconstructionContext & context, uint16_t idx, StringBuilder & dest) const override;
        void reconstructSuffix(const ReconstructionContext & context, StringBuilder & dest) const override;

    protected:
        TrueFunctionNodeBase(ArgumentList && args) noexcept:
            FunctionNode(std::move(args)) {
        }
        ~TrueFunctionNodeBase() noexcept = default;
    };
    

}


#endif 
