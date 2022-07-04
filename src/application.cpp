#include <application.h>

namespace rt
{
    Application::Application()
        : m_renderer(RenderParams{
              .outputSize = {640, 480},
              .tileSize = {64, 64},
          }),
          m_window(*this), m_scene()
    {
        m_scene.addShape(new Shapes::Sphere());
        std::cout << m_scene << std::endl;
    }

    Application::~Application()
    {
    }

    Renderer &Application::getRenderer() { return m_renderer; }
    Scene &Application::getScene() { return m_scene; }

    void Application::run()
    {
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}