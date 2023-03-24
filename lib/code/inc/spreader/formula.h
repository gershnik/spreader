// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_FORMULA_H_INCLUDED
#define SPR_HEADER_FORMULA_H_INCLUDED

#include <spreader/types.h>
#include <spreader/ast-node.h>

namespace Spreader {

    class Formula : public ref_counted<Formula>, private FunctionNode {

    friend ref_counted<Formula>;
    friend class FormulaEvaluator;

    public:
        static auto parse(String str, Point at) -> std::pair<refcnt_ptr<Formula>, FormulaReferencesPtr>;

        auto reconstructAt(const ConstFormulaReferencesPtr & refs, Point pt) const -> String;

    private:
        Formula(AstNodePtr && root):
            FunctionNode(ArgumentList(std::move(root))) {
        }

        auto execute(ExecutionContext & context) const -> bool override;
        void reconstructAfterChild(const ReconstructionContext & context, uint16_t idx, StringBuilder & dest) const override;

    };
    using FormulaPtr = refcnt_ptr<Formula>;
    using ConstFormulaPtr = refcnt_ptr<const Formula>;
}

#endif
