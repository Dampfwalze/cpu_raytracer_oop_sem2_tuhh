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

        RENDER_LOG(ray, "\n");
        // ray *= m::inverse(cameraMatrix);

        m::dvec4 origin = invCam * m::dvec4(ray.origin, 1);
        m::dvec4 direction = (invCam * m::dvec4(ray.origin + ray.direction, 1));

        ray = m::ray(origin.xyz() / origin.w, direction.xyz() / direction.w - origin.xyz() / origin.w);

        // ray.normalize();

        RENDER_LOG(ray, "\n");

        Intersection nearestInter;
        SceneShape *nearestObj;
        double nearestDepth2 = std::numeric_limits<double>::max();

        for (auto &&i : scene.objects)
        {
            m::dmat4 mat = i->transform.getMatrix();
            m::dmat4 invMat = m::inverse(i->transform.getMatrix());

            m::ray localRay = ray * invMat;

            auto intersection = i->intersect(localRay);

            if (!intersection.has_value())
                continue;

            auto &inter = intersection.value();

            inter.position = mat * m::dvec4(inter.position, 1.0);
            inter.normal = mat * m::dvec4(inter.normal, 0.0);

            double depth2 = m::distance2(ray.origin, inter.position);
            if (depth2 < nearestDepth2)
            {
                nearestDepth2 = depth2;
                nearestInter = inter;
                nearestObj = i.get();
            }
        }

        if (nearestDepth2 != std::numeric_limits<double>::max())
        {
            Material *material = scene.getMaterial(nearestObj->materialIndex);
            if (material == nullptr)
                return m::Color<double>(255, 0, 255);
            return material->render(nearestInter.position, nearestInter.normal);
        }

        return m::dvec3(coords, 0.0);
    }
}