#ifndef SCENE_LIGHTS_HPP
#define SCENE_LIGHTS_HPP

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

            PointLight(m::dvec3 position = m::dvec3(0), m::Color<float> color = m::Color<float>(0.9));

            std::optional<m::dvec3> getLightDirection(const m::dvec3 &position) const override;
            virtual m::Color<float> getColor(const m::dvec3 &position) const override;
        };
    } // namespace Lights

} // namespace rt

#endif // SCENE_LIGHTS_HPP
