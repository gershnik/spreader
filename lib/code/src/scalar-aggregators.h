// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_SCALAR_AGGREGATORS_H_INCLUDED
#define SPR_HEADER_SCALAR_AGGREGATORS_H_INCLUDED

#include <spreader/scalar.h>

#include <functional>
#include <optional>

namespace Spreader {

    namespace ScalarDetail {

        template<class Op, bool AllSemantics>
        class NumericAggregator {

        public:
            auto addDirect(const Scalar & val) noexcept -> bool {
                return applyVisitorCoercedTo<Number>([&](const auto & val) -> bool {

                    using T = std::remove_cvref_t<decltype(val)>;
                
                    if constexpr (std::is_same_v<T, Number>) {

                        m_aggregate.add(val.value());
                        return true;

                    } else {
                        m_error = val;
                        return false;
                    }
                }, val); 
            }
            
            auto addIndirect(const Scalar & val) noexcept -> bool {
                
                return applyVisitor([&](const auto & val) {

                    using T = std::remove_cvref_t<decltype(val)>;
                    
                    if constexpr (std::is_same_v<T, Number>) {
                        
                        m_aggregate.add(val.value());
                        return true;
                        
                    } else if constexpr (AllSemantics && std::is_same_v<T, bool>) {
                        
                        m_aggregate.add(val);
                        return true;
                        
                    } else if constexpr (AllSemantics && std::is_same_v<T, String>) {
                        
                        m_aggregate.add(0);
                        return true;
                        
                    } else if constexpr (std::is_same_v<T, Error>) {
                        
                        m_error = val;
                        return false;
                        
                    } else {
                        
                        return true;
                    }
                    
                }, val);

            }

            auto result() const noexcept -> Scalar {
                
                if (m_error)
                    return *m_error;
                return m_aggregate.value();
            }

            auto isError() const noexcept -> bool {
                return bool(m_error);
            }

            void setError(Error err) noexcept {
                m_error = err;
            }

        private:
            Op m_aggregate;
            std::optional<Error> m_error;
        };
        
        template<bool AllSemantics>
        class NumericCounter {

        public:
            auto addDirect(const Scalar & val) noexcept -> bool {
                if constexpr (AllSemantics) {
                    if (!get<Scalar::Blank>(&val)) {
                        ++m_count;
                    }
                } else {
                    applyVisitorCoercedTo<Number>([&](auto val) {
                        if constexpr (std::is_same_v<decltype(val), Number>) {
                            ++m_count;
                        }
                    }, val);
                }
                return true;
            }
            
            auto addIndirect(const Scalar & val) noexcept -> bool {
                if constexpr (AllSemantics) {
                    if (!get<Scalar::Blank>(&val)) {
                        ++m_count;
                    }
                } else {
                    if (get<Number>(&val)) {
                        ++m_count;
                    }
                }
                return true;
            }

            auto result() const noexcept -> Scalar {
                return m_count;
            }

            auto isError() const noexcept -> bool {
                return false;
            }

        private:
            unsigned m_count = 0;
        };

        template<class Op>
        class BooleanAggregator {
        public:
            auto addDirect(const Scalar & val) noexcept -> bool {
                return applyVisitor([&](const auto & val) {

                    using T = std::remove_cvref_t<decltype(val)>;
                    
                    if constexpr (std::is_same_v<T, bool>) {
                        
                        m_aggregate.add(val);
                        return true;
                        
                    } else if constexpr (IsCoercible<T, bool>) {
                        
                        m_aggregate.add(coerceTo<bool>(val));
                        return true;
                        
                    } else if constexpr (std::is_same_v<T, Error>) {
                        
                        m_error = val;
                        return false;
                        
                    } else {
                        
                        return true;
                    }
                    
                }, val);
            }

            auto addIndirect(const Scalar & val) noexcept -> bool {
                return addDirect(val);
            }

            auto result() const noexcept -> Scalar {
                
                if (m_error)
                    return *m_error;
                return m_aggregate.value();
            }

            auto isError() const noexcept -> bool {
                return bool(m_error);
            }

            void setError(Error err) noexcept {
                m_error = err;
            }

        private:
            Op m_aggregate;
            std::optional<Error> m_error;
        };
        
        class StringConcat {
        public:
            auto addDirect(const Scalar & val) noexcept -> bool {
                return applyVisitorCoercedTo<String>([&](auto && val) {

                    using ArgType = std::remove_cvref_t<decltype(val)>;

                    if constexpr (std::is_same_v<Error, ArgType>) {
                        m_error = val;
                        return false;
                    } else {
                        m_builder.append(val);
                        return true;
                    }

                }, val);
            }
            
            auto addIndirect(const Scalar & val) noexcept -> bool {
                return addDirect(val);
            }

            auto result() noexcept -> Scalar {
                
                if (m_error)
                    return *m_error;
                return m_builder.build();
            }
            
            auto isError() const noexcept -> bool {
                return bool(m_error);
            }
            
            void setError(Error err) noexcept {
                m_error = err;
            } 

            struct State {
                StringBuilder::size_type size;
                std::optional<Error> error;
            };

            friend auto saveState(const StringConcat & obj) noexcept -> State {
                return State{obj.m_builder.storage_size(), obj.m_error};
            }

            friend void restoreState(StringConcat & obj, State && state) noexcept {
                obj.m_builder.resize_storage(state.size);
                obj.m_error = std::move(state.error);
            }
        private:
            StringBuilder m_builder;
            std::optional<Error> m_error;
        };
    }
    
}

#endif
