#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <rtmath.h>
#include <thread_pool.h>
#include <scene/scene.h>
#include <frame_buffer.h>
#include <render_params.h>

namespace rt
{
    namespace m = math;

    class Renderer
    {
    public:
        struct RenderTask
        {
            m::Rect<size_t> rect;

            Renderer *owner;

            void run();
        };

    private:
    protected:
        ThreadPool<RenderTask> *threadPool;

        Scene *scene;

        FrameBuffer *frameBuffer;

        RenderParams *renderParams;

    public:
        Renderer();
        virtual ~Renderer();

    protected:
        virtual void render();
        virtual void renderTile(const m::Rect<size_t> &tile);
        virtual void renderPixel(const m::vec2<size_t> &coords);

        virtual void beginFrame();
        virtual void endFrame();

        friend RenderTask;

    public:
        inline void doRender(ThreadPool<RenderTask> *threadPool, Scene *scene, FrameBuffer *frameBuffer, RenderParams *renderParams)
        {
            this->threadPool = threadPool;
            this->scene = scene;
            this->frameBuffer = frameBuffer;
            this->renderParams = renderParams;
            beginFrame();
            render();
            endFrame();
            this->renderParams = nullptr;
            this->frameBuffer = nullptr;
            this->scene = nullptr;
            this->threadPool = nullptr;
        }
    };
} // namespace rt

#endif // RENDERER_HPP