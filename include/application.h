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
    public:
        enum class EventType
        {
            None,
            Render,
        };

        struct Events
        {
            struct Render
            {
            };
        };

        struct Event
        {
            EventType type;
            union
            {
                Events::Render render;
            };

            Event(const Events::Render &render);
        };

    private:
        std::map<std::string, std::shared_ptr<Renderer>> m_renderers;

        RenderDispatcher m_renderDispatcher;

        WindowThread m_window;
        Scene m_scene;

        std::deque<Event> m_eventQue;
        std::mutex m_queMutex;
        std::condition_variable m_queCV;

    public:
        Application();
        ~Application();

        RenderDispatcher &getRenderDispatcher();
        Scene &getScene();

        std::map<std::string, std::shared_ptr<Renderer>> &getRenderers();

        void run();

        template <class T>
        Application &operator<<(T event)
        {
            std::lock_guard<std::mutex> lk(m_queMutex);
            m_eventQue.emplace_back(event);
            return *this;
        }
    };

} // namespace rt

#endif // APPLICATION_HPP