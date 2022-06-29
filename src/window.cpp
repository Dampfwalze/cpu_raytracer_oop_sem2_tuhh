#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <window.h>

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <string>

namespace rt
{
    static inline void ReplaceAll(std::string &str, const std::string &from, const std::string &to)
    {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos)
        {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
        }
    }

    void glfwCheckError(const char *fileName, size_t line, const char *function, const char *message)
    {
        const char *description;
        int value = glfwGetError(&description);
        if (value != GLFW_NO_ERROR)
        {
#ifdef _DEBUG
            std::string m(message);
            ReplaceAll(m, "\n", "\n         ");
            std::string d(description);
            ReplaceAll(d, "\n", "\n             ");
            std::stringstream ss;
            ss << "GLFW error in file " << fileName << ":" << line
               << " in function " << function
               << "\nMessage: " << m
               << "\nDescription: " << d;
            throw std::runtime_error(ss.str());
#else
            std::string m(message);
            ReplaceAll(m, "\n", "\n         ");
            std::string d(description);
            ReplaceAll(d, "\n", "\n             ");
            std::stringstream ss;
            ss << "GLFW error:"
               << "\nMessage: " << m
               << "\nDescription: " << d;
            throw std::runtime_error(ss.str());
#endif
        }
    }

    Windowing::Windowing()
    {
        glfwInit();
        GLFW_CHECK_ERROR("GLFW initialization failed!");
        std::cout << "GLFW successfully initialized!" << std::endl;
    }

    Windowing::~Windowing()
    {
        glfwTerminate();
    }

    void Windowing::pollEvents()
    {
        glfwPollEvents();
    }

    Window::Window()
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        m_window = glfwCreateWindow(640, 480, "Ray Tracer", NULL, NULL);
        GLFW_CHECK_ERROR("Window creation failed!");

        beginDraw();
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
        // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        // ImGui::StyleColorsClassic();

        ImGui_ImplGlfw_InitForOpenGL(m_window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        std::cout << "OpenGL version: " << (char *)glGetString(GL_VERSION) << std::endl;
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_window);
    }

    void Window::beginDraw()
    {
        glfwMakeContextCurrent(m_window);
        GLFW_CHECK_ERROR("Failed to make context current");
    }

    void Window::beginGUI()
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void Window::endGUI()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    bool Window::shouldClose() const
    {
        return glfwWindowShouldClose(m_window);
    }

    void Window::swapBuffers()
    {
        glfwSwapBuffers(m_window);
        GLFW_CHECK_ERROR("Failed to swap buffers");
    }

    int Window::getWidth() const
    {
        int v;
        glfwGetWindowSize(m_window, &v, NULL);
        return v;
    }

    int Window::getHeight() const
    {
        int v;
        glfwGetWindowSize(m_window, NULL, &v);
        return v;
    }
}