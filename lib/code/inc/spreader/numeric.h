// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_NUMERIC_H_INCLUDED
#define SPR_HEADER_NUMERIC_H_INCLUDED

#include <spreader/compiler.h>

#include <cmath>
#include <cstring>
#include <concepts>
#include <limits>

#include <stdint.h>

namespace Spreader::Numeric {

    constexpr uint64_t divByZeroPattern = 0xFFF8000000000001;
    constexpr uint64_t notANumberPattern = 0x7FF8000000000000;
    
    constexpr uint64_t nonFiniteMask = 0x7FF0000000000000;
    
    template<uint64_t Pattern>
    constexpr bool isNotFinitePattern = ((Pattern & nonFiniteMask) == nonFiniteMask);

    constexpr double divByZero = SPR_BIT_CAST(double, divByZeroPattern);
    constexpr double notANumber = SPR_BIT_CAST(double, notANumberPattern);

    template<class T> 
    requires(std::is_arithmetic_v<T>)
    constexpr inline auto signum(T val) -> int {
        return (T(0) < val) - (val < T(0));
    }
    
    inline auto flooredModulo(double dividend, double divisor) {
        double modTruncated = std::fmod(dividend, divisor);
        double x = (signum(modTruncated) == -signum(divisor));
        return modTruncated + x * divisor;
    }

    enum class RoundMode {
        Round,
        Ceiling,
        Floor
    };

    template<RoundMode Mode>
    inline auto round(double val) noexcept -> double {
        if constexpr (Mode == RoundMode::Round)
            return std::round(val);
        else if constexpr (Mode == RoundMode::Ceiling)
            return std::ceil(val);
        else if constexpr (Mode == RoundMode::Floor)
            return std::floor(val);
    }
    
    inline auto roundTo15(double val) noexcept -> double {
        int exp;
        double mant = std::frexp(val, &exp);
        mant = std::round(mant * 1E15) / 1E15;
        return std::ldexp(mant, exp);
    }

    template<std::floating_point T>
    class KahanBabushkaKleinSum {
    public:
        constexpr void add(T val) noexcept {

            T t = m_sum + val;

            T c;
            if (abs(m_sum) >= abs(val))
                c = (m_sum - t) + val;
            else
                c = (val - t) + m_sum;
            
            m_sum = t;
            t = m_cs + c;

            T cc;
            if (abs(m_cs) >= abs(c))
                cc = (m_cs - t) + c;
            else
                cc = (c - t) + m_cs;
            
            m_cs = t;
            m_ccs = m_ccs + cc;
        }

        constexpr auto value() const noexcept -> T {
            return m_sum + m_cs + m_ccs;
        }
    private:
        T m_sum = 0;
        T m_cs  = 0;
        T m_ccs = 0;
    };

    template<std::floating_point T>
    class OnlineAverage {
    public:
        constexpr void add(T val) noexcept {
            ++m_count;
            const T increment = (val - m_current.value()) / m_count;
            m_current.add(increment);
        }

        constexpr auto value() const noexcept -> T {
            return m_current.value();
        }

    private:
        KahanBabushkaKleinSum<T> m_current;
        T m_count = 0;
    };
    
    template<std::floating_point T, bool IsSample>
    class OnlineStdDev {
    public:
        constexpr void add(T val) noexcept {
            const T countMinusOne = m_count++;
            const T delta = val - m_currentAverage.value();
            const T incrementOfAverage = delta / m_count;
            const T incrementOfDenomTimesSigmaSquared = (countMinusOne / m_count) * delta * delta;
            m_currentAverage.add(incrementOfAverage);
            m_currentDenomTimesSigmaSquared.add(incrementOfDenomTimesSigmaSquared);
        }

        constexpr auto value() const noexcept -> T {
            SPR_ASSERT_LOGIC(m_count > 0);
            T denom;
            if constexpr (IsSample) {
                if (m_count == 1)
                    return divByZero;
                denom = m_count - 1;
            } else {
                denom = m_count;
            }
            return std::sqrt(m_currentDenomTimesSigmaSquared.value() / denom);
        }

    private:
        KahanBabushkaKleinSum<T> m_currentAverage;
        KahanBabushkaKleinSum<T> m_currentDenomTimesSigmaSquared;
        T m_count = 0;
    };
    
    template<std::totally_ordered T>
    requires(std::is_same_v<decltype(std::numeric_limits<T>::min()), T>)
    class Max {
    public:
        constexpr void add(T val) noexcept {
            if (val > m_value)
                m_value = val;
        }
        constexpr auto value() const noexcept -> T {
            return m_value;
        }
    private:
        T m_value = std::numeric_limits<T>::min();
    };
    
    template<std::totally_ordered T>
    requires(std::is_same_v<decltype(std::numeric_limits<T>::max()), T>)
    class Min {
    public:
        constexpr void add(T val) noexcept {
            if (val < m_value)
                m_value = val;
        }
        constexpr auto value() const noexcept -> T {
            return m_value;
        }
    private:
        T m_value = std::numeric_limits<T>::max();
    };
}


#endif
