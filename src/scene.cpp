#include <scene/scene.h>

#include <stream_formatter.h>
#include <iomanip>

namespace rt
{

    Scene::Scene(std::vector<std::unique_ptr<SceneShape>> &objects, const Camera &camera)
        : objects(std::move(objects)), camera(camera) {}

    Scene::Scene(std::vector<std::unique_ptr<SceneShape>> &&objects, const Camera &camera)
        : objects(std::move(objects)), camera(camera) {}

    Scene::Scene(const Camera &camera)
        : camera(camera) {}

    Scene::~Scene() {}

    void Scene::addShape(SceneShape *shape)
    {
        objects.emplace_back(shape);
    }

    std::ostream &operator<<(std::ostream &stream, const Scene &scene)
    {
        stream << "Scene { Shapes: ";
        for (auto &&s : scene.objects)
            stream << *s << ", ";
        stream << "Camera: " << scene.camera;
        return stream << "}";
    }
}
