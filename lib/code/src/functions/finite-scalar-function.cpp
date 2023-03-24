// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include "true-function-creation.h"
#include "../execution-context.h"
#include "../scalar-numeric-functions.h"
#include "../scalar-date-functions.h"
#include "../scalar-string-functions.h"
#include "../scalar-boolean-functions.h"
#include "../scalar-info-functions.h"

#include <tuple>

namespace Spreader {

    template<class Op, bool HasArgs = (Op::minArgs > 0)>
    class FiniteScalarFunction : public TrueFunctionNodeBase {
        using Super = TrueFunctionNodeBase;

    protected:
        struct ExecutionStackEntry : Super::ExecutionStackEntry {
            using Super::ExecutionStackEntry::ExecutionStackEntry;

            SPR_NO_UNIQUE_ADDRESS Op op;
            Size extent{1,1};
        };
        
        FiniteScalarFunction(ArgumentList && args) noexcept:
            Super(std::move(args)) {
        }
        ~FiniteScalarFunction() noexcept = default;
    public:
        auto onAfterArgument(ExecutionContext & context) const -> TraversalEventOutcome override {

            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
            
            if (!context.suppressEvaluation) {
                
                if constexpr (Op::hasContext) {
                    if (entry->handledChildIdx == 0) {
                        setContext(entry->op, context);
                    }
                }
                
                bool res = context.generateScalar(context.returnedValue, [&, entry](const Scalar & val) {
                    entry->op.handleArgument(entry->handledChildIdx, val);
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
                context.returnedValue = std::move(entry->op.result());
            }
            context.returnedExtent = entry->extent;
            return true;
        }
        
    private:
        template<class X = Op>
        requires(X::hasContext)
        static void setContext(X & op, ExecutionContext & context) {
            if constexpr (std::is_same_v<std::remove_cvref_t<typename X::Context>, NameManager>) {
                op.setContext(context.names());
            } else {
                static_assert(dependentFalse<X>, "context passing not defined");
            }
        }

    };

    template<class Op>
    class FiniteScalarFunction<Op, false> : public TrueFunctionNodeBase {
        using Super = TrueFunctionNodeBase;

    protected:
        FiniteScalarFunction(ArgumentList && args) noexcept:
            Super(std::move(args)) {
        }
        ~FiniteScalarFunction() noexcept = default;
    public:
        auto onAfterArgument(ExecutionContext & /*context*/) const -> TraversalEventOutcome override {
            SPR_FATAL_ERROR("arguments for function with no arguments");
        }

        auto execute(ExecutionContext & context) const -> bool override {
            if (!context.suppressEvaluation) {
                if constexpr (Op::hasContext) {
                 
                    Op op;
                    setContext(op, context);
                    context.returnedValue = std::move(op.result());
                } else {
                    context.returnedValue = std::move(Op().result());
                }
            }
            context.returnedExtent = Size{1, 1};
            return true;
        }
        
    private:
        template<class X = Op>
        requires(X::hasContext)
        static void setContext(X & op, ExecutionContext & context) {
            if constexpr (std::is_same_v<std::remove_cvref_t<typename X::Context>, NameManager>) {
                op.setContext(context.names());
            } else {
                static_assert(dependentFalse<X>, "context passing not defined");
            }
        }

    };

    #define SPR_MAP_FUNCTION(id, prefix, type) SPR_MAP_FUNCTION_ID_TO_IMPL(id, prefix, type::minArgs, type::maxArgs, FiniteScalarFunction<type>);

    SPR_MAP_FUNCTION(FunctionId::Address,       "ADDRESS",    ScalarAddress);
    SPR_MAP_FUNCTION(FunctionId::Ceil,          "CEIL",       ScalarCeil);
    SPR_MAP_FUNCTION(FunctionId::Date,          "DATE",       ScalarDate);
    SPR_MAP_FUNCTION(FunctionId::DateDif,       "DATEDIF",    ScalarDateDif);
    SPR_MAP_FUNCTION(FunctionId::Day,           "DAY",        ScalarDay);
    SPR_MAP_FUNCTION(FunctionId::Days,          "DAYS",       ScalarDays);
    SPR_MAP_FUNCTION(FunctionId::EDate,         "EDATE",      ScalarEDate);
    SPR_MAP_FUNCTION(FunctionId::EOMonth,       "EOMONTH",    ScalarEOMonth);
    SPR_MAP_FUNCTION(FunctionId::ErrorType,     "ERROR.TYPE", ScalarErrorType);
    SPR_MAP_FUNCTION(FunctionId::Find,          "FIND",       ScalarFind);
    SPR_MAP_FUNCTION(FunctionId::Floor,         "FLOOR",      ScalarFloor);
    SPR_MAP_FUNCTION(FunctionId::Hour,          "HOUR",       ScalarHour);
    SPR_MAP_FUNCTION(FunctionId::Int,           "INT",        ScalarInt);
    SPR_MAP_FUNCTION(FunctionId::IsBlank,       "ISBLANK",    ScalarIsBlank);
    SPR_MAP_FUNCTION(FunctionId::IsErr,         "ISERR",      ScalarIsErr);
    SPR_MAP_FUNCTION(FunctionId::IsError,       "ISERROR",    ScalarIsError);
    SPR_MAP_FUNCTION(FunctionId::IsEven,        "ISEVEN",     ScalarIsEven);
    SPR_MAP_FUNCTION(FunctionId::IsLogical,     "ISLOGICAL",  ScalarIsLogical);
    SPR_MAP_FUNCTION(FunctionId::IsNA,          "ISNA",       ScalarIsNA);
    SPR_MAP_FUNCTION(FunctionId::IsNonText,     "ISNONTEXT",  ScalarIsNonText);
    SPR_MAP_FUNCTION(FunctionId::IsNumber,      "ISNUMBER",   ScalarIsNumber);
    SPR_MAP_FUNCTION(FunctionId::IsOdd,         "ISODD",      ScalarIsOdd);
    SPR_MAP_FUNCTION(FunctionId::IsoWeekNum,    "ISOWEEKNUM", ScalarIsoWeekNum);
    SPR_MAP_FUNCTION(FunctionId::IsText,        "ISTEXT",     ScalarIsText);
    SPR_MAP_FUNCTION(FunctionId::Left,          "LEFT",       ScalarLeft);
    SPR_MAP_FUNCTION(FunctionId::Len,           "LEN",        ScalarLen);
    SPR_MAP_FUNCTION(FunctionId::Lower,         "LOWER",      ScalarLower);
    SPR_MAP_FUNCTION(FunctionId::Mid,           "MID",        ScalarMid);
    SPR_MAP_FUNCTION(FunctionId::Minute,        "MINUTE",     ScalarMinute);
    SPR_MAP_FUNCTION(FunctionId::Mod,           "MOD",        ScalarMod);
    SPR_MAP_FUNCTION(FunctionId::Month,         "MONTH",      ScalarMonth);
    SPR_MAP_FUNCTION(FunctionId::MRound,        "MROUND",     ScalarMRound);
    SPR_MAP_FUNCTION(FunctionId::Not,           "NOT",        ScalarNot);
    SPR_MAP_FUNCTION(FunctionId::Now,           "NOW",        ScalarNow);
    SPR_MAP_FUNCTION(FunctionId::Replace,       "REPLACE",    ScalarReplace);
    SPR_MAP_FUNCTION(FunctionId::Right,         "RIGHT",      ScalarRight);
    SPR_MAP_FUNCTION(FunctionId::Round,         "ROUND",      ScalarRound);
    SPR_MAP_FUNCTION(FunctionId::RoundDown,     "ROUNDDOWN",  ScalarRoundDown);
    SPR_MAP_FUNCTION(FunctionId::RoundUp,       "ROUNDUP",    ScalarRoundUp);
    SPR_MAP_FUNCTION(FunctionId::Second,        "SECOND",     ScalarSecond);
    SPR_MAP_FUNCTION(FunctionId::Sign,          "SIGN",       ScalarSign);
    SPR_MAP_FUNCTION(FunctionId::Substitute,    "SUBSTITUTE", ScalarSubstitute);
    SPR_MAP_FUNCTION(FunctionId::Time,          "TIME",       ScalarTime);
    SPR_MAP_FUNCTION(FunctionId::Today,         "TODAY",      ScalarToday);
    SPR_MAP_FUNCTION(FunctionId::Trim,          "TRIM",       ScalarTrim);
    SPR_MAP_FUNCTION(FunctionId::Upper,         "UPPER",      ScalarUpper);
    SPR_MAP_FUNCTION(FunctionId::Weekday,       "WEEKDAY",    ScalarWeekday);
    SPR_MAP_FUNCTION(FunctionId::WeekNum,       "WEEKNUM",    ScalarWeekNum);
    SPR_MAP_FUNCTION(FunctionId::Year,          "YEAR",       ScalarYear);
}
