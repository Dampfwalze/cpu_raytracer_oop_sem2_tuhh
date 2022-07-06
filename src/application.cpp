#include <application.h>
#include <optional>
#include <stream_formatter.h>

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
          m_window(*this), m_scene{
                               Camera(Transform(m::dvec3(0, 0, -2))),
                           }
    {
        m_scene.addShape(new Shapes::Sphere(0.1));
        rtstd::formatterstream(std::cout) << m_scene << std::endl;
    }

    Application::~Application()
    {
    }

    RenderDispatcher &Application::getRenderDispatcher() { return m_renderDispatcher; }
    Scene &Application::getScene() { return m_scene; }

    std::map<std::string, std::shared_ptr<Renderer>> &Application::getRenderers() { return m_renderers; }

    void Application::run()
    {
        bool running = true;
        while (running)
        {
            std::optional<Event> event;
            {
                std::unique_lock<std::mutex> lk(m_queMutex);
                m_queCV.wait_for(lk, std::chrono::milliseconds(100), [this]
                                 { return !m_eventQue.empty(); });
                if (!m_eventQue.empty())
                {
                    event = m_eventQue[0];
                    m_eventQue.pop_front();
                }
            }
            if (event.has_value())
            {
                switch (event.value().type)
                {
                case EventType::Render:
                    m_renderDispatcher.render(m_scene);
                    break;
                case EventType::CloseApplication:
                    running = false;
                    break;
                }
            }
        }
    }

    Application::Event::Event(const Events::Render &event)
        : type(EventType::Render), render(event) {}
    Application::Event::Event(const Events::CloseApplication &event)
        : type(EventType::CloseApplication), closeApplication(event) {}
}