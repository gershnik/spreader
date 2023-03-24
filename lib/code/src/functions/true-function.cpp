// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include "true-function.h"
#include "mini-trie.h"

namespace Spreader {
    
    template<FunctionId Id>
    auto createTrueFunctionNode(FormulaParser::Parser & parser, ArgumentList && args) -> AstNodePtr;
    
    template<FunctionId Id>
    extern const char * FunctionPrefix;
    
    template<FunctionId ...ids>
    class TrueFunctionNodeFactoryImpl {
    private:
        using CreatorPtr = auto (*)(FormulaParser::Parser &, ArgumentList && args) -> AstNodePtr;
    public:
        SPR_WARNING_PUSH
        SPR_CLANG_WARNING_DISABLE("-Wundefined-var-template") //stupid clang doesn't realize FunctionPrefix variable is extern
        TrueFunctionNodeFactoryImpl() noexcept:
            m_creators{createTrueFunctionNode<ids>...},
            m_prefixParser({FunctionPrefix<ids>...})
        {}
        SPR_WARNING_POP
        
        auto prefixToId(const char * prefix) const -> std::optional<FunctionId> {
            char ucase[32];
            unsigned len = 0;
            for( ; ; ) {
                if (len == std::size(ucase))
                    return std::nullopt;
                char c = prefix[len];
                if (!c)
                    break;
                if (c >= 'a' && c <= 'z')
                    ucase[len] = (c - 'a') + 'A';
                else
                    ucase[len] = c;
                ++len;
            }
            auto cur = ucase;
            auto last = ucase + len;
            auto res = m_prefixParser.prefixMatch(cur, last);
            if (res == m_prefixParser.noMatch || cur != last)
                return std::nullopt;
            return FunctionId(res);
        }
        
        auto create(FormulaParser::Parser & parser, FunctionId id, ArgumentList && args) const -> AstNodePtr {
            unsigned idx = static_cast<unsigned>(id);
            SPR_ASSERT_INPUT(idx < std::size(m_creators));

            return m_creators[idx](parser, std::move(args));
        }
        
    private:
        CreatorPtr m_creators[sizeof...(ids)];
        MiniTrie<char, uint16_t> m_prefixParser;
    };

    template<unsigned ...ids>
    auto dummyMakeTrueFunctionNodeFactory(std::integer_sequence<unsigned, ids...>) -> TrueFunctionNodeFactoryImpl<FunctionId(ids)...>;

    using TrueFunctionNodeFactory = decltype(dummyMakeTrueFunctionNodeFactory(std::make_integer_sequence<unsigned, unsigned(FunctionId::NumberOfFunctions)>{}));
    
    
    static const TrueFunctionNodeFactory g_trueFunctionNodeFactory;
    
    auto functionPrefixToId(const char * prefix) -> std::optional<FunctionId> {
        return g_trueFunctionNodeFactory.prefixToId(prefix);
    }
    
    auto createTrueFunctionNode(FormulaParser::Parser & parser, FunctionId id, ArgumentList && args) -> AstNodePtr {
        return g_trueFunctionNodeFactory.create(parser, id, std::move(args));
    }
    
    void TrueFunctionNodeBase::reconstructAfterChild(const ReconstructionContext & /*context*/, uint16_t idx, StringBuilder & dest) const {
        
        if (idx < this->childrenCount() - 1)
            dest.append(U',').append(' ');
    }
    
    void TrueFunctionNodeBase::reconstructSuffix(const ReconstructionContext & /*context*/, StringBuilder & dest) const {
        dest.append(U')');
    }
    
}
