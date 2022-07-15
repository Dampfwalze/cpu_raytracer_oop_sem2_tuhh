#include <scene/transform.h>
#include <iomanip>

namespace rt
{
    Transform::Transform(const m::dvec3 &position, const m::dquat &rotation, const m::dvec3 &scale)
        : position(position), rotation(rotation), scale(scale) {}

    void Transform::cacheMatrix() const
    {
        cached.matrix = m::scale(m::translate(m::dmat4(1), position) * glm::toMat4(rotation), scale);
        cached.inverseMatrix = m::inverse(cached.matrix);
        cached.inverseTransposeMatrix = m::transpose(cached.inverseMatrix);
    }

    std::ostream &operator<<(std::ostream &stream, const Transform &transform)
    {
        return stream << "Transform { position: " << transform.position
                      << ", rotation: " << transform.rotation
                      << ", scale: " << transform.scale << " }";
    }
} // namespace rt
