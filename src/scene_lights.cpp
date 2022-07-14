#include <scene/scene_lights.h>

namespace rt
{
    SceneLight::SceneLight(const std::string_view &name)
        : SceneObject(name) {}

    std::optional<double> SceneLight::getMaxDistance() const { return std::nullopt; }

    namespace Lights
    {
        PointLight::PointLight(const std::string_view &name, m::dvec3 position, m::Color<float> color)
            : position(position), color(color), SceneLight(name) {}
        PointLight::PointLight(m::dvec3 position, m::Color<float> color)
            : position(position), color(color), SceneLight("PointLight") {}

        std::optional<m::dvec3> PointLight::getLightDirection(const m::dvec3 &position) const
        {
            return this->position - position;
        }

        m::Color<float> PointLight::getColor(const m::dvec3 &position) const
        {
            return color;
        }

        std::ostream &PointLight::toString(std::ostream &stream) const
        {
            return stream << "PointLight { name: \"" << name << "\", position: " << position << ", color: " << color << " }";
        }

        DirectionalLight::DirectionalLight(const std::string_view &name, m::dvec3 direction, m::Color<float> color)
            : direction(direction), color(color), SceneLight(name) {}
        DirectionalLight::DirectionalLight(m::dvec3 direction, m::Color<float> color)
            : direction(direction), color(color), SceneLight("DirectionalLight") {}

        std::optional<m::dvec3> DirectionalLight::getLightDirection(const m::dvec3 &position) const
        {
            return direction;
        }

        m::Color<float> DirectionalLight::getColor(const m::dvec3 &position) const
        {
            return color;
        }

        std::ostream &DirectionalLight::toString(std::ostream &stream) const
        {
            return stream << "DirectionalLight { name: \"" << name << "\", direction: " << direction << ", color: " << color << " }";
        }
    }
}