// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include "unary-operators.h"
#include "../execution-context.h"

using namespace Spreader;

template class Spreader::UnaryOperatorNode<ScalarNegate>;
template class Spreader::UnaryOperatorNode<ScalarAffirm>;
template class Spreader::UnaryOperatorNode<ScalarPercent>;

namespace Spreader {
    template<class Op> struct OperatorSymbol;
}

template<> struct Spreader::OperatorSymbol<ScalarNegate>  { static constexpr char32_t value = U'-';  };
template<> struct Spreader::OperatorSymbol<ScalarAffirm>  { static constexpr char32_t value = U'+';  };
template<> struct Spreader::OperatorSymbol<ScalarPercent> { static constexpr char32_t value = U'%';  };


template<class UnaryOp>
auto UnaryOperatorNode<UnaryOp>::createExecutionStackEntry(ExecutionMemoryResource & mem, TreeTraversalState state) const -> ExecutionStackEntryPtr {
    return ExecutionStackEntryPtr(new (mem) ExecutionStackEntry(this, state));
}


template<class UnaryOp>
auto UnaryOperatorNode<UnaryOp>::onAfterArgument(ExecutionContext & context) const -> TraversalEventOutcome {
    auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
    SPR_ASSERT_LOGIC(entry->handledChildIdx == 0);

    if (!context.suppressEvaluation) {
        bool res = context.generateScalar(context.returnedValue, [&, entry](const Scalar & val) {
            entry->value = UnaryOp().handleArg(val);
        });
        if (!res)
            return TraversalEventOutcome::Pause;
    }
    return TraversalEventOutcome::Continue;
}

template<class UnaryOp>
auto UnaryOperatorNode<UnaryOp>::execute(ExecutionContext & context) const -> bool {
    auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
    if (!context.suppressEvaluation)
        context.returnedValue = std::move(entry->value);
    //just propagate returnedExtent whatever it happens to be
    return true;
}


template<class UnaryOp>
void UnaryOperatorNode<UnaryOp>::reconstructPrefix(const ReconstructionContext & /*context*/, StringBuilder & dest) const {
    if constexpr (UnaryOp::isPrefix) {
        dest.append(OperatorSymbol<UnaryOp>::value);
    }
}

template<class UnaryOp>
void UnaryOperatorNode<UnaryOp>::reconstructSuffix(const ReconstructionContext & /*context*/, StringBuilder & dest) const {
    if constexpr (!UnaryOp::isPrefix) {
        dest.append(OperatorSymbol<UnaryOp>::value);
    }
}

template<class UnaryOp>
void UnaryOperatorNode<UnaryOp>::reconstructAfterChild(const ReconstructionContext & /*context*/, [[maybe_unused]] uint16_t idx, StringBuilder & /*dest*/) const {
    SPR_ASSERT_LOGIC(idx == 0);
}
