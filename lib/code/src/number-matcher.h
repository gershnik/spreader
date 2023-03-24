// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_NUMBER_MATCHER_H_INCLUDED
#define SPR_HEADER_NUMBER_MATCHER_H_INCLUDED

#include <spreader/floating-decimal.h>

#include <string>

namespace Spreader {

    template<class Char>
    class NumberMatcher {
    public:
        template<class ItF, class ItL>
        requires(std::is_convertible_v<typename std::iterator_traits<ItF>::iterator_category, std::forward_iterator_tag> &&
                std::is_same_v<typename std::iterator_traits<ItF>::value_type, Char> &&
                std::equality_comparable_with<ItF, ItL>)
        auto prefixMatch(ItF & first, ItL last) noexcept -> bool  {
            
            if (first == last)
                return false;
            
            FloatingDecimal::Builder builder;
            
            bool hasDigits = false;
            bool hasDot = false;
            
            ItF cur = first;
            Char c = *cur;
            if (c == Char('-')) {
                builder.setNegative();
            } else if (c == Char('+')) {
                ;
            } else if (c == Char('.')) {
                hasDot = true;
                builder.markDecimalPoint();
            } else if (isDigit(c)) {
                hasDigits = true;
                (void)builder.addMantissaDigit(c - Char('0'));
            } else {
                return false;
            }
            
            for(++cur; cur != last; ++cur) {
                c = *cur;
                
                if (c == Char('.')) {
                    if (hasDot)
                        break;
                    
                    builder.markDecimalPoint();
                    hasDot = true;
                    continue;
                }
                
                if (isDigit(c)) {
                    if (!builder.addMantissaDigit(c - Char('0')))
                        return false;
                    hasDigits = true;
                    continue;
                }
                
                if (c == Char('e') || c == Char('E')) {
                    auto next = cur;
                    ++next;
                    if (next == last)
                        break;
                    c = *next;
                    if (c == Char('-') || c == Char('+')) {
                        if (++next == last)
                            break;
                        if (c == Char('-'))
                            builder.setExponentNegative();
                        c = *next;
                    }
                    if (isDigit(c)) {
                        for(cur = next; cur != last; ++cur) {
                            c = *cur;
                            if (!isDigit(c))
                                break;
                            if (!builder.addExponentDigit(c - Char('0')))
                                return false;
                        }
                    }
                }
                
                break;
            }
            
            if (!hasDigits) {
                return false;
            }
            
            m_value = builder.build().toDouble();
            first = cur;
            return true;
        }
        
        auto value() const noexcept -> double {
            return m_value;
        }
    private:
        static auto isDigit(Char c) noexcept -> bool {
            return c >= Char('0') && c <= Char('9');
        }
    private:
        double m_value;
    };

}


#endif 
