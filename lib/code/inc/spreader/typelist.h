// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_TYPELIST_H_INCLUDED
#define SPR_HEADER_TYPELIST_H_INCLUDED


namespace Spreader {

    template<class... Types>
    struct Typelist {
        static constexpr unsigned size = sizeof...(Types);
        
        template<template <class...> class Func>
        using WrapIn = Func<Types...>;
        
        template<class... Types1>
        using Append = Typelist<Types..., Types1...>;
        
        template<template <class> class Func>
        using Transform = Typelist<Func<Types>...>;

        template<class T>
        static constexpr bool contains = (std::is_same_v<Types, T> || ...);
    };

    template<>
    struct Typelist<> {
        
        static constexpr unsigned size = 0;
        
        template<template <class...> class Func>
        using WrapIn = Func<>;

        template<class... Types1>
        using Append  = Typelist<Types1...>;
        
        template<template <class> class Func>
        using Transform = Typelist<>;

        template<class T>
        static constexpr bool contains = false;
    };

    template<class LHS, class RHS> 
    struct DoCat;

    template<class... LHSTypes, class... RHSTypes>
    struct DoCat<Typelist<LHSTypes...>, Typelist<RHSTypes...>> {
        using Type = typename Typelist<LHSTypes...>::template Append<RHSTypes...>;
    };

    template<class LHS, class RHS> 
    using Cat = typename  DoCat<LHS, RHS>::Type;

}

#endif
