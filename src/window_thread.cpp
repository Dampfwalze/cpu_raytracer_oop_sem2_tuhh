#include <glad/glad.h>

#include <window_thread.h>

#include <application.h>
#include <render_dispatcher.h>
#include <frame_buffer.h>
#include <shader.h>
#include <gl_error.h>

#include <imgui.h>

#include <iostream>
#include <array>

namespace rt
{
    WindowThread::WindowThread(Application &application)
        : m_application(application), std::thread(&WindowThread::run, this) {}

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
        if (buffer.getSize() == math::u64vec2(0))
            return;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, (int)buffer.getWidth(), (int)buffer.getHeight(), 0, GL_RGB, GL_FLOAT, &buffer[0]);
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

        static RenderParams renderParams = m_application.getRenderDispatcher().renderParams;

        window.beginDraw();

        // glDebugMessageCallback(errorCallback, nullptr);

        // GLuint vao;
        // GLCALL(glGenVertexArrays(1, &vao));
        // GLCALL(glBindVertexArray(vao));

        // Create OpenGL texture buffer
        GLuint texture;
        GLCALL(glGenTextures(1, &texture));
        GLCALL(glBindTexture(GL_TEXTURE_2D, texture));

        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));

        // std::array<float, 3> vertecies[] = {
        //     {1.0, 1.0, 0.0},
        //     {1.0, -1.0, 0.0},
        //     {-1.0, -1.0, 0.0},
        //     {1.0, 1.0, 0.0},
        //     {-1.0, -1.0, 0.0},
        //     {-1.0, 1.0, 0.0},
        // };

        // GLuint quadBuffer;
        // GLCALL(glGenBuffers(1, &quadBuffer));

        // GLCALL(glBindBuffer(GL_ARRAY_BUFFER, quadBuffer));
        // GLCALL(glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), vertecies, GL_STATIC_DRAW));

        // GLCALL(glEnableVertexAttribArray(0));
        // GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr));

        // Shader shader = Shader::fromFile("resource/shader/shader.vert", "resource/shader/shader.frag");

        while (true)
        {
            if (window.shouldClose())
                exit(0);

            // handle termination
            {
                std::unique_lock<std::mutex> lk(m_termination_mutex);
                m_termination_cv.wait_for(lk, std::chrono::milliseconds(16), [this]
                                          { return m_terminate; });
                if (m_terminate)
                    break;
            }

            // GLCALL(glClear(GL_COLOR_BUFFER_BIT));

            // GLCALL(glActiveTexture(GL_TEXTURE0));
            // GLCALL(glBindTexture(GL_TEXTURE_2D, texture));
            // GLCALL(glBindBuffer(GL_ARRAY_BUFFER, quadBuffer));

            // shader.bind();
            // shader.setUniform(0, 0);
            //
            // GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));

            // Copy data from framebuffer to OpenGL texture on the GPU
            copyBuffer(m_application.getRenderDispatcher().getFrameBuffer());

            window.beginGUI();

            window.renderDockSpace();

            ImGui::ShowDemoWindow(nullptr);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            ImGui::Begin("Viewport");
            ImGui::PopStyleVar();
            ImGui::Image(reinterpret_cast<ImTextureID>((size_t)texture), ImVec2{(float)m_application.getRenderDispatcher().getFrameBuffer().getWidth(), (float)m_application.getRenderDispatcher().getFrameBuffer().getHeight()});
            ImGui::End();

            ImGui::Begin("Control");
            if (ImGui::Button("Render"))
            {
                m_application.getRenderDispatcher().renderParams = renderParams;
                m_application << Application::Events::Render();
            }

            static int tileSize = (int)renderParams.tileSize.x;
            ImGui::SliderInt("Tile size", &tileSize, 0, 128);
            renderParams.tileSize = {(size_t)tileSize, (size_t)tileSize};

            if (ImGui::TreeNode("Renderers"))
            {
                for (auto &&i : m_application.getRenderers())
                {
                    if (ImGui::Selectable(i.first.c_str(), m_application.getRenderDispatcher().getRenderer() == i.second))
                        m_application.getRenderDispatcher().setRenderer(i.second);
                }

                ImGui::TreePop();
            }

            ImGui::End();

            window.endGUI();

            window.swapBuffers();

            windowing.pollEvents();
        }
    }

} // namespace rt
