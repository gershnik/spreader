// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include <spreader/coerce.h>

#include "number-matcher.h"
#include "whitespace.h"

using namespace Spreader;


auto CoerceTo<Number>::operator()(const String & val) const noexcept -> std::optional<Number> {
    
    NumberMatcher<String::storage_type> matcher;
    auto access = String::char_access(val);
    
    auto last = access.end();
    auto first = skipWhiteSpace(access.begin(), last);
    if (first == last)
        return std::nullopt;
    if (!matcher.prefixMatch(first, last))
        return std::nullopt;
    first = skipWhiteSpace(first, last);
    if (first != last)
        return std::nullopt;
    double d = matcher.value();
    
    return Number::fromDouble(d, [](auto val) -> std::optional<Number> {
        if constexpr (std::is_same_v<decltype(val), Error>)
            return std::nullopt;
        else
            return val;
    });
}
    
auto CoerceTo<String>::operator()(Number val) const noexcept -> String {
    
    return val.toString();
}
