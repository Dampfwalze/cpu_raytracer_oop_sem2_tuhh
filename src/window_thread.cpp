#include <glad/glad.h>

#include <window_thread.h>

#include <frame_buffer.h>
#include <shader.h>
#include <gl_error.h>

#include <imgui.h>

#include <iostream>
#include <array>

namespace rt
{
    WindowThread::WindowThread()
        : std::thread(&WindowThread::run, this) {}

    WindowThread::~WindowThread()
    {
        terminate();
        join();
    }

    void WindowThread::terminate()
    {
        std::lock_guard<std::mutex> lk(m_termination_mutex);
        m_terminate = true;
        m_termination_cv.notify_all();
    }

    static void copyBuffer(const FrameBuffer &buffer)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, buffer.getWidth(), buffer.getHeight(), 0, GL_RGB, GL_FLOAT, &buffer[0]);
    }

    void APIENTRY errorCallback(GLenum source,
                                GLenum type,
                                GLuint id,
                                GLenum severity,
                                GLsizei length,
                                const GLchar *message,
                                const void *userParam)
    {
        std::cout << "OpenGL Error: " << message << std::endl;
    }

    void WindowThread::run()
    {
        Windowing windowing;
        Window window;

        window.beginDraw();

        // glDebugMessageCallback(errorCallback, nullptr);

        GLuint vao;
        GLCALL(glGenVertexArrays(1, &vao));
        GLCALL(glBindVertexArray(vao));

        GLuint texture;
        GLCALL(glGenTextures(1, &texture));
        GLCALL(glBindTexture(GL_TEXTURE_2D, texture));

        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));

        FrameBuffer frameBuffer(window.getWidth(), window.getHeight());
        for (size_t y = 0; y < frameBuffer.getHeight(); y++)
        {
            for (size_t x = 0; x < frameBuffer.getWidth(); x++)
            {
                if (((y + x) / 20) % 3 == 0)
                    frameBuffer[x + y * frameBuffer.getWidth()] = {0, 1, 0};
                else if (((y + x) / 20) % 3 == 1)
                    frameBuffer[x + y * frameBuffer.getWidth()] = {1, 0, 0};
                else
                    frameBuffer[x + y * frameBuffer.getWidth()] = {0, 0, 1};
            }
        }

        copyBuffer(frameBuffer);

        std::array<float, 3> vertecies[] = {
            {1.0, 1.0, 0.0},
            {1.0, -1.0, 0.0},
            {-1.0, -1.0, 0.0},
            {1.0, 1.0, 0.0},
            {-1.0, -1.0, 0.0},
            {-1.0, 1.0, 0.0},
        };

        GLuint quadBuffer;
        GLCALL(glGenBuffers(1, &quadBuffer));

        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, quadBuffer));
        GLCALL(glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), vertecies, GL_STATIC_DRAW));

        GLCALL(glEnableVertexAttribArray(0));
        GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr));

        Shader shader = Shader::fromFile("resource/shader/shader.vert", "resource/shader/shader.frag");

        while (true)
        {
            if (window.shouldClose())
                exit(0);

            // handle termination
            std::unique_lock<std::mutex> lk(m_termination_mutex);
            m_termination_cv.wait_for(lk, std::chrono::milliseconds(16), [this]
                                      { return m_terminate; });
            if (m_terminate)
                break;

            GLCALL(glClear(GL_COLOR_BUFFER_BIT));

            GLCALL(glActiveTexture(GL_TEXTURE0));
            GLCALL(glBindTexture(GL_TEXTURE_2D, texture));
            GLCALL(glBindBuffer(GL_ARRAY_BUFFER, quadBuffer));

            shader.bind();
            shader.setUniform(0, 0);

            GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));

            window.beginGUI();
            ImGui::ShowDemoWindow(nullptr);

            ImGui::Begin("Test");
            ImGui::Button("Testi");
            ImGui::End();

            window.endGUI();

            window.swapBuffers();

            windowing.pollEvents();
        }
    }

} // namespace rt
