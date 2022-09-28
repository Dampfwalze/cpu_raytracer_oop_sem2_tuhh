#include <scene/material.h>

#include <scene/scene.h>
#include <rt_renderer.h>
#include <pixel_logger.h>
#include <rt_imgui.h>

namespace rt
{
    Material::Material(const std::string_view &name)
        : SceneObject(name) {}

    namespace Materials
    {
        LitMaterial::LitMaterial(const std::string_view &name,
                                 SamplerRef<> color,
                                 float ambient,
                                 float diffuse,
                                 float specular,
                                 float reflection)
            : color(std::move(color)), ambient(ambient), diffuse(diffuse), specular(specular), reflection(reflection), Material(name) {}

        LitMaterial::LitMaterial(SamplerRef<> color,
                                 float ambient,
                                 float diffuse,
                                 float specular,
                                 float reflection)
            : color(std::move(color)), ambient(ambient), diffuse(diffuse), specular(specular), reflection(reflection), Material("Lit") {}

        static inline m::Color<float> mixColor(m::Color<float> c1, m::Color<float> c2, float mixingFactor)
        {
            float j = mixingFactor;
            float i = 1 / j;
            return m::Color<float>(
                (1 - (1 - c1.r * i) * (1 - c2.r * i)) * j,
                (1 - (1 - c1.g * i) * (1 - c2.g * i)) * j,
                (1 - (1 - c1.b * i) * (1 - c2.b * i)) * j);
        }

        m::Color<float> LitMaterial::render(const m::dvec3 &position, const m::dvec3 &normal_, const m::dvec3 &hitDirection_,
                                            const SampleInfo &sampleInfo, const Scene &scene, const RTRenderer &renderer, int recursionDepth)
        {
            PIXEL_LOGGER_LOG("Material { ");
            using Color = m::Color<float>;

            m::dvec3 normal = glm::normalize(normal_);
            m::dvec3 hitDirection = glm::normalize(hitDirection_);

            Color e_ambiant = Color(ambient);

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
                result = mixColor(result, diffuse * lightColor * glm::max(0.0f, (float)m::dot(normal, lightDirection)), renderer.renderParams->mixingFactor);
                result = mixColor(result, specular * lightColor * glm::max(0.0f, (float)m::dot(hitDirection, m::reflect(lightDirection, normal))), renderer.renderParams->mixingFactor);
            }
            PIXEL_LOGGER_LOG(" }");
            auto sampled = color ? color->sample(sampleInfo) : Color(1.0, 0.0, 1.0);
            return mixColor(result, e_reflection, renderer.renderParams->mixingFactor) * sampled;
        }

        bool LitMaterial::onInspectorGUI()
        {
            bool changed = false;

            if (ImGui::TreeNodeEx("Color", ImGuiTreeNodeFlags_FramePadding))
            {
                changed |= color.onInspectorGUI();
                ImGui::TreePop();
            }
            changed |= rtImGui::Drag<float, float>("ambient", ambient, 0.01f, 0, 1.0f);
            changed |= rtImGui::Drag<float, float>("diffuse", diffuse, 0.01f, 0, 1.0f);
            changed |= rtImGui::Drag<float, float>("specular", specular, 0.01f, 0, 1.0f);
            changed |= rtImGui::Drag<float, float>("reflection", reflection, 0.01f, 0, 1.0f);
            return changed;
        }

        std::ostream &LitMaterial::toString(std::ostream &stream) const
        {
            return stream << "LitMaterial { name: \"" << name
                          << "\", color: " << color
                          << ", ambient: " << ambient
                          << ", diffuse: " << diffuse
                          << ", specular: " << specular
                          << ", reflection: " << reflection << " }";
        }
    }
}