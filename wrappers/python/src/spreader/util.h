// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef HEADER_PYWRAPPER_UTIL_H_INCLUDED
#define HEADER_PYWRAPPER_UTIL_H_INCLUDED

#include <tuple>
#include <optional>
#include <cstdlib>

template <class Dest, class Src, class Func, size_t... Indices>
auto doTupleTransform(Src && src, Func func, std::index_sequence<Indices...>) -> Dest {
    return Dest(func(std::get<Indices>(std::forward<Src>(src)))...);  
}


template <class Dest, class Src, class Func>
requires(std::tuple_size_v<Src> == std::tuple_size_v<Dest>)
auto tupleTransform(Src && src, Func func) -> Dest {
    return doTupleTransform<Dest>(std::forward<Src>(src), func, std::make_index_sequence<std::tuple_size_v<Src>>());
}

template <class Dest, template<size_t> class Func, class Src, size_t... Indices>
auto doTupleTransformIndexed(Src && src, std::index_sequence<Indices...>) -> Dest {
    return Dest(Func<Indices>()(std::get<Indices>(std::forward<Src>(src)))...);  
}


template <class Dest, template<size_t> class Func, class Src>
requires(std::tuple_size_v<Src> == std::tuple_size_v<Dest>)
auto tupleTransformIndexed(Src && src) -> Dest {
    return doTupleTransformIndexed<Dest, Func>(std::forward<Src>(src), std::make_index_sequence<std::tuple_size_v<Src>>());
}


#endif
