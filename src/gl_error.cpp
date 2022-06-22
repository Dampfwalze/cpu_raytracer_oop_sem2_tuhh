#include <gl_error.h>
#include <glad/glad.h>
#include <iostream>

namespace rt
{
    void clearGLErrors(const char *statement, const char *function, const char *file, const int line)
    {
        while (GLenum error = glGetError())
        {
            std::cout << "Error: Uncought OpenGL error (" << error << ")\n"
                      << file << ":" << line << "\n\tat: " << statement << "\n\tat: " << function << "\n";
        }
    }

    void catchGLErrors(const char *statement, const char *function, const char *file, const int line)
    {
        while (GLenum error = glGetError())
        {
            std::cout << "\n"
                      << "Error: OpenGL error (" << error << "):\n"
                      << file << ":" << line << "\n\tat: " << statement << "\n\tat: " << function << "\n";
        }
    }
} // namespace rt
