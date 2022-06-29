#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <frame_buffer.h>
#include <thread_pool.h>

namespace rt
{

    class Renderer
    {
    private:
        class RenderTask
        {
        private:
        public:
            RenderTask();
            ~RenderTask();

            void run();
        };

    private:
        FrameBuffer m_frameBuffer;

        ThreadPool<RenderTask> m_threadPool;

    public:
        Renderer();
        ~Renderer();
    };

} // namespace rt

#endif // RENDERER_HPP
