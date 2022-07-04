#ifndef FRAME_BUFFER_HPP
#define FRAME_BUFFER_HPP

#include <stdio.h>

#include <rtmath.h>

namespace rt
{
    namespace m = math;

    class FrameBuffer
    {
    private:
        m::Pixel<float> *m_buffer;
        m::u64vec2 m_size;

    public:
        FrameBuffer(size_t width, size_t height);
        FrameBuffer();
        ~FrameBuffer();

        size_t getWidth() const;
        size_t getHeight() const;
        m::u64vec2 getSize() const;

        inline void resize(size_t width, size_t height) { resize(m::u64vec2(width, height)); }
        void resize(m::u64vec2 size);

        void clear(const m::Pixel<float> &color = {0, 0, 0});

        m::Pixel<float> &operator[](size_t index) const;
        m::Pixel<float> &at(size_t x, size_t y) const;
    };

} // namespace rt

#endif // FRAME_BUFFER_HPP