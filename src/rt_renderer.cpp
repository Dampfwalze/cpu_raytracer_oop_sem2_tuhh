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

        auto invCam = m::inverse(cameraMatrix);

        log(ray, "\n");
        // ray *= m::inverse(cameraMatrix);

        m::dvec4 origin = invCam * m::dvec4(ray.origin, 1);
        m::dvec4 direction = (invCam * m::dvec4(ray.origin + ray.direction, 1));

        ray = m::ray(origin.xyz() / origin.w, direction.xyz() / direction.w - origin.xyz() / origin.w);

        // ray.normalize();

        log(ray, "\n");

        for (auto &&i : scene.objects)
        {
            m::dmat4 mat = m::inverse(i->transform.getMatrix());
            m::ray localRay = ray * mat;

            auto intersection = i->intersect(localRay);

            if (intersection.has_value())
            {
                auto l = m::dot(glm::normalize(intersection.value().normal), m::dvec3(0, -1, 0));
                return m::Color<double>{l};
            }
        }

        return m::dvec3(coords, 0.0);
    }
}