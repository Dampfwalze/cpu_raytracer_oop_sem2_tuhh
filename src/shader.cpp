#include <shader.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <glad/glad.h>
#include <gl_error.h>

namespace rt
{
    static std::string readFile(const char *path)
    {
        std::ifstream fileStream(path);
        std::stringstream ss;
        std::string line;
        while (std::getline(fileStream, line))
            ss << line << "\n";
        return ss.str();
    }

    static const char *getShaderTypeString(GLenum type)
    {
        switch (type)
        {
        case GL_FRAGMENT_SHADER:
            return "fragment shader";
        case GL_VERTEX_SHADER:
            return "fragment shader";
        default:
            return "shader";
        }
    }

    static GLuint compileShader(std::string src, GLenum type)
    {
        GLCALL(GLuint shader = glCreateShader(type));
        const char *srcChar = src.c_str();
        GLCALL(glShaderSource(shader, 1, &srcChar, nullptr));
        GLCALL(glCompileShader(shader));

        int result;
        GLCALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &result));
        if (!result)
        {
            int length;
            GLCALL(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length));
            char *message = (char *)alloca(length * sizeof(char));
            GLCALL(glGetShaderInfoLog(shader, length, &length, message));

            std::cout << "Error: Failed to compile " << getShaderTypeString(type) << ":\n"
                      << message << "\n";

            GLCALL(glDeleteShader(shader));
            return 0;
        }
        return shader;
    }

    Shader::Shader(std::string vertexShader, std::string fragmentShader)
    {
        GLCALL(m_id = glCreateProgram());

        GLuint vs = compileShader(vertexShader, GL_VERTEX_SHADER);
        GLuint fs = compileShader(fragmentShader, GL_FRAGMENT_SHADER);

        if (vs != 0)
        {
            GLCALL(glAttachShader(m_id, vs));
        }
        if (fs != 0)
        {
            GLCALL(glAttachShader(m_id, fs));
        }

        GLCALL(glLinkProgram(m_id));
        GLCALL(glValidateProgram(m_id));

        GLCALL(glDeleteShader(vs));
        GLCALL(glDeleteShader(fs));
    }

    Shader::~Shader()
    {
        GLCALL(glDeleteProgram(m_id));
    }

    GLuint Shader::getID()
    {
        return m_id;
    }

    void Shader::bind()
    {
        GLCALL(glUseProgram(m_id));
    }

    void Shader::unBind()
    {
        GLCALL(glUseProgram(0));
    }

    void Shader::setUniform(int loc, int value)
    {
        bind();
        glUniform1i(loc, value);
    }

    Shader Shader::fromFile(const char *vertexShaderPath, const char *fragmentShaderPath)
    {
        return Shader(readFile(vertexShaderPath), readFile(fragmentShaderPath));
    }
} // namespace rt
