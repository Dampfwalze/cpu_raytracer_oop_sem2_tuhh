#ifndef RT_RENDERER_HPP
#define RT_RENDERER_HPP

#include <renderer.h>

namespace rt
{
    namespace m = math;

    class RTRenderer : public Renderer::PixelRenderer
    {
    private:
    public:
        RTRenderer(const Scene &scene);
        ~RTRenderer();

        m::Pixel<float> renderPixel() const override;
    };

} // namespace rt

#endif // RT_RENDERER_HPP