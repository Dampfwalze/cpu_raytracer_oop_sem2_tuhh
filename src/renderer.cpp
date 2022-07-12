#include <renderer.h>

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

        std::this_thread::sleep_for(std::chrono::microseconds(100));

        threadPool->wait();
    }

    void Renderer::RenderTask::run()
    {
        owner->renderTile(rect);
    }

    void Renderer::renderTile(const m::Rect<size_t> &tile)
    {
        for (size_t y = tile.start.y; y < tile.getEnd().y; y++)
            for (size_t x = tile.start.x; x < tile.getEnd().x; x++)
            {
                renderPixel(m::u64vec2(x, y));
            }
    }

    void Renderer::renderPixel(const m::vec2<size_t> &coords) {}
    void Renderer::beginFrame() {}
    void Renderer::endFrame() {}
}