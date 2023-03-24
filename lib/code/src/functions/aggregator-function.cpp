// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include "true-function-creation.h"
#include "../execution-context.h"
#include "../scalar-numeric-aggregators.h"
#include "../scalar-boolean-aggregators.h"
#include "../scalar-string-aggregators.h"


namespace Spreader {

    template<class Aggregator>
    class AggregatorFunction : public TrueFunctionNodeBase {
        using Super = TrueFunctionNodeBase;
        
    protected:
        struct ExecutionStackEntry : Super::ExecutionStackEntry {
            using Super::ExecutionStackEntry::ExecutionStackEntry;

            Aggregator aggregator;
            Point savedOffset;
        };
        
        AggregatorFunction(ArgumentList && args) noexcept:
            Super(std::move(args)) {
        }
        
        ~AggregatorFunction() noexcept = default;

    public:
        void onBeforeArguments(ExecutionContext & context) const override {
            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
            entry->savedOffset = context.offset;
            context.offset = Point{0, 0};
        }

        auto onAfterArgument(ExecutionContext & context) const -> TraversalEventOutcome override {
            
            if (context.suppressEvaluation)
                return TraversalEventOutcome::ContinueAndSkipAllChildren;

            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);

            SPR_ASSERT_LOGIC(!entry->aggregator.isError());

            return applyVisitor([&, entry](auto && val) -> TraversalEventOutcome {
                using T = std::remove_cvref_t<decltype(val)>;

                if constexpr (std::is_same_v<T, Scalar>) {

                    if (!entry->aggregator.addDirect(val))
                        return TraversalEventOutcome::ContinueAndSkipAllChildren;
                    if (context.incrementOffset(context.returnedExtent))
                        return TraversalEventOutcome::RepeatChild;
                    return TraversalEventOutcome::Continue;
                    
                } else if constexpr (std::is_same_v<T, Point>) {
                    
                    TraversalEventOutcome ret = context.evaluateCell(val, [&](const Scalar & cellVal) {
                    
                        if (!entry->aggregator.addIndirect(cellVal))
                            return TraversalEventOutcome::ContinueAndSkipAllChildren;
                        return TraversalEventOutcome::Continue;
                        
                    }, [&](bool /*isCircular*/) {
                        
                        return TraversalEventOutcome::Pause;
                    });
                    
                    if (ret != TraversalEventOutcome::Continue)
                        return ret;
                    
                    if (context.incrementOffset(context.returnedExtent))
                        return TraversalEventOutcome::RepeatChild;
                    return TraversalEventOutcome::Continue;

                } else if constexpr (std::is_same_v<T, ArrayPtr>) {
                    
                    SPR_ASSERT_LOGIC(context.returnedExtent == val->size());

                    if (!forEachWhileTrue(val->begin(), val->end(), [entry](const Scalar & elem) { 
                            return entry->aggregator.addIndirect(elem); 
                        })) {
                        return TraversalEventOutcome::ContinueAndSkipAllChildren;
                    }
                    return TraversalEventOutcome::Continue;

                } else if constexpr (std::is_same_v<T, Rect>) {
                    
                    SPR_ASSERT_LOGIC(context.returnedExtent == val.size);

                    auto aggrState = saveState(entry->aggregator);
                    auto aggregateRes = context.aggregateRect(val, [entry](const Scalar & elem) {
                        return entry->aggregator.addIndirect(elem);
                    });
                    switch(aggregateRes) {

                        case ExecutionContext::AggregateRectResult::Success:
                            return TraversalEventOutcome::Continue;

                        case ExecutionContext::AggregateRectResult::Aborted:
                            return TraversalEventOutcome::ContinueAndSkipAllChildren;

                        case ExecutionContext::AggregateRectResult::HasDependencies:
                            restoreState(entry->aggregator, std::move(aggrState));
                            return TraversalEventOutcome::Pause;
                    }

                    SPR_ASSERT_LOGIC(false);
                    SPR_FATAL_ERROR("impossible return from aggregateRect");
                }

            }, context.returnedValue);
        }

        auto execute(ExecutionContext & context) const -> bool override {

            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
            
            context.offset = entry->savedOffset;
            if (!context.suppressEvaluation)
                context.returnedValue = std::move(entry->aggregator.result());
            context.returnedExtent = Size{1, 1};

            return true;
        }

    };
    
    #define SPR_MAP_AGGREGATE_FUNCTION_ID(id, prefix, agg) SPR_MAP_FUNCTION_ID_TO_IMPL(id, prefix, 1, UINT_MAX, AggregatorFunction<agg>)
    
    SPR_MAP_AGGREGATE_FUNCTION_ID(FunctionId::And,      "AND",        ScalarAnd);
    SPR_MAP_AGGREGATE_FUNCTION_ID(FunctionId::Average,  "AVERAGE",    ScalarAverage);
    SPR_MAP_AGGREGATE_FUNCTION_ID(FunctionId::AverageA, "AVERAGEA",   ScalarAverageA);
    SPR_MAP_AGGREGATE_FUNCTION_ID(FunctionId::Concat,   "CONCAT",     ScalarConcat);
    SPR_MAP_AGGREGATE_FUNCTION_ID(FunctionId::Count,    "COUNT",      ScalarCount);
    SPR_MAP_AGGREGATE_FUNCTION_ID(FunctionId::CountA,   "COUNTA",     ScalarCountA);
    SPR_MAP_AGGREGATE_FUNCTION_ID(FunctionId::Max,      "MAX",        ScalarMax);
    SPR_MAP_AGGREGATE_FUNCTION_ID(FunctionId::MaxA,     "MAXA",       ScalarMaxA);
    SPR_MAP_AGGREGATE_FUNCTION_ID(FunctionId::Min,      "MIN",        ScalarMin);
    SPR_MAP_AGGREGATE_FUNCTION_ID(FunctionId::MinA,     "MINA",       ScalarMinA);
    SPR_MAP_AGGREGATE_FUNCTION_ID(FunctionId::Or,       "OR",         ScalarOr);
    SPR_MAP_AGGREGATE_FUNCTION_ID(FunctionId::StDev_S,  "STDEV.S",    ScalarStdDev);
    SPR_MAP_AGGREGATE_FUNCTION_ID(FunctionId::StDev_P,  "STDEV.P",    ScalarStdDevP);
    SPR_MAP_AGGREGATE_FUNCTION_ID(FunctionId::StDev,    "STDEV",      ScalarStdDev);
    SPR_MAP_AGGREGATE_FUNCTION_ID(FunctionId::StDevA,   "STDEVA",     ScalarStdDevA);
    SPR_MAP_AGGREGATE_FUNCTION_ID(FunctionId::StDevP,   "STDEVP",     ScalarStdDevP);
    SPR_MAP_AGGREGATE_FUNCTION_ID(FunctionId::StDevPA,  "STDEVPA",    ScalarStdDevPA);
    SPR_MAP_AGGREGATE_FUNCTION_ID(FunctionId::Sum,      "SUM",        ScalarSum);
    SPR_MAP_AGGREGATE_FUNCTION_ID(FunctionId::XOr,      "XOR",        ScalarXOr);
    
}

