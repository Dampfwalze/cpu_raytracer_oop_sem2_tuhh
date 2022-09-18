#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <optional>
#include <rtmath.h>

namespace rt
{
    namespace m = math;

    struct Transform
    {
        m::dvec3 position;
        m::dquat rotation;
        m::dvec3 scale;

        struct Cached
        {
            m::dmat4 matrix;
            m::dmat4 inverseMatrix;
            m::dmat4 inverseTransposeMatrix;
        } mutable cached;

    public:
        Transform(const m::dvec3 &position = m::dvec3(0), const m::dquat &rotation = m::dquat(1, 0, 0, 0), const m::dvec3 &scale = m::dvec3(1));

        void cacheMatrix() const;
    };

    std::ostream &operator<<(std::ostream &stream, const Transform &transform);
} // namespace rt

#endif // TRANSFORM_HPP