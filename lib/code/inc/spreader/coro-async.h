// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_CORO_PAUSED_H_INCLUDED
#define SPR_HEADER_CORO_PAUSED_H_INCLUDED

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


namespace Spreader {

    class AsyncNotify : public ref_counted<AsyncNotify, REFCNT_FLAGS> {
        friend ref_counted;
    public:
        virtual void complete() noexcept = 0;
    protected:
        AsyncNotify() = default;
        virtual ~AsyncNotify() = default;
    };

    void resumeLater(std::coroutine_handle<> h) noexcept;
    
    struct YieldExecution {
        static auto await_ready() -> bool { 
            auto now = std::chrono::steady_clock::now();
            if (now - s_lastSuspended > std::chrono::milliseconds(100)) {
                s_lastSuspended = now;
                return false;
            }
            return true; 
        }
        auto await_suspend(std::coroutine_handle<> h) const noexcept 
            { resumeLater(h); }
        static void await_resume() {}

        static inline std::chrono::steady_clock::time_point s_lastSuspended;
    };

    inline auto yieldExecution() -> YieldExecution 
        { return {}; }

    template<class T = void>
    class AsyncTask;

    template<>
    class AsyncTask<void> {
    public:
        auto isDone() const noexcept -> bool 
            { return m_handle.done(); }
        void setNotify(const refcnt_ptr<AsyncNotify> & notify) 
            { m_promise->notify = notify; }
        void setNotify(refcnt_ptr<AsyncNotify> && notify) 
            { m_promise->notify = std::move(notify); }

        //Awaitable protocol
       
        static auto await_ready() -> bool { return false; }
        auto await_suspend(std::coroutine_handle<> h) const noexcept {  
            m_promise->parentHandle = h;
        }
        static void await_resume() {}

        //Promise
        struct promise_type
        {
            auto get_return_object() -> AsyncTask { 
                return {this}; 
            }
            static auto initial_suspend() -> std::suspend_never { return {}; }
            auto final_suspend() noexcept -> std::suspend_never { 
                if (parentHandle)
                    parentHandle.resume();
                return {}; 
            }
            void return_void() {
                if (this->notify)
                    this->notify->complete();
            }
            static void unhandled_exception() { throw; }

            refcnt_ptr<AsyncNotify> notify;
            std::coroutine_handle<> parentHandle;
        };
    private:
        AsyncTask(promise_type * pr): 
            m_promise(pr),
            m_handle(std::coroutine_handle<promise_type>::from_promise(*pr)) 
        {}

    private:
        promise_type * m_promise;
        std::coroutine_handle<promise_type> m_handle;
    };
}

#endif
