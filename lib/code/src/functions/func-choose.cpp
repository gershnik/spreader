// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include "true-function-creation.h"
#include "../execution-context.h"

namespace Spreader {

    class FunctionChoose : public TrueFunctionNodeBase {
        using Super = TrueFunctionNodeBase;

    protected:
        struct ExecutionStackEntry : Super::ExecutionStackEntry {
            using Super::ExecutionStackEntry::ExecutionStackEntry;

            bool originalSuppressEvaluation;
            decltype(handledChildIdx) childIndexToUse = 0;
            ScalarGenerator value;
            Size extent;
        };

        FunctionChoose(ArgumentList && args) noexcept:
            Super(std::move(args)) {
        }
        ~FunctionChoose() noexcept = default;
    public:
        void onBeforeArguments(ExecutionContext & context) const override {
            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
            entry->originalSuppressEvaluation = context.suppressEvaluation;
        }
        
        auto onAfterArgument(ExecutionContext & context) const -> TraversalEventOutcome override {

            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
            
            if (entry->handledChildIdx == 0) {
                
                if (!context.suppressEvaluation) {
                    
                    bool res = context.generateScalar(context.returnedValue, [&, entry](const Scalar & scalar) {
                        
                        applyVisitorCoercedTo<Number>([&](auto val)  {
                            
                            using T = std::remove_cvref_t<decltype(val)>;
                            
                            if constexpr (std::is_same_v<T, Number>) {
                                using IndexType = decltype(ExecutionStackEntry::handledChildIdx);
                                entry->value = Error::InvalidValue; //the default unless we find proper arg
                                if (val.value() > 0 && std::trunc(val.value()) < std::numeric_limits<IndexType>::max())
                                    entry->childIndexToUse = IndexType(val.value());
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
    
    SPR_MAP_FUNCTION_ID_TO_IMPL(FunctionId::Choose, "CHOOSE", 2, UINT_MAX, FunctionChoose);

}




