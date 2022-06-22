#include <glad/glad.h>
#include <window.h>

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
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        m_window = glfwCreateWindow(640, 480, "Ray Tracer", NULL, NULL);
        GLFW_CHECK_ERROR("Window creation failed!");

        beginDraw();
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
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