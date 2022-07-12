#include <render_thread.h>

namespace rt
{
    RenderThread::Event::Event(EventType type)
        : type(type) {}

    RenderThread::Event::Event(Scene &scene, FrameBuffer &frameBuffer)
        : type(EventType::Render), scene(&scene), frameBuffer(&frameBuffer) {}

    RenderThread::RenderThread(Renderer *renderer)
        : m_threadPool(std::thread::hardware_concurrency() == 0 ? 1 : std::thread::hardware_concurrency()),
          m_renderer(renderer),
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
        if (m_isRendering)
            return;
        m_eventStream << Event(scene, frameBuffer);
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

                if (!m_threadPool.isEmpty())
                    m_threadPool.clear();

                m_renderParams = renderParams;

                m_isRendering = true;
                m_renderer->doRender(&m_threadPool, event.scene, event.frameBuffer, &m_renderParams);
                m_isRendering = false;
                break;
            }
        }
    }
}