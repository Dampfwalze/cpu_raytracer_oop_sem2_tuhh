#ifndef SCENE_LIGHTS_HPP
#define SCENE_LIGHTS_HPP

#include <optional>
#include <rtmath.h>
#include <scene/scene_object.h>

namespace rt
{
    namespace m = math;

    class SceneLight : public SceneObject
    {
    public:
        SceneLight(const std::string_view &name);
        virtual ~SceneLight() = default;

        virtual std::optional<m::dvec3> getLightDirection(const m::dvec3 &position) const = 0;
        virtual std::optional<double> getMaxDistance() const;
        virtual m::Color<float> getColor(const m::dvec3 &position) const = 0;
    };

    namespace Lights
    {
        struct PointLight : public SceneLight
        {
            m::dvec3 position;
            m::Color<float> color;

            PointLight(const std::string_view &name, m::dvec3 position = m::dvec3(0), m::Color<float> color = m::Color<float>(0.9f));
            PointLight(m::dvec3 position = m::dvec3(0), m::Color<float> color = m::Color<float>(0.9f));

            virtual std::optional<m::dvec3> getLightDirection(const m::dvec3 &position) const override;
            virtual m::Color<float> getColor(const m::dvec3 &position) const override;

            virtual std::ostream &toString(std::ostream &stream) const override;
        };

        struct DirectionalLight : public SceneLight
        {
            m::dvec3 direction;
            m::Color<float> color;

            DirectionalLight(const std::string_view &name, m::dvec3 direction = m::dvec3(0, -1, 0), m::Color<float> color = m::Color<float>(0.9f));
            DirectionalLight(m::dvec3 direction = m::dvec3(0, -1, 0), m::Color<float> color = m::Color<float>(0.9f));

            virtual std::optional<m::dvec3> getLightDirection(const m::dvec3 &position) const override;
            virtual m::Color<float> getColor(const m::dvec3 &position) const override;

            virtual std::ostream &toString(std::ostream &stream) const override;
        };
    } // namespace Lights

} // namespace rt

#endif // SCENE_LIGHTS_HPP
