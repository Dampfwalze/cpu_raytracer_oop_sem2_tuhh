#include <scene/scene_shapes.h>

#include <stream_formatter.h>

#include <rt_imgui.h>

namespace rt
{
    bool SceneShape::onInspectorGUI()
    {
        return rtImGui::Drag<Transform, double>("Transform", transform, 0.01, std::nullopt, std::nullopt, "%.3f");
    }

    std::ostream &operator<<(std::ostream &stream, const SceneShape &shape)
    {
        shape.toString(stream);
        return stream;
    }

    namespace Shapes
    {
        // Sphere

        Sphere::Sphere(double radius, const Transform &transform)
            : radius(radius), SceneShape(transform) {}
        Sphere::~Sphere() {}

        std::optional<Intersection> Sphere::intersect(const m::ray<double> &ray) const
        {
            const m::dvec3 &o = ray.origin;
            const m::dvec3 &d = ray.direction;

            double a = m::dot(d, d);
            double b = 2 * (m::dot(o, d));
            double c = m::dot(o, o) - radius * radius;

            double result = b * b - 4 * a * c;

            if (result < 0)
                return std::nullopt;

            double t = (-b - std::sqrt(result)) / (2 * a);
            if (t < 0)
                return std::nullopt;

            Intersection i;
            i.position = ray(t);
            i.normal = i.position;

            return i;
        }

        bool Sphere::onInspectorGUI()
        {
            return SceneShape::onInspectorGUI() | rtImGui::Drag("Radius", radius, 0.01);
        }

        std::ostream &Sphere::toString(std::ostream &stream) const
        {
            stream << "Sphere { " << transform << ", radius: " << radius << "}";
            return stream;
        }

        // Plane

        Plane::Plane(const Transform &transform)
            : SceneShape(transform) {}
        Plane::~Plane() {}

        std::optional<Intersection> Plane::intersect(const m::ray<double> &ray) const
        {
            double t = -ray.origin.y / ray.direction.y;
            if (t < 0)
                return std::nullopt;
            Intersection i;
            i.position = ray(t);
            i.normal = m::dvec3(0, -1, 0);
            return i;
        }

        std::ostream &Plane::toString(std::ostream &stream) const
        {
            stream << "Plane { " << transform << " }";
            return stream;
        }
    }
}