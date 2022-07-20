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

        auto invCam = scene->camera.cached.inverseMatrix;

        // Ray is in camera space
        m::ray<double> ray(m::dvec3(coords, -1), m::dvec3(0, 0, 1));

        PIXEL_LOGGER_LOG(ray, "\n");

        // Ray is in world space now
        ray = ray.transformPerspective(invCam);

        frameBuffer->at(pixelCoords) = castPropagationRay(ray);
    }

    m::Color<float> RTRenderer::castPropagationRay(const m::ray<double> &ray, int recursion) const
    {
        PIXEL_LOGGER_LOG("Cast Propagation Ray { ");
        auto maybeIntersection = scene->castRay(ray);

        if (!maybeIntersection)
        {
            PIXEL_LOGGER_LOG("No Intersection! }\n");
            return m::Color<float>(0);
            // return m::Color<float>(115, 185, 255) / 255.0f;
        }

        auto &intersection = maybeIntersection.value();
        PIXEL_LOGGER_LOG("Intesected: ", intersection.object->name);

        Material *material = scene->getMaterial(intersection.object->materialIndex);
        if (material == nullptr)
        {
            PIXEL_LOGGER_LOG(", No Material! }\n");
            return m::Color<double>(1, 0, 1);
        }
        PIXEL_LOGGER_LOG(", \n");
        auto result = material->render(intersection.position, intersection.normal, ray.direction, intersection.sampleInfo, *scene, *this, recursion);
        PIXEL_LOGGER_LOG(" }\n");
        return result;
    }

    std::optional<m::Color<float>> RTRenderer::castLightRay(const m::dvec3 position, const SceneLight &light) const
    {
        auto dir = light.getLightDirection(position);
        if (!dir)
            return std::nullopt;

        m::ray<double> ray(position,
                           dir.value());

        auto maybeIntersection = scene->castRay(ray, light.getMaxDistance());

        if (maybeIntersection)
            return std::nullopt;

        return light.getColor(position);
    }
}