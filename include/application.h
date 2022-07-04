#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <window_thread.h>
#include <render_dispatcher.h>
#include <scene.h>

namespace rt
{
    class Application
    {
    private:
        RenderDispatcher m_renderDispatcher;
        WindowThread m_window;
        Scene m_scene;

    public:
        Application();
        ~Application();

        RenderDispatcher &getRenderDispatcher();
        Scene &getScene();

        void run();
    };

} // namespace rt

#endif // APPLICATION_HPP