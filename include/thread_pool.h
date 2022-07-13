#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <vector>
#include <thread>
#include <functional>

#include <event_stream.h>

namespace rt
{
    template <typename _Task>
    class ThreadPool
    {
    public:
        using task_type = _Task;

    private:
        struct Event
        {
            bool terminate;
            union
            {
                task_type task;
            };

            static constexpr Event Terminate() { return Event(true); }

            Event(const _Task &task);
            Event(_Task &&task);
            Event(const Event &other);
            Event(Event &&other);
            ~Event() {}

        private:
            Event(bool terminate);
        };

    private:
        std::vector<std::thread> m_workers;
        EventStream<Event> m_stream;

    public:
        ThreadPool(size_t threadCount);

        ThreadPool(const ThreadPool &) = delete;
        ThreadPool(ThreadPool &&) = delete;

        ~ThreadPool();

        inline bool isEmpty() const { return m_stream.isEmpty(); }

        inline void clear() { m_stream.clear(); }

        ThreadPool &operator<<(const task_type &task);
        ThreadPool &operator<<(task_type &&task);

    private:
        void run();
    };

    template <typename _Task>
    ThreadPool<_Task>::Event::Event(const _Task &task)
        : terminate(false), task(task) {}

    template <typename _Task>
    ThreadPool<_Task>::Event::Event(_Task &&task)
        : terminate(false), task(std::move(task)) {}

    template <typename _Task>
    ThreadPool<_Task>::Event::Event(const Event &other)
        : terminate(other.terminate), task(other.task) {}

    template <typename _Task>
    ThreadPool<_Task>::Event::Event(Event &&other)
        : terminate(other.terminate), task(std::move(other.task)) {}

    template <typename _Task>
    ThreadPool<_Task>::Event::Event(bool terminate)
        : terminate(terminate) {}

    template <typename _Task>
    ThreadPool<_Task>::ThreadPool(size_t threadCount)
    {
        m_workers.reserve(threadCount);
        for (size_t i = 0; i < threadCount; i++)
            m_workers.emplace_back(&ThreadPool<_Task>::run, this);

        std::cout << "Running with " << threadCount << " threads!" << std::endl;
    }

    template <typename _Task>
    ThreadPool<_Task>::~ThreadPool()
    {
        for (auto &&worker : m_workers)
            m_stream << Event::Terminate();
        for (auto &&worker : m_workers)
            worker.join();
    }

    template <typename _Task>
    void ThreadPool<_Task>::run()
    {
        while (true)
        {
            Event event = std::move(m_stream.get());

            if (event.terminate)
                break;

            event.task();
        }
    }

    template <typename _Task>
    ThreadPool<_Task> &ThreadPool<_Task>::operator<<(const task_type &task)
    {
        m_stream << task;
        return *this;
    }

    template <typename _Task>
    ThreadPool<_Task> &ThreadPool<_Task>::operator<<(task_type &&task)
    {
        m_stream << std::move(task);
        return *this;
    }
} // namespace rt

#endif // THREAD_POOL_HPP
