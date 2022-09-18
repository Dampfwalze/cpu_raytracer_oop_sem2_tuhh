#ifndef RT_RENDERER_HPP
#define RT_RENDERER_HPP

#include <renderer.h>

namespace rt
{
    namespace m = math;

    class RTRenderer : public Renderer
    {
    public:
        void beginFrame() override;

        void renderPixel(const m::vec2<size_t> &coords) override;

        m::Color<float>                castPropagationRay(const m::ray<double> &ray, int recursion = 5) const;
        std::optional<m::Color<float>> castLightRay(const m::dvec3 position, const SceneLight &light) const;
    };

} // namespace rt

#endif // RT_RENDERER_HPP