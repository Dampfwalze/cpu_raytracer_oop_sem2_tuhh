#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <rtmath.h>
#include <scene/scene_object.h>

namespace rt
{
    namespace m = math;

    class Scene;
    class RTRenderer;

    class Material : public SceneObject
    {
    public:
        Material(const std::string_view &name);

        virtual m::Color<float> render(const m::dvec3 &position,
                                       const m::dvec3 &normal,
                                       const m::dvec3 &hitDirection,
                                       const Scene &scene,
                                       const RTRenderer &renderer,
                                       int recursionDepth) = 0;
    };

    namespace Materials
    {
        class LitMaterial : public Material
        {
        public:
            m::Color<float> color;

            float ambient;
            float diffuse;
            float specular;
            float reflection;

        public:
            LitMaterial(const std::string_view &name,
                        const m::Color<float> &color,
                        float ambient = 0.1f,
                        float diffuse = 1.0f,
                        float specular = 1.0f,
                        float reflection = 0.1f);
            LitMaterial(const m::Color<float> &color,
                        float ambient = 0.1f,
                        float diffuse = 1.0f,
                        float specular = 1.0f,
                        float reflection = 0.1f);

            virtual m::Color<float> render(const m::dvec3 &position,
                                           const m::dvec3 &normal,
                                           const m::dvec3 &hitDirection,
                                           const Scene &scene,
                                           const RTRenderer &renderer,
                                           int recursionDepth) override;

            virtual std::ostream &toString(std::ostream &stream) const override;
        };

    } // namespace Materials

} // namespace rt

#endif // MATERIAL_HPP