#ifndef RTMATH_HPP
#define RTMATH_HPP

#include <iostream>
#include <algorithm>
#include <glm/glm.hpp>

namespace rt
{
    namespace math
    {

        using namespace glm;

        template <typename T>
        using vec1 = vec<1, T, defaultp>;
        template <typename T>
        using vec2 = vec<2, T, defaultp>;
        template <typename T>
        using vec3 = vec<3, T, defaultp>;
        template <typename T>
        using vec4 = vec<4, T, defaultp>;

        template <typename T>
        vec2<T> min(const vec2<T> &v, T _x, T _y) { return vec2<T>(std::min(v.x, _x), std::min(v.y, _y)); }
        template <typename T>
        vec2<T> max(const vec2<T> &v, T _x, T _y) { return vec2<T>(std::max(v.x, _x), std::max(v.y, _y)); }

        // template <typename T>
        // struct Vec2
        //{
        //     union
        //     {
        //         struct
        //         {
        //             T x, y;
        //         };
        //         struct
        //         {
        //             T width, height;
        //         };
        //     };
        //
        //    Vec2(T x, T y) : x(x), y(y) {}
        //    Vec2(T v) : x(v), y(v) {}
        //    Vec2() : x(), y() {}
        //
        //    Vec2<T> min(T _x, T _y) { return Vec2(std::min(x, _x), std::min(y, _y)); }
        //    Vec2<T> max(T _x, T _y) { return Vec2(std::max(x, _x), std::max(y, _y)); }
        //
        //    void operator+=(Vec2<T> other) { x += other.x, y += other.y; }
        //    Vec2<T> operator+(Vec2<T> other) { return {x + other.x, y + other.y}; }
        //
        //    void operator-=(Vec2<T> other) { x -= other.x, y -= other.y; }
        //    Vec2<T> operator-(Vec2<T> other) { return {x - other.x, y - other.y}; }
        //
        //    bool operator==(Vec2<T> other) { return x == other.x && y == other.y; }
        //};
        //
        // template <typename T>
        // struct Vec3
        //{
        //    union
        //    {
        //        struct
        //        {
        //            T x, y, z;
        //        };
        //        struct
        //        {
        //            T width, height, length;
        //        };
        //        struct
        //        {
        //            T r, g, b;
        //        };
        //    };
        //
        //    Vec3(T x, T y, T z) : x(x), y(y), z(z) {}
        //    Vec3(T v) : x(v), y(v), z(v) {}
        //    Vec3() : x(), y(), z() {}
        //
        //    Vec3<T> min(T _x, T _y, T _z) { return Vec3(std::min(x, _x), std::min(y, _y), std::min(z, _z)); }
        //    Vec3<T> max(T _x, T _y, T _z) { return Vec3(std::max(x, _x), std::max(y, _y), std::max(z, _z)); }
        //
        //    void operator+=(Vec3<T> other) { x += other.x, y += other.y, z += other.z; }
        //    Vec3<T> operator+(Vec3<T> other) { return {x + other.x, y + other.y, z + other.z}; }
        //
        //    void operator-=(Vec3<T> other) { x -= other.x, y -= other.y, z -= other.z; }
        //    Vec3<T> operator-(Vec3<T> other) { return {x - other.x, y - other.y, z - other.z}; }
        //
        //    bool operator==(Vec3<T> other) { return x == other.x && y == other.y && z == other.z; }
        //};

        template <typename T>
        struct Rect
        {
            vec2<T> start;
            vec2<T> size;

            Rect(vec2<T> start, vec2<T> size) : start(start), size(size) {}
            Rect() : start(), size() {}

            inline vec2<T> getEnd() { return start + size; }

            Rect<T> min(T _x, T _y) { return Rect(math::min<T>(start, _x, _y), math::min<T>(getEnd(), _x, _y) - start); }
            Rect<T> max(T _x, T _y) { return Rect(math::max<T>(start, _x, _y), math::max<T>(getEnd(), _x, _y) - start); }
            inline Rect<T> min(vec2<T> v) { return min(v.x, v.y); }
            inline Rect<T> max(vec2<T> v) { return max(v.x, v.y); }
        };

        template <typename T>
        std::ostream &operator<<(std::ostream &stream, Rect<T> &rect)
        {
            stream << "Rect { x=" << rect.start.x << " y=" << rect.start.y << " w=" << rect.size.x << " h=" << rect.size.y << " }";
            return stream;
        }

    } // namespace math

} // namespace rt

#endif // RTMATH_HPP