// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

// Adapted from Ryu & Ryu Printf library (https://github.com/ulfjack/ryu)
// Copyright 2018 Ulf Adams
// See Acknowledgments.md in the root of this repository for details

#ifndef SPR_HEADER_FLOATING_DECIMAL_H_INCLUDED
#define SPR_HEADER_FLOATING_DECIMAL_H_INCLUDED

#include <spreader/compiler.h>
#include <spreader/types.h>
#include <spreader/error-handling.h>
#include <spreader/number.h>

#include <span>
#include <array>
#include <stdint.h>

namespace Spreader {

    class FloatingDecimal {
    private:
        using Char = String::storage_type;

        static constexpr size_t maxFixedLen = 25 /* e.g. -0.0000012345678901234567 */;
        static constexpr size_t maxExpLen = 24 /*  e.g. -1.2345678901234567E-324 */;
        static constexpr size_t maxOutputLen = std::max(maxFixedLen, maxExpLen);
        
        static constexpr int32_t minDecPower = -324;
        static constexpr int32_t maxDecPower = 309;
        static constexpr uint64_t maxDecimalMantissa = 17976931348623157;
        
    public:
        explicit FloatingDecimal(Number n) noexcept;

        void appendTo(StringBuilder & dest) const {
            std::array<Char, maxOutputLen> buf;
            auto len = format(buf);
            dest.append(&buf[0], len);
        }

        auto toString() const -> String {
            std::array<Char, maxOutputLen> buf;
            auto len = format(buf);
            return String(&buf[0], len);
        }

        auto toDouble() const noexcept -> double;
        
        class Builder {
        public:
            void setNegative() noexcept {
                SPR_ASSERT_INPUT(m_mantissa == 0);
                m_negative = true;
            }
            
            auto addMantissaDigit(unsigned digit) noexcept -> bool {
                SPR_ASSERT_INPUT(digit <= 9);
                SPR_ASSERT_INPUT(m_exponent == 0);
                uint64_t newMantissa = m_mantissa * 10 + digit;
                if (newMantissa > maxDecimalMantissa)
                    return false;
                m_mantissa = newMantissa;
                if (m_inFraction)
                    m_fractionCount++;
                if (m_mantissa != 0)
                    ++m_mantissaDigitsCount;
                return true;
            }
            
            void markDecimalPoint() noexcept {
                SPR_ASSERT_INPUT(!m_inFraction);
                m_inFraction = true;
            }
            
            void setExponentNegative() noexcept {
                SPR_ASSERT_INPUT(m_exponent == 0);
                m_exponentSign = -1;
            }
            
            auto addExponentDigit(unsigned digit) noexcept -> bool {
                SPR_ASSERT_INPUT(digit <= 9);
                int32_t newExponent = m_exponent * 10 + m_exponentSign * int32_t(digit);
                int32_t fullExponent = newExponent - m_fractionCount + (m_mantissaDigitsCount - 1);
                if (fullExponent < minDecPower || fullExponent >= maxDecPower) {
                    return false;
                }
                m_exponent = newExponent;
                return true;
            }
            
            auto build() const noexcept -> FloatingDecimal {
                
                return FloatingDecimal(m_negative, m_mantissa, m_exponent - m_fractionCount);
            }
            
        private:
            bool m_negative = false;
            bool m_inFraction = false;
            int8_t m_mantissaDigitsCount = 0;
            int8_t m_exponentSign = 1;
            uint64_t m_mantissa = 0;
            int32_t m_exponent = 0;
            int32_t m_fractionCount = 0;
        };

    private:
        constexpr FloatingDecimal(bool negative, uint64_t mantissa, int32_t exponent) noexcept :
            m_negative(negative),
            m_mantissa(mantissa),
            m_exponent(exponent)
        {}
        
        auto format(std::span<Char, maxOutputLen> output) const noexcept -> unsigned;
        
        auto getMantissaDigits(std::span<Char, 17> dest) const noexcept -> unsigned;
        static auto formatExponent(int32_t exp, std::span<Char, 4> dest) noexcept -> unsigned;

        static auto constructSmall(uint64_t ieeeMantissa, uint32_t ieeeExponent, FloatingDecimal & res) noexcept -> bool;
        static void constructGeneral(uint64_t ieeeMantissa, uint32_t ieeeExponent, FloatingDecimal & res) noexcept;

        
    private:
        bool m_negative;
        uint64_t m_mantissa;
        // Decimal exponent's range is -324 to 308
        // inclusive, and can fit in a short if needed.
        int32_t m_exponent;
    };
}


#endif
