// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_SCALAR_OPERATORS_H_INCLUDED
#define SPR_HEADER_SCALAR_OPERATORS_H_INCLUDED

#include <spreader/scalar.h>


namespace Spreader {

    namespace ScalarDetail {

        template<class Op, bool IsPrefix, class Dest>
        struct UnaryOperator {
            static inline constexpr bool isPrefix = IsPrefix;

            template<class T>
            requires(std::is_same_v<std::remove_cvref_t<T>, Scalar>)
            auto handleArg(T && arg) const noexcept -> Scalar {
                return applyVisitorCoercedTo<Dest>([&](auto && val) -> Scalar {

                    using ArgType = std::remove_cvref_t<decltype(val)>;

                    if constexpr (std::is_same_v<Error, ArgType>) {
                        return val;
                    } else {
                        return Op()(std::forward<decltype(val)>(val));
                    }

                }, std::forward<T>(arg));
            }
        };

        template<class Op, class Dest>
        class BinaryOperator {

        public:
            Scalar result;

            template<class T>
            requires(std::is_same_v<std::remove_cvref_t<T>, Scalar>)
            void handleFirst(T && arg) noexcept  {
                applyVisitorCoercedTo<Dest>([&](auto && val) {
                    this->result = val;
                }, std::forward<T>(arg));
            }

            template<class T>
            requires(std::is_same_v<std::remove_cvref_t<T>, Scalar>)
            void handleSecond(T && arg) noexcept {

                if (auto * lhs = get<Dest>(&this->result)) {
                    
                    applyVisitorCoercedTo<Dest>([&](auto && val)  {
                        
                        using ArgType = std::remove_cvref_t<decltype(val)>;
                        
                        if constexpr (std::is_same_v<Error, ArgType>) {
                            this->result = val;
                        } else {
                            this->result = Op()(*lhs, std::forward<decltype(val)>(val));
                        }
                        
                    }, std::forward<T>(arg));
                }
            }

        };

        template<class Op>
        struct ComparisonOperator {

            Scalar result;

            template<class T>
            requires(std::is_same_v<std::remove_cvref_t<T>, Scalar>)
            void handleFirst(T && arg) noexcept  {
                this->result = std::forward<T>(arg);
            }

            template<class T>
            requires(std::is_same_v<std::remove_cvref_t<T>, Scalar>)
            void handleSecond(T && arg) noexcept {
                this->compare(this->result, std::forward<T>(arg), [&](auto res){
                    this->result = res;
                });
            }

            template<class LT, class RT, class Handler>
            requires(std::is_same_v<std::remove_cvref_t<LT>, Scalar> && std::is_same_v<std::remove_cvref_t<RT>, Scalar>)
            static auto compare(LT && lhs, RT && rhs, Handler handler) {
                return applyVisitor([&](auto && lhs, auto && rhs)  {

                    using LHS = std::remove_cvref_t<decltype(lhs)>;
                    using RHS = std::remove_cvref_t<decltype(rhs)>;

                    if constexpr (std::is_same_v<Error, LHS>) {
                        return handler(lhs);
                    } else if constexpr (std::is_same_v<Error, RHS>) {
                        return handler(rhs);
                    } else if constexpr (std::is_same_v<Scalar::Blank, LHS>) {
                        
                        if constexpr (std::is_same_v<Scalar::Blank, RHS>) {
                            return handler(Op()(CoerceTo<Number>()(lhs), CoerceTo<Number>()(rhs)));
                        } else {
                            return handler(Op()(CoerceTo<RHS>()(lhs), std::forward<decltype(rhs)>(rhs)));
                        }

                    } else if constexpr (std::is_same_v<bool, LHS>) {
                        
                        if constexpr (std::is_same_v<Scalar::Blank, RHS>) {
                            return handler(Op()(std::forward<LHS>(lhs), CoerceTo<bool>()(rhs)));
                        } else if constexpr (std::is_same_v<bool, RHS>) {
                            return handler(Op()(std::forward<decltype(lhs)>(lhs), std::forward<decltype(rhs)>(rhs)));
                        } else if constexpr (std::is_same_v<Number, RHS>) {
                            return handler(Op()(2, 0));
                        } else if constexpr (std::is_same_v<String, RHS>) {
                            return handler(Op()(2, 1));
                        }

                    } else if constexpr (std::is_same_v<Number, LHS>) {
                        
                        if constexpr (std::is_same_v<Scalar::Blank, RHS>) {
                            return handler(Op()(std::forward<decltype(lhs)>(lhs), CoerceTo<Number>()(rhs)));
                        } else if constexpr (std::is_same_v<bool, RHS>) {
                            return handler(Op()(0, 2));
                        } else if constexpr (std::is_same_v<Number, RHS>) {
                            return handler(Op()(std::forward<decltype(lhs)>(lhs), std::forward<decltype(rhs)>(rhs)));
                        } else if constexpr (std::is_same_v<String, RHS>) {
                            return handler(Op()(0, 1));
                        }

                    } else if constexpr (std::is_same_v<String, LHS>) {
                        
                        if constexpr (std::is_same_v<Scalar::Blank, RHS>) {
                            return handler(Op()(std::forward<decltype(lhs)>(lhs), CoerceTo<String>()(rhs)));
                        } else if constexpr (std::is_same_v<bool, RHS>) {
                            return handler(Op()(1, 2));
                        } else if constexpr (std::is_same_v<Number, RHS>) {
                            return handler(Op()(1, 0));
                        } else if constexpr (std::is_same_v<String, RHS>) {
                            return handler(Op()(std::forward<decltype(lhs)>(lhs), std::forward<decltype(rhs)>(rhs)));
                        }

                    } 

                }, std::forward<LT>(lhs), std::forward<RT>(rhs));
            }

        };

    }

    
    using ScalarEquals =          ScalarDetail::ComparisonOperator<std::equal_to<>>;
    using ScalarNotEquals =       ScalarDetail::ComparisonOperator<std::not_equal_to<>>;
    using ScalarGreater =         ScalarDetail::ComparisonOperator<std::greater<>>;
    using ScalarGreaterEquals =   ScalarDetail::ComparisonOperator<std::greater_equal<>>;
    using ScalarLess =            ScalarDetail::ComparisonOperator<std::less<>>;
    using ScalarLessEquals =      ScalarDetail::ComparisonOperator<std::less_equal<>>;
}

#endif
