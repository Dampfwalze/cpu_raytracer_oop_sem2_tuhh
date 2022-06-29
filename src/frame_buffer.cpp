#include <frame_buffer.h>

namespace rt
{
    FrameBuffer::FrameBuffer(size_t width, size_t height)
        : m_buffer(new Pixel[width * height]), m_size(width, height) {}

    FrameBuffer::FrameBuffer()
        : m_buffer(nullptr), m_size(0) {}

    FrameBuffer::~FrameBuffer()
    {
        delete[] m_buffer;
    }

    size_t FrameBuffer::getWidth() const { return m_size.width; }
    size_t FrameBuffer::getHeight() const { return m_size.height; }

    math::Vec2<size_t> FrameBuffer::getSize() const
    {
        return m_size;
    }

    void FrameBuffer::resize(math::Vec2<size_t> size)
    {
        delete[] m_buffer;
        m_buffer = new Pixel[size.width * size.height];
        m_size = size;
    }

    void FrameBuffer::clear(const Pixel &color)
    {
        for (size_t i = 0; i < m_size.width * m_size.height; i++)
            (*this)[i] = color;
    }

    Pixel &FrameBuffer::operator[](size_t index) const { return m_buffer[index]; }

    Pixel &FrameBuffer::at(size_t x, size_t y) const { return m_buffer[y * m_size.width + x]; }
}