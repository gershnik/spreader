// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include <formula-parser.h>
#include <formula-lexer.h>

#include <spreader/formula.h>

#include "execution-context.h"

using namespace Spreader;

namespace Spreader::FormulaParser {
    
    class StringScanner {
    public:
        StringScanner(const String & str): m_stringAccess(str) {
            if (sprflex_init(&m_scanner) != 0) {
                int err = errno;
                if (err == ENOMEM)
                    throw std::bad_alloc();
                std::terminate();
            }
            m_buffer = sprf_scan_string(m_stringAccess.c_str(), m_scanner);
            if (!m_buffer)
                std::terminate();
            
            sprf_switch_to_buffer(m_buffer, m_scanner);
        }
        ~StringScanner() {
            
            if (m_scanner) {
                if (m_buffer)
                    sprf_delete_buffer(m_buffer, m_scanner);
                sprflex_destroy(m_scanner);
            }
                
        }
        StringScanner(const StringScanner &) = delete;
        auto operator=(const StringScanner &) -> StringScanner & = delete;
        StringScanner(StringScanner && src) noexcept = delete;
        auto operator=(StringScanner && src) -> StringScanner & = delete;
        
        auto get() const -> yyscan_t {
            return m_scanner;
        }
    private:
        String::char_access m_stringAccess;
        yyscan_t m_scanner = nullptr;
        YY_BUFFER_STATE m_buffer = nullptr;
    };
}

auto Formula::parse(String str, Point at) -> std::pair<FormulaPtr, FormulaReferencesPtr> {

    FormulaParser::Builder builder(str, at);

    FormulaParser::StringScanner scanner(str);
    FormulaParser::Parser parser(builder, scanner.get());
    parser();
    auto res = builder.build();
    return {refcnt_attach(new Formula(std::move(res.first))), std::move(res.second)};
}

auto Formula::execute(ExecutionContext & context) const -> bool {

    if ((context.returnedExtent.width > 1 || context.returnedExtent.height > 1) && context.offset == Point{0, 0}) {
        context.returnedExtent = Size {
            std::min(context.grid().maxSize().width - context.at().x, context.returnedExtent.width),
            std::min(context.grid().maxSize().height - context.at().y, context.returnedExtent.height)
        };
        Point pt;
        for (pt.y = context.at().y; pt.y < context.at().y + context.returnedExtent.height; ++pt.y) {
            for (pt.x = context.at().x; pt.x < context.at().x + context.returnedExtent.width; ++pt.x) {
                auto * existing = context.grid().getCell(pt);
                bool isSpill = applyToCell(existing, [&context](auto ptr) {
                    using T = std::remove_cv_t<std::remove_pointer_t<decltype(ptr)>>;

                    if constexpr (std::is_same_v<T, ValueCell>) {
                        return !ptr->isBlank();
                    } else if constexpr (std::is_same_v<T, FormulaCell>) {
                        return false;
                    } else if constexpr (std::is_same_v<T, FormulaCellExtension>) {
                        return ptr->parent()->location() != context.at();
                    } else {
                        return false;
                    }
                });
                if (isSpill) {
                    context.returnedValue = Error::Spill;
                    context.returnedExtent = Size{1, 1};
                    return true;
                }
            }
        }
    }
    
    return true;
}

void Formula::reconstructAfterChild(const ReconstructionContext & /*context*/, uint16_t /*idx*/, StringBuilder & /*dest*/) const {
}

auto Formula::reconstructAt(const ConstFormulaReferencesPtr & refs, Point pt) const -> String {

    using Traversal = AstReconstructionTraversal;

    StringBuilder builder;
    Traversal traversal(*this);
    ReconstructionContext context {
        .references = refs,
        .at = pt
    };
    [[maybe_unused]]
    auto res = traversal.traverse([&context, &builder](auto && event){ 

        using EventType = std::remove_cvref_t<decltype(event)>;

        if constexpr (std::is_same_v<EventType, typename Traversal::Enter>) {
            if (event.stackEntry->node->isBracketed())
                builder.append(U'(');
            event.stackEntry->node->reconstructPrefix(context, builder);
        } else if constexpr (std::is_same_v<EventType, typename Traversal::AfterChild>) {
            SPR_ASSERT_LOGIC(dynamic_cast<const FunctionNode *>(event.stackEntry->node));
            auto function = static_cast<const FunctionNode *>(event.stackEntry->node);
            auto * functionEntry = static_cast<FunctionNode::ReconstructionStackEntry *>(event.stackEntry);

            function->reconstructAfterChild(context, functionEntry->handledChildIdx++, builder);
        } else if constexpr (std::is_same_v<EventType, typename Traversal::Exit>) {
            event.stackEntry->node->reconstructSuffix(context, builder);
            if (event.stackEntry->node->isBracketed())
                builder.append(U')');
        }
        return TraversalEventOutcome::Continue;
    });
    SPR_ASSERT_LOGIC(res == Traversal::Done);
    return builder.build();
}
