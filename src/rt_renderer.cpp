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

        log("Ray: ", ray, "\n",
            "Matrix:\n", cameraMatrix, "\n",
            "Inverse:\n", m::inverse(cameraMatrix), "\n");

        // log("Calculation: ", m::inverse(cameraMatrix) * m::dvec4(ray.origin, 1), "\n");
        log(m::dvec4(ray.direction + ray.origin, 1), "\n");
        log(m::inverse(cameraMatrix) * m::dvec4(ray.direction + ray.origin, 1), "\n");
        log(m::inverse(cameraMatrix) * m::dvec4(ray.origin, 1), "\n");
        log("Calculation: ", (m::inverse(cameraMatrix) * m::dvec4(ray.direction + ray.origin, 1)) - m::inverse(cameraMatrix) * m::dvec4(ray.origin, 1), "\n");

        auto invCam = m::inverse(cameraMatrix);

        log(ray, "\n");
        // ray *= m::inverse(cameraMatrix);

        m::dvec4 origin = invCam * m::dvec4(ray.origin, 1);
        m::dvec4 direction = (invCam * m::dvec4(ray.origin + ray.direction, 1));

        ray = m::ray(origin.xyz() / origin.w, direction.xyz() / direction.w - origin.xyz() / origin.w);

        ray.normalize();

        log(ray, "\n");

        for (auto &&i : scene.objects)
        {
            auto sphere = dynamic_cast<Shapes::Sphere &>(*i);

            m::dmat4 mat = m::inverse(i->transform.getMatrix());
            m::ray localRay = ray * mat;

            // log(mat, "\n", localRay, "\n");

            m::dvec3 &o = localRay.origin;
            m::dvec3 &d = localRay.direction;

            double r_sq = sphere.radius * sphere.radius;

            double a = m::dot(d, d); // d.x * d.x + d.y * d.y;
            double b = 2 * (m::dot(o, d));
            double c = m::dot(o, o) - r_sq; // o.x * o.x + o.y * o.y;

            double result = b * b - 4 * a * c;

            // log(result, "\n");

            if (result >= 0)
                return m::Color<double>(1, 1, 0);
        }

        return m::dvec3(coords, 0.0);

        // if (((pixelCoords.y + pixelCoords.x) / 20) % 3 == 0)
        //     return {0, 1, 0};
        // else if (((pixelCoords.y + pixelCoords.x) / 20) % 3 == 1)
        //     return {1, 0, 0};
        // else
        //     return {0, 0, 1};
    }
}