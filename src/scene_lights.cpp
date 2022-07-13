#include <scene/scene_lights.h>

namespace rt
{
    std::optional<double> SceneLight::getMaxDistance() const { return std::nullopt; }

    namespace Lights
    {
        PointLight::PointLight(m::dvec3 position, m::Color<float> color)
            : position(position), color(color) {}

        std::optional<m::dvec3> PointLight::getLightDirection(const m::dvec3 &position) const
        {
            return this->position - position;
        }

        m::Color<float> PointLight::getColor(const m::dvec3 &position) const
        {
            return color;
        }

        DirectionalLight::DirectionalLight(m::dvec3 direction, m::Color<float> color)
            : direction(direction), color(color) {}

        std::optional<m::dvec3> DirectionalLight::getLightDirection(const m::dvec3 &position) const
        {
            return direction;
        }

        m::Color<float> DirectionalLight::getColor(const m::dvec3 &position) const
        {
            return color;
        }
    }
}