// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_UTIL_H_INCLUDED
#define SPR_HEADER_UTIL_H_INCLUDED

#include <spreader/error-handling.h>

#include <utility>
#include <functional>

namespace Spreader {

    // Round size up to next multiple of alignment.
    static constexpr auto alignSize(size_t s, size_t alignment) noexcept -> size_t {
        SPR_ASSERT_INPUT(s + alignment > s);
        return (s + alignment - 1) & ~(alignment - 1);
    }
    
    namespace Util {
        template <class F, class Tuple, size_t... I>
        constexpr decltype(auto) ApplyImpl(F&& f, Tuple&& t, std::index_sequence<I...>)
        {
            // This implementation is valid since C++20 (via P1065R2)
            // In C++17, a constexpr counterpart of std::invoke is actually needed here
            return std::invoke(std::forward<F>(f), std::get<I>(std::forward<Tuple>(t))...);
        }
     
        template <class F, class Tuple, class Idx>
        inline constexpr bool ApplyIsNoexcept = false;
     
        template <class F, class Tuple, size_t... I>
        inline constexpr bool ApplyIsNoexcept<F, Tuple, std::index_sequence<I...>> =
            noexcept(std::invoke(std::declval<F>(), std::get<I>(std::declval<Tuple>())...));
     
    }
     
    template <size_t N, class F, class Tuple>
    requires(N <= std::tuple_size_v<std::remove_reference_t<Tuple>>)
    constexpr decltype(auto) ApplyN(F&& f, Tuple&& t) noexcept(Util::ApplyIsNoexcept<F, Tuple, std::make_index_sequence<N>>)
    {
        return Util::ApplyImpl(
            std::forward<F>(f), std::forward<Tuple>(t),
            std::make_index_sequence<N>{});
    }

    //tehcnically this could be done via find_if 
    //but this violates "say what you mean" principle as well
    //as forcing extra iterator comparison which may or may not be eliminated by optimizer
    template<class ItF, class ItL, class Op>
    auto forEachWhileTrue(ItF first, ItL last, Op op) -> bool {
        for(; first != last; ++first) {
            if (!op(*first))
                return false;
        }
        return true;
    }

    template<std::copy_constructible T>
    auto saveState(const T & val) noexcept(std::is_nothrow_copy_constructible_v<T>) -> T {
        return T(val);
    }

    template<std::move_constructible T>
    void restoreState(T & val, T && state) noexcept(std::is_nothrow_move_constructible_v<T>) {
        val = std::move(state);
    }
    
    template<bool Val, class... Args>
    static constexpr bool dependentBool = Val;
    
    template<class... Args>
    static constexpr bool dependentFalse = dependentBool<false, Args...>;
    
    template<class... Args>
    static constexpr bool dependentTrue = dependentBool<true, Args...>;

}

#endif
