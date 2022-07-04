#include <renderer.h>
namespace rt
{
    Renderer::PixelRenderer::PixelRenderer(const Scene &scene) : scene(scene) {}
    Renderer::PixelRenderer::~PixelRenderer() {}

    Renderer::Renderer() {}
    Renderer::~Renderer() {}
}