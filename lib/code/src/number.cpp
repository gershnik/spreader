// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include <spreader/number.h>
#include <spreader/floating-decimal.h>

using namespace Spreader;

auto Number::toString() const -> String {
    return FloatingDecimal(*this).toString();
}
