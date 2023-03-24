// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include "true-function-creation.h"
#include "../execution-context.h"

namespace Spreader {

    class FunctionIf : public TrueFunctionNodeBase {
        using Super = TrueFunctionNodeBase;

    protected:
        struct ExecutionStackEntry : Super::ExecutionStackEntry {
            using Super::ExecutionStackEntry::ExecutionStackEntry;

            bool originalSuppressEvaluation;
            decltype(handledChildIdx) childIndexToUse = 0;
            ScalarGenerator value;
            Size extent;
        };

        FunctionIf(ArgumentList && args) noexcept:
            Super(std::move(args)) {
        }
        ~FunctionIf() noexcept = default;
    public:
        void onBeforeArguments(ExecutionContext & context) const override {
            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
            entry->originalSuppressEvaluation = context.suppressEvaluation;
        }
        
        auto onAfterArgument(ExecutionContext & context) const -> TraversalEventOutcome override {

            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
            SPR_ASSERT_LOGIC(entry->handledChildIdx < 3);

            if (entry->handledChildIdx == 0) {
                
                if (!context.suppressEvaluation) {
                    bool res = context.generateScalar(context.returnedValue, [&, entry](const Scalar & scalar) {
                        
                        applyVisitorCoercedTo<bool>([&](auto val)  {
                            
                            using T = std::remove_cvref_t<decltype(val)>;
                            
                            if constexpr (std::is_same_v<T, bool>) {
                                entry->childIndexToUse = 2 - val;
                            } else if constexpr (std::is_same_v<T, Error>) {
                                entry->value = val;
                            }
                            
                        }, scalar);
                        
                        if (entry->childIndexToUse != 1)
                            context.suppressEvaluation = true;
                    });
                    if (!res)
                        return TraversalEventOutcome::Pause;
                }
                
                entry->extent = context.returnedExtent;
                
                return TraversalEventOutcome::Continue;
                
            } else {
                
                if (!entry->originalSuppressEvaluation) {
                    if (entry->childIndexToUse == entry->handledChildIdx) {
                        SPR_ASSERT_LOGIC(!context.suppressEvaluation);
                        entry->value = std::move(context.returnedValue);
                    }
                    context.suppressEvaluation = (entry->childIndexToUse != entry->handledChildIdx + 1);
                }
                entry->extent.extendTo(context.returnedExtent);
                return TraversalEventOutcome::Continue;
            }
        }

        auto execute(ExecutionContext & context) const -> bool override {
            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
            context.suppressEvaluation = entry->originalSuppressEvaluation;
            if (!context.suppressEvaluation) {
                context.singlifyIfNeeded(entry->value, entry->extent);
                context.returnedValue = std::move(entry->value);
            }
            context.returnedExtent = entry->extent;
            return true;
        }
    };
    
    SPR_MAP_FUNCTION_ID_TO_IMPL(FunctionId::If, "IF", 3, 3, FunctionIf);

}




