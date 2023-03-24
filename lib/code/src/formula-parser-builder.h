// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_FORMULA_PARSER_BUILDER_H_INCLUDED
#define SPR_FORMULA_PARSER_BUILDER_H_INCLUDED

#include <spreader/ast-node.h>
#include <spreader/formula-references.h>

#include <vector>

namespace Spreader::FormulaParser {

    class Builder {

    public:
        Builder(const String & str, Point at):
            m_original(str),
            m_evalPoint(at) {
        }

        auto evalPoint() const noexcept -> const Point & {
            return m_evalPoint;
        } 

        template<class RefType>
        auto addReference(const RefType & ref) -> size_t {
            return m_referencesBuilder.add(ref);
        }
        
        void onSyntaxError() {
            m_referencesBuilder.reset();
            m_root.reset(new ParseErrorNode(Error::InvalidFormula, m_original));
        }
        void onRootNode(AstNodePtr && node) {
            m_root = std::move(node);
        }
        auto build() -> std::pair<AstNodePtr, FormulaReferencesPtr> {
            return {std::move(m_root), m_referencesBuilder.build()};
        }

    private:
        const String & m_original;
        const Point m_evalPoint;
        AstNodePtr m_root;
        FormulaReferencesBuilder m_referencesBuilder;
    };


}

#endif
