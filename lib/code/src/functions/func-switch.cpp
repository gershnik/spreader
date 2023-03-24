// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include "true-function-creation.h"
#include "../execution-context.h"
#include "../scalar-operators.h"

namespace Spreader {

    class FunctionSwitch : public TrueFunctionNodeBase {
        using Super = TrueFunctionNodeBase;

    protected:
        struct ExecutionStackEntry : Super::ExecutionStackEntry {
            using Super::ExecutionStackEntry::ExecutionStackEntry;

            bool originalSuppressEvaluation;
            std::optional<Scalar> compValue;
            ScalarGenerator value;
            Size extent;
        };

        FunctionSwitch(ArgumentList && args) noexcept:
            Super(std::move(args)) {
        }
        ~FunctionSwitch() noexcept = default;
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
                        
                        if (auto err = get<Error>(&scalar)) {
                            entry->value = *err;
                            context.suppressEvaluation = true;
                        } else {
                            entry->value = Error::InvalidArgs; //default in case match not found
                            entry->compValue = scalar;
                        }
                    });
                    if (!res)
                        return TraversalEventOutcome::Pause;
                }

                entry->extent = context.returnedExtent;
                
                return TraversalEventOutcome::Continue;

            } else if (entry->handledChildIdx % 2 != 0) {  //odd arguments: value to check

                if (entry->compValue) {
                    SPR_ASSERT_LOGIC(!context.suppressEvaluation);
                    bool res = context.generateScalar(context.returnedValue, [&, entry](const Scalar & scalar) {
                        
                        ScalarEquals::compare(*entry->compValue, scalar, [&, entry](auto compResult) {
                            using T = std::remove_cvref_t<decltype(compResult)>;
                            if constexpr (std::is_same_v<T, Error>) {
                                entry->value = compResult;
                                entry->compValue = std::nullopt;
                                context.suppressEvaluation = true;
                            } else  {
                                context.suppressEvaluation = !compResult;
                            }
                        });
                    });
                    if (!res)
                        return TraversalEventOutcome::Pause;
                }
                
                entry->extent.extendTo(context.returnedExtent);
                
                return TraversalEventOutcome::Continue;

            } else { //even arguments: value to return
                
                if (!entry->originalSuppressEvaluation) {
                    if (!context.suppressEvaluation) {
                        entry->value = std::move(context.returnedValue);
                    }
                    context.suppressEvaluation = !entry->compValue;
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
    
    SPR_MAP_FUNCTION_ID_TO_IMPL(FunctionId::Switch, "SWITCH", 3, UINT_MAX, FunctionSwitch);

}

