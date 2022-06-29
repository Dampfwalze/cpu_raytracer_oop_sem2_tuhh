#ifndef WINDOW_THREAD_HPP
#define WINDOW_THREAD_HPP

#include <window.h>
#include <renderer.h>

#include <thread>
#include <mutex>
#include <condition_variable>

namespace rt
{
    class WindowThread : public std::thread
    {
    private:
        std::mutex m_termination_mutex;
        std::condition_variable m_termination_cv;
        bool m_terminate = false;

        Renderer &m_renderer;

    public:
        WindowThread(Renderer &renderer);
        ~WindowThread();

        void terminate();

    private:
        void run();
    };

} // namespace rt

#endif // WINDOW_THREAD_HPP