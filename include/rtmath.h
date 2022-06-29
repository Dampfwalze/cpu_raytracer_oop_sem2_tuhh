#ifndef RTMATH_HPP
#define RTMATH_HPP

#include <iostream>
#include <algorithm>

namespace rt
{
    namespace math
    {
        template <typename T>
        struct Vec2
        {
            union
            {
                struct
                {
                    T x, y;
                };
                struct
                {
                    T width, height;
                };
            };

            Vec2(T x, T y) : x(x), y(y) {}
            Vec2(T v) : x(v), y(v) {}
            Vec2() : x(), y() {}

            Vec2<T> min(T _x, T _y) { return Vec2(std::min(x, _x), std::min(y, _y)); }
            Vec2<T> max(T _x, T _y) { return Vec2(std::max(x, _x), std::max(y, _y)); }

            void operator+=(Vec2<T> other) { x += other.x, y += other.y; }
            Vec2<T> operator+(Vec2<T> other) { return {x + other.x, y + other.y}; }

            void operator-=(Vec2<T> other) { x -= other.x, y -= other.y; }
            Vec2<T> operator-(Vec2<T> other) { return {x - other.x, y - other.y}; }

            bool operator==(Vec2<T> other) { return x == other.x && y == other.y; }
        };

        template <typename T>
        struct Rect
        {
            Vec2<T> start;
            Vec2<T> size;

            Rect(Vec2<T> start, Vec2<T> size) : start(start), size(size) {}
            Rect() : start(), size() {}

            inline Vec2<T> getEnd() { return start + size; }

            Rect<T> min(T _x, T _y) { return Rect(start.min(_x, _y), getEnd().min(_x, _y) - start); }
            Rect<T> max(T _x, T _y) { return Rect(start.max(_x, _y), getEnd().max(_x, _y) - start); }
            inline Rect<T> min(Vec2<T> v) { return min(v.x, v.y); }
            inline Rect<T> max(Vec2<T> v) { return max(v.x, v.y); }
        };

        template <typename T>
        std::ostream &operator<<(std::ostream &stream, Rect<T> &rect)
        {
            stream << "Rect { x=" << rect.start.x << " y=" << rect.start.y << " w=" << rect.size.width << " h=" << rect.size.height << " }";
            return stream;
        }

    } // namespace math

} // namespace rt

#endif // RTMATH_HPP