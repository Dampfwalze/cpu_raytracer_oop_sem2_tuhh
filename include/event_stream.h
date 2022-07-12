#ifndef EVENT_STREAM_HPP
#define EVENT_STREAM_HPP

#include <deque>
#include <mutex>
#include <condition_variable>

namespace rt
{
    template <typename _Event>
    class EventStream
    {
    public:
        using event_type = _Event;
        using duration = std::chrono::nanoseconds;

        duration waitingTime;

    private:
        std::deque<event_type> m_que;
        std::mutex m_mutex;
        std::condition_variable m_cv;

    public:
        template <typename _Dur = duration>
        EventStream(_Dur waitingTime = duration::zero())
            : waitingTime(std::chrono::duration_cast<duration>(waitingTime)) {}

        inline std::unique_lock<std::mutex> getLock() { return std::unique_lock<std::mutex>(m_mutex); }

        inline event_type get()
        {
            std::unique_lock<std::mutex> lk = getLock();
            return get(lk);
        }
        event_type get(std::unique_lock<std::mutex> &lock);

        inline bool isEmpty() const { return m_que.empty(); }

        void clear();

        template <typename... Args>
        EventStream<event_type> &operator<<(Args &&...args);
    };

    template <typename _Event>
    _Event EventStream<_Event>::get(std::unique_lock<std::mutex> &lk)
    {
        while (true)
        {
            if (waitingTime != duration::zero())
                m_cv.wait_for(lk, waitingTime, [this]
                              { return !m_que.empty(); });
            else
                m_cv.wait(lk, [this]
                          { return !m_que.empty(); });

            if (m_que.empty())
                continue;

            event_type event = std::move(m_que[0]);
            m_que.pop_front();
            return std::move(event);
        }
    }

    template <typename _Event>
    void EventStream<_Event>::clear()
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_que.clear();
    }

    template <typename _Event>
    template <typename... Args>
    EventStream<_Event> &EventStream<_Event>::operator<<(Args &&...args)
    {
        {
            std::lock_guard<std::mutex> lk(m_mutex);
            m_que.emplace_back(std::forward<Args>(args)...);
        }
        m_cv.notify_one();
        return *this;
    }

} // namespace rt

#endif // EVENT_STREAM_HPP