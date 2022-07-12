#include <scene/transform.h>
#include <iomanip>

namespace rt
{
    Transform::Transform(const m::dvec3 &position, const m::dquat &rotation)
        : position(position), rotation(rotation) {}

    void Transform::cacheMatrix() const
    {
        cached.matrix = m::translate(glm::toMat4(rotation), position);
        cached.inverseMatrix = m::inverse(cached.matrix);
    }

    std::ostream &operator<<(std::ostream &stream, const Transform &transform)
    {
        stream << "Transform { " << std::setw(10) << "position: " << transform.position << ", " << std::setw(10) << "rotation: " << transform.rotation << "}";
        return stream;
    }
} // namespace rt
