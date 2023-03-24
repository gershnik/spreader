// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include "true-function-creation.h"
#include "../execution-context.h"

namespace Spreader {

    template<bool IsColumn>
    class FunctionRowColumn : public TrueFunctionNodeBase {
        using Super = TrueFunctionNodeBase;

    protected:
        struct ExecutionStackEntry : Super::ExecutionStackEntry {
            using Super::ExecutionStackEntry::ExecutionStackEntry;

            Point origin = {0,0};
            Size extent = {0,0};
            bool invalidArg = false;
        };

        FunctionRowColumn(ArgumentList && args) noexcept:
            Super(std::move(args)) {
        }
        ~FunctionRowColumn() noexcept = default;
    public:

        auto onAfterArgument(ExecutionContext & context) const -> TraversalEventOutcome override {

            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
            
            entry->extent = context.returnedExtent;

            if (!context.suppressEvaluation) {

                applyVisitor([entry](auto && val) {

                    using T = std::remove_cvref_t<decltype(val)>;
                    
                    if constexpr (std::is_same_v<T, Scalar> || std::is_same_v<T, ArrayPtr>) {
                        entry->invalidArg = true;
                    } else if constexpr (std::is_same_v<T, Point>) {
                        entry->origin = val;
                    } else if constexpr (std::is_same_v<T, Rect>) {
                        entry->origin = val.origin;
                    } else {
                        static_assert(dependentFalse<T>, "unhandled type");
                    }

                }, context.returnedValue);
            }
            return TraversalEventOutcome::Continue;
        }

        auto execute(ExecutionContext & context) const -> bool override {

            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);

            if (entry->extent == Size{0, 0}) {
                entry->origin = context.at();
                entry->extent = Size{1, 1};
            }
            
            if constexpr (IsColumn) {
                context.returnedExtent = Size{entry->extent.width, 1};
            } else {
                context.returnedExtent = Size{1, entry->extent.height};
            }

            if (!context.suppressEvaluation) {
                if (entry->invalidArg) {
                    context.returnedValue = Error::InvalidValue;
                } else {
                    if constexpr (IsColumn) {
                        context.returnedValue = entry->origin.x + context.offset.x + 1;
                    } else {
                        context.returnedValue = entry->origin.y + context.offset.y + 1;
                    }
                }
            }

            return true;
        }

    };
    
    SPR_MAP_FUNCTION_ID_TO_IMPL(FunctionId::Column, "COLUMN", 0, 1, FunctionRowColumn<true>);
    SPR_MAP_FUNCTION_ID_TO_IMPL(FunctionId::Row,    "ROW",    0, 1, FunctionRowColumn<false>);

}
