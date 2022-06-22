#ifndef GL_ERROR_HPP
#define GL_ERROR_HPP

#define GLCALL(x)                                    \
    clearGLErrors(#x, __func__, __FILE__, __LINE__); \
    x;                                               \
    catchGLErrors(#x, __func__, __FILE__, __LINE__)

namespace rt
{
    void clearGLErrors(const char *statement, const char *function, const char *file, const int line);
    void catchGLErrors(const char *statement, const char *function, const char *file, const int line);
} // namespace rt

#endif // GL_ERROR_HPP