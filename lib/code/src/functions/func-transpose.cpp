// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include "true-function-creation.h"
#include "../execution-context.h"

#include <spreader/iteration.h>

namespace Spreader {

    class FunctionTranspose : public TrueFunctionNodeBase {
        using Super = TrueFunctionNodeBase;

    protected:
        struct ExecutionStackEntry : Super::ExecutionStackEntry {
            using Super::ExecutionStackEntry::ExecutionStackEntry;

            ScalarGenerator result;
            Size extent;
        };

        FunctionTranspose(ArgumentList && args) noexcept:
            Super(std::move(args)) {
        }
        ~FunctionTranspose() noexcept = default;
    public:

        void onBeforeArguments(ExecutionContext & context) const override {
            //auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
            
            std::swap(context.offset.x, context.offset.y);
        }

        auto onAfterArgument(ExecutionContext & context) const -> TraversalEventOutcome override {
            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);

            entry->extent = Size{context.returnedExtent.height, context.returnedExtent.width};
            
            TraversalEventOutcome ret = TraversalEventOutcome::Continue;

            if (!context.suppressEvaluation) {

                applyVisitor([&,entry](const auto & val) {

                    using T = std::remove_cvref_t<decltype(val)>;

                    if constexpr (std::is_same_v<T, Rect>) {

                        if (auto validPoint = context.generatePoint(val, context.offset)) {
                            ret = handlePoint(context, *validPoint);
                        } else {
                            entry->result = Error::InvalidArgs;
                        }

                    } else if constexpr (std::is_same_v<T, ArrayPtr>) {

                        entry->result = Array::create(entry->extent, [&,entry](Scalar * data) {
                            for(SizeCursor cur(entry->extent); cur; ++cur) {
                                Point myPt = *cur;
                                Point srcPt{myPt.y, myPt.x};
                                new (data + myPt.y * entry->extent.width + myPt.x) Scalar((*val)[srcPt]);
                            }
                        });

                    } else if constexpr (std::is_same_v<T, Point>) {
                        
                        ret = handlePoint(context, val);

                    } else if constexpr (std::is_same_v<T, Scalar>) {

                        entry->result = val;
                    } else {
                        static_assert(dependentFalse<T>, "unhandled type");
                    }

                }, context.returnedValue);
            }

            std::swap(context.offset.x, context.offset.y);
            return ret;
        }

        static auto handlePoint(ExecutionContext & context, Point pt) -> TraversalEventOutcome {
            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);

            return context.evaluateCell(pt, [entry](const Scalar & cellVal) {
                    entry->result = cellVal;
                    return TraversalEventOutcome::Continue;
                }, [](bool /*isCurcular*/) {
                    return TraversalEventOutcome::Pause;
                });
        }

        auto execute(ExecutionContext & context) const -> bool override {

            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);

            if (!context.suppressEvaluation) {
                context.returnedValue = std::move(entry->result);
            }
            context.returnedExtent = entry->extent;
            return true;
        }

    };
    
    SPR_MAP_FUNCTION_ID_TO_IMPL(FunctionId::Transpose, "TRANSPOSE", 1, 1, FunctionTranspose);

}
