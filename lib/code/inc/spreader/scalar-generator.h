// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_SCALAR_GENERATOR_H_INCLUDED
#define SPR_HEADER_SCALAR_GENERATOR_H_INCLUDED

#include <spreader/scalar.h>
#include <spreader/array.h>
#include <spreader/geometry.h>


namespace Spreader {

    class CellGrid;

    class ScalarGenerator {

    public:
        using Types = Typelist<
                        Scalar,
                        Point,
                        ArrayPtr, 
                        Rect>;

    private:
        using ImplType = Types::WrapIn<std::variant>;

    public:
        ScalarGenerator() noexcept = default;
        
        template<class T>
        requires(std::is_constructible_v<ImplType, T &&>)
        ScalarGenerator(T && val) noexcept(std::is_nothrow_constructible_v<ImplType, T &&>) : 
            m_impl(std::forward<T>(val)) {
        }

        template<class T>
        requires(std::is_assignable_v<ImplType, T &&>)
        ScalarGenerator & operator=(T && val) noexcept(std::is_nothrow_assignable_v<ImplType, T &&>) {
            m_impl = std::forward<T>(val);
            return *this;
        }

        friend void swap(ScalarGenerator & lhs, ScalarGenerator & rhs) {
            using std::swap;
            swap(lhs.m_impl, rhs.m_impl);
        }

        template<class T>
        friend constexpr auto get(const ScalarGenerator & v) noexcept -> const T & {
            auto p = std::get_if<T>(&v.m_impl);
            SPR_ASSERT_INPUT(p);
            return *p;
        }
        template<class T>
        friend constexpr auto get(ScalarGenerator & v) noexcept -> T & {
            auto p = std::get_if<T>(&v.m_impl);
            SPR_ASSERT_INPUT(p);
            return *p;
        }
        template<class T>
        friend constexpr auto get(ScalarGenerator && v) noexcept -> T && {
            auto p = std::get_if<T>(&v.m_impl);
            SPR_ASSERT_INPUT(p);
            return *p;
        }
        template<class T>
        friend constexpr auto get(const ScalarGenerator * v) noexcept -> const T * {
            return std::get_if<T>(v ? &v->m_impl : nullptr);
        }
        template<class T>
        friend constexpr auto get(ScalarGenerator * v) noexcept -> T * {
            return std::get_if<T>(v ? &v->m_impl : nullptr);
        }

        template<class Visitor, class... ScalarGenerators>
        friend auto applyVisitor(Visitor && visitor, ScalarGenerators && ...generators) 
        requires(std::is_same_v<std::remove_cvref_t<ScalarGenerators>, ScalarGenerator> && ...) {
            return std::visit(std::forward<Visitor>(visitor), std::forward<ScalarGenerators>(generators).m_impl...);
        }

    private:
        ImplType m_impl;
    };
}

#endif
