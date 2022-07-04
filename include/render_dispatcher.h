#ifndef RENDER_DISPATCHER_HPP
#define RENDER_DISPATCHER_HPP

#include <frame_buffer.h>
#include <thread_pool.h>
#include <rtmath.h>
#include <render_params.h>
#include <scene.h>

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

        public:
            RenderTask(math::Rect<size_t> rect, FrameBuffer &frameBuffer, Scene &scene);
            ~RenderTask();

            void run();
        };

    private:
        FrameBuffer m_frameBuffer;

        ThreadPool<RenderTask> m_threadPool;

    public:
        RenderParams renderParams;

    public:
        RenderDispatcher(const RenderParams &renderParams);
        ~RenderDispatcher();

        inline FrameBuffer &getFrameBuffer() { return m_frameBuffer; }

        void render(Scene &scene);
    };

} // namespace rt

#endif // RENDER_DISPATCHER_HPP
