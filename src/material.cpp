#include <material.h>

namespace rt
{
    Material::Material() {}
    Material::~Material() {}

    namespace Materials
    {
        LitMaterial::LitMaterial(const m::Color<float> &color)
            : color(color) {}

        LitMaterial::~LitMaterial() {}

        m::Color<float> LitMaterial::render(const m::dvec3 &position, const m::dvec3 &normal)
        {
            double l = m::dot(glm::normalize(normal), m::dvec3(0, -1, 0));
            return color * (float)l;
        }
    }
}