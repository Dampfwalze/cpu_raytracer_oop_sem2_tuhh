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
        m::dvec2 coords = static_cast<m::dvec2>(pixelCoords) / static_cast<m::dvec2>(screenSize) * 2.0 - m::dvec2(1);
        m::ray<double> ray(m::dvec3(coords, -1), m::dvec3(0, 0, 1));

        log("Hello from pixel at: ", pixelCoords);

        if (((pixelCoords.y + pixelCoords.x) / 20) % 3 == 0)
            return {0, 1, 0};
        else if (((pixelCoords.y + pixelCoords.x) / 20) % 3 == 1)
            return {1, 0, 0};
        else
            return {0, 0, 1};
    }
}