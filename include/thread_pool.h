#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <iostream>
#include <sstream>
#include <optional>
#include <functional>
#include <deque>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace rt
{

    template <typename T>
    class ThreadPool
    {
    private:
        class TaskPool
        {
        private:
            std::mutex m_tasks_mutex;
            std::condition_variable m_tasks_cv;

            std::deque<T> m_tasks;

        public:
            std::optional<T> get(std::function<bool()> predicate);
            void submit(T task);

            void notifyAll();

            bool empty() const;

            void clear();
        };

        class WorkerThread
        {
        private:
            TaskPool &m_taskPool;
            std::thread m_thread;

            std::mutex m_termination_mutex;
            bool m_terminate = false;

        public:
            WorkerThread(TaskPool &m_taskPool);
            WorkerThread(WorkerThread &&other);
            ~WorkerThread();

            void setTerminateFlag();

        private:
            void run();
        };

    private:
        TaskPool m_taskPool;
        std::vector<WorkerThread> m_workers;

    public:
        ThreadPool(int threadCount);
        ~ThreadPool();

        void submit(T task);

        bool empty() const;

        void clear();
    };

    // --- Implementation ---

    // TaskPool

    template <typename T>
    std::optional<T> ThreadPool<T>::TaskPool::get(std::function<bool()> predicate)
    {
        while (true)
        {
            std::unique_lock<std::mutex> lk(m_tasks_mutex);
            m_tasks_cv.wait(lk, [this, predicate]
                            { return m_tasks.size() > 0 || predicate(); });

            if (predicate())
                return std::nullopt;

            if (m_tasks.size() <= 0)
                continue;

            T task = std::move(m_tasks[0]);
            m_tasks.pop_front();
            return task;
        }
    }

    template <typename T>
    void ThreadPool<T>::TaskPool::submit(T task)
    {
        std::lock_guard<std::mutex> lk(m_tasks_mutex);
        m_tasks.push_back(std::move(task));
        m_tasks_cv.notify_one();
    }

    template <typename T>
    void ThreadPool<T>::TaskPool::notifyAll()
    {
        m_tasks_cv.notify_all();
    }

    template <typename T>
    bool ThreadPool<T>::TaskPool::empty() const { return m_tasks.empty(); }
    template <typename T>
    void ThreadPool<T>::TaskPool::clear()
    {
        std::lock_guard<std::mutex> lk(m_tasks_mutex);
        m_tasks.clear();
    }

    // WorkerThread

    template <typename T>
    ThreadPool<T>::WorkerThread::WorkerThread(TaskPool &m_taskPool)
        : m_taskPool(m_taskPool), m_thread(&ThreadPool::WorkerThread::run, this) {}

    template <typename T>
    ThreadPool<T>::WorkerThread::WorkerThread(WorkerThread &&other)
        : m_taskPool(other.m_taskPool), m_thread(std::move(other.m_thread)) {}

    template <typename T>
    ThreadPool<T>::WorkerThread::~WorkerThread()
    {
        m_thread.join();
    }

    template <typename T>
    void ThreadPool<T>::WorkerThread::setTerminateFlag()
    {
        std::lock_guard<std::mutex> lk(m_termination_mutex);
        m_terminate = true;
    }

    template <typename T>
    void ThreadPool<T>::WorkerThread::run()
    {
        while (!m_terminate)
        {
            std::optional<T> task = m_taskPool.get([this]
                                                   { return m_terminate; });
            if (task.has_value())
                task.value().run();
        }

        std::cout << "Terminate Worker Thread" << std::endl;
    }

    // ThreadPool

    template <typename T>
    ThreadPool<T>::ThreadPool(int threadCount)
        : m_workers()
    {
        m_workers.reserve(threadCount);
        for (size_t i = 0; i < threadCount; i++)
            m_workers.emplace_back(m_taskPool);

        std::cout << "Running with " << threadCount << " threads!" << std::endl;
    }

    template <typename T>
    ThreadPool<T>::~ThreadPool()
    {
        for (auto &&i : m_workers)
            i.setTerminateFlag();
        m_taskPool.notifyAll();
    }

    template <typename T>
    void ThreadPool<T>::submit(T task)
    {
        m_taskPool.submit(task);
    }

    template <typename T>
    bool ThreadPool<T>::empty() const { return m_taskPool.empty(); }
    template <typename T>
    void ThreadPool<T>::clear() { m_taskPool.clear(); }

} // namespace rt

#endif // THREAD_POOL_HPP
