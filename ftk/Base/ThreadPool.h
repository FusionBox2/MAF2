#pragma once

#include "ftkConfigure.h"

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

BEGIN_FTK_NAMESPACE

template<class... _Types,
    size_t... _Indices> inline
    auto invoke_stored_explicit(std::tuple<_Types...>&& _Tuple, std::index_sequence<_Indices...>)
    -> decltype(std::invoke(std::get<_Indices>(std::move(_Tuple))...))
{	// invoke() a tuple with explicit parameter ordering
    return (std::invoke(std::get<_Indices>(std::move(_Tuple))...));
}

template<class... _Types> inline
auto invoke_stored(std::tuple<_Types...>&& _Tuple)
-> decltype(invoke_stored_explicit(std::move(_Tuple), std::index_sequence_for<_Types...>()))
{	// invoke() a tuple
    return (invoke_stored_explicit(std::move(_Tuple), std::index_sequence_for<_Types...>()));
}

class no_copy_function_copied : public std::exception
{
public:
    virtual const char *what() const noexcept override
    {
        return "non copyable function copy called";
    }
};

//function taken from std::async implementation
template<class... _Types>
class no_copy_function
{	// async() is built on packaged_task internals which incorrectly use std::function, which requires that things
    // be copyable. We can't fix this in an update, so this adapter turns copies into terminate(). When VSO#153581
    // is fixed, remove this adapter.
    using _Storaget = std::tuple<std::decay_t<_Types>...>;

public:
    no_copy_function(){}
    explicit no_copy_function(_Types&&... _Vals)
        : m_storage(std::forward<_Types>(_Vals)...)
    {	// Initializes no_copy_function with a decayed callable object and arguments
    }

    no_copy_function(const no_copy_function& _Other)
        : m_storage(std::move(_Other.m_storage))
    {   // Very Bad Things^TM
        throw no_copy_function_copied();
    }

    no_copy_function(no_copy_function&& _Other) = default;
    no_copy_function& operator=(const no_copy_function&) = delete;
    no_copy_function& operator=(no_copy_function&&) = default;

    auto operator()() -> decltype(invoke_stored(std::move(std::declval<_Storaget&>())))
    {	// Invokes stored callable object
        return (invoke_stored(std::move(m_storage)));
    }

private:
    mutable _Storaget m_storage;
};

template<class _Fty, class... _ArgTypes>
auto make_no_copy_function(_Fty&& _Fnarg, _ArgTypes&&... _Args)
{   // return a future object whose associated asynchronous state
    // manages a callable object launched with supplied policy
    return no_copy_function<_Fty, _ArgTypes...>(
            std::forward<_Fty>(_Fnarg),
            std::forward<_ArgTypes>(_Args)...
            );
}

class ThreadPool {
public:
    ThreadPool(size_t threads, size_t maxRequests = (std::numeric_limits<size_t>::max)());
    // add new work item to the pool
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
    {
        using return_t = std::invoke_result_t<F, Args...>;
        using task_t = std::packaged_task<return_t()>;

        std::future<return_t> retVal;
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            if (!m_stop && m_tasks.size() < m_maxRequests)
            {
                try
                {
                    auto task = std::make_shared<std::packaged_task<return_t()> >(make_no_copy_function(std::forward<F>(f), std::forward<Args>(args)...));
                    retVal = task->get_future();
                    m_tasks.emplace([task]{(*task)();});
                    m_condition.notify_one();
                }
                catch (std::bad_alloc&)
                {
                    return std::future<return_t>();
                }
                catch (no_copy_function_copied&)
                {
                    return std::future<return_t>();
                }
                catch (...)
                {
                    return std::future<return_t>();
                }
            }
        }
        return retVal;
    }
    ~ThreadPool();

protected:
    void ThreadFunc()
    {
        for (;;)
        {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(m_queueMutex);
                m_condition.wait(lock,
                                 [this] { return m_stop || !m_tasks.empty(); });
                if (m_stop)
                    return;
                task = std::move(m_tasks.front());
                m_tasks.pop();
            }
            task();
        }
    }

private:
    // need to keep track of threads so we can join them
    std::vector<std::thread> m_workers;
    // the task queue
    std::queue< std::function<void()> > m_tasks;
    size_t m_maxRequests;
    
    // synchronization
    std::mutex m_queueMutex;
    std::condition_variable m_condition;
    bool m_stop;
};
 
// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads, size_t maxRequests)
    : m_maxRequests(maxRequests)
    , m_stop(false)
{
    for(size_t i = 0; i < threads; ++i)
    {
        m_workers.emplace_back(
            [this]
            {
                try
                {
                    ThreadFunc();
                }
                catch (no_copy_function_copied&)
                {
                }
                catch (...)
                {
                }
            }
        );
    }
}


// the destructor joins all threads
inline ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_stop = true;
    }
    m_condition.notify_all();
    for(std::thread &worker: m_workers)
        worker.join();
}

END_FTK_NAMESPACE
