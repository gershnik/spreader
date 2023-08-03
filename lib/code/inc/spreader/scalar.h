// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_SCALAR_H_INCLUDED
#define SPR_HEADER_SCALAR_H_INCLUDED

#include <spreader/error-handling.h>
#include <spreader/error.h>
#include <spreader/number.h>
#include <spreader/coerce.h>
#include <spreader/typelist.h>
#include <spreader/floating-decimal.h>
#include <spreader/compiler.h>

#include <variant>

#if SPR_TESTING
    #include <iostream>
#endif

namespace Spreader {

    class Scalar {
    public:
        using Blank = std::monostate;

        using Types = Typelist<
            Blank,
            bool,
            Number,
            String,
            Error
        >;

    private:
        using ImplType = Types::WrapIn<std::variant>;

    public:
        constexpr Scalar() noexcept = default;

        template<class T>
        requires(std::is_constructible_v<ImplType, T &&>)
        constexpr Scalar(T && val) noexcept(std::is_nothrow_constructible_v<ImplType, T &&>) : 
            m_impl(std::forward<T>(val)) 
        {}
        
        constexpr Scalar(std::integral auto val) noexcept requires(!std::is_constructible_v<ImplType, decltype(val)>) :
            m_impl(Number(val))
        {}
        
        constexpr Scalar(double val) noexcept {
            Number::fromDouble(val, [&](auto v) {
                m_impl.emplace<decltype(v)>(v);
            });
        }

        template<class T>
        requires(Types::contains<std::remove_cvref_t<T>>)
        auto assign(T && arg) -> std::remove_cvref_t<T> & {
            return m_impl.emplace<std::remove_cvref_t<T>>(std::forward<T>(arg));
        }

        friend void swap(Scalar & lhs, Scalar & rhs) {
            using std::swap;
            swap(lhs.m_impl, rhs.m_impl);
        }

        constexpr auto isBlank() const noexcept -> bool {
            return std::get_if<Blank>(&m_impl);
        }

        template<class T>
        friend constexpr auto get(const Scalar & v) noexcept -> const T & {
            auto p = std::get_if<T>(&v.m_impl);
            SPR_ASSERT_INPUT(p);
            return *p;
        }
        template<class T>
        friend constexpr auto get(Scalar & v) noexcept -> T & {
            auto p = std::get_if<T>(&v.m_impl);
            SPR_ASSERT_INPUT(p);
            return *p;
        }
        template<class T>
        friend constexpr auto get(Scalar && v) noexcept -> T && {
            auto p = std::get_if<T>(&v.m_impl);
            SPR_ASSERT_INPUT(p);
            return std::move(*p);
        }
        template<class T>
        friend constexpr auto get(const Scalar * v) noexcept -> const T * {
            return std::get_if<T>(v ? &v->m_impl : nullptr);
        }
        template<class T>
        friend constexpr auto get(Scalar * v) noexcept -> T * {
            return std::get_if<T>(v ? &v->m_impl : nullptr);
        }

        template<class Visitor, class... Scalars>
        friend auto applyVisitor(Visitor && visitor, Scalars && ...values) 
        requires(std::is_same_v<std::remove_cvref_t<Scalars>, Scalar> && ...) {
            return std::visit(std::forward<Visitor>(visitor), std::forward<Scalars>(values).m_impl...);
        }

        friend auto operator==(const Scalar & lhs, const Scalar & rhs) noexcept -> bool {
            return applyVisitor([](auto && lhs, auto && rhs) -> bool {

                using LHS = std::remove_cvref_t<decltype(lhs)>;
                using RHS = std::remove_cvref_t<decltype(rhs)>;

                if constexpr (std::is_same_v<LHS, RHS>) 
                    return lhs == rhs;
                else
                    return false;
            }, lhs, rhs);
        }
        friend auto operator!=(const Scalar & lhs, const Scalar & rhs) noexcept -> bool {
            return applyVisitor([](auto && lhs, auto && rhs) -> bool {

                using LHS = std::remove_cvref_t<decltype(lhs)>;
                using RHS = std::remove_cvref_t<decltype(rhs)>;

                if constexpr (std::is_same_v<LHS, RHS>) 
                    return lhs != rhs;
                else
                    return true;
            }, lhs, rhs);
        }


        void reconstruct(StringBuilder & dest) const {
            struct Reconstructor {
                StringBuilder & dest;

                void operator()(Scalar::Blank) const {
                    ;
                }
                void operator()(bool value) const {
                    dest.append(value ? U"TRUE" : U"FALSE");
                }
                void operator()(Number value) const {
                    FloatingDecimal(value).appendTo(dest);
                }
                void operator()(const String & value) {
                    dest.append(U'"');
                    for(char32_t c: String::utf32_view(value)) {
                        if (c == U'"')
                            dest.append(c);
                        dest.append(c);
                    }
                    dest.append(U'"');
                }
                void operator()(Error err) {
                    dest.append(toString(err));
                }
            };
            
            applyVisitor(Reconstructor{dest}, *this);
        }

        template<class Dest, class Visitor, class S>
        requires(std::is_same_v<std::remove_cvref_t<S>, Scalar>)
        friend auto applyVisitorCoercedTo(Visitor && visitor, S && arg) {
            return applyVisitor([&visitor](auto && val) {

                using T = std::remove_cvref_t<decltype(val)>;

                if constexpr (std::is_same_v<T, Error> || std::is_same_v<T, Dest>)
                    return std::forward<Visitor>(visitor)(val);
                else if constexpr (IsCoercible<T, Dest>)
                    return std::forward<Visitor>(visitor)(coerceTo<Dest>(val));
                else if constexpr (IsOptionallyCoercible<T, Dest>) {
                    if (auto coerced = coerceTo<Dest>(val))
                        return std::forward<Visitor>(visitor)(*coerced);
                    else 
                        return std::forward<Visitor>(visitor)(Error::InvalidValue);
                } else 
                    return std::forward<Visitor>(visitor)(Error::InvalidValue);

            }, std::forward<decltype(arg)>(arg));
        }
        
    private:
        ImplType m_impl;
    };

#if SPR_TESTING
    
    inline auto operator<<(std::ostream & str, const Scalar & rhs) -> std::ostream & {
        applyVisitor([&str](auto & val) {
            using T = std::remove_cvref_t<decltype(val)>;

            if constexpr (std::is_same_v<T, Scalar::Blank>) 
                str << "<empty>";
            else
                str << val;
        }, rhs);
        return str;
    }

#endif

}

#endif
