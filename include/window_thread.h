#ifndef WINDOW_THREAD_HPP
#define WINDOW_THREAD_HPP

#include <window.h>

#include <condition_variable>
#include <filesystem>
#include <mutex>
#include <thread>

namespace rt
{
    namespace m = math;

    class Application;

    class WindowThread : public std::thread
    {
    private:
        std::mutex m_termination_mutex;
        std::condition_variable m_termination_cv;
        bool m_terminate = false;

        Application &m_application;

    public:
        WindowThread(Application &application);
        ~WindowThread();

        void terminate();

    private:
        void cameraController();
        void run();

        void render(m::u64vec2 size);

        void save();
        void saveAs();
        void open();
        void newScene();
    };

} // namespace rt

#endif // WINDOW_THREAD_HPP