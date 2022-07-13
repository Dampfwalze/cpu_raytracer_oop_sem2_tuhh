#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <rtmath.h>
#include <thread_pool.h>
#include <scene/scene.h>
#include <frame_buffer.h>
#include <render_params.h>
#include <future>

namespace rt
{
    namespace m = math;

    class Renderer
    {
    public:
        using task_type = std::packaged_task<void()>;

    protected:
        ThreadPool<task_type> *threadPool;

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

    public:
        void doRender(ThreadPool<task_type> *threadPool, Scene *scene, FrameBuffer *frameBuffer, RenderParams *renderParams);
    };
} // namespace rt

#endif // RENDERER_HPP