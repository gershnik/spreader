// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_SCALAR_INFO_FUNCTIONS_H_INCLUDED
#define SPR_HEADER_SCALAR_INFO_FUNCTIONS_H_INCLUDED

#include <spreader/scalar.h>
#include <spreader/name-manager.h>
#include "scalar-function.h"

namespace Spreader {

    struct ScalarInfo {


        struct ErrorType {

            auto operator()(Scalar val) const noexcept -> Scalar {

                return applyVisitor([](const auto & v) -> Scalar {

                    using T = std::remove_cvref_t<decltype(v)>;

                    if constexpr (std::is_same_v<T, Error>) {
                        auto num = std::underlying_type_t<Error>(v);
                        if (num >= 1 && num <= std::underlying_type_t<Error>(Error::Spill))
                            return Number(num);
                    }
                    return Error::InvalidArgs;

                }, val);
            };

        };
        
        struct IsBlank {
            auto operator()(Scalar val) const noexcept -> bool {
                return get<Scalar::Blank>(&val);
            }
        };
        struct IsErr {
            auto operator()(Scalar val) const noexcept -> bool {
                auto err = get<Error>(&val);
                return err && *err != Error::InvalidArgs;
            }
        };
        struct IsNA {
            auto operator()(Scalar val) const noexcept -> bool {
                auto err = get<Error>(&val);
                return err && *err == Error::InvalidArgs;
            }
        };
        struct IsError {
            auto operator()(Scalar val) const noexcept -> bool {
                return get<Error>(&val);
            }
        };
        struct IsLogical {
            auto operator()(Scalar val) const noexcept -> bool {
                return get<bool>(&val);
            }
        };
        struct IsText {
            auto operator()(Scalar val) const noexcept -> bool {
                return get<String>(&val);
            }
        };
        struct IsNonText {
            auto operator()(Scalar val) const noexcept -> bool {
                return !get<String>(&val);
            }
        };
        struct IsNumber {
            auto operator()(Scalar val) const noexcept -> bool {
                return get<Number>(&val);
            }
        };
        
        struct IsEven {
            auto operator()(Scalar val) const noexcept -> Scalar {
            
                return applyVisitor([](const auto & v) -> Scalar {
                    
                    using T = std::remove_cvref_t<decltype(v)>;

                    if constexpr (std::is_same_v<T, Number>) {
                        auto truncated = std::trunc(v.value());
                        return std::fmod(truncated, 2) == 0;
                    } else if constexpr (std::is_same_v<T, Scalar::Blank>) {
                        return true;
                    } else if constexpr (std::is_same_v<T, Error>) {
                        return v;
                    } else {
                        return Error::InvalidValue;
                    }
                    
                }, val);
                
            }
        };
        
        struct IsOdd {
            auto operator()(Scalar val) const noexcept -> Scalar {
            
                return applyVisitor([](const auto & v) -> Scalar {
                    
                    using T = std::remove_cvref_t<decltype(v)>;

                    if constexpr (std::is_same_v<T, Number>) {
                        auto truncated = std::trunc(v.value());
                        return std::fmod(truncated, 2) != 0;
                    } else if constexpr (std::is_same_v<T, Scalar::Blank>) {
                        return false;
                    } else if constexpr (std::is_same_v<T, Error>) {
                        return v;
                    } else {
                        return Error::InvalidValue;
                    }
                    
                }, val);
            }
        };

        class Address {
        public:
            using Context = NameManager;
            
            Address(const Context & names): m_names(names) {
            }

            auto operator()(Number row, Number col, Number type) -> Scalar {

                if (row.value() < 1 || row.value() >= CellGrid::maxSize().height)
                    return Error::InvalidValue;
                if (col.value() < 1 || col.value() >= CellGrid::maxSize().width)
                    return Error::InvalidValue;
                if (type.value() < 1 || type.value() >= 5)
                    return Error::InvalidValue;

                auto rowIdx = SizeType(row.value()) - 1;
                auto colIdx = SizeType(col.value()) - 1;
                auto typeVal = unsigned(type.value());

                StringBuilder builder;

                switch(typeVal) {
                    case 1: 
                        builder.append(U'$');
                        [[fallthrough]];
                    case 2: 
                        m_names.indexToColumn(colIdx, builder);
                        builder.append(U'$');
                        m_names.indexToRow(rowIdx, builder);
                        break;
                    case 3: 
                        builder.append(U'$');
                        [[fallthrough]];
                    case 4: 
                        m_names.indexToColumn(colIdx, builder);
                        m_names.indexToRow(rowIdx, builder);
                        break;
                    default:
                        SPR_ASSERT_LOGIC(false);
                }
                return builder.build();
            }

            auto operator()(Number row, Number col) -> Scalar {
                return (*this)(row, col, 1);
            }

        private:
            const NameManager & m_names;
        };
    };

    using ScalarAddress =          ScalarDetail::ScalarFunction<ScalarInfo::Address,          Typelist<Number, Number>, Typelist<Number>>;
    using ScalarErrorType =        ScalarDetail::ScalarFunction<ScalarInfo::ErrorType,        Typelist<Scalar>>;
    using ScalarIsBlank =          ScalarDetail::ScalarFunction<ScalarInfo::IsBlank,          Typelist<Scalar>>;
    using ScalarIsErr =            ScalarDetail::ScalarFunction<ScalarInfo::IsErr,            Typelist<Scalar>>;
    using ScalarIsEven =           ScalarDetail::ScalarFunction<ScalarInfo::IsEven,           Typelist<Scalar>>;
    using ScalarIsNA =             ScalarDetail::ScalarFunction<ScalarInfo::IsNA,             Typelist<Scalar>>;
    using ScalarIsError =          ScalarDetail::ScalarFunction<ScalarInfo::IsError,          Typelist<Scalar>>;
    using ScalarIsLogical =        ScalarDetail::ScalarFunction<ScalarInfo::IsLogical,        Typelist<Scalar>>;
    using ScalarIsText =           ScalarDetail::ScalarFunction<ScalarInfo::IsText,           Typelist<Scalar>>;
    using ScalarIsNonText =        ScalarDetail::ScalarFunction<ScalarInfo::IsNonText,        Typelist<Scalar>>;
    using ScalarIsNumber =         ScalarDetail::ScalarFunction<ScalarInfo::IsNumber,         Typelist<Scalar>>;
    using ScalarIsOdd =            ScalarDetail::ScalarFunction<ScalarInfo::IsOdd,            Typelist<Scalar>>;
}

#endif 
