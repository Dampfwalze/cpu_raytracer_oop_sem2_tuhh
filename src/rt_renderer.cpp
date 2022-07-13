#include <rt_renderer.h>
#include <pixel_logger.h>

namespace rt
{
    void RTRenderer::beginFrame()
    {
        scene->cacheFrameData(frameBuffer->getSize());
    }

    void RTRenderer::renderPixel(const m::vec2<size_t> &pixelCoords)
    {
        auto screenSize = frameBuffer->getSize();
        auto coords = static_cast<m::dvec2>(pixelCoords) / static_cast<m::dvec2>(screenSize) * 2.0 - m::dvec2(1);

        auto &outColor = frameBuffer->at(pixelCoords);
        outColor = m::dvec3(coords, 0.0);

        auto invCam = scene->camera.cached.inverseMatrix;

        // Ray is in camera space
        m::ray<double> ray(m::dvec3(coords, -1), m::dvec3(0, 0, 1));

        PIXEL_LOGGER_LOG(ray, "\n");

        // Ray is in world space now
        ray = ray.transformPerspective(invCam);

        auto maybeIntersection = scene->castRay(ray);

        if (maybeIntersection)
        {
            auto &intersection = maybeIntersection.value();
            Material *material = scene->getMaterial(intersection.object->materialIndex);
            if (material == nullptr)
            {
                outColor = m::Color<double>(1, 0, 1);
                return;
            }
            outColor = material->render(intersection.position, intersection.normal);
            return;
        }
    }
}