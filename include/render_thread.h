#ifndef RENDER_THREAD_HPP
#define RENDER_THREAD_HPP

#include <thread>

#include <event_stream.h>
#include <frame_buffer.h>
#include <render_params.h>
#include <renderer.h>
#include <rtmath.h>
#include <scene/scene.h>
#include <thread_pool.h>

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
                    Scene       *scene;
                    FrameBuffer *frameBuffer;
                };
            };
            Event(EventType type);
            Event(Scene &scene, FrameBuffer &frameBuffer);
        };

    private:
        EventStream<Event> m_eventStream;

        ThreadPool<Renderer::task_type> *m_threadPool;

        Renderer *m_renderer;

        bool m_isRendering = false;

        RenderParams m_renderParams;

        std::mutex              m_renderFinished_mutex;
        std::condition_variable m_renderFinished_cv;

    public:
        RenderParams renderParams;

        std::string renderLog;

    public:
        RenderThread(ThreadPool<Renderer::task_type> *threadPool, Renderer *renderer = nullptr);
        ~RenderThread();

        void terminate();

        void startRender(Scene &scene, FrameBuffer &frameBuffer);

        inline bool isRendering() const { return m_isRendering; }
        void        waitUntilFinished();
        void        waitUntilStarted();

        void             setRenderer(Renderer *renderer);
        inline Renderer *getRenderer() { return m_renderer; };

    private:
        void run();
    };
} // namespace rt

#endif // RENDER_THREAD_HPP