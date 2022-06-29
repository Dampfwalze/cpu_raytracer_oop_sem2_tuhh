#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <window_thread.h>
#include <renderer.h>

namespace rt
{
    class Application
    {
    private:
        Renderer m_renderer;
        WindowThread m_window;

    public:
        Application();
        ~Application();

        void run();
    };

} // namespace rt

#endif // APPLICATION_HPP