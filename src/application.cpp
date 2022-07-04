#include <application.h>

namespace rt
{
    Application::Application()
        : m_renderers({
              {"Raytracing", std::make_shared<RendererWith<RTRenderer>>()},
          }),
          m_renderDispatcher(RenderParams{
                                 .outputSize = {640, 480},
                                 .tileSize = {64, 64},
                             },
                             static_pointer_cast<Renderer>(m_renderers["Raytracing"])),
          m_window(*this), m_scene()
    {
        m_scene.addShape(new Shapes::Sphere());
        std::cout << m_scene << std::endl;
    }

    Application::~Application()
    {
    }

    RenderDispatcher &Application::getRenderDispatcher() { return m_renderDispatcher; }
    Scene &Application::getScene() { return m_scene; }

    std::map<std::string, std::shared_ptr<Renderer>> &Application::getRenderers() { return m_renderers; }

    void Application::run()
    {
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}