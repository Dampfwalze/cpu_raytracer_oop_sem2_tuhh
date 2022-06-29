#include <application.h>

namespace rt
{
    Application::Application()
        : m_renderer(RenderParams{
              .outputSize = {640, 480},
              .tileSize = {64, 64},
          }),
          m_window(m_renderer)
    {
    }

    Application::~Application()
    {
    }

    void Application::run()
    {
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}