#include <glad/glad.h>

#include <window_thread.h>

#include <application.h>
#include <render_dispatcher.h>
#include <frame_buffer.h>
#include <shader.h>
#include <gl_error.h>

#include <rt_imgui.h>

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
                m_application << Application::Events::CloseApplication();

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
            auto& frameBuffer = m_application.getRenderDispatcher().getFrameBuffer();
            copyBuffer(frameBuffer);

            window.beginGUI();

            window.renderDockSpace();

            ImGui::ShowDemoWindow(nullptr);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            ImGui::Begin("Viewport");
            ImGui::PopStyleVar();
            {
                int mouseClickedCount = ImGui::GetMouseClickedCount(ImGuiMouseButton_Left);

                auto cursorPos = ImGui::GetCursorScreenPos();
                auto mousePos = ImGui::GetMousePos();
                if (mousePos.x >= cursorPos.x && mousePos.y >= cursorPos.y &&
                    mousePos.x < cursorPos.x + frameBuffer.getWidth() && mousePos.y < cursorPos.y + frameBuffer.getHeight() &&
                    mouseClickedCount > 0)
                {
                    renderParams.logPixel = { mousePos.x - cursorPos.x, mousePos.y - cursorPos.y };
                }
                if (renderParams.logPixel.has_value()) {
                    m::fvec2 pos = renderParams.logPixel.value() + m::uvec2(cursorPos.x, cursorPos.y);
                    ImGui::GetForegroundDrawList()->AddLine({ pos.x - 10, pos.y }, { pos.x + 10, pos.y }, 0xffffffff);
                    ImGui::GetForegroundDrawList()->AddLine({ pos.x, pos.y - 10 }, { pos.x, pos.y + 10 }, 0xffffffff);
                }
            }
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

            if (m_application.getRenderDispatcher().renderLog != "") {
                static bool open = true;
                ImGui::SetNextItemOpen(open);
                if (open = ImGui::TreeNode("Render log")) {
                    std::lock_guard<std::mutex> lk(m_application.getRenderDispatcher().logMutex);
                    ImGui::TextWrapped(m_application.getRenderDispatcher().renderLog.c_str());
                    ImGui::TreePop();
                }
            }

            ImGui::End();

            ImGui::Begin("Scene Inspector");

            if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (rtImGui::Drag<Transform, double>("Transform", m_application.getScene().camera.transform, 0.01, std::nullopt, std::nullopt, "%.3f"))
                    m_application << Application::Events::Render();
            }
            if (ImGui::CollapsingHeader("Objects", ImGuiTreeNodeFlags_DefaultOpen))
            {
                auto &objects = m_application.getScene().objects;
                for (size_t i = 0; i < objects.size(); i++)
                {
                    ImGui::PushID(i);
                    if (ImGui::TreeNode(objects[i]->className()))
                    {
                        if (objects[i]->onInspectorGUI())
                            m_application << Application::Events::Render();
                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                }
            }

            ImGui::End();

            window.endGUI();

            window.swapBuffers();

            windowing.pollEvents();
        }
    }

} // namespace rt
