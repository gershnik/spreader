// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include "binary-operators.h"
#include "../execution-context.h"

using namespace Spreader;

template class Spreader::BinaryOperatorNode<ScalarAdd>;
template class Spreader::BinaryOperatorNode<ScalarSubtract>;
template class Spreader::BinaryOperatorNode<ScalarMultiply>;
template class Spreader::BinaryOperatorNode<ScalarDivide>;
template class Spreader::BinaryOperatorNode<ScalarPower>;
template class Spreader::BinaryOperatorNode<ScalarAmpersand>;
template class Spreader::BinaryOperatorNode<ScalarEquals>;
template class Spreader::BinaryOperatorNode<ScalarNotEquals>;
template class Spreader::BinaryOperatorNode<ScalarGreater>;
template class Spreader::BinaryOperatorNode<ScalarGreaterEquals>;
template class Spreader::BinaryOperatorNode<ScalarLess>;
template class Spreader::BinaryOperatorNode<ScalarLessEquals>;


namespace Spreader {
    template<class Op> struct OperatorSymbol;
}

template<> struct Spreader::OperatorSymbol<ScalarAdd>             { static constexpr char32_t value   = U'+';  };
template<> struct Spreader::OperatorSymbol<ScalarSubtract>        { static constexpr char32_t value   = U'-';  };
template<> struct Spreader::OperatorSymbol<ScalarMultiply>        { static constexpr char32_t value   = U'*';  };
template<> struct Spreader::OperatorSymbol<ScalarDivide>          { static constexpr char32_t value   = U'/';  };
template<> struct Spreader::OperatorSymbol<ScalarPower>           { static constexpr char32_t value   = U'^';  };
template<> struct Spreader::OperatorSymbol<ScalarAmpersand>       { static constexpr char32_t value   = U'&';  };
template<> struct Spreader::OperatorSymbol<ScalarEquals>          { static constexpr char32_t value   = U'=';  };
template<> struct Spreader::OperatorSymbol<ScalarNotEquals>       { static constexpr char32_t value[] = U"<>"; };
template<> struct Spreader::OperatorSymbol<ScalarGreater>         { static constexpr char32_t value   = U'>';  };
template<> struct Spreader::OperatorSymbol<ScalarGreaterEquals>   { static constexpr char32_t value[] = U">="; };
template<> struct Spreader::OperatorSymbol<ScalarLess>            { static constexpr char32_t value   = U'<';  };
template<> struct Spreader::OperatorSymbol<ScalarLessEquals>      { static constexpr char32_t value[] = U"<="; };


template<class BinaryOp>
auto BinaryOperatorNode<BinaryOp>::createExecutionStackEntry(ExecutionMemoryResource & mem, TreeTraversalState state) const -> ExecutionStackEntryPtr {
    return ExecutionStackEntryPtr(new (mem) ExecutionStackEntry(this, state));
}

template<class BinaryOp>
auto BinaryOperatorNode<BinaryOp>::onAfterArgument(ExecutionContext & context) const -> TraversalEventOutcome {

    auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
    SPR_ASSERT_LOGIC(entry->handledChildIdx < 2);

    if (entry->handledChildIdx == 0) {
        if (!context.suppressEvaluation) {
            bool res = context.generateScalar(context.returnedValue, [&, entry](const Scalar & val) {
                entry->op.handleFirst(val);
            });
            if (!res)
                return TraversalEventOutcome::Pause;
        }
        
        entry->extent = context.returnedExtent;
    } else {
        if (!context.suppressEvaluation) {
            bool res = context.generateScalar(context.returnedValue, [&, entry](const Scalar & val) {
                entry->op.handleSecond(val);
            });
            if (!res)
                return TraversalEventOutcome::Pause;
        }
        entry->extent.extendTo(context.returnedExtent);
    }

    return TraversalEventOutcome::Continue;
}

template<class BinaryOp>
auto BinaryOperatorNode<BinaryOp>::execute(ExecutionContext & context) const -> bool {
    auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
    if (!context.suppressEvaluation)
        context.returnedValue = std::move(entry->op.result);
    context.returnedExtent = entry->extent;
    return true;
}


template<class BinaryOp>
void BinaryOperatorNode<BinaryOp>::reconstructAfterChild(const ReconstructionContext & /*context*/, uint16_t idx, StringBuilder & dest) const {
    if (idx == 0)
        dest.append(U' ').append(OperatorSymbol<BinaryOp>::value).append(U' ');
}
