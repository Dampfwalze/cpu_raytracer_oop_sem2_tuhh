#include <Renderer.h>

namespace rt
{
    Renderer::RenderTask::RenderTask()
    {
    }

    Renderer::RenderTask::~RenderTask()
    {
    }

    void Renderer::RenderTask::run()
    {
        std::cout << "Render Task run by: " << std::this_thread::get_id() << std::endl;
    }

    Renderer::Renderer() : m_threadPool(std::thread::hardware_concurrency() == 0 ? 1 : std::thread::hardware_concurrency())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        for (size_t i = 0; i < 3; i++)
            m_threadPool.submit(RenderTask());
    }

    Renderer::~Renderer()
    {
    }
}