#include <rt_imgui.h>
#include <scene/scene_lights.h>

namespace rt
{
    SceneLight::SceneLight(const std::string_view &name, float intensity)
        : intensity(intensity), SceneObject(name) {}

    std::optional<double> SceneLight::getMaxDistance() const { return std::nullopt; }

    namespace Lights
    {
        PointLight::PointLight(const std::string_view &name, m::dvec3 position, m::Color<float> color, float intensity)
            : position(position), color(color), SceneLight(name, intensity) {}
        PointLight::PointLight(m::dvec3 position, m::Color<float> color, float intensity)
            : position(position), color(color), SceneLight("PointLight", intensity) {}

        std::optional<m::dvec3> PointLight::getLightDirection(const m::dvec3 &position) const
        {
            return this->position - position;
        }

        m::Color<float> PointLight::getColor(const m::dvec3 &position) const
        {
            return color * intensity / (float)m::distance2(position, this->position);
        }

        bool PointLight::onInspectorGUI()
        {
            return ImGui::ColorEdit3("Color", (float *)&color) |
                   rtImGui::Drag("Intensity", intensity, 0.01f) |
                   rtImGui::Drag("Position", position, 0.01f);
        }

        std::ostream &PointLight::toString(std::ostream &stream) const
        {
            return stream << "PointLight { name: \"" << name << "\", position: " << position << ", color: " << color << " }";
        }

        DirectionalLight::DirectionalLight(const std::string_view &name, m::dvec3 direction, m::Color<float> color, float intensity)
            : direction(direction), color(color), SceneLight(name, intensity) {}
        DirectionalLight::DirectionalLight(m::dvec3 direction, m::Color<float> color, float intensity)
            : direction(direction), color(color), SceneLight("DirectionalLight", intensity) {}

        std::optional<m::dvec3> DirectionalLight::getLightDirection(const m::dvec3 &position) const
        {
            return direction;
        }

        m::Color<float> DirectionalLight::getColor(const m::dvec3 &position) const
        {
            return color * intensity;
        }

        bool DirectionalLight::onInspectorGUI()
        {
            return ImGui::ColorEdit3("Color", (float *)&color) |
                   rtImGui::Drag("Intensity", intensity, 0.01f) |
                   rtImGui::Drag("Direction", direction, 0.01f);
        }

        std::ostream &DirectionalLight::toString(std::ostream &stream) const
        {
            return stream << "DirectionalLight { name: \"" << name << "\", direction: " << direction << ", color: " << color << " }";
        }
    }
}