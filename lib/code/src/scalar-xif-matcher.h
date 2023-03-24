// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_SCALAR_XIF_MATCHER_H_INCLUDED
#define SPR_HEADER_SCALAR_XIF_MATCHER_H_INCLUDED

#include <spreader/scalar.h>

#include "wildcarder.h"
#include "number-matcher.h"

namespace Spreader {

    class XIfMatcher {
        
    private:
        template<class Char> struct Constants;
        
        using CharType = String::storage_type;
        
        
        struct EqualTo {
          
            template<class Comp, class Val>
            auto operator()(const XIfMatcher &, const Comp &, const Val &) const noexcept -> bool
                { return false; }
            
            template<class T>
            auto operator()(const XIfMatcher &, const T & comp, const T & val) const noexcept -> bool
                { return comp == val; }
            
            auto operator()(const XIfMatcher & owner, const String & comp, const String & val) const noexcept -> bool {
                auto upperText = val.to_upper();
                return owner.m_wildcarder.match(String::char_access(comp), String::char_access(upperText));
            }
            
            auto operator()(const XIfMatcher &, Scalar::Blank, const String & val) const noexcept -> bool {
                
                //this is only possible if the condition is an empty string or blank scalar (see XIfMatcher constructor)
                //so we know this isn't "<op><empty>" condition
                return val.empty();
            }
            
            auto operator()(const XIfMatcher &, Number comp, const String & val) const noexcept -> bool {
                
                if (auto num = CoerceTo<Number>()(val)) {
                    return comp == *num;
                }
                return false;
            }
        };
        
        struct NotEqualTo {
            
            auto operator()(const XIfMatcher & owner, const auto & comp, const auto & val) const noexcept -> bool
                { return !EqualTo()(owner, comp, val); }
        };
        
        template<class Rel>
        struct Relational {
            template<class Comp, class Val>
            auto operator()(const XIfMatcher &, const Comp &, const Val &) const noexcept -> bool
                { return false; }
            
            auto operator()(const XIfMatcher &, Number comp, Number val) const noexcept -> bool
                { return Rel()(val, comp); }
            
            auto operator()(const XIfMatcher &, bool comp, bool val) const noexcept -> bool
                { return Rel()(val, comp); }
            
            auto operator()(const XIfMatcher &, const String & comp, const String & val) const noexcept -> bool {
                return Rel()(val.to_upper(), comp);
            }
        };
        
        using Less = Relational<std::less<>>;
        using LessEqual = Relational<std::less_equal<>>;
        using Greater = Relational<std::greater<>>;
        using GreaterEqual = Relational<std::greater_equal<>>;
        
    public:
        template<class T>
        requires(std::is_same_v<std::remove_cvref_t<T>, Scalar>)
        XIfMatcher(T && cond):
            m_wildcarder(CharType('~')) {

            applyVisitor([&](auto && val) {

                using ValType = std::remove_cvref_t<decltype(val)>;

                if constexpr (std::is_same_v<ValType, String>) {

                    if (!val.empty())
                        parseMatchString(val);

                } else {
                    m_compValue = val;
                }

            }, std::forward<T>(cond));
        }
        
        template<class T>
        requires(std::is_convertible_v<std::remove_cvref_t<T>, Scalar> &&
                 !std::is_same_v<std::remove_cvref_t<T>, Scalar>)
        XIfMatcher(T && cond): XIfMatcher(Scalar(std::forward<T>(cond)))
        {}

        template<class T>
        requires(std::is_same_v<std::remove_cvref_t<T>, Scalar>)
        auto operator()(T && arg) const noexcept -> bool {
            
            return applyVisitor([&](auto && comp, auto && val) {
                return visit([&](const auto & op) {
                    return op(*this, std::forward<decltype(comp)>(comp), std::forward<decltype(val)>(val));
                }, m_op);
            }, m_compValue, std::forward<T>(arg));
        }
        
        template<class T>
        requires(std::is_convertible_v<std::remove_cvref_t<T>, Scalar> &&
                 !std::is_same_v<std::remove_cvref_t<T>, Scalar>)
        auto operator()(T && arg) const noexcept -> bool {
            return (*this)(Scalar(std::forward<T>(arg)));
        }

    private:
        void parseMatchString(const String & str);

    private:
        Scalar m_compValue;
        std::variant<EqualTo, NotEqualTo, Less, LessEqual, Greater, GreaterEqual> m_op;
        mutable Wildcarder<CharType, String::char_access::iterator, String::char_access::iterator> m_wildcarder;
    };
}


#endif 
