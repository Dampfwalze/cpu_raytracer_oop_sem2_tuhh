#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <rtmath.h>
#include <scene/scene.h>

namespace rt
{
    namespace m = math;

    class Renderer
    {
    public:
        class PixelRenderer
        {
        public:
            m::uvec2 pixelCoords;

            const Scene &scene;

        public:
            PixelRenderer(const Scene &scene);
            virtual ~PixelRenderer();

            virtual m::Pixel<float> renderPixel() const = 0;
        };

    public:
        Renderer();
        virtual ~Renderer();

        virtual PixelRenderer *createPixelRenderer(const Scene &scene) const = 0;
    };

    template <class T>
    class RendererWith : public Renderer
    {
    public:
        RendererWith() {}
        ~RendererWith() {}

        virtual PixelRenderer *createPixelRenderer(const Scene &scene) const override { return new T(scene); }
    };

} // namespace rt

#endif // RENDERER_HPP