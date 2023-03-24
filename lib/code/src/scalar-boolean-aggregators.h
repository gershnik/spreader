// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_SCALAR_BOOLEAN_AGGREGATORS_H_INCLUDED
#define SPR_HEADER_SCALAR_BOOLEAN_AGGREGATORS_H_INCLUDED

#include "scalar-aggregators.h"

namespace Spreader {

    struct BooleanAgregators {

        struct And {

            void add(bool val) noexcept {

                if (get<Error>(&result)) {
                    result = val;
                } else if (!val) {
                    result = false;
                }
            }

            auto value() const noexcept -> Scalar {
                return result;
            }

            Scalar result{Error::InvalidValue};
        };
        
        struct Or {

            void add(bool val) noexcept {

                if (get<Error>(&result)) {
                    result = val;
                } else if (val) {
                    result = true;
                }
            }

            auto value() const noexcept -> Scalar {
                return result;
            }

            Scalar result{Error::InvalidValue};
        };
        
        struct XOr {

            void add(bool val) noexcept {
                
                result = applyVisitor([val](auto current) -> bool {
                    
                    using T = std::remove_cvref_t<decltype(current)>;
                    
                    if constexpr (std::is_same_v<T, Error>) {
                        return val;
                    } else if constexpr (std::is_same_v<T, bool>) {
                        return current ^ val;
                    } else {
                        SPR_FATAL_ERROR("unexpected scalar type");
                    }
                    
                }, result);
            }

            auto value() const noexcept -> Scalar {
                return result;
            }

            Scalar result{Error::InvalidValue};
        };
    };

    using ScalarAnd =       ScalarDetail::BooleanAggregator<BooleanAgregators::And>;
    using ScalarOr =        ScalarDetail::BooleanAggregator<BooleanAgregators::Or>;
    using ScalarXOr =       ScalarDetail::BooleanAggregator<BooleanAgregators::XOr>;
}

#endif 

