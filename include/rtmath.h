#ifndef RTMATH_HPP
#define RTMATH_HPP

#include <algorithm>
#include <iomanip>
#include <iostream>

#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace rt
{
    namespace math
    {

        using namespace glm;

        template <typename T = double>
        using vec1 = vec<1, T, defaultp>;
        template <typename T>
        using vec2 = vec<2, T, defaultp>;
        template <typename T>
        using vec3 = vec<3, T, defaultp>;
        template <typename T>
        using vec4 = vec<4, T, defaultp>;

        template <typename T = double>
        using mat2 = mat<2, 2, T, defaultp>;
        template <typename T = double>
        using mat2x2 = mat2<T>;
        template <typename T = double>
        using mat3 = mat<3, 3, T, defaultp>;
        template <typename T = double>
        using mat3x3 = mat3<T>;
        template <typename T = double>
        using mat4 = mat<4, 4, T, defaultp>;
        template <typename T = double>
        using mat4x4 = mat4<T>;

        template <typename T>
        using Color = vec3<T>;
        template <typename T>
        using Pixel = Color<T>;

        template <typename T>
        vec<2, T, defaultp> min(const vec<2, T, defaultp> &v, T _x, T _y) { return vec<2, T, defaultp>(std::min(v.x, _x), std::min(v.y, _y)); }
        template <typename T>
        vec<2, T, defaultp> max(const vec<2, T, defaultp> &v, T _x, T _y) { return vec<2, T, defaultp>(std::max(v.x, _x), std::max(v.y, _y)); }

        template <typename T>
        struct ray
        {
            vec3<T> origin;
            vec3<T> direction;

            ray(vec3<T> origin = vec3<T>(), vec3<T> direction = vec3<T>()) : origin(origin), direction(direction) {}

            ray<T> transformPerspective(const mat4<T> &matrix) const
            {
                auto   n_origin = matrix * vec4<T>(origin, 1);
                auto   n_direction = matrix * vec4<T>(direction + origin, 1);
                ray<T> n;
                n.origin = n_origin.xyz() / n_origin.w;
                n.direction = n_direction.xyz() / n_direction.w - n.origin;
                return n;
            }

            // Only works with matricies, that are not effecting the w component of a vector.
            // If you matrix does not fit this restriction, use ray<T>.transformPerspective(mat4<T>) instead.
            ray<T> &operator*=(const mat4<T> &matrix)
            {
                assert(matrix[0].w == 0 && matrix[1].w == 0 && matrix[2].w == 0 && matrix[3].w == 1 && "Given Matrix effects w component, use ray<T>.transformPerspective(mat4<T>) instead!");
                origin = matrix * vec4<T>(origin, 1);
                direction = matrix * vec4<T>(direction, 0);
                return *this;
            }

            inline ray<T> &normalize()
            {
                direction = glm::normalize(direction);
                return *this;
            }

            inline vec3<T> operator()(T scalar) const
            {
                return origin + direction * scalar;
            }
        };

        // Only works with matricies, that are not effecting the w component of a vector.
        // If you matrix does not fit this restriction, use ray<T>.transformPerspective(mat4<T>) instead.
        template <typename T>
        ray<T> operator*(const mat4<T> &matrix, const ray<T> &ray)
        {
            assert(abs(matrix[0].w) < 0.001 && abs(matrix[1].w) < 0.001 && abs(matrix[2].w) < 0.001 && abs(matrix[3].w - 1) < 0.001 && "Given Matrix effects w component, use ray<T>.transformPerspective(mat4<T>) instead!");
            math::ray<T> n;
            n.origin = matrix * vec4<T>(ray.origin, 1);
            n.direction = matrix * vec4<T>(ray.direction, 0);
            return n;
        }

        template <typename T>
        ray<T> normalize(ray<T> r) { return r.normalize(); }

        template <typename T>
        std::ostream &operator<<(std::ostream &stream, ray<T> &ray)
        {
            stream << "Ray { origin: " << ray.origin << ", direction: " << ray.direction << " }";
            return stream;
        }

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

            inline vec2<T> getEnd() const { return start + size; }

            Rect<T>        min(T _x, T _y) const { return Rect(math::min<T>(start, _x, _y), math::min<T>(getEnd(), _x, _y) - start); }
            Rect<T>        max(T _x, T _y) const { return Rect(math::max<T>(start, _x, _y), math::max<T>(getEnd(), _x, _y) - start); }
            inline Rect<T> min(vec2<T> v) const { return min(v.x, v.y); }
            inline Rect<T> max(vec2<T> v) const { return max(v.x, v.y); }
        };

        template <typename T>
        std::ostream &operator<<(std::ostream &stream, Rect<T> &rect)
        {
            stream << "Rect { x=" << rect.start.x << " y=" << rect.start.y << " w=" << rect.size.x << " h=" << rect.size.y << " }";
            return stream;
        }

#pragma warning(push)
#pragma warning(disable : 4244)
        template <typename T, typename D>
        T Vec1(const D &vec)
        {
            return T(vec.x);
        }
        template <typename T, typename D>
        T Vec2(const D &vec) { return T(vec.x, vec.y); }
        template <typename T, typename D>
        T Vec3(const D &vec) { return T(vec.x, vec.y, vec.z); }
        template <typename T, typename D>
        T Vec4(const D &vec) { return T(vec.x, vec.y, vec.z, vec.w); }
#pragma warning(pop)

        template <typename T>
        inline T map(T value, T istart, T istop, T ostart, T ostop)
        {
            return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
        }
    } // namespace math

} // namespace rt

namespace glm
{
    template <typename T, qualifier Q>
    std::ostream &operator<<(std::ostream &stream, const vec<1, T, Q> &v)
    {
        stream << "vec1(" << v.x << ")";
        return stream;
    }

    template <typename T, qualifier Q>
    std::ostream &operator<<(std::ostream &stream, const vec<2, T, Q> &v)
    {
        stream << "vec2(" << v.x << ", " << v.y << ")";
        return stream;
    }

    template <typename T, qualifier Q>
    std::ostream &operator<<(std::ostream &stream, const vec<3, T, Q> &v)
    {
        stream << "vec3(" << v.x << ", " << v.y << ", " << v.z << ")";
        return stream;
    }

    template <typename T, qualifier Q>
    std::ostream &operator<<(std::ostream &stream, const vec<4, T, Q> &v)
    {
        stream << "vec4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
        return stream;
    }

    template <typename T, qualifier Q>
    std::ostream &operator<<(std::ostream &stream, const qua<T, Q> &q)
    {
        stream << "quat(" << q.w << " + " << q.x << "i - " << q.y << "j + " << q.z << "k)";
        return stream;
    }

    template <int x, int y, typename T, qualifier Q>
    std::ostream &operator<<(std::ostream &stream, const mat<x, y, T, Q> &m)
    {
        stream << "mat(" << std::setprecision(3);
        for (int i = 0; i < y; i++)
        {
            for (int j = 0; j < x; j++)
            {
                stream << std::setw(6) << m[j][i] << " ";
            }
            stream << ((i == y - 1) ? ")" : "\n    ");
        }
        return stream << std::setprecision(4);
    }
} // namespace glm

#endif // RTMATH_HPP