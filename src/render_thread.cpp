#include <pixel_logger.h>
#include <profiler.h>
#include <render_thread.h>
#include <stream_formatter.h>

namespace rt
{
    RenderThread::Event::Event(EventType type)
        : type(type) {}

    RenderThread::Event::Event(Scene &scene, FrameBuffer &frameBuffer)
        : type(EventType::Render), scene(&scene), frameBuffer(&frameBuffer) {}

    RenderThread::RenderThread(ThreadPool<Renderer::task_type> *threadPool, Renderer *renderer)
        : m_threadPool(threadPool), m_renderer(renderer),
          renderParams({
              {64, 64},
          }),
          std::thread(&RenderThread::run, this)
    {
    }

    RenderThread::~RenderThread()
    {
        terminate();
        join();
    }

    void RenderThread::terminate()
    {
        m_eventStream << EventType::Terminate;
    }

    void RenderThread::startRender(Scene &scene, FrameBuffer &frameBuffer)
    {
        if (!m_eventStream.isEmpty())
            return;
        m_eventStream << Event(scene, frameBuffer);
    }

    void RenderThread::waitUntilFinished()
    {
        std::unique_lock<std::mutex> lock(m_renderFinished_mutex);
        m_renderFinished_cv.wait(lock, [this]
                                 { return !m_isRendering; });
    }

    void RenderThread::setRenderer(Renderer *renderer)
    {
        assert(!isRendering());
        m_renderer = renderer;
    }

    void RenderThread::run()
    {
        while (true)
        {
            Event event = m_eventStream.get();
            switch (event.type)
            {
            case EventType::Terminate:
                return;
            case EventType::Render:
                assert(m_renderer != nullptr);
                assert(event.frameBuffer != nullptr);

                m_renderParams = renderParams;

                m_isRendering = true;

                std::stringstream      ss;
                rtstd::formatterstream logger(ss);
                PixelLogger::logger.setStream(&logger);

                Profiling::profiler.beginFrame();

                m_renderer->doRender(m_threadPool, event.scene, event.frameBuffer, &m_renderParams);

                Profiling::profiler.endFrame();

                PixelLogger::logger.setStream(nullptr);
                renderLog = ss.str();

                m_isRendering = false;
                m_renderFinished_cv.notify_all();
                break;
            }
        }
    }
}