#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <frame_buffer.h>
#include <thread_pool.h>
#include <rtmath.h>
#include <render_params.h>

namespace rt
{

    class Renderer
    {
    private:
        class RenderTask
        {
        private:
            math::Rect<size_t> m_rect;
            FrameBuffer &m_frameBuffer;

        public:
            RenderTask(math::Rect<size_t> rect, FrameBuffer &frameBuffer);
            ~RenderTask();

            void run();
        };

    private:
        FrameBuffer m_frameBuffer;

        ThreadPool<RenderTask> m_threadPool;

    public:
        RenderParams renderParams;

    public:
        Renderer(const RenderParams &renderParams);
        ~Renderer();

        inline FrameBuffer &getFrameBuffer() { return m_frameBuffer; }

        void render();
    };

} // namespace rt

#endif // RENDERER_HPP
