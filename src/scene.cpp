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
}