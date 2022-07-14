#include <material.h>

#include <scene/scene.h>
#include <rt_renderer.h>
#include <pixel_logger.h>

namespace rt
{
    Material::Material(const std::string_view &name)
        : SceneObject(name) {}

    namespace Materials
    {
        LitMaterial::LitMaterial(const std::string_view &name,
                                 const m::Color<float> &color,
                                 float ambient,
                                 float diffuse,
                                 float specular,
                                 float reflection)
            : color(color), ambient(ambient), diffuse(diffuse), specular(specular), reflection(reflection), Material(name) {}
        LitMaterial::LitMaterial(const m::Color<float> &color,
                                 float ambient,
                                 float diffuse,
                                 float specular,
                                 float reflection)
            : color(color), ambient(ambient), diffuse(diffuse), specular(specular), reflection(reflection), Material("Lit") {}

        static inline m::Color<float> mixColor(m::Color<float> c1, m::Color<float> c2)
        {
            return m::Color<float>(
                1 - (1 - c1.r) * (1 - c2.r),
                1 - (1 - c1.g) * (1 - c2.g),
                1 - (1 - c1.b) * (1 - c2.b));
        }

        m::Color<float> LitMaterial::render(const m::dvec3 &position, const m::dvec3 &normal_, const m::dvec3 &hitDirection, const Scene &scene, const RTRenderer &renderer, int recursionDepth)
        {
            PIXEL_LOGGER_LOG("Material { ");
            using Color = m::Color<float>;

            m::dvec3 normal = glm::normalize(normal_);

            Color e_ambiant = color * ambient;

            Color e_reflection(0);
            if (recursionDepth > 0)
            {
                m::ray<double> reflected(
                    position,
                    m::reflect(hitDirection, normal));
                e_reflection = renderer.castPropagationRay(reflected, recursionDepth - 1) * reflection;
            }
            PIXEL_LOGGER_LOG("Reflection: ", e_reflection, "\n");

            std::vector<SceneLight *> i_Lights;
            i_Lights.reserve(scene.lights.size());
            for (auto &&light : scene.lights)
            {
                auto color = renderer.castLightRay(position, *light);
                if (color)
                    i_Lights.push_back(light.get());
            }

            Color result = e_ambiant;
            for (auto &&i_light : i_Lights)
            {
                Color lightColor = i_light->getColor(position);
                auto lightDirection = glm::normalize(i_light->getLightDirection(position).value());
                result = mixColor(result, diffuse * lightColor * (float)m::dot(normal, lightDirection));
                result = mixColor(result, specular * lightColor * (float)m::dot(hitDirection, m::reflect(lightDirection, normal)));
            }
            PIXEL_LOGGER_LOG(" }");
            return mixColor(result, e_reflection);
        }
    }
}