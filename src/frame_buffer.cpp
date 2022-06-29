#include <frame_buffer.h>

namespace rt
{
    FrameBuffer::FrameBuffer(size_t width, size_t height)
        : m_buffer(new Pixel[width * height]), m_width(width), m_height(height) {}

    FrameBuffer::FrameBuffer()
        : m_buffer(nullptr), m_width(0), m_height(0) {}

    FrameBuffer::~FrameBuffer()
    {
        delete[] m_buffer;
    }

    size_t FrameBuffer::getWidth() const { return m_width; }
    size_t FrameBuffer::getHeight() const { return m_height; }

    Pixel &FrameBuffer::operator[](size_t index) const { return m_buffer[index]; }

    Pixel &FrameBuffer::at(size_t x, size_t y) const { return m_buffer[y * m_width + x]; }
}