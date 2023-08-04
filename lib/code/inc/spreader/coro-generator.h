// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_CORO_GENERATOR_H_INCLUDED
#define SPR_HEADER_CORO_GENERATOR_H_INCLUDED

#if __has_include(<coroutine>)
    #include <coroutine>
#else
    #include <experimental/coroutine>
    namespace std {
        using std::experimental::suspend_always;
        using std::experimental::suspend_never;
        using std::experimental::coroutine_handle;
    }
#endif

#include <optional>
#include <utility>

namespace Spreader {

    template<class T>
    class CoroGenerator {
    public:
        class promise_type {
        public:
            promise_type() noexcept = default;
            ~promise_type() noexcept  = default;

            auto get_return_object() noexcept -> CoroGenerator {
                return CoroGenerator(
                    std::coroutine_handle<promise_type>::from_promise(*this)
                );
            }

            static auto initial_suspend() noexcept -> std::suspend_always 
                { return {}; }

            static auto final_suspend() noexcept -> std::suspend_always
                { return {}; }

            template<class... Args>
            auto yield_value(Args && ...args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
                -> std::suspend_always
            {
                m_value.emplace(std::forward<Args>(args)...);
                return {};
            }

            static void return_void() {}

            static void unhandled_exception() 
                { throw; }

            auto get() -> const T & 
                { return *m_value; }

        private:
            std::optional<T> m_value;
        };

    private:

        using Handle = std::coroutine_handle<promise_type>;

    public:

        class iterator {
            friend CoroGenerator;
        public:
            using reference = const T &;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = const T *;
            using iterator_category = std::input_iterator_tag;

            iterator() noexcept {}

            auto operator*() const -> reference 
                { return this->m_coro.promise().get(); }

            auto operator++() -> iterator & {
                this->m_coro.resume();
                return *this;
            }

            void operator++(int) 
                { this->m_coro.resume(); }

            friend auto operator==(const iterator& it, std::default_sentinel_t) noexcept -> bool 
                { return !it.m_coro || it.m_coro.done(); }
            friend auto operator==(std::default_sentinel_t, const iterator& it) noexcept -> bool 
                { return !it.m_coro || it.m_coro.done(); }
            friend auto operator!=(const iterator& it, std::default_sentinel_t rhs) noexcept -> bool 
                { return !(it == rhs); }
            friend auto operator!=(std::default_sentinel_t lhs, const iterator& it) noexcept -> bool 
                { return !(lhs == it); }

        private:
            explicit iterator(Handle coro) noexcept :
                m_coro(coro)
            {}

        private:
            Handle m_coro;
        };

    public:

        CoroGenerator() = default;
        CoroGenerator(const CoroGenerator&) = delete;
        auto operator=(const CoroGenerator&) -> CoroGenerator & = delete;

        CoroGenerator(CoroGenerator && g) noexcept :
            m_coro(std::exchange(g.m_coro, {}))
        {}

        auto operator=(CoroGenerator && g) noexcept -> CoroGenerator & {
            this->~CoroGenerator();
            new (this) CoroGenerator(std::move(g));
            return *this;
        }

        ~CoroGenerator() {
            if (m_coro) {
                m_coro.destroy();
            }
        }

        auto begin() -> iterator {
            if (m_coro)
                m_coro.resume();
            return iterator{m_coro};
        }

        auto end() -> std::default_sentinel_t 
            { return {}; }

    private:
        explicit CoroGenerator(Handle coro) noexcept :
            m_coro(coro)
        {}

    private:
        Handle m_coro;
    };
}

#endif
