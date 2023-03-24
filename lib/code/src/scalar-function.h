// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_SCALAR_FUNCTION_H_INCLUDED
#define SPR_HEADER_SCALAR_FUNCTION_H_INCLUDED

#include <spreader/scalar.h>

#include <tuple>

namespace Spreader {

    namespace ScalarDetail {
        
        template<class Op>
        static auto detectContext(typename Op::Context *) -> std::true_type;
        template<class Op>
        static auto detectContext(...) -> std::false_type;
        
        template<class Op>
        static constexpr bool hasContext = decltype(detectContext<Op>(nullptr))::value;
        
        template<class Op, bool HasContext=hasContext<Op>>
        class ScalarFunctionBase;
        
        template<class Op>
        class ScalarFunctionBase<Op, true> {
        public:
            static constexpr bool hasContext = true;
            using Context = typename Op::Context;
            
            void setContext(Context & context) {
                m_context = context;
            }
            
        protected:
            auto makeOp() const -> Op {
                return Op(m_context);
            }
        private:
            Context m_context;
        };
        
        template<class Op>
        class ScalarFunctionBase<Op, false> {
        public:
            static constexpr bool hasContext = false;
        protected:
            auto makeOp() const -> Op {
                return Op();
            }
        };

        template<class Op, class RequiredTL, class OptionalTL = Typelist<>>
        //requires(ugh)
        class ScalarFunction : public ScalarFunctionBase<Op> {
        private:
            using TupleType = typename Cat<RequiredTL, OptionalTL>:: template WrapIn<std::tuple>;
        public:
            static constexpr unsigned minArgs = RequiredTL::size;
            static constexpr unsigned maxArgs = RequiredTL::size + OptionalTL::size;
            
            template<unsigned I = 0>
            void handleArgument(unsigned idx, const Scalar & arg) {
                if (idx == I) {

                    m_argCount = idx + 1;

                    using ParameterType = typename std::tuple_element<I, decltype(this->m_args)>::type;

                    if constexpr (std::is_same_v<ParameterType, Scalar>) {
                        get<I>(this->m_args) = arg;
                    } else {
                        applyVisitorCoercedTo<ParameterType>([&](auto && val) {

                            using ArgType = std::remove_cvref_t<decltype(val)>;

                            if constexpr (std::is_same_v<Error, ArgType>) {
                                this->m_error = val;
                            } else {
                                get<I>(this->m_args) = val;
                            }

                        }, arg);
                    }
                    return;
                }

                if constexpr (I == maxArgs - 1) {
                    SPR_ASSERT_LOGIC(false);
                } else {
                    this->handleArgument<I + 1>(idx, arg);
                }
            }
            
            auto result() const noexcept -> Scalar {
                if (this->m_error) {
                    return *this->m_error;
                } else {
                    return this->invoke();
                }
            }
            
            auto isError() const noexcept -> bool {
                return bool(this->m_error);
            }
            
        private:
            
            template<unsigned I = maxArgs>
            auto invoke() const noexcept -> Scalar {
                
                if (m_argCount == I) {
                    return ApplyN<I>(this->makeOp(), this->m_args);
                }
                
                if constexpr (I == minArgs) {
                    SPR_FATAL_ERROR("impossible recursion");
                } else {
                    return this->invoke<I-1>();
                }
            }
            
        private:
            std::optional<Error> m_error;
            TupleType m_args;
            unsigned m_argCount = 0;
        };

        template<class Op, class RequiredTL>
        //requires(std::is_invocable_r_v<Scalar, Op, Args...>)
        class ScalarFunction<Op, RequiredTL, Typelist<>> : public ScalarFunctionBase<Op> {
        private:
            using TupleType = typename RequiredTL::template WrapIn<std::tuple>;
        public:
            static constexpr unsigned minArgs = RequiredTL::size;
            static constexpr unsigned maxArgs = RequiredTL::size;
            
            template<unsigned I = 0>
            void handleArgument(unsigned idx, const Scalar & arg) {
                if (idx == I) {

                    using ParameterType = typename std::tuple_element<I, decltype(this->m_args)>::type;

                    if constexpr (std::is_same_v<ParameterType, Scalar>) {
                        get<I>(this->m_args) = arg;
                    } else {
                        applyVisitorCoercedTo<ParameterType>([&](auto && val) {

                            using ArgType = std::remove_cvref_t<decltype(val)>;

                            if constexpr (std::is_same_v<Error, ArgType>) {
                                this->m_error = val;
                            } else {
                                get<I>(this->m_args) = val;
                            }

                        }, arg);
                    }
                    return;
                }

                if constexpr (I == maxArgs - 1) {
                    SPR_ASSERT_LOGIC(false);
                } else {
                    this->handleArgument<I + 1>(idx, arg);
                }
            }
            
            auto result() const noexcept -> Scalar {
                if (this->m_error) {
                    return *this->m_error;
                } else {
                    return std::apply(this->makeOp(), this->m_args);
                }
            }
            
            auto isError() const noexcept -> bool {
                return bool(this->m_error);
            }
        private:
            std::optional<Error> m_error;
            TupleType m_args;
        };
        
        template<class Op, class T>
        requires(std::is_invocable_r_v<Scalar, Op, T>)
        class ScalarFunction<Op, Typelist<T>, Typelist<>> : public ScalarFunctionBase<Op> {
        public:
            static constexpr unsigned minArgs = 1;
            static constexpr unsigned maxArgs = 1;
            
            void handleArgument([[maybe_unused]] unsigned idx, const Scalar & arg) {
                SPR_ASSERT_INPUT(idx == 0);

                if constexpr (std::is_same_v<T, Scalar>) {
                    this->m_result = this->makeOp()(arg);
                } else {
                    applyVisitorCoercedTo<T>([&](auto && val) {

                        using ArgType = std::remove_cvref_t<decltype(val)>;

                        if constexpr (std::is_same_v<Error, ArgType>) {
                            this->m_result = val;
                        } else {
                            this->m_result = this->makeOp()(val);
                        }

                    }, arg);
                }
            }
            
            auto result() const & noexcept -> const Scalar & {
                return m_result;
            }
            
            auto result() && noexcept -> Scalar && {
                return std::move(m_result);
            }
            
            auto isError() const noexcept -> bool {
                return get<Error>(&m_result);
            }
        private:
            Scalar m_result;
        };
        
        template<class Op, class T>
        requires(std::is_invocable_r_v<Scalar, Op, T> && std::is_invocable_r_v<Scalar, Op>)
        class ScalarFunction<Op, Typelist<>, Typelist<T>> : public ScalarFunctionBase<Op> {
        public:
            static constexpr unsigned minArgs = 0;
            static constexpr unsigned maxArgs = 1;
            
            void handleArgument([[maybe_unused]] unsigned idx, const Scalar & arg) {
                SPR_ASSERT_INPUT(idx == 0);
                
                if constexpr (std::is_same_v<T, Scalar>) {
                    this->m_result = this->makeOp()(arg);
                } else {
                    applyVisitorCoercedTo<T>([&](auto && val) {

                        using ArgType = std::remove_cvref_t<decltype(val)>;

                        if constexpr (std::is_same_v<Error, ArgType>) {
                            this->m_result = val;
                        } else {
                            this->m_result = this->makeOp()(val);
                        }

                    }, arg);
                }
            }
            
            auto result() const noexcept(noexcept(Op()())) -> Scalar {
                if (!m_result)
                    return this->makeOp()();
                return *m_result;
            }
            
            auto isError() const noexcept -> bool {
                return m_result && get<Error>(&*m_result);
            }
        private:
            std::optional<Scalar> m_result;
        };
        
        template<class Op>
        requires(std::is_invocable_r_v<Scalar, Op>)
        class ScalarFunction<Op, Typelist<>, Typelist<>> : public ScalarFunctionBase<Op> {
        public:
            static constexpr unsigned minArgs = 0;
            static constexpr unsigned maxArgs = 0;
            
            auto result() const & noexcept -> Scalar {
                return this->makeOp()();
            }
        };

    }

}

#endif
