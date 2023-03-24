// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include "true-function-creation.h"
#include "../execution-context.h"
#include <spreader/name-manager.h>

namespace Spreader {

    class FunctionIndirect : public TrueFunctionNodeBase {
        using Super = TrueFunctionNodeBase;

    protected:
        struct ExecutionStackEntry : Super::ExecutionStackEntry {
            using Super::ExecutionStackEntry::ExecutionStackEntry;

            Point originalOffset;
            bool originalSuppressEvaluation;
        };

        FunctionIndirect(ArgumentList && args) noexcept:
            Super(std::move(args)) {
        }
        ~FunctionIndirect() noexcept = default;
    public:

        void onBeforeArguments(ExecutionContext & context) const override {
            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
            entry->originalSuppressEvaluation = context.suppressEvaluation;
            context.suppressEvaluation = false; //we must evaluate to find out size!
            entry->originalOffset = context.offset;
            context.offset = Point{0, 0}; //we must evalaute initial arg!
        }

        auto onAfterArgument(ExecutionContext & context) const -> TraversalEventOutcome override {

            //auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
            
            if (context.returnedExtent != Size{1, 1}) {
                //This is what Excel does and this is what we have no other choice but to do too!
                //Parsing of a calar argument can produce a rectangle and this is what we use 
                //for the resultant array. This doesn't mesh with using arguments extent at all.
                context.returnedValue = Error::InvalidValue;
                return TraversalEventOutcome::Continue;
            }
            
            auto res = context.generateScalar(context.returnedValue, [&] (const Scalar & scalar) {
                
                applyVisitorCoercedTo<String>([&](const auto & val){
                    
                    using T = std::remove_cvref_t<decltype(val)>;
                    
                    if constexpr (std::is_same_v<T, String>) {
                        
                        if (auto rect = context.names().parseArea(val, context.grid().size())) {
                            if (rect->size != Size{1, 1})
                                context.returnedValue = *rect;
                            else
                                context.returnedValue = rect->origin;
                            context.returnedExtent = rect->size;
                            return;
                        }
                        
                    }
                    context.returnedValue = Error::InvalidReference;
                    context.returnedExtent = Size{1, 1};
                    
                }, scalar);
            });
            if (!res)
                return TraversalEventOutcome::Pause;
            return TraversalEventOutcome::Continue;
        }

        auto execute(ExecutionContext & context) const -> bool override {

            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);

            context.suppressEvaluation = entry->originalSuppressEvaluation;
            context.offset = entry->originalOffset;

            return true;
        }

    };
    
    SPR_MAP_FUNCTION_ID_TO_IMPL(FunctionId::Indirect, "INDIRECT", 1, 1, FunctionIndirect);

}
