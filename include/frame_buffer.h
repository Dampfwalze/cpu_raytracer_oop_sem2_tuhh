#ifndef FRAME_BUFFER_HPP
#define FRAME_BUFFER_HPP

#include <stdint.h>

namespace rt
{
    struct Pixel
    {
        float r, g, b;
    };

    class FrameBuffer
    {
    private:
        Pixel *m_buffer;
        size_t m_width, m_height;

    public:
        FrameBuffer(size_t width, size_t height);
        ~FrameBuffer();

        size_t getWidth() const;
        size_t getHeight() const;

        Pixel &operator[](size_t index) const;
        Pixel &at(size_t x, size_t y) const;
    };

} // namespace rt

#endif // FRAME_BUFFER_HPP