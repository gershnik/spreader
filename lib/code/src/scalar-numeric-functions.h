// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_SCALAR_NUMERIC_FUNCTIONS_H_INCLUDED
#define SPR_HEADER_SCALAR_NUMERIC_FUNCTIONS_H_INCLUDED

#include <spreader/number.h>
#include "scalar-operators.h"
#include "scalar-function.h"

namespace Spreader {

    constexpr auto operator+(Number lhs, Number rhs) noexcept -> double { return lhs.value() + rhs.value(); }
    constexpr auto operator-(Number lhs, Number rhs) noexcept -> double { return lhs.value() - rhs.value(); }
    constexpr auto operator*(Number lhs, Number rhs) noexcept -> double { return lhs.value() * rhs.value(); }
    constexpr auto operator/(Number lhs, Number rhs) noexcept -> double {
        if (rhs.value() == 0.) {
            return Numeric::divByZero;
        }
        return lhs.value() / rhs.value();
    }

    constexpr auto operator-(Number arg) noexcept -> Number { return Number(-arg.m_value); }
    constexpr auto operator+(Number arg) noexcept -> Number { return arg; }

    struct NumericFunctions {

        struct Percent {
            constexpr auto operator()(Number arg) const noexcept -> Number
                { return Number(arg.m_value / 100); }
        };

        struct Sign {
            constexpr auto operator()(Number arg) const noexcept -> Number
                { return Numeric::signum(arg.m_value); }
        };

        struct Power {
            auto operator()(Number val, Number exp) const noexcept -> double {
            
                if (val.value() < 0 && std::trunc(exp.value()) != exp.value())
                    return Numeric::notANumber;
                if (val.value() == 0) {
                    if (exp.value() == 0)
                        return Numeric::notANumber;
                    if (exp.value() < 0)
                        return Numeric::divByZero;
                }
                return std::pow(val.value(), exp.value());
            }
        };

        template<Numeric::RoundMode Mode>
        struct RoundToMultiple {
            constexpr auto operator()(Number val, Number mult) const noexcept -> double {
                if (val.value() == 0 || mult.value() == 0)
                    return 0.;
                //For reasons unknown Excel has different error conditions for MROUND vs. CEILING/FLOOR
                if constexpr (Mode != Numeric::RoundMode::Round) {
                    if (val.value() > 0 && mult.value() < 0)
                        return Numeric::notANumber;
                } else {
                    if (std::signbit(val.value()) != std::signbit(mult.value()))
                        return Numeric::notANumber;
                }
                return Numeric::round<Mode>(val.value() / mult.value()) * mult.value();
            }
        };

        template<Numeric::RoundMode Mode>
        struct RoundToPrecision {
            constexpr auto operator()(Number val, Number precision) const noexcept -> Number {
    
                double digits = std::round(precision.m_value);
                
                if (digits >= 0) {
                    if (digits > std::numeric_limits<double>::max_digits10)
                        digits = std::numeric_limits<double>::max_digits10;
                    
                    double power = std::pow(10., digits);
                    SPR_ASSERT_LOGIC(!std::isinf(power));
                    double multiplied = val.m_value * power;
                    if (std::isinf(multiplied))
                        return val;
                    multiplied = Numeric::round<Mode>(multiplied);
                    return Number(multiplied / power);
                    
                } else {
                    
                    if (digits < -std::numeric_limits<double>::max_digits10)
                        digits = -std::numeric_limits<double>::max_digits10;
                    
                    double power = std::pow(10., -digits);
                    SPR_ASSERT_LOGIC(power != 0);
                    double divided = val.m_value / power;
                    if (std::isinf(divided))
                        return val;
                    divided = Numeric::round<Mode>(divided);
                    return Number(divided * power);
                }
            }
        };

        struct Int {
            auto operator()(Number val) const noexcept -> Number {
                return Number(std::floor(val.m_value));
            }
        };

        struct Mod {
            auto operator()(Number val, Number divisor) const noexcept -> double{
                if (divisor.value() == 0)
                    return Numeric::divByZero;
                return Numeric::flooredModulo(val.value(), divisor.value());
            }
        };
    };

    using ScalarNegate =    ScalarDetail::UnaryOperator<std::negate<>,              true,  Number>;
    using ScalarAffirm =    ScalarDetail::UnaryOperator<std::identity,              true,  Number>;
    using ScalarPercent =   ScalarDetail::UnaryOperator<NumericFunctions::Percent,  false, Number>;

    using ScalarAdd =       ScalarDetail::BinaryOperator<std::plus<>,               Number>;
    using ScalarSubtract =  ScalarDetail::BinaryOperator<std::minus<>,              Number>;
    using ScalarMultiply =  ScalarDetail::BinaryOperator<std::multiplies<>,         Number>;
    using ScalarDivide =    ScalarDetail::BinaryOperator<std::divides<>,            Number>;
    using ScalarPower =     ScalarDetail::BinaryOperator<NumericFunctions::Power,   Number>;


    using ScalarCeil =      ScalarDetail::ScalarFunction<NumericFunctions::RoundToMultiple<Numeric::RoundMode::Ceiling>, Typelist<Number, Number>>;
    using ScalarFloor =     ScalarDetail::ScalarFunction<NumericFunctions::RoundToMultiple<Numeric::RoundMode::Floor>, Typelist<Number, Number>>;
    using ScalarInt =       ScalarDetail::ScalarFunction<NumericFunctions::Int, Typelist<Number>>;
    using ScalarMod =       ScalarDetail::ScalarFunction<NumericFunctions::Mod, Typelist<Number, Number>>;
    using ScalarMRound =    ScalarDetail::ScalarFunction<NumericFunctions::RoundToMultiple<Numeric::RoundMode::Round>, Typelist<Number, Number>>;
    using ScalarRound =     ScalarDetail::ScalarFunction<NumericFunctions::RoundToPrecision<Numeric::RoundMode::Round>, Typelist<Number, Number>>;
    using ScalarRoundUp =   ScalarDetail::ScalarFunction<NumericFunctions::RoundToPrecision<Numeric::RoundMode::Ceiling>, Typelist<Number, Number>>;
    using ScalarRoundDown = ScalarDetail::ScalarFunction<NumericFunctions::RoundToPrecision<Numeric::RoundMode::Floor>, Typelist<Number, Number>>;
    using ScalarSign =      ScalarDetail::ScalarFunction<NumericFunctions::Sign, Typelist<Number>>;
}


#endif 
