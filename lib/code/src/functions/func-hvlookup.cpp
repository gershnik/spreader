// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include "true-function-creation.h"
#include "../execution-context.h"
#include "../scalar-lookup.h"

namespace Spreader {

    template<ScalarLookup::Direction Dir>
    class FunctionHVLookup : public TrueFunctionNodeBase {
        using Super = TrueFunctionNodeBase;

    protected:
        struct ExecutionStackEntry : Super::ExecutionStackEntry {
            using Super::ExecutionStackEntry::ExecutionStackEntry;

            std::optional<Error> error;
            Scalar value;
            std::variant<Rect, ArrayPtr> range;
            SizeType maxDirectionIndex;
            SizeType directionIndex;
            bool approximate = true;
            bool originalSuppressEvaluation;
            Size extent{1, 1};
        };

        FunctionHVLookup(ArgumentList && args) noexcept:
            Super(std::move(args)) {
        }
        ~FunctionHVLookup() noexcept = default;
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
                            entry->range = val;
                            entry->maxDirectionIndex = Dir == ScalarLookup::Vertical ? val.size.width : val.size.height;
                        } else if constexpr (std::is_same_v<T, ArrayPtr>) {
                            entry->range = val;
                            entry->maxDirectionIndex = Dir == ScalarLookup::Vertical ? val->size().width : val->size().height;
                        } else if constexpr (std::is_same_v<T, Point>) {
                            entry->range = Rect{.origin = val, .size = {1, 1}};
                        } else {
                            entry->error = Error::InvalidArgs;
                            context.suppressEvaluation = true;
                        }

                    }, context.returnedValue);
                }
                
                return TraversalEventOutcome::Continue;

            } else if (entry->handledChildIdx == 2) {

                if (!context.suppressEvaluation) {
                    bool res = context.generateScalar(context.returnedValue, [&,entry](const Scalar & val) {
                        applyVisitorCoercedTo<Number>([&,entry](const auto & num) {

                            using T = std::remove_cvref_t<decltype(num)>;

                            if constexpr (std::is_same_v<T, Error>) {
                                entry->error = Error::InvalidArgs;
                                context.suppressEvaluation = true;
                            } else {
                                if (num.value() < 1 || num.value() > entry->maxDirectionIndex) {
                                    entry->error = Error::InvalidValue;
                                    context.suppressEvaluation = true;
                                } else {
                                    entry->directionIndex = SizeType(num.value()) - 1;
                                }
                            }

                        }, val);
                    });
                    if (!res)
                        return TraversalEventOutcome::Pause;
                }

                entry->extent.extendTo(context.returnedExtent);

                return TraversalEventOutcome::Continue;

            } else {
                SPR_ASSERT_LOGIC(entry->handledChildIdx == 3);

                if (!context.suppressEvaluation) {
                    bool res = context.generateScalar(context.returnedValue, [&,entry](const Scalar & val) {
                        applyVisitorCoercedTo<bool>([&,entry](const auto & flag) {

                            using T = std::remove_cvref_t<decltype(flag)>;

                            if constexpr (std::is_same_v<T, Error>) {
                                entry->error = Error::InvalidArgs;
                                context.suppressEvaluation = true;
                            } else {
                                entry->approximate = flag;
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
                    context.returnedValue = ScalarLookup::lookup<Dir>(entry->approximate, entry->value, 
                                                                      range, entry->directionIndex);
                    return true;
                } else if constexpr (std::is_same_v<T, Rect>) {

                    auto res = ScalarLookup::lookup<Dir>(entry->approximate, entry->value, 
                                                         ScalarLookup::RectParam{context, range}, entry->directionIndex);
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
    
    SPR_MAP_FUNCTION_ID_TO_IMPL(FunctionId::HLookup, "HLOOKUP", 3, 4, FunctionHVLookup<ScalarLookup::Horizontal>);
    SPR_MAP_FUNCTION_ID_TO_IMPL(FunctionId::VLookup, "VLOOKUP", 3, 4, FunctionHVLookup<ScalarLookup::Vertical>);

}
