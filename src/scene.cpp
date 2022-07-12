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

    size_t Scene::addMaterial(Material *material)
    {
        materials[materials.size()] = std::unique_ptr<Material>(material);
        return materials.size() - 1;
    }

    Material *Scene::getMaterial(size_t index) const
    {
        auto f = materials.find(index);
        if (f == materials.end())
            return nullptr;
        return f->second.get();
    }

    void Scene::cacheFrameData(const m::u64vec2 &screenSize) const
    {
        for (auto &&object : objects)
            object->transform.cacheMatrix();
        camera.cacheMatrix(screenSize.x / (double)screenSize.y);
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
