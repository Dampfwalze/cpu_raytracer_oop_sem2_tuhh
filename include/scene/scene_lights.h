#ifndef SCENE_LIGHTS_HPP
#define SCENE_LIGHTS_HPP

#include <optional>
#include <rtmath.h>

namespace rt
{
    namespace m = math;

    class SceneLight
    {
    public:
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

            PointLight(m::dvec3 position = m::dvec3(0), m::Color<float> color = m::Color<float>(0.9f));

            std::optional<m::dvec3> getLightDirection(const m::dvec3 &position) const override;
            m::Color<float> getColor(const m::dvec3 &position) const override;
        };

        struct DirectionalLight : public SceneLight
        {
            m::dvec3 direction;
            m::Color<float> color;

            DirectionalLight(m::dvec3 direction = m::dvec3(0, -1, 0), m::Color<float> color = m::Color<float>(0.9f));

            std::optional<m::dvec3> getLightDirection(const m::dvec3 &position) const override;
            m::Color<float> getColor(const m::dvec3 &position) const override;
        };
    } // namespace Lights

} // namespace rt

#endif // SCENE_LIGHTS_HPP
