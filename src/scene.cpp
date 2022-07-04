#include <scene.h>

namespace rt
{
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
    namespace Shapes
    {
        Sphere::~Sphere()
        {
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
