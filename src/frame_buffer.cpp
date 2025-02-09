#include <frame_buffer.h>

namespace rt
{

    FrameBuffer::FrameBuffer(size_t width, size_t height)
        : m_buffer(new m::Pixel<float>[width * height]), m_size(width, height) {}

    FrameBuffer::FrameBuffer()
        : m_buffer(nullptr), m_size(0) {}

    FrameBuffer::~FrameBuffer()
    {
        delete[] m_buffer;
    }

    size_t FrameBuffer::getWidth() const { return m_size.x; }
    size_t FrameBuffer::getHeight() const { return m_size.y; }

    m::u64vec2 FrameBuffer::getSize() const
    {
        return m_size;
    }

    void FrameBuffer::resize(m::u64vec2 size)
    {
        delete[] m_buffer;
        m_buffer = new m::Pixel<float>[size.x * size.y];
        m_size = size;
    }

    void FrameBuffer::clear(const m::Pixel<float> &color)
    {
        for (size_t i = 0; i < m_size.x * m_size.y; i++)
            (*this)[i] = color;
    }

    m::Pixel<float> &FrameBuffer::operator[](size_t index) const { return m_buffer[index]; }

    m::Pixel<float> &FrameBuffer::at(size_t x, size_t y) const { return m_buffer[y * m_size.x + x]; }
    m::Pixel<float> &FrameBuffer::at(m::vec2<size_t> c) const { return m_buffer[c.y * m_size.x + c.x]; }
}