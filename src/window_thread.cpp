#include <glad/glad.h>

#include <window_thread.h>

#include <application.h>
#include <frame_buffer.h>
#include <shader.h>
#include <gl_error.h>
#include <profiler.h>

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
            copyBuffer(m_application.frameBuffer);

            window.beginGUI();

            window.renderDockSpace();

            ImGui::ShowDemoWindow(nullptr);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            ImGui::Begin("Viewport");
            ImGui::PopStyleVar();

            auto imageSize = m::Vec2<m::u64vec2>(ImGui::GetContentRegionAvail());
            {
                auto cursorPos = m::Vec2<m::u64vec2>(ImGui::GetCursorScreenPos());

                ImGui::Image(reinterpret_cast<ImTextureID>((size_t)texture), m::Vec2<ImVec2>(m_application.frameBuffer.getSize()));

                auto &logPixel = m_application.renderThread.renderParams.logPixel;
                if (ImGui::IsItemHovered() && ImGui::GetMouseClickedCount(ImGuiMouseButton_Left) > 0)
                {
                    auto mousePos = m::Vec2<m::u64vec2>(ImGui::GetMousePos());
                    logPixel = mousePos - cursorPos;
                }
                if (logPixel.has_value())
                {
                    m::fvec2 pos = logPixel.value() + cursorPos;
                    ImGui::GetWindowDrawList()->AddLine({pos.x - 10, pos.y}, {pos.x + 10, pos.y}, 0xffffffff);
                    ImGui::GetWindowDrawList()->AddLine({pos.x, pos.y - 10}, {pos.x, pos.y + 10}, 0xffffffff);
                }
            }

            ImGui::End();

            ImGui::Begin("Control");
            ImGui::BeginDisabled(m_application.renderThread.isRendering());
            if (ImGui::Button("Render"))
            {
                Profiling::profiler.enabled = false;
                if (m_application.frameBuffer.getSize() != imageSize)
                    m_application.frameBuffer.resize(imageSize);
                m_application << Application::Events::Render();
            }
            ImGui::SameLine();
            if (ImGui::Button("Profile"))
            {
                Profiling::profiler.enabled = true;
                if (m_application.frameBuffer.getSize() != imageSize)
                    m_application.frameBuffer.resize(imageSize);
                m_application << Application::Events::Render();
            }
            ImGui::EndDisabled();

            auto &tileSize = m_application.renderThread.renderParams.tileSize;
            size_t min = 1, max = 128;
            ImGui::SliderScalar("Tile size", ImGuiDataType_U64, &tileSize.x, &min, &max);
            tileSize.y = tileSize.x;

            if (ImGui::TreeNode("Renderers"))
            {
                for (auto &&i : m_application.renderers)
                {
                    if (ImGui::Selectable(i.first.c_str(), m_application.renderThread.getRenderer() == i.second.get()) && !m_application.renderThread.isRendering())
                        m_application.renderThread.setRenderer(i.second.get());
                }

                ImGui::TreePop();
            }

            if (m_application.renderThread.renderLog != "")
            {
                static bool open = true;
                ImGui::SetNextItemOpen(open);
                if (open = ImGui::TreeNode("Render log"))
                {
                    auto padding = ImGui::GetStyle().FramePadding;
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, padding);
                    ImGui::BeginChild("Render_Log", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysUseWindowPadding);
                    ImGui::PopStyleVar();
                    ImGui::TextWrapped(m_application.renderThread.renderLog.c_str());
                    ImGui::EndChild();
                    ImGui::TreePop();
                }
            }

            ImGui::End();

            ImGui::Begin("Scene Inspector");

            if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (rtImGui::Drag<Transform, double>("Transform", m_application.scene.camera.transform, 0.01, std::nullopt, std::nullopt, "%.3f"))
                    m_application << Application::Events::Render();
            }
            if (ImGui::CollapsingHeader("Objects", ImGuiTreeNodeFlags_DefaultOpen))
            {
                auto &objects = m_application.scene.objects;
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

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            ImGui::Begin("Profiler");
            {
                ImGui::PopStyleVar();

                static std::unique_ptr<Profiling::FrameProfile> profile;
                if (!m_application.renderThread.isRendering() && Profiling::profiler.getProfile() != nullptr)
                    profile = Profiling::profiler.exchangeProfile();

                if (profile)
                    rtImGui::drawProfiler("Profiler", *profile, ImGui::GetContentRegionAvail().y);
            }
            ImGui::End();

            window.endGUI();

            window.swapBuffers();

            windowing.pollEvents();
        }
    }

} // namespace rt
