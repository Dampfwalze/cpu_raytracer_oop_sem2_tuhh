#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <window_thread.h>
#include <scene/scene.h>
#include <event_stream.h>
#include <render_thread.h>

#include <map>

namespace rt
{
    class Application
    {
    public:
        enum class EventType
        {
            None,
            Render,
            CloseApplication,
        };

        struct Events
        {
            struct Render
            {
            };
            struct CloseApplication
            {
            };
        };

        struct Event
        {
            EventType type;
            union
            {
                Events::Render render;
                Events::CloseApplication closeApplication;
            };

            Event(const Events::Render &event);
            Event(const Events::CloseApplication &event);
        };

    public:
        std::map<std::string, std::unique_ptr<Renderer>> renderers;

        ThreadPool<std::packaged_task<void()>> threadPool;

        Scene scene;

        FrameBuffer frameBuffer;

        RenderThread renderThread;

    private:
        WindowThread m_window;

        EventStream<Event> m_eventStream;

    public:
        bool rendering = false;

    public:
        Application();
        ~Application();

        void run();

        template <class T>
        Application &operator<<(T event)
        {
            m_eventStream << event;
            return *this;
        }
    };

    template <>
    inline Application &Application::operator<<(Application::Events::Render event)
    {
        renderThread.startRender(scene, frameBuffer);
        return *this;
    }
} // namespace rt

#endif // APPLICATION_HPP