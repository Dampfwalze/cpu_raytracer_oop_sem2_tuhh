#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <event_stream.h>
#include <render_thread.h>
#include <resource_container.h>
#include <scene/scene.h>
#include <window_thread.h>

#include <map>
#include <optional>
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
            RenderOutput,
        };

        struct Events
        {
            struct Render
            {
            };
            struct RenderOutput
            {
                std::filesystem::path path;
                m::u64vec2            size;
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

        using Event = std::variant<Events::Render, Events::CloseApplication, Events::LoadScene, Events::SaveScene, Events::RenderOutput>;

    public:
        std::filesystem::path originPath;

        std::map<std::string, std::unique_ptr<Renderer>> renderers;

        ThreadPool<std::packaged_task<void()>> threadPool;

        ResourceContainer resources;

        std::unique_ptr<Scene> scene;

        FrameBuffer frameBuffer;

        RenderThread renderThread;

        std::optional<std::filesystem::path> savePath;

        m::u64vec2                           outputSize = {1920, 1080};
        std::optional<std::filesystem::path> outputPath;

        bool useGui = true;

    private:
        std::optional<WindowThread> m_window;

        EventStream<Event> m_eventStream;

    public:
        bool rendering = false;

    public:
        Application(const std::filesystem::path         &originPath,
                    bool                                 useGui = true,
                    std::optional<std::filesystem::path> sceneFile = std::nullopt,
                    std::optional<std::filesystem::path> outputPath = std::nullopt,
                    m::u64vec2                           outputSize = {1920, 1080});
        ~Application();

        void run();

        template <class T>
        Application &operator<<(T event)
        {
            m_eventStream << event;
            return *this;
        }

    private:
        void renderOutput(const std::filesystem::path &path, m::u64vec2 size);

        void loadScene(const std::filesystem::path &path);
        void saveScene(const std::filesystem::path &path);
        void loadDefaultScene();
    };

    template <>
    inline Application &Application::operator<<(Application::Events::Render event)
    {
        renderThread.startRender(*scene, frameBuffer);
        return *this;
    }
} // namespace rt

#endif // APPLICATION_HPP