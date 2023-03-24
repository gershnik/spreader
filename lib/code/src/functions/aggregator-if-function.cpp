// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include "true-function-creation.h"
#include "../execution-context.h"
#include "../scalar-numeric-aggregators.h"
#include "../scalar-xif-matcher.h"

namespace Spreader {

    template<class Aggregator>
    class AggregatorIfFunction : public TrueFunctionNodeBase {
        using Super = TrueFunctionNodeBase;
    protected:
        struct ExecutionStackEntry : Super::ExecutionStackEntry {
            using Super::ExecutionStackEntry::ExecutionStackEntry;

            Size extent{1,1};
            bool originalSuppressEvaluation;
            Aggregator aggregator;
            Rect matchRect;
            std::optional<XIfMatcher> matcher;
            std::optional<Rect> valueRect;
        };

        AggregatorIfFunction(ArgumentList && args) noexcept:
            Super(std::move(args)) {
        }
        ~AggregatorIfFunction() noexcept = default;
    public:
        void onBeforeArguments(ExecutionContext & context) const override {
            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
            entry->originalSuppressEvaluation = context.suppressEvaluation;
        }
        
        auto onAfterArgument(ExecutionContext & context) const -> TraversalEventOutcome override {

            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);

            if (entry->handledChildIdx == 0) {

                if (!context.suppressEvaluation) {
                    
                    applyVisitor([&](const auto & val) {
                        
                        using T = std::remove_cvref_t<decltype(val)>;
                        
                        if constexpr (std::is_same_v<T, Point>) {
                            entry->matchRect = {val, Size{1, 1}};
                        } else if constexpr (std::is_same_v<T, Rect>) {
                            entry->matchRect = val;
                        } else if constexpr (std::is_same_v<T, Scalar> || std::is_same_v<T, ArrayPtr>) {
                            entry->aggregator.setError(Error::InvalidFormula);
                            context.suppressEvaluation = true;
                        } else {
                            static_assert(dependentFalse<T>, "unhandled type");
                        }
                        
                    }, context.returnedValue);
                    
                    return TraversalEventOutcome::Continue;
                }
                
                return TraversalEventOutcome::Continue;
                
            } else if (entry->handledChildIdx == 1) {

                SPR_ASSERT_LOGIC(!entry->matcher);

                entry->extent = context.returnedExtent;
                
                if (context.suppressEvaluation) {
                    //no point processing the last argument since extent is only affected by this one
                    return TraversalEventOutcome::ContinueAndSkipAllChildren;
                }
                    
                bool res = context.generateScalar(context.returnedValue, [&, entry](const Scalar & val) {
                    entry->matcher.emplace(val);
                });
                
                if (!res)
                    return TraversalEventOutcome::Pause;
                
                return TraversalEventOutcome::Continue;
                    
                
            } else {
                SPR_ASSERT_LOGIC(entry->handledChildIdx == 2);

                if (!context.suppressEvaluation) {
                    
                    applyVisitor([&](const auto & val) {
                        
                        using T = std::remove_cvref_t<decltype(val)>;
                        
                        if constexpr (std::is_same_v<T, Point>) {
                            entry->valueRect = {val, Size{1, 1}};
                        } else if constexpr (std::is_same_v<T, Rect>) {
                            entry->valueRect = val;
                        } else if constexpr (std::is_same_v<T, Scalar> || std::is_same_v<T, ArrayPtr>) {
                            entry->aggregator.setError(Error::InvalidFormula);
                            context.suppressEvaluation = true;
                        } else {
                            static_assert(dependentFalse<T>, "unhandled type");
                        }
                        
                    }, context.returnedValue);
                }
                
                return TraversalEventOutcome::Continue;
            }
        }
        
        static auto calculate(ExecutionContext & context) -> bool {
            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
            
            ExecutionContext::AggregateRectResult res;
            
            auto aggrState = saveState(entry->aggregator);
            auto & matcher = *entry->matcher;
            if (entry->valueRect)
                res = context.aggregateRects(entry->matchRect, *entry->valueRect, [&, entry](const Scalar & comp, const Scalar & value) {
                    if (matcher(comp)) {
                        return entry->aggregator.addIndirect(value);
                    }
                    return true;
                });
            else
                res = context.aggregateRect(entry->matchRect, [&, entry](const Scalar & value) {
                    if (matcher(value)) {
                        return entry->aggregator.addIndirect(value);
                    }
                    return true;
                });
            
            if (res == ExecutionContext::AggregateRectResult::HasDependencies) {
                restoreState(entry->aggregator, std::move(aggrState));
                return false;
            }
            
            return true;
        }
        
        auto execute(ExecutionContext & context) const -> bool override {

            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
            
            if (!entry->originalSuppressEvaluation) {
                
                if (!entry->aggregator.isError()) {
                    
                    if (!calculate(context))
                        return false;
                }
                
                context.returnedValue = std::move(entry->aggregator.result());
            }
            
            context.returnedExtent = entry->extent;
            context.suppressEvaluation = entry->originalSuppressEvaluation;
            return true;
        }

    };
    
    #define SPR_MAP_AGGREGATE_FUNCTION_ID(id, prefix, agg) SPR_MAP_FUNCTION_ID_TO_IMPL(id, prefix, 2, 3, AggregatorIfFunction<agg>)
    
    SPR_MAP_AGGREGATE_FUNCTION_ID(FunctionId::AverageIf, "AVERAGEIF", ScalarAverage);
    SPR_MAP_AGGREGATE_FUNCTION_ID(FunctionId::SumIf,     "SUMIF",     ScalarSum);

    
}
