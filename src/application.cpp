#include <application.h>

#include <optional>
#include <stream_formatter.h>
#include <rt_renderer.h>

namespace rt
{
    Application::Application()
        : scene{
              Camera(Transform(m::dvec3(0, 1, 2))),
          },
          m_window(*this)
    {
        renderers.emplace("Raytracing", new RTRenderer());
        renderThread.setRenderer(renderers["Raytracing"].get());

        auto defaultMaterial = scene.addMaterial(new Materials::LitMaterial("Default", m::Color<float>(1)));
        auto redLit = scene.addMaterial(new Materials::LitMaterial("Spheres", {1, 0, 0}));

        scene.addShape(new Shapes::Sphere("Sphere 1", 0.3, Transform(m::dvec3(0.5, 0.5, 0)), redLit));
        scene.addShape(new Shapes::Sphere("Sphere 2", 0.3, Transform(m::dvec3(-0.5, 0.5, 0)), redLit));
        // scene.addShape(new Shapes::Sphere(0.1, Transform(m::dvec3(0, 0, 0))));
        // scene.addShape(new Shapes::Sphere(0.1, Transform(m::dvec3(0.6, 1, 0.5))));
        scene.addShape(new Shapes::Plane(Transform(m::dvec3(0, 0, 0))));
        // scene.addLight(new Lights::PointLight{m::dvec3()});
        scene.addLight(new Lights::DirectionalLight());
        rtstd::formatterstream(std::cout) << scene << std::endl;
    }

    Application::~Application()
    {
    }

    void Application::run()
    {
        bool running = true;
        while (running)
        {
            Event event = m_eventStream.get();
            switch (event.type)
            {
            case EventType::CloseApplication:
                running = false;
                break;
            }
        }
    }

    Application::Event::Event(const Events::Render &event)
        : type(EventType::Render), render(event) {}
    Application::Event::Event(const Events::CloseApplication &event)
        : type(EventType::CloseApplication), closeApplication(event) {}
}