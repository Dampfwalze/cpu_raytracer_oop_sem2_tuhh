#include <rt_renderer.h>

namespace rt
{
    void RTRenderer::beginFrame()
    {
        scene->cacheFrameData(frameBuffer->getSize());
    }

    void RTRenderer::renderPixel(const m::vec2<size_t> &pixelCoords)
    {
        auto screenSize = frameBuffer->getSize();

        m::dvec2 coords = static_cast<m::dvec2>(pixelCoords) / static_cast<m::dvec2>(screenSize) * 2.0 - m::dvec2(1);
        m::ray<double> ray(m::dvec3(coords, -1), m::dvec3(0, 0, 1));
        auto &outColor = frameBuffer->at(pixelCoords);
        outColor = m::dvec3(coords, 0.0);

        auto invCam = scene->camera.cached.inverseMatrix;

        // ray *= m::inverse(cameraMatrix);

        m::dvec4 origin = invCam * m::dvec4(ray.origin, 1);
        m::dvec4 direction = (invCam * m::dvec4(ray.origin + ray.direction, 1));

        ray = m::ray(origin.xyz() / origin.w, direction.xyz() / direction.w - origin.xyz() / origin.w);

        // ray.normalize();

        Intersection nearestInter;
        SceneShape *nearestObj;
        double nearestDepth2 = std::numeric_limits<double>::max();

        for (auto &&i : scene->objects)
        {
            m::dmat4 mat = i->transform.cached.matrix;
            m::dmat4 invMat = i->transform.cached.inverseMatrix;

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
            Material *material = scene->getMaterial(nearestObj->materialIndex);
            if (material == nullptr)
            {
                outColor = m::Color<double>(255, 0, 255);
                return;
            }
            outColor = material->render(nearestInter.position, nearestInter.normal);
            return;
        }
    }
}