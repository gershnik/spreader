// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_TRUE_FUNCTION_CREATION_H_INCLUDED
#define SPR_HEADER_TRUE_FUNCTION_CREATION_H_INCLUDED

#include "true-function.h"

#include <formula-parser.h>

namespace Spreader {
    
    template<FunctionId Id> class TrueFunctionNode;
    
    template<FunctionId Id> const char * FunctionPrefix;

    template<FunctionId Id>
    auto createTrueFunctionNode(FormulaParser::Parser & parser, ArgumentList && args) -> AstNodePtr {
        
        if constexpr (TrueFunctionNode<Id>::minArgs > 0) {
            if (args.size() < TrueFunctionNode<Id>::minArgs) {
                parser.error("too few arguments");
                return nullptr;
            }
        }
        if constexpr (TrueFunctionNode<Id>::maxArgs < UINT_MAX) {
            if (args.size() > TrueFunctionNode<Id>::maxArgs) {
                parser.error("too many arguments");
                return nullptr;
            }
        }
        return AstNodePtr(new TrueFunctionNode<Id>(std::move(args)));
    }
    
    #define SPR_MAP_FUNCTION_ID_TO_IMPL(id, prefix, mn, mx, type) \
        template<> class TrueFunctionNode<id> final : public type { \
        public:\
            static constexpr unsigned minArgs = mn; \
            static constexpr unsigned maxArgs = mx; \
            TrueFunctionNode(ArgumentList && args) noexcept: \
                type(std::move(args)) { \
            }\
            void reconstructPrefix(const ReconstructionContext &, StringBuilder & dest) const override { \
                dest.append(U##prefix "("); \
            }\
            auto createExecutionStackEntry(ExecutionMemoryResource & mem, TreeTraversalState state) const -> ExecutionStackEntryPtr override {\
                return ExecutionStackEntryPtr(new (mem) ExecutionStackEntry(this, state)); \
            }\
        private:\
            ~TrueFunctionNode() noexcept = default; \
        };\
        template auto createTrueFunctionNode<id>(FormulaParser::Parser & parser, ArgumentList && args) -> AstNodePtr; \
        template<> const char * FunctionPrefix<id> = prefix "(";
}

#endif
