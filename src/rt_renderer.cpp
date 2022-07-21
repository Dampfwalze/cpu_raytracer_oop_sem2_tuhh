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

        auto color = castPropagationRay(ray, renderParams->recursionDeph);

        // Tone mapping
        switch (renderParams->toneMappingAlgorithm)
        {
        case RenderParams::Reinhard:
            color = color / (color + m::Color<float>(1));
            break;
        case RenderParams::Exposure:
            color = m::fvec3(1.0f) - m::exp(-color * renderParams->exposure);
            break;
        }
        // gamma correction
        color = m::pow(color * renderParams->scale, m::fvec3(1.0f / renderParams->gamma));

        frameBuffer->at(pixelCoords) = color;
    }

    m::Color<float> RTRenderer::castPropagationRay(const m::ray<double> &ray, int recursion) const
    {
        PIXEL_LOGGER_LOG("Cast Propagation Ray { ");
        auto maybeIntersection = scene->castRay(ray);

        if (!maybeIntersection)
        {
            PIXEL_LOGGER_LOG("No Intersection! }\n");
            return scene->environmentTexture->sample({
                .type = SampleInfoType::Direction,
                .asDirection = ray.direction,
            });
            return m::Color<float>(0);
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