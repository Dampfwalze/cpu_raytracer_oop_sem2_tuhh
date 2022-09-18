#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>
#include <string>

namespace rt
{
    class Shader
    {
    private:
        GLuint m_id;

    public:
        Shader(std::string vertexShader, std::string fragmentShader);
        ~Shader();

        GLuint getID();

        void bind();
        void unBind();

        void setUniform(int loc, int value);

        static Shader fromFile(const char *vertexShaderPath, const char *fragmentShaderPath);
    };
} // namespace rt

#endif // SHADER_HPP