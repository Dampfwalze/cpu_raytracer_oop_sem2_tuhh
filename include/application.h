#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <window_thread.h>
#include <renderer.h>
#include <scene.h>

namespace rt
{
    class Application
    {
    private:
        Renderer m_renderer;
        WindowThread m_window;
        Scene m_scene;

    public:
        Application();
        ~Application();

        Renderer &getRenderer();
        Scene &getScene();

        void run();
    };

} // namespace rt

#endif // APPLICATION_HPP