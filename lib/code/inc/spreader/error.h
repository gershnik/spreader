// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_ERROR_H_INCLUDED
#define SPR_HEADER_ERROR_H_INCLUDED

#include <spreader/types.h>

#if SPR_TESTING
    #include <iostream>
#endif

namespace Spreader {

    enum class Error {
        //the numeric values need to be aligned with expected ERROR.TYPE return values
        NullRange = 1,    //#NULL!
        DivisionByZero,   //#DIV/0!
        InvalidValue,     //#VALUE!
        InvalidReference, //#REF!
        InvalidName,      //#NAME?
        NotANumber,       //#NUM!
        InvalidArgs,      //#N/A
        GettingData,      //#GETTING_DATA
        Spill,            //#SPILL!
        
        //any non-Excel compatible errors
        InvalidFormula,   //#ERROR!
    };

    auto toString(Error err) -> String;

#if SPR_TESTING
    
    inline auto operator<<(std::ostream & str, Error rhs) -> std::ostream & {
        return str << toString(rhs);
    }
    
#endif
}

#endif 
