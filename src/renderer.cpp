#include <renderer.h>
#include <pixel_logger.h>
#include <profiler.h>

namespace rt
{
    Renderer::Renderer() {}
    Renderer::~Renderer() {}

    void Renderer::render()
    {
        auto size = frameBuffer->getSize();
        for (size_t x = 0; x < size.x; x += renderParams->tileSize.x)
        {
            for (size_t y = 0; y < size.y; y += renderParams->tileSize.y)
            {
                RenderTask task = {
                    m::Rect(m::u64vec2(x, y), renderParams->tileSize).min(size),
                    this,
                };
                *threadPool << task;
            }
        }

        Profiling::profiler.profileTask("Wait constant");
        std::this_thread::sleep_for(std::chrono::microseconds(100));

        Profiling::profiler.profileTask("Wait for finish");
        threadPool->wait();
    }

    void Renderer::RenderTask::run()
    {
        Profiling::profiler.profileTask("Render Tile");
        owner->renderTile(rect);
        Profiling::profiler.profileTask("Get");
    }

    void Renderer::renderTile(const m::Rect<size_t> &tile)
    {
        for (size_t y = tile.start.y; y < tile.getEnd().y; y++)
            for (size_t x = tile.start.x; x < tile.getEnd().x; x++)
            {
                if (renderParams->logPixel == m::u64vec2(x, y))
                    PixelLogger::logger.beginLog();
                renderPixel(m::u64vec2(x, y));
                PixelLogger::logger.endLog();
            }
    }

    void Renderer::renderPixel(const m::vec2<size_t> &coords) {}
    void Renderer::beginFrame() {}
    void Renderer::endFrame() {}

    void Renderer::doRender(ThreadPool<RenderTask> *threadPool, Scene *scene, FrameBuffer *frameBuffer, RenderParams *renderParams)
    {
        this->threadPool = threadPool;
        this->scene = scene;
        this->frameBuffer = frameBuffer;
        this->renderParams = renderParams;
        Profiling::profiler.profileTask("BeginFrame");
        beginFrame();
        Profiling::profiler.profileTask("Render");
        render();
        Profiling::profiler.profileTask("EndFrame");
        endFrame();
        Profiling::profiler.endTask();
        this->renderParams = nullptr;
        this->frameBuffer = nullptr;
        this->scene = nullptr;
        this->threadPool = nullptr;
    }
}