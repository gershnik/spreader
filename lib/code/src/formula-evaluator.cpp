// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include "formula-evaluator.h"

using namespace Spreader;

FormulaEvaluator::FormulaEvaluator(AstNode::ExecutionMemoryResource & memory,
                                   CellGrid & grid,
                                   NameManager & names,
                                   const ConstFormulaPtr & formula,
                                   const ConstFormulaReferencesPtr & refs,
                                   Point at,
                                   bool generation):
    m_traversal(*formula, memory),
    m_context(grid, names, refs.get(), at, generation)
{}

auto FormulaEvaluator::eval() -> bool {
    
    if (m_hasFullResult) {
        
        bool res = m_context.generateScalar(m_context.returnedValue, [&](const Scalar & scalar) {
            m_currentValue = scalar;
        });
        return res || m_context.circularDependency;
    }

    auto res = m_traversal.traverse([this](auto && event){

        using EventType = std::remove_cvref_t<decltype(event)>;

        if constexpr (std::is_same_v<EventType, Traversal::BeforeChild>) {

            SPR_ASSERT_LOGIC(dynamic_cast<const FunctionNode *>(event.stackEntry->node));
            auto function = static_cast<const FunctionNode *>(event.stackEntry->node);
            auto * functionEntry = static_cast<FunctionNode::ExecutionStackEntry *>(event.stackEntry);
            if (functionEntry->handledChildIdx == 0) {
                m_context.stackPointer = functionEntry;
                function->onBeforeArguments(m_context);
            }
            return TraversalEventOutcome::Continue;

        } else if constexpr (std::is_same_v<EventType, Traversal::AfterChild>) {

            SPR_ASSERT_LOGIC(dynamic_cast<const FunctionNode *>(event.stackEntry->node));
            auto function = static_cast<const FunctionNode *>(event.stackEntry->node);
            auto * functionEntry = static_cast<FunctionNode::ExecutionStackEntry *>(event.stackEntry);

            m_context.stackPointer = static_cast<AstNode::ExecutionStackEntry *>(event.stackEntry);
            auto outcome = function->onAfterArgument(m_context);
            if (bool(outcome & TraversalEventOutcome::Continue)) {
                
                auto skipNext = uint16_t(outcome & ~TraversalEventOutcome::Continue);
                if (function->childrenCount() - functionEntry->handledChildIdx <= skipNext + 1)
                    functionEntry->handledChildIdx = function->childrenCount();
                else
                    functionEntry->handledChildIdx += skipNext + 1;
            }/* else if (outcome == TraversalEventOutcome::RestartChildren) {
                functionEntry->handledChildIdx = 0;
            }*/

            return outcome;

        } else if constexpr (std::is_same_v<EventType, Traversal::Exit>) {

            m_context.stackPointer = static_cast<AstNode::ExecutionStackEntry *>(event.stackEntry);
            if (!event.stackEntry->node->execute(m_context))
                return TraversalEventOutcome::Pause;
            return TraversalEventOutcome::Continue;
        } else {
            return TraversalEventOutcome::Continue;
        }
    });
    
    if (res == Traversal::Done) {
        
        bool available = m_context.generateScalar(m_context.returnedValue, [&](const Scalar & scalar, bool isSingleItem) {
            m_currentValue = scalar;
            m_hasFullResult = !isSingleItem;
        });
        return available || m_context.circularDependency;
    }
    
    return m_context.circularDependency;
}

