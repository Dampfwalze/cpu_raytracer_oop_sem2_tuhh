#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <rtmath.h>
#include <scene/scene.h>

#define RENDER_LOG(...)    \
    if (logger != nullptr) \
    log(__VA_ARGS__)

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
            m::uvec2 screenSize;

            m::dmat4 cameraMatrix;

            mutable std::ostream* logger = nullptr;

            const Scene &scene;

        public:
            PixelRenderer(const Scene &scene);
            virtual ~PixelRenderer();

            virtual m::Pixel<float> renderPixel() const = 0;

            template<typename T>
            void log(T val) const
            {
                if (logger == nullptr) return;
                *logger << val;
            }
            template<typename T, typename ... Args>
            void log(T val, Args... vals) const
            {
                if (logger == nullptr) return;
                *logger << val;
                log(vals...);
            }
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