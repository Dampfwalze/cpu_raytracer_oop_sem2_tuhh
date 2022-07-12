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
            ~Event() {}

        private:
            Event(bool terminate);
        };

    private:
        std::vector<std::thread> m_workers;
        EventStream<Event> m_stream;

        uint64_t m_workingCount = 0;

        std::function<void()> m_emptyCallback;

        mutable std::mutex m_waitMutex;
        mutable std::condition_variable m_waitCV;

    public:
        ThreadPool(size_t threadCount);

        ThreadPool(const ThreadPool &) = delete;
        ThreadPool(ThreadPool &&) = delete;

        ~ThreadPool();

        inline bool isWorking() const { return m_workingCount > 0; }

        inline void setEmptyCallback(const std::function<void()> &emptyCallback) { m_emptyCallback = emptyCallback; }

        inline bool isEmpty() const { return m_stream.isEmpty(); }

        inline void clear() { m_stream.clear(); }

        void wait() const;

        ThreadPool &operator<<(const task_type &task);

    private:
        void run();
    };

    template <typename _Task>
    ThreadPool<_Task>::Event::Event(const _Task &task)
        : terminate(false), task(task) {}

    template <typename _Task>
    ThreadPool<_Task>::Event::Event(bool terminate)
        : terminate(terminate) {}

    template <typename _Task>
    ThreadPool<_Task>::ThreadPool(size_t threadCount)
        : m_workingCount(0)
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
        bool notFirst = false;

        while (true)
        {
            auto lk = m_stream.getLock();

            if (notFirst)
            {
                m_workingCount--;
                if (m_workingCount == 0 && m_stream.isEmpty())
                {
                    m_waitCV.notify_all();
                    if (m_emptyCallback)
                    {
                        lk.unlock();
                        m_emptyCallback();
                        lk.lock();
                    }
                }
            }
            notFirst = true;

            Event event = std::move(m_stream.get(lk));

            if (event.terminate)
                break;

            m_workingCount++;
            lk.unlock();

            event.task.run();
        }
    }

    template <typename _Task>
    void ThreadPool<_Task>::wait() const
    {
        if (m_stream.isEmpty())
            return;
        std::unique_lock lk(m_waitMutex);
        m_waitCV.wait(lk);
    }

    template <typename _Task>
    ThreadPool<_Task> &ThreadPool<_Task>::operator<<(const task_type &task)
    {
        m_stream << task;
        return *this;
    }
} // namespace rt

#endif // THREAD_POOL_HPP
