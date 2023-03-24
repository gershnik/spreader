// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_UNARY_OPERATORS_H_INCLUDED
#define SPR_UNARY_OPERATORS_H_INCLUDED

#include <spreader/ast-node.h>

#include "../scalar-numeric-functions.h"

namespace Spreader {

    template<class BinaryOp>
    class UnaryOperatorNode final : public FunctionNode {

    private:
        struct ExecutionStackEntry : public FunctionNode::ExecutionStackEntry {
            using FunctionNode::ExecutionStackEntry::ExecutionStackEntry;

            Scalar value;
        };
    public:
        UnaryOperatorNode(AstNodePtr && arg) noexcept:
            FunctionNode(ArgumentList(std::move(arg))) {
        }

        auto createExecutionStackEntry(ExecutionMemoryResource & mem, TreeTraversalState state) const -> ExecutionStackEntryPtr override;
        auto onAfterArgument(ExecutionContext & context) const -> TraversalEventOutcome override;
        auto execute(ExecutionContext & context) const -> bool override;
        
        void reconstructPrefix(const ReconstructionContext & context, StringBuilder & dest) const override;
        void reconstructSuffix(const ReconstructionContext & context, StringBuilder & dest) const override;
        void reconstructAfterChild(const ReconstructionContext & context, uint16_t idx, StringBuilder & dest) const override;
    private:
        ~UnaryOperatorNode() noexcept = default;
    };

    using NegateNode =  UnaryOperatorNode<ScalarNegate>;
    using AffirmNode =  UnaryOperatorNode<ScalarAffirm>;
    using PercentNode = UnaryOperatorNode<ScalarPercent>;

}


#endif
