// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_SCALAR_NUMERIC_AGGREGATORS_H_INCLUDED
#define SPR_HEADER_SCALAR_NUMERIC_AGGREGATORS_H_INCLUDED

#include <spreader/numeric.h>

#include "scalar-aggregators.h"

namespace Spreader {

    using ScalarSum =       ScalarDetail::NumericAggregator<Numeric::KahanBabushkaKleinSum<double>, /*AllSemantics*/false>;
    using ScalarAverage =   ScalarDetail::NumericAggregator<Numeric::OnlineAverage<double>, /*AllSemantics*/false>;
    using ScalarAverageA =  ScalarDetail::NumericAggregator<Numeric::OnlineAverage<double>, /*AllSemantics*/true>;
    using ScalarStdDev =    ScalarDetail::NumericAggregator<Numeric::OnlineStdDev<double, /*IsSample*/true>, /*AllSemantics*/false>;
    using ScalarStdDevA =   ScalarDetail::NumericAggregator<Numeric::OnlineStdDev<double, /*IsSample*/true>, /*AllSemantics*/true>;
    using ScalarStdDevP =   ScalarDetail::NumericAggregator<Numeric::OnlineStdDev<double, /*IsSample*/false>, /*AllSemantics*/false>;
    using ScalarStdDevPA =  ScalarDetail::NumericAggregator<Numeric::OnlineStdDev<double, /*IsSample*/false>, /*AllSemantics*/true>;
    using ScalarMax =       ScalarDetail::NumericAggregator<Numeric::Max<double>, /*AllSemantics*/false>;
    using ScalarMaxA =      ScalarDetail::NumericAggregator<Numeric::Max<double>, /*AllSemantics*/true>;
    using ScalarMin =       ScalarDetail::NumericAggregator<Numeric::Min<double>, /*AllSemantics*/false>;
    using ScalarMinA =      ScalarDetail::NumericAggregator<Numeric::Min<double>, /*AllSemantics*/true>;
    using ScalarCount =     ScalarDetail::NumericCounter</*AllSemantics*/false>;
    using ScalarCountA =    ScalarDetail::NumericCounter</*AllSemantics*/true>;
}

#endif
