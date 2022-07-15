#include <scene/scene_shapes.h>

#include <stream_formatter.h>

#include <rt_imgui.h>

namespace rt
{
    SceneShape::SceneShape(const std::string_view &name, const Transform &transform, size_t materialIndex)
        : transform(transform), materialIndex(materialIndex), SceneObject(name) {}

    bool SceneShape::onInspectorGUI()
    {
        return rtImGui::Drag("Transform", transform, 0.01f);
    }

    namespace Shapes
    {
        // Sphere

        Sphere::Sphere(const std::string_view &name, double radius, const Transform &transform, size_t materialIndex)
            : radius(radius), SceneShape(name, transform, materialIndex) {}
        Sphere::Sphere(double radius, const Transform &transform, size_t materialIndex)
            : radius(radius), SceneShape("Sphere", transform, materialIndex) {}

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
            if (t < 0.01)
                return std::nullopt;

            Intersection i;
            i.position = ray(t);
            i.normal = i.position;
            i.object = (SceneShape *)this;

            return i;
        }

        bool Sphere::onInspectorGUI()
        {
            return SceneShape::onInspectorGUI() | rtImGui::Drag("Radius", radius, 0.01f);
        }

        std::ostream &Sphere::toString(std::ostream &stream) const
        {
            stream << "Sphere { name: \"" << name << "\", transform: " << transform << ", radius: " << radius << "}";
            return stream;
        }

        // Plane

        Plane::Plane(const std::string_view &name, const Transform &transform, size_t materialIndex)
            : SceneShape(name, transform, materialIndex) {}
        Plane::Plane(const Transform &transform, size_t materialIndex)
            : SceneShape("Plane", transform, materialIndex) {}

        std::optional<Intersection> Plane::intersect(const m::ray<double> &ray) const
        {
            double t = -ray.origin.y / ray.direction.y;
            if (t < 0.01)
                return std::nullopt;
            Intersection i;
            i.position = ray(t);
            i.normal = m::dvec3(0, 1, 0);
            i.object = (SceneShape *)this;
            return i;
        }

        std::ostream &Plane::toString(std::ostream &stream) const
        {
            return stream << "Plane { name: \"" << name << "\", transform: " << transform << " }";
        }
    }
}