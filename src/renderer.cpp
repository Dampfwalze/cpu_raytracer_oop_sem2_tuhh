#include <renderer.h>

namespace rt
{
    namespace m = math;

    Renderer::RenderTask::RenderTask(m::Rect<size_t> rect, FrameBuffer &frameBuffer, Scene &scene)
        : m_rect(rect),
          m_frameBuffer(frameBuffer), m_scene(scene)
    {
    }

    Renderer::RenderTask::~RenderTask()
    {
    }

    void Renderer::RenderTask::run()
    {
        for (size_t y = m_rect.start.y; y < m_rect.getEnd().y; y++)
        {
            for (size_t x = m_rect.start.x; x < m_rect.getEnd().x; x++)
            {
                if (((y + x) / 20) % 3 == 0)
                    m_frameBuffer.at(x, y) = {0, 1, 0};
                else if (((y + x) / 20) % 3 == 1)
                    m_frameBuffer.at(x, y) = {1, 0, 0};
                else
                    m_frameBuffer.at(x, y) = {0, 0, 1};
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    Renderer::Renderer(const RenderParams &renderParams)
        : m_threadPool(std::thread::hardware_concurrency() == 0 ? 1 : std::thread::hardware_concurrency()), renderParams(renderParams) {}

    Renderer::~Renderer()
    {
    }

    void Renderer::render(Scene &scene)
    {
        std::cout << "Start render" << std::endl;

        if (m_frameBuffer.getWidth() != renderParams.outputSize.width || m_frameBuffer.getHeight() != renderParams.outputSize.height)
            m_frameBuffer.resize(renderParams.outputSize.width, renderParams.outputSize.height);

        m_frameBuffer.clear();
        for (size_t x = 0; x < m_frameBuffer.getWidth(); x += renderParams.tileSize.width)
        {
            for (size_t y = 0; y < m_frameBuffer.getHeight(); y += renderParams.tileSize.height)
            {
                m_threadPool.submit(RenderTask(
                    m::Rect(m::Vec2(x, y), renderParams.tileSize).min(renderParams.outputSize),
                    m_frameBuffer,
                    scene));
            }
        }
    }
}