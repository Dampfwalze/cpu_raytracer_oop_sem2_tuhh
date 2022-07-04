#include <rt_renderer.h>
namespace rt
{
    RTRenderer::RTRenderer(const Scene &scene)
        : PixelRenderer(scene)
    {
    }

    RTRenderer::~RTRenderer()
    {
    }

    m::Pixel<float> RTRenderer::renderPixel() const
    {
        if (((pixelCoords.y + pixelCoords.x) / 20) % 3 == 0)
            return {0, 1, 0};
        else if (((pixelCoords.y + pixelCoords.x) / 20) % 3 == 1)
            return {1, 0, 0};
        else
            return {0, 0, 1};
    }
}