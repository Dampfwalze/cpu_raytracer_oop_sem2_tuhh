#ifndef RENDER_THREAD_HPP
#define RENDER_THREAD_HPP

#include <thread>

#include <event_stream.h>
#include <scene/scene.h>
#include <thread_pool.h>
#include <rtmath.h>
#include <renderer.h>
#include <frame_buffer.h>
#include <render_params.h>

namespace rt
{
    namespace m = math;

    class RenderThread : public std::thread
    {
    private:
        enum class EventType
        {
            Terminate,
            Render,
        };

        struct Event
        {
            EventType type;
            union
            {
                struct
                {
                    Scene *scene;
                    FrameBuffer *frameBuffer;
                };
            };
            Event(EventType type);
            Event(Scene &scene, FrameBuffer &frameBuffer);
        };

    private:
        EventStream<Event> m_eventStream;

        ThreadPool<Renderer::RenderTask> m_threadPool;

        Renderer *m_renderer;

        bool m_isRendering = false;

        RenderParams m_renderParams;

    public:
        RenderParams renderParams;

    public:
        RenderThread(Renderer *renderer = nullptr);
        ~RenderThread();

        void terminate();

        void startRender(Scene &scene, FrameBuffer &frameBuffer);

        inline bool isRendering() const { return m_isRendering; }

        void setRenderer(Renderer *renderer);
        inline Renderer *getRenderer() { return m_renderer; };

    private:
        void run();
    };
} // namespace rt

#endif // RENDER_THREAD_HPP