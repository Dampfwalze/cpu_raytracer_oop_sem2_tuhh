#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <event_stream.h>
#include <render_thread.h>
#include <resource_container.h>
#include <scene/scene.h>
#include <window_thread.h>

#include <map>
#include <variant>

namespace rt
{
    class Application
    {
    public:
        enum class EventType
        {
            Render,
            CloseApplication,
            LoadScene,
            SaveScene,
        };

        struct Events
        {
            struct Render
            {
            };
            struct CloseApplication
            {
            };
            struct LoadScene
            {
                std::filesystem::path path;
            };
            struct SaveScene
            {
                std::filesystem::path path;
            };
        };

        using Event = std::variant<Events::Render, Events::CloseApplication, Events::LoadScene, Events::SaveScene>;

    public:
        std::filesystem::path originPath;

        std::map<std::string, std::unique_ptr<Renderer>> renderers;

        ThreadPool<std::packaged_task<void()>> threadPool;

        ResourceContainer resources;

        std::unique_ptr<Scene> scene;

        FrameBuffer frameBuffer;

        RenderThread renderThread;

    private:
        WindowThread m_window;

        EventStream<Event> m_eventStream;

    public:
        bool rendering = false;

    public:
        Application(const std::filesystem::path &originPath);
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
        renderThread.startRender(*scene, frameBuffer);
        return *this;
    }
} // namespace rt

#endif // APPLICATION_HPP