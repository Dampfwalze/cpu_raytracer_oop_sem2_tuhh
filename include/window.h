#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <GLFW/glfw3.h>
#include <imgui.h>

#ifdef _DEBUG
#define GLFW_CHECK_ERROR(msg) rt::glfwCheckError(__FILE__, __LINE__, __func__, msg)
#else
#define GLFW_CHECK_ERROR(msg) rt::glfwCheckError(NULL, 0, NULL, msg)
#endif

namespace rt
{
    void glfwCheckError(const char *fileName, size_t line, const char *function, const char *message);

    class Windowing
    {
    private:
    public:
        Windowing();
        ~Windowing();

        void pollEvents();
    };

    class Window
    {
    private:
        GLFWwindow   *m_window;
        ImGuiContext *m_imgui_context;

    public:
        Window();
        ~Window();

        void beginDraw();
        void beginGUI();
        void endGUI();

        bool shouldClose() const;
        void swapBuffers();

        int getWidth() const;
        int getHeight() const;

        void renderDockSpace() const;
    };

} // namespace rt

#endif // WINDOW_HPP