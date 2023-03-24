// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include "true-function-creation.h"
#include "../execution-context.h"

namespace Spreader {

    class FunctionIndex : public TrueFunctionNodeBase {
        using Super = TrueFunctionNodeBase;
    protected:
        struct ExecutionStackEntry : Super::ExecutionStackEntry {
            using Super::ExecutionStackEntry::ExecutionStackEntry;

            Size extent{1,1};
            ScalarGenerator range;
            Number row = 0;
            Number column = 0;
            ScalarGenerator result;
            bool columnPresent = false;
            bool extentGovernedByRowColumn = false;
        };

        FunctionIndex(ArgumentList && args) noexcept:
            Super(std::move(args)) {
        }
        ~FunctionIndex() noexcept = default;
    public:
        auto onAfterArgument(ExecutionContext & context) const -> TraversalEventOutcome override {

            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);

            if (entry->handledChildIdx == 0) {

                if (!context.suppressEvaluation) {
                    entry->range = context.returnedValue;
                }
                
                return TraversalEventOutcome::Continue;
                
            } else if (entry->handledChildIdx == 1) {

                if (!context.suppressEvaluation) {
                    
                    bool res = context.generateScalar(context.returnedValue, [entry](const Scalar & val) {
                        
                        applyVisitorCoercedTo<Number>([entry](const auto & arg) {

                            using ArgType = std::remove_cvref_t<decltype(arg)>;

                            if constexpr (std::is_same_v<Error, ArgType>) {
                                entry->result = arg;
                            } else {
                                entry->row = arg;
                            }

                        }, val);
                    });
                    
                    if (!res)
                        return TraversalEventOutcome::Pause;
                }
                
                if (context.returnedExtent != Size{1, 1})
                    entry->extentGovernedByRowColumn = true;
                entry->extent = context.returnedExtent;
                
                return TraversalEventOutcome::Continue;
                    
                
            } else {
                SPR_ASSERT_LOGIC(entry->handledChildIdx == 2);

                if (!context.suppressEvaluation) {
                    bool res = context.generateScalar(context.returnedValue, [entry](const Scalar & val) {
                        applyVisitorCoercedTo<Number>([entry](const auto & arg) {

                            using ArgType = std::remove_cvref_t<decltype(arg)>;

                            if constexpr (std::is_same_v<Error, ArgType>) {
                                entry->result = arg;
                            } else {
                                entry->column = arg;
                                entry->columnPresent = true;
                            }

                        }, val);
                    });
                    
                    if (!res)
                        return TraversalEventOutcome::Pause;
                }
                
                if (context.returnedExtent != Size{1, 1})
                    entry->extentGovernedByRowColumn = true;
                entry->extent.extendTo(context.returnedExtent);
                
                return TraversalEventOutcome::Continue;
            }
        }
        
        static void calculate(ExecutionContext & context, const Scalar & val, SizeType row, SizeType column, bool /*columnPresent*/) {
            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
            if (get<Error>(&val))
                entry->result = val;
            else if (row > 1 || column > 1)
                entry->result = Error::InvalidReference;
            else
                entry->result = val;
        }
        
        static void calculate(ExecutionContext & context, Point val, SizeType row, SizeType column, bool /*columnPresent*/) {
            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
            if (row > 1 || column > 1)
                entry->result = Error::InvalidReference;
            else
                entry->result = val;
        }
        
        static auto calculateOffset(ExecutionContext & context, Size valSize,
                                    SizeType row, SizeType column, bool columnPresent) noexcept -> std::optional<Point> {
            
            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
            
            Point pt;
            switch( ((row == 0) << 1) | (column == 0) ) {
                
            case 0b00:  //neither is 0
                pt = {column - 1, row - 1};
                break;
                
            case 0b01:  //column is 0
                if (entry->extentGovernedByRowColumn) {
                    pt = {0, row - 1};
                } else if (columnPresent || valSize.height > 1) {
                    pt = {context.offset.x, row - 1};
                    entry->extent = Size{valSize.width, 1};
                } else {
                    //Special case, single column value and column not present.
                    //Use row index as column index
                    pt = {row - 1, 0};
                }
                break;
                
            case 0b10:  //row is 0
                if (entry->extentGovernedByRowColumn) {
                    pt = {column - 1, 0};
                } else {
                    pt = {column - 1, context.offset.y};
                    entry->extent = Size{1, valSize.height};
                }
                break;
                
            case 0b11:  //both are 0
                if (entry->extentGovernedByRowColumn) {
                    pt = Point{0, 0};
                } else {
                    pt = context.offset;
                    entry->extent = valSize;
                } 
                break;
                
            default:
                SPR_ASSERT_LOGIC(false);
            }
            
            if (pt.x < valSize.width && pt.y < valSize.height)
                return pt;
            
            if (!entry->extentGovernedByRowColumn)
                entry->extent = Size{1, 1};
            return std::nullopt;
        }
        
        static void calculate(ExecutionContext & context, const ArrayPtr & val, SizeType row, SizeType column, bool columnPresent) noexcept {
            
            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
            
            auto size = val->size();
            if (auto offset = calculateOffset(context, size, row, column, columnPresent))
                entry->result = (*val)[*offset];
            else
                entry->result = Error::InvalidReference;
        }
        
        static void calculate(ExecutionContext & context, Rect val, SizeType row, SizeType column, bool columnPresent) noexcept {
            
            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
            
            if (auto offset = calculateOffset(context, val.size, row, column, columnPresent)) {
                
                auto clampedOffset = Size {
                    std::min(context.grid().maxSize().width - val.origin.x, offset->x),
                    std::min(context.grid().maxSize().height - val.origin.y, offset->y)
                };
                Point pt = val.origin + clampedOffset;
                
                entry->result = pt;
                
            } else {
                entry->result = Error::InvalidReference;
            }
            
        }
        
        void calculate(ExecutionContext & context) const noexcept {
            
            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
            
            if (entry->row.value() < 0 || entry->row.value() >= context.grid().maxSize().height) {
                entry->result = Error::InvalidValue;
                return;
            }
            
            if (entry->column.value() < 0 || entry->column.value() >= context.grid().maxSize().width) {
                entry->result = Error::InvalidValue;
                return;
            }
            
            auto intRow = SizeType(entry->row.value());
            auto intColumn = SizeType(entry->column.value());
            
            applyVisitor([&](const auto & val) {
                calculate(context, val, intRow, intColumn, entry->columnPresent);
            }, entry->range);
            
        }

        auto execute(ExecutionContext & context) const -> bool override {

            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
            
            if (!context.suppressEvaluation) {
                
                auto scalar = get<Scalar>(&entry->result);
                SPR_ASSERT_LOGIC(scalar); //at this point result is either blank or error scalar
                
                if (!get<Error>(scalar)) {
                    calculate(context);
                }
                
                context.returnedValue = std::move(entry->result);
            }
            context.returnedExtent = entry->extent;
            return true;
        }

    };
    
    SPR_MAP_FUNCTION_ID_TO_IMPL(FunctionId::Index, "INDEX", 2, 3, FunctionIndex);

    
}
