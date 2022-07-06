#include <scene/scene.h>

#include <stream_formatter.h>
#include <iomanip>

namespace rt
{

    Scene::Scene(std::vector<std::unique_ptr<SceneShape>> &objects)
        : m_objects(std::move(objects)) {}

    Scene::Scene(std::vector<std::unique_ptr<SceneShape>> &&objects)
        : m_objects(std::move(objects)) {}

    Scene::Scene()
        : m_objects() {}

    Scene::~Scene() {}

    void Scene::addShape(SceneShape *shape)
    {
        m_objects.emplace_back(shape);
    }

    std::ostream &operator<<(std::ostream &stream, const Scene &scene)
    {
        stream << "Scene { Shapes: ";
        for (auto &&s : scene.m_objects)
            if (s == scene.m_objects.back())
                stream << *s;
            else
                stream << *s << ", ";
        return stream << "}";
    }
}
