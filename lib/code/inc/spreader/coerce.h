// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_COERCE_H_INCLUDED
#define SPR_HEADER_COERCE_H_INCLUDED

#include <spreader/types.h>
#include <spreader/number.h>

#include <variant>

namespace Spreader {

    template<class To> struct CoerceTo {};

    template<> struct CoerceTo<bool> {
        constexpr auto operator()(std::monostate) const noexcept -> bool { return false; }
        constexpr auto operator()(Number val) const noexcept -> bool { return val != 0; }
        
        template<class X> auto operator()(X val) const = delete;
    };
    template<> struct CoerceTo<Number> {
        constexpr auto operator()(std::monostate) const noexcept -> Number { return Number(0); }
        constexpr auto operator()(bool val) const noexcept -> Number { return Number(val); }
        auto operator()(const String & val) const noexcept -> std::optional<Number>;
        
        template<class X> auto operator()(X val) const = delete;
    };
    template<> struct CoerceTo<String> {
        auto operator()(std::monostate) const noexcept -> String { return SPRS(""); }
        auto operator()(bool val) const noexcept -> String { return val ? SPRS("TRUE") : SPRS("FALSE"); }
        auto operator()(Number val) const noexcept -> String;
        
        template<class X> auto operator()(X val) const = delete;
    };

    template<class From, class To> constexpr bool IsCoercible = std::is_invocable_r_v<To, CoerceTo<To>, From>;
    template<class From, class To> constexpr bool IsOptionallyCoercible = std::is_invocable_r_v<std::optional<To>, CoerceTo<To>, From>;
    
    template<class To, class From>
    requires(IsCoercible<std::remove_cvref_t<From>, To> || IsOptionallyCoercible<std::remove_cvref_t<From>, To>)
    decltype(auto) coerceTo(From && from) noexcept(noexcept(CoerceTo<To>()(std::forward<From>(from)))) {
        return CoerceTo<To>()(std::forward<From>(from));
    }
}

#endif
