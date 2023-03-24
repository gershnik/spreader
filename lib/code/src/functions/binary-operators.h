// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_BINARY_OPERATIONS_H_INCLUDED
#define SPR_BINARY_OPERATIONS_H_INCLUDED

#include <spreader/ast-node.h>

#include "../scalar-numeric-functions.h"
#include "../scalar-string-functions.h"

namespace Spreader {

    template<class BinaryOp>
    class BinaryOperatorNode final : public FunctionNode {
    private:
        struct ExecutionStackEntry : public FunctionNode::ExecutionStackEntry {
            using FunctionNode::ExecutionStackEntry::ExecutionStackEntry;

            BinaryOp op;
            Size extent;
        };
    public:
        BinaryOperatorNode(AstNodePtr && lhs, AstNodePtr && rhs) noexcept:
            FunctionNode(ArgumentList(std::move(lhs), std::move(rhs))) {
        }

        auto createExecutionStackEntry(ExecutionMemoryResource & mem, TreeTraversalState state) const -> ExecutionStackEntryPtr override;
        auto onAfterArgument(ExecutionContext & context) const -> TraversalEventOutcome override;
        auto execute(ExecutionContext & context) const -> bool override;
        
        void reconstructAfterChild(const ReconstructionContext & context, uint16_t idx, StringBuilder & dest) const override;
    private:
        ~BinaryOperatorNode() noexcept = default;
    };

    using AddNode =             BinaryOperatorNode<ScalarAdd>;
    using SubtractNode =        BinaryOperatorNode<ScalarSubtract>;
    using MultiplyNode =        BinaryOperatorNode<ScalarMultiply>;
    using DivideNode =          BinaryOperatorNode<ScalarDivide>;
    using PowerNode =           BinaryOperatorNode<ScalarPower>;
    using AmpersandNode =       BinaryOperatorNode<ScalarAmpersand>;
    using EqualsNode =          BinaryOperatorNode<ScalarEquals>;
    using NotEqualsNode =       BinaryOperatorNode<ScalarNotEquals>;
    using GreaterNode =         BinaryOperatorNode<ScalarGreater>;
    using GreaterEqualsNode =   BinaryOperatorNode<ScalarGreaterEquals>;
    using LessNode =            BinaryOperatorNode<ScalarLess>;
    using LessEqualsNode =      BinaryOperatorNode<ScalarLessEquals>;

}

#endif 
