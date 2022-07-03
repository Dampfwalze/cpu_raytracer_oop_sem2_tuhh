#include <scene.h>

namespace rt
{
    Scene::Scene(std::vector<std::unique_ptr<SceneObject>> &objects)
        : m_objects(std::move(objects)) {}

    Scene::Scene(std::vector<std::unique_ptr<SceneObject>> &&objects)
        : m_objects(std::move(objects)) {}

    Scene::Scene()
        : m_objects() {}

    Scene::~Scene()
    {
    }
}