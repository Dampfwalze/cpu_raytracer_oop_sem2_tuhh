#ifndef RENDER_DISPATCHER_HPP
#define RENDER_DISPATCHER_HPP

#include <frame_buffer.h>
#include <thread_pool.h>
#include <rtmath.h>
#include <render_params.h>
#include <scene/scene.h>
#include <renderer.h>

namespace rt
{

    class RenderDispatcher
    {
    private:
        class RenderTask
        {
        private:
            math::Rect<size_t> m_rect;
            FrameBuffer &m_frameBuffer;
            Scene &m_scene;
            std::shared_ptr<Renderer> m_renderer;

        public:
            RenderTask(math::Rect<size_t> rect, FrameBuffer &frameBuffer, Scene &scene, std::shared_ptr<Renderer> &renderer);
            ~RenderTask();

            void run();
        };

    private:
        FrameBuffer m_frameBuffer;

        ThreadPool<RenderTask> m_threadPool;

        std::shared_ptr<Renderer> m_renderer;

    public:
        RenderParams renderParams;

    public:
        RenderDispatcher(const RenderParams &renderParams, const std::shared_ptr<Renderer> &renderer);
        ~RenderDispatcher();

        inline FrameBuffer &getFrameBuffer() { return m_frameBuffer; }

        inline std::shared_ptr<Renderer> getRenderer() { return m_renderer; }
        inline void setRenderer(std::shared_ptr<Renderer> &renderer) { m_renderer = renderer; }

        void render(Scene &scene);
    };

} // namespace rt

#endif // RENDER_DISPATCHER_HPP
