#include <scene/transform.h>
#include <iomanip>

namespace rt
{
    Transform::Transform()
        : rotation(1, 0, 0, 0) {}

    m::dmat4 const &Transform::getMatrix()
    {
        if (m_matrix.has_value())
            return m_matrix.value();

        m_matrix = glm::toMat4(rotation);
        m_matrix = m::translate(m_matrix.value(), position);

        return m_matrix.value();
    }

    std::ostream &operator<<(std::ostream &stream, const Transform &transform)
    {
        stream << "Transform { " << std::setw(10) << "position: " << transform.position << ", " << std::setw(10) << "rotation: " << transform.rotation << "}";
        return stream;
    }
} // namespace rt
