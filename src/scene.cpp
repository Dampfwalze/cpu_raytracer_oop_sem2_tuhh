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

    void Scene::addLight(SceneLight *light)
    {
        lights.emplace_back(light);
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

    // Ray is in world space
    std::optional<Intersection> Scene::castRay(const m::ray<double> &ray, std::optional<double> maxLength2) const
    {
        Intersection nearestInter;
        double nearestDist2 = std::numeric_limits<double>::max();

        for (auto &&i : objects)
        {
            m::dmat4 mat = i->transform.cached.matrix;
            m::dmat4 invMat = i->transform.cached.inverseMatrix;

            // Ray is in local object space now
            m::ray localRay = invMat * ray;

            auto maybeIntersection = i->intersect(localRay);

            if (!maybeIntersection)
                continue;

            auto &intersection = maybeIntersection.value();

            // Transform intersection details back to world space
            intersection.position = mat * m::dvec4(intersection.position, 1.0);
            intersection.normal = mat * m::dvec4(intersection.normal, 0.0);

            double dist2 = m::distance2(ray.origin, intersection.position);
            if (dist2 < nearestDist2 && (!maxLength2 || dist2 < *maxLength2))
            {
                nearestDist2 = dist2;
                nearestInter = intersection;
            }
        }

        if (nearestDist2 != std::numeric_limits<double>::max())
            return nearestInter;
        return std::nullopt;
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
