// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include "true-function-creation.h"
#include "../execution-context.h"

namespace Spreader {

    //CONCATENATE is the only infinite scalar function so far
    //so there seems to be no point templatizing it
    //if new infinite scalar functions are added this needs to be
    //generalized
    class FunctionConcatenate : public TrueFunctionNodeBase {
        using Super = TrueFunctionNodeBase;

    protected:
        struct ExecutionStackEntry : Super::ExecutionStackEntry {
            using Super::ExecutionStackEntry::ExecutionStackEntry;

            std::variant<StringBuilder, Error> result;
            Size extent{1,1};
        };
        
        FunctionConcatenate(ArgumentList && args) noexcept:
            Super(std::move(args)) {
        }
        ~FunctionConcatenate() noexcept = default;
    public:
        auto onAfterArgument(ExecutionContext & context) const -> TraversalEventOutcome override {

            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
            
            if (!context.suppressEvaluation) {
                bool res = context.generateScalar(context.returnedValue, [&, entry](const Scalar & val) {
                    
                    if (auto builder = get_if<StringBuilder>(&entry->result)) {
                        applyVisitorCoercedTo<String>([entry, builder](auto && val) {
                            
                            using ArgType = std::remove_cvref_t<decltype(val)>;
                            
                            if constexpr (std::is_same_v<Error, ArgType>) {
                                entry->result = val;
                            } else {
                                builder->append(val);
                            }
                            
                        }, val);
                    }
                });
                if (!res)
                    return TraversalEventOutcome::Pause;
            }

            entry->extent.extendTo(context.returnedExtent);
            return TraversalEventOutcome::Continue;
        }

        auto execute(ExecutionContext & context) const -> bool override {
            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
            if (!context.suppressEvaluation) {
                context.returnedValue = visit([](auto & val) -> Scalar {
                    
                    using ArgType = std::remove_cvref_t<decltype(val)>;
                    
                    if constexpr (std::is_same_v<Error, ArgType>)
                        return val;
                    else
                        return val.build();
                    
                }, entry->result);
            }
            context.returnedExtent = entry->extent;
            return true;
        }
    };

    SPR_MAP_FUNCTION_ID_TO_IMPL(FunctionId::Concatenate, "CONCATENATE", 1, UINT_MAX, FunctionConcatenate);

}
