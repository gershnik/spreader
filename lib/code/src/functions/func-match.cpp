// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include "true-function-creation.h"
#include "../execution-context.h"
#include "../scalar-lookup.h"

namespace Spreader {

    class FunctionMatch : public TrueFunctionNodeBase {
        using Super = TrueFunctionNodeBase;

    protected:
        struct ExecutionStackEntry : Super::ExecutionStackEntry {
            using Super::ExecutionStackEntry::ExecutionStackEntry;

            std::optional<Error> error;
            Scalar value;
            std::variant<Rect, ArrayPtr> range;
            int type = 1;
            ScalarLookup::Direction dir;
            bool originalSuppressEvaluation;
            Size extent{1, 1};
        };

        FunctionMatch(ArgumentList && args) noexcept:
            Super(std::move(args)) {
        }
        ~FunctionMatch() noexcept = default;
    public:

        void onBeforeArguments(ExecutionContext & context) const override {
            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);

            entry->originalSuppressEvaluation = context.suppressEvaluation;
        }

        auto onAfterArgument(ExecutionContext & context) const -> TraversalEventOutcome override {
            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);

            if (entry->handledChildIdx == 0) {

                if (!context.suppressEvaluation) {

                    bool res = context.generateScalar(context.returnedValue, [entry](const Scalar & val) {
                         entry->value = val;
                    });
                    if (!res)
                        return TraversalEventOutcome::Pause;

                    if (auto error = get<Error>(&entry->value)) {
                        entry->error = *error;
                        context.suppressEvaluation = true;
                    }
                }

                entry->extent = context.returnedExtent;

                return TraversalEventOutcome::Continue;

            } else if (entry->handledChildIdx == 1) {

                if (!context.suppressEvaluation) {

                    applyVisitor([&,entry](const auto & val) {

                        using T = std::remove_cvref_t<decltype(val)>;

                        if constexpr (std::is_same_v<T, Rect>) {
                            if (val.size.width == 1) {
                                entry->dir = ScalarLookup::Horizontal;
                                entry->range = val;
                            } else if (val.size.height == 1) {
                                entry->dir = ScalarLookup::Vertical;
                                entry->range = val;
                            } else {
                                entry->error = Error::InvalidArgs;
                                context.suppressEvaluation = true;
                            }
                        } else if constexpr (std::is_same_v<T, ArrayPtr>) {
                            if (val->size().width == 1) {
                                entry->dir = ScalarLookup::Horizontal;
                                entry->range = val;
                            } else if (val->size().height == 1) {
                                entry->dir = ScalarLookup::Vertical;
                                entry->range = val;
                            } else {
                                entry->error = Error::InvalidArgs;
                                context.suppressEvaluation = true;
                            }
                        } else if constexpr (std::is_same_v<T, Point>) {
                            entry->range = Rect{.origin = val, .size = {1, 1}};
                            entry->dir = ScalarLookup::Horizontal;
                        } else {
                            entry->error = Error::InvalidArgs;
                            context.suppressEvaluation = true;
                        }

                    }, context.returnedValue);
                }
                
                return TraversalEventOutcome::Continue;

            } else {
                SPR_ASSERT_LOGIC(entry->handledChildIdx == 2);

                if (!context.suppressEvaluation) {
                    bool res = context.generateScalar(context.returnedValue, [&,entry](const Scalar & val) {
                        applyVisitorCoercedTo<Number>([&,entry](const auto & num) {

                            using T = std::remove_cvref_t<decltype(num)>;

                            if constexpr (std::is_same_v<T, Error>) {
                                entry->error = num;
                                context.suppressEvaluation = true;
                            } else {
                                entry->type = num.value() <  0 ? -1 : (
                                              num.value() == 0 ?  0 : (
                                                                  1));
                            }

                        }, val);
                    });
                    if (!res)
                        return TraversalEventOutcome::Pause;
                }

                entry->extent.extendTo(context.returnedExtent);

                return TraversalEventOutcome::Continue;
            }
        }

        static auto calculate(ExecutionContext & context) -> bool {
            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);

            if (entry->error) {
                context.returnedValue = *entry->error;
                return true;
            }

            return visit([&,entry](const auto & range) {

                using T = std::remove_cvref_t<decltype(range)>;
                
                if constexpr (std::is_same_v<T, ArrayPtr>) {
                    context.returnedValue = ScalarLookup::match(entry->type, entry->value, range);
                    return true;
                } else if constexpr (std::is_same_v<T, Rect>) {

                    auto res = ScalarLookup::match(entry->type, entry->value, ScalarLookup::RectParam{context, range});
                    if (res) {
                        context.returnedValue = std::move(*res);
                        return true;
                    }
                    return false;
                }
            }, entry->range);
        }

        auto execute(ExecutionContext & context) const -> bool override {
            
            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);

            if (!entry->originalSuppressEvaluation) {
                if (!calculate(context))
                    return false;
            }
            context.returnedExtent = entry->extent;
            context.suppressEvaluation = entry->originalSuppressEvaluation;
            return true;
        }

    };
    
    SPR_MAP_FUNCTION_ID_TO_IMPL(FunctionId::Match, "MATCH", 2, 3, FunctionMatch);

}
