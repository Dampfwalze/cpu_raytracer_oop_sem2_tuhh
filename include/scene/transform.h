#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <rtmath.h>
#include <optional>

namespace rt
{
    namespace m = math;

    struct Transform
    {
        m::dvec3 position;
        m::dquat rotation;

    private:
        std::optional<m::dmat4> m_matrix;

    public:
        Transform();

        m::dmat4 const &getMatrix();
    };

    std::ostream &operator<<(std::ostream &stream, const Transform &transform);
} // namespace rt

#endif // TRANSFORM_HPP