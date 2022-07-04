#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <window_thread.h>
#include <render_dispatcher.h>
#include <scene.h>
#include <rt_renderer.h>

#include <map>

namespace rt
{
    class Application
    {
    private:
        std::map<std::string, std::shared_ptr<Renderer>> m_renderers;

        RenderDispatcher m_renderDispatcher;

        WindowThread m_window;
        Scene m_scene;

    public:
        Application();
        ~Application();

        RenderDispatcher &getRenderDispatcher();
        Scene &getScene();

        std::map<std::string, std::shared_ptr<Renderer>> &getRenderers();

        void run();
    };

} // namespace rt

#endif // APPLICATION_HPP