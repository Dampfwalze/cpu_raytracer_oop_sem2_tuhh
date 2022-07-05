#include <render_dispatcher.h>

namespace rt
{
    namespace m = math;

    RenderDispatcher::RenderTask::RenderTask(m::Rect<size_t> rect, FrameBuffer &frameBuffer, Scene &scene, std::shared_ptr<Renderer> &renderer)
        : m_rect(rect), m_frameBuffer(frameBuffer), m_scene(scene), m_renderer(renderer) {}

    RenderDispatcher::RenderTask::~RenderTask()
    {
    }

    void RenderDispatcher::RenderTask::run()
    {
        auto renderer = std::unique_ptr<Renderer::PixelRenderer>(m_renderer->createPixelRenderer(m_scene));
        for (size_t y = m_rect.start.y; y < m_rect.getEnd().y; y++)
        {
            for (size_t x = m_rect.start.x; x < m_rect.getEnd().x; x++)
            {
                renderer->pixelCoords = m::uvec2(x, y);
                m_frameBuffer.at(x, y) = renderer->renderPixel();
                // if (((y + x) / 20) % 3 == 0)
                //     m_frameBuffer.at(x, y) = {0, 1, 0};
                // else if (((y + x) / 20) % 3 == 1)
                //     m_frameBuffer.at(x, y) = {1, 0, 0};
                // else
                //     m_frameBuffer.at(x, y) = {0, 0, 1};
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    RenderDispatcher::RenderDispatcher(const RenderParams &renderParams, const std::shared_ptr<Renderer> &renderer)
        : m_threadPool(std::thread::hardware_concurrency() == 0 ? 1 : std::thread::hardware_concurrency()),
          renderParams(renderParams), m_renderer(renderer) {}

    RenderDispatcher::~RenderDispatcher()
    {
    }

    void RenderDispatcher::render(Scene &scene)
    {
        std::cout << "Start render" << std::endl;

        if (m_frameBuffer.getWidth() != renderParams.outputSize.x || m_frameBuffer.getHeight() != renderParams.outputSize.y)
            m_frameBuffer.resize(renderParams.outputSize.x, renderParams.outputSize.y);

        m_frameBuffer.clear();
        for (size_t x = 0; x < m_frameBuffer.getWidth(); x += renderParams.tileSize.x)
        {
            for (size_t y = 0; y < m_frameBuffer.getHeight(); y += renderParams.tileSize.y)
            {
                m_threadPool.submit(RenderTask(
                    m::Rect(m::u64vec2(x, y), renderParams.tileSize).min(renderParams.outputSize),
                    m_frameBuffer,
                    scene,
                    m_renderer));
            }
        }
    }
}