#include <glad/glad.h>

#include <window_thread.h>

#include <application.h>
#include <frame_buffer.h>
#include <gl_error.h>
#include <profiler.h>
#include <shader.h>

#include <rt_imgui.h>

#include <array>
#include <iostream>

#include <file_dialog.h>

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

    inline void calcPitchYaw(const m::dquat &quat, double *pitch, double *yaw)
    {
        auto rot = m::rotate(quat, m::dvec3(0, 0, 1));
        *yaw = m::atan(rot.x / rot.z);
        if (rot.z == 0)
            *yaw = (rot.x > 0) ? m::pi<double>() / 2 : -m::pi<double>() / 2;
        if (rot.z < 0)
            *yaw += m::pi<double>();
        *pitch = m::asin(-rot.y);
    }

    void WindowThread::cameraController()
    {
        static double pitch, yaw;

        auto &IO = ImGui::GetIO();
        auto  delta = IO.MouseDelta;

        // if (delta.x == 0 && delta.y == 0)
        //     return;

        auto &camera = m_application.scene->camera;
        auto &q = camera.transform.rotation;
        // auto a = glm::eulerAngles(q);

        if (IO.MouseClicked[ImGuiMouseButton_Right])
        {
            calcPitchYaw(q, &pitch, &yaw);
        }

        pitch = glm::min(m::pi<double>() / 2, glm::max(-m::pi<double>() / 2, pitch - delta.y * 0.0025));
        yaw -= delta.x * 0.0025;

        q = m::rotate(m::dquat(1, 0, 0, 0), yaw, m::dvec3(0, 1, 0));
        q = m::rotate(q, pitch, m::dvec3(1, 0, 0));

        if (ImGui::IsKeyDown(ImGuiKey_W))
        {
            auto d = m::rotate(q, m::dvec3(0, 0, -1));
            camera.transform.position += d * 0.03;
        }
        else if (ImGui::IsKeyDown(ImGuiKey_S))
        {
            auto d = m::rotate(q, m::dvec3(0, 0, 1));
            camera.transform.position += d * 0.03;
        }

        if (ImGui::IsKeyDown(ImGuiKey_A))
        {
            auto d = m::rotate(q, m::dvec3(-1, 0, 0));
            camera.transform.position += d * 0.03;
        }
        if (ImGui::IsKeyDown(ImGuiKey_D))
        {
            auto d = m::rotate(q, m::dvec3(1, 0, 0));
            camera.transform.position += d * 0.03;
        }

        if (ImGui::IsKeyDown(ImGuiKey_Q))
        {
            auto d = m::rotate(q, m::dvec3(0, -1, 0));
            camera.transform.position += d * 0.03;
        }
        if (ImGui::IsKeyDown(ImGuiKey_E))
        {
            auto d = m::rotate(q, m::dvec3(0, 1, 0));
            camera.transform.position += d * 0.03;
        }

        m_application
            << Application::Events::Render();
    }

    void WindowThread::render(m::u64vec2 size)
    {
        if (m_application.frameBuffer.getSize() != size && !m_application.renderThread.isRendering())
            m_application.frameBuffer.resize(size);
        m_application << Application::Events::Render();
    }

    void WindowThread::save()
    {
        if (m_application.savePath)
            m_application << Application::Events::SaveScene(*m_application.savePath);
        else
            saveAs();
    }

    void WindowThread::saveAs()
    {
        try
        {
            auto path = FileDialog::saveDialog("yaml", "yaml");
            if (path)
            {
                m_application.savePath = *path;
                m_application << Application::Events::SaveScene{*path};
            }
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }
    }

    void WindowThread::open()
    {
        try
        {
            auto path = FileDialog::fileDialog("yaml");
            if (path)
            {
                m_application.savePath = *path;
                m_application << Application::Events::LoadScene{*path};
            }
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }
    }

    void WindowThread::newScene()
    {
        try
        {
            m_application << Application::Events::LoadScene{std::nullopt};
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }
    }

    static void copyBuffer(const FrameBuffer &buffer, GLuint texture)
    {
        if (buffer.getSize() == math::u64vec2(0))
            return;
        GLCALL(glBindTexture(GL_TEXTURE_2D, texture));
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, (int)buffer.getWidth(), (int)buffer.getHeight(), 0, GL_RGB, GL_FLOAT, &buffer[0]);
    }

    void APIENTRY errorCallback(GLenum        source,
                                GLenum        type,
                                GLuint        id,
                                GLenum        severity,
                                GLsizei       length,
                                const GLchar *message,
                                const void   *userParam)
    {
        std::cout << "OpenGL Error: " << message << std::endl;
    }

    void WindowThread::run()
    {
        Windowing windowing;
        Window    window;

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

        GLuint previewTexture;
        GLCALL(glGenTextures(1, &previewTexture));
        GLCALL(glBindTexture(GL_TEXTURE_2D, previewTexture));

        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));

        // std::array<float, 3> vertices[] = {
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
        // GLCALL(glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), vertices, GL_STATIC_DRAW));

        // GLCALL(glEnableVertexAttribArray(0));
        // GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr));

        // Shader shader = Shader::fromFile("resource/shader/shader.vert", "resource/shader/shader.frag");

        size_t frame = 0;

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
            copyBuffer(m_application.frameBuffer, texture);

            window.beginGUI();

            window.renderDockSpace();

            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("New"))
                    {
                        newScene();
                    }
                    if (ImGui::MenuItem("Open" /*, "Ctrl+O"*/))
                    {
                        open();
                    }
                    if (ImGui::MenuItem("Save" /*, "Ctrl+S"*/))
                    {
                        save();
                    }
                    if (ImGui::MenuItem("Save As.."))
                    {
                        saveAs();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }

            ImGui::ShowDemoWindow(nullptr);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            ImGui::Begin("Viewport");
            ImGui::PopStyleVar();

            auto imageSize = m::Vec2<m::u64vec2>(ImGui::GetContentRegionAvail());
            {
                auto cursorPos = m::Vec2<m::u64vec2>(ImGui::GetCursorScreenPos());

                ImGui::Image(reinterpret_cast<ImTextureID>((size_t)texture), m::Vec2<ImVec2>(m_application.frameBuffer.getSize()), ImVec2(0, 1), ImVec2(1, 0));

                auto &logPixel = m_application.renderThread.renderParams.logPixel;

                if (ImGui::IsItemHovered() && ImGui::GetIO().MouseClicked[ImGuiMouseButton_Left])
                {
                    auto mousePos = m::Vec2<m::u64vec2>(ImGui::GetMousePos());
                    logPixel = {mousePos.x - cursorPos.x, imageSize.y + cursorPos.y - mousePos.y};
                }
                if (ImGui::IsItemHovered() && ImGui::GetIO().MouseDown[ImGuiMouseButton_Right])
                {
                    cameraController();
                }
                if (logPixel.has_value())
                {
                    m::fvec2 pos = {logPixel->x + cursorPos.x, imageSize.y + cursorPos.y - logPixel->y};
                    ImGui::GetWindowDrawList()->AddLine({pos.x - 10, pos.y}, {pos.x + 10, pos.y}, 0xffffffff);
                    ImGui::GetWindowDrawList()->AddLine({pos.x, pos.y - 10}, {pos.x, pos.y + 10}, 0xffffffff);
                }
            }

            ImGui::End();

            ImGui::Begin("Control");
            if (ImGui::CollapsingHeader("Control", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::BeginDisabled(m_application.renderThread.isRendering());
                if (ImGui::Button("Render"))
                {
                    Profiling::profiler.enabled = false;
                    render(imageSize);
                }
                ImGui::SameLine();
                if (ImGui::Button("Profile"))
                {
                    Profiling::profiler.enabled = true;
                    render(imageSize);
                }
                ImGui::EndDisabled();

                // Output section

                ImGui::SeparatorText("Output");

                // Size
                m::uvec2 __size = m_application.outputSize;
                rtImGui::Drag<m::uvec2, size_t>("Size", __size, 1, 1, std::nullopt, "%d");
                m_application.outputSize = __size;

                // File browser
                auto buffer = m_application.outputPath ? m_application.outputPath->string() : "";
                ImGui::InputText("##", buffer.data(), buffer.size(), ImGuiInputTextFlags_ReadOnly);
                ImGui::SameLine();
                if (ImGui::Button("Browse"))
                {
                    auto path = FileDialog::saveDialog("png,jpg,bmp", "jpg");
                    if (path.has_value())
                    {
                        std::cout << "Selected file: " << *path << std::endl;
                        m_application.outputPath = *path;
                    }
                }

                // Render button
                ImGui::BeginDisabled(!m_application.outputPath || m_application.renderThread.isRendering());
                if (ImGui::Button("Render output") && m_application.outputPath)
                {
                    m_application << Application::Events::RenderOutput(*m_application.outputPath, m_application.outputSize);
                }
                ImGui::EndDisabled();
            }

            if (ImGui::CollapsingHeader("Parameters", ImGuiTreeNodeFlags_DefaultOpen))
            {
                auto &renderParams = m_application.renderThread.renderParams;
                bool  changed = false;

                auto  &tileSize = renderParams.tileSize;
                size_t min = 1, max = 128;
                ImGui::SliderScalar("Tile size", ImGuiDataType_U64, &tileSize.x, &min, &max);
                tileSize.y = tileSize.x;

                changed |= rtImGui::Drag("Mixing factor", renderParams.mixingFactor, 0.01f);

                changed |= ImGui::InputScalar("Recursion depth", ImGuiDataType_U32, &renderParams.recursionDepth, &((const int &)1));

                ImGui::SeparatorText("Tone mapping");

                if (ImGui::BeginCombo("Tone mapping algorithm", toneMappingAlgorithmToString(renderParams.toneMappingAlgorithm)))
                {
                    for (size_t i = 0; i < RenderParams::ToneMappingAlgorithm_COUNT; i++)
                    {
                        if (ImGui::Selectable(toneMappingAlgorithmToString((RenderParams::ToneMappingAlgorithm)i), renderParams.toneMappingAlgorithm == i))
                        {
                            renderParams.toneMappingAlgorithm = (RenderParams::ToneMappingAlgorithm)i;
                            changed = true;
                        }
                    }
                    ImGui::EndCombo();
                }
                changed |= rtImGui::Drag<float, float>("Exposure", renderParams.exposure, 0.01f, 0.0f);
                changed |= rtImGui::Drag<float, float>("Gamma", renderParams.gamma, 0.01f, 0.0f);
                changed |= rtImGui::Drag<float, float>("Scale", renderParams.scale, 0.01f, 0.0f);

                if (changed)
                    render(imageSize);
            }

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
                    ImGui::TextWrapped("%s", m_application.renderThread.renderLog.c_str());
                    ImGui::EndChild();
                    ImGui::TreePop();
                }
            }

            ImGui::End();

            ImGui::Begin("Scene Inspector");

            ImGui::PushItemWidth(ImGui::GetFontSize() * -8);

            if (m_application.scene->onInspectorGUI())
                render(imageSize);

            ImGui::End();

            ImGui::Begin("Resources");

            for (auto &&resource : m_application.resources)
            {
                auto name = resource.getPath().filename().string();
                ImGui::Selectable(name.c_str());
                if (ImGui::BeginDragDropSource())
                {
                    auto payload = resource.getPtr();
                    ImGui::SetDragDropPayload("Resource", &payload, sizeof(payload));

                    ImGui::Text("%s", name.c_str());

                    ImGui::EndDragDropSource();
                }

                if (ImGui::IsItemHovered())
                {
                    float maxWidth = ImGui::GetFontSize() * 35.0f;
                    ImGui::BeginTooltip();
                    ImGui::PushTextWrapPos(maxWidth);
                    ImGui::TextUnformatted(resource.getPath().string().c_str());
                    if (resource && dynamic_cast<Resources::TextureResource *>((Resource *)resource))
                    {
                        auto  &img = dynamic_cast<Resources::TextureResource &>(*(Resource *)resource);
                        auto   size = img.getSize();
                        GLenum formatMap[] = {
                            GL_RED,
                            0,
                            GL_RGB,
                            GL_RGBA,
                        };
                        auto col = img[0];
                        ImGui::Text("HDR: %s", img.isHDR() ? "Yes" : "No");
                        ImGui::Text("Size: (%u, %u)", size.x, size.y);
                        GLCALL(glBindTexture(GL_TEXTURE_2D, previewTexture));
                        GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, (int)size.x, (int)size.y, 0, formatMap[img.getChannels() - 1], img.isHDR() ? GL_FLOAT : GL_UNSIGNED_BYTE, img.get()));

                        m::dvec2 prevSize = size;
                        if (size.x > (unsigned)maxWidth)
                        {
                            prevSize *= maxWidth / (double)size.x;
                        }
                        ImGui::Image(reinterpret_cast<ImTextureID>((size_t)previewTexture), m::Vec2<ImVec2>(prevSize));
                    }

                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }
                if (resource.hasException())
                {
                    ImGui::SameLine();
                    ImGui::Text("(!)");
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                        try
                        {
                            std::rethrow_exception(resource.getException());
                        }
                        catch (const std::exception &e)
                        {
                            ImGui::TextUnformatted(e.what());
                        }
                        ImGui::PopTextWrapPos();
                        ImGui::EndTooltip();
                    }
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

            if (frame == 2)
            {
                render(imageSize);
            }
            if (frame < 3)
            {
                frame++;
            }
            else if (m_application.frameBuffer.getSize() != imageSize && !m_application.renderThread.isRendering() //
                     && imageSize.x * imageSize.y > 0 && imageSize.x * imageSize.y < std::numeric_limits<int>::max() / 2)
            {
                m_application.frameBuffer.resize(imageSize);
                m_application << Application::Events::Render();
            }

            window.endGUI();

            window.swapBuffers();

            windowing.pollEvents();
        }
    }

} // namespace rt
