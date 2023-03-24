// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_NUMBER_H_INCLUDED
#define SPR_HEADER_NUMBER_H_INCLUDED

#include <spreader/error.h>
#include <spreader/compiler.h>
#include <spreader/numeric.h>

#include <bit>
#include <cmath>
#include <concepts>

#if SPR_TESTING
    #include <iostream>
#endif

namespace Spreader {
    class Number {
        friend struct NumericFunctions;
        friend struct DateFunctions;
        friend class FloatingDecimal;
    public:
        constexpr Number() = default;
        
        constexpr Number(std::integral auto val): m_value(double(val))
        {}
        
        template<uint64_t Pattern>
        requires(!Numeric::isNotFinitePattern<Pattern>)
        static constexpr auto bitPatternConstant() {
            return Number(SPR_BIT_CAST(double, Pattern));
        }

        static constexpr decltype(auto) fromDouble(double val, auto && op) {
            uint64_t bitpattern = SPR_BIT_CAST(uint64_t, val);
            if ((bitpattern & Numeric::nonFiniteMask) == Numeric::nonFiniteMask) {
                if (bitpattern == Numeric::divByZeroPattern)
                    return op(Error::DivisionByZero);
                else
                    return op(Error::NotANumber);
            } else {
                return op(Number(val));
            }
        }

        friend constexpr auto operator==(Number lhs, Number rhs) noexcept -> bool = default;
        friend constexpr auto operator!=(Number lhs, Number rhs) noexcept -> bool = default;
        friend constexpr auto operator<(Number lhs, Number rhs) noexcept -> bool  { return lhs.m_value <  rhs.m_value; }
        friend constexpr auto operator<=(Number lhs, Number rhs) noexcept -> bool { return lhs.m_value <= rhs.m_value; }
        friend constexpr auto operator>(Number lhs, Number rhs) noexcept -> bool  { return lhs.m_value >  rhs.m_value; }
        friend constexpr auto operator>=(Number lhs, Number rhs) noexcept -> bool { return lhs.m_value >= rhs.m_value; }
        friend constexpr auto operator<=>(Number lhs, Number rhs) noexcept -> std::strong_ordering {
            auto res = -(lhs.m_value < rhs.m_value) + (lhs.m_value > rhs.m_value);
            switch (res) {
                case -1: return std::strong_ordering::less;
                case  0: return std::strong_ordering::equal;
                case  1: return std::strong_ordering::greater;
                default: SPR_UNREACHABLE;
            }
        }
        
        friend constexpr auto operator-(Number arg) noexcept -> Number;
        
        constexpr auto value() const noexcept -> double
            { return m_value; }
        
        auto toString() const -> String;
    private:
        constexpr Number(double val): m_value(val) 
        {}
           
    private:
        double m_value = 0;
    };
    
    #define SPRN(x) Number::bitPatternConstant<SPR_BIT_CAST(uint64_t, (x))>()

#if SPR_TESTING
    inline auto operator<<(std::ostream & str, Number rhs) -> std::ostream & {
        return str << rhs.toString();
    }
#endif
}

#endif 
