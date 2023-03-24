// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_SCALAR_BOOLEAN_FUNCTIONS_H_INCLUDED
#define SPR_HEADER_SCALAR_BOOLEAN_FUNCTIONS_H_INCLUDED

#include "scalar-function.h"

namespace Spreader {

    struct BooleanFunctions {

        struct Not {
            constexpr auto operator()(bool value) const noexcept -> bool{
                return !value;
            }
        };

    };

    using ScalarNot =       ScalarDetail::ScalarFunction<BooleanFunctions::Not,         Typelist<bool>>;

}


#endif


