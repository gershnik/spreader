// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include <spreader/error.h>
#include <spreader/error-handling.h>

using namespace Spreader;

auto Spreader::toString(Error err) -> String {
    switch(err) {
        case Error::NullRange:         return SPRS("#NULL!");
        case Error::DivisionByZero:    return SPRS("#DIV/0!");
        case Error::InvalidValue:      return SPRS("#VALUE!");
        case Error::InvalidReference:  return SPRS("#REF!");
        case Error::InvalidName:       return SPRS("#NAME?");
        case Error::NotANumber:        return SPRS("#NUM!");
        case Error::InvalidArgs:       return SPRS("#N/A");
        case Error::GettingData:       return SPRS("#GETTING_DATA");
        case Error::Spill:             return SPRS("#SPILL!");
        case Error::InvalidFormula:    return SPRS("#ERROR!");
    }
    SPR_FATAL_ERROR("unhandled enum value");
}

