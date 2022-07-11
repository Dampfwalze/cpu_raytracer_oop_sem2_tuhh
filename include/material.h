#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <rtmath.h>

namespace rt
{
    namespace m = math;

    class Material
    {
    private:
    public:
        Material();
        virtual ~Material();

        virtual m::Color<float> render(const m::dvec3 &position, const m::dvec3 &normal) = 0;
    };

    namespace Materials
    {
        class LitMaterial : public Material
        {
        public:
            m::Color<float> color;

        public:
            LitMaterial(const m::Color<float> &color);
            virtual ~LitMaterial();

            virtual m::Color<float> render(const m::dvec3 &position, const m::dvec3 &normal) override;
        };

    } // namespace Materials

} // namespace rt

#endif // MATERIAL_HPP