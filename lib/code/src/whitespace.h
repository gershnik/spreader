// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_WHITESPACE_H_INCLUDED
#define SPR_HEADER_WHITESPACE_H_INCLUDED

namespace Spreader {

    auto skipWhiteSpace(auto first, auto last) -> decltype(first) {
    
        using Char = decltype(*first);
        
        return std::find_if(first, last, [](Char c) {
            return c != Char(' ') && c != Char('\t') && c != Char('\n');
        });
    }

}


#endif