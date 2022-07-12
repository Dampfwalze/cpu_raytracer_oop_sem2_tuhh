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
    };

} // namespace rt

#endif // RT_RENDERER_HPP