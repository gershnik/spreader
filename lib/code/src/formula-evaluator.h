// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_FORMULA_EVALUATOR_H_INCLUDED
#define SPR_HEADER_FORMULA_EVALUATOR_H_INCLUDED

#include <spreader/formula.h>

#include "execution-context.h"

#include <optional>

namespace Spreader {

    class FormulaEvaluator {

    private:
        using Traversal = AstExecutionTraversal;

    public:
        FormulaEvaluator(AstNode::ExecutionMemoryResource & memory, 
                         CellGrid & grid, 
                         NameManager & names,
                         const ConstFormulaPtr & formula, 
                         const ConstFormulaReferencesPtr & refs,
                         Point at,
                         bool generation);

        auto eval() -> bool;
        
        void setDependencyHandler(ExecutionContext::DependencyHandler * depHandler) {
            m_context.setDependencyHandler(depHandler);
        }

        auto isCircularDependency() const -> bool {
            return m_context.circularDependency;
        }

        auto result() const noexcept -> const Scalar & {
            return m_currentValue;
        }
        auto result() noexcept -> Scalar & {
            return m_currentValue;
        }
        auto extent() const noexcept -> Size {
            return m_context.returnedExtent;
        }
        
        auto offset() const noexcept -> Point {
            return m_context.offset;
        }
        
        
        auto nextOffset() noexcept -> bool {
            if (!m_context.incrementOffset(m_context.returnedExtent))
                return false;
            SPR_ASSERT_LOGIC(m_context.suppressEvaluation == false);
            SPR_ASSERT_LOGIC(m_context.circularDependency == false);
            if (!m_hasFullResult)
                m_traversal.reset();
            return true;
        }
    private:
        Traversal m_traversal;
        ExecutionContext m_context;
        Scalar m_currentValue;
        bool m_hasFullResult = false;
    };
    
    static_assert(std::is_move_assignable_v<FormulaEvaluator> && std::is_move_constructible_v<FormulaEvaluator>,
                  "FormulaEvaluator must be movable");
}

#endif
