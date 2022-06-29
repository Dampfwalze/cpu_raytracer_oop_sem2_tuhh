#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <window_thread.h>
#include <renderer.h>

namespace rt
{
    class Application
    {
    private:
        WindowThread m_window;
        Renderer m_renderer;

    public:
        Application();
        ~Application();

        void run();
    };

} // namespace rt

#endif // APPLICATION_HPP