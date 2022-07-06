#include <scene.h>

#include <stream_formatter.h>
#include <iomanip>

namespace rt
{
    std::ostream &operator<<(std::ostream &stream, const SceneShape &shape)
    {
        shape.toString(stream);
        return stream;
    }

    Scene::Scene(std::vector<std::unique_ptr<SceneShape>> &objects)
        : m_objects(std::move(objects)) {}

    Scene::Scene(std::vector<std::unique_ptr<SceneShape>> &&objects)
        : m_objects(std::move(objects)) {}

    Scene::Scene()
        : m_objects() {}

    Scene::~Scene()
    {
    }

    void Scene::addShape(SceneShape *shape)
    {
        m_objects.emplace_back(shape);
    }

    std::ostream &operator<<(std::ostream &stream, const Scene &shape)
    {
        auto ss = stream << "Scene:" << rtstd::pushIndent();
        ss << "\nShapes:";
        for (auto &&s : shape.m_objects)
            ss << rtstd::pushIndent() << "\n"
               << *s
               << rtstd::popIndent();
        ss << rtstd::popIndent();
        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, const SceneShape::Transform &transform)
    {
        stream << "Transform:\n    " << std::setw(10) << "position: " << transform.position << "\n    " << std::setw(10) << "rotation: " << transform.rotation;
        return stream;
    }

    namespace Shapes
    {
        Sphere::~Sphere()
        {
        }

        std::ostream &Sphere::toString(std::ostream &stream) const
        {
            stream << rtstd::pushIndent() << "Sphere:\n"
                   << transform << "\nr: " << r << rtstd::popIndent();
            return stream;
        }
    }

    SceneShape::Transform::Transform()
        : rotation(1, 0, 0, 0) {}

    m::dmat4 const &SceneShape::Transform::getMatrix()
    {
        if (m_matrix.has_value())
            return m_matrix.value();

        m_matrix = glm::toMat4(rotation);
        m_matrix = m::translate(m_matrix.value(), position);

        return m_matrix.value();
    }
}
