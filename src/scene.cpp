#include <scene/scene.h>

#include <stream_formatter.h>
#include <iomanip>

namespace rt
{

    Scene::Scene(std::vector<std::unique_ptr<SceneShape>> &objects, const Camera &camera)
        : m_objects(std::move(objects)), m_camera(camera) {}

    Scene::Scene(std::vector<std::unique_ptr<SceneShape>> &&objects, const Camera &camera)
        : m_objects(std::move(objects)), m_camera(camera) {}

    Scene::Scene(const Camera &camera)
        : m_camera(camera) {}

    Scene::~Scene() {}

    void Scene::addShape(SceneShape *shape)
    {
        m_objects.emplace_back(shape);
    }

    std::ostream &operator<<(std::ostream &stream, const Scene &scene)
    {
        stream << "Scene { Shapes: ";
        for (auto &&s : scene.m_objects)
            stream << *s << ", ";
        stream << "Camera: " << scene.m_camera;
        return stream << "}";
    }
}
